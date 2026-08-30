from __future__ import annotations

from pydantic import BaseModel, Field
from sklearn.ensemble import IsolationForest


class EventVector(BaseModel):
    ip_freq: int = Field(ge=0)
    hour: int = Field(ge=0, le=23)
    event_type_encoded: int = Field(ge=0)


_training_vectors = [
    [1, 9, 0], [2, 10, 0], [1, 14, 1], [3, 16, 1], [2, 12, 2],
    [1, 18, 0], [4, 11, 2], [2, 15, 1], [3, 13, 0], [1, 17, 2],
]
_model = IsolationForest(contamination=0.2, random_state=42).fit(_training_vectors)


def analyze_event(event: EventVector) -> dict[str, bool | float]:
    vector = [[event.ip_freq, event.hour, event.event_type_encoded]]
    return {"anomaly": bool(_model.predict(vector)[0] == -1), "score": float(_model.decision_function(vector)[0])}
