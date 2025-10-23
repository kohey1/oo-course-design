// friendlistwidget.cpp
#include "friendlistwidget.h"

#include <QVBoxLayout>
#include <QDateTime>
#include <QDebug>

FriendListWidget::FriendListWidget(QWidget *parent) : QWidget(parent)
{
    // 创建列表控件
    m_listWidget = new QListWidget(this);

    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置列表样式
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "border: none; "
        "background-color: white; "
        "outline: 0; "  // 移除焦点边框
        "}"
        "QListWidget::item { "
        "border: none; "
        "background-color: transparent; "  // 项背景透明，让自定义Widget处理
        "}"
        "QListWidget::item:selected { "
        "background-color: transparent; "  // 选中状态也透明
        "}"
        );

    // 布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listWidget);

    // 连接信号
    connect(m_listWidget, &QListWidget::itemClicked, this, &FriendListWidget::onItemClicked);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &FriendListWidget::onItemDoubleClicked);


    m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_listWidget, &QListWidget::customContextMenuRequested, this, &FriendListWidget::signalSender);
}


void FriendListWidget::setFriends(const QVector<QString> &friends, const QString& type)
{
    if(m_userId.isEmpty())
    {
        qDebug() << "void FriendListWidget::setFriends(const QVector<QString> &friends,const QString& type):未设置userId";
        return;
    }

    this->opera_type = e_Friends;
    m_listWidget->clear();
    m_itemUserMap.clear();
    if(friends.isEmpty()) return;

    // 创建好友和时间戳的映射
    QVector<QPair<User_ServiceZbk*, QDateTime>> friendTimeList;

    for (const auto &str : friends) {
        User_ServiceZbk* user = UsersZbk::FindUser_IdService(str, type);
        if (user == NULL) continue;

        ChatSessionZbk* session = ChatSessionZbk::getOrCreatePrivateSession(m_userId, user->getId(), user->getType());
        QDateTime lastTime;

        // 获取会话的最后一条消息时间
        if (!session->getMessages().isEmpty()) {
            lastTime = session->getMessages().back().getTimestamp();
        } else {
            // 如果没有消息，使用一个很早的时间
            lastTime = QDateTime::fromMSecsSinceEpoch(0);
        }

        friendTimeList.append(qMakePair(user, lastTime));
    }

    // 按照最后消息时间降序排序（最新的在前面）
    std::sort(friendTimeList.begin(), friendTimeList.end(),
              [](const QPair<User_ServiceZbk*, QDateTime>& a,
                 const QPair<User_ServiceZbk*, QDateTime>& b) {
                  return a.second > b.second;
              });

    // 按照排序后的顺序创建列表项
    for (const auto &pair : friendTimeList) {
        User_ServiceZbk* user = pair.first;
        ChatSessionZbk* session = ChatSessionZbk::getOrCreatePrivateSession(m_userId, user->getId(), user->getType());

        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 70));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        // 创建自定义Widget
        FriendItemWidget *widget = new FriendItemWidget;

        // 设置Widget内容
        widget->setAvatar(QPixmap(user->getAvatar()));
        widget->setNickname(user->getName());

        // 设置最后一条消息内容
        QString status = "";
        if (!session->getMessages().isEmpty()) {
            status = session->getMessages().back().getContent();
        }
        widget->setStatus(status);

        // 设置最后消息时间
        QDateTime lastTime = pair.second;
        QString timeText = "";
        if (lastTime.isValid() && lastTime.toMSecsSinceEpoch() > 0)
        {
            timeText=formatLastMessageTime(lastTime);
        }
        widget->setLastTime(timeText);

        // 将Widget设置为项的Widget
        m_listWidget->setItemWidget(item, widget);

        // 保存项与用户的映射
        m_itemUserMap[item] = user;
    }
}


