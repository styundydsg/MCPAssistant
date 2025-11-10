// mcpclient.h
#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <functional>

class MCPClient : public QObject
{
    Q_OBJECT
public:
    explicit MCPClient(const QString& serverUrl, QObject *parent = nullptr);

    // 通用工具调用接口
    void callTool(const QString& toolName, const QJsonObject& params,
                  std::function<void(QJsonObject)> callback,
                  bool useJsonBody = true);

    void getToolInfo(const QString& toolName, std::function<void(QJsonObject)> callback);
    void listTools(std::function<void (QJsonObject)> callback);
    void handleNetworkReply(QNetworkReply *reply, std::function<void (QJsonObject)> callback);
    void callToolPost(const QString &toolName, const QJsonObject &params, std::function<void (QJsonObject)> callback, bool useJsonBody);
    void callToolGet(const QString &toolName, const QJsonObject &params, std::function<void (QJsonObject)> callback);
private:
    QNetworkAccessManager* m_manager;
    QString m_serverUrl;
};
