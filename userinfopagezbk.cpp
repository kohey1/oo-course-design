#include "userinfopagezbk.h"
#include "ui_userinfopagezbk.h"

UserInfoPageZbk::UserInfoPageZbk(UsersZbk* own,User_ServiceZbk* user,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserInfoPageZbk)
{

    ui->setupUi(this);
    m_user=user;
    m_own=own;
    setUserInfoPage();

}

UserInfoPageZbk::UserInfoPageZbk(UsersZbk *own, QWidget *parent) : QDialog(parent) , ui(new Ui::UserInfoPageZbk)
{
    ui->setupUi(this);
    m_own=own;
    setMainUserInfoPage();

}

UserInfoPageZbk::~UserInfoPageZbk()
{
    delete ui;
}

void UserInfoPageZbk::setUserInfoPage()
{

    if(m_user==NULL)return;

    ui->gb_FriendInfo->setTitle(ServicesZbk::toZhcn[m_user->getType()]+"好友信息");
    ui->lbl_setf_Age->setText(QString::number(m_user->age())+"岁");
    ui->lbl_setf_Avatar->setPixmap(QPixmap(m_user->getAvatar()));
    ui->lbl_setf_Gender->setText(User_ServiceZbk::GendertoString(m_user->gender()));
    ui->lbl_setf_Id->setText("Id "+m_user->getId());
    ui->lbl_setf_Name->setText(m_user->getName());
    ui->lbl_setf_birthday->setText("生日 "+m_user->birthday().toString("yyyy-MM-dd"));
    ui->lbl_setf_Tage->setText("T龄 "+QString::number(m_user->registerDate().daysTo(QDate::currentDate()))+"天");
    ui->lbl_setf_Location->setText("地区 "+m_user->Location());
    disconnect(ui->btn_ModifyItem, &QPushButton::clicked, 0, 0);
    if(m_user->getId()==m_own->getData()[m_user->getType()]->getId())
    {
        ui->btn_ModifyItem->setText("修改资料");
        connect(ui->btn_ModifyItem,&QPushButton::clicked,this,[=](){emit modifyUserInfo();});
    }
    else if(m_own->getData()[m_user->getType()]->m_friends.contains(m_user->getId()))
    {
        ui->btn_ModifyItem->setText("发消息");
        connect(ui->btn_ModifyItem,&QPushButton::clicked,this,[=](){emit sendMessage(m_user);this->close();});
    }
    else
    {
        ui->btn_ModifyItem->setText("加好友");
        connect(ui->btn_ModifyItem,&QPushButton::clicked,this,[=](){emit addFriend(m_user);setUserInfoPage();});
    }


}


void UserInfoPageZbk::setMainUserInfoPage()
{
    if(m_own==NULL)return;
    ui->gb_FriendInfo->setTitle("UniTencent信息");
    ui->lbl_setf_Age->setText("");
    ui->lbl_setf_Avatar->setPixmap(QPixmap(m_own->getavatarPath()));
    ui->lbl_setf_Gender->setText(User_ServiceZbk::GendertoString(m_own->gender()));
    ui->lbl_setf_Id->setText("Id "+m_own->getId());
    ui->lbl_setf_Name->setText(m_own->getNickname());
    ui->lbl_setf_birthday->setText("生日 "+m_own->getBirthday().toString("yyyy-MM-dd"));
    ui->lbl_setf_Tage->setText("T龄 "+QString::number(m_own->getRegisterDate().daysTo(QDate::currentDate()))+"天");
    qDebug()<<"registerDate:"<<m_own->getRegisterDate().toString();
    ui->lbl_setf_Location->setText("地区 "+m_own->getLocation());
    ui->btn_ModifyItem->setText("修改资料");
    disconnect(ui->btn_ModifyItem, &QPushButton::clicked, 0, 0);
    connect(ui->btn_ModifyItem,&QPushButton::clicked,this,[=](){emit modifyMainUserInfo();});
}
