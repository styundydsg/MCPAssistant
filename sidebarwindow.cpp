#include "sidebarwindow.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>

#include "llmcaller.h"
#include "mcp_sse_client.h"


SidebarWindow::SidebarWindow(QWidget *parent)
    : QWidget(parent)
{
    // 1. 设置窗口标志：无边框、置顶、工具窗口（不在任务栏显示主窗口图标）
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);

    // 2. 设置窗口背景色和半透明效果（可选）
    //setAttribute(Qt::WA_TranslucentBackground, true); // 如果需要完全透明背景
    setStyleSheet("SidebarWindow { background-color: #FCCDD4; color: #FCCDD4; }");

    setupUI();

    // 创建 LLMCaller 实例
    m_llmCaller = new LLMCaller(this);

    connect(m_llmCaller, &LLMCaller::responseReceived, this, &SidebarWindow::onResponseReceived);
    connect(m_llmCaller, &LLMCaller::errorOccurred, this, &SidebarWindow::onErrorOccurred);


    moveToScreenSide();
}

SidebarWindow::~SidebarWindow()
{

}

void SidebarWindow::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10); // 设置边距
    mainLayout->setSpacing(10);

    // 创建消息显示区域
    m_displayArea = new QTextEdit(this);
    m_displayArea->setReadOnly(true);
    m_displayArea->setStyleSheet("QTextEdit { background-color: #F9F9F9; border: 1px solid #CCFFFF; border-radius: 5px; }");

    // 创建输入区域（水平布局）
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_inputLine = new QLineEdit(this);
    m_inputLine->setPlaceholderText("输入消息...");
    m_inputLine->setStyleSheet("QLineEdit { background-color: #CCFFFF; border: 1px solid #FCCDD4; border-radius: 5px; padding: 5px; }");
    QPushButton *sendButton = new QPushButton("发送", this);
    sendButton->setStyleSheet("QPushButton { background-color: #CCFFFF; color: #FCCDD4; border: none; border-radius: 5px; padding: 5px 10px; }");

    // 将输入框和按钮添加到输入布局
    inputLayout->addWidget(m_inputLine);
    inputLayout->addWidget(sendButton);

    // 将所有控件添加到主布局
    mainLayout->addWidget(m_displayArea, 1); // 消息区域占据大部分空间
    mainLayout->addLayout(inputLayout);      // 输入区域在底部

    // 连接信号和槽
    connect(m_inputLine, &QLineEdit::returnPressed, this, &SidebarWindow::onSendMessage);
    connect(sendButton, &QPushButton::clicked, this, &SidebarWindow::onSendMessage);

    setLayout(mainLayout);
}

void SidebarWindow::onSendMessage()
{
    QString userMessage = m_inputLine->text().trimmed();
    if (userMessage.isEmpty()) {
        return;
    }

    // 在显示区域显示用户的消息
    m_displayArea->append(QString("<b style='color:#333;'>You:</b> %1").arg(userMessage));

    // 显示一个“正在思考”的提示
    m_displayArea->append("<b style='color:#888;'>AI:</b> ... (Thinking)");

    // 清空输入框
    m_inputLine->clear();

    // 调用 LLMCaller 的方法来发送请求
    m_llmCaller->callApi(userMessage);
}

void SidebarWindow::onResponseReceived(const QString &response)
{

    qDebug() << "=== Response Received ===";
    qDebug() << "Full text:" << m_displayArea->toPlainText();
    qDebug() << "Response:" << response;
    // 移除 "Thinking..." 提示
    // 我们可以移动光标到上一行末尾并选中整行来删除
    QTextCursor cursor = m_displayArea->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 添加 AI 的真实回复
    m_displayArea->append(QString("<b style='color:#0056b3;'>AI:</b> %1").arg(response));
}

void SidebarWindow::onErrorOccurred(const QString &error)
{
    // 同样，先移除 "Thinking..." 提示
    QTextCursor cursor = m_displayArea->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    // 显示错误信息
    m_displayArea->append(QString("<span style='color:red;'><b>Error:</b> %1</span>").arg(error));
}

void SidebarWindow::moveToScreenSide()
{
    // 获取主屏幕
    QScreen *screen = QApplication::primaryScreen();
    if (!screen) {
        qWarning() << "无法获取屏幕信息！";
        return;
    }

    QRect screenGeometry = screen->availableGeometry(); // 获取可用屏幕几何（排除任务栏）

    // 设置窗口大小和位置
    int windowWidth = 350; // 固定宽度
    int windowHeight = screenGeometry.height() * 0.8; // 高度为屏幕高度的80%
    int xPos = screenGeometry.right() - windowWidth; // X坐标为屏幕右边减去窗口宽度
    int yPos = screenGeometry.top() + (screenGeometry.height() - windowHeight) / 2; // Y坐标居中

    setGeometry(xPos, yPos, windowWidth, windowHeight);
}

bool SidebarWindow::isClickOnBlankArea(const QPoint &pos) {
    // 检查点击位置是否在所有子组件之外
    for (QObject *child : children()) {
        if (QWidget *childWidget = qobject_cast<QWidget*>(child)) {
            if (childWidget->isVisible() && childWidget->geometry().contains(pos)) {
                return false; // 点击在子组件上
            }
        }
    }
    return true; // 点击在空白区域
}

// --- 拖动功能实现 右键空白区域关闭---
void SidebarWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        if (isClickOnBlankArea(event->pos())) {
            // 弹出确认对话框
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "确认关闭",
                "您确定要关闭窗口吗？",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No  // 默认选择"否"
                );

            if (reply == QMessageBox::Yes) {
                qDebug() << "用户选择关闭窗口";
                QApplication::quit();
                return;
            } else {
                qDebug() << "用户取消关闭窗口";
                // 这里可以添加取消关闭后的操作，或者什么都不做
            }
            return; // 事件已处理
        }
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }

    QWidget::mousePressEvent(event);
}



void SidebarWindow::mouseMoveEvent(QMouseEvent *event)
{

    if (m_isDragging && event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }

}


void SidebarWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    event->accept();
}

