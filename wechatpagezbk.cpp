#include "wechatpagezbk.h"
#include "ui_wechatpagezbk.h"

WeChatPageZbk::WeChatPageZbk(UsersZbk *LoginUser,QWidget *parent): QWidget(parent), ui(new Ui::WeChatPageZbk),user(LoginUser)
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
    if(user->getData()["WeChat"]->m_friends.isEmpty())
    {
        qDebug()<<"WXFriendNULL!"<<Qt::endl;
    }
    ui->lstFriend->setUserId(user->getData()["WeChat"]->getId());
    ui->lstFriend->setFriends(user->getData()["WeChat"]->m_friends,"WeChat");
    if(user->getData()["WeChat"]->m_groups.isEmpty())
    {
        qDebug()<<"WXGroupNULL!"<<Qt::endl;
    }
    else
        qDebug()<<"groupid:"<<user->getData()["WeChat"]->m_groups.front();
    if(ui->lstGroup->setGroups(user->getData()["WeChat"]->m_groups,"WeChat"))
        qDebug()<<"WXsucceed"<<Qt::endl;

    QAction* addFriendAct=new QAction("添加好友");
    //addFriendAct->setIcon(QIcon(":/pic/addFriend.png"));
    QAction* createGroupAct=new QAction("创建群聊");
    //createGroupAct->setIcon(QIcon(":/pic/createGroup.png"));



    // ui->moreButton->addAction(addFriendAct);
    // ui->moreButton->addAction(createGroupAct);
    connect(addFriendAct,&QAction::triggered,this,&WeChatPageZbk::on_addButton_clicked);
    connect(createGroupAct,&QAction::triggered,this,&WeChatPageZbk::oncreateGroupClicked);




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


WeChatPageZbk::~WeChatPageZbk()
{
    delete ui;
    if(searchPage&&searchPage->isVisible())
    {
        searchPage->close();
    }
}
void WeChatPageZbk::paintEvent(QPaintEvent *event)
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

void WeChatPageZbk::on_letSearch_textChanged(const QString &arg1)
{
    QString input=arg1;
    QVector<QString> foundFriends,foundGroups;
    for(QString str:user->getData()["WeChat"]->m_friends)
    {
        User_ServiceZbk* found=UsersZbk::FindUser_IdService(str,"WeChat");
        if(found==NULL)
        {
            qDebug()<<"WeChatPageZbk::on_letSearch_textChanged:friend"<<str<<" failed"<<Qt::endl;
            continue;
        }
        if(found->getId().contains(input)||found->getName().contains(input))
            foundFriends.push_back(str);

    }

    for(QString str:user->getData()["WeChat"]->m_groups)
    {
        GroupsZbk* found=groupFactory::FindGroup_IdService(str,"WeChat");
        if(found==NULL)
        {
            qDebug()<<"WeChatPageZbk::on_letSearch_textChanged:group"<<str<<" failed"<<Qt::endl;
            continue;
        }
        if(found->getId().contains(input)||found->getName().contains(input))
            foundGroups.push_back(str);

    }
    ui->lstFriend->setFriends(foundFriends,"WeChat");
    ui->lstGroup->setGroups(foundGroups,"WeChat");
    update();
}


void WeChatPageZbk::on_addButton_clicked()
{
    // 先检查服务状态
    if (!user->getIsusing().value("WeChat", false)) {
        QMessageBox::warning(this, "提示", "微信服务未启用，无法打开搜索");
        return;

    }

    if (!searchPage) {
        searchPage = new SearchPageZbk(user, "WeChat");
        //searchPage->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    }
    connect(searchPage,&SearchPageZbk::refreshPage,this,&WeChatPageZbk::reloadFriends);
    connect(searchPage,&SearchPageZbk::friendAdded,this,[&](User_ServiceZbk* targetUser){emit addFriend(targetUser);});

    searchPage->show();
}

void WeChatPageZbk::reloadFriends()
{
    ui->lstFriend->setFriends(user->getData()["WeChat"]->m_friends,"WeChat");
    if(user->getData()["WeChat"]->m_groups.isEmpty())
    {
        qDebug()<<"WXGroupNULL!";
    }
    else
        qDebug()<<"wx firstgroupid:"<<user->getData()["WeChat"]->m_groups.front();
    if(ui->lstGroup->setGroups(user->getData()["WeChat"]->m_groups,"WeChat"))
        qDebug()<<"WXsucceed";
}


void WeChatPageZbk::oncreateGroupClicked()
{
    createGroupPage=NULL;
    if (!createGroupPage) {
        createGroupPage = new createGroupZbk(user, "WeChat");
        createGroupPage->setAttribute(Qt::WA_DeleteOnClose);
        //searchPage->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    }
    connect(createGroupPage,&createGroupZbk::groupCreated,this,&WeChatPageZbk::reloadFriends);
    createGroupPage->exec();
}
