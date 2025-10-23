#ifndef CHATMESSAGEMODELZBK_H
#define CHATMESSAGEMODELZBK_H

#include <QAbstractListModel>
#include <QVector>
#include "chatsessionzbk.h"
#include "user_service.h"

class ChatMessageModelZbk : public QAbstractListModel
{
    Q_OBJECT

public:
    enum MessageRoles {
        SenderIdRole = Qt::UserRole + 1,
        SenderNameRole,
        ContentRole,
        TimestampRole,
        AvatarRole,
        IsOwnMessageRole
    };

    explicit ChatMessageModelZbk(QObject *parent = nullptr);

    void setSession(ChatSessionZbk* session, const QString& currentUserId);
    void addMessage(const QString& senderId, const QString& content);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    ChatSessionZbk* m_session;
    QString m_currentUserId;
    QString m_serviceType;
    QVector<ChatMessageZbk> m_messages;

    QString getSenderName(const QString& senderId) const;
    QString getAvatarPath(const QString& senderId) const;
};

#endif // CHATMESSAGEMODELZBK_H
