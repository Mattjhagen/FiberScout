# FiberScout V1

Offline-first lead-interest capture firmware for the M5Stack Cardputer ADV. It creates a local Wi-Fi access point and captive portal; it does not connect to nearby networks, claim internet access, collect Wi-Fi credentials, or expose an administrative portal to hotspot visitors.

## V1.1 neighborhood intelligence

The `feature/wigle-neighborhood-intelligence` branch adds a localhost-only authenticated React/Vite dashboard and Node/SQLite backend. It imports WiGLE Android CSV exports, keeps wireless observations separate from opt-in leads, aggregates observations into coarse map cells, and provides import history, map, analytics, and manual lead management. See [`docs/ANDROID_WIGLE.md`](docs/ANDROID_WIGLE.md) and [`docs/SYNC_SCHEMA_V1.md`](docs/SYNC_SCHEMA_V1.md).

```sh
npm install
cp .env.example .env
# Set ADMIN_PASSWORD_BCRYPT, SESSION_SECRET, and a local database path.
npm run dev
```

The backend listens only on `127.0.0.1:8787`. Production mode requires an explicit bcrypt admin hash and session secret. Never commit `.env`, exports, BSSIDs, or customer data.

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
