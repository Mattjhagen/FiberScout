# FiberScout V2.1

The Cardputer ADV serves the original `tmobile-pass` page, CSS, photo, favicon,
manifest, vCard and locally vendored Font Awesome 6.4.0 solid icons. Its own
Wi-Fi access point provides the page at http://192.168.4.1/ with wildcard DNS
and captive redirects. No Siyata hotspot, R510 configuration or internet uplink
is used. The original four plan-specific SMS links, telephone and email actions
are retained. A collapsed help section explains normal-browser access and
phone-app limitations; a browser link cannot force every captive viewer to exit.

```sh
git submodule update --init
python3 -m unittest discover -s tests -p test_portal_bundle.py
pio test -e native
pio run -e cardputer-adv
```

`tmobile-pass` is a pinned Git submodule and the source of truth. Update it to a
reviewed website revision and rebuild; no assets are fetched at build time.
`scripts/build_portal.py` generates gzip assets in flash, automatically before
the PlatformIO firmware build. No LittleFS mount, format or filesystem upload
is needed. Existing personal-data storage is not initialized by this portal.
Font Awesome licensing is in `portal-vendor/fontawesome/LICENSE.txt`.

Before flashing, identify the USB device and ESP32-S3/8 MB flash, save a full
8 MB flash backup outside this repository, record its checksum, and inspect its
partition table and active app. Do not erase flash or upload a filesystem.
See [V2.1 validation](docs/V2_1_VALIDATION.md) for this build's verification and
remaining phone checks. Old V1 documentation below is historical.

# FiberScout V1

Offline-first lead-interest capture firmware for the M5Stack Cardputer ADV. It creates a local Wi-Fi access point and captive portal; it does not connect to nearby networks, claim internet access, collect Wi-Fi credentials, or expose an administrative portal to hotspot visitors.

## V1.1 neighborhood intelligence

The `feature/wigle-neighborhood-intelligence` branch adds a localhost-only authenticated React/Vite dashboard and Node/SQLite backend. It imports WiGLE Android CSV exports, keeps wireless observations separate from opt-in leads, aggregates observations into coarse map cells, and provides import history, map, analytics, and manual lead management. See [`docs/ANDROID_WIGLE.md`](docs/ANDROID_WIGLE.md) and [`docs/SYNC_SCHEMA_V1.md`](docs/SYNC_SCHEMA_V1.md).

```sh
npm ci
cp .env.example .env
# Set ADMIN_PASSWORD_BCRYPT, SESSION_SECRET, and a local database path.
npm run dev
```

The backend listens only on `127.0.0.1:8787`. Production mode requires an explicit bcrypt admin hash and session secret. Never commit `.env`, exports, BSSIDs, or customer data.

Security and private Android access details are in [`docs/SECURITY_HARDENING.md`](docs/SECURITY_HARDENING.md). Run `npm run db:check` to verify SQLite integrity. The Vite development server proxies `/api` to the localhost backend; production serves the built dashboard from the backend.

## Hardware

M5Stack Cardputer-Adv (K132-Adv): ESP32-S3FN8/Stamp-S3A, 8 MB flash, ST7789V2 240×135 display, TCA8418 keyboard on I²C (SDA 8, SCL 9, INT 11), and microSD (CS 12, MOSI 14, SCK 40, MISO 39). Manufacturer reference: https://docs.m5stack.com/en/core/Cardputer-Adv

PlatformIO uses the documented `esp32-s3-devkitc-1` definition, 8 MB flash, and M5Cardputer library.

## Build and tests

```sh
python3 -m pip install --user platformio
pio run
pio test -e native
```

The build reserves two OTA app slots and LittleFS. No physical device was connected or flashed here.

## Flash

Put the ADV in download mode: power switch OFF, hold G0 while powering on. Linux:

```sh
pio run -e cardputer-adv -t upload --upload-port /dev/ttyACM0
```

macOS:

```sh
pio run -e cardputer-adv -t upload --upload-port /dev/cu.usbmodemXXXX
```

Recommended upload selects bootloader, partitions, and application. The application image itself is at `0x10000`; `dist/FLASH_OFFSETS.txt` records the package offsets.

Default AP: `Try T-Mobile Fiber`; portal: `http://192.168.4.1/`. SPACE starts/stops a session.

## V1 limitation

Leads are stored as versioned NDJSON in LittleFS and are not encrypted at rest. V1 is suitable only for controlled tests. Authenticated wired export/delete, encrypted storage, and live HTTPS sync are deferred pending provisioning and backend decisions. The sync extension will not use HTTP.
