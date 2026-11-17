# FiberScout — Car WiFi reliability upgrade

This change keeps the existing V2.1 captive portal and its offline assets unchanged.

- Requests the ESP32-S3 supported 19.5 dBm transmit-power setting, without modifying the regulatory domain or adding an amplifier. The radio may clamp the effective power; range is not guaranteed.
- Disables WiFi sleep where applicable.
- Keeps SoftAP at 192.168.4.1.
- Checks AP mode and IP every 10 seconds; only attempts to restore the AP if unhealthy continuously for 30 seconds. Zero connected clients is **not** a failure.
- Exposes radio TX level (integer dBm), AP restart count, uptime, free heap, and client count on Cardputer display.
- Does not require internet or a Siyata hotspot, and does not change customer portal contents.

## Test on Mac before merging

```sh
cd ~/FiberScout
git fetch origin
git switch --track origin/feature/car-wifi-reliability
pio run -e cardputer-adv
```

After a successful build, verify the actual USB port before uploading:

```sh
ls /dev/cu.usb*
pio run -e cardputer-adv -t upload --upload-port /dev/cu.usbmodemXXXX
```

Test continuous operation overnight, stable captive loading, and a walking range check on Android/iPhone. Confirm the reported TX value and watch the AP reset count. Do not assume higher transmit power improves the return link from the phone.

**Important vehicle safety**: A USB accessory continuously powered by a car's always-hot socket can drain the starter battery. Use a switched accessory supply or appropriate low-voltage cut-off, and secure the device away from direct sun/heat. Ensure the customer-facing SSID and any hotspot operation comply with your employer's policies and applicable rules.

This code has not yet been compiled or flashed in this GitHub-only edit.
