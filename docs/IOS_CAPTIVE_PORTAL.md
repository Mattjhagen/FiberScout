# Simple offline customer page

The landing page shows Matthew's photo, phone number, email and all four original
plan cards immediately, using the pinned site's black, white and magenta theme.
Phone and email appear above the plans and again in the footer. No Safari setup
or app handoff is required to read the information. A short note explains that
customers can use their phone apps directly if a contact link is blocked.

Call and email remain optional links; plan SMS buttons and the download button
are removed from the customer page. No message is sent automatically. Email
requires internet. The original local assets remain bundled, including the
photo, stylesheet, icons and vCard. Previously shared `/safari.html` addresses
serve the same simplified page. No JavaScript or external assets are added.

The device's portrait car-mode UI, magenta header/footer and Wi-Fi recovery
changes from PR #6 remain included.

## iOS constraints

No documented reliable web API was found to force the captive viewer into full
Safari. This version avoids requiring that handoff altogether.

- Apple captive Wi-Fi guidance: https://support.apple.com/en-us/102554
- Apple URL scheme reference: https://developer.apple.com/library/archive/featuredarticles/iPhoneURLScheme_Reference/

## Validation

Python tests check local resources, preserved assets, all four plans, visible
contact information before the plans, absence of the old walkthrough and SMS
buttons, compatibility route and deterministic generation. Native tests check
route inventory and gzip assets; the cardputer-adv firmware is compiled.

Physical checks remain pending: join the hotspot on iPhone and Android, confirm
photo, contact information and plans are readable offline, and verify device
orientation. App links may still be restricted by the captive viewer; the visible
number and email are the primary contact fallback. No hardware was flashed here.