bool FriendListWidget::setGroups(const QVector<QString> &groups, const QString& type)
{
    this->opera_type = e_Groups;
    bool res = true;
    if(!ServicesZbk::checkService(type)) return false;

    m_listWidget->clear();
    m_itemGroupMap.clear();
    if(groups.isEmpty()) return false;

    // 创建群组和时间戳的映射
    QVector<QPair<GroupsZbk*, QDateTime>> groupTimeList;

    for(const auto &str : groups)
    {
        qDebug() << QString("setting group %1 ...").arg(str);
        GroupsZbk* group = groupFactory::FindGroup_IdService(str, type);
        if(group == NULL)
        {
            res = false;
            qDebug() << "failed:" << str << Qt::endl;
            continue;
        }

        // 获取群组会话的最后一条消息时间
        QDateTime lastTime;
        QVector<QString> members = group->getMembers();
        ChatSessionZbk* session = ChatSessionZbk::getOrCreateGroupSession(group->getId(), members, type);

        if (!session->getMessages().isEmpty()) {
            lastTime = session->getMessages().back().getTimestamp();
        } else {
            // 如果没有消息，使用群组创建时间或一个很早的时间
            if (group->createDate().isValid()) {
                lastTime = QDateTime(group->createDate(), QTime(0, 0));
            } else {
                lastTime = QDateTime::fromMSecsSinceEpoch(0);
            }
        }

        groupTimeList.append(qMakePair(group, lastTime));
    }

    // 按照最后消息时间降序排序（最新的在前面）
    std::sort(groupTimeList.begin(), groupTimeList.end(),
              [](const QPair<GroupsZbk*, QDateTime>& a,
                 const QPair<GroupsZbk*, QDateTime>& b) {
                  return a.second > b.second;
              });

    // 按照排序后的顺序创建列表项
    for(const auto &pair : groupTimeList)
    {
        GroupsZbk* group = pair.first;
        QDateTime lastTime = pair.second;
        QVector<QString> members = group->getMembers();
        ChatSessionZbk* session = ChatSessionZbk::getOrCreateGroupSession(group->getId(), members, type);

        QListWidgetItem* item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 70));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        // 创建自定义Widget
        FriendItemWidget *widget = new FriendItemWidget;

        // 设置Widget内容
        widget->setAvatar(QPixmap(group->getAvatar()));
        QString name=group->getName();
        if(name.length()>=10)
        {
            name.resize(10);
            name.append("...");
        }
        name.append("("+QString::number(group->getSize())+")");

        widget->setNickname(name);

        // 设置最后一条消息内容
        QString status = "";
        if (!session->getMessages().isEmpty()) {
            User_ServiceZbk* sender=User_ServiceZbk::findUser_IdService(session->getMessages().back().getSenderId(),group->getServiceType());
            if(sender)
            {
                status =sender->getName()+" : "+session->getMessages().back().getContent();
            }
            else
                status ="未知用户 : "+session->getMessages().back().getContent();

        }
        widget->setStatus(status);

        // 设置最后消息时间
        QString timeText = formatLastMessageTime(lastTime);
        widget->setLastTime(timeText);

        // 将Widget设置为项的Widget
        m_listWidget->setItemWidget(item, widget);

        // 保存项与群组的映射
        m_itemGroupMap[item] = group;
    }

    return res;
}

void FriendListWidget::setSearchFriends(const QMap<QString,QString> &friends, const QString &type)
{
    if(opera_type!=e_createGroups)
        opera_type=e_SearchingFriends;
    m_listWidget->clear();
    m_itemUserMap.clear();
    if(friends.isEmpty())return;
    for (auto ad_pair =friends.constBegin();ad_pair!=friends.constEnd();ad_pair++) {
        User_ServiceZbk* user = UsersZbk::FindUser_IdService(ad_pair.key(), type);
        if (user == NULL) continue;
        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 70)); // 设置项的大小
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        // 创建自定义Widget
        searchingItemsZbk *widget = new searchingItemsZbk;
        // 设置Widget内容
        widget->setAvatar(QPixmap(user->getAvatar()));
        widget->setNickname(user->getName());
        widget->setId(ad_pair.value());
        // 将Widget设置为项的Widget
        m_listWidget->setItemWidget(item, widget);
        // 保存项与用户的映射
        m_itemUserMap[item] = user;
    }
}

void FriendListWidget::setSearchFriends(const QVector<QString> &friends, const QString &type)
{
    if(opera_type!=e_createGroups)
        opera_type=e_SearchingFriends;
    m_listWidget->clear();
    m_itemUserMap.clear();
    if(friends.isEmpty())return;
    for (auto ad_pair =friends.constBegin();ad_pair!=friends.constEnd();ad_pair++) {
        User_ServiceZbk* user = UsersZbk::FindUser_IdService(*ad_pair, type);
        if (user == NULL) continue;
        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 70)); // 设置项的大小
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        // 创建自定义Widget
        searchingItemsZbk *widget = new searchingItemsZbk;
        // 设置Widget内容
        widget->setAvatar(QPixmap(user->getAvatar()));
        widget->setNickname(user->getName());
        widget->setId(*ad_pair);
        // 将Widget设置为项的Widget
        m_listWidget->setItemWidget(item, widget);
        // 保存项与用户的映射
        m_itemUserMap[item] = user;
    }
}

