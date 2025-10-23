#ifndef USERINFOPAGEZBK_H
#define USERINFOPAGEZBK_H

#include <QDialog>
#include "userszbk.h"

namespace Ui {
class UserInfoPageZbk;
}

class UserInfoPageZbk : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoPageZbk(UsersZbk* own,User_ServiceZbk* user,QWidget *parent = nullptr);
    explicit UserInfoPageZbk(UsersZbk* own,QWidget *parent = nullptr);
    ~UserInfoPageZbk();

    void setUserInfoPage();
    void setMainUserInfoPage();


signals:
    void sendMessage(User_ServiceZbk* user);
    void addFriend(User_ServiceZbk* user);
    void modifyUserInfo();
    void modifyMainUserInfo();





private:
    Ui::UserInfoPageZbk *ui;
    User_ServiceZbk* m_user;
    UsersZbk* m_own;
};

#endif // USERINFOPAGEZBK_H
