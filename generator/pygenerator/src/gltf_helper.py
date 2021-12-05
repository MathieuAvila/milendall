"""
Helpers to create GLTF file
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

def get_texture_definition_with_map(filename, map_method):
    """ return a texture definition based on a map method to provide."""
    my_def = { "texture": filename , "map_method": map_method }
    return my_def

def get_texture_definition_with_function(filename, function, context):
    """ return a texture definition based on a map method to provide."""
    my_def = { "texture": filename , "function": function, "context": context }
    return my_def

def get_texture_definition_function_simple_mapper(filename, scale_x=1.0, scale_y=1.0):
    """ return a simple function mapper."""

    my_context = { "scale_x": scale_x, "scale_y": scale_y}

    def mapper(points, face, context, my_points):

        # checks
        if len(my_points) < 3:
            logging.info("Face has {} points, must have at least 3.", len(my_points))
            return

        print("===========")
        print(len(my_points))
        print(my_points[0])
        print(my_points[1])
        print(my_points[2])

        # get points 0,1 to compute U
        u = my_points[1] - my_points[0]
        U = cgtypes.vec3(u.x, u.y, u.z)
        U = U.normalize()
        # get points 0,N-1 to compute V
        new_vec_org = my_points[len(my_points)-1] - my_points[0]
        new_vec = cgtypes.vec3(new_vec_org.x, new_vec_org.y, new_vec_org.z)
        normal = new_vec.cross(U).normalize()
        V = U.cross(normal)
        print("U = %s, V= %s" % (U, V))

        for p in my_points:
            print(p)
            p.u = (p * U) * scale_x
            p.v = (p * V) * scale_y

    return get_texture_definition_with_function(filename , mapper, my_context)

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
        elif len(elements[0]) == 4:
            elem_type = "VEC4"
            all2 = [elem[2] for elem in elements]
            all3 = [elem[3] for elem in elements]
            lmin.append(min(all2))
            lmax.append(max(all2))
            lmin.append(min(all3))
            lmax.append(max(all3))
        else:
            raise "unhandled list type"
    else:
        elem_type = "SCALAR"
        if isinstance(elements[0], int):
            component_type = 5123
        elif isinstance(elements[0], float):
            component_type = 5126
        else:
            raise "unexpected scalar type"
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
                    "byteLength": end_pos - start_pos #,
                    #"target": target
                    }
                    )
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

def vec4_to_vec3(v):
    return cgtypes.vec3(v.x, v.y, v.z)
