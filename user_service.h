#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <QObject>
#include <QVector>
#include <QMap>
#include "qdatetime.h"
#include "serviceszbk.h"
enum Gender
{
    Male,
    Female,
    Notset
};


class DataManagerZbk;



class User_ServiceZbk
{
    friend class UsersZbk;
    friend class DataManagerZbk;
public:
    User_ServiceZbk();
    User_ServiceZbk(QString type,QString name,QString pass,QString mainId);
    ~User_ServiceZbk();
    QString getName()const;
    QString getType()const;
    QString getAvatar()const;
    QString getId()const;
    bool getOnline()const;

    void setIsOnline(bool state);
    void setAvator(QString);
    void setMainId(QString id);


    QVector<QString>m_friends;
    QVector<QString>m_groups;
    static QMap<QString,QVector<User_ServiceZbk*>>allServiceUsers;
    static User_ServiceZbk* findUser_IdService(const QString& id,const QString& service);
    QString getmainId() const;

    QDate registerDate() const;
    void setRegisterDate(const QDate &newRegisterDate);

    int age() const;
    void setAge(int newAge);

    Gender gender() const;
    void setGender(Gender newGender);
    static QString GendertoString(Gender g);
    QString Location() const;
    void setLocation(const QString &newLocation);


    QDate birthday() const;
    void setBirthday(const QDate &newBirthday);

    void setNickName(const QString &newNickName);

    QString Pass() const;
    void setPass(const QString &newPass);
    static const QString SYSTEM_USER_ID;

private:

    QString m_nickName;
    QString m_Pass;
    QString m_serType;
    QString m_avatarPath;
    QString m_Id;
    QString m_Location;
    bool m_isOnline=false;
    QString m_mainId;
    QDate m_registerDate;
    int m_age;
    Gender m_gender;
    QDate m_birthday;





    QString generateId()const;
    void setId(QString);



    static void initializeSystemUser();
};

#endif // USER_SERVICE_H
