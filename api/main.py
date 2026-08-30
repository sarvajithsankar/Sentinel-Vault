from __future__ import annotations

import struct
from datetime import datetime, timezone
from pathlib import Path

from fastapi import FastAPI, Query
from fastapi.middleware.cors import CORSMiddleware

from anomaly import analyze_event, EventVector

app = FastAPI(title="Sentinel-Vault API")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],
    allow_credentials=False,
    allow_methods=["*"],
    allow_headers=["*"],
)

VAULT_PATH = Path(__file__).resolve().parents[1] / "data" / "secure.vault"
HEADER = struct.Struct("<8sIIq")
RECORD = struct.Struct("<qiii64s64s")


def _text(value: bytes) -> str:
    return value.split(b"\0", 1)[0].decode("utf-8", errors="replace")


def read_vault() -> tuple[list[dict], int | None]:
    if not VAULT_PATH.exists():
        return [], None
    with VAULT_PATH.open("rb") as vault:
        raw_header = vault.read(HEADER.size)
        if len(raw_header) != HEADER.size:
            return [], None
        magic, _, record_count, last_sync = HEADER.unpack(raw_header)
        if not magic.startswith(b"SVLT001"):
            return [], None
        events = []
        for _ in range(record_count):
            raw_record = vault.read(RECORD.size)
            if len(raw_record) != RECORD.size:
                break
            timestamp, _, threat_score, blacklisted, ip, event_type = RECORD.unpack(raw_record)
            events.append({
                "timestamp": datetime.fromtimestamp(timestamp, tz=timezone.utc).isoformat(),
                "ip": _text(ip),
                "type": _text(event_type),
                "status": "Threat" if blacklisted else "Normal",
                "threat_score": threat_score,
            })
        return events, last_sync


@app.get("/events")
def get_events(page: int = Query(1, ge=1), page_size: int = Query(50, ge=1, le=200)):
    events, _ = read_vault()
    start = (page - 1) * page_size
    return {"items": events[start : start + page_size], "page": page, "page_size": page_size, "total": len(events)}


@app.get("/threats")
def get_threats():
    events, _ = read_vault()
    highest_scores: dict[str, int] = {}
    for event in events:
        if event["status"] == "Threat":
            highest_scores[event["ip"]] = max(highest_scores.get(event["ip"], 0), event["threat_score"])
    return [{"ip": ip, "threat_score": score} for ip, score in sorted(highest_scores.items(), key=lambda item: item[1], reverse=True)]


@app.get("/stats")
def get_stats():
    events, last_sync = read_vault()
    return {
        "total_events": len(events),
        "threats_detected": sum(event["status"] == "Threat" for event in events),
        "mirrors_active": 2 if VAULT_PATH.exists() and VAULT_PATH.with_suffix(".vault.bak").exists() else 0,
        "last_sync_time": datetime.fromtimestamp(last_sync, tz=timezone.utc).isoformat() if last_sync else None,
    }


@app.post("/analyze")
def analyze(event: EventVector):
    return analyze_event(event)
