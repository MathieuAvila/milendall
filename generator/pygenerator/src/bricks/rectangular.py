"""
structure definition for a simple rectangular brick
"""

from __future__ import annotations

import logging
import math

from brick_structure import BrickStructure
import concrete_room
import cgtypes.vec3
import cgtypes.mat4

import milendall_math

from .register import register_brick_type

from merge_utils import merge
from typing_defs import ElementWithValues, SelectorLike

logger = logging.getLogger("rectangular")
logger.setLevel(logging.INFO)

class BrickRectangular(BrickStructure):

    _name = "rectangular"

    _element: ElementWithValues | None

    def __init__(self, _element: ElementWithValues | None = None) -> None:
        """ init brick """
        self._element = _element

    def get_instance(self, brick: ElementWithValues) -> BrickRectangular:
        """Return an instante"""
        return BrickRectangular(brick)

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        logger.debug("checking if rectangular is ok: always ! rectangular rules the world !")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """ force set values:
        - set values to brick size"""
        if self._element is None:
            raise RuntimeError("BrickRectangular requires an element to instantiate")
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default= {
            "size" : [ 5.0 , 2.5, 5.0 ],
            "reverse" : False
        }

        self._element.values.parameters.structure_private = merge( my_default, structure_parameters)
        if self._element is None:
            raise RuntimeError("BrickRectangular requires an element to generate")
        structure_private = self._element.values.parameters.structure_private

        logger.debug("private: %s", str(structure_private))

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room"""
        structure_private = self._element.values.parameters.structure_private
        direction_size = structure_private["size"]

        # create main object
        parent = concrete.add_child(None, "parent")

        wall_matrices = [
            cgtypes.mat4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                -1.0, 0.0, 0.0, direction_size[0],
                0.0, 1.0, 0.0,  0.0,
                0.0, 0.0, -1.0, direction_size[2],
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, 1.0, 0,
                0.0, 1.0, 0.0, 0.0,
                -1.0, 0.0, 0.0, direction_size[2],
                0.0, 0.0, 0.0, 1.0),
            cgtypes.mat4(
                0.0, 0.0, -1.0, direction_size[0],
                0.0, 1.0, 0.0, 0.0,
                1.0, 0.0, 0.0, 0,
                0.0, 0.0, 0.0, 1.0),

            (cgtypes.mat4.translation(cgtypes.vec3(0.0, 0.0, direction_size[2])) *
            cgtypes.mat4.rotation(-math.pi / 2.0, cgtypes.vec3(1.0, 0.0, 0.0))),

            (cgtypes.mat4.translation(cgtypes.vec3(0.0, direction_size[1], 0.0)) *
            cgtypes.mat4.rotation(math.pi / 2.0, cgtypes.vec3(1.0, 0.0, 0.0)))
        ]
        wall_kind = [
            concrete_room.Node.HINT_WALL,
            concrete_room.Node.HINT_WALL,
            concrete_room.Node.HINT_WALL,
            concrete_room.Node.HINT_WALL,
            concrete_room.Node.HINT_GROUND,
            concrete_room.Node.HINT_CEILING,
            ]
        pads = self._element.values.pads

        sizes = [
                 [0,1],
                 [0,1],
                 [2,1],
                 [2,1],
                 [0,2],
                 [0,2],
                ]

        pO = cgtypes.vec3(0.0, 0.0, 0.0)
        pX = cgtypes.vec3(1.0, 0.0, 0.0)
        pY = cgtypes.vec3(0.0, 1.0, 0.0)

        for wall_dir in range(0, 6):
            wall_mat = wall_matrices[wall_dir]
            size = sizes[wall_dir]
            org_points = [
                pO,
                pX * direction_size[size[0]],
                pX * direction_size[size[0]] + pY * direction_size[size[1]],
                pY * direction_size[size[1]]
                ]
            if structure_private["reverse"]:
                org_points = org_points[::-1]
            org_faces = [[0,1,2,3]]
            faces = milendall_math.Faces(org_points, org_faces)
            if pads is not None:
                for pad in pads:
                    d = pad["definition"]
                    o = d["origin"]
                    S = d["size"]
                    s = cgtypes.vec3(S[0], S[1], 0.0)
                    if d["face"] == wall_dir:
                        # punch hole
                        face = [cgtypes.vec3(),
                                cgtypes.vec3(s.x, 0.0, 0.0),
                                cgtypes.vec3(s.x, s.y, 0.0),
                                cgtypes.vec3(0.0, s.y, 0.0)]
                        if structure_private["reverse"]:
                            face = face[::-1]
                        # apply transformation to hole, based on border number
                        local_pad_mat = cgtypes.mat4(1.0)
                        if structure_private["reverse"]:
                            if d["border"] == 0 or d["border"] == 1:
                                local_pad_mat = local_pad_mat * (
                                    cgtypes.mat4.translation(cgtypes.vec3(S[0], 0.0, 0.0)) *
                                    cgtypes.mat4.rotation(math.pi, cgtypes.vec3(0.0, 1.0, 0.0)))
                            else:
                                local_pad_mat = local_pad_mat * (
                                    cgtypes.mat4.translation(cgtypes.vec3(0.0, S[1], 0.0)) *
                                    cgtypes.mat4.rotation(math.pi, cgtypes.vec3(1.0, 0.0, 0.0)))

                        if d["border"] == 0:
                            local_pad_mat = local_pad_mat * cgtypes.mat4(1.0) # aka: do nothing
                        elif d["border"] == 1:
                            local_pad_mat = (
                                cgtypes.mat4.translation(cgtypes.vec3(0.0, direction_size[1], 0.0)) *
                                cgtypes.mat4.rotation(-math.pi / 2.0, cgtypes.vec3(0.0, 0.0, 1.0))
                                ) * local_pad_mat
                        elif d["border"] == 2:
                            local_pad_mat = (
                                cgtypes.mat4.translation(cgtypes.vec3(direction_size[0], direction_size[1], 0.0)) *
                                cgtypes.mat4.rotation(-math.pi, cgtypes.vec3(0.0, 0.0, 1.0))
                                ) * local_pad_mat
                        elif d["border"] == 3:
                            local_pad_mat = (
                                cgtypes.mat4.translation(cgtypes.vec3(direction_size[0], 0.0, 0.0)) *
                                cgtypes.mat4.rotation(-math.pi * 3.0 / 2.0, cgtypes.vec3(0.0, 0.0, 1.0))
                                ) * local_pad_mat
                        else:
                            raise Exception("Border must be between 0 and 3")
                        local_pad_mat = cgtypes.mat4.translation(cgtypes.vec3(o[0], o[1], 0.0))* local_pad_mat

                        trans_face = [ local_pad_mat * p for p in face]
                        logger.debug("trans_face %i %s " %( d["border"], trans_face))
                        faces.hole(trans_face)
                        # add pad
                        concrete.add_child("parent", pad.pad_id, wall_mat* local_pad_mat)

            holed = faces.get_points_faces()

            #transformed_points = [ wall_mat * (cgtypes.vec4(p.x, p.y, p.z, 1.0))  for p in holed[0]]
            #transformed_points_vec3 = [ cgtypes.vec3(p.x, p.y, p.z) for p in transformed_points]
            transformed_points_vec3 = []
            for p in holed[0]:
                cp = wall_mat * (cgtypes.vec4(p.x, p.y, p.z, 1.0))
                cp2 = cgtypes.vec3(cp.x, cp.y, cp.z)
                cp2.u = p.x
                cp2.v = p.y
                cp2.has_tex = True
                transformed_points_vec3.append(cp2)

            logger.debug(transformed_points_vec3)
            index_face = parent.add_structure_points(transformed_points_vec3)
            parent.add_structure_faces(
                index_face,
                holed[1],
                concrete_room.Node.CAT_PHYS_VIS,
                [wall_kind[wall_dir], concrete_room.Node.HINT_BUILDING],
                {concrete_room.Node.PHYS_TYPE : concrete_room.Node.PHYS_TYPE_HARD} )



register_brick_type(BrickRectangular())
