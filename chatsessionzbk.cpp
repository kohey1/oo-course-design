#include "chatsessionzbk.h"
#include <algorithm>

QMap<QString, QVector<ChatSessionZbk*>> ChatSessionZbk::allSessions;

ChatSessionZbk::ChatSessionZbk(SessionType type, const QVector<QString>& participantsList,const QString& id, const QString& service)
    : sessionType(type), participants(participantsList), sessionId(id), serviceType(service)
{
    // 检查是否已经存在相同的会话
    bool exists = false;
    for (ChatSessionZbk* session : allSessions[serviceType]) {
        if (session->getSessionId() == sessionId && session->getSessionType() == sessionType) {
            exists = true;
            break;
        }
    }

    if (!exists) {
        allSessions[serviceType].append(this);
    } else {
        qDebug() << "警告: 尝试创建重复会话:" << sessionId << "服务:" << serviceType;
    }
}

ChatSessionZbk::~ChatSessionZbk() {
    allSessions[serviceType].removeAll(this);
}

void ChatSessionZbk::addMessage(const QString& senderId, const QString& content)
{
    ChatMessageZbk message(senderId, content, MessageType::TEXT);
    messages.append(message);
}

void ChatSessionZbk::addMessage(const ChatMessageZbk &message)
{
    messages.append(message);
}

QVector<ChatMessageZbk> ChatSessionZbk::getMessagesFromUser(const QString& userId) const {
    QVector<ChatMessageZbk> result;
    for (const ChatMessageZbk& msg : messages)
    {
        if (msg.getSenderId() == userId)
        {
            result.append(msg);
        }
    }
    return result;
}

QVector<ChatMessageZbk> ChatSessionZbk::searchMessages(const QString& keyword) const {
    QVector<ChatMessageZbk> result;
    for (const ChatMessageZbk& msg : messages) {
        if (msg.containsKeyword(keyword)) {
            result.append(msg);
        }
    }
    return result;
}

void ChatSessionZbk::updateParticipants(const QVector<QString>& newParticipants) {
    if (sessionType == SessionType::GROUP_CHAT) {
        participants = newParticipants;
    }
}

bool ChatSessionZbk::involvesUser(const QString& userId) const {
    return participants.contains(userId);
}


ChatSessionZbk* ChatSessionZbk::getOrCreatePrivateSession(const QString& user1, const QString& user2, const QString& serviceType)
{
    QString sessionId = generatePrivateSessionId(user1, user2);

    // 查找现有会话
    for (ChatSessionZbk* session : allSessions[serviceType])
    {
        if (session->getSessionId() == sessionId && session->getSessionType() == SessionType::PRIVATE_CHAT) {
            qDebug() << "*********已找到会话 " << sessionId;
            return session;
        }
    }

    // 创建新会话
    qDebug() << "*********未找到会话 " << sessionId;
    QVector<QString> participants = {user1, user2};
    ChatSessionZbk* newSession = new ChatSessionZbk(SessionType::PRIVATE_CHAT, participants, sessionId, serviceType);
    return newSession;
}

ChatSessionZbk* ChatSessionZbk::getOrCreateGroupSession(const QString& groupId, const QVector<QString>& members,const QString& serviceType)
{
    // 查找现有会话
    for (ChatSessionZbk* session : allSessions[serviceType])
    {
        if (session->getSessionId() == groupId && session->getSessionType() == SessionType::GROUP_CHAT)
        {
            // 更新成员列表
            session->updateParticipants(members);
            return session;
        }
    }

    // 创建新会话
    ChatSessionZbk* newSession = new ChatSessionZbk(SessionType::GROUP_CHAT, members, groupId, serviceType);
    return newSession;
}

ChatSessionZbk* ChatSessionZbk::findSession(const QString& sessionId, const QString& serviceType)
{
    for (ChatSessionZbk* session : allSessions[serviceType])
    {
        if (session->getSessionId() == sessionId)
        {
            return session;
        }
    }
    return nullptr;
}

QVector<ChatSessionZbk*> ChatSessionZbk::getUserSessions(const QString& userId, const QString& serviceType)
{
    QVector<ChatSessionZbk*> result;
    for (ChatSessionZbk* session : allSessions[serviceType])
    {
        if (session->involvesUser(userId))
        {
            result.append(session);
        }
    }
    return result;
}

QVector<ChatMessageZbk> ChatSessionZbk::getPrivateChatHistory(const QString& user1, const QString& user2,
                                                              const QString& serviceType) {
    QString sessionId = generatePrivateSessionId(user1, user2);
    ChatSessionZbk* session = findSession(sessionId, serviceType);
    if (session) {
        return session->getMessages();
    }
    return QVector<ChatMessageZbk>();
}

QVector<ChatMessageZbk> ChatSessionZbk::getGroupChatHistory(const QString& groupId, const QString& serviceType) {
    ChatSessionZbk* session = findSession(groupId, serviceType);
    if (session && session->getSessionType() == SessionType::GROUP_CHAT) {
        return session->getMessages();
    }
    return QVector<ChatMessageZbk>();
}

// 删除方法
void ChatSessionZbk::deletePrivateSession(const QString& user1, const QString& user2,
                                          const QString& serviceType) {
    QString sessionId = generatePrivateSessionId(user1, user2);

    for (int i = 0; i < allSessions[serviceType].size(); ++i) {
        ChatSessionZbk* session = allSessions[serviceType][i];
        if (session->getSessionId() == sessionId &&
            session->getSessionType() == SessionType::PRIVATE_CHAT) {
            delete session; // 直接删除会话和所有消息
            break;
        }
    }
}

void ChatSessionZbk::deleteGroupSession(const QString& groupId, const QString& serviceType) {
    for (int i = 0; i < allSessions[serviceType].size(); ++i) {
        ChatSessionZbk* session = allSessions[serviceType][i];
        if (session->getSessionId() == groupId &&
            session->getSessionType() == SessionType::GROUP_CHAT) {
            delete session; // 直接删除会话和所有消息
            break;
        }
    }
}

QString ChatSessionZbk::generatePrivateSessionId(const QString& user1, const QString& user2) {
    if (user1 < user2) {
        return "private_" + user1 + "_" + user2;
    } else {
        return "private_" + user2 + "_" + user1;
    }
}

bool ChatSessionZbk::removeMessage(const QString& messageId)
{
    for (int i = 0; i < messages.size(); ++i) {
        if (messages[i].getMessageId() == messageId) {
            messages.remove(i);
            return true;
        }
    }
    return false;
}

int ChatSessionZbk::removeMessages(std::function<bool(const ChatMessageZbk&)> predicate)
{
    int removedCount = 0;
    for (int i = messages.size() - 1; i >= 0; --i) {
        if (predicate(messages[i])) {
            messages.remove(i);
            removedCount++;
        }
    }
    return removedCount;
}
