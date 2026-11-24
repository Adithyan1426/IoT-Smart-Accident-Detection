CREATE TABLE IF NOT EXISTS telemetry (
 id INTEGER PRIMARY KEY AUTOINCREMENT, received_at TEXT NOT NULL,
 sender_id INTEGER, hop_count INTEGER, sequence INTEGER, accident INTEGER NOT NULL,
 ax REAL, ay REAL, az REAL, lat REAL, lng REAL, speed REAL, altitude REAL,
 satellites INTEGER, gps_valid INTEGER NOT NULL DEFAULT 0
);
