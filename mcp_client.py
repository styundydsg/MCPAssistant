# bridge_server.py
import asyncio
from fastapi import FastAPI
from fastmcp import Client
import uvicorn

app = FastAPI()

# 初始化 MCP 客户端（指向 fastmcp_server.py 提供的 SSE 端点）
MCP_URL = "http://192.168.40.131:8000/sse"

@app.post("/process")
async def process_data(uri: str):
    async with Client(MCP_URL) as client:
        result = await client.call_tool("process_data", {"uri": uri})
        return {"summary": result.content[0].text}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)
