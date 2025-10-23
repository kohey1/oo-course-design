#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QWidget>
#include<QRegularExpressionValidator>
#include<QRegularExpression>
#include<QMessageBox>
#include"userszbk.h"

namespace Ui {
class RegisterPage;
}

class RegisterPage : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterPage(QWidget *parent = nullptr);
    ~RegisterPage();

private slots:
    void on_regi_Finish_Button_clicked();

private:
    Ui::RegisterPage *ui;
};

#endif // REGISTERPAGE_H
