"""
A Concrete Room is a full description of a single room. Its goal is to be converted
to a gltf scene file once everything is defined. So it's an intermediate representation
of a gltf, easier to manipulate during construction by the room itself and the
associated gates.

Note that it is not reconstructed from a file, because all parameters from gates/rooms/objects
should be sufficient to reconstruct a full one.
However, it can be dumped to a file to ease debugging of rooms/gates/objects
"""

from asyncio.log import logger
import logging
import json
import itertools
import struct
import shutil
import os
import copy

import cgtypes.vec3
import cgtypes.mat4

from gltf_helper import get_texture_definition, create_accessor

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

class Node:

    def __init__(self, _name, _parent = None, _matrix = None):
        self.structure_points = []
        self.structure_faces = []
        self.dressing = {}
        self.matrix = _matrix
        self.name = _name
        self.parent = _parent
        self.gravity = None
        self.gravity_script = None
        self.gravity_mode = None
        self.triggers = []

    def set_gravity(self, gravity):
        self.gravity = gravity

    def set_gravity_script(self, gravity, mode):
        self.gravity_script = gravity
        self.gravity = {}
        self.gravity_mode = mode

    def add_structure_points(self, points):
        """
        Add structure points to object.
        Return index for insertion for faces creation
        """
        index = len(self.structure_points)
        self.structure_points.extend(points)
        return index

    CATEGORY_VISUAL = "vis"
    CATEGORY_PHYSICS = "phy"

    CAT_PHYS = [CATEGORY_PHYSICS]
    CAT_VIS = [CATEGORY_VISUAL]
    CAT_PHYS_VIS = [CATEGORY_VISUAL, CATEGORY_PHYSICS]

    # for visuals, what kinds of surfaces it represents.
    HINT_GROUND  = "ground"
    HINT_WALL    = "wall"
    HINT_CEILING = "ceiling"
    HINT_WATER   = "water"
    HINT_OTHER   = "other"

    # for visuals, in what kind of structures it is linked.
    HINT_DOOR       = "door"
    HINT_BUILDING   = "building"
    HINT_WINDOW     = "window"
    HINT_HOLE       = "hole"
    HINT_PORTAL     = "portal"

    # for physical, metadata types
    PHYS_TYPE        = "type"     # key for types
    PHYS_TYPE_PORTAL = "portal"   # for portals, only to be used by gates, with 1 face
    PHYS_TYPE_HARD   = "hard"     # can be used for walls or ground
    PORTAL_CONNECT   = "connect"
    GATE_ID          = "gate"

    # for gravity, method to expose
    GRAVITY_SIMPLE   = "simple"

    # for triggers, definitions for API, not used in JSON
    TRIGGER_ENTER_BOX = True
    TRIGGER_LEAVE_BOX = False
    TRIGGER_SET_TRUE  = True
    TRIGGER_SET_FALSE = False

    # for triggers, values used in JSON
    TRIGGER_SET        = "set_trigger"         # will contain the name of the trigger
    TRIGGER_BOX        = "box"                 # definition of box as a 2-dim table
    TRIGGER_BOX_MODE   = "is_box_mode_enter"   # set to true if trigger is changed when entering, false if leaving
    TRIGGER_BOX_TARGET = "target_value"        # set to value in which trigger must be set

    def add_trigger_box(self, enter_or_leave, set_true_or_false, box_min, box_max, event_name):
        self.triggers.append(
            {
                self.TRIGGER_BOX : [ [box_min.x,box_min.y,box_min.z], [box_max.x,box_max.y,box_max.z] ],
                self.TRIGGER_SET : event_name,
                self.TRIGGER_BOX_MODE : enter_or_leave,
                self.TRIGGER_BOX_TARGET : set_true_or_false
            })

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
            "hints": hints,
            "physics": physics
            })

    def get_visual_face(self, hints):
        """get visual faces, matching those hints"""
        result = []
        for faces in self.structure_faces:
            if self.CATEGORY_VISUAL in faces["category"]:
                if set(hints).issubset(set(faces["hints"])):
                    result.append(faces)
        return result

    def get_physical_faces(self):
        """get physical faces"""
        result = []
        for faces in self.structure_faces:
            if self.CATEGORY_PHYSICS in faces["category"]:
                result.append(faces)
        return result

    def add_dressing_faces(self, points, faces, texture):
        """
        Add dressing faces to object. Used by dressings, not structures
        faces are a table of table of points. There can be any number as long as they
        are of convex shape and in the same plan.
        texture is metadata for textures and is mandatory. There is only one per call. Do
        multiple calls to have different textures.
        """
        if "function" in texture:

            if texture["texture"] not in self.dressing:
                self.dressing[texture["texture"]] = { "points": [], "faces":[] } # points are x,y,z,u,v
            texture_block = self.dressing[texture["texture"]]
            points_block = texture_block["points"]
            faces_block = texture_block["faces"]

            for face in faces:
                new_face = []
                my_points = []
                # build points
                for p in face:
                    new_point = copy.deepcopy(points[p])

                    print("p = %s, type %s" % (points[p], type(points[p])))
                    print("p copy = %s" % (new_point))

                    points_block.append(new_point)
                    my_points.append(new_point)
                    print("%i %i" % (len(my_points), len(points_block)))
                    new_face.append(len(points_block)-1)
                # request function to feed UV
                texture["function"](points, face, texture["context"], my_points)
                faces_block.append(new_face)
            return

        if texture["texture"] not in self.dressing:
            self.dressing[texture["texture"]] = { "points": [], "faces":[] } # points are x,y,z,u,v
        texture_block = self.dressing[texture["texture"]]
        points_block = texture_block["points"]
        faces_block = texture_block["faces"]

        # compute used points and projections at the same time
        used_points = sorted(set([ n for f in faces for n in f]))
        used_points_translation = {}
        new_points = []
        for index in used_points:
            old_point = points[index]
            if "proj" in texture:
                projected_uv = texture["proj"] * cgtypes.vec4(old_point.x, old_point.y, old_point.z, 1.0)
            elif "map_method" in texture:
                projected_uv = texture["map_method"](old_point)
                #print(projected_uv)
            else:
                raise "No method to map texture"
            new_point = copy.deepcopy(old_point)
            new_point.u = projected_uv.x
            new_point.v = projected_uv.y
            used_points_translation[index] = len(new_points)
            new_points.append(new_point)

        new_faces = [ [ used_points_translation[n] for n in f ] for f in faces ]

        faces = new_faces
        points = new_points

        # map points
        new_point = [] # for each point, contain the position in the final list
        for point in points:
            if point not in points_block:
               points_block.append(point)
            new_index = points_block.index(point)
            new_point.append(new_index)
        # insert new faces
        for face in faces:
            new_face = [ new_point[p] for p in face ]
            faces_block.append(new_face)

    def get_extras(self, gltf_node):
        if not "extras" in gltf_node:
            gltf_node["extras"] = {}
        extra = gltf_node["extras"]
        return extra

    def gltf_generate_gravity(self, gltf_node, gravity_list):
        if self.gravity is not None:
            extra = self.get_extras(gltf_node)
            extra["gravity"] = self.gravity
            if self.gravity_script:
                gravity_list.append({"name": self.name, "script": self.gravity_script, "mode": self.gravity_mode})

    def gltf_generate_triggers(self, gltf_node):
        if len(self.triggers) != 0:
            extra = self.get_extras(gltf_node)
            extra["triggers"] = copy.deepcopy(self.triggers)

    def gltf_generate_physical_faces(self, gltf, gltf_node, data_file):
        list_physical_faces = self.get_physical_faces()
        if list_physical_faces != []:
            # generate only if there are real faces
            extra = self.get_extras(gltf_node)
            extra["phys_faces"] = []
            extra_phys_faces = extra["phys_faces"]
            # add points accessor
            points = [ [n.x, n.y, n.z] for n in self.structure_points]
            extra["points"] = create_accessor(
                        data_file,
                        gltf,
                        points)

            for faces in list_physical_faces:
                # add faces accessor.
                # this is a stupid "point_count, point0, point1... pointX, point_count..." list
                poly_list = []
                for poly in faces["faces"]:
                    poly_list.append(len(poly))
                    for index in poly:
                        poly_list.append(index)
                face_accessor = create_accessor(
                        data_file,
                        gltf,
                        poly_list)
                extra_phys_faces.append({ "data": faces["physics"] , "accessor": face_accessor})


