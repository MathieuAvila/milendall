"""
Helpers to load and validate JSON file
"""

from __future__ import annotations

from typing import Any, Callable, cast

import logging
import os
import yaml
import json
import jsonschema
from jsonschema.validators import validator_for
from referencing import Registry, Resource
import json5

SCHEMA_LOCATION = "../../schema/"

logger = logging.getLogger("json_helper")
logger.setLevel(logging.INFO)

def _handler(path: str) -> object:
    if not path.startswith('http://'):
        raise Exception('Not a http URL: {}'.format(path))
    path_no_scheme = path[len('http://'):]
    name = os.path.basename(path_no_scheme)
    schema_path = SCHEMA_LOCATION + name
    real_path = os.path.realpath(schema_path)
    with open(real_path, "r") as f:
        j = json.load(f)
        return j


def _load_json_or_yaml(json_path: str,
                       object_hook: Callable[[dict[str, object]], object] | None = None) -> object:
    try_yaml = False
    try_jsonc = False
    try:
        with open(json_path, "r") as read_file:
            obj = json.load(read_file, object_hook=object_hook)
            return obj
    except (OSError, json.JSONDecodeError):
        try_jsonc = True
    if try_jsonc:
        try:
            jsonc_path = os.path.splitext(json_path)[0] + '.jsonc'
            with open(jsonc_path, "r") as read_file:
                obj_jsonc = json5.load(read_file)
                obj = json.loads(json.dumps(obj_jsonc), object_hook=object_hook)
                return obj
        except jsonschema.exceptions.ValidationError:
            raise
        except Exception:
            try_yaml = True
    if try_yaml:
        yaml_path = os.path.splitext(json_path)[0] + '.yaml'
        with open(yaml_path, "r") as read_file:
            obj_yaml = yaml.safe_load(read_file)
        obj = json.loads(json.dumps(obj_yaml), object_hook=object_hook)

    return obj

def _build_registry(schema_path: str) -> Registry:
    def _retrieve(uri: str) -> Resource:
        if uri.startswith("file://"):
            path = uri[len("file://"):]
            with open(path, "r") as read_file:
                data = json.load(read_file)
            return Resource.from_contents(data)
        if uri.startswith("http://"):
            data = _handler(uri)
            return Resource.from_contents(data)
        raise ValueError(f"Unsupported schema URI: {uri}")

    with open(schema_path, "r") as read_schema_file:
        schema = json.load(read_schema_file)
    registry = Registry(retrieve=_retrieve).with_resource(  # type: ignore[call-arg]
        "file://" + schema_path,
        Resource.from_contents(schema),
    )
    return registry


def load_and_validate_json(json_path: str, schema_name: str,
                           decode_hook: Callable[[dict[str, object]], object] | None = None) -> object:
    schema_path = os.path.realpath(SCHEMA_LOCATION + schema_name)
    registry = _build_registry(schema_path)
    with open(schema_path, "r") as read_schema_file:
        schema = json.load(read_schema_file)

    obj = _load_json_or_yaml(json_path)
    validator_cls = validator_for(schema)
    validator_cls.check_schema(schema)
    validator_cls(schema, registry=registry).validate(cast(Any, obj))

    obj = _load_json_or_yaml(json_path, object_hook=decode_hook)
    return obj

def check_json_fragment(fragment: object, schema_name: str) -> None:
    '''If such schema exists, check against it. Otherwise keep going'''
    schema_path = os.path.realpath(SCHEMA_LOCATION + schema_name)
    try:
        with open(schema_path, "r") as read_schema_file:
            schema = json.load(read_schema_file)
    except Exception:
        logger.warning("Schema does not exist: %s",schema_path)
        return
    registry = _build_registry(schema_path)
    validator_cls = validator_for(schema)
    validator_cls.check_schema(schema)
    validator_cls(schema, registry=registry).validate(cast(Any, fragment))

class JSONEncoder(json.JSONEncoder):

    def default(self, o: object) -> object:
        if hasattr(o, "values"):
            return o.values
        else:
            return {}

def dump_json(obj: object) -> str:
    """dump internal state for later use"""
    return json.dumps(obj, cls=JSONEncoder, indent=1)
