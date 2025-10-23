#include "servicelistpagezbk.h"
#include "qregularexpression.h"
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

ServiceListPageZbk::ServiceListPageZbk(UsersZbk* user, QWidget *parent)
    : QWidget(parent), m_user(user)
{
    // 初始化按钮组
    m_enabledGroup = new QButtonGroup(this);
    m_disabledGroup = new QButtonGroup(this);

    // 设置互斥性为false，允许同时选择多个按钮
    m_enabledGroup->setExclusive(false);
    m_disabledGroup->setExclusive(false);

    // 连接按钮点击信号
    connect(m_enabledGroup, &QButtonGroup::buttonClicked, this, &ServiceListPageZbk::onServiceButtonClicked);
    connect(m_disabledGroup, &QButtonGroup::buttonClicked, this, &ServiceListPageZbk::onServiceButtonClicked);

    setupUI();
    populateButtonGroups();
}

ServiceListPageZbk::~ServiceListPageZbk()
{
    // 不需要手动删除按钮组，Qt的父子对象系统会自动处理
}



void ServiceListPageZbk::onServiceButtonClicked(QAbstractButton* button)
{
    // 获取按钮对应的服务ID
    QString serviceId = button->property("serviceId").toString();

    if (serviceId.isEmpty()) {
        qWarning() << "无法获取按钮对应的服务ID";
        return;
    }

    // 获取当前服务状态
    QMap<QString, bool> serviceStatus = m_user->getIsusing();
    bool currentStatus = serviceStatus.value(serviceId, false);





    // QString statusText = newStatus ? "已启用" : "已禁用";
    // QMessageBox::information(this, "服务状态变更",
    //                          QString("服务 '%1' 的状态已变更为: %2").arg(serviceId).arg(statusText));




    if(currentStatus==true)
    {
        QString opera=currentStatus?"注销":"启用";
        QMessageBox::StandardButton res=QMessageBox::question(this,"请确认","是否"+opera+serviceId+"服务?");
        if(res==QMessageBox::No)return;

        QMessageBox::StandardButton res1=QMessageBox::question(this,"请再次确认","此操作会注销您的"+serviceId+"账号且无法撤销，是否继续？");
        if(res1==QMessageBox::No)return;
        // 切换服务状态
        bool newStatus = !currentStatus;
        serviceStatus[serviceId] = newStatus;
        m_user->removeService(serviceId);

        // 刷新UI
        refreshUI();
    }
    if(currentStatus==false)
    {
        AddServicePageZbk* regi=new AddServicePageZbk(serviceId,m_user);
        qDebug()<<m_user->getId()<<Qt::endl;
        qDebug()<<serviceId<<Qt::endl;
        regi->show();
        connect(regi, &AddServicePageZbk::register_success, this, &ServiceListPageZbk::refreshUI);
    }
}

void ServiceListPageZbk::refreshUI()
{
    // 重新填充按钮组
    emit service_changed();
    populateButtonGroups();

    // 更新布局
    m_enabledWidget->updateGeometry();
    m_disabledWidget->updateGeometry();
    update();

}


void ServiceListPageZbk::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 创建启用服务区域
    QLabel* enabledLabel = new QLabel("已启用服务", this);
    enabledLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 5px;");
    mainLayout->addWidget(enabledLabel);

    m_enabledWidget = new QWidget(this);
    m_enabledLayout = new QVBoxLayout(m_enabledWidget); // 改为垂直布局
    m_enabledLayout->setSpacing(10);
    m_enabledLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(m_enabledWidget);

    // 添加分隔线
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMaximumHeight(2);
    mainLayout->addWidget(line);

    // 创建未启用服务区域
    QLabel* disabledLabel = new QLabel("未启用服务", this);
    disabledLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 5px;");
    mainLayout->addWidget(disabledLabel);

    m_disabledWidget = new QWidget(this);
    m_disabledLayout = new QVBoxLayout(m_disabledWidget); // 改为垂直布局
    m_disabledLayout->setSpacing(10);
    m_disabledLayout->setAlignment(Qt::AlignTop);
    mainLayout->addWidget(m_disabledWidget);

    // 添加弹簧，使内容靠上显示
    mainLayout->addStretch();

    // 设置主布局的边距和间距
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);
}