class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        try:
            res = o.__dict__
            return res
        except AttributeError:
            pass
        return None

def get_texture_id(gltf, texture_list, texture):
    """Get a texture id if it was already created, of insert it otherwise """
    gltf_textures = gltf["textures"]
    gltf_images = gltf["images"]
    gltf_materials = gltf["materials"]
    if texture not in texture_list:  # need to insert
        gltf_images.append({ "uri": texture })
        gltf_textures.append( { "sampler": 0, "source": len(gltf_images)-1 } )
        gltf_materials.append(
            {
                "pbrMetallicRoughness": {
                    "baseColorTexture": {
                        "index": len(gltf_textures) -1
                    },
                    "metallicFactor": 0.0
                },
            })
        texture_list[texture] = len(gltf_materials)-1
    return texture_list[texture]

class ConcreteRoom:

    def __init__(self):
        self.objects = []
        self.gravity = None
        self.animations = []

    def get_objects(self):
        """return direct references to the list of objects included in the room
           Is this a good idea to provide a direct access? Not sure..."""
        return self.objects

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

    def get_node(self, name):
        """ accessor to a given node"""
        for obj in self.objects:
            if obj.name == name:
                return obj
        return None

    def add_animation(self, animation):
        self.animations.append(animation)

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

    def merge(self, other_room):
        self.objects.extend(other_room.objects)
        self.animations.extend(other_room.animations)

    def append_prefix(self, prefix):
        """For every object, set a prefix to its name"""
        for o in self.objects:
            o.name = "%s%s" % (prefix, o.name)

    def set_root(self, root_id):
        """For every object that has a void parent, set its parent to root_id"""
        for o in self.objects:
            logger.info("o parent: %s" , o.parent)
            if o.parent is None:
                o.parent = root_id

    def generate_gravity(self, gravity_list, directory):
        """
        dump the gravity information to a file
        """
        if len(gravity_list) == 0:
            return
        with open(directory + "/script.lua", "w") as write_file:
            for gravity in gravity_list:
                write_file.write("function gravity_" + gravity["name"] + "(tab_in)\n")
                write_file.write("local tab_out = {}\n")
                write_file.write(gravity["script"]+ "\n")
                write_file.write("	return tab_out\n")
                write_file.write("end\n\n")

    def get_node_rank(self, node_name):
        """used by animation to get node id reference"""
        for count, value in enumerate(self.objects):
            if node_name == value.name:
                return count + 1

    def generate_gltf(self, directory):
        """
        dump the scene to a file
        """
        gltf = {
            "asset": {
                "generator": "MILENDALL-1.0",
                "version": "2.0"
            },
            "extras" : {},
            "scene": 0,
            "scenes": [
                {
                    "nodes": [
                        0
                    ]
                }
            ],
            "nodes": [],
            "meshes": [],
            "buffers": [
                {
                    "uri": "data.bin",
                    "byteLength" : 1
                }
            ],
            "bufferViews": [],
            "materials": [],
            "accessors": [],
            "images": [],
            "textures": [],
            "samplers": [
                {
                    "magFilter": 9729,
                    "minFilter": 9986,
                    "wrapS": 10497,
                    "wrapT": 10497
                }
            ],
            "animations": []
        }

        # data file
        data_file = open(directory + "/data.bin", "wb")
        if data_file is None:
            raise "Unable to open data file"

        # gravity list and modes
        gravity_list = []

        # determine children of each node
        for node in self.objects:
            node.children=[ count+1 for count, value in enumerate(self.objects)
                            if node.name == value.parent]
        # get root nodes
        roots = [count+1 for count, value in enumerate(self.objects) if value.parent is None ]
        gltf_nodes = gltf["nodes"]
        gltf_meshes = gltf["meshes"]
        gltf_nodes.append( { "children" : roots } )
        texture_list = {}
        for node in self.objects:
            gltf_node = { "name" : node.name }
            if node.children != []:
                gltf_node["children"] = node.children
            if node.matrix is not None:
                gltf_node["matrix"] = [
                    node.matrix[0][0], node.matrix[0][1], node.matrix[0][2], 0,
                    node.matrix[1][0], node.matrix[1][1], node.matrix[1][2], 0,
                    node.matrix[2][0], node.matrix[2][1], node.matrix[2][2], 0,
                    node.matrix[3][0], node.matrix[3][1], node.matrix[3][2], 1,
                    ]
            gltf_nodes.append(gltf_node)

            primitives = []

            for texture, faces_block in node.dressing.items():

                # add 2 accessors: points and uv
                points = [ [n.x, n.y, n.z] for n in faces_block["points"]]
                points_accessor = create_accessor(
                        data_file,
                        gltf,
                        points)
                uvs = [ [n.u, n.v] for n in faces_block["points"]]
                tex_coords_accessor = create_accessor(
                        data_file,
                        gltf,
                        uvs)

                texture_id = get_texture_id(gltf, texture_list, texture)

                # accessor for indices, convert faces to triangles
                poly_list = []
                for poly in faces_block["faces"]:
                    for end_index in range(2,len(poly)):
                        poly_list.append(poly[0])
                        poly_list.append(poly[end_index-1])
                        poly_list.append(poly[end_index])
                indices_accessor = create_accessor(
                    data_file,
                    gltf,
                    poly_list)

                # append a primitive object
                primitives.append(
                        {
                            "attributes": {
                                "POSITION": points_accessor,
                                "TEXCOORD_0": tex_coords_accessor
                            },
                            "indices": indices_accessor,
                            "mode": 4,
                            "material": texture_id
                        })
            if primitives != []:
                gltf_meshes.append({
                        "primitives": primitives,
                        "name": "Mesh_%s" %(node.name)
                })
                gltf_node["mesh"] = len(gltf_meshes) - 1

            # generate list of physical faces
            node.gltf_generate_physical_faces(gltf, gltf_node, data_file)

            # add gravity
            node.gltf_generate_gravity(gltf_node, gravity_list)

            # add triggers
            node.gltf_generate_triggers(gltf_node)

        # generate each animation associated to this room
        for animation in self.animations:
            animation.generate_gltf(gltf, data_file, self)

        gltf["buffers"][0]["byteLength"] = data_file.tell()

        # dump gltf file
        j = json.dumps(gltf, indent=1)
        with open(directory + "/room.gltf", "w") as write_file:
            write_file.write(j)
        data_file.close()

        # dump scripts, if any
        self.generate_gravity(gravity_list, directory)

def preview(i_file, o_file, root_dir = os.path.realpath(os.getcwd() + "/../../../data/")):
    """Build a preview of the result gltf so that it can be viewed with a 3rd party"""
    logging.info("Generate to: %s, from %s, with root dir for data: %s", o_file, i_file, root_dir)
    with open(i_file, "r") as gltf_file:
        gltf = json.load(gltf_file)
    src_dir = os.path.dirname(i_file)
    for image in gltf["images"]:
        uri = src_dir + "/" + image["uri"]
        n_uri = os.path.basename(uri)
        dst = src_dir + "/" + n_uri
        if uri != n_uri:
            shutil.copyfile(root_dir + "/" + image["uri"], dst)
            image["uri"] = n_uri
    j = json.dumps(gltf, indent=1)
    with open(o_file, "w") as write_file:
        write_file.write(j)
