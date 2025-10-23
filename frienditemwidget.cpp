// frienditemwidget.cpp
#include "frienditemwidget.h"
#include <QPainter>

FriendItemWidget::FriendItemWidget(QWidget *parent) : QWidget(parent),m_isHovered(false)
{
    // 创建控件（保持不变）
    m_avatarLabel = new QLabel(this);
    m_avatarLabel->setFixedSize(50, 50);
    m_avatarLabel->setScaledContents(true);
    m_avatarLabel->setStyleSheet("background-color:transparent");

    m_nicknameLabel = new QLabel(this);
    m_nicknameLabel->setStyleSheet("font-weight: bold; font-size: 14px; background-color: transparent;");

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: gray; font-size: 12px; background-color: transparent;");

    m_timeLabel = new QLabel(this);
    m_timeLabel->setStyleSheet("color: gray; font-size: 10px; background-color: transparent;");
    m_timeLabel->setAlignment(Qt::AlignRight);

    // 创建布局（保持不变）
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_avatarLabel);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(m_nicknameLabel);
    infoLayout->addWidget(m_statusLabel);
    infoLayout->setSpacing(2);

    mainLayout->addLayout(infoLayout);
    mainLayout->addWidget(m_timeLabel);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(10);

    // 设置样式字符串（重要修改）
    m_normalStyle =
        "FriendItemWidget {"
        "   background-color: transparent;"
        "   border-bottom: 1px solid #eeeeee;"
        "   border-radius: 0px;"
        "}";

    m_hoverStyle =
        "FriendItemWidget {"
        "   background-color: #f5f5f5;"
        "   border-bottom: 1px solid #eeeeee;"
        "   border-radius: 5px;"
        "}";

    // 应用初始样式
    this->setStyleSheet(m_normalStyle);

    // 确保启用鼠标跟踪和自动填充背景
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);

    // 设置背景角色，确保背景色可以显示
    this->setBackgroundRole(QPalette::Window);




}

void FriendItemWidget::setAvatar(const QPixmap &avatar)
{
    // 创建圆形头像
    QPixmap circularAvatar(50, 50);
    circularAvatar.fill(Qt::transparent);

    QPainter painter(&circularAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(avatar.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 50, 50);

    m_avatarLabel->setPixmap(circularAvatar);
}

void FriendItemWidget::setNickname(const QString &nickname)
{
    m_nicknameLabel->setText(nickname);
}

void FriendItemWidget::setStatus(const QString &status)
{
    m_statusLabel->setText(status);
}

void FriendItemWidget::setLastTime(const QString &time)
{
    m_timeLabel->setText(time);
}

void FriendItemWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    if (m_isHovered) {
        painter.fillRect(rect(), QColor(0xf5, 0xf5, 0xf5)); // 悬停背景色
    } else {
        painter.fillRect(rect(), Qt::transparent); // 正常状态透明
    }

    // 绘制底部边框
    painter.setPen(QColor(0xee, 0xee, 0xee));
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());

    // 确保子控件正常绘制
    QWidget::paintEvent(event);
}

void FriendItemWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = true;
    update(); // 触发重绘

    QWidget::enterEvent(event);
}

void FriendItemWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = false;
    update(); // 触发重绘

    QWidget::leaveEvent(event);
}
