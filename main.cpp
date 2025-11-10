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

    // 获取所有工具
    client.listTools([](QJsonObject result) {
        if (result.contains("tools")) {
            QJsonArray tools = result["tools"].toArray();
            for (const QJsonValue& tool : tools) {
                QJsonObject toolObj = tool.toObject();
                qDebug() << "Tool:" << toolObj["name"].toString();
                qDebug() << "Description:" << toolObj["description"].toString();
            }
        }
    });
    return app.exec();
}
