"""
Helpers to create an animation in a room
"""

from __future__ import annotations

from typing import Protocol, TypedDict

from gltf_helper import create_accessor


class NodeProvider(Protocol):
    def get_node_rank(self, node_name: str) -> int:
        """Return the node index for a given name."""


class ActionKey(TypedDict):
    time: float
    value: list[float]

class Animation:

    name: str
    action_list: list[dict[str, object]]
    at_false: float
    at_true: float
    event: str

    def __init__(self, name: str, at_false: float, at_true: float, event: str) -> None:
        """
        name -- This is the name of the animation as it is refefenced in the final file
        at_false -- is intended to be at this timestamp when event is FALSE
        at_true -- is intended to be at this timestamp when event is TRUE
        event -- linked to this event name
        """
        self.name = name
        self.action_list = []
        self.at_false = at_false
        self.at_true = at_true
        self.event = event

    ACTION_TRANSLATION = "translation"
    ACTION_ROTATION = "rotation"
    ACTION_SCALE = "scale"

    def append_action(self, node_name: str, action: str, key_table: list[ActionKey]) -> None:
        """
        append an action table to a node
        node_name -- the node to which the action applies
        action -- one of the previously defined constants
        key_table -- a table of elements of type: [time=FLOAT, value=VEC3]
        """
        self.action_list.append({"node":node_name, "action": action, "table": key_table})

    def append_prefix(self, prefix: str) -> None:
        for anim in self.action_list:
            anim["node"] = prefix + anim["node"]

    def generate_gltf(self, gltf: dict[str, object], data_file: object,
                      node_provider: NodeProvider) -> None:
        animation = {
            "name" : self.name,
            "channels" : [],
            "samplers" : []
        }
        for counter, action in enumerate(self.action_list):
            values_list = [ key["value"] for key in action["table"]]
            values_accessor = create_accessor(
                        data_file,
                        gltf,
                        values_list)
            timestamp_list = [ key["time"] for key in action["table"]]
            timestamp_accessor = create_accessor(
                        data_file,
                        gltf,
                        timestamp_list)
            sampler = {
                "input" : timestamp_accessor,
                "output" : values_accessor,
                "interpolation":"LINEAR"
            }
            channel = {
                "sampler":counter,
                "target": {
                    "node" : node_provider.get_node_rank(action["node"]),
                    "path" : action["action"]
                }
            }
            animation["samplers"].append(sampler)
            animation["channels"].append(channel)
        if not "animations" in gltf:
            gltf["animations"] = []
        gltf["animations"].append(animation)
        gltf_extras = gltf["extras"]
        if "animations" not in gltf_extras:
            gltf_extras["animations"] = []
        gltf_animations = gltf_extras["animations"]
        gltf_animations.append({
            "name" : self.name,
            "false" : self.at_false,
            "true" : self.at_true,
            "event" : self.event
            })

        # add metadata information for room. What to animate, on which event ?

