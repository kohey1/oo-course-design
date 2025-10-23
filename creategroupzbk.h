#ifndef CREATEGROUPZBK_H
#define CREATEGROUPZBK_H

#include <QDialog>
#include "friendlistwidget.h"

namespace Ui {
class createGroupZbk;
}

class createGroupZbk : public QDialog
{
    Q_OBJECT

public:
    explicit createGroupZbk(UsersZbk* LoginUser,QString type,QWidget *parent = nullptr);
    createGroupZbk(UsersZbk* LoginUser,QString type,QVector<QString>targetMember,QWidget *parent = nullptr);
    ~createGroupZbk();

private slots:
    void on_btn_Cancel_clicked();
    void updateSelection(QVector<User_ServiceZbk*>selectedUsers);

    void on_btn_Confirm_clicked();

private:

    Ui::createGroupZbk *ui;
    UsersZbk* m_user;
    QString m_type;

    QVector<User_ServiceZbk*>member;
signals:
    void groupCreated();

};

#endif // CREATEGROUPZBK_H
