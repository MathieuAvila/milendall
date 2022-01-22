"""
test level
"""

import logging

import unittest

import json_helper
import jsonschema

logging.basicConfig()
logging.getLogger("test_json_helper").setLevel(logging.INFO)

class TestJsonHelper(unittest.TestCase):

    def test_invalid(self):
        """ test loading invalid json"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/invalid.json", "event.json")
        except jsonschema.exceptions.ValidationError:
            has_error = True
        self.assertTrue(has_error)

    def test_valid_basic(self):
        """ test loading a valid json, with no sub schema"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/valid_simple.json", "event.json")
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

    def test_valid_complex(self):
        """ test loading a valid json, with sub schema"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/valid_complex.json", "file_rooms_logic.json")
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

if __name__ == '__main__':
    unittest.main()
