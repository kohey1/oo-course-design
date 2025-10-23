#include "mainpagezbk.h"
#include "ui_mainpagezbk.h"
#include "ui_qqpagezbk.h"
#include "ui_wechatpagezbk.h"
#include "ui_weibopagezbk.h"

MainPageZbk::MainPageZbk(UsersZbk* user,QWidget *parent): QWidget(parent), ui(new Ui::MainPageZbk),m_user(user)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon(":/pic/favicon.ico"));
    this->setAttribute(Qt::WA_DeleteOnClose);


    this->ui->btn_topNick->setFlat(true);
    this->ui->btn_topNick->setText(m_user->getNickname());
    this->ui->btn_topNick->setStyleSheet("QPushButton {border: none; background: transparent;color:black;}");
    QPixmap circularAvatar(50, 50);
    circularAvatar.fill(Qt::transparent);

    QPainter painter(&circularAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QPixmap(user->getavatarPath()).scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 50, 50);

    this->ui->lbl_topava->setPixmap(circularAvatar);
    this->ui->lbl_topIcon->setPixmap(QPixmap(":/pic/favicon.ico"));
    this->ui->lbl_topIcon->setScaledContents(true);
    serpage=NULL;
    if(serpage==NULL)
        serpage=new ServiceListPageZbk(m_user,ui->stackedWidget);

    // qqpage = new QQPageZbk(user,ui->stackedWidget);
    // qqpage->setObjectName("qqpage");
    ui->m_sideBar->setFlat(true);

    m_sideBarLayout=new QVBoxLayout(ui->m_sideBar);
    m_sideBarLayout->setSpacing(5);
    m_sideBarLayout->setContentsMargins(0,10,5,10);
    m_sideBarLayout->setAlignment(Qt::AlignTop);



    sideButtons=new QButtonGroup(this);
    sideButtons->setExclusive(true);


    for(auto str:ServicesZbk::allServices)
    {
        m_btnIschecked[str->Id]=false;
    }
    m_btnIschecked["more"]=true;





    // ui->stackedWidget->addWidget(qqpage);
    // ui->stackedWidget->setCurrentWidget(qqpage);
    updateServiceButtons();
    //connect(serpage,&ServiceListPageZbk::service_changed,this,&MainPageZbk::updateServiceButtons);
    ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
    m_targetFriend=NULL;
    m_targetGroup=NULL;
    m_MessageState=toNull;
    connect(ui->lst_groupMember,&FriendListWidget::GroupMemberRightClicked,this,&MainPageZbk::showManageMenu);

    QAction* searchIconAction=new QAction;
    searchIconAction->setIcon(QIcon(":/pic/search.png"));
    ui->let_SearchGroupMember->addAction(searchIconAction,QLineEdit::LeadingPosition);
    ui->btn_setItem->setPopupMode(QToolButton::InstantPopup);
    ui->btn_setg_ChangeType->setPopupMode(QToolButton::InstantPopup);
    createMenus();

    m_model=new ChatMessageModelZbk(this);
    m_delegate=new ChatMessageDelegateZbk(this);
    ui->lst_messages->setModel(m_model);
    ui->lst_messages->setItemDelegate(m_delegate);
    ui->lst_messages->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->lst_messages->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->lst_messages->setAlternatingRowColors(false);
    ui->lst_messages->setSelectionMode(QAbstractItemView::NoSelection);



    connect(ui->btn_sendMessage,&QPushButton::clicked,this,&MainPageZbk::sendMessage);
    connect(ui->messageEditor,&QTextEdit::textChanged,[this](){ui->btn_sendMessage->setEnabled(!ui->messageEditor->toPlainText().trimmed().isEmpty());});
    connect(ui->lst_messages->verticalScrollBar(),&QScrollBar::rangeChanged,this,&MainPageZbk::scrollToBottom);
    connect(ui->lst_systemNotices, &FriendListWidget::SystemNoticeRightClicked, this, &MainPageZbk::onSystemNoticeRightClicked);
    connect(m_user,&UsersZbk::refreshPage,this,&MainPageZbk::reloadFriend_GroupList);
    reloadSystemNotice();
}

MainPageZbk::~MainPageZbk()
{
    for(auto it=m_user->getIsusing().begin();it!=m_user->getIsusing().end();it++)
    {
        if(it.value())
            m_user->getData()[it.key()]->setIsOnline(false);
    }

    delete ui;
}

void MainPageZbk::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        QRect rctop=rect();
        rctop.setBottom(this->ui->topwidget->geometry().bottom());
        if(rctop.contains(event->pos()))
        {
            m_topIsPressing=true;
            m_startPos=event->globalPosition();
            m_framePos=this->frameGeometry().topLeft();

        }

    }

}

