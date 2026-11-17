#include "web_portal.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"

extern FiberConfig config;
static WebServer server(80);
static DNSServer dns;

static const char kPortal[] PROGMEM = R"FIBERHTML(<!DOCTYPE html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#e20074"><title>T-Mobile Fiber | Matt Hagen</title>
<style>
:root{color-scheme:dark;--pink:#e20074;--bg:#101013;--panel:#1b1b20;--muted:#b7b5bc}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:white;font:15px -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif}
main{max-width:540px;margin:auto;padding:18px 14px 50px}
header{text-align:center;padding:26px 10px 14px}.eyebrow{color:#ff55ab;letter-spacing:.11em;text-transform:uppercase;font-size:12px;font-weight:750}
h1{font-size:28px;line-height:1.15;margin:12px 0 8px}h2{font-size:20px;margin:28px 0 12px}p{color:var(--muted);line-height:1.5;margin:10px 0}
.card{background:var(--panel);border:1px solid #36343a;border-radius:17px;padding:19px;margin:12px 0}
.pop{border-color:var(--pink);box-shadow:0 0 16px #e200741c}.tag{display:inline-block;color:white;background:var(--pink);border-radius:999px;padding:5px 10px;font-size:11px;font-weight:800}
.planhead{display:flex;justify-content:space-between;align-items:center;gap:12px}
.planhead strong{font-size:19px}.speed{color:#cac8cc;font-size:12px}.price{font-weight:850;font-size:30px;margin:12px 0}
.price small{font-size:13px;font-weight:400;color:var(--muted)}
.old{color:#b5b1b7;text-decoration:line-through;font-size:14px;font-weight:400}
ul{padding-left:20px;color:#ddd9de;font-size:13px;line-height:1.8}
a.btn{display:block;border:1px solid #5b5259;border-radius:11px;padding:14px;text-align:center;color:white;text-decoration:none;font-weight:750;margin-top:12px;min-height:46px}
a.btn.primary{background:var(--pink);border-color:var(--pink)}
.actions{display:grid;grid-template-columns:1fr 1fr;gap:8px}.actions a{margin:0}
.note{background:#29252a;border-left:3px solid #e20074;border-radius:8px;padding:12px;font-size:13px;line-height:1.55;color:#eee}
a.inline{color:#ff79bb}footer{text-align:center;color:#999;font-size:12px;padding-top:18px}
</style></head><body><main>
<header><div class="eyebrow">Local Fiber Information</div><h1>Matt Hagen</h1>
<p>T-Mobile Fiber · Direct-to-Door Sales</p>
<p>Explore plans and reach out directly. This page is served from a nearby device and works without internet.</p>
</header>
<div class="card"><div class="actions">
<a class="btn primary" href="sms:+14023587059?body=Hi%20Matthew%2C%20I%27m%20interested%20in%20T-Mobile%20Fiber!">Text me</a>
<a class="btn" href="tel:+14023587059">Call me</a>
<a class="btn" href="mailto:Matt.hagen1@t-mobile.com?subject=T-Mobile%20Fiber%20Question">Email me</a>
<a class="btn" href="/Matthew_Hagen_TMobile_Fiber.vcf">Save contact</a>
</div></div>
<div class="note"><b>No internet provided:</b> You can browse all plans here. Calling, texting, and downloading a contact may require opening this page in your normal browser. Email generally requires internet to send. If an action doesn't open, disconnect from this Wi-Fi and use mobile data. Nothing is sent automatically.</div>
<h2>Fiber Plans</h2><p>Plan details reproduced from fiber.mattjhagen.xyz. Offers and eligibility are subject to confirmation.</p>
<section class="card"><div class="planhead"><strong>Fiber 300</strong><span class="speed">300 Mbps</span></div><div class="price"><span class="old">$55</span> $35 <small>/month</small></div>
<ul><li>Wi-Fi router included</li><li>Installation included</li><li>Unlimited data</li><li>$10 AutoPay discount</li></ul>
<a class="btn primary" href="sms:+14023587059?body=Hi%20Matthew%2C%20I%20want%20to%20sign%20up%20for%20Fiber%20300!">Select Fiber 300 · Text Matt</a></section>
<section class="card pop"><span class="tag">MOST POPULAR</span><div class="planhead"><strong>Fiber 1 Gig</strong><span class="speed">1 Gbps</span></div><div class="price"><span class="old">$70</span> $55 <small>/month</small></div>
<ul><li>Wi-Fi router included</li><li>Mesh Wi-Fi extender as needed</li><li>Installation included</li><li>Unlimited data</li></ul>
<a class="btn primary" href="sms:+14023587059?body=Hi%20Matthew%2C%20I%20want%20to%20sign%20up%20for%20Fiber%201%20Gig!">Select Fiber 1 Gig · Text Matt</a></section>
<section class="card"><div class="planhead"><strong>Fiber 2 Gig</strong><span class="speed">2 Gbps</span></div><div class="price"><span class="old">$80</span> $70 <small>/month</small></div>
<ul><li>Fastest speeds &amp; strongest Wi-Fi</li><li>Wi-Fi router &amp; mesh extender</li><li>Installation included</li><li>Unlimited data</li></ul>
<a class="btn primary" href="sms:+14023587059?body=Hi%20Matthew%2C%20I%20want%20to%20sign%20up%20for%20Fiber%202%20Gig!">Select Fiber 2 Gig · Text Matt</a></section>
<section class="card"><div class="planhead"><strong>Fiber 5 Gig</strong><span class="speed">5 Gbps</span></div><div class="price"><span class="old">$100</span> $90 <small>/month</small></div>
<ul><li>10 Year Price Guarantee</li><li>Headroom for everything at once</li><li>Installation included</li><li>Exclusive T-Mobile Tuesdays perks</li></ul>
<a class="btn primary" href="sms:+14023587059?body=Hi%20Matthew%2C%20I%20want%20to%20sign%20up%20for%20Fiber%205%20Gig!">Select Fiber 5 Gig · Text Matt</a></section>
<div class="card"><h2>Talk to Matt</h2><p><a class="inline" href="tel:+14023587059">(402) 358-7059</a></p><p><a class="inline" href="mailto:Matt.hagen1@t-mobile.com">Matt.hagen1@t-mobile.com</a></p>
<p>For the online version, disconnect from this Wi-Fi and open <b>fiber.mattjhagen.xyz</b> using mobile data.</p></div>
<footer>FiberScout V2 · Offline information portal · No information collected</footer>
</main></body></html>)FIBERHTML";

static const char kVcard[] PROGMEM = R"VCARD(BEGIN:VCARD
VERSION:3.0
N:Fiber;T-Mobile;;;
FN:T-Mobile Fiber
ORG:T-Mobile Fiber
TITLE:Direct-to-Door Sales Representative
TEL;TYPE=CELL,VOICE:+14023587059
EMAIL;TYPE=WORK:Matt.hagen1@t-mobile.com
URL:https://fiber.mattjhagen.xyz/
NOTE:Matthew Hagen - T-Mobile Fiber representative
END:VCARD
)VCARD";

static void landing() {
  server.sendHeader("Cache-Control","no-store");
  server.send_P(200,"text/html; charset=utf-8",kPortal);
}
static void contactCard() {
  server.sendHeader("Content-Disposition","attachment; filename=\"Matthew_Hagen_TMobile_Fiber.vcf\"");
  server.sendHeader("Cache-Control","no-store");
  server.send_P(200,"text/vcard; charset=utf-8",kVcard);
}
static void captiveRedirect() {
  server.sendHeader("Location","http://192.168.4.1/",true);
  server.send(302,"text/plain","");
}
void portalBegin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(config.ssid.c_str());
  dns.start(53,"*",WiFi.softAPIP());
  server.on("/",HTTP_GET,landing);
  server.on("/index.html",HTTP_GET,landing);
  server.on("/Matthew_Hagen_TMobile_Fiber.vcf",HTTP_GET,contactCard);
  server.on("/generate_204",HTTP_GET,captiveRedirect);
  server.on("/hotspot-detect.html",HTTP_GET,captiveRedirect);
  server.on("/connecttest.txt",HTTP_GET,captiveRedirect);
  server.on("/ncsi.txt",HTTP_GET,captiveRedirect);
  server.onNotFound(captiveRedirect);
  server.begin();
}
void portalLoop() {
  dns.processNextRequest();
  server.handleClient();
}
