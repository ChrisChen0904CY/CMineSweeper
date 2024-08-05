#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>

class CPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CPushButton(QWidget *parent = nullptr);

signals:
    void rightClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::RightButton) {
            emit rightClicked();
        } else {
            QPushButton::mousePressEvent(event);
        }
    }
};

#endif // CPUSHBUTTON_H
