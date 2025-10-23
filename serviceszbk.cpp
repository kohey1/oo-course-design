#include "serviceszbk.h"

ServicesZbk::ServicesZbk(QString use, QString off, QString id,QString id_cn):using_Icon(use),offLine_Icon(off),Id(id)
{
    allServices.push_back(this);
    if(id_cn=="null")Id_cn=id;
    else Id_cn=id_cn;
    toZhcn[id]=Id_cn;
    toEng[Id_cn]=id;
}

QMap<QString,QString>ServicesZbk::toZhcn;
QMap<QString,QString>ServicesZbk::toEng;

ServicesZbk::~ServicesZbk()
{
    using_Icon.clear();
    offLine_Icon.clear();
    Id.clear();
    Id_cn.clear();
    allServices.removeAll(this);
}

bool ServicesZbk::checkService(const QString &type)
{
    for(auto i:std::as_const(allServices))
    {
        if(i->Id==type)return true;
    }
    qDebug()<<type<<"不是已加入的服务id!"<<Qt::endl;
    return false;
}

bool ServicesZbk::hasAdministors(const QString &type)
{
    if(type=="QQ")return true;
    else if(type=="WeChat")return false;
    else return true;
}


QVector<ServicesZbk*>ServicesZbk::allServices;

