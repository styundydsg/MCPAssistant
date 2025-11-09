#include "mainwindow.h"
#include "sidebarwindow.h"
#include <QApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEventLoop>
#include <QTimer>
#include <QJsonArray>
#include "mcpclient.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //SidebarWindow w;
    //w.show();
    //return app.exec();

    MCPClient client("http://192.168.40.131:9000"); // 桥接层地址

    client.callAdd(5, 8);
    return app.exec();
}