void FriendListWidget::setMutiMode()
{
    m_listWidget->setSelectionMode(QAbstractItemView::MultiSelection); // 改为 ExtendedSelection
    m_listWidget->setSelectionBehavior(QAbstractItemView::SelectItems);   // 选择项

    // 设置列表样式
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "border: none; "
        "background-color: white; "
        "outline: 0; "  // 移除焦点边框
        "}"
        "QListWidget::item { "
        "border: none; "
        "background-color: transparent; "  // 项背景透明，让自定义Widget处理
        "}"
        "QListWidget::item:selected { "
        "background-color: #e3f2fd; "  // 选中状态背景色（浅蓝色）
        "border: 1px solid #bbdefb; "  // 选中状态边框
        "}"
        );
    this->opera_type=e_createGroups;
    connect(m_listWidget,&QListWidget::itemSelectionChanged,this,&FriendListWidget::selectionChanged);

}

void FriendListWidget::setItemKinds(ItemKinds kind)
{
    this->opera_type=kind;
}

void FriendListWidget::setManageMembers(const QVector<QString> &friends, const QString &type, const ItemKinds kind)
{
    opera_type=kind;
    m_listWidget->clear();
    m_itemUserMap.clear();
    if(friends.isEmpty())return;


    for(const QString &str:friends)
    {
        User_ServiceZbk* user=UsersZbk::FindUser_IdService(str,type);
        if(user==NULL)continue;
        QListWidgetItem* item=new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200,50));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        // 创建自定义Widget
        searchingItemsZbk *widget = new searchingItemsZbk;
        // 设置Widget内容
        widget->setAvatar(QPixmap(user->getAvatar()));
        widget->setNickname(user->getName());
        widget->setId("");
        // 将Widget设置为项的Widget
        m_listWidget->setItemWidget(item, widget);
        // 保存项与用户的映射
        m_itemUserMap[item] = user;

    }


}

User_ServiceZbk *FriendListWidget::getUserOnPos(const QPoint &q)
{
    QListWidgetItem* item=m_listWidget->itemAt(q);
    if(item==NULL)
    {
        qDebug()<<"User_ServiceZbk *FriendListWidget::getUserOnPos(const QPoint &q):未找到坐标对应项";
        return NULL;
    }
    if(!m_itemUserMap.contains(item))
    {
        qDebug()<<"User_ServiceZbk *FriendListWidget::getUserOnPos(const QPoint &q):未找到项对应好友";
        return NULL;
    }
    return m_itemUserMap[item];
}

QMap<QListWidgetItem *, User_ServiceZbk *> FriendListWidget::getitemUserMap() const
{
    return m_itemUserMap;
}

QMap<QListWidgetItem *, GroupsZbk *> FriendListWidget::getitemGroupMap() const
{
    return m_itemGroupMap;
}

int FriendListWidget::getSize() const
{
    switch (opera_type)
    {
    case e_Friends:
    case e_SearchingFriends:
    case e_ManageGroupMembers:
    case e_createGroups:
        return m_itemUserMap.size();
        break;
    case e_Groups:
        return m_itemGroupMap.size();
        break;
    case e_SystemNotices:
        if(m_systemnotice_isNull==true)return 0;
        return m_itemSystemNoticeMap.size();
        break;
    default:
        return 0;
        break;
    }
}






void FriendListWidget::onItemClicked(QListWidgetItem *item)
{
    if (m_itemUserMap.contains(item)) {
        // 发出点击信号
        if(opera_type==e_Friends)
            emit friendClicked(m_itemUserMap[item]);
        else if(opera_type==e_SearchingFriends)
            emit SearchfriendClicked(m_itemUserMap[item]);

        else
            qDebug()<<"void FriendListWidget::onItemClicked:not a user?"<<Qt::endl;

        qDebug() << "好友被点击:" << m_itemUserMap[item]->getName();
    }
    else if(m_itemGroupMap.contains(item))
    {
        if(opera_type==e_Groups)
        {
            emit groupClicked(m_itemGroupMap[item]);
        }
    }
}

