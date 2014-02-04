#include "GrabCalculationTest.hpp"

void GrabCalculationTest::testCase1()
{
    QRgb result;
    unsigned char buf[16];
    memset(buf, 0xfa, 16);
    QVERIFY2(Grab::Calculations::calculateAvgColor(&result, buf, BufferFormatArgb, 16, QRect(0,0,4,1)) == 0xfa, "Failure. calculateAvgColor returned wrong errorcode");
    QCOMPARE(result, qRgb(0xfa,0xfa,0xfa));
}
