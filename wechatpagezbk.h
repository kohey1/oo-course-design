#ifndef WECHATPAGEZBK_H
#define WECHATPAGEZBK_H

#include <QWidget>
#include <QMenu>
#include <QPainter>
#include "searchpagezbk.h"
#include "creategroupzbk.h"
namespace Ui {
class WeChatPageZbk;
}
class MainPageZbk;
class WeChatPageZbk : public QWidget
{
    Q_OBJECT
    friend class MainPageZbk;
public:
    explicit WeChatPageZbk(UsersZbk* LoginUser,QWidget *parent = nullptr);
    ~WeChatPageZbk();
protected:
    void paintEvent(QPaintEvent* event)override;

private slots:
    void on_letSearch_textChanged(const QString &arg1);

    void on_addButton_clicked();
    void reloadFriends();
    void oncreateGroupClicked();
private:
    Ui::WeChatPageZbk *ui;
    UsersZbk* user;
    SearchPageZbk* searchPage=NULL;
    createGroupZbk* createGroupPage=NULL;
signals:
    void addFriend(User_ServiceZbk* targetUser);



};

#endif // WECHATPAGEZBK_H