void MainPageZbk::mouseMoveEvent(QMouseEvent *event)
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

void MainPageZbk::mouseReleaseEvent(QMouseEvent *event)
{
    m_topIsPressing=false;
}


void MainPageZbk::on_btnCloseWin_clicked()
{
    this->close();
}


void MainPageZbk::on_btnHideWin_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}






void MainPageZbk::onServiceButtonClicked()
{
    nochecked = false;
    for(auto str : ServicesZbk::allServices) {
        m_btnIschecked[str->Id] = false;
    }
    m_btnIschecked["more"] = false;

    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    QString serviceType = clickedButton->property("serviceType").toString();
    m_type=serviceType;
    m_btnIschecked[serviceType] = true;

    qDebug() << "button:" << serviceType << Qt::endl;

    if(serviceType == "more") {
        if(serpage) {
            ui->stackedWidget->setCurrentWidget(serpage);
        }
        ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
        m_MessageState=toNull;
        ui->messageEditor->clear();
        return;
    }

    // 检查服务是否存在且已启用
    if(!m_user->getIsusing().value(serviceType, false)) {
        qDebug() << "服务未启用:" << serviceType;
        return;
    }

    // 检查服务数据是否存在
    if(!m_user->getData().contains(serviceType) || !m_user->getData()[serviceType]) {
        qDebug() << "服务数据不存在:" << serviceType;
        return;
    }

    if(m_user->getData()[serviceType]->getOnline())
    {
        if(serviceType == "QQ" && qqpage)
        {
            ui->stackedWidget->setCurrentWidget(qqpage);
        }
        else if(serviceType == "WeChat" && wxpage)
        {
            ui->stackedWidget->setCurrentWidget(wxpage);
        }

        else if(serviceType == "WeiBo" && wbpage)
        {
            ui->stackedWidget->setCurrentWidget(wbpage);
        }
    }
    else
    {
        // 安全地设置在线状态
        if(m_user->getData()[serviceType])
        {
            m_user->getData()[serviceType]->setIsOnline(true);
            updateServiceButtons();
        }
    }



    ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
    m_MessageState=toNull;
    ui->messageEditor->clear();
}

void MainPageZbk::showMessagePage(User_ServiceZbk *user)
{

    if(user==NULL)return;
    if(m_MessageState==toFriend)
    {
        if(user->getId()!=m_targetFriend->getId())
            ui->messageEditor->clear();
    }
    else
    {
        m_targetGroup=NULL;
        ui->messageEditor->clear();
    }
    m_MessageState=toFriend;
    m_targetFriend=user;
    disconnect(ui->btn_setItem,&QPushButton::clicked,0,0);
    ui->btn_setItem->setMenu(setItemMenu);
    ui->btn_setItem->setArrowType(Qt::NoArrow);
    ui->Message_stackedWidget->setCurrentWidget(ui->common_MessagePage);
    ui->lbl_ItemName->setText(user->getName());
    m_session=ChatSessionZbk::getOrCreatePrivateSession(m_user->getData()[m_type]->getId(),user->getId(),user->getType());
    if(m_session){loadMessages(m_session);qDebug()<<"loading session"<<m_session->getSessionId();};
}

void MainPageZbk::showGroupMessagePage(GroupsZbk *group)
{
    if(group==NULL)return;
    if(m_MessageState==toGroup)
    {
        if(group->getId()!=m_targetGroup->getId())
            ui->messageEditor->clear();
    }
    else
    {
        m_targetFriend=NULL;
        ui->messageEditor->clear();
    }

    m_MessageState=toGroup;
    m_targetGroup=group;
    ui->btn_setItem->setMenu(NULL);
    connect(ui->btn_setItem,&QPushButton::clicked,this,&MainPageZbk::on_btn_setItem_clicked);
    loadAddMemberMenu();
    m_session=ChatSessionZbk::getOrCreateGroupSession(m_targetGroup->getId(),m_targetGroup->getMembers(),m_targetGroup->getServiceType());
    if(m_session)loadMessages(m_session);

    disconnect(ui->btn_setg_ExitGroup,&QPushButton::clicked,0,0);
    if(m_user->getData()[group->getServiceType()]->getId()==group->OwnerId())
    {
        ui->btn_setg_ExitGroup->setText("解散群聊");
        connect(ui->btn_setg_ExitGroup,&QPushButton::clicked,this,[=](){
            if(QMessageBox::question(this,"解散群聊",QString("是否解散群聊 %1 ?").arg(group->getName()))==QMessageBox::No)return;
            ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
            QString tmp_type=group->getServiceType();
            groupFactory::deleteGroup(group->getId(),group->getServiceType());
            reloadFriend_GroupList(tmp_type);
            m_targetGroup=NULL;
            m_MessageState=toNull;
        });
    }
    else
    {
        ui->btn_setg_ExitGroup->setText("退出群聊");
        connect(ui->btn_setg_ExitGroup,&QPushButton::clicked,this,[=](){
            if(QMessageBox::question(this,"退出群聊",QString("是否退出群聊 %1 ?").arg(group->getName()))==QMessageBox::No)return;
            ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
            m_MessageState=toNull;
            QString tmp_type=group->getServiceType();
            groupFactory::RemoveMember(group,m_user->getData()[group->getServiceType()]->getId());
            // group->removeMember(m_user->getData()[group->getServiceType()]->getId());
            reloadFriend_GroupList(tmp_type);
            m_targetGroup=NULL;
        });
    }
    ui->lbl_ItemName->setText(group->getName());
    ui->Message_stackedWidget->setCurrentWidget(ui->common_MessagePage);

    if(m_targetGroup->allowSubGroups())
    {
        ui->btn_createSubGroup->setEnabled(true);
        ui->btn_createSubGroup->setToolTip("选择当前群中的用户创建群聊");
    }
    else
    {
        ui->btn_createSubGroup->setEnabled(false);
        ui->btn_createSubGroup->setToolTip(QString("%1群不允许创建子群").arg(ServicesZbk::toZhcn[m_targetGroup->getServiceType()]));
    }

}

