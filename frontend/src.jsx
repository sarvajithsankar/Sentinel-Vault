import React, { useEffect, useState } from "react";
import { createRoot } from "react-dom/client";
import "./style.css";

const API = "http://localhost:8000";
function Card({ label, value }) { return <article><p>{label}</p><strong>{value}</strong></article>; }

function App() {
  const [events, setEvents] = useState([]);
  const [threats, setThreats] = useState([]);
  const [stats, setStats] = useState({ total_events: 0, threats_detected: 0, mirrors_active: 0, last_sync_time: null });
  const refresh = async () => {
    const [eventsResponse, threatsResponse, statsResponse] = await Promise.all([fetch(`${API}/events`), fetch(`${API}/threats`), fetch(`${API}/stats`)]);
    setEvents((await eventsResponse.json()).items); setThreats(await threatsResponse.json()); setStats(await statsResponse.json());
  };
  useEffect(() => { refresh(); }, []);
  const syncTime = stats.last_sync_time ? new Date(stats.last_sync_time).toLocaleString() : "Not synced";
  return <main>
    <header><div><h1>Sentinel-Vault</h1><p>SIEM dashboard</p></div><button onClick={refresh}>Refresh</button></header>
    <section className="stats"><Card label="Total Events" value={stats.total_events} /><Card label="Threats Detected" value={stats.threats_detected} /><Card label="Mirrors Active" value={stats.mirrors_active} /></section>
    <section><h2>Event log</h2><table><thead><tr><th>Timestamp</th><th>IP</th><th>Type</th><th>Status</th><th>Score</th></tr></thead><tbody>{events.map((event, index) => <tr key={`${event.timestamp}-${index}`}><td>{new Date(event.timestamp).toLocaleString()}</td><td>{event.ip}</td><td>{event.type}</td><td>{event.status}</td><td>{event.threat_score}</td></tr>)}</tbody></table></section>
    <section><h2>Threat panel</h2><ul className="threats">{threats.map((threat) => <li key={threat.ip}>{threat.ip}<span className={threat.threat_score > 70 ? "high-score" : ""}>{threat.threat_score}</span></li>)}</ul></section>
    <section><h2>RAID status</h2><div className="raid"><div><strong>Mirror 1</strong><span>{syncTime}</span><b>{stats.mirrors_active >= 1 ? "OK" : "OUT OF SYNC"}</b></div><div><strong>Mirror 2</strong><span>{syncTime}</span><b>{stats.mirrors_active >= 2 ? "OK" : "OUT OF SYNC"}</b></div></div></section>
  </main>;
}
createRoot(document.getElementById("root")).render(<App />);
