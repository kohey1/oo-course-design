#ifndef ADDSERVICEPAGEZBK_H
#define ADDSERVICEPAGEZBK_H

#include <QWidget>
#include <QObject>
#include "userszbk.h"
namespace Ui {
class AddServicePageZbk;
}

class AddServicePageZbk : public QWidget
{
    Q_OBJECT

public:
    explicit AddServicePageZbk(QString type,UsersZbk* user,QWidget *parent = nullptr);
    ~AddServicePageZbk();

private slots:
    void on_btnFast_clicked();

    void on_btnRegister_clicked();
signals:
    void register_success();
private:
    Ui::AddServicePageZbk *ui;
    UsersZbk* user1;
    QString serviceType;
};

#endif // ADDSERVICEPAGEZBK_H
