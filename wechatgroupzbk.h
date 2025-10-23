#ifndef WECHATGROUPZBK_H
#define WECHATGROUPZBK_H
#include "groupszbk.h"
#include <QObject>

class WeChatGroupZbk:public GroupsZbk
{
    Q_OBJECT
public:
    WeChatGroupZbk();
    ~WeChatGroupZbk()=default;

    bool hasAdministrators() const override;
    bool allowSubGroups() const override;
};

#endif // WECHATGROUPZBK_H
