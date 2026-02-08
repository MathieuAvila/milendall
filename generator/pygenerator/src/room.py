"""
Definition of room. Not to be confused with room_spec (RoomSpec)
"""

from __future__ import annotations

from typing import Callable

import json

import brick
import concrete_room
import pathlib
from element import Element
import json_helper
from state import LevelState
import logging
import cgtypes.mat4
import cgtypes.vec3

from munch import DefaultMunch

from typing_defs import BrickLike, RoomValues, SelectorLike, TextWriter

logger = logging.getLogger("room")
logger.setLevel(logging.INFO)

class Room(Element):

    filename_map: dict[LevelState, str] = {
        LevelState.Instantiated : "room-instantiated.json",
        LevelState.Personalized : "room-personalized.json",
        LevelState.DressingInstantiated : "room-dressing-instantiated.json",
        LevelState.DressingPersonalized : "room-dressing-personalized.json",
    }

    name: str
    state: LevelState
    level_directory: str
    selector: SelectorLike
    values: RoomValues

    def __init__(self, level_directory: str, name: str, selector: SelectorLike) -> None:
        '''Initialize a Room to defaults'''
        self.name = name
        self.state = LevelState.New
        self.level_directory = level_directory
        self.selector = selector
        values: dict[str, object] = {
                "bricks":[]
            }
        self.values = DefaultMunch.fromDict(values)  # type: ignore[assignment]
        if getattr(self.values, "parameters", None) is None:
            self.values.parameters = DefaultMunch.fromDict({
                "structure_class": None,
                "dressing_class": None,
                "structure_private": None,
                "structure_parameters": None,
                "dressing_private": None,
                "dressing_parameters": None,
            })
        if getattr(self.values, "objects", None) is None:
            self.values.objects = None

    def decode_room(self) -> Callable[[dict[str, object]], object]:
        """automatically set object type"""
        def _decode_room(dct: dict[str, object]) -> object:
            if 'b_id' in dct:
                return brick.Brick(dct, self.selector)
            return dct
        return _decode_room

    def load(self, state: LevelState) -> None:
        self.state = state
        logger.info("Load room %s from: %s, state: %s" % (self.name, self.level_directory, self.state))
        file_name = self.level_directory + "/" + self.name + "/" + self.filename_map[state]
        values = json_helper.load_and_validate_json(
                file_name,
                "file_room_bricks.json",
                decode_hook=self.decode_room())
        logger.debug("Has read: %s", values)
        self.values = DefaultMunch.fromDict(values)  # type: ignore[assignment]
        if getattr(self.values, "parameters", None) is None:
            self.values.parameters = DefaultMunch.fromDict({
                "structure_class": None,
                "dressing_class": None,
                "structure_private": None,
                "structure_parameters": None,
                "dressing_private": None,
                "dressing_parameters": None,
            })
        if getattr(self.values, "objects", None) is None:
            self.values.objects = None
        logger.debug("Has values: %s", self.values)

    def add_brick(self, brick: BrickLike) -> None:
        self.values.bricks.append(brick)

    def structure_personalization(self) -> None:
        '''Instantiate everything'''
        logger.info("Personalization room %s " % (self.name))
        self.state = LevelState.Personalized
        for brick in self.values.bricks:
            brick.structure_personalization()

    def dressing_instantiation(self) -> None:
        '''Instantiate everything'''
        logger.info("Dressing instantiation room %s " % (self.name))
        self.state = LevelState.DressingInstantiated
        for brick in self.values.bricks:
            brick.dressing_instantiation()

    def dressing_personalization(self) -> None:
        '''Instantiate everything'''
        logger.info("Dressing personalization room %s " % (self.name))
        self.state = LevelState.DressingPersonalized
        for brick in self.values.bricks:
            brick.dressing_personalization()

    def save(self, level_directory: str | None = None) -> None:
        '''Save to file. File depends on instantiated or not'''
        if level_directory is None:
            level_directory = self.level_directory
        file_name = self.filename_map[self.state]

        room_path = level_directory + "/" + self.name
        room_file = room_path + "/" + file_name
        pathlib.Path(room_path).mkdir(parents=True, exist_ok=True)
        dump_payload: dict[str, object] = {
            "$schema": "file_room_bricks.json",
            "bricks": self.values.bricks,
        }
        if self.values.human_name is not None:
            dump_payload["human_name"] = self.values.human_name
        _j = json.dumps(dump_payload, cls=json_helper.JSONEncoder, indent=1)
        with open(room_file, "w") as output_file:
            output_file.write(_j)

    def dump_graph(self, output_room: TextWriter, output_main: TextWriter) -> None:
        """
        dump a graphviz repr of a room
        """

        label = self.name
        logger.info("Dump room %s" % (self.name))
        for brick in self.values.bricks:
            brick.dump_graph(output_room, output_main, self.name)
        #output.write('"' + self.name +'" ' + '[ label=< ' + label+ ' > ] ;\n')

    def finalize(self, level_directory: str | None = None, preview: bool = False, concrete_test_param: concrete_room.ConcreteRoom | None = None) -> None:  # type: ignore[override]
        """ Perform final generate and dressing on one room."""

        logger.info("finalize room %s " % (self.name))

        if concrete_test_param is not None:
            concrete = concrete_test_param
        else:
            concrete = concrete_room.ConcreteRoom()

        for brick in self.values.bricks:
            logger.debug("in brick %s" % brick.values.b_id)
            concrete_brick = concrete_room.ConcreteRoom()
            brick.finalize(concrete_brick)
            concrete_brick.append_prefix(brick.values.b_id + "_")
            root_pad = brick.values.root_pad
            if root_pad is not None:
                root_id = root_pad.ref_b_id + "_" + root_pad.ref_pad_id
                parent_root_id = root_id
                logger.debug("set root pad to: %s", root_id)
                if root_pad.translation is not None or root_pad.rotation is not None:
                    mat = cgtypes.mat4(1.0)
                    if root_pad.rotation is not None:
                        r = root_pad.rotation
                        if not isinstance(r, dict):
                            raise RuntimeError("Invalid root pad rotation")
                        axis = r.get("axis")
                        angle = r.get("angle")
                        if not isinstance(axis, list) or not isinstance(angle, (int, float)):
                            raise RuntimeError("Invalid root pad rotation values")
                        mat = mat * cgtypes.mat4.rotation(angle, cgtypes.vec3(axis[0], axis[1], axis[2]))  # type: ignore[arg-type]
                    if root_pad.translation is not None:
                        t = root_pad.translation
                        mat = cgtypes.mat4.translation(cgtypes.vec3(t[0],t[1],t[2])) * mat  # type: ignore[arg-type]
                    parent_root_id = "%s%s%s" %( root_id , "_root_", brick.values.b_id)
                    concrete.add_child(root_id, parent_root_id, mat)
                concrete_brick.set_root(parent_root_id)
            concrete.merge(concrete_brick)
        if self.values["human_name"]:
            concrete.add_private_data({"human_name":self.values["human_name"]})
        if level_directory is None:
            level_directory = self.level_directory
        room_path = level_directory + "/" + self.name
        pathlib.Path(room_path).mkdir(parents=True, exist_ok=True)
        concrete.generate_gltf(room_path)
        if preview:
            concrete_room.preview(room_path + "/room.gltf", room_path + "/room_preview.gltf")
