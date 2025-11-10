import sys
import asyncio
import threading
from PySide6.QtWidgets import QApplication, QWidget, QVBoxLayout, QTextEdit, QLineEdit, QPushButton
from agent_bridge import chat_with_agent  # 这是前面的大模型 + MCP 逻辑

class ChatWindow(QWidget):
    def __init__(self, loop):
        super().__init__()
        self.loop = loop
        self.setWindowTitle("智能代理对话")
        self.resize(600, 400)

        layout = QVBoxLayout(self)
        self.chat_box = QTextEdit()
        self.chat_box.setReadOnly(True)
        self.input_box = QLineEdit()
        self.send_btn = QPushButton("发送")

        layout.addWidget(self.chat_box)
        layout.addWidget(self.input_box)
        layout.addWidget(self.send_btn)
        self.send_btn.clicked.connect(self.send_message)

    def send_message(self):
        user_text = self.input_box.text().strip()
        if not user_text:
            return
        self.chat_box.append(f"🧑‍💻 你：{user_text}")
        self.input_box.clear()

        # 用线程安全的方式提交协程到异步事件循环
        future = asyncio.run_coroutine_threadsafe(self.get_reply(user_text), self.loop)
        # future.result() 不要在主线程等待，会卡界面

    async def get_reply(self, text):
        reply = await chat_with_agent(text)
        # 回到主线程更新UI
        self.chat_box.append(f"🤖 AI：{reply}")

def start_asyncio_loop(loop):
    asyncio.set_event_loop(loop)
    loop.run_forever()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    loop = asyncio.new_event_loop()
    threading.Thread(target=start_asyncio_loop, args=(loop,), daemon=True).start()

    win = ChatWindow(loop)
    win.show()
    sys.exit(app.exec())

