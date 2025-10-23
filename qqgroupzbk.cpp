#include "qqgroupzbk.h"

QQGroupZbk::QQGroupZbk()
{

}

bool QQGroupZbk::hasAdministrators() const
{
    return true;
}




bool QQGroupZbk::allowSubGroups() const
{
    return true;
}



// bool QQGroupZbk::removeMember(const QString& id)
// {
//     if(m_members.contains(id))
//     {
//         m_members.removeAll(id);
//         User_ServiceZbk* user=User_ServiceZbk::findUser_IdService(id,m_serviceType);
//         if(!user)return false;
//         user->m_groups.removeAll(m_id);
//         return true;
//     }
//     return false;
// }

