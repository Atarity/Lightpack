#include "GrabCalculationTest.hpp"

void GrabCalculationTest::testCase1()
{
    QRgb result;
    unsigned char buf[16];
    memset(buf, 0xfa, 16);
    QVERIFY2(Grab::Calculations::calculateAvgColor(&result, buf, Grab::BufferFormatArgb, 16, QRect(0,0,4,1)) == 0, "Failure. calculateAvgColor returned wrong errorcode");
    QCOMPARE(result, qRgb(0xfa,0xfa,0xfa));
    QVERIFY2(Grab::Calculations::calculateAvgColor(&result, buf, Grab::BufferFormatAbgr, 16, QRect(0,0,4,1)) == 0, "Failure. calculateAvgColor returned wrong errorcode");
    QCOMPARE(result, qRgb(0xfa,0xfa,0xfa));
}