void MainPageZbk::clearSerivceButtons()
{
    for(QPushButton* button:m_serviceButtons)
    {
        m_sideBarLayout->removeWidget(button);
        delete button;
    }
    m_serviceButtons.clear();
}

void MainPageZbk::clearServicePages()
{
    // 安全地删除页面
    if(qqpage) {

        disconnect(qqpage->ui->lstFriend,&FriendListWidget::friendClicked,this,&MainPageZbk::showMessagePage);


        ui->stackedWidget->removeWidget(qqpage);
        delete qqpage;
        qqpage = NULL;
    }
    if(wxpage) {
        ui->stackedWidget->removeWidget(wxpage);
        delete wxpage;
        wxpage = NULL;
    }
    if(wbpage) {
        ui->stackedWidget->removeWidget(wbpage);
        delete wbpage;
        wbpage = NULL;
    }
    if(serpage) {
        // 不要删除serpage，因为它会被重新使用
        // 只是从stackedWidget中移除
        ui->stackedWidget->removeWidget(serpage);
        // 不断开信号连接，因为我们希望继续接收更新
    }
}

void MainPageZbk::updateServiceButtons()
{
    clearSerivceButtons();
    clearServicePages();
    bool count=true;//默认选择第一个服务
    if(!m_user)return;
    if(serpage) {
        // 先断开可能的旧连接，避免重复连接
        disconnect(serpage, &ServiceListPageZbk::service_changed, this, &MainPageZbk::updateServiceButtons);
        connect(serpage, &ServiceListPageZbk::service_changed, this, &MainPageZbk::updateServiceButtons);
    }
    for(auto serviceType:ServicesZbk::allServices)
    {
        //仅初始化用户已启用的服务
        if(!(m_user->getIsusing()[serviceType->Id]))continue;


        QString state;

        //对于每个已添加服务，生成按钮并加入buttonGroup
        QPushButton* button=new QPushButton;
        button->setCheckable(true);
        button->setFixedSize(61,61);
        button->setProperty("serviceType",serviceType->Id);
        if(m_user->getData()[serviceType->Id]->getOnline()) {button->setIcon(QIcon(serviceType->using_Icon));state="(在线)";}
        else {button->setIcon(QIcon(serviceType->offLine_Icon));state="(离线)";}
        button->setToolTip(serviceType->Id_cn+state);
        button->setIconSize(QSize(button->width()-20,button->height()-20));
        if(m_btnIschecked[serviceType->Id])button->setChecked(true);
        if(serviceType->Id=="QQ")
            button->setStyleSheet("QPushButton { text-align: bottom; padding: 5px; }"
                                "QPushButton:checked {background-color: #0078d4;  color: white;}");
        else if(serviceType->Id=="WeChat")
            button->setStyleSheet("QPushButton { text-align: bottom; padding: 5px; }"
                                  "QPushButton:checked {background-color: green;color: white;}");
        else if(serviceType->Id=="WeiBo")
            button->setStyleSheet("QPushButton { text-align: bottom; padding: 5px; }"
                                  "QPushButton:checked {background-color: yellow;color: white;}");
        m_sideBarLayout->addWidget(button);
        sideButtons->addButton(button);
        connect(button, &QPushButton::clicked, this, &MainPageZbk::onServiceButtonClicked);
        m_serviceButtons[serviceType->Id]=button;




        //根据用户在线状态，选择性初始化各服务界面
        if(serviceType->Id=="QQ"&&m_user->getData()["QQ"]->getOnline())
        {
            qqpage=new QQPageZbk(m_user,ui->stackedWidget);
            connect(qqpage->ui->lstFriend,&FriendListWidget::friendClicked,this,&MainPageZbk::showMessagePage);
            connect(qqpage->ui->lstGroup,&FriendListWidget::groupClicked,this,&MainPageZbk::showGroupMessagePage);
            connect(qqpage,&QQPageZbk::addFriend,this,&MainPageZbk::on_addFriendResquseted);
            qqpage->setObjectName("qqpage");
            ui->stackedWidget->addWidget(qqpage);
        }
        else if(serviceType->Id=="WeChat"&&m_user->getData()["WeChat"]->getOnline())
        {

            wxpage=new WeChatPageZbk(m_user,ui->stackedWidget);
            connect(wxpage->ui->lstFriend,&FriendListWidget::friendClicked,this,&MainPageZbk::showMessagePage);
            connect(wxpage->ui->lstGroup,&FriendListWidget::groupClicked,this,&MainPageZbk::showGroupMessagePage);
            connect(wxpage,&WeChatPageZbk::addFriend,this,&MainPageZbk::on_addFriendResquseted);


            wxpage->setObjectName("wxpage");
            ui->stackedWidget->addWidget(wxpage);
        }
        else if(serviceType->Id=="WeiBo"&&m_user->getData()["WeiBo"]->getOnline())
        {
            wbpage=new WeiBoPageZbk(m_user,ui->stackedWidget);
            connect(wbpage->ui->lstFriend,&FriendListWidget::friendClicked,this,&MainPageZbk::showMessagePage);
            connect(wbpage,&WeiBoPageZbk::addFriend,this,&MainPageZbk::on_addFriendResquseted);

            wbpage->setObjectName("wbpage");
            ui->stackedWidget->addWidget(wbpage);
        }
    }


    //额外页：服务控制中心
    QPushButton* button=new QPushButton;
    button->setToolTip("更多服务");
    button->setCheckable(true);
    button->setFixedSize(61,61);
    button->setProperty("serviceType","more");
    button->setIcon(QIcon(":/pic/services1.ico"));
    button->setStyleSheet("QPushButton { text-align: bottom; padding: 5px; }"
                          "QPushButton:checked {background-color: gray;  color: white;}");
    m_sideBarLayout->addWidget(button);
    sideButtons->addButton(button);
    connect(button, &QPushButton::clicked, this, &MainPageZbk::onServiceButtonClicked);
    if(serpage==NULL)
        serpage=new ServiceListPageZbk(m_user,ui->stackedWidget);
    ui->stackedWidget->addWidget(serpage);
    m_serviceButtons["more"]=button;



    QMap<QString, bool>::const_iterator it;
    for (it =m_btnIschecked.constBegin(); it != m_btnIschecked.constEnd(); ++it)
    {
        if(it.value())
        {
            m_serviceButtons[it.key()]->setChecked(true);
            break;
        }
    }
    QAbstractButton* checkedButton = sideButtons->checkedButton();
    if(checkedButton)
    {
        QString serviceType = checkedButton->property("serviceType").toString();
        m_type=serviceType;
        if(serviceType == "QQ" && qqpage) {
            ui->stackedWidget->setCurrentWidget(qqpage);
        } else if(serviceType == "WeChat" && wxpage) {
            ui->stackedWidget->setCurrentWidget(wxpage);
        } else if(serviceType == "WeiBo" && wbpage) {
            ui->stackedWidget->setCurrentWidget(wbpage);
        } else if(serviceType == "more" && serpage) {
            ui->stackedWidget->setCurrentWidget(serpage);
        }
    }
    isFirst = false;
   // DataManagerZbk::instance().saveAllData();

    emit service_changed();
}

