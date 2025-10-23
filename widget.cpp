#include "widget.h"
#include "ui_widget.h"

void Widget::initlogin()
{
    this->setWindowTitle("登录-UniTencent");

    this->ui->idedit->setAlignment(Qt::AlignHCenter);
    this->ui->idedit->setPlaceholderText("请输入账号");
    this->ui->idedit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{11}")));
    this->ui->idedit->setClearButtonEnabled(true);

    this->ui->passedit->setAlignment(Qt::AlignHCenter);
    this->ui->passedit->setEchoMode(QLineEdit::Password);
    this->ui->passedit->setPlaceholderText("请输入密码");
    this->ui->passedit->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9]+$")));
    this->ui->passedit->setClearButtonEnabled(true);

    this->ui->loginButton-> setStyleSheet("QPushButton{background-color: rgb(99, 172, 245);border-radius:5px;color: white;}"
                                         "QPushButton:hover{background-color:rgb(152, 205, 245); }"
                                         "QPushButton:pressed{background-color:rgb(116, 151, 245);}");
    this->ui->registerButton->setFlat(true);
    this->ui->registerButton->setStyleSheet("QPushButton {border: none; background: transparent;color:blue;}");
    ServicesZbk* QQ=new ServicesZbk(":/pic/qq_using.ico",":/pic/qq_offLine.ico","QQ");
    ServicesZbk* WeChat=new ServicesZbk(":/pic/wechat_using.ico",":/pic/wechat_offLine.ico","WeChat","微信");
    ServicesZbk* WeiBo=new ServicesZbk(":/pic/weibo_using.ico",":/pic/weibo_offLine.ico","WeiBo","微博");
}




Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_rePage=NULL;
    this->setWindowIcon(QIcon(":/pic/favicon.ico"));
    qDebug()<<"";
    qDebug()<<"************************************未加载时：***************************";
    for(auto it=ChatSessionZbk::allSessions.begin();it!=ChatSessionZbk::allSessions.end();it++)
    {
        for(auto i:*it)
        {
            qDebug()<<QString("group %1 %2").arg(i->getSessionId(),QString::number(i->getMessages().count()));
        }
    }
    qDebug()<<"***************************************************************";

    srand((unsigned int)time(NULL));
    DataManagerZbk::instance().loadAllData();
    DataManagerZbk::instance().ensureSystemSessions();
    initlogin();

    qDebug()<<"************************************加载后：***************************";
    for(auto it=ChatSessionZbk::allSessions.begin();it!=ChatSessionZbk::allSessions.end();it++)
    {
        for(auto i:*it)
        {
            qDebug()<<QString("group %1 %2").arg(i->getSessionId(),QString::number(i->getMessages().count()));
        }
    }
    qDebug()<<"***************************************************************";

    //inittestUser();


}



