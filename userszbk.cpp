#include "userszbk.h"

// UsersZbk::UsersZbk()
// {
//     for(auto i:std::as_const(ServicesZbk::allServices))
//     {
//         m_Isusing[i->Id]=false;
//         serviceData[i->Id]=NULL;
//     }
//     allUsers.push_back(this);
// }

UsersZbk::UsersZbk(QString name,QString pass):m_password(pass),m_nickname(name)
{
    m_id=generateId();
    m_gender=Notset;
    m_location="未设置";
    m_birthday.setDate(2000,1,1);
    m_avatarPath="://pic/87D2B7BB181C40F83726AF5B40E793AC.ico";
    for(auto i:std::as_const(ServicesZbk::allServices))
    {
        m_Isusing[i->Id]=false;
        serviceData[i->Id]=NULL;
    }
    allUsers.push_back(this);

}

UsersZbk::UsersZbk(QString name, QString pass, QString id, QString path):m_password(pass),m_nickname(name),m_avatarPath(path)
{
    if(id=="0")m_id=generateId();
    else m_id=id;
    m_gender=Notset;
    m_location="未设置";
    m_birthday.setDate(2000,1,1);
    for(auto i:std::as_const(ServicesZbk::allServices))
    {
        m_Isusing[i->Id]=false;
        serviceData[i->Id]=NULL;

    }
    allUsers.push_back(this);
}
int UsersZbk::lastId=100000;

QString UsersZbk::getId(){return m_id;}
QString UsersZbk::getPassword(){return m_password;}
QString UsersZbk::getLocation(){return m_location;}
QString UsersZbk::getNickname(){return m_nickname;}
QDate UsersZbk::getBirthday(){return m_birthday;}
QDate UsersZbk::getRegisterDate(){return m_registerDate;}

QString UsersZbk::getavatarPath()
{
    return this->m_avatarPath;
}

QMap<QString, bool> &UsersZbk::getIsusing()
{
    return this->m_Isusing;
}

QMap<QString, User_ServiceZbk *> &UsersZbk::getData()
{
    return serviceData;
}

void UsersZbk::setUserServiceId(const QString &id, const QString &type)
{
    QString tmp=serviceData[type]->m_Id;
    serviceData[type]->m_Id=id;
    for(auto i:serviceData[type]->m_friends)//用户的每一个好友id
    {
        auto ufriend=FindUser_IdService(i,type);
        if(!ufriend)continue;
        auto res=std::find(ufriend->m_friends.begin(),ufriend->m_friends.end(),tmp);//在好友的好友列表中找到自己的原id，改为现id
        if(res!=NULL)*res=id;
    }


    for(auto i:std::as_const(serviceData[type]->m_groups))
    {
        auto ugroup=groupFactory::FindGroup_IdService(i,type);
        if(!ugroup)continue;
        auto res=std::find(ugroup->getMembers().begin(),ugroup->getMembers().end(),tmp);
        if(res!=NULL)*res=id;
    }

}

namespace {

QString generateRequestId()
{
    return QString("req_%1_%2").arg(
                                   QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"))
        .arg(QRandomGenerator::global()->bounded(1000));
}
QJsonObject createFriendRequestData(const QString& applicantId, const QString& applicantName,
                                    const QString& verifyMessage, const QString& requestId) {
    QJsonObject requestData;
    requestData["type"] = "friend_request";
    requestData["applicantId"] = applicantId;
    requestData["applicantName"] = applicantName;
    requestData["verifyMessage"] = verifyMessage;
    requestData["requestId"] = requestId;
    requestData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    requestData["status"] = "pending"; // pending, accepted, rejected
    return requestData;
}
}

//检查是否已存在未处理的好友申请
bool UsersZbk::hasPendingFriendRequest(const QString& targetUserId, const QString& serviceType)
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        return false;
    }

    QString myServiceId = serviceData[serviceType]->getId();

    // 获取目标用户的系统会话
    ChatSessionZbk* targetSystemSession = ChatSessionZbk::getOrCreatePrivateSession(
        targetUserId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (!targetSystemSession) {
        return false;
    }

    // 遍历系统消息，查找未处理的好友申请
    QVector<ChatMessageZbk> messages = targetSystemSession->getMessages();
    for (const ChatMessageZbk& message : messages) {
        // 解析消息内容
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message.getContent().toUtf8(), &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString noticeType = obj["type"].toString();
            QString applicantId = obj["applicantId"].toString();

            // 检查是否是当前用户发送的未处理好友申请
            if (noticeType == "friend_request" && applicantId == myServiceId) {
                return true;
            }
        }
    }

    return false;
}

