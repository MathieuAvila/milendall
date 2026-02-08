"""
structure definition for a simple bridge (pont)

Parametric parts:
- deck (ground) with length (x), thickness (y), width (z)
- two side rails (walls) with configurable height

Follows the same generation approach as rectangular.py
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

logger = logging.getLogger("pont")
logger.setLevel(logging.INFO)


class BrickPont(BrickStructure):

    _name = "pont"

    _element: ElementWithValues | None

    def __init__(self, _element: ElementWithValues | None = None) -> None:
        """ init brick """
        self._element = _element

    def get_instance(self, brick: ElementWithValues) -> BrickPont:
        """Return an instance"""
        return BrickPont(brick)

    def check_structure(self) -> bool:
        """check everything is as expected.
        """
        logger.debug("checking if pont is ok")
        return True

    def instantiate(self, selector: SelectorLike) -> None:
        """Set default values and merge with provided parameters.
        - size: [length, thickness, width]
        - rail_height: height of the side rails
        - reverse: reverse winding order if True
        """
        if self._element is None:
            raise RuntimeError("BrickPont requires an element to instantiate")
        structure_parameters = self._element.values.parameters.structure_parameters

        my_default = {
            "size": [6.0, 0.5, 3.0],
            "rail_height": 1.2,
            "reverse": False,
        }

        self._element.values.parameters.structure_private = merge(my_default, structure_parameters)
        if self._element is None:
            raise RuntimeError("BrickPont requires an element to generate")
        structure_private = self._element.values.parameters.structure_private

        logger.debug("private: %s", str(structure_private))

    def generate(self, concrete: concrete_room.ConcreteRoom) -> None:
        """Perform instantiation on concrete_room: deck + two rails
        Deck is built with two faces (top and underside). Rails are two vertical faces.
        """
        structure_private = self._element.values.parameters.structure_private
        direction_size = structure_private["size"]  # [length (x), thickness (y), width (z)]
        length = float(direction_size[0])
        thickness = float(direction_size[1])
        width = float(direction_size[2])
        rail_height = float(structure_private.get("rail_height", 1.2))

        # create main object
        parent = concrete.add_child(None, "parent")

        # Matrices to orient base XY face into required planes
        # Deck top: rotate XY to XZ (Y -> Z)
        deck_top_mat = cgtypes.mat4.rotation(-math.pi / 2.0, cgtypes.vec3(1.0, 0.0, 0.0))
        # Deck underside: translate up by thickness and rotate opposite
        deck_bottom_mat = (
            cgtypes.mat4.translation(cgtypes.vec3(0.0, thickness, 0.0))
            * cgtypes.mat4.rotation(math.pi / 2.0, cgtypes.vec3(1.0, 0.0, 0.0))
        )
        # Left rail: vertical wall at z=0 (identity orientation)
        left_rail_mat = cgtypes.mat4(1.0)
        # Right rail: vertical wall at z=width
        right_rail_mat = cgtypes.mat4.translation(cgtypes.vec3(0.0, 0.0, width)) * cgtypes.mat4(1.0)

        wall_matrices = [
            deck_top_mat,     # ground
            deck_bottom_mat,  # underside (ceiling)
            left_rail_mat,    # left wall
            right_rail_mat,   # right wall
        ]
        wall_kind = [
            concrete_room.Node.HINT_GROUND,
            concrete_room.Node.HINT_CEILING,
            concrete_room.Node.HINT_WALL,
            concrete_room.Node.HINT_WALL,
        ]

        # Sizes per face (in local XY before transform)
        sizes = [
            [length, width],       # deck top
            [length, width],       # deck bottom
            [length, rail_height], # left rail
            [length, rail_height], # right rail
        ]

        pO = cgtypes.vec3(0.0, 0.0, 0.0)
        pX = cgtypes.vec3(1.0, 0.0, 0.0)
        pY = cgtypes.vec3(0.0, 1.0, 0.0)

        for wall_dir in range(0, len(wall_matrices)):
            wall_mat = wall_matrices[wall_dir]
            size = sizes[wall_dir]
            org_points = [
                pO,
                pX * size[0],
                pX * size[0] + pY * size[1],
                pY * size[1],
            ]
            if structure_private["reverse"] is True:
                org_points = org_points[::-1]
            org_faces = [[0, 1, 2, 3]]
            faces = milendall_math.Faces(org_points, org_faces)

            holed = faces.get_points_faces()

            transformed_points_vec3 = []
            for p in holed[0]:
                cp = wall_mat * (cgtypes.vec4(p.x, p.y, p.z, 1.0))
                cp2 = cgtypes.vec3(cp.x, cp.y, cp.z)
                # keep UV from original XY
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
                {concrete_room.Node.PHYS_TYPE: concrete_room.Node.PHYS_TYPE_HARD},
            )


register_brick_type(BrickPont())