void MainPageZbk::on_addFriendResquseted(User_ServiceZbk *user)
{
    // m_user->addFriend(user->getId(),user->getType());
    // if(user->getType()=="QQ")qqpage->reloadFriends();
    // else if(user->getType()=="WeChat")wxpage->reloadFriends();
    // else if(user->getType()=="WeiBo")wbpage->reloadFriends();
    m_user->sendFriendRequest(user->getId(),user->getType());
    QMessageBox::information(this,"好友申请","已发送好友申请");

}







void MainPageZbk::on_btn_setItem_clicked()
{

    if(m_MessageState==toGroup)
    {
        if(m_targetGroup==NULL)return;
        ui->Message_stackedWidget->setCurrentWidget(ui->MessagePage_groupSetting);
        ui->lbl_setg_Avatar->setPixmap(QPixmap(m_targetGroup->getAvatar()));
        ui->lbl_setg_Avatar->setScaledContents(true);
        ui->lbl_setg_Date->setText("建群日期 "+m_targetGroup->createDate().toString("yyyy-MM-dd"));
        ui->lbl_setg_Id->setText("群号 "+m_targetGroup->getId());
        ui->lbl_setg_Size->setText("成员数 "+QString::number(m_targetGroup->getMembers().size())+"人");
        ui->lbl_setg_Type->setText("当前类型 "+ServicesZbk::toZhcn[m_targetGroup->getServiceType()]);
        ui->let_setg_Name->setText(m_targetGroup->getName());
        if(m_targetGroup->canManageMembers(m_user->getData()[m_targetGroup->getServiceType()]->getId()))
        {
            ui->let_setg_Name->setEnabled(true);
            ui->let_setg_Name->setToolTip("");
            ui->btn_setg_ChangeType->setEnabled(true);
            ui->btn_setg_ChangeType->setToolTip("");
        }
        else
        {
            ui->let_setg_Name->setEnabled(false);
            ui->let_setg_Name->setToolTip("群成员不可修改群名称");
            ui->btn_setg_ChangeType->setEnabled(false);
            ui->btn_setg_ChangeType->setToolTip("群成员不可变更群类型");
        }

        ui->lst_groupMember->setManageMembers(m_targetGroup->getMembers(),m_targetGroup->getServiceType(),e_ManageGroupMembers);
    }
}

