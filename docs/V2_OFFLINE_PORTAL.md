# FiberScout V2 — Offline plan portal

This branch contains a self-contained captive-portal webpage derived from the public **Mattjhagen/tmobile-pass** `index.html` and `style.css` (inspected November 17, 2026). It reproduces the four displayed plans and contact links; prices and availability must be verified before field use.

## Operation

The Cardputer ADV broadcasts its configured access point (default: `Try T-Mobile Fiber`) and serves `http://192.168.4.1/` locally. DNS redirects captive network-detection queries toward this local page. It is not an internet service. The site does not ask for or record prospect information.

The landing page includes:

- Fiber 300: $35/month, original display $55
- Fiber 1 Gig: $55/month, original display $70
- Fiber 2 Gig: $70/month, original display $80
- Fiber 5 Gig: $90/month, original display $100
- Plan-specific prefilled SMS composer links and Call, Text, Email links
- An offline downloadable `.vcf` contact card

**Caveat:** `sms:`, `tel:`, `mailto:`, and vCard download actions depend on the mobile OS, browser, and captive-network mini-browser. The page explains that customers may need to open `http://192.168.4.1/` in a regular browser or disconnect from the hotspot and use cellular data. Drafting an SMS does not send it; sending an email requires connectivity.

The offline portal intentionally does not initialize V1's lead-storage or sync functionality, but flashing only the application may leave older stored personal data in flash. Do not repurpose or distribute devices holding V1 customer information without properly wiping it.

## Build / flash on Mac

```sh
git fetch origin
git switch --track origin/feature/v2-offline-plan-portal
python3 -m pip install --user platformio # if needed; alternatively use an existing pio install
pio run -e cardputer-adv
ls /dev/cu.usb*
# Replace PORT with the verified Cardputer ADV USB serial port:
pio run -e cardputer-adv -t upload --upload-port PORT
pio device monitor -b 115200 -p PORT
```

Confirm the device/port before upload, and preserve existing firmware/settings if needed. Bootloader mode: power OFF, hold G0 while powering on, if normal upload fails. Do not assume a particular serial port. Use synthetic / no personal data for initial tests.

## Manual QA

1. Verify SSID broadcasts, join from Android, confirm captive portal opens or browse `http://192.168.4.1/`.
2. Enable airplane mode with WiFi explicitly re-enabled to test that the plan page and contact vCard are locally served.
3. Inspect all four prices and the four prefilled SMS bodies.
4. Test SMS, tel, mailto, and contact download in the captive mini-browser and in Chrome / Safari as available.
5. Confirm no forms or lead collection endpoints remain active.
6. Test a reconnect after device power cycle and verify Cardputer dashboard.

No actual Mac/USB build, flashing, or phone QA has been performed by the GitHub connector. A local PlatformIO build is required before use.
