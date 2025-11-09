// mcpclient.cpp
#include "mcpclient.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>

MCPClient::MCPClient(const QString &serverUrl, QObject *parent)
    : QObject(parent), m_serverUrl(serverUrl)
{
    m_manager = new QNetworkAccessManager(this);
}

void MCPClient::callAdd(int a, int b)
{
    QUrl url(m_serverUrl + "/add");
    QUrlQuery query;
    query.addQueryItem("a", QString::number(a));
    query.addQueryItem("b", QString::number(b));
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply* reply = m_manager->post(request, QByteArray());

    QObject::connect(reply, &QNetworkReply::finished, [reply]() {
        QByteArray response = reply->readAll();
        qDebug() << "Response:" << response;
        reply->deleteLater();
    });
}

void MCPClient::callTool(const QString &toolName, const QJsonObject &params,
                         std::function<void(QJsonObject)> callback,
                         bool useJsonBody)
{
    QNetworkRequest request;
    QByteArray data;

    if(useJsonBody) {
        // JSON Body
        QUrl url(m_serverUrl + "/" + toolName);
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QJsonDocument doc(params);
        data = doc.toJson();
    } else {
        // URL 参数
        QUrl url(m_serverUrl + "/" + toolName);
        QUrlQuery query;
        for (const QString& key : params.keys())
            query.addQueryItem(key, QString::number(params[key].toInt()));
        url.setQuery(query);
        request.setUrl(url);
        data = QByteArray(); // body 为空
    }

    QNetworkReply* reply = m_manager->post(request, data);

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
