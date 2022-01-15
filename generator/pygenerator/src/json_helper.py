"""
Helpers to load and validate JSON file
"""

import logging
import pathlib
import os

import json
import jsonschema

SCHEMA_LOCATION = "../../schema/"

def _handler(path):
        if not path.startswith('http://'):
            raise Exception('Not a http URL: {}'.format(path))
        path_no_scheme = path[len('http://'):]
        name = os.path.basename(path_no_scheme)
        schema_path = SCHEMA_LOCATION + name
        real_path = os.path.realpath(schema_path)
        with open(real_path, "r") as f:
            j = json.load(f)
            return j

def load_and_validate_json(json_path, schema_name, decode_hook=None):
    schema_path = SCHEMA_LOCATION + schema_name
    real_path = os.path.realpath(schema_path)
    with open(real_path, "r") as read_schema_file:
        schema = json.load(read_schema_file)
    schemaurl = "file://" + real_path
    resolver = jsonschema.RefResolver(schemaurl, referrer=schema, handlers = { 'http': _handler , "file": _handler} )
    #resolver = jsonschema.RefResolver.from_schema(schema)

    with open(json_path, "r") as read_file:
        obj = json.load(read_file)
        jsonschema.validate(instance=obj, resolver=resolver, schema=schema)

    with open(json_path, "r") as read_file:
        obj = json.load(read_file, object_hook=decode_hook)
        return obj


class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        if hasattr(o, "values"):
            return o.values
        else:
            return {}

def dump_json(obj):
    """dump internal state for later use"""
    return json.dumps(obj, cls=JSONEncoder, indent=1)
