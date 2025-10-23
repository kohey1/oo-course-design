#include "addservicepagezbk.h"
#include "qmessagebox.h"
#include "ui_addservicepagezbk.h"

AddServicePageZbk::AddServicePageZbk(QString type, UsersZbk* user, QWidget *parent):QWidget(parent),ui(new Ui::AddServicePageZbk), user1(user), serviceType(type)
{
    ui->setupUi(this);
    if(type!="WeChat"){ui->letId->setEnabled(false); ui->letId->setText(user1->getId());ui->letId->setToolTip("用户id");}
    else ui->letId->setPlaceholderText("微信号");
    ui->letId->setClearButtonEnabled(true);
    if(type=="QQ")ui->letName->setPlaceholderText("昵称");
    else ui->letName->setPlaceholderText("用户名");
    ui->letName->setClearButtonEnabled(true);
    ui->letPass->setEchoMode(QLineEdit::Password);
    this->ui->btnFast-> setStyleSheet("QPushButton{background: transparent;color: blue;}"
                                         "QPushButton:hover{border:none;background: transparent;color:red; }"
                                     "QPushButton:pressed {background: transparent;color:red;}");
}
AddServicePageZbk::~AddServicePageZbk()
{
    delete ui;
}
void AddServicePageZbk::on_btnFast_clicked()
{
    ui->letId->setText(user1->getId());
    ui->letName->setText(user1->getNickname());
}
void AddServicePageZbk::on_btnRegister_clicked()
{
    if(ui->letId->text().isEmpty()||ui->letName->text().isEmpty()||ui->letPass->text().isEmpty())
    {
        QMessageBox::warning(this,"信息不全","请将信息填写完整");
        return;
    }
    for(auto i:User_ServiceZbk::allServiceUsers[serviceType])
    {
        if(i->getId()==ui->letId->text())
        {
            QMessageBox::warning(this,"请检查","该id已被其他用户使用");
            return;
        }
    }
    user1->getData()[serviceType]=new User_ServiceZbk(serviceType,ui->letName->text(),ui->letPass->text(),user1->getId());
    user1->setUserServiceId(ui->letId->text(),serviceType);
    QMap<QString, bool> isUsing = user1->getIsusing();
    isUsing[serviceType] = true;
    user1->setIsusing(isUsing);
    user1->getData()[serviceType]->setIsOnline(false);
    emit register_success();
    this->close(); // 关闭窗口
}

