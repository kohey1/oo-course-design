#ifndef SEARCHPAGEZBK_H
#define SEARCHPAGEZBK_H

#include <QWidget>
#include <QFocusEvent>
#include "friendlistwidget.h"

namespace Ui {
class SearchPageZbk;
}

class SearchPageZbk : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPageZbk(UsersZbk* LoginUser,QString type,QWidget *parent = nullptr);
    ~SearchPageZbk();


    void initAdviceFriends();



    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    QString type() const;

private slots:
    void on_btn_topclose_clicked();

    void on_btn_topmin_clicked();

    void on_btnUser_clicked();

    void on_btnGroup_clicked();

    void on_btnSearch_User_clicked();

    void on_addUserButton_clicked();

    void on_btnSearch_group_clicked();

    void on_addGroupButton_clicked();
    void switchAdviced(User_ServiceZbk* user);

private:
    Ui::SearchPageZbk *ui;
    UsersZbk* m_user;
    QMap<QString,QString>m_adviceFriends;//<id,reason>
    QString m_type;
    bool m_topIsPressing;
    QPointF m_startPos;
    QPointF m_framePos;

    void initPages();

signals:
    void friendAdded(User_ServiceZbk* targetUser);
    void refreshPage();



};

#endif // SEARCHPAGEZBK_H
