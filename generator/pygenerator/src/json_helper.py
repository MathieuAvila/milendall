"""
Helpers to load and validate JSON file
"""

import logging
import pathlib
import os
import yaml
import json
import jsonschema
import json5

SCHEMA_LOCATION = "../../schema/"

logger = logging.getLogger("json_helper")
logger.setLevel(logging.INFO)

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

def _load_json_or_yaml(json_path, object_hook=None):
    try_yaml = False
    try_jsonc = False
    try:
        with open(json_path, "r") as read_file:
            obj = json.load(read_file, object_hook=object_hook)
            return obj
    except:
        try_jsonc = True
    if try_jsonc:
        try:
            jsonc_path = os.path.splitext(json_path)[0] + '.jsonc'
            with open(jsonc_path, "r") as read_file:
                obj_jsonc = json5.load(read_file)
                obj = json.loads(json.dumps(obj_jsonc), object_hook=object_hook)
                return obj
        except jsonschema.exceptions.ValidationError as e:
            raise
        except Exception as e:
            try_yaml = True
    if try_yaml:
        yaml_path = os.path.splitext(json_path)[0] + '.yaml'
        with open(yaml_path, "r") as read_file:
            obj_yaml = yaml.load(read_file)
        obj = json.loads(json.dumps(obj_yaml), object_hook=object_hook)

    return obj

def load_and_validate_json(json_path, schema_name, decode_hook=None):
    schema_path = SCHEMA_LOCATION + schema_name
    real_path = os.path.realpath(schema_path)
    with open(real_path, "r") as read_schema_file:
        schema = json.load(read_schema_file)
    schemaurl = "file://" + real_path
    resolver = jsonschema.RefResolver(schemaurl, referrer=schema, handlers = { 'http': _handler , "file": _handler} )
    #resolver = jsonschema.RefResolver.from_schema(schema)

    obj = _load_json_or_yaml(json_path)
    jsonschema.validate(instance=obj, resolver=resolver, schema=schema)

    obj = _load_json_or_yaml(json_path, object_hook=decode_hook)
    return obj

def check_json_fragment(fragment, schema_name):
    '''If such schema exists, check against it. Otherwise keep going'''
    schema_path = SCHEMA_LOCATION + schema_name
    real_path = os.path.realpath(schema_path)
    try:
        with open(real_path, "r") as read_schema_file:
            schema = json.load(read_schema_file)
    except Exception as e:
        logger.warning("Schema does not exist: %s",real_path)
        return
    schemaurl = "file://" + real_path
    resolver = jsonschema.RefResolver(schemaurl, referrer=schema, handlers = { 'http': _handler , "file": _handler} )
    jsonschema.validate(instance=fragment, resolver=resolver, schema=schema)

class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        if hasattr(o, "values"):
            return o.values
        else:
            return {}

def dump_json(obj):
    """dump internal state for later use"""
    return json.dumps(obj, cls=JSONEncoder, indent=1)
