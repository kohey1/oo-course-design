#include "datamanagerzbk.h"
#include <QDebug>
#include <QStandardPaths>

DataManagerZbk& DataManagerZbk::instance()
{
    static DataManagerZbk instance;
    return instance;
}

DataManagerZbk::DataManagerZbk(QObject *parent) : QObject(parent) {}

QString DataManagerZbk::getDataDir() const
{
    // 存储在可执行文件同级目录的 data 文件夹中
    QString dataDir = QCoreApplication::applicationDirPath() + "/data";


    QDir dir(dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");  // 创建数据目录
        qDebug() << "创建数据目录:" << dataDir;
    }

    return dataDir;
}

QString DataManagerZbk::getFilePath(const QString& filename) const
{
    return getDataDir() + "/" + filename;
}

bool DataManagerZbk::saveAllData()
{
    bool success = true;
    success &= saveUsers();
    success &= saveGroups();
    success &= saveChatSessions();
    success&=saveLastId();
    
    if (success) {
        qDebug() << "所有数据保存成功";
    } else {
        qDebug() << "数据保存失败";
    }
    
    return success;
}

bool DataManagerZbk::loadAllData()
{
    bool success = true;
    success &= loadUsers();
    success &= loadGroups();
    success &= loadChatSessions();
    success&=loadLastId();
    
    if (success) {
        rebuildUserAssociations();
        rebuildGroupAssociations();
        removeDuplicateSessions();  // 新增：清理重复会话

        ensureSystemSessions();
        qDebug() << "所有数据加载成功";
    } else {
        qDebug() << "数据加载失败";
    }
    
    return success;
}

// 用户数据持久化
// bool DataManagerZbk::saveUsers()
// {
//     QJsonArray usersArray;
    
//     for (UsersZbk* user : UsersZbk::allUsers)
//     {
//         usersArray.append(userToJson(user));
//     }
    
//     QJsonObject root;
//     root["users"] = usersArray;
//     root["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
//     QFile file(getFilePath("users.json"));
//     if (!file.open(QIODevice::WriteOnly)) {
//         qDebug() << "无法打开用户数据文件:" << file.errorString();
//         return false;
//     }
    
//     file.write(QJsonDocument(root).toJson());
//     file.close();
//     return true;
// }