void ServiceListPageZbk::populateButtonGroups()
{
    // 先清除布局（但不删除widget）
    clearLayoutWithoutDeletingWidgets(m_enabledLayout);
    clearLayoutWithoutDeletingWidgets(m_disabledLayout);

    // 然后统一删除按钮组中的按钮
    QList<QAbstractButton*> enabledButtons = m_enabledGroup->buttons();
    QList<QAbstractButton*> disabledButtons = m_disabledGroup->buttons();

    // 删除已启用服务的按钮
    for (QAbstractButton* button : enabledButtons) {
        m_enabledGroup->removeButton(button);
        if (button) {
            button->deleteLater(); // 使用deleteLater安全删除

        }
    }

    // 删除未启用服务的按钮
    for (QAbstractButton* button : disabledButtons) {
        m_disabledGroup->removeButton(button);
        if (button) {
            button->deleteLater(); // 使用deleteLater安全删除

        }
    }

    // 获取用户的服务启用状态
    QMap<QString, bool> serviceStatus = m_user->getIsusing();

    // 分别处理已启用和未启用服务
    QList<ServicesZbk*> enabledServices;
    QList<ServicesZbk*> disabledServices;

    for (ServicesZbk* service : ServicesZbk::allServices) {
        QString serviceId = service->Id;
        bool isEnabled = serviceStatus.value(serviceId, false);

        if (isEnabled) {
            enabledServices.append(service);
        } else {
            disabledServices.append(service);
        }
    }

    // 创建新的按钮行
    createServiceButtonRows(enabledServices, m_enabledLayout, m_enabledGroup, true);
    createServiceButtonRows(disabledServices, m_disabledLayout, m_disabledGroup, false);
}

void ServiceListPageZbk::clearLayoutWithoutDeletingWidgets(QLayout* layout)
{
    if (!layout) return;

    while (QLayoutItem* item = layout->takeAt(0)) {
        // 如果item包含widget，只移除不删除
        if (QWidget* widget = item->widget()) {
            widget->setParent(nullptr); // 解除父子关系
        }
        // 如果item包含子布局，递归清理（但通常您的布局结构不需要这个）
        else if (QLayout* childLayout = item->layout()) {
            clearLayoutWithoutDeletingWidgets(childLayout);
            // 注意：这里不删除childLayout，因为它会被item管理
        }

        delete item; // 删除布局项本身
    }
}
void ServiceListPageZbk::createServiceButtonRows(const QList<ServicesZbk*>& services,
                                                 QVBoxLayout* containerLayout,
                                                 QButtonGroup* buttonGroup,
                                                 bool isEnabled)
{

    if (services.isEmpty()) {
        QLabel* emptyLabel = new QLabel(isEnabled ? "暂无已启用服务" : "暂无未启用服务");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; margin: 20px;");
        containerLayout->addWidget(emptyLabel);
        return;
    }

    int buttonsPerRow = 3; // 每行最多3个按钮
    int currentRowButtonCount = 0;
    QHBoxLayout* currentRowLayout = nullptr;

    for (ServicesZbk* service : services) {
        // 每行开始时创建新的水平布局
        if (currentRowButtonCount % buttonsPerRow == 0) {
            currentRowLayout = new QHBoxLayout();
            currentRowLayout->setSpacing(10);
            currentRowLayout->setAlignment(Qt::AlignLeft);
            containerLayout->addLayout(currentRowLayout);
        }

        // 创建服务按钮
        QPushButton* button = createServiceButton(service);
        button->setProperty("serviceId", service->Id);
        buttonGroup->addButton(button);

        // 添加到当前行
        currentRowLayout->addWidget(button);
        currentRowButtonCount++;

        // 如果当前行已满，重置计数器
        if (currentRowButtonCount % buttonsPerRow == 0) {
            currentRowButtonCount = 0;
        }
    }

    // 如果最后一行不满，添加弹簧使其左对齐
    if (currentRowLayout && currentRowButtonCount > 0) {
        currentRowLayout->addStretch();
    }
}

void ServiceListPageZbk::clearLayout(QLayout* layout)
{

}

QPushButton* ServiceListPageZbk::createServiceButton(ServicesZbk* service)
{
    QPushButton* button = new QPushButton();

    // 设置按钮大小
    button->setFixedSize(70, 70); // 固定大小，确保整齐排列
    button->setCheckable(true);

    // 创建垂直布局用于图标和文本
    QVBoxLayout* buttonLayout = new QVBoxLayout(button);
    buttonLayout->setSpacing(5);
    buttonLayout->setContentsMargins(5, 5, 5, 5);

    // 服务图标
    QLabel* iconLabel = new QLabel();
    QPixmap iconPixmap(service->using_Icon);
    if (!iconPixmap.isNull()) {
        iconLabel->setPixmap(iconPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    iconLabel->setAlignment(Qt::AlignCenter);

    // 服务名称
    QLabel* nameLabel = new QLabel(service->Id_cn);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 10px;");
    nameLabel->setWordWrap(true); // 文本换行

    // 添加到按钮布局
    buttonLayout->addWidget(iconLabel);
    buttonLayout->addWidget(nameLabel);

    // 设置按钮样式
    QString buttonStyle = R"(
        QPushButton {
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: transparent;
        }
        QPushButton:hover {
            border-color: #0078d4;
            background-color: transparent;
        }
        QPushButton:checked {
            border: 2px solid #0078d4;
            background-color: transparent;
        }
    )";

    button->setStyleSheet(buttonStyle);

    return button;
}
