#ifndef QQGROUPZBK_H
#define QQGROUPZBK_H

#include <QObject>
#include "groupszbk.h"

class QQGroupZbk:public GroupsZbk
{
    Q_OBJECT
public:
    QQGroupZbk();

    ~QQGroupZbk()=default;


    bool hasAdministrators() const override ;

    //bool removeMember(const QString& userId)override;
    bool allowSubGroups() const override;








};

#endif // QQGROUPZBK_H
