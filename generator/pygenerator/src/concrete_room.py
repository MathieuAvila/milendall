"""
A Concrete Room is a full description of a single room. Its goal is to be converted
to a gltf scene file once everything is defined. So it's an intermediate representation
of a gltf, easier to manipulate during construction by the room itself and the
associated gates.

Note that it is not reconstructed from a file, because all parameters from gates/rooms/objects
should be sufficient to reconstruct a full one.
However, it can be dumped to a file to ease debugging of rooms/gates/objects
"""

import logging
import json
import itertools
import struct
import shutil
import os
import copy

import cgtypes.vec3
import cgtypes.mat4

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

def get_texture_definition(filename, axes=[ ["x"], [ "y"] ] , scale = 1.0, offset = cgtypes.vec3()):
    """ return an easy to use texture definition based on 4 points (vec3)
    and their textures offset (vec3) with 3rd component being 0."""
    my_def = { "texture": filename }
    transform = cgtypes.mat4(
        0, 0, 0, offset.x,
        0, 0, 0, offset.y,
        0, 0, 0, offset.z,
        1.0, 1.0, 1.0,     0)
    for u_v in range(0,2):
        for my_axe in axes[u_v]:
            if my_axe == "x":
                transform[u_v,0] = scale
            if my_axe == "y":
                transform[u_v,1] = scale
            if my_axe == "z":
                transform[u_v,2] = scale
    my_def["proj"] = transform
    return my_def

class Node:

    def __init__(self, _name, _parent = None, _matrix = None):
        self.structure_points = []
        self.structure_faces = []
        self.dressing = {}
        self.matrix = _matrix
        self.name = _name
        self.parent = _parent

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

    def add_dressing_faces(self, points, faces, texture):
        """
        Add dressing faces to object. Used by dressings, not structures
        faces are a table of table of points. There can be any number as long as they
        are of convex shape and in the same plan.
        texture is metadata for textures and is mandatory. There is only one per call. Do
        multiple calls to have different textures.
        """

        print ("ADD TO TEXTURE: " + texture["texture"])

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
            projected_uv = texture["proj"] * cgtypes.vec4(old_point.x, old_point.y, old_point.z, 1.0)
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
            #print("Point [%f,%f,%f %f,%f] new_index is: %i" % (
            #            point.x, point.y, point.z, point.u, point.v,
            #            new_index))
        # insert new faces
        for face in faces:
            new_face = [ new_point[p] for p in face ]
            faces_block.append(new_face)

class JSONEncoder(json.JSONEncoder):

    def default(self, o):
        try:
            res = o.__dict__
            return res
        except AttributeError:
            pass
        return None

def create_accessor(data_file, gltf, elements):
    """Append an accessor for a given elements list data"""

    if isinstance(elements[0], list):
        component_type = 5126
        target = 34962
        all0 = [elem[0] for elem in elements]
        all1 = [elem[1] for elem in elements]
        lmin = []
        lmin.append(min(all0))
        lmin.append(min(all1))
        lmax  = []
        lmax.append(max(all0))
        lmax.append(max(all1))
        final_list = list(itertools.chain.from_iterable(elements))
        if len(elements[0]) == 2:
            elem_type = "VEC2"
        elif len(elements[0]) == 3:
            elem_type = "VEC3"
            all2 = [elem[2] for elem in elements]
            lmin.append(min(all2))
            lmax.append(max(all2))
        else:
            raise "unhandled list type"
    else:
        elem_type = "SCALAR"
        component_type = 5123
        target = 34963
        lmin = [ min(elements) ]
        lmax = [ max(elements) ]
        final_list = elements

    start_pos = data_file.tell()
    if component_type == 5126:
        pad = start_pos % 8
        for _ in range(pad):
            data_file.write(struct.pack('c', b'a'))
        start_pos = data_file.tell()
        for value in final_list:
            data_file.write(struct.pack('f', value))
    if component_type == 5123:
        pad = start_pos % 4
        for _ in range(pad):
            data_file.write(struct.pack('c', b'a'))
        start_pos = data_file.tell()
        for value in final_list:
            data_file.write(struct.pack('H', value))
    end_pos = data_file.tell()
    gltf_buffer_views = gltf["bufferViews"]
    gltf_accessors = gltf["accessors"]
    gltf_buffer_views.append({
                    "buffer": 0,
                    "byteOffset": start_pos,
                    "byteLength": end_pos - start_pos,
                    "target": target})
    gltf_accessors.append(
                {
                    "bufferView": len(gltf_buffer_views) - 1,
                    "byteOffset": 0,
                    "componentType": component_type,
                    "count": len(elements),
                    "type": elem_type,
                    "min": lmin,
                    "max": lmax
                })
    return len(gltf_accessors) - 1

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

    def generate_gltf(self, directory):
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
        }

        # data file
        data_file = open(directory + "/data.bin", "wb")
        if data_file is None:
            raise "Unable to open data file"

        # determine children of each node
        for node in self.objects:
            node.children=[ count+1 for count, value in enumerate(self.objects)
                            if node.name == value.parent]
            print ("node: %s , children: %s" %( node.name, str(node.children)))
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

        gltf["buffers"][0]["byteLength"] = data_file.tell()
        j = json.dumps(gltf, indent=1)
        with open(directory + "/room.gltf", "w") as write_file:
            write_file.write(j)
        data_file.close()

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

        logging.info("Check: original:'%s' uri:'%s' with '%s' to '%s'",
                        image["uri"], uri, n_uri, dst)
        if uri != n_uri:
            logging.info("Copy")
            shutil.copyfile(root_dir + "/" + image["uri"], dst)
            image["uri"] = n_uri
    j = json.dumps(gltf, indent=1)
    with open(o_file, "w") as write_file:
        write_file.write(j)
