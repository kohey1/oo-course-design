// friendlistwidget.h
#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>
#include <QListWidget>

#include "userszbk.h"
#include "frienditemwidget.h"
#include "searchingitemszbk.h"
#include <QMessageBox>
#include <QMenu>

enum ItemKinds
{
    e_Friends,
    e_Groups,
    e_SearchingFriends,
    e_SearchingGroups,
    e_createGroups,
    e_ManageGroupMembers,
    e_SystemNotices  // 新增：系统通知类型

};

// 系统通知项数据结构
struct SystemNoticeItem {
    QString serviceType;
    QString noticeId;
    QString content;
    QDateTime timestamp;
    QString senderId;  // 申请人的ID
    QString senderName; // 申请人的昵称
    QString requestId;  // 申请的唯一ID
    bool isProcessed;   // 是否已处理
};


class FriendListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FriendListWidget(QWidget *parent = nullptr);
    void setFriends(const QVector<QString> &friends,const QString& type);//涉及对话加载，使用前要设置setUserId()
    bool setGroups(const QVector<QString> &groups,const QString& type);
    void setSearchFriends(const QMap<QString, QString> &friends, const QString& type);
    void setSearchFriends(const QVector<QString> &friends, const QString& type);
    void setMutiMode();
    void setItemKinds(ItemKinds kind);
    void setManageMembers(const QVector<QString> &friends,const QString& type,const ItemKinds kind);
    void setSystemNotices(UsersZbk* user);



    User_ServiceZbk* getUserOnPos(const QPoint& q);
    SystemNoticeItem getSystemNoticeOnPos(const QPoint& pos); // 新增：获取位置对应的系统通知


    QMap<QListWidgetItem*, User_ServiceZbk*> getitemUserMap()const;
    QMap<QListWidgetItem*,GroupsZbk*> getitemGroupMap()const;
    int getSize()const;



    QString userId() const;
    void setUserId(const QString &newUserId);

signals:
    void friendClicked(User_ServiceZbk *user);
    void SearchfriendClicked(User_ServiceZbk *user);
    void groupClicked(GroupsZbk* group);
    void updateSelection(QVector<User_ServiceZbk*>selectedUser);


    //右键项

    void GroupMemberRightClicked(const QPoint& p,User_ServiceZbk* user);
    void SystemNoticeRightClicked(const QPoint& p, const SystemNoticeItem& notice); // 新增：系统通知右键

private slots:
    void onItemClicked(QListWidgetItem *item);

    void onItemDoubleClicked(QListWidgetItem *item);
    void selectionChanged();

    //右键项
    void signalSender(const QPoint& p);




private:
    QListWidget *m_listWidget;
    QMap<QListWidgetItem*, User_ServiceZbk*> m_itemUserMap; // 项与用户的映射
    QMap<QListWidgetItem*,GroupsZbk*>m_itemGroupMap;
    QMap<QListWidgetItem*, SystemNoticeItem> m_itemSystemNoticeMap; // 项与系统通知的映射

    ItemKinds opera_type;
    QString m_userId;

    QString formatLastMessageTime(const QDateTime& dateTime);
    // 解析系统通知内容
    SystemNoticeItem parseSystemNotice(const ChatMessageZbk& message, const QString& serviceType);
    // 创建系统通知项的显示文本
    QString createSystemNoticeText(const SystemNoticeItem& notice);

    bool m_systemnotice_isNull=true;


};

#endif // FRIENDLISTWIDGET_H