void MainPageZbk::DeleteChattingFriend()
{
    onDeleteFriendRequested(m_targetFriend);

    m_targetFriend=NULL;
    m_MessageState=toNull;
    ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);

}

void MainPageZbk::onDeleteFriendRequested(User_ServiceZbk *user)
{
    if(user==NULL)return;
    if(QMessageBox::question(this,"请检查","是否删除"+user->getType()+"好友 "+user->getName()+" ?")==QMessageBox::No)return;
    m_user->removeFriend(user->getId(),user->getType());
    // if(user->getType()=="QQ")qqpage->reloadFriends();
    // else if(user->getType()=="WeChat")wxpage->reloadFriends();
    reloadFriend_GroupList(user->getType());
    DataManagerZbk::instance().saveAllData();
    user=NULL;


}

void MainPageZbk::showChattingFriendInfo()
{
    onshowUserInfoRequested(m_targetFriend);
}

void MainPageZbk::onshowUserInfoRequested(User_ServiceZbk *user)
{
    m_userInfoPage=NULL;
    m_userInfoPage=new UserInfoPageZbk(m_user,user);
    m_userInfoPage->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_userInfoPage,&UserInfoPageZbk::addFriend,this,&MainPageZbk::on_addFriendResquseted);
    connect(m_userInfoPage,&UserInfoPageZbk::sendMessage,this,&MainPageZbk::showMessagePage);
    connect(m_userInfoPage,&UserInfoPageZbk::modifyUserInfo,[this,user](){
        m_userInfoPage->close();
        modifyLoginUserInfoPageZbk* modifyPage=new modifyLoginUserInfoPageZbk(user);
        modifyPage->setAttribute(Qt::WA_DeleteOnClose);
        modifyPage->exec();

    });
    m_userInfoPage->exec();
}

void MainPageZbk::onshowUserInfoRequested(UsersZbk *user)
{
    m_userInfoPage=NULL;
    m_userInfoPage=new UserInfoPageZbk(user);
    m_userInfoPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_userInfoPage,&UserInfoPageZbk::modifyMainUserInfo,[this,user](){
        m_userInfoPage->close();
        modifyLoginUserInfoPageZbk* modifyPage=new modifyLoginUserInfoPageZbk(user);
        connect(modifyPage,&modifyLoginUserInfoPageZbk::LoginUserInfoModified,this,[=](){ui->btn_topNick->setText(m_user->getNickname());DataManagerZbk::instance().saveUsers();});
        modifyPage->setAttribute(Qt::WA_DeleteOnClose);
        modifyPage->exec();

    });
    m_userInfoPage->exec();
}



void MainPageZbk::sendMessage()
{
    QString content=ui->messageEditor->toPlainText().trimmed();
    if(content.isEmpty())return;

    m_model->addMessage(m_user->getData()[m_type]->getId(),content);
    qDebug()<<"发送消息时服务："<<m_type;
    reloadFriend_GroupList(m_type);
    DataManagerZbk::instance().saveChatSessions();


    ui->messageEditor->clear();
}

void MainPageZbk::scrollToBottom()
{
    ui->lst_messages->scrollToBottom();
}


