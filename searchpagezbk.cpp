#include "searchpagezbk.h"
#include "ui_searchpagezbk.h"

SearchPageZbk::SearchPageZbk(UsersZbk* LoginUser,QString type,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchPageZbk),m_user(LoginUser),m_type(type)
{

    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    if(parent)
    {
        this->resize(parent->width(),parent->height());
        ui->mainStackedWidget->resize(parent->width(),parent->height());
    }
    QAction* searchAction=new QAction(this);
    searchAction->setIcon(QIcon(":/pic/search.png"));
    ui->letInput->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->letInput->setPlaceholderText("搜索");
    ui->letInput->setStyleSheet("border-bottom: none;");




    initAdviceFriends();
    ui->lst_advice->setSearchFriends(m_adviceFriends,m_type);
    ui->buttonGroup->setExclusive(true);
    ui->btnGroup->setCheckable(true);
    ui->btnUser->setCheckable(true);







    initPages();
    connect(ui->lst_advice,&FriendListWidget::SearchfriendClicked,this,&SearchPageZbk::switchAdviced);


}



SearchPageZbk::~SearchPageZbk()
{
    delete ui;
}

void SearchPageZbk::initAdviceFriends()
{
    m_adviceFriends.clear();
    for(auto it=m_user->getIsusing().constBegin();it!=m_user->getIsusing().constEnd();it++)//在自己当前服务之外且已启用的服务中
    {
        if(it.key()==m_type||it.value()==false)continue;
        for(QString fmainstr:m_user->getData()[it.key()]->m_friends)//对于这些其他服务中的好友，检查是不是有当前服务账号且当前用户没添加
        {
            UsersZbk* foundUser=UsersZbk::Find_UniTencentUser(fmainstr);//找到主账号方便跨服寻找
            if(foundUser==NULL)continue;
            if(!foundUser->getIsusing()[m_type])continue;//检查是不是有当前服务账号

            if(m_user->getData()[m_type]->m_friends.contains(foundUser->getData()[m_type]->getId()))continue;
            QString info;
            info=ServicesZbk::toZhcn[it.key()]+"好友:"+foundUser->getData()[m_type]->getName()+";";
            m_adviceFriends[foundUser->getData()[m_type]->getId()].append(info);
            qDebug()<<info<<Qt::endl;

        }

    }
}

QString SearchPageZbk::type() const
{
    return m_type;
}





void SearchPageZbk::on_btn_topclose_clicked()
{
    this->close();
}


void SearchPageZbk::on_btn_topmin_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void SearchPageZbk::initPages()
{
    if(m_type=="WeChat")
    {
        ui->btnGroup->setEnabled(false);
        ui->btnGroup->setText("不可找群");
        ui->btnGroup->setToolTip("微信中不可通过群号找群");
    }
    ui->btnUser->setChecked(true);
    ui->mainStackedWidget->setCurrentWidget(ui->page1_finduser);
    ui->user_stackedWidget->setCurrentWidget(ui->page_advice);



    ui->group_stackedWidget->setCurrentWidget(ui->page_NULL);
}



// 示例：在焦点事件中改变样式
void SearchPageZbk::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    // 设置激活状态的样式，例如一个蓝色的边框
    this->setStyleSheet("QWidget#SearchPageZbk { border: 2px solid #0099FF; }");
}

void SearchPageZbk::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
    // 设置非激活状态的样式，例如一个灰色的边框或无边框
    this->setStyleSheet("QWidget#SearchPageZbk { border: 1px solid #CCCCCC; }");
}

void SearchPageZbk::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        QRect rctop=rect();
        rctop.setBottom(this->ui->lbl_top->geometry().bottom());
        if(rctop.contains(event->pos()))
        {
            m_topIsPressing=true;
            m_startPos=event->globalPosition();
            m_framePos=this->frameGeometry().topLeft();

        }

    }

}

void SearchPageZbk::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        if(m_topIsPressing)
        {
            QPointF delta=event->globalPosition()-m_startPos;
            this->move((m_framePos+delta).toPoint());
        }
    }
}

void SearchPageZbk::mouseReleaseEvent(QMouseEvent *event)
{
    m_topIsPressing=false;
}


void SearchPageZbk::on_btnUser_clicked()
{
    ui->mainStackedWidget->setCurrentWidget(ui->page1_finduser);
    ui->user_stackedWidget->setCurrentWidget(ui->page_advice);
}


void SearchPageZbk::on_btnGroup_clicked()
{
    ui->mainStackedWidget->setCurrentWidget(ui->findGroup_page);
    ui->group_stackedWidget->setCurrentWidget(ui->page_NULL);
}


