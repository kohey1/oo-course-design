#include "registerpage.h"
#include "ui_registerpage.h"

RegisterPage::RegisterPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterPage)
{
    ui->setupUi(this);
    this->ui->regi_Finish_Button->setStyleSheet("QPushButton{background-color: rgb(99, 172, 245);border-radius:5px;color: white;}"
                                                "QPushButton:hover{background-color:rgb(152, 205, 245); }"
                                                "QPushButton:pressed{background-color:rgb(116, 151, 245);}");
    this->ui->passEdit->setClearButtonEnabled(true);
    this->ui->passEdit->setEchoMode(QLineEdit::Password);
    this->ui->passEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
    this->ui->nickEdit->setClearButtonEnabled(true);

    this->setWindowTitle("注册-UniTencent");
    this->ui->passEdit_2->setClearButtonEnabled(true);
    this->ui->passEdit_2->setEchoMode(QLineEdit::Password);
    this->ui->passEdit_2->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));








}

RegisterPage::~RegisterPage()
{
    delete ui;
}

void RegisterPage::on_regi_Finish_Button_clicked()
{
    if(this->ui->nickEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"请检查","昵称不允许为空");
        return;
    }
    if(this->ui->nickEdit->text().contains(','))
    {
        QMessageBox::warning(this,"请检查","昵称中不允许包含逗号");
        return;
    }

    if(this->ui->passEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"请检查","密码不允许为空");
        return;
    }
    if(this->ui->passEdit_2->text().isEmpty())
    {
        QMessageBox::warning(this,"请检查","请确认您的密码");
        return;
    }
    if(this->ui->passEdit->text()!=this->ui->passEdit_2->text())
    {
        QMessageBox::warning(this,"请检查","两次输入的密码不一致");
        return;
    }
    UsersZbk* newUser=new UsersZbk(this->ui->nickEdit->text(),this->ui->passEdit->text());
    QString userId=newUser->getId();
    newUser->allUsers.push_back(newUser);
    QString information="注册成功！您的账号为"+userId;
    QMessageBox::information(this,"注册",information);
    this->hide();

}

