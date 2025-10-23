#ifndef GROUPFACTORY_H
#define GROUPFACTORY_H

#include <QWidget>

#include "qqgroupzbk.h"
#include "wechatgroupzbk.h"

#include "chatmessagezbk.h"
#include "chatsessionzbk.h"
class groupFactory
{

public:
    groupFactory();
    static void AddMember(GroupsZbk* group,const QString& userId);
    static bool RemoveMember(GroupsZbk *group, const QString& userId);//返回true表示移除该成员后，该群还在（没被因人少而解散）
    static GroupsZbk* createGroup(const QString& type,const QString& ownerId);
    static QMap<QString,QVector<GroupsZbk*>>allGroups;
    static GroupsZbk* FindGroup_IdService(const QString& id,const QString& ser);
    static QString generateId();
    static void deleteGroup(const QString& id,const QString& type);
    //static void changeGroupType(GroupsZbk* group,const QString& targetType);//直接转换，没有进行任何检查
    static void syncGroupMembers(const QString& groupId, const QString& serviceType);
    static int lastId;
    static GroupsZbk *createGroup(const QString &type, const QString &id, const QString &ownerId, const QString &name, const QString &path=":/pic/groupIcon.png");
};




#endif // GROUPFACTORY_H
