#include "chatmessagezbk.h"
#include <QUuid>

ChatMessageZbk::ChatMessageZbk(const QString& sender,
                               const QString& msgContent,
                               MessageType type)
    : senderId(sender), content(msgContent), messageType(type), isRead(false)
{
    timestamp = QDateTime::currentDateTime();
    messageId = generateMessageId();
}

QString ChatMessageZbk::generateMessageId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool ChatMessageZbk::containsKeyword(const QString& keyword) const {
    return content.contains(keyword, Qt::CaseInsensitive);
}

QJsonObject ChatMessageZbk::getFriendRequestData() const {
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj["type"].toString() == "friend_request") {
            return obj;
        }
    }
    return QJsonObject();
}

bool ChatMessageZbk::isFriendRequest() const {
    return getFriendRequestData().contains("applicantId");
}

bool ChatMessageZbk::isFriendRequestResponse() const {
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();
        return type == "friend_request_accepted" || type == "friend_request_rejected";
    }
    return false;
}

void ChatMessageZbk::setMessageId(const QString &newMessageId)
{
    messageId = newMessageId;
}



void ChatMessageZbk::setTimestamp(const QDateTime &newTimestamp)
{
    if (timestamp == newTimestamp)
        return;
    timestamp = newTimestamp;
}
