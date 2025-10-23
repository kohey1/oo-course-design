#include "modifyloginuserinfopagezbk.h"
#include "ui_modifyloginuserinfopagezbk.h"

modifyLoginUserInfoPageZbk::modifyLoginUserInfoPageZbk(User_ServiceZbk* user,QWidget *parent): QDialog(parent), ui(new Ui::modifyLoginUserInfoPageZbk),m_user(user)
{
    ui->setupUi(this);
    mode=MY_SERVICE;
    ui->lbl_avatar->setPixmap(QPixmap(user->getAvatar()));
    ui->lbl_avatar->setScaledContents(true);
    ui->let_Location->setText(user->Location());
    ui->let_Name->setText(user->getName());
    ui->cb_Gender->setCurrentIndex(user->gender());
    ui->de_birthday->setDate(user->birthday());
    ui->de_birthday->setCalendarPopup(true);
}

modifyLoginUserInfoPageZbk::modifyLoginUserInfoPageZbk(UsersZbk *mainuser, QWidget *parent):QDialog(parent), ui(new Ui::modifyLoginUserInfoPageZbk),m_mainUser(mainuser)
{
    ui->setupUi(this);
    mode=MY_UNITENCENT;
    ui->lbl_avatar->setPixmap(QPixmap(mainuser->getavatarPath()));
    ui->lbl_avatar->setScaledContents(true);
    ui->let_Location->setText(mainuser->getLocation());
    ui->let_Name->setText(mainuser->getNickname());
    ui->cb_Gender->setCurrentIndex(mainuser->gender());
    ui->de_birthday->setDate(mainuser->getBirthday());
    ui->de_birthday->setCalendarPopup(true);
}

modifyLoginUserInfoPageZbk::~modifyLoginUserInfoPageZbk()
{
    delete ui;
}

void modifyLoginUserInfoPageZbk::on_btn_confirmInfo_clicked()
{
    bool isChanged=false;
    if(mode==MY_SERVICE)
    {
        if(ui->let_Location->text()!=m_user->Location())
        {
            m_user->setLocation(ui->let_Location->text());
            isChanged=true;
        }
        if(ui->let_Name->text()!=m_user->getName())
        {
            m_user->setNickName(ui->let_Name->text());
            isChanged=true;
        }
        if(ui->cb_Gender->currentIndex()!=m_user->gender())
        {
            m_user->setGender(Gender(ui->cb_Gender->currentIndex()));
            isChanged=true;
        }
        if(ui->de_birthday->date()!=m_user->birthday())
        {
            m_user->setBirthday(ui->de_birthday->date());
            m_user->setAge(m_user->birthday().daysTo(QDate::currentDate())/365);
            isChanged=true;
        }
    }
    else if(mode==MY_UNITENCENT)
    {
        if(ui->let_Location->text()!=m_mainUser->getLocation())
        {
            m_mainUser->setLocation(ui->let_Location->text());
            isChanged=true;
        }
        if(ui->let_Name->text()!=m_mainUser->getNickname())
        {
            m_mainUser->setNickname(ui->let_Name->text());
            isChanged=true;
        }
        if(ui->cb_Gender->currentIndex()!=m_mainUser->gender())
        {
            m_mainUser->setGender(Gender(ui->cb_Gender->currentIndex()));
            isChanged=true;
        }
        if(ui->de_birthday->date()!=m_mainUser->getBirthday())
        {
            m_mainUser->setBirthday(ui->de_birthday->date());
            isChanged=true;
        }
    }




    if(isChanged)
        emit LoginUserInfoModified();

    this->close();

}


void modifyLoginUserInfoPageZbk::on_btn_cancel_clicked()
{
    this->close();
}

