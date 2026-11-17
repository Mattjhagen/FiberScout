"""Preview the exact bundled responses on localhost without any network assets."""
from http.server import BaseHTTPRequestHandler, HTTPServer
from build_portal import assets
import gzip
bundle = assets()
class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        path = self.path.split('?',1)[0]
        if path == '/index.html': path = '/'
        if path not in bundle:
            self.send_response(302); self.send_header('Location','/'); self.end_headers(); return
        mime, data = bundle[path]; data = gzip.compress(data, mtime=0)
        self.send_response(200); self.send_header('Content-Type',mime)
        self.send_header('Content-Encoding','gzip'); self.send_header('Content-Length',str(len(data)))
        self.end_headers(); self.wfile.write(data)
HTTPServer(('127.0.0.1',8793),Handler).serve_forever()
