# FiberScout device synchronization schema

The firmware remains independently functional and does not transmit lead data over its open AP. This versioned envelope is the contract for a future authenticated HTTPS sync endpoint:

```json
{"schemaVersion":1,"deviceId":"generated-at-provisioning","sessionId":"local-session-id","firmwareVersion":"1.1.0","submissionCount":0,"leads":[]}
```

The eventual endpoint must use TLS certificate validation, an explicitly provisioned device credential, replay protection, and server-side authorization. No credentials or API keys are stored in this repository. Until that provisioning work is complete, Cardputer leads remain local and must not be exposed through the captive portal.