void SearchPageZbk::on_btnSearch_User_clicked()
{
    QString inputId=ui->letInput->text();
    ui->addUserButton->setEnabled(true);
    ui->addUserButton->setText("添加");
    for(auto i:User_ServiceZbk::allServiceUsers[m_type])
    {
        if(i->getId()==inputId)
        {
            ui->user_stackedWidget->setCurrentWidget(ui->page_foundUser);
            ui->lbl_foundUserId->setText(i->getId());
            ui->lbl_foundUserName->setText(i->getName());
            ui->lbl_foundUserAvatar->setPixmap(QPixmap(i->getAvatar()));
            ui->lbl_foundUserAge->setText(QString::number(i->age())+"岁");
            ui->lbl_foundUserGender->setText("性别 "+User_ServiceZbk::GendertoString(i->gender()));
            ui->lbl_foundUserTAge->setText("T龄 "+QString::number(i->registerDate().daysTo(QDate::currentDate()))+"天");
            if((m_user->getData()[m_type]->getId()==inputId)||m_user->getData()[m_type]->m_friends.contains(inputId))
            {
                ui->addUserButton->setEnabled(false);
                ui->addUserButton->setText("已经是您的好友");
            }
            return;



        }

    }
    ui->user_stackedWidget->setCurrentWidget(ui->page_NullUser);
}


void SearchPageZbk::on_addUserButton_clicked()
{
    // m_user->addFriend(ui->lbl_foundUserId->text(),m_type);
    // emit friendAdded();
    // initAdviceFriends();
    // ui->lst_advice->setSearchFriends(m_adviceFriends,m_type);
    // ui->addUserButton->setText("已是您的好友");
    // ui->addUserButton->setEnabled(false);
    User_ServiceZbk* found=UsersZbk::FindUser_IdService(ui->lbl_foundUserId->text(),m_type);
    if(!found)return;
    emit friendAdded(found);
}
void SearchPageZbk::on_btnSearch_group_clicked()
{
    QString inputId=ui->letInput_group->text();
    ui->addGroupButton->setEnabled(true);
    ui->addGroupButton->setText("加入");
    ui->group_stackedWidget->setCurrentWidget(ui->foundGroupPage);
    for(auto i:groupFactory::allGroups[m_type])
    {
        if(i->getId()==inputId)
        {
            ui->lbl_groupAvatar->setPixmap(QPixmap(i->getAvatar()));
            ui->lbl_groupId->setText(i->getId());
            ui->lbl_groupName->setText(i->getName());
            ui->lbl_groupOwner->setText("群主 "+UsersZbk::FindUser_IdService(i->OwnerId(),m_type)->getName());
            ui->lbl_groupDate->setText("建群日期 "+i->createDate().toString("yyyy-MM-dd"));
            ui->lbl_groupSize->setText(QString::number(i->getSize())+"人");





            if(m_user->getData()[m_type]->m_groups.contains(inputId))
            {
                ui->addGroupButton->setEnabled(false);
                ui->addGroupButton->setText("已在该群中");
            }
            return;
        }

    }
    ui->group_stackedWidget->setCurrentWidget(ui->page_NULL);


}




void SearchPageZbk::on_addGroupButton_clicked()
{
    m_user->addGroup(ui->letInput_group->text(),m_type);
    emit refreshPage();
    ui->addGroupButton->setText("已在该群中");
    ui->addGroupButton->setEnabled(false);
}

void SearchPageZbk::switchAdviced(User_ServiceZbk* user)
{
    ui->addUserButton->setEnabled(true);
    ui->addUserButton->setText("添加");
    ui->user_stackedWidget->setCurrentWidget(ui->page_foundUser);
    ui->lbl_foundUserId->setText(user->getId());
    ui->lbl_foundUserName->setText(user->getName());
    ui->lbl_foundUserAvatar->setPixmap(QPixmap(user->getAvatar()));
    ui->lbl_foundUserAge->setText(QString::number(user->age())+"岁");
    ui->lbl_foundUserGender->setText("性别 "+User_ServiceZbk::GendertoString(user->gender()));
    ui->lbl_foundUserTAge->setText("T龄 "+QString::number(user->registerDate().daysTo(QDate::currentDate()))+"天");
    if(m_user->getData()[m_type]->m_friends.contains(user->getId()))
    {
        ui->addUserButton->setEnabled(false);
        ui->addUserButton->setText("已经是您的好友");
    }

}

