#include "groupszbk.h"
GroupsZbk::GroupsZbk()
{

}

QString GroupsZbk::getId() const
{
    return m_id;
}

QString GroupsZbk::getName() const
{
    return m_name;
}

QString GroupsZbk::getServiceType() const
{
    return m_serviceType;
}

QVector<QString> GroupsZbk::getMembers() const
{
    return m_members;
}

QString GroupsZbk::getAvatar() const
{
    return m_avatarPath;

}

void GroupsZbk::setId(const QString &id)
{
    m_id=id;
}




void GroupsZbk::setMembers(const QVector<QString> &newMembers)
{
    m_members = newMembers;
}

void GroupsZbk::setServiceType(const QString &newServiceType)
{
    m_serviceType = newServiceType;
}

QVector<QString> GroupsZbk::Managers()
{
    return m_Managers;
}

void GroupsZbk::setManagers(const QVector<QString> &newManagers)
{
    m_Managers = newManagers;
}

QDate GroupsZbk::createDate() const
{
    return m_createDate;
}

void GroupsZbk::setCreateDate(const QDate &newCreateDate)
{
    m_createDate = newCreateDate;
}

int GroupsZbk::getSize() const
{
    return m_members.size();
}

QString GroupsZbk::OwnerId() const
{
    return m_OwnerId;
}

void GroupsZbk::setOwnerId(const QString &newOwnerId)
{
    m_OwnerId = newOwnerId;
}

void GroupsZbk::setName(const QString &name)
{
    m_name=name;
}

void GroupsZbk::setAvatarPath(const QString &path)
{
    m_avatarPath=path;
}

bool GroupsZbk::addMember(const QString &userId)
{
    // if(!User_ServiceZbk::allServiceUsers["QQ"].contains(userId))
    //     return false;
    bool flag=false;
    for(auto i:std::as_const(User_ServiceZbk::allServiceUsers[m_serviceType]))
    {
        if(i->getId()==userId)
        {
            flag=true;
            i->m_groups.push_front(this->m_id);
        }

    }
    if(!flag)return false;
    if(m_members.contains(userId))return true;
    m_members.push_back(userId);
    return true;
}

bool GroupsZbk::removeMember(const QString &userId)
{
    if(m_members.contains(userId))
    {
        m_members.removeAll(userId);
        User_ServiceZbk* user=User_ServiceZbk::findUser_IdService(userId,m_serviceType);
        if(!user)return false;
        user->m_groups.removeAll(m_id);
        return true;
    }
    return false;
}

bool GroupsZbk::canManageMembers(const QString &userId) const
{
    if(userId==m_OwnerId||(hasAdministrators()&&m_Managers.contains(userId)))
        return true;
    else
        return false;
}

void GroupsZbk::addManager(const QString &userId)
{
    if(!hasAdministrators())
    {
        qDebug()<<QString("void GroupsZbk::addManager(const QString &userId):群 %1 为 %2 类型，无法设置管理员").arg(m_name,m_serviceType);
        return;
    }
    if(canManageMembers(userId))
    {
        qDebug()<<QString("void GroupsZbk::addManager(const QString &userId):id %1 的用户已经是群 %2 的管理员或群主").arg(userId,m_name);
        return;
    }
    m_Managers.push_back(userId);
}

void GroupsZbk::removeManager(const QString &userId)
{
    if(!hasAdministrators())
    {
        qDebug()<<QString("void GroupsZbk::removeManager(const QString &userId):群 %1 为 %2 类型，无法取消管理员").arg(m_name,m_serviceType);
        return;
    }
    if(!m_Managers.contains(userId))
    {
        qDebug()<<QString("void GroupsZbk::removeManager(const QString &userId):id %1 的用户不是群 %2 的管理员").arg(userId,m_name);
        return;
    }
    m_Managers.removeAll(userId);
}







