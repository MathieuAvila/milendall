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

def get_texture_definition_with_map(filename, map_method):
    """ return a texture definition based on a map method to provide."""
    my_def = { "texture": filename , "map_method": map_method }
    return my_def

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
