#ifndef WEIBOPAGEZBK_H
#define WEIBOPAGEZBK_H

#include <QWidget>
#include <QMenu>
#include <QPainter>
#include "searchpagezbk.h"
#include "creategroupzbk.h"

namespace Ui {
class WeiBoPageZbk;
}
class MainPageZbk;

class WeiBoPageZbk : public QWidget
{
    Q_OBJECT
    friend class MainPageZbk;

public:
    explicit WeiBoPageZbk(UsersZbk *LoginUser, QWidget *parent = nullptr);
    ~WeiBoPageZbk();
    void paintEvent(QPaintEvent *event);
    void reloadFriends();


private slots:
    void on_letSearch_textChanged(const QString &arg1);
    void on_addButton_clicked();



private:
    Ui::WeiBoPageZbk *ui;
    UsersZbk* user;
    SearchPageZbk* searchPage=NULL;
signals:
    void addFriend(User_ServiceZbk* targetUser);

};

#endif // WEIBOPAGEZBK_H
