#ifndef SIDEBARWINDOW_H
#define SIDEBARWINDOW_H

#include <QWidget>

class LLMCaller;
class MCPClient;

class QTextEdit;
class QLineEdit;

class SidebarWindow:public QWidget
{
    Q_OBJECT
public:
    SidebarWindow(QWidget *parent = nullptr);
    ~SidebarWindow();

    bool isClickOnBlankArea(const QPoint &pos);
protected:
    // 实现窗口拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    //LLM
    void onSendMessage();
    void onResponseReceived(const QString &response); // 处理从 LLMCaller 收到的成功响应
    void onErrorOccurred(const QString &error);       // 处理从 LLMCaller 收到的错误



private:
    void setupUI(); // 设置UI布局
    void moveToScreenSide(); // 移动窗口到屏幕右侧

    QTextEdit *m_displayArea; // 消息显示区域
    QLineEdit *m_inputLine;   // 输入框

    // 拖动相关的成员变量
    QPoint m_dragPosition;
    bool m_isDragging = false;

    LLMCaller *m_llmCaller;
    MCPClient* m_mcpClient;
};

#endif // SIDEBARWINDOW_H
