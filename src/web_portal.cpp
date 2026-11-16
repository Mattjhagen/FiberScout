#include "web_portal.h"
#include "lead_store.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"

extern FiberConfig config;
static WebServer server(80);
static DNSServer dns;
static const char* kNextUrl = "https://fiber.mattjhagen.xyz/";

static String page() {
  String s = R"HTML(<!doctype html><html lang="en"><head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<meta name="theme-color" content="#e20074">
<title>FiberScout | Fiber Internet</title>
<style>
:root{color-scheme:light;--magenta:#e20074;--ink:#17131a;--muted:#625c66;--line:#ddd7df}
*{box-sizing:border-box}body{margin:0;background:#f8f5f8;color:var(--ink);font:16px system-ui,-apple-system,sans-serif}
header{background:#111;color:#fff;padding:20px 22px;border-bottom:5px solid var(--magenta)}
.brand{font-size:22px;font-weight:850;letter-spacing:-.7px}.brand span{color:#ff4aa2}
main{max-width:600px;margin:24px auto;padding:0 16px 40px}
.card{background:#fff;border:1px solid #eee;border-radius:18px;padding:24px;box-shadow:0 8px 28px #1a0c1910}
.eyebrow{color:var(--magenta);font-weight:800;text-transform:uppercase;font-size:12px;letter-spacing:1px}
h1{font-size:clamp(27px,6vw,37px);line-height:1.14;margin:10px 0 14px;letter-spacing:-1px}
p{color:var(--muted);line-height:1.55}label{display:block;font-weight:650;font-size:14px;margin-top:15px}
input,select{display:block;width:100%;padding:13px;margin-top:7px;border:1px solid var(--line);border-radius:10px;background:white;color:var(--ink);font:inherit}
input:focus,select:focus{outline:2px solid var(--magenta);outline-offset:1px}
.consent{display:flex;gap:10px;align-items:flex-start;font-weight:400;line-height:1.45}
.consent input{width:auto;margin:4px 0 0;flex-shrink:0}
button,.cta{display:block;text-align:center;width:100%;padding:15px;border:0;border-radius:11px;background:var(--magenta);color:white;font:inherit;font-weight:800;text-decoration:none;margin-top:20px}
.fine{font-size:12px;color:var(--muted)}footer{text-align:center;font-size:12px;color:var(--muted);padding:18px}
</style></head><body><header><div class="brand">Fiber<span>Scout</span></div></header>
<main><section class="card"><div class="eyebrow">Fiber internet</div>
<h1>Better internet starts here.</h1>
<p>Interested in fiber internet in your neighborhood? Leave your information and request a follow-up.</p>
<form method="post" action="/submit">
<label>First name *<input name="first" maxlength="40" required autocomplete="given-name"></label>
<label>Last name (optional)<input name="last" maxlength="40" autocomplete="family-name"></label>
<label>Phone or email *<input name="contact" maxlength="120" required></label>
<label>Street address (optional)<input name="address" maxlength="120" autocomplete="street-address"></label>
<label>Current internet provider (optional)<input name="provider" maxlength="60"></label>
<label>Approximate monthly internet bill (optional)<input name="bill" maxlength="30"></label>
<label>Preferred contact method<select name="preferred"><option>Phone</option><option>Email</option><option>No preference</option></select></label>
<label>Interested in receiving more information?<select name="interested"><option>Yes</option><option>No</option></select></label>
<label class="consent"><input type="checkbox" name="consent" value="yes" required><span>I agree that the FiberScout operator may use this information to respond to my request. This is voluntary and is not marketing SMS consent.</span></label>
<p class="fine">Collector: )HTML";
  s += config.collector;
  s += "</p><p class='fine'>";
  s += config.privacyNotice;
  s += R"HTML(</p><button type="submit">Continue</button></form></section>
<footer>FiberScout field portal · Independent lead-interest collection</footer></main></body></html>)HTML";
  return s;
}

static String nextPage() {
  String s = R"HTML(<!doctype html><html lang="en"><head><meta name="viewport" content="width=device-width,initial-scale=1"><meta name="theme-color" content="#e20074">
<title>Continue to Fiber</title><style>
body{margin:0;background:#f8f5f8;color:#17131a;font:16px system-ui,sans-serif}
header{background:#111;color:#fff;padding:22px;border-bottom:5px solid #e20074;font-size:22px;font-weight:800}
main{max-width:560px;margin:32px auto;padding:0 18px}.card{background:#fff;padding:28px;border-radius:18px;box-shadow:0 8px 28px #1a0c1910}
h1{line-height:1.2}p{line-height:1.6;color:#5d5761}.cta{display:block;text-align:center;background:#e20074;color:white;text-decoration:none;font-weight:800;padding:16px;border-radius:12px;margin:24px 0}
small{color:#625c66}
</style></head><body><header>FiberScout</header><main><div class="card">
<h1>Continue to T-Mobile Fiber</h1>
<p>Your information was saved on this device. To visit the Fiber website, disconnect from the <strong>Try T-Mobile Fiber</strong> Wi-Fi network and use mobile data or another internet connection.</p>
<a class="cta" href="https://fiber.mattjhagen.xyz/" rel="noopener">Open fiber.mattjhagen.xyz</a>
<small>If the site doesn't load, disconnect from this Wi-Fi and open the address in your browser. The Cardputer hotspot does not provide internet access.</small>
</div></main></body></html>)HTML";
  return s;
}

static void landing() { server.send(200, "text/html", page()); }
static void submit() {
  if (server.args() > 10) { server.send(413, "text/plain", "Request too large"); return; }
  Lead l;
  l.first=server.arg("first"); l.last=server.arg("last"); l.contact=server.arg("contact");
  l.address=server.arg("address"); l.provider=server.arg("provider"); l.bill=server.arg("bill");
  l.preferred=server.arg("preferred"); l.interested=server.arg("interested"); l.consent=server.hasArg("consent");
  String e;
  if (!appendLead(l,e)) {
    server.send(400,"text/html","<h2>Request not saved</h2><p>"+e+"</p><a href='/'>Back</a>");
    return;
  }
  server.sendHeader("Location","/continue",true);
  server.send(303,"text/plain","");
}
static void continuePage() { server.send(200,"text/html",nextPage()); }
static void redirect() { server.sendHeader("Location","/",true); server.send(302,"text/plain",""); }
void portalBegin() {
  WiFi.mode(WIFI_AP); WiFi.softAP(config.ssid.c_str());
  dns.start(53,"*",WiFi.softAPIP());
  server.on("/",HTTP_GET,landing);
  server.on("/submit",HTTP_POST,submit);
  server.on("/continue",HTTP_GET,continuePage);
  server.on("/generate_204",HTTP_GET,redirect);
  server.on("/hotspot-detect.html",HTTP_GET,redirect);
  server.on("/connecttest.txt",HTTP_GET,redirect);
  server.on("/ncsi.txt",HTTP_GET,redirect);
  server.onNotFound(redirect);
  server.begin();
}
void portalLoop() { dns.processNextRequest(); server.handleClient(); }
