# bridge_server.py
from fastapi import FastAPI
from fastmcp import Client
import uvicorn

app = FastAPI()

MCP_URL = "http://192.168.40.131:8000/sse"

@app.post("/add")
async def add(a: int, b: int):
    async with Client(MCP_URL) as client:
        result = await client.call_tool("add", {"a": a, "b": b})
        return {"sum": result.content[0].text}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=9000)

