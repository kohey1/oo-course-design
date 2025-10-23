// frienditemwidget.h
#ifndef FRIENDITEMWIDGET_H
#define FRIENDITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEnterEvent>

class FriendItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FriendItemWidget(QWidget *parent = nullptr);
    void setAvatar(const QPixmap &avatar);
    void setNickname(const QString &nickname);
    void setStatus(const QString &status);
    void setLastTime(const QString &time);

protected:
    void enterEvent(QEnterEvent *event) override;    // 鼠标进入事件
    void leaveEvent(QEvent *event) override;         // 鼠标离开事件
    void paintEvent(QPaintEvent *event) override;
private:
    QLabel *m_avatarLabel;
    QLabel *m_nicknameLabel;
    QLabel *m_statusLabel;
    QLabel *m_timeLabel;

    QString m_normalStyle;    // 正常状态样式
    QString m_hoverStyle;     // 悬停状态样式
    bool m_isHovered;
};

#endif // FRIENDITEMWIDGET_H
