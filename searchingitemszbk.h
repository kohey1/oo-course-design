#ifndef SEARCHINGITEMSZBK_H
#define SEARCHINGITEMSZBK_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QEnterEvent>


namespace Ui {
class searchingItemsZbk;
}

class searchingItemsZbk : public QWidget
{
    Q_OBJECT

public:
    explicit searchingItemsZbk(QWidget *parent = nullptr);
    ~searchingItemsZbk();
    void setAvatar(const QPixmap &avatar);
    void setNickname(const QString &nickname);
    void setId(const QString& id);
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);

private:
    Ui::searchingItemsZbk *ui;
    bool m_isHovered=false;
};

#endif // SEARCHINGITEMSZBK_H
