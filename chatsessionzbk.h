#ifndef CHATSESSIONZBK_H
#define CHATSESSIONZBK_H


#include "chatmessagezbk.h"
#include <QString>
#include <QVector>
#include <QMap>

enum class SessionType {
    PRIVATE_CHAT,  // 两人会话
    GROUP_CHAT,    // 群组会话
    SYSTEM_CHAT    // 系统会话
};
class ChatSessionZbk {
public:
    ChatSessionZbk(SessionType type, const QVector<QString>& participants, const QString& sessionId, const QString& serviceType);
    ~ChatSessionZbk();
    void addMessage(const QString& senderId, const QString& content);
    void addMessage(const ChatMessageZbk& message);
    QString getSessionId() const { return sessionId; }
    SessionType getSessionType() const { return sessionType; }
    QString getServiceType() const { return serviceType; }
    QVector<QString> getParticipants() const { return participants; }
    QVector<ChatMessageZbk> getMessages() const { return messages; }
    //查找
    QVector<ChatMessageZbk> getMessagesFromUser(const QString& userId) const;
    QVector<ChatMessageZbk> searchMessages(const QString& keyword) const;
    // 成员管理（主要用于群聊）
    void updateParticipants(const QVector<QString>& newParticipants);
    bool involvesUser(const QString& userId) const;
    //会话管理
    static ChatSessionZbk* getOrCreatePrivateSession(const QString& user1, const QString& user2,
                                                     const QString& serviceType);
    static ChatSessionZbk* getOrCreateGroupSession(const QString& groupId,
                                                   const QVector<QString>& members,
                                                   const QString& serviceType);
    static ChatSessionZbk* findSession(const QString& sessionId, const QString& serviceType);
    static QVector<ChatSessionZbk*> getUserSessions(const QString& userId, const QString& serviceType);
    static QVector<ChatMessageZbk> getPrivateChatHistory(const QString& user1, const QString& user2,
                                                         const QString& serviceType);
    static QVector<ChatMessageZbk> getGroupChatHistory(const QString& groupId, const QString& serviceType);
    static void deletePrivateSession(const QString& user1, const QString& user2,
                                     const QString& serviceType);
    static void deleteGroupSession(const QString& groupId, const QString& serviceType);
    static QMap<QString, QVector<ChatSessionZbk*>> allSessions;
    static QString generatePrivateSessionId(const QString& user1, const QString& user2);
    bool removeMessage(const QString& messageId);//删除特定消息
    int removeMessages(std::function<bool(const ChatMessageZbk&)> predicate);//根据条件删除消息
    void clearMessages() { messages.clear(); }
private:
    SessionType sessionType;
    QVector<QString> participants;
    QString sessionId;
    QString serviceType;
    QVector<ChatMessageZbk> messages;


};

#endif // CHATSESSIONZBK_H
