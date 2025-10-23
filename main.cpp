#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Widget w;
    w.show();
    int res=a.exec();
    qDebug()<<"****************exit:*****************";
    for(auto it=ChatSessionZbk::allSessions.begin();it!=ChatSessionZbk::allSessions.end();it++)
    {
        for(auto i:*it)
        {
            qDebug()<<QString("group %1 %2").arg(i->getSessionId(),QString::number(i->getMessages().count()));
        }
    }
    qDebug()<<"***************************************************************";


    DataManagerZbk::instance().saveAllData();

    return res;
}
