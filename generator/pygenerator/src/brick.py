"""
interface definition of a real brick
"""

from munch import DefaultMunch
import logging

from element import Element
import json_helper

logging.basicConfig()
logger = logging.getLogger("brick")
logger.setLevel(logging.INFO)

class Brick(Element):

    def __init__(self, values, selector):
        self.values = DefaultMunch.fromDict(values)
        self.selector = selector

        # check subparts against schema, if they exist
        v = self.values
        logger.info("Some schema checking for %s" % v.parameters.structure_class)
        if "pads" in v:
            for f in v.pads:
                if "definition" in f:
                    schema = "bricks/" + v.parameters.structure_class + "/pad.json"
                    logger.info("Check pad fragment against %s" % schema)
                    json_helper.check_json_fragment(f.definition, schema)
        if "parameters" in v:
            if "structure_parameters" in v.parameters:
                schema = "bricks/" + v.parameters.structure_class + "/structure_parameters.json"
                logger.info("Check structure_parameters fragment against %s" % schema)
                json_helper.check_json_fragment(v.parameters.structure_parameters, schema)


    def get_class(self):
        """ get my class for selector"""
        return "brick"

    def get_id(self):
        """ return ID depending on type"""
        return self.values.b_id

    def dump_graph(self, output_room, output_main, dump_prefix):
        """
        dump a graphviz repr of a brick
        """

        label = self.values.b_id
        v = self.values
        p = v.parameters
        my_name = dump_prefix + "_" + v.b_id
        if self.values.triggers is not None:
            label += "<BR/><B>".join(["\n"] + [t.trigger_id for t in v.triggers])+"</B>"
        if p.structure_class is not None:
            label += "<BR/><I>S: "+ p.structure_class + "</I>"
        if p.dressing_class is not None:
            label += "<BR/><I>D: "+ p.dressing_class + "</I>"
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
            for p in v.portals:
                i_portal = p.gate_id
                if p.connect == "A":
                    output_main.write('"' + my_name + '" -> "' + i_portal + '" [ label="' + p.connect +'"];\n')
                else:
                    output_main.write('"' + i_portal + '" -> "' + my_name + '" [ label="' + p.connect +'"];\n')