bool UsersZbk::updateFriendRequest(const QString& targetUserId, const QString& serviceType,
                                   const QString& newVerifyMessage)
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        return false;
    }

    QString myServiceId = serviceData[serviceType]->getId();
    QString myName = serviceData[serviceType]->getName();

    // 获取目标用户的系统会话
    ChatSessionZbk* targetSystemSession = ChatSessionZbk::getOrCreatePrivateSession(
        targetUserId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (!targetSystemSession) {
        return false;
    }

    // 查找并删除旧的好友申请消息
    QVector<ChatMessageZbk> messages = targetSystemSession->getMessages();
    for (int i = messages.size() - 1; i >= 0; --i) {
        const ChatMessageZbk& message = messages[i];

        // 解析消息内容
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message.getContent().toUtf8(), &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString noticeType = obj["type"].toString();
            QString applicantId = obj["applicantId"].toString();

            // 如果是当前用户发送的未处理好友申请，删除它
            if (noticeType == "friend_request" && applicantId == myServiceId) {
                targetSystemSession->removeMessage(message.getMessageId());
            }
        }
    }

    // 发送新的好友申请
    QString requestId = generateRequestId();
    QJsonObject requestData = createFriendRequestData(myServiceId, myName, newVerifyMessage, requestId);
    QJsonDocument doc(requestData);
    QString messageContent = doc.toJson(QJsonDocument::Compact);

    targetSystemSession->addMessage(User_ServiceZbk::SYSTEM_USER_ID, messageContent);

    return true;
}


void UsersZbk::sendFriendRequest(const QString& targetUserId, const QString& serviceType,
                                 const QString& verifyMessage)
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        qDebug() << "用户没有开通" << serviceType << "服务，无法发送好友申请";
        return;
    }

    // 检查目标用户是否存在
    User_ServiceZbk* targetUser = FindUser_IdService(targetUserId, serviceType);
    if (!targetUser) {
        qDebug() << "目标用户" << targetUserId << "不存在";
        return;
    }

    // 已经是好友，不再发送申请
    if (serviceData[serviceType]->m_friends.contains(targetUserId)) {
        qDebug() << "已经是好友，无需重复添加";
        return;
    }

    // 检查是否已存在未处理的好友申请
    if (hasPendingFriendRequest(targetUserId, serviceType)) {
        qDebug() << "已存在未处理的好友申请，更新申请内容";

        // 更新申请内容
        if (updateFriendRequest(targetUserId, serviceType, verifyMessage)) {
            qDebug() << "好友申请已更新";
        } else {
            qDebug() << "好友申请更新失败";
        }
        return;
    }

    // 生成申请数据
    QString requestId = generateRequestId();
    QString applicantId = serviceData[serviceType]->getId();
    QString applicantName = serviceData[serviceType]->getName();

    QJsonObject requestData = createFriendRequestData(applicantId, applicantName, verifyMessage, requestId);
    QJsonDocument doc(requestData);
    QString messageContent = doc.toJson(QJsonDocument::Compact);

    // 确保目标用户的系统会话存在
    ChatSessionZbk* systemSession = ChatSessionZbk::getOrCreatePrivateSession(
        targetUser->getId(), User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (systemSession) {
        // 发送系统消息
        systemSession->addMessage(User_ServiceZbk::SYSTEM_USER_ID, messageContent);
        qDebug() << "好友申请已发送给用户" << targetUserId;
    }

}

void UsersZbk::acceptFriendRequest(const QString& applicantId, const QString& serviceType,const QString& requestMessageId)
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType])
    {
        return;
    }

    // 删除申请消息
    if (!requestMessageId.isEmpty()) {
        removeSystemMessage(requestMessageId, serviceType);
    }

    // 直接添加好友
    addFriend(applicantId, serviceType);
    emit refreshPage(serviceType);
    // 向申请人发送接受通知
    User_ServiceZbk* applicant = FindUser_IdService(applicantId, serviceType);
    if (applicant) {
        ChatSessionZbk* applicantSystemSession = ChatSessionZbk::getOrCreatePrivateSession(
            applicantId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

        if (applicantSystemSession) {
            QJsonObject responseData;
            responseData["type"] = "friend_request_accepted";
            responseData["acceptorId"] = serviceData[serviceType]->getId();
            responseData["acceptorName"] = serviceData[serviceType]->getName();
            responseData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

            QJsonDocument doc(responseData);
            applicantSystemSession->addMessage(User_ServiceZbk::SYSTEM_USER_ID, doc.toJson(QJsonDocument::Compact));
        }
    }

    qDebug() << "已接受" << applicantId << "的好友申请";
}

