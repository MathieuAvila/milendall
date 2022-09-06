"""
Additional brick that brings only gravity on a parametric function
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4
import math

from .register import register_brick_type

from math import *

from jsonmerge import merge

logger = logging.getLogger("gravity_sampler")
logger.setLevel(logging.INFO)

class BrickGravitySampler(BrickStructure):

    _name = "gravity_sampler"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return BrickGravitySampler(room)

    def check_fit(self):
        """ Pass the Room, and list of gates, check it can be applied. """
        logger.info("checking if gravity_sampler fits")

    def check_structure(self):
        """check everything is as expected.
        """
        logger.info("checking if gravity_sampler is ok.")
        return True

    def instantiate(self, selector):
        """ force set values:
        - set values to room size"""
        structure_parameters = self._element.values.parameters.structure_parameters
        if structure_parameters == None:
            self._element.values.parameters.structure_parameters = {}
            structure_parameters = self._element.values.parameters.structure_parameters

        my_default = {
            # 100 samples, from 0.0. to 1.0
            "segments_nr": 100,
            # linear alongside X from 0.0 to 10.0, this is LUA code ! You can use "t" for more parametrization
            "func": [ "(u)*10", "0.0", "0.0"],
            # gravity flipping around Y, this is LUA code, you can use time (t)
            "gravity": [ "0.0", "math.cos((u + t) * 6.28 / 10.0)", "0.0"],
            #"up": ["0.0", "cos(float(t) * 6.28", "0.0"], # UP direction this is LUA code !
            # Set this if you want UP to follow gravity
            "mode_up_gravity" : "follow",
            # no more distant than 2.0 from point, this is LUA code !
            "weight" : "(function() if distance < 2.0 then return 1.0 else return 0.0 end end)()",
            # recompute delay
            "frequency" : 1.0
        }
        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        logger.info("setup: %s", str(self._element.values.parameters.structure_private))

    def generate(self, concrete):
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.parameters.structure_private

        setup = structure_private
        parent = concrete.add_child(None, "parent")


        gravity=setup["gravity"]
        func=setup["func"]

        if setup["mode_up_gravity"] == "follow":
            up = setup["gravity"]
        else:
            up = setup["up"]

        parent.set_gravity_script(
            'function optimize_gravity(x, y, z, t, count)\n' +
            '   local tab_out = {}\n' +
            '   local dist = 100000000\n' +
            '   local current = -1\n' +
            '   for d = 0, count do\n' +
            '       local u = d / count\n' +
            '       local x0 = %s\n' % func[0]+
            '       local y0 = %s\n' % func[1] +
            '       local z0 = %s\n' % func[2] +
            '       local l_dist = math.sqrt((x0 - x)^2 + (y0 - y)^2 + (z0 - z)^2)\n' +
            '       if (l_dist < dist) then\n' +
            '           current = u\n' +
            '           dist = l_dist\n' +
            '       end\n' +
            '   end\n' +
            '   -- print("c=" .. current)\n'
            '   -- print("d=" .. dist)\n'
            '   tab_out["dist"] = dist\n' +
            '   tab_out["u"] = current\n' +
            '   return tab_out\n' +
            'end\n\n' +
            'local t = tab_in["t"]\n'
            'local optim_grav = optimize_gravity(tab_in["x"], tab_in["y"], tab_in["z"], t, %s)\n' % setup["segments_nr"] +
            'local distance = optim_grav["dist"]\n'
            'local u = optim_grav["u"]\n'
            'tab_out["g_x"] = %s \n' % gravity[0] +
            'tab_out["g_y"] = %s \n' % gravity[1] +
            'tab_out["g_z"] = %s \n' % gravity[2] +
            'tab_out["u_x"] = %s \n' % up[0] +
            'tab_out["u_y"] = %s \n' % up[1] +
            'tab_out["u_z"] = %s \n' % up[2] +
            'tab_out["w"] = %s\n' % setup["weight"] +
            'tab_out["v"] = %s\n' % setup["frequency"], [ concrete_room.Node.GRAVITY_SIMPLE ])



register_brick_type(BrickGravitySampler())
