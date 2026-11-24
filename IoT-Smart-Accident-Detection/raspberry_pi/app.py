import os
import sqlite3
from datetime import datetime, timezone
from pathlib import Path
from flask import Flask, jsonify, render_template, request

BASE = Path(__file__).resolve().parent
DB = Path(os.environ.get("ACCIDENT_DB", BASE / "accident.db"))
app = Flask(__name__)

SCHEMA = '''CREATE TABLE IF NOT EXISTS telemetry (
 id INTEGER PRIMARY KEY AUTOINCREMENT,
 received_at TEXT NOT NULL, sender_id INTEGER, hop_count INTEGER,
 sequence INTEGER, accident INTEGER NOT NULL,
 ax REAL, ay REAL, az REAL, lat REAL, lng REAL,
 speed REAL, altitude REAL, satellites INTEGER,
 gps_valid INTEGER NOT NULL DEFAULT 0
);'''

def connect():
    conn = sqlite3.connect(DB)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    with connect() as db:
        db.execute(SCHEMA)

@app.get("/")
def index():
    return render_template("dashboard.html")

@app.get("/health")
def health():
    return jsonify(status="ok")

@app.post("/api/telemetry")
def telemetry():
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify(error="Expected JSON object"), 400
    try:
        row = {
            "received_at": datetime.now(timezone.utc).isoformat(),
            "sender_id": int(data.get("sender_id", 1)),
            "hop_count": int(data.get("hop_count", 0)),
            "sequence": int(data.get("sequence", 0)),
            "accident": int(bool(int(data["accident"]))),
            "ax": float(data["ax"]), "ay": float(data["ay"]), "az": float(data["az"]),
            "lat": float(data.get("lat", 0)), "lng": float(data.get("lng", 0)),
            "speed": float(data.get("speed", 0)), "altitude": float(data.get("altitude", 0)),
            "satellites": int(data.get("satellites", 0)),
            "gps_valid": int(bool(data.get("gps_valid", False)))
        }
    except (KeyError, TypeError, ValueError):
        return jsonify(error="Missing or invalid telemetry fields"), 400
    with connect() as db:
        db.execute('''INSERT INTO telemetry
        (received_at,sender_id,hop_count,sequence,accident,ax,ay,az,lat,lng,
         speed,altitude,satellites,gps_valid)
        VALUES (:received_at,:sender_id,:hop_count,:sequence,:accident,:ax,:ay,:az,
         :lat,:lng,:speed,:altitude,:satellites,:gps_valid)''', row)
    return jsonify(status="stored", sequence=row["sequence"]), 201

@app.get("/api/data")
def latest():
    with connect() as db:
        item = db.execute("SELECT * FROM telemetry ORDER BY id DESC LIMIT 1").fetchone()
    return jsonify(dict(item) if item else {})

@app.get("/api/history")
def history():
    try:
        limit = max(1, min(int(request.args.get("limit", 100)), 500))
    except ValueError:
        limit = 100
    with connect() as db:
        rows = db.execute("SELECT * FROM telemetry ORDER BY id DESC LIMIT ?", (limit,)).fetchall()
    return jsonify([dict(row) for row in rows])

init_db()
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False)
