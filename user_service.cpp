#include "user_service.h"

// User_ServiceZbk::User_ServiceZbk(QString type,QString name, QString pass,QString mainId):m_nickName(name),m_Pass(pass),m_serType(type),m_mainId(mainId)
// {
//     static bool systemInitialized = initializeSystemUser();
//     Q_UNUSED(systemInitialized);
//     m_avatarPath=":/pic/87D2B7BB181C40F83726AF5B40E793AC.ico";
//     m_Id=generateId();
//     allServiceUsers[type].push_back(this);
//     m_registerDate=QDate::currentDate();
//     m_gender=Notset;
//     m_Location="未设置";
//     m_birthday.setDate(2000,1,1);
//     m_age=m_birthday.daysTo(QDate::currentDate())/365;
// }

User_ServiceZbk::~User_ServiceZbk()
{
    m_nickName.clear();
    m_Pass.clear();

    m_avatarPath.clear();
    m_Id.clear();
    m_friends.clear();
    m_groups.clear();
    allServiceUsers[m_serType].removeAll(this);
    m_serType.clear();
}



const QString User_ServiceZbk::SYSTEM_USER_ID = "system_user";

// 独立的系统用户初始化方法
void User_ServiceZbk::initializeSystemUser()
{

    static bool initialized = false;
    if (initialized) return;

    // 为每个服务类型创建系统用户
    for (const auto& serviceType : {"QQ", "WeChat", "WeiBo"}) {
        if (!allServiceUsers.contains(serviceType)) {
            allServiceUsers[serviceType] = QVector<User_ServiceZbk*>();
        }

        // 检查是否已存在系统用户
        bool systemUserExists = false;
        for (auto user : allServiceUsers[serviceType]) {
            if (user->m_Id == SYSTEM_USER_ID) {
                systemUserExists = true;
                break;
            }
        }

        if (!systemUserExists) {
            // 创建系统用户 - 直接设置属性，避免递归
            User_ServiceZbk* systemUser = new User_ServiceZbk();

            // 直接设置属性，不通过构造函数
            systemUser->m_serType = serviceType;
            systemUser->m_nickName = "系统通知";
            systemUser->m_Pass = "system";
            systemUser->m_mainId = "system_master";
            systemUser->m_Id = SYSTEM_USER_ID;
            systemUser->m_avatarPath = ":/pic/system_notification.ico";
            systemUser->m_registerDate = QDate::currentDate();
            systemUser->m_gender = Notset;
            systemUser->m_Location = "系统";
            systemUser->m_birthday = QDate(2000, 1, 1);
            systemUser->m_age = systemUser->m_birthday.daysTo(QDate::currentDate()) / 365;
            systemUser->m_isOnline = true;

            allServiceUsers[serviceType].push_back(systemUser);
            qDebug() << "创建系统用户 for" << serviceType;
        }
    }

    initialized = true;
}

User_ServiceZbk::User_ServiceZbk(QString type,QString name, QString pass,QString mainId)
    : m_nickName(name), m_Pass(pass), m_serType(type), m_mainId(mainId)
{
    m_avatarPath=":/pic/87D2B7BB181C40F83726AF5B40E793AC.ico";
    m_Id=generateId();
    allServiceUsers[type].push_back(this);
    m_registerDate=QDate::currentDate();
    m_gender=Notset;
    m_Location="未设置";
    m_birthday.setDate(2000,1,1);
    m_age=m_birthday.daysTo(QDate::currentDate())/365;
}

// 添加默认构造函数（仅供系统用户使用）
User_ServiceZbk::User_ServiceZbk()
{
}

QString User_ServiceZbk::getName()const
{
    return m_nickName;
}

QString User_ServiceZbk::getType()const
{
    return m_serType;
}

QString User_ServiceZbk::getAvatar()const
{
    return m_avatarPath;
}

QString User_ServiceZbk::getId()const
{
    return m_Id;
}

void User_ServiceZbk::setIsOnline(bool state)
{
    m_isOnline=state;
}

bool User_ServiceZbk::getOnline()const
{
    return m_isOnline;
}

void User_ServiceZbk::setId(QString id)
{
    m_Id=id;
}

QString User_ServiceZbk::GendertoString(Gender g)
{
    switch (g)
    {
    case Male:
        return "男";
    case Female:
        return "女";
    case Notset:
        return "未设置";
    default:
        break;
    }
}

QString User_ServiceZbk::Location() const
{
    return m_Location;
}

void User_ServiceZbk::setLocation(const QString &newLocation)
{
    m_Location = newLocation;
}

QDate User_ServiceZbk::birthday() const
{
    return m_birthday;
}

void User_ServiceZbk::setBirthday(const QDate &newBirthday)
{
    m_birthday = newBirthday;
}

void User_ServiceZbk::setNickName(const QString &newNickName)
{
    m_nickName = newNickName;
}

QString User_ServiceZbk::Pass() const
{
    return m_Pass;
}

void User_ServiceZbk::setPass(const QString &newPass)
{
    m_Pass = newPass;
}

void User_ServiceZbk::setAvator(QString path)
{
    m_avatarPath=path;
}

void User_ServiceZbk::setMainId(QString id)
{
    m_mainId=id;
}

User_ServiceZbk *User_ServiceZbk::findUser_IdService(const QString &id,const QString& service)
{
    for(auto i:std::as_const(User_ServiceZbk::allServiceUsers[service]))
    {
        if(i->m_Id==id)return i;
    }
    qDebug()<<"未找到"<<"id"<<id<<",type"<<service<<"的用户"<<Qt::endl;
    return NULL;
}




QString User_ServiceZbk::generateId() const
{
    QString res;
    while(1)
    {
        if(res.isEmpty()==false) res.clear();
        for(int i=0;i<10;i++)
        {
            res.append(QString::number(rand()%10));
        }
        for(auto i:std::as_const(allServiceUsers[m_serType]))
        {
            if(i->m_Id==res)continue;
        }
        break;
    }
    return res;
}

QString User_ServiceZbk::getmainId() const
{
    return m_mainId;
}

QDate User_ServiceZbk::registerDate() const
{
    return m_registerDate;
}

void User_ServiceZbk::setRegisterDate(const QDate &newRegisterDate)
{
    m_registerDate = newRegisterDate;
}

int User_ServiceZbk::age() const
{
    return m_age;
}

void User_ServiceZbk::setAge(int newAge)
{
    m_age = newAge;
}

Gender User_ServiceZbk::gender() const
{
    return m_gender;
}

void User_ServiceZbk::setGender(Gender newGender)
{
    m_gender = newGender;
}

QMap<QString,QVector<User_ServiceZbk*>>User_ServiceZbk::allServiceUsers;
