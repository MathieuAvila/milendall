"""
math helper
"""

from __future__ import annotations

import logging

import cgtypes.vec3
import cgtypes.vec4
import cgtypes.mat4

import copy

logging.basicConfig()
logger = logging.getLogger("math")
logger.setLevel(logging.INFO)

class Plane():

    normal: cgtypes.vec3.vec3
    equation: cgtypes.vec4.vec4

    def __init__(self, p0: cgtypes.vec3.vec3, p1: cgtypes.vec3.vec3,
                 p2: cgtypes.vec3.vec3) -> None:
        '''Init from 3 points'''
        u = p1 - p0
        U = cgtypes.vec3(u.x, u.y, u.z)
        U = U.normalize()
        # get points 0,N-1 to compute V
        new_vec_org = p2 - p0
        new_vec = cgtypes.vec3(new_vec_org.x, new_vec_org.y, new_vec_org.z)
        self.normal = new_vec.cross(U).normalize()
        self.equation = cgtypes.vec4(self.normal.x, self.normal.y, self.normal.z, - p0*self.normal)

    def classify(self, p: cgtypes.vec3.vec3) -> float:
        """Return the computed value of the the point P"""
        pX = cgtypes.vec4(p.x, p.y, p.z, 1.0)
        return pX * self.equation

    def cut_point(self, p0: cgtypes.vec3.vec3, p1: cgtypes.vec3.vec3) -> cgtypes.vec3.vec3:
        """Find the point where the face cuts the line p0-p1 in 2"""
        c0 = self.classify(p0)
        c1 = self.classify(p1)
        if c1 - c0 == 0:
            logger.error("Enable to find cutting line")
            return p0
        alpha = - c0 / (c1 - c0)
        p = p0 + alpha * (p1 - p0)
        return p

def purge_face(face: list[cgtypes.vec3.vec3]) -> list[cgtypes.vec3.vec3] | None:
    """For a given face, purge all redundant points.
    If less than 3 point are left return None"""
    index = 0
    result = []
    while index != len(face):
        next = (index + 1) % len(face)
        if face[index] != face[next]:
            result.append(face[index])
        index = index + 1
    if len(result) >= 3:
        return result
    return None

def cut_face_by_plane(face: list[cgtypes.vec3.vec3],
                      plane: Plane) -> list[list[cgtypes.vec3.vec3] | None]:
    """Cut a plane by another plane.
    Return 2 lists of faces: one which are on the right side,
    the others which are the other side.
    """
    values = [ plane.classify(p) for p in face]
    sign = [ v > 0.0 for v in values]
    positive = len([ v for v in sign if v])
    logger.debug("")
    [ logger.debug(p) for p in face]
    logger.debug(values)
    logger.debug(positive)

    if positive == 0:
        return [ None, copy.deepcopy(face)]
    if positive == len(face):
        return [ copy.deepcopy(face), None ]
    index_first_neg = 0
    # find first negative
    while sign[ index_first_neg ]:
        index_first_neg = index_first_neg + 1
    # find first positive after first negative:
    index_first_pos = index_first_neg + 1
    while not sign[ index_first_pos % len(face) ]:
        index_first_pos = index_first_pos + 1
    logger.debug("index_first_neg %s index_first_pos %s " % (index_first_neg,index_first_pos ))
    # now find the cutting point
    pPOS = face[ index_first_pos % len(face) ]
    pNEG = face[ (index_first_pos-1) % len(face) ]
    pCUT0 = plane.cut_point(pPOS, pNEG)
    logger.debug("cut0 %s " % (pCUT0))

    # advance to next negative
    index_next_neg = index_first_pos + 1
    while sign[ index_next_neg % len(face) ]:
        index_next_neg = index_next_neg + 1
    pNEG2 = face[ index_next_neg % len(face) ]
    pPOS2 = face[ (index_next_neg-1) % len(face) ]
    pCUT1 = plane.cut_point(pNEG2, pPOS2)
    logger.debug("index_next_neg %s " % index_next_neg)
    logger.debug("pCUT1 %s " % (pCUT1))

    my_index = index_next_neg
    face_neg = [ ]
    while not sign[ my_index % len(face) ]:
        logger.debug("add %s" % my_index)
        face_neg.append(copy.deepcopy(face[my_index % len(face)]))
        my_index = my_index + 1
    face_neg.append(copy.deepcopy(pCUT0))
    face_neg.append(copy.deepcopy(pCUT1))

    face_pos = [ ]
    while sign[ my_index % len(face) ]:
        logger.debug("add %s" % my_index)
        face_pos.append(copy.deepcopy(face[my_index % len(face)]))
        my_index = my_index + 1
    face_pos.append(copy.deepcopy(pCUT1))
    face_pos.append(copy.deepcopy(pCUT0))

    logger.debug("face_pos %s" % face_pos)
    logger.debug("face_neg %s" % face_neg)

    return [ purge_face(face_pos), purge_face(face_neg) ]


def cut_faces_by_plane(faces: list[list[cgtypes.vec3.vec3]],
                       plane: Plane) -> list[list[list[cgtypes.vec3.vec3]]]:
    """Cut the list of faces by a plane.
    Return 2 lists of faces: one which are on the right side,
    the others which are the other side.
    """
    result = [ [], []]
    for face in faces:
        f_in, f_out = cut_face_by_plane(face, plane)
        if f_in is not None:
            result[0].append(f_in)
        if f_out is not None:
            result[1].append(f_out)
    return result

def get_point_in_list(points: list[cgtypes.vec3.vec3], p: cgtypes.vec3.vec3) -> int:
    # get current point in points if any
    index = 0
    found = -1
    while index < len(points):
        if p == points[index]:
            found = -1
            break
        index = index + 1
    if found != -1:
        return found
    points.append(p)
    return len(points)-1

class Faces():

    faces: list[list[cgtypes.vec3.vec3]]

    def __init__(self, points: list[cgtypes.vec3.vec3],
                 faces: list[list[int]]) -> None:
        """Initialize a list of faces, by dereferencing index to real points"""
        self.faces = []
        for face in faces:
            f = [ points[i] for i in face ]
            self.faces.append(f)

    def get_points_faces(self) -> list[list[cgtypes.vec3.vec3] | list[list[int]]]:
        """return a compacted set of points and faces"""
        points: list[cgtypes.vec3.vec3] = []
        faces: list[list[int]] = []
        for f in self.faces:
            new_f = []
            for p in f:
                new_f.append(get_point_in_list(points, p))
            faces.append(new_f)
        return [points, faces]

    def hole(self, face: list[cgtypes.vec3.vec3]) -> None:
        """Punch a hole inside the Faces, by the given face.
        It is expected that the hole hits the faces on their projection to each faces
        All faces are affected by this hole.
        Points are a list of points that make up a convex polygon.
        returns a compacted set of points and faces
        """
        plane = Plane(face[0], face[1], face[2])
        normal = plane.normal
        index = 0
        keep_list = []
        check_list = self.faces
        while index < len(face):
            new_plane = Plane(face[index], face[(index+1) % len(face)], face[index] + normal)
            new_list = cut_faces_by_plane(check_list, new_plane)
            logger.debug("new_list %s" % new_list)
            keep_list.extend(new_list[0])
            check_list = new_list[1]
            index = index + 1
        logger.debug("keep_list %s" % keep_list)
        self.faces = keep_list