// chatmessagemodel.cpp
#include "chatmessagemodelzbk.h"
#include <QDateTime>

ChatMessageModelZbk::ChatMessageModelZbk(QObject *parent)
    : QAbstractListModel(parent), m_session(nullptr)
{
}

void ChatMessageModelZbk::setSession(ChatSessionZbk* session, const QString& currentUserId)
{
    beginResetModel();
    qDebug()<<"void ChatMessageModelZbk::setSession(ChatSessionZbk* session, const QString& currentUserId):加载session:"<<session->getSessionId()<<" "<<QString::number(session->getMessages().size());
    m_session = session;
    m_currentUserId = currentUserId;
    m_serviceType = session ? session->getServiceType() : "";
    if (m_session) {
        m_messages = m_session->getMessages();
    } else {
        m_messages.clear();
    }

    endResetModel();
}

void ChatMessageModelZbk::addMessage(const QString& senderId, const QString& content)
{
    if (!m_session) return;

    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_session->addMessage(senderId, content);
    m_messages = m_session->getMessages();
    endInsertRows();
}

int ChatMessageModelZbk::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_messages.size();
}

QVariant ChatMessageModelZbk::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    const ChatMessageZbk& message = m_messages.at(index.row());

    switch (role) {
    case SenderIdRole:
        return message.getSenderId();
    case SenderNameRole:
        return getSenderName(message.getSenderId());
    case ContentRole:
        return message.getContent();
    case TimestampRole:
        return message.getTimestamp().toString("yyyy-MM-dd hh:mm:ss");
    case AvatarRole:
        return getAvatarPath(message.getSenderId());
    case IsOwnMessageRole:
        return message.getSenderId() == m_currentUserId;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChatMessageModelZbk::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[SenderIdRole] = "senderId";
    roles[SenderNameRole] = "senderName";
    roles[ContentRole] = "content";
    roles[TimestampRole] = "timestamp";
    roles[AvatarRole] = "avatar";
    roles[IsOwnMessageRole] = "isOwnMessage";
    return roles;
}

QString ChatMessageModelZbk::getSenderName(const QString& senderId) const
{
    if (!m_session || m_serviceType.isEmpty()) return senderId;

    // 只在当前会话的服务类型中查找
    if (User_ServiceZbk::allServiceUsers.contains(m_serviceType)) {
        for (User_ServiceZbk* user : User_ServiceZbk::allServiceUsers[m_serviceType]) {
            if (user->getId() == senderId) {
                return user->getName();
            }
        }
    }

    return senderId;
}

QString ChatMessageModelZbk::getAvatarPath(const QString& senderId) const
{
    if (!m_session || m_serviceType.isEmpty()) return ":/pic/default_avatar.png";

    // 只在当前会话的服务类型中查找
    if (User_ServiceZbk::allServiceUsers.contains(m_serviceType)) {
        for (User_ServiceZbk* user : User_ServiceZbk::allServiceUsers[m_serviceType]) {
            if (user->getId() == senderId) {
                QString avatar = user->getAvatar();
                return avatar.isEmpty() ? ":/pic/default_avatar.png" : avatar;
            }
        }
    }

    return ":/pic/default_avatar.png";
}
