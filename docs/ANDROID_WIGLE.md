# Android WiGLE workflow

1. Run WiGLE WiFi Wardriving during an authorized survey. Do not treat observed networks as households, ISP evidence, or contact consent.
2. In WiGLE, open Database and choose **CSV Export Run** for the current run or **CSV Export DB** for the full local database. The app can export CSV to device storage/share sheet. WiGLE documents both export modes and CSV support in its [Android project](https://github.com/wiglenet/wigle-wifi-wardriving).
3. Open FiberScout on the R510 from a trusted local browser. Sign in to the authenticated dashboard.
4. Open **WiGLE imports**, choose the CSV through Android’s file picker, add a route label, and select Import survey. `.csv`, `.wiglecsv`, and gzip-compressed `.gz` exports are supported.
5. Review import counts, then use Overview, Map, and Analytics. Map cells are coarse aggregates; raw BSSID coordinates and customer addresses are not published to the map.
6. Review voluntary leads separately. Follow-up is manual; the system does not automatically contact anyone.

The dashboard binds to `127.0.0.1` by default. Use a secure local access method already approved for the server; do not expose it publicly without a separate deployment review.
