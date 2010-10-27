#include "grabdesktopwindowleds.h"

GrabDesktopWindowLeds::GrabDesktopWindowLeds(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timeEval = new TimeEvaluations();

    desktop_width = QApplication::desktop()->width();
    desktop_height = QApplication::desktop()->height();    

    // Read settings once
    this->ambilight_refresh_delay_ms = settings->value("RefreshAmbilightDelayMs").toInt();

    this->ambilight_width = settings->value("WidthAmbilight").toInt();
    this->ambilight_height = settings->value("HeightAmbilight").toInt();

    this->ambilight_white_balance_r = settings->value("WhiteBalanceCoefRed").toDouble();
    this->ambilight_white_balance_g = settings->value("WhiteBalanceCoefGreen").toDouble();
    this->ambilight_white_balance_b = settings->value("WhiteBalanceCoefBlue").toDouble();

    this->ambilight_color_depth = settings->value("HwColorDepth").toInt();


    createLabelsGrabPixelsRects();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateLedsColorsIfChanged()));

    timer->start(ambilight_refresh_delay_ms);
}

GrabDesktopWindowLeds::~GrabDesktopWindowLeds()
{
    delete timer;
    delete timeEval;

    for(int i=0; i<labelGrabPixelsRects.count(); i++){
        labelGrabPixelsRects[i]->close();
    }

    labelGrabPixelsRects.clear();

    delete labelGrabPixelsRects;
}


void GrabDesktopWindowLeds::createLabelsGrabPixelsRects()
{
    labelGrabPixelsRects.clear();

    QColor labelsColors[LEDS_COUNT] = { Qt::blue, Qt::yellow, Qt::red, Qt::green };

    for(int i=0; i<LEDS_COUNT; i++){
        labelGrabPixelsRects.append(new QLabel(this, Qt::FramelessWindowHint | Qt::SplashScreen));
        labelGrabPixelsRects[i]->setFocusPolicy(Qt::NoFocus);

        // Fill label with labelColors[i] color
        QPalette pal = labelGrabPixelsRects[i]->palette();
        pal.setBrush(labelGrabPixelsRects[i]->backgroundRole(), QBrush(labelsColors[i]));
        labelGrabPixelsRects[i]->setPalette(pal);
    }

    delete labelsColors;

    updateSizesLabelsGrabPixelsRects();
}

void GrabDesktopWindowLeds::updateSizesLabelsGrabPixelsRects()
{
    for(int i=0; i<LEDS_COUNT; i++){
        labelGrabPixelsRects[i]->setFixedWidth(ambilight_width);
        labelGrabPixelsRects[i]->setFixedHeight(ambilight_height);
    }

    labelGrabPixelsRects[RIGHT_UP]->move(desktop_width - ambilight_width, desktop_height / 2 - ambilight_height);
    labelGrabPixelsRects[RIGHT_DOWN]->move(desktop_width - ambilight_width, desktop_height / 2);
    labelGrabPixelsRects[LEFT_UP]->move(0, QApplication::desktop()->height() / 2 - ambilight_height);
    labelGrabPixelsRects[LEFT_DOWN]->move(0, QApplication::desktop()->height() / 2);
}


void GrabDesktopWindowLeds::setVisibleGrabPixelsRects(bool state)
{
    for(int i=0; i<labelGrabPixelsRects.count(); i++){
        if(state){
            labelGrabPixelsRects[i]->show();
        }else{
            labelGrabPixelsRects[i]->hide();
        }
    }
}




void GrabDesktopWindowLeds::updateLedsColorsIfChanged()
{    
    timeEval->howLongItStart();

    bool needToUpdate = false;

    int x = 0, y = 0;
    int colorsNew[LEDS_COUNT][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };

    for(int ledIndex=0; ledIndex<LEDS_COUNT; ledIndex++){
        switch(ledIndex){
        case LEFT_UP:
            x = 0;
            y = (desktop_height/2) - ambilight_height;
            break;
        case LEFT_DOWN:
            x = 0;
            y = (desktop_height/2);
            break;
        case RIGHT_UP:
            x = (desktop_width-1) - ambilight_width;
            y = (desktop_height/2) - ambilight_height;
            break;
        case RIGHT_DOWN:
            x = (desktop_width-1) - ambilight_width;
            y = (desktop_height/2);
            break;
        default:
            qWarning("void GrabDesktopWindowLeds::updateLedsColorsIfChanged(): This is impossible, but led_index=%d", ledIndex);
            break;
        }

        QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(), x, y,
                                          ambilight_width, ambilight_height);
        QPixmap scaledPix = pix.scaled(1,1, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QImage im = scaledPix.toImage();

        colors[ledIndex][R] = (im.pixel(0,0) >> 0x10) & 0xff;
        colors[ledIndex][G] = (im.pixel(0,0) >> 0x08) & 0xff;
        colors[ledIndex][B] = (im.pixel(0,0) >> 0x00) & 0xff;
    }


    // Find average for each led color
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        for(int color=0; color < 3; color++){
            // Each led color must be in 0..pwm_value_max
            colors[ledIndex][color] = (int)((double)colors[ledIndex][color] / (256.0 / ambilight_color_depth)); // now pwm_value_max==64

            //  9.6 mA - all off
            // 90.0 mA - all on
            //colors[led_index][color] = ambilight_color_depth;
        }
    }

    // White balance
    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        colors[ledIndex][R] *= white_balance_r;
        colors[ledIndex][G] *= white_balance_g;
        colors[ledIndex][B] *= white_balance_b;
    }

    for(int ledIndex=0; ledIndex < LEDS_COUNT; ledIndex++){
        for(int color=0; color < 3; color++){
            if(colors[ledIndex][color] != colorsNew[ledIndex][color]){                
                colors[ledIndex][color] = colorsNew[ledIndex][color];
                needToUpdate = true;
            }
        }
    }

    if(needToUpdate){
        emit updateLedsColors(colors);        
    }
    emit ambilightTimeOfUpdatingColors(timeEval->howLongItEnd());
}



void GrabDesktopWindowLeds::setAmbilightWidth(int w)
{
    this->ambilight_width = w;
}

void GrabDesktopWindowLeds::setAmbilightHeight(int h)
{
    this->ambilight_height = h;
}

void GrabDesktopWindowLeds::setAmbilightRefreshDelayMs(int ms)
{
    this->ambilight_refresh_delay_ms = ms;
}

void GrabDesktopWindowLeds::setAmbilightColorDepth(int color_depth)
{
    this->ambilight_color_depth = color_depth;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceR(double r)
{
    this->ambilight_white_balance_r = r;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceG(double g)
{
    this->ambilight_white_balance_g = g;
}

void GrabDesktopWindowLeds::setAmbilightWhiteBalanceB(double b)
{
    this->ambilight_white_balance_b = b;
}
