#ifndef SERVICESZBK_H
#define SERVICESZBK_H
#include <QString>
#include <QVector>
#include <QMap>
#include "windowbutton.h"
#include <QMessageBox>

class ServicesZbk
{
public:
    ServicesZbk(QString,QString,QString,QString id_cn="null");
    ~ServicesZbk();
    QString using_Icon;
    QString offLine_Icon;
    QString Id;
    QString Id_cn;
    static QVector<ServicesZbk*>allServices;
    static bool checkService(const QString& type);
    static QMap<QString,QString>toZhcn;
    static QMap<QString,QString>toEng;
    static bool hasAdministors(const QString& type);

};

#endif // SERVICESZBK_H
