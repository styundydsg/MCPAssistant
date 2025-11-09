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

    // 调用 add 工具
    void callAdd(int a, int b, std::function<void(int)> callback, bool useJsonBody = true);
    void callAdd(int a, int b);

    // 通用工具调用接口
    void callTool(const QString& toolName, const QJsonObject& params,
                  std::function<void(QJsonObject)> callback,
                  bool useJsonBody = true);

private:
    QNetworkAccessManager* m_manager;
    QString m_serverUrl;
};
