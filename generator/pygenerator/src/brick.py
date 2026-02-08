"""
interface definition of a real brick
"""

from __future__ import annotations

from typing import Mapping

from munch import DefaultMunch
import logging


from element import Element
import json_helper
from typing_defs import BrickValues, SelectorLike, TextWriter

logging.basicConfig()
logger = logging.getLogger("brick")
logger.setLevel(logging.INFO)

class Brick(Element):

    values: BrickValues
    selector: SelectorLike

    def __init__(self, values: Mapping[str, object], selector: SelectorLike) -> None:
        self.values = DefaultMunch.fromDict(values)  # type: ignore[assignment]
        self.selector = selector

        # check subparts against schema, if they exist
        v = self.values
        logger.debug("Some schema checking for %s" % v.parameters.structure_class)
        if v.pads is not None:
            if v.parameters.structure_class is None:
                raise RuntimeError("Brick requires a structure class for pad validation")
            for f in v.pads:
                if getattr(f, "definition", None) is not None:
                    schema = "bricks/" + v.parameters.structure_class + "/pad.json"
                    logger.debug("Check pad fragment against %s" % schema)
                    json_helper.check_json_fragment(f.definition, schema)
        if v.parameters.structure_parameters is not None:
            if v.parameters.structure_class is None:
                raise RuntimeError("Brick requires a structure class for parameter validation")
            schema = "bricks/" + v.parameters.structure_class + "/structure_parameters.json"
            logger.debug("Check structure_parameters fragment against %s" % schema)
            json_helper.check_json_fragment(v.parameters.structure_parameters, schema)


    def get_class(self) -> str:
        """ get my class for selector"""
        return "brick"

    def get_id(self) -> str:
        """ return ID depending on type"""
        return self.values.b_id


    def dump_graph(self, output_room: TextWriter, output_main: TextWriter, dump_prefix: str) -> None:
        """
        dump a graphviz repr of a brick
        """

        logger.debug("Dump brick %s" % (self.values["b_id"]))
        label = self.values.b_id
        v = self.values
        params = v.parameters
        my_name = dump_prefix + "_" + v.b_id
        triggers = v.triggers
        if triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in triggers])+"</B>"
        if params.structure_class is not None:
            label += "<BR/><I>S: "+ params.structure_class + "</I>"
        if params.dressing_class is not None:
            label += "<BR/><I>D: "+ params.dressing_class + "</I>"
        #output.write('"' + dump_prefix + "_" + v.b_id +'" ' + '[ label=< ' + label+ ' > ] ;\n')

        #output_room.write('"' + my_name +'" ' + '[ label=< ' + label+ ' > ] ;\n')
        output_room.write('subgraph "cluster_' + my_name +'" {\n')
        output_room.write('label=< ' + label+ ' > ;\n')

        if v.root_pad is not None:
            r_pad = dump_prefix + "_" + v.root_pad.ref_b_id + "_" + v.root_pad.ref_pad_id
            output_room.write('"' + my_name + '" -> "' + r_pad + '";\n')
        if v.pads is not None:
            for i in v.pads:
                i_pad = dump_prefix + "_" + v.b_id + "_" + i.pad_id
                output_room.write('"' + i_pad + '" -> "' + my_name + '"[constraint=false];\n')
                output_room.write('"' + i_pad + '" ' + '[ shape=ellipse style="filled,dashed" color=black fillcolor=lightblue border=black style=filled label=< ' + i.pad_id+ ' > ] ;\n')

        output_room.write('};\n')

        if v.portals is not None:
            for portal in v.portals:
                i_portal = portal.gate_id
                if portal.connect == "A":
                    output_main.write('"' + my_name + '" -> "' + i_portal + '" [ label="' + portal.connect +'" ];\n')
                else:
                    output_main.write('"' + i_portal + '" -> "' + my_name + '" [ label="' + portal.connect +'" ];\n')

