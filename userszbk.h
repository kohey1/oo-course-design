#ifndef USERSZBK_H
#define USERSZBK_H
#include <QDate>
#include <QString>
#include <QList>
#include <QMap>
#include <QDebug>


#include "groupfactory.h"
#include <QJsonObject>
#include <QRandomGenerator>
enum Services
{
    QQ,
    WeChat,
    WeiBo
};

class UsersZbk:public QObject
{

    Q_OBJECT
public:
    //UsersZbk();
    UsersZbk(QString name,QString pass);
    UsersZbk(QString name,QString pass,QString id,QString path);

    QString getId();
    QString getPassword();
    QString getLocation();
    QString getNickname();
    QDate getBirthday();
    QDate getRegisterDate();
    QString getavatarPath();
    QMap<QString, bool> &getIsusing();
    QMap<QString,User_ServiceZbk*>& getData();
    static int lastId;



    void setUserServiceId(const QString& id,const QString& type);

    void setIsusing(const QMap<QString, bool>& isusing);
    void setavaterPath(QString path);
    void setId(QString str);




    void addService_fast(QString type);
    void removeService(QString type);


    void addGroup(const QString& groupId,const QString& serviceType);
    void addFriend(const QString& friendId,const QString& serviceType);
    void removeFriend(const QString& friendId,const QString& serviceType);

    static QVector<UsersZbk*>allUsers;
    static User_ServiceZbk* FindUser_IdService(QString id,QString ser);//id+服务查找用户
    static UsersZbk* Find_UniTencentUser(QString id);//id查找UniTencent用户
    static GroupsZbk* changeGroupType(GroupsZbk* group,const QString& targetType,QString& state);//直接转换，没有进行任何检查


    // 聊天记录相关方法
    void addPrivateMessage(const QString& friendServiceId, const QString& serviceType, const QString& content);
    void addGroupMessage(const QString& groupId, const QString& serviceType, const QString& content);

    QVector<ChatSessionZbk*> getUserSessions(const QString& serviceType) const;
    QVector<ChatMessageZbk> getPrivateChatHistory(const QString& friendServiceId, const QString& serviceType) const;
    QVector<ChatMessageZbk> getGroupChatHistory(const QString& groupId, const QString& serviceType) const;
    // 好友申请相关方法
    void sendFriendRequest(const QString& targetUserId, const QString& serviceType,
                           const QString& verifyMessage = "");
    // 修改：添加消息ID参数用于删除消息
    void acceptFriendRequest(const QString& applicantId, const QString& serviceType,
                             const QString& requestMessageId);
    void rejectFriendRequest(const QString& applicantId, const QString& serviceType,
                             const QString& requestMessageId);

    // 新增：删除系统消息
    bool removeSystemMessage(const QString& messageId, const QString& serviceType);

    // 获取系统通知（好友申请）
    QVector<ChatMessageZbk> getSystemNotifications(const QString& serviceType) const;
    QVector<ChatSessionZbk*> getSystemSessions(const QString& serviceType) const;
    // 新增：检查是否已存在未处理的好友申请
    bool hasPendingFriendRequest(const QString& targetUserId, const QString& serviceType);

    // 新增：更新已存在的好友申请
    bool updateFriendRequest(const QString& targetUserId, const QString& serviceType,
                             const QString& newVerifyMessage);



    Gender gender() const;
    void setGender(Gender newGender);

    void setLocation(const QString &newLocation);

    void setNickname(const QString &newNickname);

    void setBirthday(const QDate &newBirthday);

    void setRegisterDate(const QDate &newRegisterDate);

private:

    QString generateId();

    QString m_id;
    QString m_password;
    QString m_nickname;
    QString m_location;
    QDate m_birthday;
    Gender m_gender;
    QDate m_registerDate;
    QString m_avatarPath;//头像路径


    QMap<QString,bool>m_Isusing;//是否开通

    QMap<QString,User_ServiceZbk*>serviceData;
signals:
    void refreshPage(const QString& type);



};

#endif // USERSZBK_H
