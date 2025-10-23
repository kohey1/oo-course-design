#include "windowbutton.h"
#include <QPainter>
#include <QEnterEvent>

windowButton::windowButton(QWidget *parent)
    : QPushButton(parent)  // 正确初始化父类
{
    baseColor = Qt::transparent;
    hoverColor = Qt::gray;
    m_isHovered = false;

    // 设置一些默认属性确保可见
    setText("Button");      // 默认文本
    //setMinimumSize(10, 30); // 最小尺寸
}

void windowButton::setBaseColor(const QColor& color)
{
    this->baseColor = color;
    update();
}

void windowButton::setHoverColor(const QColor &color)
{
    this->hoverColor = color;
    update();
}

void windowButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    QColor currentColor = m_isHovered ? hoverColor : baseColor;
    painter.setBrush(currentColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    // 绘制文本
    painter.setPen(Qt::black); // 设置文本颜色
    painter.drawText(rect(), Qt::AlignCenter, text());
}

void windowButton::enterEvent(QEnterEvent *e)
{
    m_isHovered = true;
    update();
    QPushButton::enterEvent(e); // 调用基类实现
}

void windowButton::leaveEvent(QEvent *e)
{
    m_isHovered = false;
    update();
    QPushButton::leaveEvent(e); // 调用基类实现
}





