from fastmcp import FastMCP
import logging

# 初始化MCP服务器实例
# 第一个参数是服务器的名称，这是一个可选但推荐的标识
server = FastMCP("My First MCP Server")
logging.basicConfig(level=logging.DEBUG)

@server.tool()
def add(a: int, b: int) -> int:
    """将两个数字相加并返回结果。

    这是一个简单的算术工具，用于演示MCP的工具调用功能。
    """
    return a + b


@server.resource("greeting://{name}")
def get_greeting(name: str) -> str:
    """根据提供的姓名，返回一条个性化的中文问候信息。

    这是一个只读资源，用于演示MCP的资源访问功能。
    """
    return f"你好，{name}！"


if __name__ == "__main__":
    # 启动MCP服务器
    
    server.run(transport="sse", host="0.0.0.0", port=8000)



