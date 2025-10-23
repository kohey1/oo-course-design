#include "qqpagezbk.h"
#include "ui_qqpagezbk.h"
#include <QMessageBox>
#include <QTimer>
QQPageZbk::QQPageZbk(UsersZbk* Loginuser,QWidget *parent):QWidget(parent),ui(new Ui::QQPageZbk), user(Loginuser)
{
    ui->setupUi(this);
    if(parent)
    {
        this->resize(parent->width(),parent->height());
        ui->twList->resize(parent->width(),parent->height());
    }
    QAction* searchAction=new QAction(this);
    searchAction->setIcon(QIcon(":/pic/search.png"));



    ui->letSearch->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->letSearch->setPlaceholderText("搜索");
    ui->letSearch->setStyleSheet("border-bottom: none;");
   // qDebug()<<"friendid:"<<user->getData()["QQ"]->m_friends.front();
    ui->lstFriend->setUserId(Loginuser->getData()["QQ"]->getId());
    ui->lstFriend->setFriends(user->getData()["QQ"]->m_friends,"QQ");
    if(user->getData()["QQ"]->m_groups.isEmpty())
    {
        qDebug()<<"QQGroupNULL!"<<Qt::endl;
    }
    else
        qDebug()<<"groupid:"<<user->getData()["QQ"]->m_groups.front();
    if(ui->lstGroup->setGroups(user->getData()["QQ"]->m_groups,"QQ"))
        qDebug()<<"QQsucceed"<<Qt::endl;

    QAction* addFriendAct=new QAction("添加好友");
    //addFriendAct->setIcon(QIcon(":/pic/addFriend.png"));
    QAction* createGroupAct=new QAction("创建群聊");
    //createGroupAct->setIcon(QIcon(":/pic/createGroup.png"));



    // ui->moreButton->addAction(addFriendAct);
    // ui->moreButton->addAction(createGroupAct);
    connect(addFriendAct,&QAction::triggered,this,&QQPageZbk::on_pushButton_clicked);
    connect(createGroupAct,&QAction::triggered,this,&QQPageZbk::oncreateGroupClicked);


    QMenu *Menu = new QMenu(ui->moreButton);
    Menu->addAction(addFriendAct);
    Menu->addAction(createGroupAct);
    ui->moreButton->setMenu(Menu);

    ui->moreButton->menu()->setStyleSheet(R"(
    QMenu {
        background-color: white;
        border: 1px solid #ddd;
        border-radius: 4px;
        padding: 4px 0;
    }
    QMenu::item {
        padding: 8px 20px;
        font-size: 13px;
        color: #333333;  /* 确保文字是深色 */
        background-color: transparent;
    }
    QMenu::item:hover {
        background-color: #f0f0f0;
        color: #333333;  /* 悬浮时文字保持深色 */
    }
    QMenu::item:selected {
        background-color: #e0e0e0;
        color: #333333;  /* 选中时文字保持深色 */
    }
)");
}



QQPageZbk::~QQPageZbk()
{
    delete ui;
    if(searchPage&&searchPage->isVisible())
    {
        searchPage->close();
    }
}
void QQPageZbk::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 设置画刷和画笔
    painter.setBrush(QColor(212,222,224));
    painter.setPen(Qt::NoPen);

    // 计算绘制区域
    QRect rctop = rect();
    rctop.setTop(0);
    rctop.setLeft(0);
    rctop.setRight(ui->twList->geometry().right());
    rctop.setBottom(ui->twList->geometry().top());

    // 绘制矩形
    painter.drawRect(rctop);
    rctop.setBottom(ui->twList->geometry().bottom());
    rctop.setTop(ui->twList->geometry().top());
    painter.setBrush(Qt::white);
    painter.drawRect(rctop);

}

void QQPageZbk::on_letSearch_textChanged(const QString &arg1)
{
    QString input=arg1;
    QVector<QString> foundFriends,foundGroups;
    for(QString str:user->getData()["QQ"]->m_friends)
    {
        User_ServiceZbk* found=UsersZbk::FindUser_IdService(str,"QQ");
        if(found==NULL)
        {
            qDebug()<<"QQPageZbk::on_letSearch_textChanged:friend"<<str<<" failed"<<Qt::endl;
            continue;
        }
        if(found->getId().contains(input)||found->getName().contains(input))
            foundFriends.push_back(str);

    }

    for(QString str:user->getData()["QQ"]->m_groups)
    {
        GroupsZbk* found=groupFactory::FindGroup_IdService(str,"QQ");
        if(found==NULL)
        {
            qDebug()<<"QQPageZbk::on_letSearch_textChanged:group"<<str<<" failed"<<Qt::endl;
            continue;
        }
        if(found->getId().contains(input)||found->getName().contains(input))
            foundGroups.push_back(str);

    }
    ui->lstFriend->setFriends(foundFriends,"QQ");
    ui->lstGroup->setGroups(foundGroups,"QQ");
    //update();
}


void QQPageZbk::on_pushButton_clicked()
{
    // 先检查服务状态
    if (!user->getIsusing().value("QQ", false)) {
        QMessageBox::warning(this, "提示", "QQ服务未启用，无法打开搜索");
        return;
    }

    if (!searchPage) {
        searchPage = new SearchPageZbk(user, "QQ");
        //searchPage->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    }
    connect(searchPage,&SearchPageZbk::refreshPage,this,&QQPageZbk::reloadFriends);
    connect(searchPage,&SearchPageZbk::friendAdded,this,[&](User_ServiceZbk* targetUser){emit addFriend(targetUser);});
    searchPage->show();
}

void QQPageZbk::reloadFriends()
{
    ui->lstFriend->setFriends(user->getData()["QQ"]->m_friends,"QQ");
    if(user->getData()["QQ"]->m_groups.isEmpty())
    {
        qDebug()<<"QQGroupNULL!"<<Qt::endl;
    }
    else
        qDebug()<<"groupid:"<<user->getData()["QQ"]->m_groups.front();
    if(ui->lstGroup->setGroups(user->getData()["QQ"]->m_groups,"QQ"))
        qDebug()<<"QQsucceed"<<Qt::endl;
}

void QQPageZbk::oncreateGroupClicked()
{
    createGroupPage=NULL;
    if (!createGroupPage) {
        createGroupPage = new createGroupZbk(user, "QQ");
        createGroupPage->setAttribute(Qt::WA_DeleteOnClose);
        //searchPage->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    }
    connect(createGroupPage,&createGroupZbk::groupCreated,this,&QQPageZbk::reloadFriends);
    createGroupPage->exec();
}