void UsersZbk::rejectFriendRequest(const QString& applicantId, const QString& serviceType,
                                   const QString& requestMessageId)
{
    // 删除申请消息
    if (!requestMessageId.isEmpty()) {
        removeSystemMessage(requestMessageId, serviceType);
    }

    // 可选：向申请人发送拒绝通知
    User_ServiceZbk* applicant = FindUser_IdService(applicantId, serviceType);
    if (applicant) {
        ChatSessionZbk* applicantSystemSession = ChatSessionZbk::getOrCreatePrivateSession(
            applicantId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

        if (applicantSystemSession) {
            QJsonObject responseData;
            responseData["type"] = "friend_request_rejected";
            responseData["rejectorId"] = serviceData[serviceType]->getId();
            responseData["rejectorName"] = serviceData[serviceType]->getName();
            responseData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

            QJsonDocument doc(responseData);
            applicantSystemSession->addMessage(User_ServiceZbk::SYSTEM_USER_ID, doc.toJson(QJsonDocument::Compact));
        }
    }

    qDebug() << "已拒绝" << applicantId << "的好友申请";
}

// 删除系统消息
bool UsersZbk::removeSystemMessage(const QString& messageId, const QString& serviceType)
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        return false;
    }

    QString myServiceId = serviceData[serviceType]->getId();
    ChatSessionZbk* systemSession = ChatSessionZbk::getOrCreatePrivateSession(
        myServiceId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (systemSession) {
        return systemSession->removeMessage(messageId);
    }

    return false;
}

QVector<ChatMessageZbk> UsersZbk::getSystemNotifications(const QString& serviceType) const
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        return QVector<ChatMessageZbk>();
    }

    QString myServiceId = serviceData[serviceType]->getId();
    ChatSessionZbk* systemSession = ChatSessionZbk::getOrCreatePrivateSession(
        myServiceId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (systemSession) {
        return systemSession->getMessages();
    }

    return QVector<ChatMessageZbk>();
}

QVector<ChatSessionZbk*> UsersZbk::getSystemSessions(const QString& serviceType) const
{
    if (!m_Isusing.value(serviceType, false) || !serviceData.contains(serviceType) || !serviceData[serviceType]) {
        return QVector<ChatSessionZbk*>();
    }

    QString myServiceId = serviceData[serviceType]->getId();
    QVector<ChatSessionZbk*> systemSessions;

    ChatSessionZbk* systemSession = ChatSessionZbk::getOrCreatePrivateSession(
        myServiceId, User_ServiceZbk::SYSTEM_USER_ID, serviceType);

    if (systemSession) {
        systemSessions.append(systemSession);
    }

    return systemSessions;
}


void UsersZbk::setavaterPath(QString path)
{
    this->m_avatarPath=path;
}
QString UsersZbk::generateId()
{
    return QString::number(++lastId);
}

void UsersZbk::setRegisterDate(const QDate &newRegisterDate)
{
    m_registerDate = newRegisterDate;

}

void UsersZbk::setBirthday(const QDate &newBirthday)
{
    m_birthday = newBirthday;
}

void UsersZbk::setNickname(const QString &newNickname)
{
    m_nickname = newNickname;
}

void UsersZbk::setLocation(const QString &newLocation)
{
    m_location = newLocation;
}

Gender UsersZbk::gender() const
{
    return m_gender;
}

void UsersZbk::setGender(Gender newGender)
{
    m_gender = newGender;
}
QVector<UsersZbk*>UsersZbk::allUsers;

void UsersZbk::setId(QString str)
{
    m_id=str;
}

