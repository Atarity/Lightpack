
#include <QtGui>
#include "desktop.h"

int Desktop::WidthAvailable = 0;
int Desktop::HeightAvailable = 0;
int Desktop::WidthFull = 0;
int Desktop::HeightFull = 0;

Desktop::Desktop()
{
    this->WidthAvailable = QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen()).width();
    this->HeightAvailable = QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen()).height();

    this->WidthFull = QApplication::desktop()->width();
    this->HeightFull = QApplication::desktop()->height();
}
