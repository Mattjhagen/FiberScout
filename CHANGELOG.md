# Changelog

## 0.1.0 - 2026-11-12

- Initial offline AP/captive portal, consent-gated local lead storage, duplicate prevention, sessions, dashboard, and packaging.
- Deferred encrypted storage, authenticated export/delete, and live HTTPS sync.

## 0.2.0 - 2026-11-12

- Added authenticated localhost Node/SQLite neighborhood intelligence dashboard.
- Added resilient WiGLE 1.6/legacy CSV and gzip import with duplicate/error accounting.
- Added coarse aggregate map, import history, analytics, and consent-aware lead management.

## 0.2.1 - 2026-11-12

- Removed the vulnerable optional sqlite3 session-store dependency chain and pinned dependencies.
- Added SQLite-backed sessions, login rate limiting, strict CSP, proxy-aware secure cookies, and Zod request validation.
- Added permanent transactional lead erasure, staged transactional WiGLE imports, startup retention cleanup, integrity checks, and private Android deployment guidance.
