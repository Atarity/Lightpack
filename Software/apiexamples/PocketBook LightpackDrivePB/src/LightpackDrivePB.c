/*
 * LightpackDrivePB.c
 *
 *  Created on: 02.09.2011
 *      Author: Mike Shatohin (brunql)
 *     Project: LightpackDrivePB 
 *
 *  Drive Lightpack from PocketBook 902 over WiFi
 *
 *  Copyright (c) 2011 Mike Shatohin, mikeshatohin [at] gmail.com
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


#include <sys/socket.h>
#include <netinet/in.h>
#include "inkview.h"

#define BUFF_SIZE    64

char m_cmdSetStatus[BUFF_SIZE] = "lock\nsetstatus:%s\nunlock\n";
char m_cmdBuffer[BUFF_SIZE];
char m_statusTxt[BUFF_SIZE];

ifont *m_droidSans64, *m_droidMono32, *m_droidMono16;

enum STATUS {
    ON = -1,
    OFF = 0
} m_status = OFF;

void updateStatus(char *s)
{
    FillArea(100, 100, ScreenWidth() - 100, ScreenHeight() / 3, WHITE);
    SetFont(m_droidSans64, BLACK);
    DrawString(100, 100, s);
    PartialUpdate(100, 100, ScreenWidth() - 100, 100);
}

void initMainScreen()
{
  ClearScreen();

  SetFont(m_droidMono32, BLACK);
  DrawString(100, ScreenHeight() / 2 - 100, "LightpackDrivePB");
  DrawString(100, ScreenHeight() / 2,       "PocketBook 902");
  DrawString(100, ScreenHeight() / 2 + 150, "brunql");
  SetFont(m_droidMono16, BLACK);
  DrawString(100, ScreenHeight() / 2 + 230, "lightpack.googlecode.com");

  FullUpdate();
}

void onOkClick(void)
{
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        updateStatus("Error:socket()");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3636);
    addr.sin_addr.s_addr = htonl(((in_addr_t) 0xC0A80102)); // 192.168.1.2
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        updateStatus("Error:connect()");
        return;
    }

    if (m_status == OFF){
        m_status = ON;
        sprintf(m_cmdBuffer, m_cmdSetStatus, "on");
        sprintf(m_statusTxt, "Status: %s", "ON");
    } else {
        m_status = OFF;
        sprintf(m_cmdBuffer, m_cmdSetStatus, "off");
        sprintf(m_statusTxt, "Status: %s", "OFF");
    }

    int result = send(sock, m_cmdBuffer, sizeof(m_cmdSetStatus), 0);
    if (result < 0){
        updateStatus("Error:send()");
        return;
    }
    close(sock);

    updateStatus(m_statusTxt);
}

int mainEventHandler(int type, int par1, int par2)
{
     if (type == EVT_INIT){
        m_droidSans64 = OpenFont("DroidSans", 80, 1);
        m_droidMono32 = OpenFont("DroidSansMono", 55, 2);
        m_droidMono16 = OpenFont("DroidSansMono", 38, 2);
    }

    if (type == EVT_SHOW){
        initMainScreen();
    }

    if (type == EVT_KEYPRESS){
        switch (par1){

        case KEY_OK:
            onOkClick();
            break;

        case KEY_BACK:
            CloseApp();
            break;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    InkViewMain(mainEventHandler);
    return 0;
}
