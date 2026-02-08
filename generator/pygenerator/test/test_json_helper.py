"""
test level
"""

from __future__ import annotations

import logging
import unittest

import json_helper
import jsonschema

logging.basicConfig()
logging.getLogger("test_json_helper").setLevel(logging.INFO)

class TestJsonHelper(unittest.TestCase):

    def test_invalid(self) -> None:
        """ test loading invalid json"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/invalid.json", "event.json")
        except jsonschema.exceptions.ValidationError:
            has_error = True
        self.assertTrue(has_error)

    def test_valid_basic(self) -> None:
        """ test loading a valid json, with no sub schema"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/valid_simple.json", "event.json")
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

    def test_valid_complex(self) -> None:
        """ test loading a valid json, with sub schema"""
        has_error = False
        try:
            json_helper.load_and_validate_json("../test/test_samples/json/valid_complex.json", "file_rooms_logic.json")
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

    def test_check_fragment_exist_success(self) -> None:
        """ test checking fragment against sub schema, with successful fragment"""
        has_error = False
        try:
            json_helper.check_json_fragment([{"lang":"fr", "value":"merde"}], "name.json")
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

    def test_check_fragment_exist_failure(self) -> None:
        """ test checking fragment against sub schema, with successful fragment"""
        has_error = False
        try:
            json_helper.check_json_fragment({"lang":"fr"}, "name.json") # not an array, and missing value
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertTrue(has_error)

    def test_check_fragment_no_schema(self) -> None:
        """ test checking fragment against sub schema, with successful fragment"""
        has_error = False
        try:
            json_helper.check_json_fragment({"lang":"fr"}, "missing_schema.json") # not an array, and missing value
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)

    def test_yaml(self) -> None:
        """ test fallback to yaml"""
        has_error = False
        try:
            obj: dict[str, str] = json_helper.load_and_validate_json(
                "../test/test_samples/json/valid.json",
                "event.json",
            ) # valid yaml
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)
        self.assertEqual(obj, {"action": "", "direction": "", "on_property": "", "on_trigger": "plop", "to_value": "plip"})

    def test_jsonc(self) -> None:
        """ test fallback to jsonc"""
        has_error = False
        try:
            obj: dict[str, str] = json_helper.load_and_validate_json(
                "../test/test_samples/json/valid_jsonc.json",
                "event.json",
            ) # valid yaml
        except jsonschema.exceptions.ValidationError as e:
            print("EXCEPTION %s" % repr(e))
            has_error = True
        self.assertFalse(has_error)
        self.assertEqual(obj, {"action": "", "direction": "", "on_property": "", "on_trigger": "", "to_value": ""})

if __name__ == '__main__':
    unittest.main()
