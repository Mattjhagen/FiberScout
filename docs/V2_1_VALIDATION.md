# V2.1 validation

## Automated checks

- Python tests resolve every HTML/CSS resource locally, compare original action
  links and binary assets byte-for-byte, check all four plan messages and verify
  deterministic generation.
- PlatformIO native tests check the generated route inventory and gzip payloads.
- PlatformIO builds the actual ESP32-S3 application with bundled assets.

## Hardware procedure

1. Identify USB serial device and verify ESP32-S3 with 8 MB flash. USB identity
   alone does not establish the board model; runtime M5 detection reports ADV.
2. Save full flash outside Git, with restrictive permissions and SHA-256. Inspect
   existing partitions and boot selection before choosing upload offsets.
3. Preserve NVS and the old data filesystem; never run erase_flash or uploadfs.
4. Verify serial boot, ADV detection and AP startup.
5. On a phone, connect to Try T-Mobile Fiber and keep the connection despite
   the no-internet warning. Check automatic captive entry and manual
   http://192.168.4.1/ access. Confirm photo, icons, all four plans and contact.
6. Try the four plan SMS links, call, email and vCard in both captive viewer and
   ordinary browser on iOS/Android. Do not actually send messages for testing.
   Email delivery requires internet. Record unsupported captive-viewer actions.

The Mac's current Wi-Fi, R510 and any router settings must remain untouched.

## Mac build, November 17, 2026

- Live site HTML matched the pinned `tmobile-pass` index byte-for-byte.
- Three Python bundle tests and the PlatformIO native asset test passed.
- PlatformIO cardputer-adv build passed: 50,156 bytes RAM (15.3%),
  approximately 1.55 MB application (53.8% of app slot).
- Local browser preview rendered the original profile photo, icons and layout.
- Connected USB 303A:1001 was verified as ESP32-S3, 8 MB flash.
- Full 8,388,608-byte backup saved outside this repository, SHA-256
  `ef73cb9d0b2ea1b94806e0039bda6c9bb06ad8695c7f72682f8a5caf3aba14ad`.
- Existing partition table matched the build; OTA metadata selected app0.
- Wrote only firmware at 0x10000; esptool verified its hash. No bootloader,
  partition, NVS, OTA metadata or filesystem write was performed.
- USB reset remained in ROM download mode. A physical power cycle and phone
  captive-portal check have been requested. On-device appearance, ADV runtime
  detection and Android/iOS action handoff remain pending until confirmed.
