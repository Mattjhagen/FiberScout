# Optional Siyata SD7 hotspot uplink for reliable stats dates

FiberScout can now act as **WiFi AP + WiFi STA** simultaneously. The customer SSID `Try T-Mobile Fiber` remains the captive portal at `192.168.4.1`. The private STA connection is used only for NTP (clock synchronization); it does not intentionally share or route Siyata internet to visitors.

## Setup directly on the Cardputer

1. Turn on the Siyata SD7 hotspot and confirm it is broadcasting **2.4 GHz WiFi** with a WPA/WPA2 compatible password. ESP32-S3 does not support 5 GHz WiFi.
2. On Cardputer ADV, press **Fn + Down** (the `.` key with the down-arrow legend) to open **TIME / HOTSPOT**.
3. Press **Enter** to edit the hotspot's exact WiFi name (SSID); erase or type with the Cardputer keyboard.
4. Press **Enter** to edit the WPA password (8–63 characters); it appears masked. Press **Enter** again to save and attempt to connect.
5. Wait for **SIYATA: CONNECTED** and **TIME: NTP SYNCED**. Use **Fn + Right** to view stats and **Fn + Left** to return to the main screen.

This works without USB configuration or embedding a password into source control. SSID/password are saved locally in ESP32 nonvolatile Preferences **without flash encryption enabled**. Anyone with physical access to an extracted flash image could potentially recover them. Use a dedicated hotspot password and avoid sharing your main account credentials.

## Behavior and limitations

- If the hotspot goes out of range, the offline customer portal remains available and WiFi STA attempts reconnection approximately every 30 seconds.
- After boot, date-grouped stats display `CLOCK NOT SET` until either a portal visitor's phone supplies the time or internet NTP synchronization succeeds. NTP synchronization is preferred and is explicitly labeled on screen.
- The clock remains accurate while powered on, including during temporary Siyata disconnections. The ESP32-S3 does not have an independent always-on battery-backed calendar RTC across a complete power loss.
- Both WiFi roles use the ESP32-S3's **one radio**. Joining the Siyata's channel can change the customer AP channel and temporarily disconnect clients. Expect possible decreases in customer WiFi performance when simultaneously maintaining both networks.
- Connection counts represent WiFi association events, not individual people, and reconnects increment the counter. Historical periods with no valid clock remain incomplete; an NTP connection cannot reconstruct their exact dates.
- NTP synchronization must be reachable on the mobile hotspot (UDP 123 and functional DNS). A hotspot that blocks NTP cannot provide verified time.

## Build / verify

PR is initially draft. After merge into master:

```bash
cd ~/FiberScout
git switch master
git pull --ff-only origin master
pio run -e cardputer-adv
ls /dev/cu.usb*
# Replace port with the currently verified Cardputer ADV port:
pio run -e cardputer-adv -t upload --upload-port /dev/cu.usbmodem1101
```

Before merging, build the PR branch by fetching it explicitly if the local Git fetch configuration excludes new feature branches:

```bash
git fetch origin pull/10/head:review-siyata-ntp
git switch review-siyata-ntp
pio run -e cardputer-adv
```

Test the three on-device pages, full hostname/password entry and masking, uptime, local customer portal, connected clients, NTP status, and retention of WiFi details across power cycle. Check if the Siyata is configured for 2.4 GHz. No physical flash/build has been run by the GitHub-only editor.
