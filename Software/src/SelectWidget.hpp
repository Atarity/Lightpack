#ifndef SELECTWIDGET_HPP
#define SELECTWIDGET_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>

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
