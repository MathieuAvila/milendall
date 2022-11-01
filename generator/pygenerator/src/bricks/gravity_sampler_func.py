"""
Additional brick that brings only gravity on a parametric function
"""

import logging
from brick_structure import BrickStructure
import concrete_room
import cgtypes.mat4
import functools

from .register import register_brick_type

from math import *

from jsonmerge import merge

logger = logging.getLogger("gravity_sampler_func")
logger.setLevel(logging.INFO)

class BrickGravitySamplerFunc(BrickStructure):

    _name = "gravity_sampler_func"

    def __init__(self, _element=None):
        """ init room """
        self._element = _element

    def get_instance(self, room:None):
        """Return an instante"""
        return BrickGravitySamplerFunc(room)

    def check_fit(self):
        """ Pass the Room, check it can be applied. """
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

        func_array = setup["func"]
        func = functools.reduce(lambda a, b: a + "\n" + b, func_array )
        print(func)

        compute_array = setup["compute"]
        compute = functools.reduce(lambda a, b: a + "\n" + b, compute_array )
        print(compute)

        parent.set_gravity_script(
            'function optimize_gravity(x1, y1, z1, t, start, finish, count)\n' +
            '   local tab_out = {}\n' +
            '   local dist = 100000000\n' +
            '   local current = -1\n' +
            '   local x0 = 0.0\n' +
            '   local y0 = 0.0\n' +
            '   local z0 = 0.0\n' +
            '   for d = 0, count do\n' +
            '       local u = start + (finish - start) * d / count\n' +
            '       local x = 0.0\n' +
            '       local y = 0.0\n' +
            '       local z = 0.0\n' +
            '       %s\n' % func +
            '       local l_dist = math.sqrt((x - x1)^2 + (y - y1)^2 + (z - z1)^2)\n' +
            '       if (l_dist < dist) then\n' +
            '           current = u\n' +
            '           dist = l_dist\n' +
            '           x0 = x\n' +
            '           y0 = y\n' +
            '           z0 = z\n' +
            '       end\n' +
            '   end\n' +
            '   tab_out["dist"] = dist\n' +
            '   tab_out["u"] = current\n' +
            '   tab_out["x0"] = x0\n' +
            '   tab_out["y0"] = y0\n' +
            '   tab_out["z0"] = z0\n' +
            '   return tab_out\n' +
            'end\n\n' +
            'local t = tab_in["t"]\n' +
            'local x = tab_in["x"]\n' +
            'local y = tab_in["y"]\n' +
            'local z = tab_in["z"]\n' +
            'local segments = %s\n' % setup["segments_nr"] +
            'local optim_grav = optimize_gravity(x, y, z, t, 0.0, 1.0, segments)\n' +
            'local u = optim_grav["u"]\n' +
            'optim_grav = optimize_gravity(x, y, z, t, u - 1/segments, u + 1/segments, 100)\n' +
            'local distance = optim_grav["dist"]\n' +
            'local x0 = optim_grav["x0"]\n' +
            'local y0 = optim_grav["y0"]\n' +
            'local z0 = optim_grav["z0"]\n' +
            'u = optim_grav["u"]\n' +
            '%s\n'  % compute+
            'tab_out["g_x"] = g_x\n' +
            'tab_out["g_y"] = g_y\n' +
            'tab_out["g_z"] = g_z\n' +
            'tab_out["u_x"] = u_x\n' +
            'tab_out["u_y"] = u_y\n' +
            'tab_out["u_z"] = u_z\n'
            'tab_out["w"] = w\n' +
            'tab_out["v"] = %s\n' % setup["frequency"], [ concrete_room.Node.GRAVITY_SIMPLE ])



register_brick_type(BrickGravitySamplerFunc())
