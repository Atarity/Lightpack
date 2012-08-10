#ifndef SELECTWIDGET_HPP
#define SELECTWIDGET_HPP

#include <QWidget>
#include <QtGui>

        class SelectWidget: public QWidget {
            Q_OBJECT
        public:
            SelectWidget();
            SelectWidget(QWidget *parent);
            virtual ~SelectWidget();
            void resizeEvent(QResizeEvent *e);
            QPushButton *upButton;
            QPushButton *downButton;
            QListWidget *list;
        public slots:
            void MoveUp();
            void MoveDown();
        };

#endif // SELECTWIDGET_HPP
