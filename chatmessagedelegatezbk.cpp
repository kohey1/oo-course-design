#include "chatmessagedelegatezbk.h"
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>

ChatMessageDelegateZbk::ChatMessageDelegateZbk(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void ChatMessageDelegateZbk::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 获取数据
    QString senderName = index.data(ChatMessageModelZbk::SenderNameRole).toString();
    QString content = index.data(ChatMessageModelZbk::ContentRole).toString();
    QString timestamp = index.data(ChatMessageModelZbk::TimestampRole).toString();
    QString avatarPath = index.data(ChatMessageModelZbk::AvatarRole).toString();
    bool isOwnMessage = index.data(ChatMessageModelZbk::IsOwnMessageRole).toBool();

    QRect rect = option.rect;


    // 设置背景色 - 通过颜色区分自己的消息
    QColor backgroundColor = isOwnMessage ? QColor(240, 248, 255) : QColor(255, 255, 255);
    if (option.state & QStyle::State_Selected) {
        backgroundColor = option.palette.highlight().color();
    }

    painter->fillRect(rect, backgroundColor);

    // 绘制头像（始终在左侧）
    QPixmap avatar(avatarPath);
    if (avatar.isNull()) {
        avatar = QPixmap(":/pic/default_avatar.png");
    }

    QRect avatarRect(rect.left() + MARGIN,
                     rect.top() + MARGIN,
                     AVATAR_SIZE, AVATAR_SIZE);

    painter->drawPixmap(avatarRect, avatar.scaled(AVATAR_SIZE, AVATAR_SIZE,
                                                  Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 计算文本区域（头像右侧）
    int textAreaLeft = avatarRect.right() + MARGIN;
    int textAreaWidth = rect.width() - textAreaLeft - MARGIN;

    // 绘制发件人名称和消息内容区域
    QRect nameRect(textAreaLeft, rect.top() + MARGIN,
                   textAreaWidth, 20);

    // 绘制发件人名称 - 如果是自己的消息，使用不同颜色
    painter->setPen(isOwnMessage ? QColor(0, 100, 200) : QColor(0, 0, 0));
    QFont nameFont = option.font;
    nameFont.setBold(true);
    painter->setFont(nameFont);
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignTop, senderName);

    // 绘制时间 - 使用较小字体和灰色
    painter->setPen(QColor(150, 150, 150));
    QFont timeFont = option.font;
    timeFont.setPointSize(9);
    painter->setFont(timeFont);

    QRect timeRect(textAreaLeft, rect.top() + MARGIN,
                   textAreaWidth - 10, 20);
    painter->drawText(timeRect, Qt::AlignRight | Qt::AlignTop, timestamp);

    // 绘制消息内容
    QRect contentRect(textAreaLeft, nameRect.bottom() + SPACING,
                      textAreaWidth, rect.height() - nameRect.height() - 2 * SPACING - MARGIN);

    painter->setPen(Qt::black);
    painter->setFont(option.font);
    painter->drawText(contentRect, Qt::TextWordWrap, content);

    // 绘制分隔线
    painter->setPen(QColor(230, 230, 230));
    painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

    painter->restore();
}

QSize ChatMessageDelegateZbk::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    Q_UNUSED(option)

    if (!index.isValid())
        return QSize(0, 0);

    QString content = index.data(ChatMessageModelZbk::ContentRole).toString();

    QFontMetrics metrics(option.font);
    int textWidth = option.rect.width() - AVATAR_SIZE - 3 * MARGIN;
    QRect textRect = metrics.boundingRect(0, 0, textWidth, 0,
                                          Qt::TextWordWrap, content);

    //总高度=头像高度+边距+文本高度+额外空间（姓名、时间）
    int height = qMax(AVATAR_SIZE + 2 * MARGIN, textRect.height() + 50);

    return QSize(option.rect.width(), height);
}
