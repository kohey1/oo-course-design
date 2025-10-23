#include "creategroupzbk.h"
#include "ui_creategroupzbk.h"

createGroupZbk::createGroupZbk(UsersZbk* LoginUser,QString type,QWidget *parent): QDialog(parent), ui(new Ui::createGroupZbk),m_user(LoginUser),m_type(type)
{
    ui->setupUi(this);
    ui->friendList->setMutiMode();
    ui->tb_selectedUser->setText("群成员:");
    ui->tb_selectedUser->append(m_user->getData()[m_type]->getName()+"(群主)");
    ui->btn_Confirm->setEnabled(false);
    this->setWindowFlag(Qt::FramelessWindowHint);

    // QMap<QString,QString>allFriends;
    // for(auto i:m_user->getData()[m_type]->m_friends)
    // {
    //     allFriends[i]="";
    // }

    ui->friendList->setSearchFriends(m_user->getData()[m_type]->m_friends,m_type);
    connect(ui->friendList,&FriendListWidget::updateSelection,this,&createGroupZbk::updateSelection);

}

createGroupZbk::createGroupZbk(UsersZbk *LoginUser, QString type, QVector<QString> targetMember, QWidget *parent): QDialog(parent), ui(new Ui::createGroupZbk),m_user(LoginUser),m_type(type)
{
    ui->setupUi(this);
    ui->friendList->setMutiMode();
    ui->tb_selectedUser->setText("群成员:");
    ui->tb_selectedUser->append(m_user->getData()[m_type]->getName()+"(群主)");
    ui->btn_Confirm->setEnabled(false);
    this->setWindowFlag(Qt::FramelessWindowHint);

    // QMap<QString,QString>allFriends;
    // for(auto i:m_user->getData()[m_type]->m_friends)
    // {
    //     allFriends[i]="";
    // }

    targetMember.removeAll(m_user->getData()[m_type]->getId());
    ui->friendList->setSearchFriends(targetMember,m_type);
    connect(ui->friendList,&FriendListWidget::updateSelection,this,&createGroupZbk::updateSelection);
}

createGroupZbk::~createGroupZbk()
{
    delete ui;
}

void createGroupZbk::on_btn_Cancel_clicked()
{
    this->close();
}

void createGroupZbk::updateSelection(QVector<User_ServiceZbk *>selectedUsers)
{
    member.clear();
    member.append(selectedUsers);
    ui->tb_selectedUser->clear();
    ui->tb_selectedUser->append("群成员:");
    ui->tb_selectedUser->append(m_user->getData()[m_type]->getName()+"(群主)");
    for(auto i:selectedUsers)
    {
        ui->tb_selectedUser->append(i->getName());
    }
    if(!selectedUsers.empty())
    {
        ui->btn_Confirm->setEnabled(true);
    }
    else
        ui->btn_Confirm->setEnabled(false);
}


void createGroupZbk::on_btn_Confirm_clicked()
{
    GroupsZbk* group=groupFactory::createGroup(m_type,m_user->getData()[m_type]->getId());
    QString name;
    bool isOversized=false;
    bool isContinueAdd=true;
    groupFactory::AddMember(group,m_user->getData()[m_type]->getId());
    name.append(m_user->getData()[m_type]->getName()+"、");
    if(name.length()>=25)
    {
        name.resize(25);
        isOversized=true;
        isContinueAdd=false;
    }

    for(auto i:member)
    {
        // group->addMember(i->getId());
        groupFactory::AddMember(group,i->getId());
        QString tmp=name+i->getName();
        if(isContinueAdd&&tmp.length()<=25)
        {
            name.append(i->getName()+"、");
        }
        else
        {
            isOversized=true;
        }
    }
    if(isContinueAdd)
        name.removeLast();
    if(isOversized)
        name.append("...");
    group->setName(name);


    emit groupCreated();
    this->close();

}

