#ifndef LLMCALLER_H
#define LLMCALLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


class LLMCaller : public QObject
{
    Q_OBJECT
public:
    explicit LLMCaller(QObject *parent = nullptr);
    ~LLMCaller();

    void callApi(const QString &userMessage);

    void processNormalResponse(const QByteArray &data);
    void processStreamResponse(const QByteArray &data);

signals:
    void responseReceived(const QString &response);
    void errorOccurred(const QString &error);

private slots:
    void onApiReplyFinished();


private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey; // 建议从配置文件或环境变量读取
    QString m_apiUrl;
};

#endif // LLMCALLER_H
