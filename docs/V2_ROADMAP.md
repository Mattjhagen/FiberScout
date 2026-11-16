# FiberScout V2 roadmap

## Implemented on this branch
- Magenta, black, and white captive portal styling with responsive form.
- Lead submission still persists through `appendLead` before proceeding.
- HTTP 303 after successful POST to a local `/continue` page.
- Continue page links to https://fiber.mattjhagen.xyz/ and explains the need to leave the internet-free AP.
- No external navigation on a failed save.

## Before deployment
- Build with `pio run -e cardputer-adv` and run `pio test -e native`.
- Validate on actual Cardputer ADV and Pixel captive-portal browser.
- Review user-provided collector/privacy strings for HTML escaping.
- Review authorized use of T-Mobile marks and brand assets.
- Confirm whether the public site supports a post-lead landing page.

## Next milestones
1. Encrypted-at-rest local lead storage and explicit data retention/deletion.
2. Authenticated, TLS-protected sync and duplicate-safe acknowledgements.
3. Provisioning for a trusted sync destination without exposing admin controls to hotspot visitors.
4. Session stats and a field dashboard.
5. Release image and checksums; OTA update strategy after security review.

V1 data is unencrypted; do not collect real customer PII on this build until security milestones are complete.