void FriendListWidget::onItemDoubleClicked(QListWidgetItem *item)
{
    if (m_itemUserMap.contains(item)) {
        qDebug() << "好友被双击:" << m_itemUserMap[item]->getName();
    }
}

void FriendListWidget::selectionChanged()
{
    QVector<User_ServiceZbk*>selectedUser;

    if(opera_type!=e_createGroups)return;
    QList<QListWidgetItem*>items=m_listWidget->selectedItems();
    for(auto item:std::as_const(items))
    {
        if(m_itemUserMap.contains(item))
        {
            selectedUser.push_back(m_itemUserMap[item]);
        }
    }
    emit updateSelection(selectedUser);

}

// void FriendListWidget::signalSender(const QPoint &p)
// {
//     switch(opera_type)
//     {
//     case e_ManageGroupMembers:
//         emit GroupMemberRightClicked(m_listWidget->mapToGlobal(p),getUserOnPos(p));
//         break;
//     default:
//         break;
//     }
// }

QString FriendListWidget::userId() const
{
    return m_userId;
}

void FriendListWidget::setUserId(const QString &newUserId)
{
    m_userId = newUserId;
}

QString FriendListWidget::formatLastMessageTime(const QDateTime& dateTime)
{
    if (!dateTime.isValid() || dateTime.toMSecsSinceEpoch() == 0) {
        return "";
    }

    QDateTime now = QDateTime::currentDateTime();
    if (dateTime.date() == now.date()) {
        // 今天显示时间
        return dateTime.toString("hh:mm");
    } else if (dateTime.daysTo(now) == 1) {
        // 昨天显示"昨天"
        return "昨天";
    } else if (dateTime.daysTo(now) < 7) {
        // 一周内显示星期几
        return dateTime.toString("ddd");
    } else {
        // 更早显示日期
        return dateTime.toString("MM/dd");
    }
}


void FriendListWidget::setSystemNotices(UsersZbk* user)
{
    if (!user) {
        qDebug() << "FriendListWidget::setSystemNotices: 用户为空";
        return;
    }

    opera_type = e_SystemNotices;
    m_listWidget->clear();
    m_itemSystemNoticeMap.clear();

    // 收集所有服务的系统通知
    QVector<SystemNoticeItem> allNotices;

    // 遍历用户的所有服务
    for (const QString& serviceType : user->getIsusing().keys()) {
        if (!user->getIsusing()[serviceType]) continue;

        // 获取该服务的系统会话
        QVector<ChatSessionZbk*> systemSessions = user->getSystemSessions(serviceType);

        for (ChatSessionZbk* session : systemSessions) {
            if (!session) continue;

            // 获取会话中的所有消息
            QVector<ChatMessageZbk> messages = session->getMessages();

            for (const ChatMessageZbk& message : messages) {
                // 解析系统通知
                SystemNoticeItem notice = parseSystemNotice(message, serviceType);
                if (!notice.content.isEmpty()) {
                    allNotices.append(notice);
                }
            }
        }
    }

    // 按时间降序排序（最新的在前面）
    std::sort(allNotices.begin(), allNotices.end(),
              [](const SystemNoticeItem& a, const SystemNoticeItem& b) {
                  return a.timestamp > b.timestamp;
              });

    // 创建列表项
    for (const SystemNoticeItem& notice : allNotices) {
        QListWidgetItem* item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 80));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);


        FriendItemWidget* widget = new FriendItemWidget();

        // 设置Widget内容
        widget->setAvatar(QPixmap(":/pic/favicon.ico"));

        // 创建显示文本
        QString displayText = createSystemNoticeText(notice);
        widget->setNickname(displayText);

        // 设置状态（详细内容）
        widget->setStatus(notice.content);

        // 设置时间
        QString timeText = formatLastMessageTime(notice.timestamp);
        widget->setLastTime(timeText);

        // 根据处理状态设置不同样式
        if (notice.isProcessed) {
            // 已处理的申请使用灰色文本
            widget->setStyleSheet(
                "FriendItemWidget {"
                "   background-color: transparent;"
                "   border-bottom: 1px solid #eeeeee;"
                "}"
                "QLabel { color: #999999; }"
                );
        }

        m_listWidget->setItemWidget(item, widget);
        m_itemSystemNoticeMap[item] = notice;
    }

    if (allNotices.isEmpty()) {
        // 如果没有系统通知，显示提示
        QListWidgetItem* item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(200, 60));
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable); // 不可选择

        FriendItemWidget* widget = new FriendItemWidget();
        widget->setAvatar(QPixmap(":/pic/system_notification.ico"));
        widget->setNickname("暂无系统通知");
        widget->setStatus("您还没有收到任何系统通知");
        widget->setLastTime("");

        m_listWidget->setItemWidget(item, widget);
        m_systemnotice_isNull=true;
    }
    else
        m_systemnotice_isNull=false;

}

