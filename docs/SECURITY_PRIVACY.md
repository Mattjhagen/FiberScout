# Security and privacy notes

- Voluntary lead fields only; no nearby-device scan, MAC collection, password collection, or automatic client association.
- Consent is separate, unchecked, and recorded with a version.
- Input lengths and argument count are bounded; user values are not reflected into the landing page.
- LittleFS records are not encrypted in V1. Do not use V1 for unattended real-customer deployment until device-key provisioning and authenticated export are implemented.
- The management API has no unauthenticated export endpoint. Authenticated lead deletion permanently removes the live lead row and related event details in a transaction, after explicit dashboard confirmation.
- Historical backups, WAL files, snapshots, and copied databases require separate retention and destruction procedures.
- No company authorization, availability, pricing, or legal language is asserted. Collector and privacy text are configurable.
