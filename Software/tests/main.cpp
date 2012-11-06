
#include <QtTest/QtTest>
#include "LightpackApiTest.hpp"
#include "GrabCalculationTest.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION

    GrabCalculationTest grabCalculationTest;
    QTest::qExec(&grabCalculationTest, argc, argv);

    LightpackApiTest lightpackApiTest;
    QTest::qExec(&lightpackApiTest, argc, argv);


    return 1;
}
