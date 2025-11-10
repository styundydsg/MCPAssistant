// mcpclient.cpp
#include "mcpclient.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>

MCPClient::MCPClient(const QString &serverUrl, QObject *parent)
    : QObject(parent), m_serverUrl(serverUrl)
{
    m_manager = new QNetworkAccessManager(this);
}

void MCPClient::callTool(const QString &toolName, const QJsonObject &params,
                         std::function<void(QJsonObject)> callback,
                         bool useJsonBody)
{
    QNetworkRequest request;
    QByteArray data;

    // 设置超时时间
    request.setTransferTimeout(30000); // 30秒超时

    // 对于FastAPI桥接服务器，总是使用JSON body
    QUrl url(m_serverUrl + "/" + toolName);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument doc(params);
    data = doc.toJson();

    qDebug() << "调用工具:" << toolName;
    qDebug() << "URL:" << url.toString();
    qDebug() << "请求数据:" << QString::fromUtf8(data);

    QNetworkReply* reply = m_manager->post(request, data);

    // 连接错误处理
    QObject::connect(reply, &QNetworkReply::errorOccurred, [reply, callback](QNetworkReply::NetworkError error) {
        qDebug() << "Network error occurred:" << error << reply->errorString();
        callback(QJsonObject{{"error", reply->errorString()}});
        reply->deleteLater();
    });

    QObject::connect(reply, &QNetworkReply::finished, [reply, callback]() {
        if (reply->error() != QNetworkReply::NoError) {
            // 错误已经在 errorOccurred 中处理
            return;
        }

        QByteArray response = reply->readAll();
        qDebug() << "原始响应:" << QString::fromUtf8(response);
        
        QJsonDocument respDoc = QJsonDocument::fromJson(response);
        if(respDoc.isObject()) {
            callback(respDoc.object());
        } else {
            qDebug() << "Invalid JSON response:" << QString::fromUtf8(response);
            callback(QJsonObject{{"error", "Invalid JSON response"}});
        }
        reply->deleteLater();
    });
}

// 获取特定工具信息
void MCPClient::getToolInfo(const QString& toolName, std::function<void(QJsonObject)> callback)
{
    QJsonObject params;
    // 这里参数为空，因为工具名称在URL路径中
    callTool("tools/" + toolName, params, callback, false);
}

void MCPClient::listTools(std::function<void(QJsonObject)> callback)
{
    QUrl url(m_serverUrl + "/tools");
    QNetworkRequest request(url);

    QNetworkReply* reply = m_manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [reply, callback]() {
        QByteArray response = reply->readAll();
        QJsonDocument respDoc = QJsonDocument::fromJson(response);
        if(respDoc.isObject())
            callback(respDoc.object());
        else
            callback(QJsonObject());
        reply->deleteLater();
    });
}
