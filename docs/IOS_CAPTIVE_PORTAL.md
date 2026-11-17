# iOS captive portal handoff

FiberScout is an offline access point. The Wi-Fi login viewer is not full Safari.
There is no documented web API that reliably forces this viewer to launch Safari.
`target="_blank"`, window.open, redirects and guessed Safari schemes are not a
supported handoff contract. We do not spoof successful internet connectivity or
use automatic app-launch attempts.

The always-visible, JavaScript-free guide gives a selectable HTTP LAN address,
manual Safari steps, conditional Share > Open in Safari guidance, and a warning
that Cancel can disconnect Wi-Fi. It appears on every device because user-agent
strings cannot reliably distinguish every captive viewer from regular Safari.

The root page preserves the pinned site's Android SMS links and all assets.
`/safari.html` uses iOS `sms:number&body=...` links for the same five messages,
and omits the contact download attribute so Safari can handle the vCard.
This is an interoperability convention requiring device testing, not an Apple
guarantee: Apple's archived SMS scheme reference documents only the recipient.
The guide also supplies a phone number and message for manual app use.
No clipboard API, Share API, external script, CDN or internet service is required.

## Research

- Apple captive Wi-Fi guidance: https://support.apple.com/en-us/102554
  Cancel can disassociate Wi-Fi; Use Without Internet keeps association.
- Apple URL scheme reference: https://developer.apple.com/library/archive/featuredarticles/iPhoneURLScheme_Reference/
  Documents native/web URL integrations, not a CNA-to-Safari launch API.
- Apple developer discussion (historical, not an API guarantee):
  https://developer.apple.com/forums/thread/62947

## Phone acceptance checks (pending physical devices)

1. Join Try T-Mobile Fiber on iPhone; verify guidance appears before contact
   actions in the captive viewer. No automatic navigation should occur.
2. Long-press/copy the address; leave the viewer, retain/rejoin Wi-Fi, open
   Safari manually and load http://192.168.4.1/safari.html. Also test typing
   http://192.168.4.1 and following Safari text & contact actions.
3. Try Share > Open in Safari only if offered. Verify the manual route works
   when Share and clipboard functionality are unavailable.
4. In Safari tap all four plan links and Text Me. Verify recipient and message
   in Messages, then cancel without sending. Preview and save the local vCard.
5. Test manual Messages/Contacts fallback. Disconnect from hotspot only after
   loading the page/card if carrier data is needed. Email requires internet.
6. On Android test the original root-page links in Chrome and the captive viewer;
   confirm photos/icons/plans/card load with internet unavailable.

Automated bundle tests cover local resources, unchanged original actions/assets,
the alternate SMS URLs and vCard link, static handoff content and deterministic
bundling. Firmware compilation cannot prove iOS app handoff or contact import.

## Build validation — November 17, 2026

- Nine Python tests passed; native route/gzip contract test passed.
- `pio run -e cardputer-adv` succeeded: RAM 50,196 bytes (15.3%);
  application 1,574,941 bytes (54.6% of app slot), after integrating car mode.
- No device was flashed. iOS/Android acceptance checks above remain pending.
- Includes the car Wi-Fi/display changes from PR #6, including the latest
  portrait orientation and footer fixes, alongside the Safari portal fix.
