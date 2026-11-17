import importlib.util
from html.parser import HTMLParser
from pathlib import Path
import re
import unittest
from urllib.parse import urljoin, urlparse

root = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('bundle', root / 'scripts/build_portal.py')
bundle = importlib.util.module_from_spec(spec)
spec.loader.exec_module(bundle)

class Links(HTMLParser):
    def __init__(self, text):
        super().__init__(); self.resources=[]; self.actions=[]; self.feed(text)
    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag in ('img','script','link'):
            self.resources.append(attrs.get('src', attrs.get('href', '')))
        if tag == 'a': self.actions.append(attrs.get('href',''))

class OfflineBundle(unittest.TestCase):
    def setUp(self): self.assets = bundle.assets()
    def test_all_dependencies_local(self):
        page = Links(self.assets['/'][1].decode())
        for resource in page.resources:
            self.assertFalse(urlparse(resource).scheme, resource)
            self.assertIn(urljoin('/',resource), self.assets)
        for path, (_, data) in self.assets.items():
            if path.endswith('.css'):
                for resource in re.findall(r'url\([\"\']?([^\)\"\']+)', data.decode()):
                    self.assertIn(urljoin(path,resource), self.assets)
    def test_simple_page_and_assets(self):
        source = root / 'tmobile-pass'
        home = self.assets['/'][1].decode()
        self.assertEqual(self.assets['/safari.html'], self.assets['/'])
        self.assertIn('Call or text: (402) 358-7059', home)
        self.assertIn('Matt.hagen1@t-mobile.com', home)
        self.assertLess(home.index('Call or text:'), home.index('pricing-section'))
        for plan in ('Fiber 300', 'Fiber 1 Gig', 'Fiber 2 Gig', 'Fiber 5 Gig'):
            self.assertIn(plan, home)
        self.assertNotIn('sms:', home)
        self.assertNotIn('Select Plan', home)
        self.assertNotIn('Open in Safari', home)
        self.assertNotIn('<script', home)
        self.assertIn('tel:4023587059', Links(home).actions)
        for name in ('style.css','avatar.JPG','favicon.png','manifest.json','Matthew_Hagen_TMobile_Fiber.vcf'):
            self.assertEqual(self.assets['/'+name][1], (source/name).read_bytes())

    def test_deterministic_generation(self):
        bundle.build(); before=(root/'include/portal_assets.h').read_bytes()
        bundle.build(); self.assertEqual(before,(root/'include/portal_assets.h').read_bytes())

if __name__ == '__main__': unittest.main()
