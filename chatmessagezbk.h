#ifndef CHATMESSAGEZBK_H
#define CHATMESSAGEZBK_H
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>

enum class MessageType {
    TEXT,
    IMAGE,
    VOICE,
    VIDEO,
    FILE,
    SYSTEM
};

class ChatMessageZbk {
public:
    ChatMessageZbk(const QString& senderId,
                   const QString& content,
                   MessageType type = MessageType::TEXT);

    // Getter方法
    QString getMessageId() const { return messageId; }
    QString getSenderId() const { return senderId; }
    QString getContent() const { return content; }
    MessageType getMessageType() const { return messageType; }
    QDateTime getTimestamp() const { return timestamp; }
    bool getIsRead() const { return isRead; }

    void markAsRead() { isRead = true; }

    // 用于搜索
    bool containsKeyword(const QString& keyword) const;


    QJsonObject getFriendRequestData() const;

    bool isFriendRequest() const;

    bool isFriendRequestResponse() const;

    void setMessageId(const QString &newMessageId);

    void setTimestamp(const QDateTime &newTimestamp);

    void setTimestamp(const QString &str);
signals:
    void timestampChanged();

private:
    QString messageId;
    QString senderId;
    QString content;
    MessageType messageType;
    QDateTime timestamp;
    bool isRead;

    QString generateMessageId();
    Q_PROPERTY(QDateTime timestamp READ getTimestamp WRITE setTimestamp NOTIFY timestampChanged FINAL)
};
#endif // CHATMESSAGEZBK_H
