#include "llmcaller.h"
#include <QProcessEnvironment>

// 构造函数
LLMCaller::LLMCaller(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    QString path = QProcessEnvironment::systemEnvironment().value("DEEPSEEK_API_KEY");
    qDebug() << "PATH:" << path;
    // --- 重要：请替换为你自己的 API Key 和 URL ---
    // 为了安全，最好从环境变量或加密的配置文件中读取
    m_apiKey = path;
    m_apiUrl = "https://api.deepseek.com/chat/completions";
    qDebug() << "API URL:" << m_apiUrl;
}

LLMCaller::~LLMCaller() {}

// 调用 API 的公共方法
void LLMCaller::callApi(const QString &userMessage)
{
    QJsonObject requestJson;
    requestJson["model"] = "deepseek-chat"; // 或其他模型
    requestJson["max_tokens"] = 4000;
    requestJson["stream"] = false; // DeepSeek支持流式传输，设为false简化处理

    QJsonArray messages;
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = "You are a helpful assistant.";
    messages.append(systemMessage);

    QJsonObject userMessageObj;
    userMessageObj["role"] = "user";
    userMessageObj["content"] = userMessage;
    messages.append(userMessageObj);

    requestJson["messages"] = messages;

    QJsonDocument doc(requestJson);
    QByteArray postData = doc.toJson();

    qDebug() << "=== Sending Request ===";
    qDebug() << "URL:" << m_apiUrl;
    qDebug() << "Request Data:" << QString::fromUtf8(postData);

    QUrl url(m_apiUrl);
    QNetworkRequest request(url);

    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    // 发送 POST 请求
    QNetworkReply *reply = m_networkManager->post(request, postData);

    // 连接 finished 信号，以便在请求完成时处理响应
    connect(reply, &QNetworkReply::finished, this, &LLMCaller::onApiReplyFinished);
}

// 处理 API 响应的槽函数
void LLMCaller::onApiReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater(); // 重要：清理 reply 对象

    if (reply->error() != QNetworkReply::NoError) {
        // 发生错误
        emit errorOccurred(QString("Network Error: %1").arg(reply->errorString()));
        return;
    }

    // 解析 JSON 响应
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    if (jsonResponse.isNull() || !jsonResponse.isObject()) {
        emit errorOccurred("Invalid JSON response received.");
        return;
    }

    QJsonObject jsonObject = jsonResponse.object();
    if (jsonObject.contains("choices")) {
        QJsonArray choices = jsonObject["choices"].toArray();
        if (!choices.isEmpty()) {
            QJsonObject firstChoice = choices[0].toObject();
            QJsonObject message = firstChoice["message"].toObject();
            QString content = message["content"].toString();
            emit responseReceived(content);
            return;
        }
    }

    // 如果没有找到内容，可能是 API 返回了错误信息
    if (jsonObject.contains("error")) {
        QJsonObject error = jsonObject["error"].toObject();
        QString errorMessage = error["message"].toString();
        emit errorOccurred(QString("API Error: %1").arg(errorMessage));
    } else {
        emit errorOccurred("Could not parse response content.");
    }
}
