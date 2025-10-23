#ifndef WINDOWBUTTON_H
#define WINDOWBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QColor>
#include <QEvent>
#include <QPaintEvent>

class windowButton : public QPushButton
{
    Q_OBJECT
public:
    explicit windowButton(QWidget* parent = nullptr);  // 添加 explicit 和默认参数

    void setBaseColor(const QColor &color);
    void setHoverColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;

private:
    QColor baseColor;
    QColor hoverColor;
    bool m_isHovered = false;  // 初始化成员变量
};

#endif // WINDOWBUTTON_H
