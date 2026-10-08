import json
import unittest

import rapidxmltojson


class ParseTests(unittest.TestCase):
    def test_repeated_namespaces(self):
        xml = '<n:root xmlns:n="urn:test"><n:item>one</n:item><n:item>two</n:item></n:root>'
        expected = {"n:root": {"@xmlns:n": "urn:test", "n:item": ["one", "two"]}}
        for _ in range(1000):
            self.assertEqual(json.loads(rapidxmltojson.parse(xml)), expected)

    def test_default_namespace(self):
        self.assertEqual(
            json.loads(rapidxmltojson.parse('<root xmlns="urn:test"><item>one</item></root>')),
            {"root": {"@xmlns": "urn:test", "item": "one"}},
        )

    def test_repeated_errors_and_recovery(self):
        for _ in range(1000):
            for xml, expected in (
                ("<root><child>", "Parse error, Details: unexpected end of data"),
                ('<root a="', "Parse error, Details: expected ' or \""),
            ):
                with self.assertRaises(rapidxmltojson.RapidXMLError) as error:
                    rapidxmltojson.parse(xml)
                self.assertEqual(str(error.exception), expected)
            self.assertEqual(rapidxmltojson.parse("<root>ok</root>"), '{"root":"ok"}')


if __name__ == "__main__":
    unittest.main()
