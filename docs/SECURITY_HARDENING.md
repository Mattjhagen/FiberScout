# V1.1 security and private deployment

## Repairs

- Direct dependencies are pinned to tested versions; the vulnerable optional `connect-sqlite3`/`sqlite3` chain was removed.
- Admin routes require a regenerated session, SQLite-backed session persistence, CSRF token, and authentication.
- Login failures are rate-limited to five attempts per client address per 15 minutes.
- Production requires `ADMIN_PASSWORD_BCRYPT` and `SESSION_SECRET`; no production credential is in source.
- Cookies are HttpOnly, SameSite=Lax, eight-hour expiry, and Secure in production or when `TRUST_PROXY=1`.
- Helmet CSP allows only the application and OpenStreetMap tile images required by the map.
- API identifiers, labels, lead updates, and login bodies use Zod validation.
- Uploads are restricted by extension/type, one file, and `UPLOAD_MAX_BYTES`.
- Errors returned to clients are generic; database paths and stack traces are not returned.
- Lead deletion is permanent in the live SQLite database: lead events are deleted and the lead row is deleted in one transaction. The dashboard requires explicit confirmation.
- The legacy `deleted_at` column remains for schema compatibility, but the delete endpoint now physically erases the row and related event content.
- Raw WiGLE observations are retained for `RAW_OBSERVATION_RETENTION_DAYS` at startup; this does not delete opt-in leads.

Permanent deletion does not erase historical filesystem backups, WAL remnants, snapshots, or copied database files. Protect and expire backups separately. `npm run db:check` runs SQLite integrity and foreign-key checks.

## Private Android access

The application remains bound to `127.0.0.1`; no firewall, Nginx, Cloudflare Tunnel, Tailscale, or NetBird configuration is changed.

Preferred low-impact option: use an Android SSH client that supports local forwarding (Termux + OpenSSH, ConnectBot, or equivalent) and create:

```sh
ssh -N -L 8787:127.0.0.1:8787 USER@R510_HOSTNAME
```

Then open `http://127.0.0.1:8787` in the Android browser while the tunnel remains active. Use SSH keys and host verification. This is transport access, not HTTPS; keep it on a trusted private network and rely on dashboard authentication.

For an approved private HTTPS reverse proxy, set `TRUST_PROXY=1`, terminate TLS at the approved proxy, and forward only to `127.0.0.1:8787`. Do not enable this without an explicit review of proxy authentication, headers, certificates, and existing service configuration.

At inspection time the `tailscale` and `netbird` binaries were present, but their daemons could not be verified from this restricted session. No connectivity was changed.

## Launch

```sh
npm ci
cp .env.example .env
# Set a bcrypt hash and a random SESSION_SECRET.
npm run build
NODE_ENV=production npm start
```

For local development, `npm run dev` starts the backend on `127.0.0.1:8787` and Vite on `127.0.0.1:5173` with an `/api` proxy.
