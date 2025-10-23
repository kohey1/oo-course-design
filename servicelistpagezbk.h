#ifndef SERVICELISTPAGEZBK_H
#define SERVICELISTPAGEZBK_H

#include <QWidget>
#include <QButtonGroup>
#include <QGridLayout>
#include <QPushButton>

#include "addservicepagezbk.h"

class ServiceListPageZbk : public QWidget
{
    Q_OBJECT

public:
    explicit ServiceListPageZbk(UsersZbk* user, QWidget *parent = nullptr);
    ~ServiceListPageZbk();

    // 刷新界面
    void refreshUI();
    void createServiceButtonRows(const QList<ServicesZbk*>& services,  QVBoxLayout* containerLayout, QButtonGroup* buttonGroup, bool isEnabled);
    void clearLayout(QLayout* layout);
private:
    void setupUI();
    void populateButtonGroups();
    void clearLayoutWithoutDeletingWidgets(QLayout* layout);
    QPushButton* createServiceButton(ServicesZbk* service);

    UsersZbk* m_user;
    QButtonGroup* m_enabledGroup;
    QButtonGroup* m_disabledGroup;
    QVBoxLayout* m_enabledLayout;
    QVBoxLayout* m_disabledLayout;
    QWidget* m_enabledWidget;
    QWidget* m_disabledWidget;

private slots:
    void onServiceButtonClicked(QAbstractButton* button);
signals:
    void service_changed();
};

#endif // SERVICELISTPAGEZBK_H
