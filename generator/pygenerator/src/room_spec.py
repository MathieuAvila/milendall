"""
Definition of a RoomSpec.
Those are the high-level definition embedded in rooms_logic, and contains
Containers definitions
Do not confuse them with Room element which are the containers for bricks.
"""

from __future__ import annotations

from typing import Mapping

from munch import DefaultMunch
import room
from state import LevelState

import logging

from typing_defs import RoomSpecValues, SelectorLike, TextWriter

logger = logging.getLogger("room_spec")
logger.setLevel(logging.INFO)

class WriteableString():
    content: str

    def __init__(self) -> None:
        self.content = ""

    def write(self, string: str) -> None:
        self.content = self.content + string

class RoomSpec():

    values: RoomSpecValues
    level_directory: str
    state: LevelState
    room: room.Room | None
    selector: SelectorLike

    def __init__(self, values: Mapping[str, object], level_directory: str,
                 state: LevelState, selector: SelectorLike) -> None:
        self.values = DefaultMunch.fromDict(values)  # type: ignore[assignment]
        if getattr(self.values, "parameters", None) is None:
            self.values.parameters = DefaultMunch.fromDict({
                "structure_class": self.values.structure_class,
                "dressing_class": self.values.dressing_class,
                "structure_private": None,
                "structure_parameters": None,
                "dressing_private": None,
                "dressing_parameters": None,
            })
        if getattr(self.values, "objects", None) is None:
            self.values.objects = None
        self.level_directory = level_directory
        # this is the reference to the Room in its directory.
        # It is instantiated when needed.
        self.state = state
        self.room = None
        self.selector = selector
        logger.debug("New room_spec %s from %s" % (self.values.room_id, self.level_directory) )

    def get_class(self) -> str:
        """Return the element class for selector lookup."""
        return "room"

    def get_id(self) -> str:
        """Return the room spec identifier."""
        return self.values.room_id

    def dump_graph(self, output: TextWriter) -> None:
        """
        dump a graphviz repr of a room, only spec part
        """

        logger.debug("Dump room_spec %s" % (self.values.name))

        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)

        # collect portals and write them out
        gate_ids: list[str] = []
        for brick in self.room.values.bricks:
            portals = brick.values.portals
            if portals is not None:
                for portal in portals:
                    gate_ids.append(portal.gate_id)
        for g in gate_ids:
            output.write('"' + g + '" [ shape=hexagon style=filled color=yellow ];\n')

        # now you can dump the room
        label = self.values.room_id
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in self.values.triggers])+"</B>"
        if self.values.structure_class is not None:
            label += "<BR/><I>S: "+ self.values.structure_class + "</I>"
        if self.values.dressing_class is not None:
            label += "<BR/><I>D: "+ self.values.dressing_class + "</I>"
        output.write('subgraph "cluster_' + self.values.room_id +'" {\n')

        output_room = WriteableString()
        output_main = WriteableString()

        logger.debug("Go to dump room %s" % (self.values.name))
        self.room.dump_graph(output_room, output_main)

        output.write(output_room.content)

        output.write('style=filled;\n')
        output.write('color=lightgrey;\n')
        output.write('label=< ' + label+ ' >;\n')
        output.write('};\n')

        output.write(output_main.content)


    def structure_personalization(self) -> None:
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(LevelState.Instantiated)
        self.room.structure_personalization()
        self.state = LevelState.Personalized

    def dressing_instantiation(self) -> None:
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(LevelState.Personalized)
        self.room.dressing_instantiation()
        self.state = LevelState.DressingInstantiated

    def dressing_personalization(self) -> None:
        """Run the brick personalization process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(LevelState.DressingInstantiated)
        self.room.dressing_personalization()
        self.state = LevelState.DressingPersonalized

    def finalize(self, output_directory: str, preview: bool = False) -> None:
        """Run the final process on the real room"""
        if self.room is None:
            self.room = room.Room(self.level_directory, self.values.room_id, self.selector)
            self.room.load(LevelState.DressingPersonalized)
        self.room.finalize(output_directory, preview)

    def save(self, output_directory: str) -> None:
        if self.room is not None:
            self.room.save(output_directory)
