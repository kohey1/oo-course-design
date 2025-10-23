#include "searchingitemszbk.h"
#include "ui_searchingitemszbk.h"

searchingItemsZbk::searchingItemsZbk(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::searchingItemsZbk)
{
    ui->setupUi(this);

}

searchingItemsZbk::~searchingItemsZbk()
{
    delete ui;
}

void searchingItemsZbk::setAvatar(const QPixmap &avatar)
{
    QPixmap circularAvatar(40, 40);
    circularAvatar.fill(Qt::transparent);

    QPainter painter(&circularAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(avatar.scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 40, 40);

    ui->lblAvator->setPixmap(circularAvatar);
}

void searchingItemsZbk::setNickname(const QString &nickname)
{
    ui->lbl_Name->setText(nickname);
}

void searchingItemsZbk::setId(const QString &id)
{
    ui->lbl_Id->setText(id);
}

void searchingItemsZbk::paintEvent(QPaintEvent *event)
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

void searchingItemsZbk::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = true;
    update(); // 触发重绘

    QWidget::enterEvent(event);
}

void searchingItemsZbk::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isHovered = false;
    update(); // 触发重绘

    QWidget::leaveEvent(event);
}