SystemNoticeItem FriendListWidget::parseSystemNotice(const ChatMessageZbk& message, const QString& serviceType)
{
    SystemNoticeItem notice;
    notice.serviceType = serviceType;
    notice.noticeId = message.getMessageId();
    notice.timestamp = message.getTimestamp();
    notice.isProcessed = false;

    QString content = message.getContent();

    // 解析JSON格式的系统通知
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &parseError);

    if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
        QJsonObject obj = doc.object();
        QString noticeType = obj["type"].toString();
        if (noticeType == "friend_request") {
            // 好友申请
            notice.senderId = obj["applicantId"].toString();
            notice.senderName = obj["applicantName"].toString();
            notice.requestId = obj["requestId"].toString();

            // 获取申请时间
            QString timestampStr = obj["timestamp"].toString();
            if (!timestampStr.isEmpty()) {
                notice.timestamp = QDateTime::fromString(timestampStr, Qt::ISODate);
            }

            // 如果有验证消息，添加到内容中
            QString verifyMessage = obj["verifyMessage"].toString();
            if (!verifyMessage.isEmpty()) {
                notice.content = QString("%1 申请添加您为好友：%2").arg(notice.senderName, verifyMessage);
            } else {
                notice.content = QString("%1 申请添加您为好友").arg(notice.senderName);
            }

            // 申请时间信息
            QString timeInfo = formatLastMessageTime(notice.timestamp);
            if (!timeInfo.isEmpty()) {
                notice.content += QString(" (%1)").arg(timeInfo);
            }
        }
        else if (noticeType == "friend_request_accepted") {
            // 好友申请被接受
            QString acceptorName = obj["acceptorName"].toString();
            notice.content = QString("%1 已同意您的好友申请").arg(acceptorName);
            notice.isProcessed = true;
        }
        else if (noticeType == "friend_request_rejected") {
            // 好友申请被拒绝
            QString rejectorName = obj["rejectorName"].toString();
            notice.content = QString("%1 拒绝了您的好友申请").arg(rejectorName);
            notice.isProcessed = true;
        }
        else {
            // 其他类型的系统通知
            notice.content = content;
        }
    }
    else {
        // 如果不是JSON格式，直接显示内容
        notice.content = content;
    }

    return notice;
}

// 创建系统通知项的显示文本
QString FriendListWidget::createSystemNoticeText(const SystemNoticeItem& notice)
{
    QString serviceName = ServicesZbk::toZhcn.value(notice.serviceType, notice.serviceType);

    if (!notice.senderName.isEmpty()) {
        return QString("[%1] %2").arg(serviceName, notice.senderName);
    } else {
        return QString("[%1] 系统通知").arg(serviceName);
    }
}

// 获取位置对应的系统通知
SystemNoticeItem FriendListWidget::getSystemNoticeOnPos(const QPoint& pos)
{
    QListWidgetItem* item = m_listWidget->itemAt(pos);
    if (item && m_itemSystemNoticeMap.contains(item)) {
        return m_itemSystemNoticeMap[item];
    }
    return SystemNoticeItem();
}

void FriendListWidget::signalSender(const QPoint &p)
{
    switch(opera_type)
    {
    case e_ManageGroupMembers:
        emit GroupMemberRightClicked(m_listWidget->mapToGlobal(p), getUserOnPos(p));
        break;
    case e_SystemNotices:
    {
        SystemNoticeItem notice = getSystemNoticeOnPos(p);
        if (!notice.senderId.isEmpty() && !notice.isProcessed) {
            emit SystemNoticeRightClicked(m_listWidget->mapToGlobal(p), notice);
        }
    }
    break;
    default:
        break;
    }
}
