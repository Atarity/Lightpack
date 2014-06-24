#include "ProxyFuncJmp.hpp"
#include "ProxyFuncVFTable.hpp"
#include "HooksTest.h"
#include "hooksutils.h"
#include <QtTest/QtTest>

HooksTest::HooksTest(QObject *parent) :
    QObject(parent)
{
}

void HooksTest::jmpHookTestCase() {
//  Mockup of target memory                    v-- our target starts here, first byte is to be sure we haven't broken it
    unsigned char procMockup[8]         ={0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22};

//  Original memory mockup to compare with
    unsigned char procUntouchedMockup[8]={0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22};

//  Hooked memory mockup                      |nop |jmp |  relative address |
    unsigned char procHookedMockup[8]   ={0x22,0x90,0xe9,0x00,0x00,0x00,0x00,0x22};

    unsigned int substAdr=0x12345678;

    int relativeAdr=substAdr - (unsigned int)(procMockup+1)-6; /*+1 because we assume second byte is our target
                                                               -6 because length of injected code is 6*/
    memcpy(procHookedMockup+3, &relativeAdr, sizeof(relativeAdr));

    ProxyFuncJmp *test = new ProxyFuncJmp(procMockup+1, (void *)substAdr, Logger::getInstance());
    QVERIFY2(!test->isHookInstalled(),"Hook shouldn't be installed");
    QVERIFY2(test->init(), "init crashed");
    QVERIFY2(test->installHook(), "installHook crashed 1");
    QVERIFY2(test->isHookInstalled(),"Hook should be installed");

    QVERIFY2(memcmp(procMockup, procHookedMockup, 8)==0,"Hook has been installed incorrectly");

    QVERIFY2(test->removeHook(), "removeHook crashed");

    QVERIFY2(memcmp(procMockup, procUntouchedMockup, 8)==0,"Hook has been removed incorrectly 1");

    QVERIFY2(test->installHook(), "installHook crashed 2");

    delete test;

    QVERIFY2(memcmp(procMockup, procUntouchedMockup, 8)==0,"Hook should be removed after object deletion");
}

void HooksTest::vfTableHookTestCase() {
//  Mockup of target memory                    v-- our target starts here, first byte is to be sure we haven't broken it
    unsigned char procMockup[8]         ={0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22};

//  Original memory mockup to compare with
    unsigned char procUntouchedMockup[8]={0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22};

//  Hooked memory mockup                      | absolute address  |
    unsigned char procHookedMockup[8]   ={0x22,0x00,0x00,0x00,0x00,0x22,0x22,0x22};

    unsigned int substAdr=0x12345678;

    memcpy(procHookedMockup+1, &substAdr, sizeof(substAdr));

    ProxyFuncVFTable *test = new ProxyFuncVFTable(procMockup+1, (void *)substAdr, Logger::getInstance());
    QVERIFY2(!test->isHookInstalled(),"Hook shouldn't be installed");
    QVERIFY2(test->init(), "init crashed");
    QVERIFY2(test->installHook(), "installHook crashed 1");
    QVERIFY2(test->isHookInstalled(),"Hook should be installed");

    QVERIFY2(memcmp(procMockup, procHookedMockup, 8)==0,"Hook has been installed incorrectly");

    QVERIFY2(test->removeHook(), "removeHook crashed");

    QVERIFY2(memcmp(procMockup, procUntouchedMockup, 8)==0,"Hook has been removed incorrectly 1");

    QVERIFY2(test->installHook(), "installHook crashed 2");

    delete test;

    QVERIFY2(memcmp(procMockup, procUntouchedMockup, 8)==0,"Hook should be removed after object deletion");
}

void HooksTest::reportLogTestCase() {

//    int a = -1;
//    InitLog(L"test", 0);
//    ReportLogInfo(L"0x%x",a);
//    CloseLog();

}
