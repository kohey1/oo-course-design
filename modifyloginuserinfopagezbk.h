#ifndef MODIFYLOGINUSERINFOPAGEZBK_H
#define MODIFYLOGINUSERINFOPAGEZBK_H

#include <QDialog>
#include "userszbk.h"
namespace Ui {
class modifyLoginUserInfoPageZbk;
}

class modifyLoginUserInfoPageZbk : public QDialog
{
    Q_OBJECT

    enum modifyTarget
    {
        MY_SERVICE,
        MY_UNITENCENT
    };

public:
    explicit modifyLoginUserInfoPageZbk(User_ServiceZbk* user,QWidget *parent = nullptr);
    explicit modifyLoginUserInfoPageZbk(UsersZbk* mainuser,QWidget* parent=nullptr);
    ~modifyLoginUserInfoPageZbk();

private slots:
    void on_btn_confirmInfo_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::modifyLoginUserInfoPageZbk *ui;
    User_ServiceZbk* m_user;
    UsersZbk* m_mainUser;
    modifyTarget mode;

signals:
    void LoginUserInfoModified();
};

#endif // MODIFYLOGINUSERINFOPAGEZBK_H
