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
    QVERIFY2( LightpackMath::withChromaHSV(qRgb(200,100,0), 100) == qRgb(200, 150, 100), "getChromaHSV() is incorrect");
}
