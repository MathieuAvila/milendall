"""
test milendall_math helper
"""

import logging
import unittest

import milendall_math
from cgtypes.vec3 import vec3
from cgtypes.vec4 import vec4

logging.basicConfig()
logger = logging.getLogger("ut_math")
logger.setLevel(logging.INFO)

pO = vec3(0.0, 0.0, 0.0)
pX = vec3(1.0, 0.0, 0.0)
pY = vec3(0.0, 1.0, 0.0)
pZ = vec3(0.0, 0.0, 1.0)
pXY = vec3(1.0, 1.0, 0.0)

class TestMath(unittest.TestCase):

    def test_00_plane_init_Org(self):
        """ test compute plane"""
        p = milendall_math.Plane(pO, pY, pX)
        self.assertEquals(p.equation, vec4(0.0, 0.0, 1.0, 0.0))

    def test_00_plane_init_OrgX(self):
        """ test compute plane"""
        p = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        self.assertEquals(p.equation, vec4(0.0, 0.0, 1.0, -1.0))

    def test_01_classify(self):
        """ test classify"""
        p = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        self.assertEquals(p.classify(pZ), 0.0)
        self.assertEquals(p.classify(pZ + pX), 0.0)
        self.assertEquals(p.classify(2* pZ), 1.0)
        self.assertEquals(p.classify(-2* pZ + pX), -3.0)

    def test_02_plane_cut_point(self):
        """ test cut point"""
        p = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        self.assertEquals(p.cut_point(pO, 2*pZ), vec3(0.0, 0.0, 1.0))

    def test_03_cut_face_by_plane_ALL_fit(self):
        """ test cut all fit"""
        plane = milendall_math.Plane(-pZ, pY - pZ, pX-pZ)
        face = [
          pO, pZ *2, pZ*2 + pX, pX
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], face)
        self.assertEquals(cut[1], None)

    def test_03_cut_face_by_plane_NONE_fit(self):
        """ test cut no fit"""
        plane = milendall_math.Plane(pZ*2 + pZ, pZ*2 + pY + pZ, pZ*2 + pX+pZ)
        face = [
          pO, pZ *2, pZ*2 + pX, pX
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], None)
        self.assertEquals(cut[1], face)

    def test_03_cut_face_by_plane_HALF_fit_rot0(self):
        """ test cut half fit 0"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pO, pZ *2, pZ*2 + pX, pX
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(1.0, 0.0, 2.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0)])


    def test_03_cut_face_by_plane_HALF_fit_rot1(self):
        """ test cut half fit 1"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pZ *2, pZ*2 + pX, pX, pO
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(1.0, 0.0, 2.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0)])

    def test_03_cut_face_by_plane_HALF_fit_rot2(self):
        """ test cut half fit 2"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pZ*2 + pX, pX, pO, pZ *2,
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(1.0, 0.0, 2.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0)])

    def test_03_cut_face_by_plane_HALF_fit_rot3(self):
        """ test cut half fit 3"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pX, pO, pZ *2, pZ*2 + pX
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(1.0, 0.0, 2.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0)])

    def test_03_cut_face_by_plane_purge(self):
        """ test cut must purge point"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pX, pO, pZ, pZ *2, pZ*2 + pX
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(1.0, 0.0, 2.0), vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0)])

    def test_03_cut_face_by_plane_remove_1_point(self):
        """ test cut remove 1 face because there is only 1 point left"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pX, pO, pZ
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], None)
        self.assertEquals(cut[1], [vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)])

    def test_03_cut_face_by_plane_remove_2_point(self):
        """ test cut remove 1 face because there are only 2 points left"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pO, pX, pZ + pX, pZ
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], None)
        self.assertEquals(cut[1], [pO, pX, pZ + pX, pZ])

    def test_03_cut_face_by_plane_3_points(self):
        """test cut 1 point only left, make 1 face with 3 points"""
        plane = milendall_math.Plane(pZ, pY + pZ, pX+pZ)
        face = [
          pO, pX, pZ * 2
         ]
        cut = milendall_math.cut_face_by_plane(face, plane)
        self.assertEquals(cut[0], [vec3(0.0, 0.0, 2.0), vec3(0.0, 0.0, 1.0), vec3(0.5, 0.0, 1.0)])
        self.assertEquals(cut[1], [vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.5, 0.0, 1.0), vec3(0.0, 0.0, 1.0)])


    def test_04_puch_hole_keep_all(self):
        """ test punch a hole into a single face, outside (keep them all)"""
        org_faces = [[pO, pX*3, pX*3 + pZ*3, pZ*3], [ [0,1,2,3] ]]
        faces = milendall_math.Faces( org_faces[0], org_faces[1])
        face = [
                pX*1000 + pZ*1000,
                pX*1000 + pZ*1001,
                pX*1001 + pZ*1001,
                pX*1001 + pZ*1000
                ]
        faces.hole(face)
        result = faces.get_points_faces()
        logger.info(result)
        self.assertEquals(result ,org_faces)


    def test_04_puch_hole_center(self):
        """ test punch a hole into a single face, inside, make a hole"""
        org_faces = [[pO, pX*3, pX*3 + pZ*3, pZ*3], [ [0,1,2,3] ]]
        faces = milendall_math.Faces( org_faces[0], org_faces[1])
        face = [
                pX*1 + pZ*1,
                pX*1 + pZ*2,
                pX*2 + pZ*2,
                pX*2 + pZ*1
                ]
        faces.hole(face)
        result = faces.get_points_faces()
        logger.info(result)
        self.assertEquals(result ,
                          [
                              [vec3(0.0, 0.0, 3.0),
                               vec3(0.0, 0.0, 0.0),
                               vec3(1.0, 0.0, 0.0),
                               vec3(1.0, 0.0, 3.0),

                               vec3(3.0, 0.0, 3.0),
                               vec3(1.0, 0.0, 3.0),
                               vec3(1.0, 0.0, 2.0),
                               vec3(3.0, 0.0, 2.0),

                               vec3(3.0, 0.0, 0.0),
                               vec3(3.0, 0.0, 2.0),
                               vec3(2.0, 0.0, 2.0),
                               vec3(2.0, 0.0, 0.0),

                               vec3(1.0, 0.0, 0.0),
                               vec3(2.0, 0.0, 0.0),
                               vec3(2.0, 0.0, 1.0),
                               vec3(1.0, 0.0, 1.0)],
                              [[0, 1, 2, 3],
                               [4, 5, 6, 7],
                               [8, 9, 10, 11],
                               [12, 13, 14, 15]]]
)


if __name__ == '__main__':
    unittest.main()
