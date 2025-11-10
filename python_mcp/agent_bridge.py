import json
import asyncio
import openai
from fastmcp import Client

# 连接到 MCP 服务
MCP_URL = "http://127.0.0.1:8000/sse"

client = openai.OpenAI(api_key="", base_url="https://api.deepseek.com")


async def call_tool(tool_name, params):
    async with Client(MCP_URL) as client:
        result = await client.call_tool(tool_name, params)
        return result.content[0].text

async def chat_with_agent(user_input):
    """
    这是一个简易的 Agent：
    1. 调用 GPT
    2. 如果 GPT 要求调用工具，则执行之
    3. 否则直接返回回复
    """

    system_prompt = """你是一个智能助手，可以调用工具。
可用的工具有：
- add(a:int, b:int): 计算加法
- greeting://{name}: 获取问候语
当你需要工具帮助时，请用 JSON 形式输出：
{"action": "tool", "name": "<tool_name>", "params": {...}}
否则，直接返回回复。
"""

    response = client.chat.completions.create(
                model="deepseek-chat",
                messages=[
                   {"role": "system", "content": system_prompt},
            	   {"role": "user", "content": user_input},
        	],
                stream=False
            )

    content = response.choices[0].message.content

    try:
        # 尝试解析为工具调用
        data = json.loads(content)
        if data.get("action") == "tool":
            tool_name = data["name"]
            params = data["params"]
            print(f"🧩 调用工具: {tool_name}({params})")
            tool_result = await call_tool(tool_name, params)
            return f"工具返回结果: {tool_result}"
    except json.JSONDecodeError:
        pass

    return content


async def main():
    print("🤖 智能聊天代理启动！输入 exit 退出。")
    while True:
        text = input("你：")
        if text.lower() == "exit":
            break
        reply = await chat_with_agent(text)
        print("AI：", reply)

if __name__ == "__main__":
    asyncio.run(main())

