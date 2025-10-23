#include "weibopagezbk.h"
#include "ui_weibopagezbk.h"

WeiBoPageZbk::WeiBoPageZbk(UsersZbk *LoginUser,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeiBoPageZbk),user(LoginUser)
{
    ui->setupUi(this);
    if(parent)
    {
        this->resize(parent->width(),parent->height());

    }
    QAction* searchAction=new QAction(this);
    searchAction->setIcon(QIcon(":/pic/search.png"));

    ui->letSearch->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->letSearch->setPlaceholderText("搜索");
    ui->letSearch->setStyleSheet("border-bottom: none;");
    if(user->getData()["WeiBo"]->m_friends.isEmpty())
    {
        qDebug()<<"WBFriendNULL!"<<Qt::endl;
    }
    ui->lstFriend->setUserId(user->getData()["WeiBo"]->getId());
    ui->lstFriend->setFriends(user->getData()["WeiBo"]->m_friends,"WeiBo");
    QAction* addFriendAct=new QAction("添加好友");
    //addFriendAct->setIcon(QIcon(":/pic/addFriend.png"));
    connect(addFriendAct,&QAction::triggered,this,&WeiBoPageZbk::on_addButton_clicked);
    QMenu *Menu = new QMenu(ui->moreButton);
    Menu->addAction(addFriendAct);
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

WeiBoPageZbk::~WeiBoPageZbk()
{
    delete ui;
    if(searchPage&&searchPage->isVisible())
    {
        searchPage->close();
    }
}
void WeiBoPageZbk::paintEvent(QPaintEvent *event)
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
    rctop.setRight(ui->lstFriend->geometry().right());
    rctop.setBottom(ui->lstFriend->geometry().top());

    // 绘制矩形
    painter.drawRect(rctop);
    rctop.setBottom(ui->lstFriend->geometry().bottom());
    rctop.setTop(ui->lstFriend->geometry().top());
    painter.setBrush(Qt::white);
    painter.drawRect(rctop);

}


void WeiBoPageZbk::on_letSearch_textChanged(const QString &arg1)
{
    QString input=arg1;
    QVector<QString> foundFriends,foundGroups;
    for(QString str:user->getData()["WeiBo"]->m_friends)
    {
        User_ServiceZbk* found=UsersZbk::FindUser_IdService(str,"WeiBo");
        if(found==NULL)
        {
            qDebug()<<"WeiBoPageZbk::on_letSearch_textChanged:friend"<<str<<" failed"<<Qt::endl;
            continue;
        }
        if(found->getId().contains(input)||found->getName().contains(input))
            foundFriends.push_back(str);

    }


    ui->lstFriend->setFriends(foundFriends,"WeiBo");

    update();
}

void WeiBoPageZbk::on_addButton_clicked()
{
    // 先检查服务状态
    if (!user->getIsusing().value("WeiBo", false)) {
        QMessageBox::warning(this, "提示", "微信服务未启用，无法打开搜索");
        return;

    }

    if (!searchPage) {
        searchPage = new SearchPageZbk(user, "WeiBo");
        //searchPage->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    }
    connect(searchPage,&SearchPageZbk::refreshPage,this,&WeiBoPageZbk::reloadFriends);
    connect(searchPage,&SearchPageZbk::friendAdded,this,[&](User_ServiceZbk* targetUser){emit addFriend(targetUser);});

    searchPage->show();
}
void WeiBoPageZbk::reloadFriends()
{
    ui->lstFriend->setFriends(user->getData()["WeiBo"]->m_friends,"WeiBo");
}

