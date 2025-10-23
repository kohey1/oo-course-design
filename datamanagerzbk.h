#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "userszbk.h"



class DataManagerZbk : public QObject
{
    Q_OBJECT

public:
    static DataManagerZbk& instance();
    
    bool saveAllData();
    bool loadAllData();
    
    // 单独保存/加载各个模块
    bool saveUsers();
    bool saveGroups();
    bool saveChatSessions();
    bool saveLastId();



    bool loadUsers();
    bool loadGroups();
    bool loadChatSessions();
    bool loadLastId();

    void initializeSystemUsers();
    void ensureSystemSessions();
    void removeDuplicateSessions();
private:
    DataManagerZbk(QObject *parent = nullptr);
    ~DataManagerZbk() = default;
    
    QString getDataDir() const;
    QString getFilePath(const QString& filename) const;
    
    // 序列化方法
    QJsonObject userToJson(UsersZbk* user);
    QJsonObject userServiceToJson(User_ServiceZbk* userService);
    QJsonObject groupToJson(GroupsZbk* group);
    QJsonObject chatSessionToJson(ChatSessionZbk* session);
    QJsonObject chatMessageToJson(const ChatMessageZbk& message);
    
    // 反序列化方法
    UsersZbk* userFromJson(const QJsonObject& json);
    User_ServiceZbk* userServiceFromJson(const QJsonObject& json, UsersZbk* mainUser);
    GroupsZbk* groupFromJson(const QJsonObject& json);
    ChatSessionZbk* chatSessionFromJson(const QJsonObject& json);
    ChatMessageZbk chatMessageFromJson(const QJsonObject& json);
    
    // 数据恢复后的关联重建
    void rebuildUserAssociations();
    void rebuildGroupAssociations();
    void updateSessionFromJson(ChatSessionZbk* session, const QJsonObject& json);
    ChatSessionZbk* createSessionFromJson(const QJsonObject& json);
};
