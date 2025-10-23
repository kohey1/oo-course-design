#ifndef CHATMESSAGEDELEGATEZBK_H
#define CHATMESSAGEDELEGATEZBK_H


#include"chatmessagemodelzbk.h"
#include <QStyledItemDelegate>

class ChatMessageDelegateZbk : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ChatMessageDelegateZbk(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    static const int AVATAR_SIZE = 40;
    static const int MARGIN = 8;
    static const int SPACING = 4;
};

#endif // CHATMESSAGEDELEGATEZBK_H