void UsersZbk::addService_fast(QString type)
{
    if(!ServicesZbk::checkService(type))return;
    if(m_Isusing[type]==true)qDebug()<<"user "<<m_id<<" has "<<type<<" service,but trying add again"<<Qt::endl;
    serviceData[type]=new User_ServiceZbk(type,this->getNickname(),this->getPassword(),m_id);
    serviceData[type]->setAvator(getavatarPath());
    serviceData[type]->setId(getId());
    m_Isusing[type]=true;
}

void UsersZbk::removeService(QString type)
{
    if(!ServicesZbk::checkService(type))return;
    if(m_Isusing[type])
    {
        m_Isusing[type]=false;
    }
    else
    {
        qDebug()<<"用户"<<m_id<<"没有"<<type<<"服务，您正在尝试删除！"<<Qt::endl;
        return;
    }



    for(auto i:serviceData[type]->m_friends)
    {
        auto ufriend=FindUser_IdService(i,type);
        if(!ufriend)continue;
        ufriend->m_friends.removeAll(serviceData[type]->m_Id);
    }
    for(auto i:std::as_const(serviceData[type]->m_groups))
    {
        auto ugroup=groupFactory::FindGroup_IdService(i,type);
        if(!ugroup)continue;
        ugroup->getMembers().removeAll(serviceData[type]->m_Id);
    }

    delete serviceData[type];
    serviceData[type]=NULL;
}

void UsersZbk::addGroup(const QString &groupId, const QString &serviceType)
{
    if(!ServicesZbk::checkService(serviceType))return;
    GroupsZbk* found=groupFactory::FindGroup_IdService(groupId,serviceType);
    if(found==NULL)
    {
        qDebug()<<"未找到id"<<groupId<<",type"<<serviceType<<"的群组"<<Qt::endl;
        return;
    }
    if(serviceData[serviceType]->m_groups.contains(groupId))return;
    // found->addMember(serviceData[serviceType]->getId());
    groupFactory::AddMember(found,serviceData[serviceType]->getId());

}

void UsersZbk::addFriend(const QString &friendId, const QString &serviceType)
{
    if(!ServicesZbk::checkService(serviceType))return;
    User_ServiceZbk* found=FindUser_IdService(friendId,serviceType);
    if(found==NULL)
    {
        qDebug()<<"未找到id"<<friendId<<",type"<<serviceType<<"的用户"<<Qt::endl;
        return;
    }
    if(serviceData[serviceType]->m_friends.contains(friendId))return;
    serviceData[serviceType]->m_friends.push_front(found->getId());
    found->m_friends.push_front(serviceData[serviceType]->getId());

    ChatSessionZbk::getOrCreatePrivateSession(serviceData[serviceType]->getId(),friendId,serviceType);

}

void UsersZbk::removeFriend(const QString &friendId, const QString &serviceType)
{
    if(!ServicesZbk::checkService(serviceType))return;
    User_ServiceZbk* found=FindUser_IdService(friendId,serviceType);
    if(found==NULL)
    {
        qDebug()<<"未找到id"<<friendId<<",type"<<serviceType<<"的用户"<<Qt::endl;
        return;
    }
    if(!serviceData[serviceType]->m_friends.contains(friendId))return;
    serviceData[serviceType]->m_friends.removeAll(found->getId());
    found->m_friends.removeAll(serviceData[serviceType]->getId());

    ChatSessionZbk::deletePrivateSession(serviceData[serviceType]->getId(),friendId,serviceType);


}

User_ServiceZbk *UsersZbk::FindUser_IdService(QString id, QString ser)
{
    // for(auto i:std::as_const(User_ServiceZbk::allServiceUsers[ser]))
    // {


    //     if(i->m_Id==id)return i;

    // }
    // qDebug()<<"未找到"<<"id"<<id<<",type"<<ser<<"的用户"<<Qt::endl;
    // return NULL;
    return User_ServiceZbk::findUser_IdService(id,ser);
}

UsersZbk *UsersZbk::Find_UniTencentUser(QString id)
{
    for(auto i:allUsers)
    {
        if(i->m_id==id)
        {
            return i;
        }
    }
    qDebug()<<"UsersZbk::Find_UniTencentUser:id "<<id<<" 的用户不存在"<<Qt::endl;
    return NULL;
}

