from __future__ import annotations

import os
import time
from typing import TypedDict

import requests
from langgraph.graph import END, START, StateGraph
from openai import OpenAI


class AlertState(TypedDict):
    threats: list[dict]
    summary: str


def poll_threats(_: AlertState) -> AlertState:
    try:
        threats = requests.get("http://api:8000/threats", timeout=10).json()
    except requests.RequestException:
        threats = []
    return {"threats": threats, "summary": ""}


def summarize(state: AlertState) -> AlertState:
    threats = state["threats"]
    prompt = "Write a plain-text incident summary followed by one one-line remediation suggestion for these blacklisted IPs: " + str(threats)
    if os.getenv("OPENAI_API_KEY"):
        response = OpenAI().responses.create(model=os.getenv("OPENAI_MODEL", "gpt-5"), input=prompt)
        return {**state, "summary": response.output_text}
    if not threats:
        return {**state, "summary": "Incident summary: no active blacklisted IPs. Remediation: continue monitoring."}
    top = threats[0]
    return {**state, "summary": f"Incident summary: {len(threats)} blacklisted IP(s); highest score is {top['ip']} ({top['threat_score']}). Remediation: block the highest-scoring IP and review its authentication activity."}


graph = StateGraph(AlertState)
graph.add_node("poll_threats", poll_threats)
graph.add_node("summarize", summarize)
graph.add_edge(START, "poll_threats")
graph.add_edge("poll_threats", "summarize")
graph.add_edge("summarize", END)
alert_agent = graph.compile()


if __name__ == "__main__":
    while True:
        print(alert_agent.invoke({"threats": [], "summary": ""})["summary"], flush=True)
        time.sleep(30)
