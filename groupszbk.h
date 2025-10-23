#ifndef GROUPSZBK_H
#define GROUPSZBK_H
#include <QString>
#include <QList>
#include <QObject>
#include <QMap>

#include "user_service.h"

class GroupsZbk : public QObject
{
    Q_OBJECT
    friend class groupFactory;
public:
    explicit GroupsZbk();
    virtual ~GroupsZbk() = default;
    QString getId() const;
    QString getName() const;
    QString getServiceType() const;
    QVector<QString> getMembers() const;
    QString getAvatar()const;
    void setName(const QString& name);
    void setAvatarPath(const QString& path);
    bool addMember(const QString& userId);
    bool removeMember(const QString& userId);


    // 权限管理接口
    virtual bool canManageMembers(const QString& userId) const;

    void addManager(const QString& userId);
    void removeManager(const QString& userId);

    // 特色功能
    virtual bool allowSubGroups() const = 0;
    virtual bool hasAdministrators() const = 0;

    static QString generateId();
    QString OwnerId() const;
    void setOwnerId(const QString &newOwnerId);
    QDate createDate() const;
    void setCreateDate(const QDate &newCreateDate);
    int getSize()const;
    QVector<QString> Managers();
    void setManagers(const QVector<QString> &newManagers);
    void setServiceType(const QString &newServiceType);
    void setMembers(const QVector<QString> &newMembers);
    void setIsSubGroup(bool newIsSubGroup);
signals:
    void memberAdded(const QString& userId);
    void memberRemoved(const QString& userId);
    void groupInfoChanged();
protected:
    void setId(const QString& id);
    QString m_id;
    QString m_name;
    QString m_serviceType;
    QVector<QString> m_members;
    QVector<QString>m_Managers;
    QString m_avatarPath;
    QString m_OwnerId;
    QDate m_createDate;
};










#endif // GROUPSZBK_H
