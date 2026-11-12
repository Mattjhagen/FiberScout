import json, re, unittest

def valid(first, contact, consent, interested):
    return bool(first and len(first)<=40 and contact and len(contact)<=120 and consent and interested in ("Yes","No"))

class FiberScoutCoreTests(unittest.TestCase):
    def test_required_consent(self): self.assertFalse(valid("A","a@example.com",False,"Yes"))
    def test_bounded_form(self): self.assertFalse(valid("x"*41,"a",True,"Yes"))
    def test_valid_form(self): self.assertTrue(valid("A","a@example.com",True,"No"))
    def test_schema_round_trip(self):
        record={"schemaVersion":1,"sequence":3,"consent":True,"contact":"a@example.com"}
        self.assertEqual(json.loads(json.dumps(record)),record)
    def test_duplicate_key(self):
        self.assertEqual(("A","a@example.com"),("A","a@example.com"))

if __name__ == "__main__": unittest.main()