void MainPageZbk::createMenus()
{
    act_changeOwner=new QAction("转让群聊");
    act_removeMember=new QAction("移出群聊");
    act_setManager=new QAction("设置管理员");
    act_viewMemberInfo=new QAction("查看资料");


    act_viewUserInfo=new QAction("查看资料");
    act_deleteFriend=new QAction("删除好友");

    setItemMenu=new QMenu(this);

    setItemMenu->addAction(act_viewUserInfo);
    connect(act_viewUserInfo,&QAction::triggered,this,&MainPageZbk::showChattingFriendInfo);
    setItemMenu->addAction(act_deleteFriend);
    connect(act_deleteFriend,&QAction::triggered,this,&MainPageZbk::DeleteChattingFriend);

    m_changeTypeMenu=new QMenu(this);
    for(auto i:ServicesZbk::allServices)//每个服务都要有对应转换按钮
    {
        if(i->Id=="WeiBo")continue;//微博没有群
        QAction* act=new QAction(i->Id_cn);
        m_changeTypeMenu->addAction(act);
        connect(act,&QAction::triggered,this,[=](){
            if(i->Id==m_targetGroup->getServiceType())return;
            QString pretype=m_targetGroup->getServiceType();
            QString res;
            GroupsZbk* newGroup=UsersZbk::changeGroupType(m_targetGroup,i->Id,res);
            if(newGroup==NULL)
            {
                QMessageBox::warning(this,"变更失败",res);
                return;
            }
            ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
            m_MessageState=toNull;
            reloadFriend_GroupList(i->Id);
            reloadFriend_GroupList(pretype);
            m_targetGroup=NULL;
            DataManagerZbk::instance().saveAllData();
            // {

            //     ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
            //     reloadFriend_GroupList(i->Id);
            //     reloadFriend_GroupList()
            // }
        });
    }

    ui->btn_setg_ChangeType->setMenu(m_changeTypeMenu);
    m_addMemberMenu=new QMenu(this);
}
void MainPageZbk::showManageMenu(const QPoint &p,User_ServiceZbk* user)
{
    QMenu* Menu=new QMenu(this);

    Menu->addAction(act_viewMemberInfo);
    disconnect(act_viewMemberInfo,&QAction::triggered,0,0);
    connect(act_viewMemberInfo,&QAction::triggered,this,[=](){onshowUserInfoRequested(user);});
    QString myId=m_user->getData()[m_targetGroup->getServiceType()]->getId();
    QString targetId=user->getId();
    if(m_targetGroup->canManageMembers(myId))//登录的用户有管理成员的权限
    {
        if((myId==m_targetGroup->OwnerId()&&(!(myId==targetId)))||(!m_targetGroup->canManageMembers(targetId)))//两种情况：用户是群主且没选中自己；用户是群主或管理员且选中的不是群主或管理员
        {

            Menu->addSeparator();
            Menu->addAction(act_removeMember);
            act_removeMember->disconnect();
            connect(act_removeMember,&QAction::triggered,this,
                    [=]()
                    {
                        QString tmp_type=m_targetGroup->getServiceType();
                        if(m_targetGroup->getSize()<=2&&QMessageBox::question(this,"解散","此操作会导致当前群人数过少而解散。是否继续？")==QMessageBox::No)
                            return;

                        if(groupFactory::RemoveMember(m_targetGroup,user->getId()))
                        {
                            ui->lst_groupMember->setManageMembers(m_targetGroup->getMembers(),m_targetGroup->getServiceType(),e_ManageGroupMembers);
                            ui->lbl_setg_Size->setText("成员数 "+QString::number(m_targetGroup->getSize())+"人");
                        }
                        else
                        {
                            m_targetGroup=NULL;
                            m_MessageState=toNull;
                            ui->Message_stackedWidget->setCurrentWidget(ui->null_MessagePage);
                            reloadFriend_GroupList(tmp_type);
                        }
                        DataManagerZbk::instance().saveAllData();


                    });
        }
    }
    if(m_targetGroup->OwnerId()==m_user->getData()[m_targetGroup->getServiceType()]->getId())//登录的用户是选中群的群主
    {
        if(!(m_user->getData()[m_targetGroup->getServiceType()]->getId()==user->getId()))//并且群主选中的用户不是群主自己
        {
            if(m_targetGroup->hasAdministrators())//这里是群主设置管理员的action,微信群没有管理员的说法
            {
                Menu->addAction(act_setManager);
                disconnect(act_setManager,&QAction::triggered,0,0);
                if(m_targetGroup->canManageMembers(user->getId()))//已经是管理员，给出取消管理的方法
                {
                    qDebug()<<QString("用户id %1 是群 %2 的管理员").arg(user->getId(),m_targetGroup->getName());
                    act_setManager->setText("取消管理员");
                    connect(act_setManager,&QAction::triggered,this,[=](){m_targetGroup->removeManager(user->getId());});
                }
                else
                {
                    act_setManager->setText("设置管理员");
                    qDebug()<<QString("用户id %1 不是群 %2 的管理员").arg(user->getId(),m_targetGroup->getName());

                    connect(act_setManager,&QAction::triggered,this,[=](){m_targetGroup->addManager(user->getId());});
                }
                DataManagerZbk::instance().saveGroups();

            }
            //只有群主能转让群组
            Menu->addAction(act_changeOwner);
            disconnect(act_changeOwner,&QAction::triggered,0,0);
            connect(act_changeOwner,&QAction::triggered,this,[=](){m_targetGroup->setOwnerId(user->getId());showGroupMessagePage(m_targetGroup);});
            DataManagerZbk::instance().saveGroups();

        }
    }
    Menu->exec(p);

    // 菜单显示后自动删除
    connect(Menu, &QMenu::aboutToHide, Menu, &QMenu::deleteLater);
}

