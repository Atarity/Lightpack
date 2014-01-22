/*
 * WinUtils.cpp
 *
 *  Created on: 25.07.11
 *     Project: Lightpack
 *
 *  Copyright (c) 2011 Timur Sattarov, Mike Shatohin
 *
 *  Lightpack a USB content-driving ambient lighting system
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "WinUtils.hpp"

#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>

#include "../src/debug.h"

#define SIZEOF_ARRAY(a) (sizeof(a)/sizeof(a[0]))

namespace WinUtils
{

const WCHAR lightpackHooksDllName[] = L"prismatik-hooks.dll";
static LPCWSTR pwstrExcludeProcesses[]={L"skype.exe", L"chrome.exe", L"firefox.exe"};

BOOL SetPrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable) {

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof (tp), NULL, NULL);
    return ((GetLastError() == ERROR_SUCCESS));

}

BOOL AcquirePrivileges() {

    HANDLE hCurrentProc = GetCurrentProcess();
    HANDLE hCurrentProcToken;

    if (!OpenProcessToken(hCurrentProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hCurrentProcToken)) {
//        syslog(LOG_ERR, "OpenProcessToken Error %u", GetLastError());
    } else {
        if (!SetPrivilege(hCurrentProcToken, SE_DEBUG_NAME, TRUE)) {
//            syslog(LOG_ERR, "SetPrivleges SE_DEBUG_NAME Error %u", GetLastError());
        } else {
            return TRUE;
        }
    }
    return FALSE;
}

QList<DWORD> * getDxProcessesIDs(QList<DWORD> * processes, LPCWSTR wstrSystemRootPath) {

    DWORD aProcesses[1024];
    HMODULE hMods[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    char debug_buf[255];
    WCHAR executableName[MAX_PATH];
    unsigned int i;

    //     Get the list of process identifiers.
    processes->clear();

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return NULL;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the names of the modules for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if (aProcesses[i] != GetCurrentProcessId()) {
            HANDLE hProcess;
            hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, aProcesses[i] );
            if (NULL == hProcess)
                goto nextProcess;

            GetModuleFileNameExW(hProcess, 0, executableName, sizeof (executableName));

            if (wcsstr(executableName, wstrSystemRootPath) != NULL) {
                goto nextProcess;
            }

            PathStripPathW(executableName);

            ::WideCharToMultiByte(CP_ACP, 0, executableName, -1, debug_buf, 255, NULL, NULL);
            DEBUG_MID_LEVEL << Q_FUNC_INFO << debug_buf;

            for (unsigned k=0; k < SIZEOF_ARRAY(pwstrExcludeProcesses); k++) {
                if (wcsicmp(executableName, pwstrExcludeProcesses[k])== 0) {
                    DEBUG_MID_LEVEL << Q_FUNC_INFO << "skipping " << pwstrExcludeProcesses;
                    goto nextProcess;
                }
            }

            // Get a list of all the modules in this process.

            if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
            {
                bool isDXPresent = false;
                for ( DWORD j = 0; j < (cbNeeded / sizeof(HMODULE)); j++ )
                {
                    WCHAR szModName[MAX_PATH];

                    if ( GetModuleFileNameExW( hProcess, hMods[j], szModName,
                                              sizeof(szModName) / sizeof(WCHAR)))
                    {

                        PathStripPathW(szModName);
                        ::WideCharToMultiByte(CP_ACP, 0, szModName, -1, debug_buf, 255, NULL, NULL);
                        DEBUG_HIGH_LEVEL << Q_FUNC_INFO << debug_buf;

                        if(wcsicmp(szModName, lightpackHooksDllName) == 0) {
                            goto nextProcess;
                        } else {
                            if (wcsicmp(szModName, L"d3d9.dll") == 0 ||
                                wcsicmp(szModName, L"dxgi.dll") == 0 )
                                isDXPresent = true;
                        }
                    }
                }
                if (isDXPresent)
                    processes->append(aProcesses[i]);

            }
nextProcess:
            // Release the handle to the process.
            CloseHandle( hProcess );
        }
    }

    return processes;
}


PVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD) {
    DWORD  dwAclLength;
    PSID   pAuthenticatedUsersSID = NULL;
    PACL   pDACL   = NULL;
    BOOL   bResult = FALSE;
    SID_IDENTIFIER_AUTHORITY siaNT = SECURITY_NT_AUTHORITY;

    // initialize the security descriptor
    if (!InitializeSecurityDescriptor(pSD,
                                      SECURITY_DESCRIPTOR_REVISION)) {
//        syslog(LOG_ERR, "InitializeSecurityDescriptor() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // obtain a sid for the Authenticated Users Group
    if (!AllocateAndInitializeSid(&siaNT, 1,
                                  SECURITY_AUTHENTICATED_USER_RID, 0, 0, 0, 0, 0, 0, 0,
                                  &pAuthenticatedUsersSID)) {
//        syslog(LOG_ERR, "AllocateAndInitializeSid() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // NOTE:
    //
    // The Authenticated Users group includes all user accounts that
    // have been successfully authenticated by the system. If access
    // must be restricted to a specific user or group other than
    // Authenticated Users, the SID can be constructed using the
    // LookupAccountSid() API based on a user or group name.

    // calculate the DACL length
    dwAclLength = sizeof(ACL)
            // add space for Authenticated Users group ACE
            + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
            + GetLengthSid(pAuthenticatedUsersSID);

    // allocate memory for the DACL
    pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                             dwAclLength);
    if (!pDACL) {
//        syslog(LOG_ERR, "HeapAlloc() failed with error %d\n", GetLastError());
        goto end;
    }

    // initialize the DACL
    if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) {
//        syslog(LOG_ERR, "InitializeAcl() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // add the Authenticated Users group ACE to the DACL with
    // GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access

    if (!AddAccessAllowedAce(pDACL, ACL_REVISION,
                             MAXIMUM_ALLOWED ,
                             pAuthenticatedUsersSID)) {
//        syslog(LOG_ERR, "AddAccessAllowedAce() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    // set the DACL in the security descriptor
    if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE)) {
//        syslog(LOG_ERR, "SetSecurityDescriptorDacl() failed with error %d\n",
//               GetLastError());
        goto end;
    }

    bResult = TRUE;

end:
    if (pAuthenticatedUsersSID) FreeSid(pAuthenticatedUsersSID);

    if (bResult == FALSE) {
        if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
        pDACL = NULL;
    }

    return (PVOID) pDACL;
}

VOID FreeRestrictedSD(PVOID ptr) {

    if (ptr) HeapFree(GetProcessHeap(), 0, ptr);

    return;
}

} // namespace WinUtils
