#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QRegularExpressionValidator>
#include "registerpage.h"
#include "mainpagezbk.h"
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE



class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::Widget *ui;
    RegisterPage* m_rePage;


    void inittestUser();
    void initlogin();

};


#endif // WIDGET_H
