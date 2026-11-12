"""Bounded localhost integration smoke test using only synthetic fixture data."""
import http.cookiejar, json, os, subprocess, time, urllib.request, uuid
from urllib.error import HTTPError

port = "8792"
env = {**os.environ, "FIBERSCOUT_DB": "./data/smoke-script.sqlite", "ADMIN_PASSWORD_BCRYPT": "$2b$04$yNTVRxLqQ4slsE0nKh64IeGmzJmYt4bUV/tJiC4yK.9JUGkDHuNWy", "SESSION_SECRET": "smoke-secret", "PORT": port}
proc = subprocess.Popen(["node", "dist-server/server/index.js"], env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
try:
    time.sleep(0.7)
    jar = http.cookiejar.CookieJar(); opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(jar))
    try: opener.open(f"http://127.0.0.1:{port}/api/overview"); raise AssertionError("unauthenticated request was accepted")
    except HTTPError as e: assert e.code == 401
    def post_json(path, body, headers=None):
        data=json.dumps(body).encode(); h={"content-type":"application/json", **(headers or {})}; return opener.open(urllib.request.Request(f"http://127.0.0.1:{port}{path}",data=data,headers=h,method="POST"))
    post_json("/api/login", {"username":"admin","password":"test-pass"})
    token=json.load(opener.open(f"http://127.0.0.1:{port}/api/csrf"))["csrf"]
    boundary="----FiberScout"+uuid.uuid4().hex
    sample=open("tests/fixtures/sample.wigle.csv","rb").read()
    body=(f"--{boundary}\r\nContent-Disposition: form-data; name=label\r\n\r\nsynthetic\r\n--{boundary}\r\nContent-Disposition: form-data; name=file; filename=sample.wigle.csv\r\nContent-Type: text/csv\r\n\r\n").encode()+sample+f"\r\n--{boundary}--\r\n".encode()
    try:
        result=json.load(opener.open(urllib.request.Request(f"http://127.0.0.1:{port}/api/imports",data=body,headers={"content-type":f"multipart/form-data; boundary={boundary}","x-csrf-token":token},method="POST")))
    except HTTPError as e:
        raise AssertionError(e.read().decode()) from e
    assert result["rowsImported"] == 3 and result["rowsInvalid"] == 1
    overview=json.load(opener.open(f"http://127.0.0.1:{port}/api/overview")); assert overview["observations"] == 3 and overview["areas"] == 1
    print("smoke: unauthenticated=401 login=ok import=ok aggregation=ok")
finally:
    proc.terminate(); proc.wait(timeout=3)
