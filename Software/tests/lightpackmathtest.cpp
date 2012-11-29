#include "lightpackmathtest.hpp"
#include "LightpackMath.hpp"
#include <QtTest/QtTest>

LightpackMathTest::LightpackMathTest(QObject *parent) :
    QObject(parent)
{
}
void LightpackMathTest::testCase1()
{
    QVERIFY2( LightpackMath::getValueHSV(qRgb(215,122,0)) == 215, "getValueHSV() is incorrect");
    QRgb testRgb = qRgb(200,100,0);
    QVERIFY2( LightpackMath::withChromaHSV(testRgb, 100) == qRgb(200, 150, 100), "getChromaHSV() is incorrect");
    QVERIFY2( LightpackMath::withChromaHSV(testRgb, 250) == qRgb(200, 75, 0), "getChromaHSV() is incorrect");

    QVERIFY2( LightpackMath::withChromaHSV(testRgb, LightpackMath::getChromaHSV(testRgb)) == testRgb, "getChromaHSV() is incorrect");
}
