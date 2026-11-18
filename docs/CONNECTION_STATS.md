# Cardputer connection statistics

Firmware adds a second, portrait-format display screen with the same magenta/black/white design as the car dashboard.

**Controls:** hold **Fn** and use the arrow keys:
- **Fn + Right** or **Fn + Down**: connection statistics
- **Fn + Left** or **Fn + Up**: hotspot status
- **Space**: toggle the existing canvassing session without changing screens

## What is counted

An ESP32-S3 SoftAP **station association (join)** is counted once when the WiFi stack reports a join. Reconnects are new events; the figures are **not** unique people, households, phone identities, or page visits. No MAC addresses, IPs, SSIDs from visitors, or personal identifiers are saved.

- Today — current calendar day, America/Chicago.
- Last 24 hours — current and previous 23 UTC hour buckets (hour-level approximation).
- Last 7 days — current and previous 6 local calendar days.
- Last 30 days — current and previous 29 local calendar days.
- Lifetime — all recorded association events, persisted across firmware restarts.

All are cumulative join events; they are not current connected clients (shown on the main screen).

## Offline clock limitation

The Cardputer ADV has no persistent battery-backed wall-clock calendar. After a cold boot, **today / 24h / 7d / 30d show CLOCK NOT SET** until someone opens the offline portal with a phone whose browser runs JavaScript. The portal sends the phone's Unix timestamp to the Cardputer using an on-device `/__clock` endpoint. No external network or hotspot is required. This is a convenience clock source, **not authenticated or tamper-proof**; an incorrect phone clock can distort the time-window statistics.

Associations recorded in the first 5 minutes of a boot can be assigned to the first accepted phone time. Older events without a known clock contribute only to lifetime, not falsely to a day.

The Cardputer retains hourly and calendar-day totals in NVS, and only writes dirty counters after at least 30 seconds. Sudden power loss may lose up to around 30 seconds of recent joins. History is limited to 24 hour buckets and 31 day buckets; statistics are incomplete if the clock was not set during portions of operation.

## Testing on the Mac / Cardputer ADV

```sh
cd ~/FiberScout
git fetch origin
git switch --track origin/feature/cardputer-connection-stats
pio run -e cardputer-adv
pio test -e native
ls /dev/cu.usb*
# Check target port before replacing existing firmware:
pio run -e cardputer-adv -t upload --upload-port /dev/cu.usbmodemXXXX
```

After flashing: confirm portal still loads and the source website layout is unchanged; test Fn+Right / Fn+Left navigation and Space. With another phone, connect to the AP and observe lifetime increment; open the captive portal and confirm date buckets become available. Turn off the Cardputer and reboot it; lifetime should persist, whereas date stats should say CLOCK NOT SET until portal time resynchronizes. Test a join without opening the portal, a reconnect, and several simultaneous clients.

This branch has not yet been physically flashed or compiled on the user's Mac.
