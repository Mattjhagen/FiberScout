CREATE TABLE IF NOT EXISTS wifi_observation_staging(
  id INTEGER PRIMARY KEY,
  import_job_id INTEGER NOT NULL REFERENCES import_jobs(id) ON DELETE CASCADE,
  observed_at TEXT, bssid TEXT NOT NULL, ssid TEXT, auth_mode TEXT,
  channel INTEGER, frequency_mhz INTEGER, rssi INTEGER, latitude REAL,
  longitude REAL, altitude_m REAL, accuracy_m REAL, network_type TEXT NOT NULL,
  source_row_hash TEXT NOT NULL,
  UNIQUE(import_job_id, source_row_hash)
);
CREATE INDEX IF NOT EXISTS idx_wifi_staging_job ON wifi_observation_staging(import_job_id);
