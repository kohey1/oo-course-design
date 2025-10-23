#ifndef MAINPAGEZBK_H
#define MAINPAGEZBK_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QTimer>
#include "servicelistpagezbk.h"
#include "qqpagezbk.h"
#include "wechatpagezbk.h"
#include "weibopagezbk.h"
#include "userinfopagezbk.h"
#include "chatmessagedelegatezbk.h"

#include "modifyloginuserinfopagezbk.h"
#include <QButtonGroup>
#include "datamanagerzbk.h"


namespace Ui {
class MainPageZbk;
}
enum MessageState
{
    toNull,
    toFriend,
    toGroup,
    toSystem
};

class MainPageZbk : public QWidget
{
    Q_OBJECT

public:
    explicit MainPageZbk(UsersZbk* user,QWidget *parent = nullptr);
    ~MainPageZbk();
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
private slots:
    void on_btnCloseWin_clicked();

    void on_btnHideWin_clicked();

    //void on_MainPageZbk_customContextMenuRequested(const QPoint &pos);


    void onServiceButtonClicked();

    //聊天界面相关
    void showMessagePage(User_ServiceZbk* user);
    void showGroupMessagePage(GroupsZbk* group);
    void on_btn_setItem_clicked();

    void DeleteChattingFriend();
    void onDeleteFriendRequested(User_ServiceZbk* user=NULL);
    void showChattingFriendInfo();
    void onshowUserInfoRequested(User_ServiceZbk* user);
    void onshowUserInfoRequested(UsersZbk* user);




    void sendMessage();
    void scrollToBottom();

    //群管理相关
    void showManageMenu(const QPoint& p,User_ServiceZbk* user);//右键群成员列表项弹出的菜单


    //void on_btn_returnInfoPage_clicked();

    void on_let_SearchGroupMember_textChanged();
    void on_let_setg_Name_editingFinished();
    void on_btn_logout_clicked();    
    void on_btn_returnGroupPage_clicked();

    void on_btn_createSubGroup_clicked();

    void on_btn_topNick_clicked();//查看或修改用户自己的信息，点击用户名以打开界面

    void on_btn_topSystemNotice_clicked();

    void onSystemNoticeRightClicked(const QPoint &globalPos, const SystemNoticeItem &notice);
private:
    Ui::MainPageZbk *ui;

    QString m_type;


    bool m_topIsPressing;
    QPointF m_startPos;
    QPointF m_framePos;
    ServiceListPageZbk* serpage=NULL;
    UsersZbk* m_user;
    QQPageZbk* qqpage=NULL;
    WeChatPageZbk* wxpage=NULL;
    WeiBoPageZbk* wbpage=NULL;
    QButtonGroup* sideButtons;
    QMap<QString,QPushButton*>m_serviceButtons;
    QVBoxLayout* m_sideBarLayout;
    QMap<QString,bool>m_btnIschecked;
    bool nochecked=true;
    bool isFirst=true;
    void reloadFriend_GroupList(const QString& type);
    void reloadSystemNotice();

    //聊天界面相关
    MessageState m_MessageState=toNull;
    User_ServiceZbk* m_targetFriend=NULL;
    GroupsZbk* m_targetGroup=NULL;
    UserInfoPageZbk* m_userInfoPage=NULL;
    QMenu* setItemMenu;                         //好友聊天界面右上角的三个点对应的菜单
    QAction* act_viewUserInfo;
    QAction* act_deleteFriend;

    ChatSessionZbk* m_session;
    ChatMessageModelZbk* m_model;
    ChatMessageDelegateZbk* m_delegate;


    void keyPressEvent(QKeyEvent*e)override;
    void setUserInfoPage(User_ServiceZbk* user);
    void loadMessages(ChatSessionZbk* session);

    //群组管理
    QMenu* m_groupMemberManageMenu;
    QAction* act_setManager;
    QAction* act_removeMember;
    QAction* act_changeOwner;
    QAction* act_viewMemberInfo;

    QMenu* m_changeTypeMenu;
    QMenu* m_addMemberMenu;
    void loadAddMemberMenu();




    void createMenus();
    void clearSerivceButtons();
    void clearServicePages();
    void updateServiceButtons();
    void on_addFriendResquseted(User_ServiceZbk* user);
    void on_chatToFriendsRequested(User_ServiceZbk* user);


signals:
    void logout();
    void service_changed();





};

#endif // MAINPAGEZBK_H
