#ifndef LIGHTPACKMATHTEST_HPP
#define LIGHTPACKMATHTEST_HPP

#include <QObject>

class LightpackMathTest : public QObject
{
    Q_OBJECT
public:
    explicit LightpackMathTest(QObject *parent = 0);
    
private slots:
    void testCase1();
};

#endif // LIGHTPACKMATHTEST_HPP