void MainPageZbk::on_let_SearchGroupMember_textChanged()
{

    QString inputInfo=ui->let_SearchGroupMember->text();
    QVector<QString>foundMember;
    for(const auto &str:m_targetGroup->getMembers())
    {
        User_ServiceZbk* user=UsersZbk::FindUser_IdService(str,m_targetGroup->getServiceType());
        if(user==NULL)continue;
        if(user->getId().contains(inputInfo)||user->getName().contains(inputInfo))
        {
            foundMember.push_back(user->getId());
        }
    }
    ui->lst_groupMember->setManageMembers(foundMember,m_targetGroup->getServiceType(),e_ManageGroupMembers);

}


void MainPageZbk::on_let_setg_Name_editingFinished()
{
    if(m_targetGroup->getName()!=ui->let_setg_Name->text())
    {
        m_targetGroup->setName(ui->let_setg_Name->text());
        // if(m_targetGroup->getServiceType()=="QQ")qqpage->reloadFriends();
        // else if(m_targetGroup->getServiceType()=="WeChat")wxpage->reloadFriends();
        reloadFriend_GroupList(m_targetGroup->getServiceType());
        QMessageBox::information(this,"提示","成功修改群名称为 "+m_targetGroup->getName());
        DataManagerZbk::instance().saveGroups();

    }

}

void MainPageZbk::on_btn_logout_clicked()
{
    if(QMessageBox::question(this,"退出登录","是否退出?")==QMessageBox::No)
        return;
    for(auto it=m_user->getIsusing().begin();it!=m_user->getIsusing().end();it++)
    {
        if(it.value())//该服务已启用
        {
            m_user->getData()[it.key()]->setIsOnline(false);
        }
    }

    this->close();
    emit logout();
}


void MainPageZbk::reloadFriend_GroupList(const QString &type)
{
    if(!ServicesZbk::checkService(type))return;
    if(!m_user->getIsusing()[type])
    {
        qDebug()<<QString("void MainPageZbk::reloadFriend_Group(const QString &type):用户%1未启用%2服务").arg(m_user->getId(),type);
        return;
    }
    if(type=="QQ"&&qqpage)qqpage->reloadFriends();
    else if(type=="WeChat"&&wxpage)wxpage->reloadFriends();
    else if(type=="WeiBo")wbpage->reloadFriends();

}

void MainPageZbk::reloadSystemNotice()
{
    ui->lst_systemNotices->setSystemNotices(m_user);
    ui->btn_topSystemNotice->setIcon(QIcon(":/pic/systemNotices.png"));
}

void MainPageZbk::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Return||e->key()==Qt::Key_Enter)
    {
        if(m_MessageState!=toNull)
        {
            if(!ui->messageEditor->toPlainText().trimmed().isEmpty())
            {
                sendMessage();
                e->accept();
            }
        }
    }
    QWidget::keyPressEvent(e);
}

void MainPageZbk::loadMessages(ChatSessionZbk *session)
{
    if(session)
    {
        qDebug()<<"void MainPageZbk::loadMessages(ChatSessionZbk *session):"<<QString::number(session->getMessages().size());

        m_model->setSession(session,m_user->getData()[m_type]->getId());
        QTimer::singleShot(100,this,&MainPageZbk::scrollToBottom);
    }
}

