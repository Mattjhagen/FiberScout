# Security and privacy notes

- Voluntary lead fields only; no nearby-device scan, MAC collection, password collection, or automatic client association.
- Consent is separate, unchecked, and recorded with a version.
- Input lengths and argument count are bounded; user values are not reflected into the landing page.
- LittleFS records are not encrypted in V1. Do not use V1 for unattended real-customer deployment until device-key provisioning and authenticated export are implemented.
- There is no public export or delete endpoint. Deletion is deferred to secure wired maintenance.
- No company authorization, availability, pricing, or legal language is asserted. Collector and privacy text are configurable.
