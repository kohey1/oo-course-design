#ifndef QQPAGEZBK_H
#define QQPAGEZBK_H

#include <QWidget>
#include <QPainter>
#include "creategroupzbk.h"
#include "searchpagezbk.h"
#include <QMenu>
namespace Ui {
class QQPageZbk;
}
class MainPageZbk;

class QQPageZbk : public QWidget
{
    Q_OBJECT
    friend class MainPageZbk;

public:
    explicit QQPageZbk(UsersZbk* Loginuser,QWidget *parent = nullptr);
    ~QQPageZbk();
protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_letSearch_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void reloadFriends();
    void oncreateGroupClicked();

private:

    Ui::QQPageZbk *ui;
    UsersZbk* user;
    SearchPageZbk* searchPage=NULL;
    createGroupZbk* createGroupPage=NULL;

signals:
    void addFriend(User_ServiceZbk* targetUser);



};

#endif // QQPAGEZBK_H