void MainPageZbk::loadAddMemberMenu()
{

    if(m_targetGroup==NULL)return;
    m_addMemberMenu->clear();
    for(auto i:m_user->getData()[m_targetGroup->getServiceType()]->m_friends)
    {
        User_ServiceZbk* user=UsersZbk::FindUser_IdService(i,m_targetGroup->getServiceType());
        if(user==NULL)continue;
        QAction* act=new QAction(QString("%1(%2)").arg(user->getName(),user->getId()));
        if(m_targetGroup->getMembers().contains(user->getId()))
        {
            act->setEnabled(false);
        }
        connect(act,&QAction::triggered,this,[=]()
            {
                // m_targetGroup->addMember(user->getId());
                groupFactory::AddMember(m_targetGroup,user->getId());
                DataManagerZbk::instance().saveAllData();
                loadAddMemberMenu();
                ui->lst_groupMember->setManageMembers(m_targetGroup->getMembers(),m_targetGroup->getServiceType(),e_ManageGroupMembers);
                ui->lbl_setg_Size->setText("成员数 "+QString::number(m_targetGroup->getSize())+"人");
            });
        m_addMemberMenu->addAction(act);
    }
    ui->btn_setg_addGroupMember->setMenu(m_addMemberMenu);

}


void MainPageZbk::on_btn_returnGroupPage_clicked()
{
    ui->Message_stackedWidget->setCurrentWidget(ui->common_MessagePage);
}


void MainPageZbk::on_btn_createSubGroup_clicked()
{
    createGroupZbk* createGroupPage=new createGroupZbk(m_user,m_targetGroup->getServiceType(),m_targetGroup->getMembers());
    createGroupPage->setAttribute(Qt::WA_DeleteOnClose);
    connect(createGroupPage,&createGroupZbk::groupCreated,this,[=](){reloadFriend_GroupList(m_targetGroup->getServiceType());DataManagerZbk::instance().saveAllData();});
    createGroupPage->exec();

}


void MainPageZbk::on_btn_topNick_clicked()
{
    QMenu* menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    for(auto it=m_user->getIsusing().constBegin();it!=m_user->getIsusing().constEnd();it++)
    {
        if(!it.value())continue;
        QAction* act=new QAction(ServicesZbk::toZhcn[it.key()]+"资料");
        connect(act,&QAction::triggered,[it,this](){onshowUserInfoRequested(m_user->getData()[it.key()]);});
        menu->addAction(act);
    }

    QAction* act=new QAction("UniTencent资料");
    connect(act,&QAction::triggered,[this](){onshowUserInfoRequested(m_user);});
    menu->addAction(act);

    ui->btn_topNick->setMenu(menu);
    ui->btn_topNick->showMenu();

}


void MainPageZbk::on_btn_topSystemNotice_clicked()
{
    ui->lst_systemNotices->setSystemNotices(m_user);
    ui->Message_stackedWidget->setCurrentWidget(ui->MessagePage_System);
    m_MessageState=toSystem;
    m_targetFriend=NULL;
    m_targetGroup=NULL;
}


// 处理系统通知的右键菜单
void MainPageZbk::onSystemNoticeRightClicked(const QPoint& globalPos, const SystemNoticeItem& notice)
{
    QMenu menu;

    if (!notice.senderId.isEmpty() && !notice.isProcessed)
    {
        // 好友申请类型的通知
        QAction* acceptAction = menu.addAction("同意申请");
        QAction* rejectAction = menu.addAction("拒绝申请");

        QAction* selectedAction = menu.exec(globalPos);

        if (selectedAction == acceptAction) {
            // 同意好友申请
            UsersZbk* currentUser = m_user;
            if (currentUser) {
                currentUser->acceptFriendRequest(notice.senderId, notice.serviceType, notice.noticeId);
                // 刷新系统通知列表
                reloadSystemNotice();
                // 显示操作结果
                QMessageBox::information(this, "操作成功", "已同意好友申请");
                DataManagerZbk::instance().saveChatSessions();

            }
        }
        else if (selectedAction == rejectAction) {
            // 拒绝好友申请
            UsersZbk* currentUser =m_user;
            if (currentUser) {
                currentUser->rejectFriendRequest(notice.senderId, notice.serviceType, notice.noticeId);
                // 刷新系统通知列表
                reloadSystemNotice();
                // 显示操作结果
                QMessageBox::information(this, "操作成功", "已拒绝好友申请");
                DataManagerZbk::instance().saveChatSessions();

            }
        }

    }
    else
    {
        qDebug()<<"other notice rightclicked";
        QAction* deleteAction=menu.addAction("删除通知");
        QAction* selectedAction = menu.exec(globalPos);
        if (selectedAction == deleteAction)
        {
            m_user->removeSystemMessage(notice.noticeId,notice.serviceType);
            reloadSystemNotice();
            DataManagerZbk::instance().saveChatSessions();
        }
    }
}



