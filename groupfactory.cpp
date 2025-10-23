#include "groupfactory.h"

groupFactory::groupFactory(){}
int groupFactory::lastId=100000;
void groupFactory::AddMember(GroupsZbk *group, const QString &userId)
{
    group->addMember(userId);
    syncGroupMembers(group->getId(),group->getServiceType());

}

bool groupFactory::RemoveMember(GroupsZbk* group, const QString &userId)
{
    if(!group)return false;
    group->removeMember(userId);
    syncGroupMembers(group->getId(),group->getServiceType());
    if(group->getSize()<2)
    {groupFactory::deleteGroup(group->getId(),group->getServiceType());return false;}

    return true;
}





GroupsZbk *groupFactory::createGroup(const QString &type,const QString& ownerId)
{


    GroupsZbk* group;
    if(type=="QQ")group=new QQGroupZbk;
    else if(type=="WeChat")group=new WeChatGroupZbk;
    else return NULL;

    group->setId(generateId());
    group->setName("groupName");
    group->setOwnerId(ownerId);
    group->setServiceType(type);


    group->setAvatarPath(":/pic/groupIcon.png");
    allGroups[type].push_back(group);
    group->setCreateDate(QDate::currentDate());

    QVector<QString> initialMembers = {ownerId};
    ChatSessionZbk::getOrCreateGroupSession(group->getId(), initialMembers, type);
    return group;
}

GroupsZbk *groupFactory::createGroup(const QString &type,const QString &id,const QString& ownerId,const QString &name,const QString&path)
{

    // 检查是否已存在相同ID的群组
    if (allGroups.contains(type)) {
        for (GroupsZbk* existingGroup : allGroups[type]) {
            if (existingGroup->getId() == id) {
                qDebug() << "群组已存在，跳过创建:" << id << name;
                return existingGroup;
            }
        }
    }

    GroupsZbk* group;
    if(type=="QQ")group=new QQGroupZbk;
    else if(type=="WeChat")group=new WeChatGroupZbk;
    else return NULL;
    if(id=="0")
        group->setId(generateId());
    else
        group->setId(id);
    group->setOwnerId(ownerId);
    group->setName(name);
    group->setAvatarPath(path);
    allGroups[type].push_back(group);
    group->setCreateDate(QDate::currentDate());
    group->setServiceType(type);


    QVector<QString> initialMembers = {ownerId};
    ChatSessionZbk::getOrCreateGroupSession(group->getId(), initialMembers, type);

    return group;
}


QMap<QString,QVector<GroupsZbk*>> groupFactory::allGroups;
GroupsZbk* groupFactory::FindGroup_IdService(const QString& id,const QString& ser)
{
    if(!ServicesZbk::checkService(ser))return NULL;
    for(const auto& i:allGroups[ser])
    {
        //qDebug()<<QString("***********查找：正在尝试群id %1").arg(i->getId());
        if(i->getId()==id)
            return i;
    }
    qDebug()<<"FindGroup_IdService:未找到id "<<id<<",type "<<ser<<" 的群组";
    return NULL;
}

QString groupFactory::generateId()
{
    return QString::number(++lastId);
}

void groupFactory::deleteGroup(const QString &id, const QString &type)
{
    GroupsZbk* group=FindGroup_IdService(id,type);
    qDebug()<<QString("正在删除群%1...").arg(id);
    if(group==NULL)
    {
        qDebug()<<QString("未找到id %1 类型%2的群聊,无法删除该群").arg(id,type);
        return;
    }
    ChatSessionZbk::deleteGroupSession(id,type);
    for(const auto &i:group->getMembers())
    {
        User_ServiceZbk* user=User_ServiceZbk::findUser_IdService(i,type);
        if(user==NULL)continue;
        user->m_groups.removeAll(id);
    }
    allGroups[type].removeIf([&](GroupsZbk* g){if(g->getId()==id)return true;return false;});

    delete group;
}

void groupFactory::syncGroupMembers(const QString &groupId, const QString &serviceType)
{
    GroupsZbk* group = groupFactory::FindGroup_IdService(groupId, serviceType);
    if (!group) return;

    QVector<QString> members = group->getMembers();
    ChatSessionZbk* session = ChatSessionZbk::findSession(groupId, serviceType);
    if (session && session->getSessionType() == SessionType::GROUP_CHAT)
    {
        session->updateParticipants(members);
    }

}

// void groupFactory::changeGroupType(GroupsZbk *group, const QString &targetType)//直接转换，没有进行任何检查
// {

//     GroupsZbk* newGroup=createGroup(targetType,group->OwnerId());
//     newGroup->setCreateDate(group->createDate());
//     newGroup->setAvatarPath(group->getAvatar());
//     for(auto i:group->getMembers())
//     {
//         User_ServiceZbk* user_service=User_ServiceZbk::findUser_IdService(i,group->getServiceType());

//     }
//     if(newGroup->hasAdministrators())
//     {

//     }








//     deleteGroup(group->getId(),group->getServiceType());

// }