GroupsZbk* UsersZbk::changeGroupType(GroupsZbk *group, const QString &targetType,QString& state)
{
    if(group==NULL)return NULL;
    if(!ServicesZbk::checkService(targetType))return NULL;
    QVector<QString>newMember;
    QVector<QString>newManager;
    for(auto i:group->getMembers())//转移用户
    {
        User_ServiceZbk* user_service=User_ServiceZbk::findUser_IdService(i,group->getServiceType());//找到该用户的服务数据
        if(user_service==NULL)
        {
            qDebug()<<(QString("void UsersZbk::changeGroupType(GroupsZbk *group, const QString &targetType):用户%1,%2不存在").arg(i,group->getServiceType()));
            continue;
        }
        UsersZbk* user=UsersZbk::Find_UniTencentUser(user_service->getmainId());//通过服务数据找到整合账号
        if(user_service==NULL)
        {
            qDebug()<<QString("void UsersZbk::changeGroupType(GroupsZbk *group, const QString &targetType):UniTencent用户%1不存在").arg(user_service->getmainId());
            continue;
        }

        if(user->getIsusing()[targetType]==false)
        {
            state.clear();
            state=QString("用户 %1 没有开启 %2 服务").arg(user_service->getId(),ServicesZbk::toZhcn[targetType]);
            return /*QString("用户 %1 没有开启 %2 服务").arg(user_service->getId(),targetType)*/NULL;
        }
        newMember.push_back(user->getData()[targetType]->getId());
        if(!ServicesZbk::hasAdministors(targetType))continue;
        if(group->Managers().contains(user_service->getId()))
            newManager.push_back(user->getData()[targetType]->getId());

    }

    GroupsZbk* newGroup=groupFactory::createGroup(targetType,group->OwnerId());

    newGroup->setCreateDate(group->createDate());
    newGroup->setAvatarPath(group->getAvatar());
    // newGroup->setMembers(newMember);
    for(const auto &i:newMember)
    {
        // newGroup->addMember(i);
        groupFactory::AddMember(newGroup,i);
    }
    newGroup->setManagers(newManager);
    newGroup->setName(group->getName());
    groupFactory::deleteGroup(group->getId(),group->getServiceType());
    return newGroup;
}


void UsersZbk::setIsusing(const QMap<QString, bool>& isusing)
{
    m_Isusing.clear();
    this->m_Isusing = isusing;
}

void UsersZbk::addPrivateMessage(const QString& friendServiceId, const QString& serviceType, const QString& content) {
    if (!serviceData.contains(serviceType) || !serviceData[serviceType]) return;

    QString myServiceId = serviceData[serviceType]->getId();
    ChatSessionZbk* session = ChatSessionZbk::getOrCreatePrivateSession(myServiceId, friendServiceId, serviceType);
    if (session)
    {
        session->addMessage(myServiceId, content);
    }

}

void UsersZbk::addGroupMessage(const QString& groupId, const QString& serviceType, const QString& content)
{
    if (!serviceData.contains(serviceType) || !serviceData[serviceType]) return;

    QString myServiceId = serviceData[serviceType]->getId();

    // 获取群组成员
    GroupsZbk* group = groupFactory::FindGroup_IdService(groupId, serviceType);
    if (!group) return;

    QVector<QString> members = group->getMembers();
    ChatSessionZbk* session = ChatSessionZbk::getOrCreateGroupSession(groupId, members, serviceType);
    if (session)
    {
        session->addMessage(myServiceId, content);
    }

}

QVector<ChatSessionZbk*> UsersZbk::getUserSessions(const QString& serviceType) const
{
    if (!serviceData.contains(serviceType) || !serviceData[serviceType])
    {
        return QVector<ChatSessionZbk*>();
    }

    QString myServiceId = serviceData[serviceType]->getId();
    return ChatSessionZbk::getUserSessions(myServiceId, serviceType);
}

QVector<ChatMessageZbk> UsersZbk::getPrivateChatHistory(const QString& friendServiceId, const QString& serviceType) const
{
    if (!serviceData.contains(serviceType) || !serviceData[serviceType])
    {
        return QVector<ChatMessageZbk>();
    }

    QString myServiceId = serviceData[serviceType]->getId();
    return ChatSessionZbk::getPrivateChatHistory(myServiceId, friendServiceId, serviceType);
}

QVector<ChatMessageZbk> UsersZbk::getGroupChatHistory(const QString& groupId, const QString& serviceType) const
{
    return ChatSessionZbk::getGroupChatHistory(groupId, serviceType);
}