void Widget::inittestUser()
{

    // ServicesZbk* QQ=new ServicesZbk(":/pic/qq_using.ico",":/pic/qq_offLine.ico","QQ");
    // ServicesZbk* WeChat=new ServicesZbk(":/pic/wechat_using.ico",":/pic/wechat_offLine.ico","WeChat","微信");
    // ServicesZbk* WeiBo=new ServicesZbk(":/pic/weibo_using.ico",":/pic/weibo_offLine.ico","WeiBo","微博");

    UsersZbk* testU=new UsersZbk("test","111","123",":/pic/123qqUseravator.jpg");


    UsersZbk* test1=new UsersZbk("bk","123","111",":/pic/qq_using.ico");
    // test1->getIsusing()["QQ"]=true;
    // test1->getIsusing()["Wechat"]=true;
    test1->addService_fast("QQ");
    test1->addService_fast("WeChat");



    testU->addService_fast("QQ");
    testU->addService_fast("WeChat");
    testU->addFriend(test1->getData()["QQ"]->getId(),"QQ");
    testU->addFriend(test1->getData()["WeChat"]->getId(),"WeChat");


    // testU->getIsusing()["QQ"]=true;
    // testU->getIsusing()["WeChat"]=true;


    UsersZbk* test2=new UsersZbk("bk_ohye","123","222",":/pic/123qqUseravator.jpg");
    // test2->getIsusing()["QQ"]=true;
    // test2->getIsusing()["Wechat"]=true;
    test2->addService_fast("QQ");
    test2->addService_fast("WeChat");
    //testU->addFriend(test2->getData()["QQ"]->getId(),"QQ");
    testU->addFriend(test2->getData()["WeChat"]->getId(),"WeChat");


    UsersZbk* test3=new UsersZbk("wait","123","333",":/pic/waitqqUseravator.jpg");
    //test3->getIsusing()["QQ"]=true;
    test3->addService_fast("QQ");
    test3->addService_fast("WeChat");
    testU->addFriend(test3->getData()["WeChat"]->getId(),"WeChat");


    UsersZbk* test4=new UsersZbk("[fire]","123","444",":/pic/444qqUseravator.jpg");
    //test4->getIsusing()["QQ"]=true;
    test4->addService_fast("QQ");
    test4->addService_fast("WeChat");
    testU->addFriend(test4->getData()["QQ"]->getId(),"QQ");


    UsersZbk* yao=new UsersZbk("芋泥味小孩","123","666",":/pic/666qqUseravator.jpg");
    //yao->getIsusing()["QQ"]=true;
    yao->addService_fast("QQ");
    testU->addFriend(yao->getData()["QQ"]->getId(),"QQ");

    UsersZbk* kan=new UsersZbk("柳智敏","123","555",":/pic/555qqUseravator.jpg");
    //kan->getIsusing()["QQ"]=true;
    kan->addService_fast("QQ");
    kan->addService_fast("WeiBo");
    kan->addService_fast("WeChat");
    testU->addFriend(kan->getData()["QQ"]->getId(),"QQ");


    UsersZbk* gu=new UsersZbk("社会主义新青年","123","777",":/pic/777qqUseravator.jpg");
    //gu->getIsusing()["QQ"]=true;
    gu->addService_fast("QQ");
    testU->addFriend(gu->getData()["QQ"]->getId(),"QQ");


    UsersZbk* lian=new UsersZbk("AAA鹈鹕镇杨桃批发连总","123","888",":/pic/888qqUseravator.jpg");
    //lian->getIsusing()["QQ"]=true;
    lian->addService_fast("QQ");
    testU->addFriend(lian->getData()["QQ"]->getId(),"QQ");




    GroupsZbk* group1=groupFactory::createGroup("QQ","0",testU->getData()["QQ"]->getId(),"电脑高手豪师傅");
    groupFactory::AddMember(group1,testU->getData()["QQ"]->getId());
    groupFactory::AddMember(group1,test3->getData()["QQ"]->getId());
    groupFactory::AddMember(group1,test4->getData()["QQ"]->getId());

    GroupsZbk* group2=groupFactory::createGroup("QQ","0",yao->getData()["QQ"]->getId(),"汤臣一品");
    groupFactory::AddMember(group2,testU->getData()["QQ"]->getId());
    groupFactory::AddMember(group2,yao->getData()["QQ"]->getId());
    groupFactory::AddMember(group2,kan->getData()["QQ"]->getId());
    groupFactory::AddMember(group2,gu->getData()["QQ"]->getId());
    groupFactory::AddMember(group2,lian->getData()["QQ"]->getId());

    GroupsZbk* group4=groupFactory::createGroup("QQ","0",gu->getData()["QQ"]->getId(),"testGroup");
    //group3->addMember(testU->getData()["QQ"]->getId());
    groupFactory::AddMember(group4,yao->getData()["QQ"]->getId());
    groupFactory::AddMember(group4,kan->getData()["QQ"]->getId());
    groupFactory::AddMember(group4,gu->getData()["QQ"]->getId());
    groupFactory::AddMember(group4,lian->getData()["QQ"]->getId());








    GroupsZbk* group3=groupFactory::createGroup("WeChat","0",test1->getData()["WeChat"]->getId(),"电脑高手豪师傅");
    groupFactory::AddMember(group3,testU->getData()["WeChat"]->getId());
    groupFactory::AddMember(group3,test1->getData()["WeChat"]->getId());
    groupFactory::AddMember(group3,test2->getData()["WeChat"]->getId());



    UsersZbk* testN=new UsersZbk("testN","123","999",":/pic/888qqUseravator.jpg");
    //testN->getIsusing()["QQ"]=true;
    testN->addService_fast("QQ");
}
Widget::~Widget()
{
    delete ui;
}

void Widget::on_loginButton_clicked()
{
    bool flag=false;
    QString inputId=this->ui->idedit->text();
    QString inputPass=this->ui->passedit->text();
    if(inputId.isEmpty())
    {
        QMessageBox::warning(this,"请检查","账号不可为空");
        return;
    }
    if(inputPass.isEmpty())
    {
        QMessageBox::warning(this,"请检查","密码不可为空");
        return;
    }
    for(auto i:std::as_const(UsersZbk::allUsers))
    {
        if(i->getId()==inputId&&i->getPassword()==inputPass)
        {
            flag=true;
            this->ui->loginButton->setText("登录成功");
            MainPageZbk* mainPage=new MainPageZbk(i);
            mainPage->show();
            connect(mainPage,&MainPageZbk::logout,this,[=](){
                m_rePage=NULL;
                this->ui->loginButton->setText("登录");
                this->show();

            });

            this->hide();
            break;
        }
    }
    if(!flag)
    {
        QMessageBox::warning(this,"请检查","用户名或密码错误");
        return;
    }
}


void Widget::on_registerButton_clicked()
{
    if(!m_rePage)
    {
        m_rePage=new RegisterPage;

    }

    m_rePage->show();
}

