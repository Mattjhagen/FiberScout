# Architecture

WiFi SoftAP provides an isolated network. DNSServer resolves all names to the AP address. WebServer exposes only the public landing page, POST endpoint, and common captive-portal probes. Lead handling uses bounded fields, explicit consent, duplicate detection, and a temporary-file commit on LittleFS.

Leads are versioned JSON records. Sessions assign local sequence IDs and preserve event order without GPS or inferred residence. The display is an operational dashboard and SPACE starts/stops a session. There is no public administrative UI.

The sync module is a disabled extension point. It recognizes only an explicitly configured HTTPS URL and token; CA provisioning, transport, and backend contract are deferred until a private backend is selected.
