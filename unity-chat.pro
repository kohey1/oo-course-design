QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 编译产物名称
TARGET = unity-chat

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addservicepagezbk.cpp \
    chatmessagedelegatezbk.cpp \
    chatmessagemodelzbk.cpp \
    chatmessagezbk.cpp \
    chatsessionzbk.cpp \
    creategroupzbk.cpp \
    datamanagerzbk.cpp \
    frienditemwidget.cpp \
    friendlistwidget.cpp \
    groupfactory.cpp \
    groupszbk.cpp \
    main.cpp \
    mainpagezbk.cpp \
    modifyloginuserinfopagezbk.cpp \
    qqgroupzbk.cpp \
    qqpagezbk.cpp \
    registerpage.cpp \
    searchingitemszbk.cpp \
    searchpagezbk.cpp \
    servicelistpagezbk.cpp \
    serviceszbk.cpp \
    user_service.cpp \
    userinfopagezbk.cpp \
    userszbk.cpp \
    wechatgroupzbk.cpp \
    wechatpagezbk.cpp \
    weibopagezbk.cpp \
    widget.cpp \
    windowbutton.cpp

HEADERS += \
    addservicepagezbk.h \
    chatmessagedelegatezbk.h \
    chatmessagemodelzbk.h \
    chatmessagezbk.h \
    chatsessionzbk.h \
    creategroupzbk.h \
    datamanagerzbk.h \
    frienditemwidget.h \
    friendlistwidget.h \
    groupfactory.h \
    groupszbk.h \
    mainpagezbk.h \
    modifyloginuserinfopagezbk.h \
    qqgroupzbk.h \
    qqpagezbk.h \
    registerpage.h \
    searchingitemszbk.h \
    searchpagezbk.h \
    servicelistpagezbk.h \
    serviceszbk.h \
    user_service.h \
    userinfopagezbk.h \
    userszbk.h \
    wechatgroupzbk.h \
    wechatpagezbk.h \
    weibopagezbk.h \
    widget.h \
    windowbutton.h

FORMS += \
    addservicepagezbk.ui \
    creategroupzbk.ui \
    mainpagezbk.ui \
    modifyloginuserinfopagezbk.ui \
    qqpagezbk.ui \
    registerpage.ui \
    searchingitemszbk.ui \
    searchpagezbk.ui \
    servicelistpagezbk.ui \
    userinfopagezbk.ui \
    wechatpagezbk.ui \
    weibopagezbk.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    main.qrc

DISTFILES += \
    pic/444qqUseravator.jpg \
    pic/waitqqUseravator.jpg
