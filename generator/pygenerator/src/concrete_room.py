"""
A Concrete Room is a full description of a single room. Its goal is to be converted
to a gltf scene file once everything is defined. So it's an intermediate representation
of a gltf, easier to manipulate during construction by the room itself and the
associated gates.

Note that it is not reconstructed from a file, because all parameters from gates/rooms/objects
should be sufficient to reconstruct a full one.
However, it can be dumped to a file to ease debugging of rooms/gates/objects
"""

import json

import cgtypes.vec3
import cgtypes.mat4

def get_texture_definition(filename, scale = 1.0, offset = cgtypes.vec3()):
    """ return an easy to use texture definition based on 4 points (vec3)
    and their textures offset (vec3) with 3rd component being 0."""
    my_def = { "texture": filename }
    transform = cgtypes.mat4(
        scale, scale, scale, offset[0],
        scale, scale, scale, offset[1],
        scale, scale, scale, offset[2],
        scale, scale, scale, 0)
    my_def.proj = transform
    return my_def

class Node:

    def __init__(self, _name, _parent = None, _matrix = None):
        self.structure_points = []
        self.structure_faces = []
        self.dressing_points = []
        self.dressing_faces = []
        if _matrix is not None:
            self.matrix = _matrix
        self.name = _name
        if _parent is not None:
            self.parent = _parent

    def add_structure_points(self, points):
        """
        Add structure points to object.
        Return index for insertion for faces creation
        """
        index = len(self.structure_points)
        self.structure_points.extend(points)
        return index

    def add_dressing_points(self, points):
        """
        Add structure points to object.
        Return index for insertion for faces creation
        """
        index = len(self.dressing_points)
        self.dressing_points.extend(points)
        return index

    CATEGORY_VISUAL = "vis"
    CATEGORY_PHYSICS = "phy"

    HINT_GROUND  = "ground"
    HINT_WALL    = "wall"
    HINT_CEILING = "ceiling"
    HINT_WATER   = "water"
    HINT_OTHER   = "other"

    HINT_DOOR       = "door"
    HINT_BUILDING   = "building"
    HINT_WINDOW     = "window"
    HINT_HOLE       = "hole"
    HINT_PORTAL     = "portal"

    def add_structure_faces(self, point_offset, faces, categories, hints, physics = None):
        """
        Add faces to object, with a given category. Used by structures, not dressings
        faces are a table of table of points. There can be any number as long as they
        are of convex shape and in the same plan.
        Use point_offset to offset all points indexes.
        categories is a table of attributes that determine the usage.
            Those can be: CATEGORY_VISUAL or CATEGORY_PHYSICS.
            Visual means it shall be used to build dressing, and transferred to the gltf
            for visualization
            Physics means it is transferred to the application private data part.
            Both attribures use are possible. None means that its definition is useless
            and will be trashed.
        hints: table of of attributes to help build constructs, can be used for visual
        physics is metadata for physics (portal, hard wall, etc...)
        """
        nfaces = []
        for face in faces:
            nface = [ p+point_offset for p in face]
            nfaces.append(nface)
        self.structure_faces.append({
            "faces":nfaces,
            "category":categories,
            "hint": hints,
            "physics": physics
            })

    def add_dressing_faces(self, point_offset, faces, texture):
        """
        Add dressing faces to object, with a given category. Used by dressings, not structures
        faces are a table of table of points. There can be any number as long as they
        are of convex shape and in the same plan.
        Use point_offset to offset all points indexes.
        texture is metadata for textures and is mandatory. There is  only one per call. Do
        multiple calls to have different textures.
        """
        nfaces = []
        for face in faces:
            nface = [ p+point_offset for p in face]
            nfaces.append(nface)
        self.structure_faces.append({
            "faces":nfaces,
            "texture": texture
            })

class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        return o.__dict__

class ConcreteRoom:

    def __init__(self):
        self.objects = []
        self.gravity = None

    def add_child(self, parent, name, matrix = None):
        """
        create a new node and return a reference to it for future use.
        Its name can be used to reference a child.
        Set "parent" to None to add it as a root node.
        Use the reference to add faces & points.
        "matrix" represents a quaternion matrix used to position the mesh
        """
        obj = Node(name, parent, matrix)
        self.objects.append(obj)
        return obj

    def setup_gravity_info(self, gravity):
        """
        Setup the gravity info of the room. This is a private section.
        """
        self.gravity = gravity

    def dump_to_json(self):
        """
        dump content to JSON for debug and unit tests
        this is not the expected final output. Use generate_gltf for this.
        """
        return json.dumps(self, cls=JSONEncoder, indent=1)

    def generate_gltf(self, filename):
        """
        dump the scene to a file
        """
        gltf = {
            "asset": {
                "generator": "MILENDALL-1.0",
                "version": "2.0"
            },
            "scene": 0,
            "scenes": [
                {
                    "nodes": [
                        0
                    ]
                }
            ],
            "nodes": [
            ],
            "meshes": [
            ]
        }
        j = json.dumps(gltf, indent=1)
        with open(filename, "w") as write_file:
            write_file.write(j)