bool DataManagerZbk::saveUsers()
{
    QString filePath = getFilePath("users.json");
    QFile::remove(filePath);

    QJsonArray usersArray;

    for (UsersZbk* user : UsersZbk::allUsers) {
        usersArray.append(userToJson(user));
    }

    QJsonObject root;
    root["users"] = usersArray;
    root["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // 创建新文件并写入
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开用户数据文件:" << file.errorString();
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();

    qDebug() << "用户数据已保存（覆盖模式）";
    return true;
}

QJsonObject DataManagerZbk::userToJson(UsersZbk* user)
{
    QJsonObject userObj;
    userObj["id"] = user->getId();
    userObj["nickname"] = user->getNickname();
    userObj["password"] = user->getPassword();
    userObj["location"] = user->getLocation();
    userObj["avatarPath"] = user->getavatarPath();
    userObj["birthday"] = user->getBirthday().toString(Qt::ISODate);
    userObj["registerDate"] = user->getRegisterDate().toString(Qt::ISODate);
    userObj["gender"] = static_cast<int>(user->gender());
    
    // 服务数据
    QJsonObject servicesObj;
    QJsonObject isUsingObj;
    
    for (const auto& service : user->getData()) {
        if (service) {
            servicesObj[service->getType()] = userServiceToJson(service);
            isUsingObj[service->getType()] = true;
        }
    }
    
    // 添加未启用的服务
    for (const auto& serviceType : user->getIsusing().keys()) {
        if (!isUsingObj.contains(serviceType)) {
            isUsingObj[serviceType] = false;
        }
    }
    
    userObj["services"] = servicesObj;
    userObj["isUsing"] = isUsingObj;
    
    return userObj;
}

QJsonObject DataManagerZbk::userServiceToJson(User_ServiceZbk* userService)
{
    QJsonObject serviceObj;
    serviceObj["id"] = userService->getId();
    serviceObj["nickName"] = userService->getName();
    serviceObj["password"] = userService->Pass();
    serviceObj["avatarPath"] = userService->getAvatar();
    serviceObj["isOnline"] = userService->getOnline();
    serviceObj["location"] = userService->Location();
    serviceObj["age"] = userService->age();
    serviceObj["gender"] = static_cast<int>(userService->gender());
    serviceObj["registerDate"] = userService->registerDate().toString(Qt::ISODate);
    serviceObj["birthday"]=userService->birthday().toString(Qt::ISODate);
    
    // 好友列表
    QJsonArray friendsArray;
    for (const QString& friendId : userService->m_friends) {
        friendsArray.append(friendId);
    }
    serviceObj["friends"] = friendsArray;
    
    // 群组列表
    QJsonArray groupsArray;
    for (const QString& groupId : userService->m_groups) {
        groupsArray.append(groupId);
    }
    serviceObj["groups"] = groupsArray;
    
    // 注意：移除了 wbFollowing 和 wbFans 字段
    
    return serviceObj;
}



UsersZbk* DataManagerZbk::userFromJson(const QJsonObject& json)
{
    QString id = json["id"].toString();
    QString nickname = json["nickname"].toString();
    QString password = json["password"].toString();
    QString avatarPath = json["avatarPath"].toString();
    
    UsersZbk* user = new UsersZbk(nickname, password, id, avatarPath);
    
    // 设置基本信息
    user->setLocation(json["location"].toString());
    user->setavaterPath(avatarPath);
    user->setNickname(nickname);
    user->setGender(static_cast<Gender>(json["gender"].toInt()));
    
    // 设置日期
    QDate birthday = QDate::fromString(json["birthday"].toString(), Qt::ISODate);
    QDate registerDate = QDate::fromString(json["registerDate"].toString(), Qt::ISODate);
    user->setBirthday(birthday);
    user->setRegisterDate(registerDate);
    // 注意：registerDate 在构造函数中已设置，这里可以跳过
    
    // 设置服务数据
    QJsonObject servicesObj = json["services"].toObject();
    QJsonObject isUsingObj = json["isUsing"].toObject();
    
    for (const QString& serviceType : servicesObj.keys()) {
        QJsonObject serviceJson = servicesObj[serviceType].toObject();
        User_ServiceZbk* userService = userServiceFromJson(serviceJson, user);
        if (userService) {
            user->getData()[serviceType] = userService;
        }
    }
    
    // 设置服务启用状态
    for (const QString& serviceType : isUsingObj.keys()) {
        user->getIsusing()[serviceType] = isUsingObj[serviceType].toBool();
    }
    
    return user;
}

User_ServiceZbk* DataManagerZbk::userServiceFromJson(const QJsonObject& json, UsersZbk* mainUser)
{
    return nullptr;
}


void DataManagerZbk::initializeSystemUsers()
{
    // 直接调用静态方法
    User_ServiceZbk::initializeSystemUser();
}


// 需要修改调用 userServiceFromJson 的地方
bool DataManagerZbk::loadUsers()
{
    QFile file(getFilePath("users.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "用户数据文件不存在，将创建新数据";
        initializeSystemUsers();
        return true;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        qDebug() << "用户数据文件格式错误";
        return false;
    }
    
    QJsonObject root = doc.object();
    QJsonArray usersArray = root["users"].toArray();
    
    for (const QJsonValue& userValue : usersArray) {
        QJsonObject userJson = userValue.toObject();
        QString id = userJson["id"].toString();
        QString nickname = userJson["nickname"].toString();
        QString password = userJson["password"].toString();
        QString avatarPath = userJson["avatarPath"].toString();
        
        UsersZbk* user = new UsersZbk(nickname, password, id, avatarPath);
        
        // 设置基本信息
        user->setLocation(userJson["location"].toString());
        user->setavaterPath(avatarPath);
        user->setNickname(nickname);
        user->setGender(static_cast<Gender>(userJson["gender"].toInt()));
        
        // 设置日期
        QDate birthday = QDate::fromString(userJson["birthday"].toString(), Qt::ISODate);
        user->setBirthday(birthday);
        user->setRegisterDate(QDate::fromString(userJson["registerDate"].toString(),Qt::ISODate));

        // 设置服务数据
        QJsonObject servicesObj = userJson["services"].toObject();
        QJsonObject isUsingObj = userJson["isUsing"].toObject();
        
        for (const QString& serviceType : servicesObj.keys()) {
            QJsonObject serviceJson = servicesObj[serviceType].toObject();
            
            // 直接创建 User_ServiceZbk
            QString serviceNickname = serviceJson["nickName"].toString();
            QString servicePassword = serviceJson["password"].toString();
            
            User_ServiceZbk* userService = new User_ServiceZbk(serviceType, serviceNickname, servicePassword, id);
            
            // 设置服务特定属性
            userService->setId(serviceJson["id"].toString());
            userService->setAvator(serviceJson["avatarPath"].toString());
            userService->setIsOnline(serviceJson["isOnline"].toBool());
            userService->setLocation(serviceJson["location"].toString());
            userService->setAge(serviceJson["age"].toInt());
            userService->setGender(static_cast<Gender>(serviceJson["gender"].toInt()));
            userService->setRegisterDate(QDate::fromString(serviceJson["registerDate"].toString(), Qt::ISODate));
            userService->setBirthday(QDate::fromString(serviceJson["birthday"].toString(),Qt::ISODate));
            // 设置好友列表
            QJsonArray friendsArray = serviceJson["friends"].toArray();
            for (const QJsonValue& friendValue : friendsArray) {
                userService->m_friends.append(friendValue.toString());
            }
            
            // 设置群组列表
            QJsonArray groupsArray = serviceJson["groups"].toArray();
            for (const QJsonValue& groupValue : groupsArray) {
                userService->m_groups.append(groupValue.toString());
            }
            
            user->getData()[serviceType] = userService;
        }
        
        // 设置服务启用状态
        for (const QString& serviceType : isUsingObj.keys()) {
            user->getIsusing()[serviceType] = isUsingObj[serviceType].toBool();
        }
        
        // UsersZbk::allUsers.append(user);
    }
    initializeSystemUsers();
    
    return true;
}


bool DataManagerZbk::saveGroups()
{
    QString filePath = getFilePath("groups.json");

    // 先删除原文件
    QFile::remove(filePath);

    QJsonObject root;
    root["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonObject groupsByService;

    for (auto it = groupFactory::allGroups.constBegin(); it != groupFactory::allGroups.constEnd(); ++it) {
        QString serviceType = it.key();
        QVector<GroupsZbk*> groups = it.value();
        QJsonArray groupsArray;

        for (GroupsZbk* group : groups) {
            qDebug()<<QString("正在保存群 %1, %2 ,%3").arg(group->getId(),group->getName(),group->getServiceType());
            groupsArray.append(groupToJson(group));
        }

        groupsByService[serviceType] = groupsArray;
    }

    root["groupsByService"] = groupsByService;

    // 创建新文件并写入
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开群组数据文件:" << file.errorString();
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();

    qDebug() << "群组数据已保存（覆盖模式）";
    return true;
}

QJsonObject DataManagerZbk::groupToJson(GroupsZbk* group)
{
    QJsonObject groupObj;
    groupObj["id"] = group->getId();
    groupObj["name"] = group->getName();
    groupObj["serviceType"] = group->getServiceType();
    groupObj["avatarPath"] = group->getAvatar();
    groupObj["ownerId"] = group->OwnerId();
    groupObj["createDate"] = group->createDate().toString(Qt::ISODate);
    
    // 成员列表
    QJsonArray membersArray;
    for (const QString& memberId : group->getMembers()) {
        membersArray.append(memberId);
    }
    groupObj["members"] = membersArray;
    
    // 管理员列表
    QJsonArray managersArray;
    for (const QString& managerId : group->Managers()) {
        managersArray.append(managerId);
    }
    groupObj["managers"] = managersArray;
    
    // 群组类型特定属性
    groupObj["hasAdministrators"] = group->hasAdministrators();
    groupObj["allowSubGroups"] = group->allowSubGroups();
    
    return groupObj;
}



bool DataManagerZbk::loadGroups()
{
    QFile file(getFilePath("groups.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "群组数据文件不存在，将创建新数据";
        return true;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull()) {
        qDebug() << "群组数据文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    QJsonObject groupsByService = root["groupsByService"].toObject();

    for (auto it = groupsByService.begin(); it != groupsByService.end(); ++it) {
        QString serviceType = it.key();
        QJsonArray groupsArray = it.value().toArray();

        for (const QJsonValue& groupValue : groupsArray) {
            GroupsZbk* group = groupFromJson(groupValue.toObject());
            // 如果 groupFactory::createGroup 已经自动添加到 allGroups，
            // 那么这里就不需要再次添加
            // 只需要确保 group 不为空即可
            Q_UNUSED(group);
        }
    }

    return true;
}

GroupsZbk* DataManagerZbk::groupFromJson(const QJsonObject& json)
{
    QString id = json["id"].toString();
    QString name = json["name"].toString();
    QString serviceType = json["serviceType"].toString();
    QString ownerId = json["ownerId"].toString();
    QString avatarPath = json["avatarPath"].toString();
    
    // 创建群组
    GroupsZbk* group = groupFactory::createGroup(serviceType, id, ownerId, name, avatarPath);
    
    if (!group) {
        qDebug() << "创建群组失败:" << serviceType << id;
        return nullptr;
    }
    
    // 设置创建日期
    group->setCreateDate(QDate::fromString(json["createDate"].toString(), Qt::ISODate));
    
    // 设置成员
    QJsonArray membersArray = json["members"].toArray();
    QVector<QString> members;
    for (const QJsonValue& memberValue : membersArray) {
        members.append(memberValue.toString());
    }
    group->setMembers(members);
    
    // 设置管理员
    QJsonArray managersArray = json["managers"].toArray();
    QVector<QString> managers;
    for (const QJsonValue& managerValue : managersArray) {
        managers.append(managerValue.toString());
    }
    group->setManagers(managers);
    
    return group;
}



bool DataManagerZbk::saveChatSessions()
{
    QString filePath = getFilePath("chatsessions.json");
    QFile::remove(filePath);

    QJsonObject root;
    root["saveTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    QJsonObject sessionsByService;

    // 使用 QSet 来记录已经保存的会话，避免重复
    QSet<QString> savedSessions;

    for (auto it = ChatSessionZbk::allSessions.constBegin(); it != ChatSessionZbk::allSessions.constEnd(); ++it) {
        QString serviceType = it.key();
        QVector<ChatSessionZbk*> sessions = it.value();
        QJsonArray sessionsArray;

        for (ChatSessionZbk* session : sessions) {
            QString sessionKey = serviceType + "_" + session->getSessionId();

            // 检查是否已经保存过这个会话
            if (!savedSessions.contains(sessionKey)) {
                sessionsArray.append(chatSessionToJson(session));
                savedSessions.insert(sessionKey);
                qDebug() << "保存会话:" << session->getSessionId()
                         << "服务:" << serviceType
                         << "消息数量:" << session->getMessages().size();
            } else {
                qDebug() << "跳过重复会话:" << session->getSessionId() << "服务:" << serviceType;
            }
        }

        sessionsByService[serviceType] = sessionsArray;
    }

    root["sessionsByService"] = sessionsByService;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开聊天会话数据文件:" << file.errorString();
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();

    qDebug() << "聊天会话数据已保存，去重后会话数量:" << savedSessions.size();
    return true;
}

bool DataManagerZbk::saveLastId()
{
    QJsonObject root;
    root["saveTime"]=QDateTime::currentDateTime().toString(Qt::ISODate);
    root["userLast"]=UsersZbk::lastId;
    root["groupLast"]=groupFactory::lastId;
    QFile file(getFilePath("lastids.json"));
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "无法打开聊天会话数据文件:" << file.errorString();
        return false;
    }

    file.write(QJsonDocument(root).toJson());
    file.close();
    return true;


}

QJsonObject DataManagerZbk::chatSessionToJson(ChatSessionZbk* session)
{
    QJsonObject sessionObj;
    sessionObj["sessionId"] = session->getSessionId();
    sessionObj["sessionType"] = static_cast<int>(session->getSessionType());
    sessionObj["serviceType"] = session->getServiceType();

    // 参与者列表
    QJsonArray participantsArray;
    for (const QString& participant : session->getParticipants()) {
        participantsArray.append(participant);
    }
    sessionObj["participants"] = participantsArray;

    // 消息列表
    QJsonArray messagesArray;
    for (const ChatMessageZbk& message : session->getMessages()) {
        messagesArray.append(chatMessageToJson(message));
    }
    sessionObj["messages"] = messagesArray;

    return sessionObj;
}

QJsonObject DataManagerZbk::chatMessageToJson(const ChatMessageZbk& message)
{
    QJsonObject messageObj;
    messageObj["messageId"] = message.getMessageId();
    messageObj["senderId"] = message.getSenderId();
    messageObj["content"] = message.getContent();
    messageObj["messageType"] = static_cast<int>(message.getMessageType());
    messageObj["timestamp"] = message.getTimestamp().toString(Qt::ISODate);
    messageObj["isRead"] = message.getIsRead();
    
    return messageObj;
}


bool DataManagerZbk::loadChatSessions()
{
    QFile file(getFilePath("chatsessions.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "聊天会话数据文件不存在，将创建新数据";
        return true;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull()) {
        qDebug() << "聊天会话数据文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    QJsonObject sessionsByService = root["sessionsByService"].toObject();

    // 使用 QSet 来记录已经加载的会话，避免重复
    QSet<QString> loadedSessions;

    for (auto it = sessionsByService.begin(); it != sessionsByService.end(); ++it) {
        QString serviceType = it.key();
        QJsonArray sessionsArray = it.value().toArray();

        for (const QJsonValue& sessionValue : sessionsArray) {
            QJsonObject sessionJson = sessionValue.toObject();
            QString sessionId = sessionJson["sessionId"].toString();
            QString sessionKey = serviceType + "_" + sessionId;

            // 检查是否已经加载过这个会话
            if (loadedSessions.contains(sessionKey)) {
                qDebug() << "跳过重复会话:" << sessionId << "服务:" << serviceType;
                continue;
            }

            loadedSessions.insert(sessionKey);

            // 查找现有会话或创建新会话
            ChatSessionZbk* existingSession = nullptr;
            for (ChatSessionZbk* session : ChatSessionZbk::allSessions[serviceType]) {
                if (session->getSessionId() == sessionId) {
                    existingSession = session;
                    break;
                }
            }

            if (existingSession) {
                // 更新现有会话的消息
                updateSessionFromJson(existingSession, sessionJson);
            } else {
                // 创建新会话
                ChatSessionZbk* newSession = createSessionFromJson(sessionJson);
                if (newSession) {
                    ChatSessionZbk::allSessions[serviceType].append(newSession);
                }
            }
        }
    }

    qDebug() << "聊天会话数据加载完成，去重后会话数量:" << loadedSessions.size();
    return true;
}
void DataManagerZbk::removeDuplicateSessions()
{
    for (auto it = ChatSessionZbk::allSessions.begin(); it != ChatSessionZbk::allSessions.end(); ++it) {
        QString serviceType = it.key();
        QVector<ChatSessionZbk*>& sessions = it.value();

        QSet<QString> seenSessions;
        QVector<ChatSessionZbk*> uniqueSessions;

        for (int i = 0; i < sessions.size(); ++i) {
            ChatSessionZbk* session = sessions[i];
            QString sessionKey = session->getSessionId() + "_" + QString::number(static_cast<int>(session->getSessionType()));

            if (!seenSessions.contains(sessionKey)) {
                seenSessions.insert(sessionKey);
                uniqueSessions.append(session);
            } else {
                qDebug() << "移除重复会话:" << session->getSessionId() << "服务:" << serviceType;
                // 注意: 这里不要删除会话对象，因为可能在其他地方使用
                // 只是从列表中移除，对象由其他地方管理
            }
        }

        sessions = uniqueSessions;
    }
}

// 更新现有会话
void DataManagerZbk::updateSessionFromJson(ChatSessionZbk* session, const QJsonObject& json)
{
    // 清空现有消息
    session->clearMessages();

    // 加载新消息
    QJsonArray messagesArray = json["messages"].toArray();
    for (const QJsonValue& messageValue : messagesArray) {
        QJsonObject messageObj = messageValue.toObject();

        QString senderId = messageObj["senderId"].toString();
        QString content = messageObj["content"].toString();
        MessageType messageType = static_cast<MessageType>(messageObj["messageType"].toInt());

        ChatMessageZbk message(senderId, content, messageType);

        // 设置消息属性
        if (messageObj.contains("messageId")) {
            message.setMessageId(messageObj["messageId"].toString());
        }

        if (messageObj.contains("timestamp")) {
            QDateTime timestamp = QDateTime::fromString(messageObj["timestamp"].toString(), Qt::ISODate);
            message.setTimestamp(timestamp);
        }

        if (messageObj.contains("isRead") && messageObj["isRead"].toBool()) {
            message.markAsRead();
        }

        session->addMessage(message);
    }

    qDebug() << "更新会话:" << session->getSessionId() << "消息数量:" << session->getMessages().size();
}

// 从JSON创建会话
ChatSessionZbk* DataManagerZbk::createSessionFromJson(const QJsonObject& json)
{
    QString sessionId = json["sessionId"].toString();
    SessionType sessionType = static_cast<SessionType>(json["sessionType"].toInt());
    QString serviceType = json["serviceType"].toString();

    // 参与者列表
    QJsonArray participantsArray = json["participants"].toArray();
    QVector<QString> participants;
    for (const QJsonValue& participantValue : participantsArray) {
        participants.append(participantValue.toString());
    }

    // 创建新会话
    ChatSessionZbk* session = new ChatSessionZbk(sessionType, participants, sessionId, serviceType);

    // 加载消息
    QJsonArray messagesArray = json["messages"].toArray();
    for (const QJsonValue& messageValue : messagesArray) {
        QJsonObject messageObj = messageValue.toObject();

        QString senderId = messageObj["senderId"].toString();
        QString content = messageObj["content"].toString();
        MessageType messageType = static_cast<MessageType>(messageObj["messageType"].toInt());

        ChatMessageZbk message(senderId, content, messageType);

        // 设置消息属性
        if (messageObj.contains("messageId")) {
            message.setMessageId(messageObj["messageId"].toString());
        }

        if (messageObj.contains("timestamp")) {
            QDateTime timestamp = QDateTime::fromString(messageObj["timestamp"].toString(), Qt::ISODate);
            message.setTimestamp(timestamp);
        }

        if (messageObj.contains("isRead") && messageObj["isRead"].toBool()) {
            message.markAsRead();
        }

        session->addMessage(message);
    }

    qDebug() << "创建新会话:" << sessionId << "类型:" << static_cast<int>(sessionType)
             << "服务:" << serviceType << "消息数量:" << session->getMessages().size();

    return session;
}
bool DataManagerZbk::loadLastId()
{
    QFile file(getFilePath("lastids.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "聊天会话数据文件不存在，将创建新数据";
        return true;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull()) {
        qDebug() << "聊天会话数据文件格式错误";
        return false;
    }

    QJsonObject root = doc.object();
    UsersZbk::lastId=root["userLast"].toInt();
    groupFactory::lastId=root["groupLast"].toInt();
    return true;
}

ChatSessionZbk* DataManagerZbk::chatSessionFromJson(const QJsonObject& json)
{
    QString sessionId = json["sessionId"].toString();
    SessionType sessionType = static_cast<SessionType>(json["sessionType"].toInt());
    QString serviceType = json["serviceType"].toString();

    // 参与者列表
    QJsonArray participantsArray = json["participants"].toArray();
    QVector<QString> participants;
    for (const QJsonValue& participantValue : participantsArray) {
        participants.append(participantValue.toString());
    }

    ChatSessionZbk* session = new ChatSessionZbk(sessionType, participants, sessionId, serviceType);

    // 加载消息
    QJsonArray messagesArray = json["messages"].toArray();
    for (const QJsonValue& messageValue : messagesArray) {
        QJsonObject messageObj = messageValue.toObject();

        QString senderId = messageObj["senderId"].toString();
        QString content = messageObj["content"].toString();
        MessageType messageType = static_cast<MessageType>(messageObj["messageType"].toInt());

        // 创建消息对象并添加到会话
        ChatMessageZbk message(senderId, content, messageType);

        if (messageObj.contains("messageId")) {
            message.setMessageId(messageObj["messageId"].toString());
        }

        if (messageObj.contains("timestamp")) {
            QDateTime timestamp = QDateTime::fromString(messageObj["timestamp"].toString(), Qt::ISODate);
            message.setTimestamp(timestamp);
        }

        if (messageObj.contains("isRead") && messageObj["isRead"].toBool()) {
            message.markAsRead();
        }

        session->addMessage(message);
    }

    qDebug() << "从JSON创建会话:" << sessionId << "类型:" << static_cast<int>(sessionType)
             << "服务:" << serviceType << "消息数量:" << session->getMessages().size();

    return session;
}


ChatMessageZbk DataManagerZbk::chatMessageFromJson(const QJsonObject& json)
{
    QString senderId = json["senderId"].toString();
    QString content = json["content"].toString();
    MessageType messageType = static_cast<MessageType>(json["messageType"].toInt());
    
    ChatMessageZbk message(senderId, content, messageType);
    
    // 设置其他属性
    if (json["isRead"].toBool()) {
        message.markAsRead();
    }
    
    return message;
}

void DataManagerZbk::rebuildGroupAssociations()
{
    // 重建群组与用户的关联
    // 使用迭代器遍历 QMap
    for (auto it = groupFactory::allGroups.begin(); it != groupFactory::allGroups.end(); ++it) {
        QString serviceType = it.key();
        QVector<GroupsZbk*> groups = it.value();

        for (GroupsZbk* group : groups) {
            for (const QString& memberId : group->getMembers()) {
                User_ServiceZbk* userService = User_ServiceZbk::findUser_IdService(memberId, group->getServiceType());
                if (userService && !userService->m_groups.contains(group->getId())) {
                    userService->m_groups.append(group->getId());
                }
            }
        }
    }
}

void DataManagerZbk::ensureSystemSessions()
{
    for (UsersZbk* user : UsersZbk::allUsers) {
        for (const QString& serviceType : user->getIsusing().keys()) {
            if (user->getIsusing()[serviceType] && user->getData().contains(serviceType)) {
                User_ServiceZbk* userService = user->getData()[serviceType];
                if (userService) {
                    // 确保系统会话存在
                    ChatSessionZbk* systemSession = ChatSessionZbk::getOrCreatePrivateSession(
                        userService->getId(), "system_user", serviceType);
                    Q_UNUSED(systemSession); // 只是确保创建，不需要使用返回值
                }
            }
        }
    }
}

void DataManagerZbk::rebuildUserAssociations()
{
    // 重建用户之间的好友关系
    for (UsersZbk* user : UsersZbk::allUsers) {
        for (const auto& servicePair : user->getData()) {
            User_ServiceZbk* userService = servicePair;
            if (!userService) continue;

            for (const QString& friendId : userService->m_friends) {
                User_ServiceZbk* friendService = User_ServiceZbk::findUser_IdService(friendId, userService->getType());
                if (friendService && !friendService->m_friends.contains(userService->getId())) {
                    friendService->m_friends.append(userService->getId());
                }
            }
        }
    }
}
