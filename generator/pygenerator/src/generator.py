#!/usr/bin/python

"""
Command-line program
"""

import sys
import getopt
import os
import logging
import selector_regular

import level

logging.basicConfig()
logging.getLogger().setLevel(logging.INFO)

def my_help():
    """
    Print help and exit
    """
    print("generator.py : Integrated tool to build levels.")
    print("               It works at various layers of generation to help control the output")
    print()
    print("generator.py strategy-create       -i level-directory")
    print("   Create strategy file from general parameters")
    print()
    print("generator.py strategy-level        -i level-directory")
    print("   Create level skeleton based on strategy file")
    print("   Input: strategy.json")
    print("   Output: level-user.json")
    print()
    print("generator.py level-instantiation   -i level-directory")
    print("   Complete level parameters based on level file")
    print("   Input: level-user.json")
    print("   Output: level-instance.json")
    print()
    print("generator.py room-instantiation    -i level-directory -r room-id")
    print("   Instantiate a given room structure based on level file")
    print("   Input: level-user.json      for room-id")
    print("   Input: level-instance.json  for other rooms")
    print("   Output: level-instance.json")
    print()
    print("generator.py level-graph           -i level-directory")
    print("   Generate a graphviz representation of the level.")
    print("   This will try to run 'dot -Tpng level.graph -olevel.png' to get a PNG image.")
    print("   Input: level-user.json")
    print("   Output: level.graph")
    print("   Output: level.png")
    print()
    print("generator.py level-objects        -i level-directory")
    print("   Fill objects for all rooms")
    print("   Input: roomdress-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for each room)")
    print()
    print("generator.py room-objects        -i level-directory -r room-id")
    print("   Fill objects for a given room")
    print("   Input: roomdress-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for given room)")
    print()
    print("generator.py level-finalize     [-p,--preview]   -i level-directory")
    print("   Generate final format. This generates gltf format files for rooms"
          " with extra-metadata")
    print("   If the dressing has not been done, work at the structure level (ugly)")
    print("   Input: roomdress-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for each room)")
    print()
    print("generator.py room-finalize        -i level-directory -r room-id")
    print("   Generate a visual representation of a given room at the dressing or structure level.")
    print("   If the dressing phase has not been done, will generate visual based on structure.")
    print("   This is helpful to check that structure is fine before choosing dressing.")
    sys.exit(0)

def check_level(directory, filename):
    """ Get level, check it loads and return it"""
    if directory == '':
        print("Error, expected directory. See --help for info.")
        sys.exit(1)
    if not filename in os.listdir(directory):
        print("Error, expected directory with ", filename, "file. See --help for info.")
        sys.exit(1)
    return level.Level(directory+"/" + filename, selector_regular.SelectorRegular())

def check_level_user(directory):
    """Get a user defined level"""
    return check_level(directory,"level-user.json")

def check_level_instance(directory):
    """Get a user instanced level"""
    return check_level(directory,"level-instance.json")

def main(argv):
    """
    when called from outside
    """
    directory = ""
    room = ""
    preview = False
    try:
        opts,args = getopt.getopt(argv,"hvi:r:p",["directory=","room="])
    except getopt.GetoptError:
        help()
    for opt, arg in opts:
        if opt == '-h':
            my_help()
        elif opt in ("-i", "--directory"):
            directory = arg
        elif opt in ("-r", "--room"):
            room = arg
        elif opt in ("-p", "--preview"):
            preview = True
        elif opt in ("-v", "--verbose"):
            logging.getLogger().setLevel(logging.DEBUG)
            logging.debug("Set verbose to debug")

    #print('Input directory is "%s"' % directory)
    #print('Selected room is "%s"' % room)

    if len(args) != 1:
        print("Error, expected action. See --help for info.")
        sys.exit(1)
    action = args[0]

    if action == "level-graph":
        loaded_level = check_level_user(directory)
        graph_file = directory + "/level.graph"
        png_file = directory + "/level.png"
        loaded_level.dump_graph(graph_file)
        os.system("dot -Tpng " + graph_file + " -o" + png_file)
    elif action == "level-instantiation":
        loaded_level = check_level_user(directory)
        loaded_level.instantiation()
        loaded_level.save(directory + "/level-instance.json")
        #print(loaded_level.dump_json() + "\n")
    elif action == "level-finalize":
        loaded_level = check_level_instance(directory)
        loaded_level.finalize(directory, preview)
    else:
        print("Error, action '" + action + "' unknown. See --help for info.")
        sys.exit(1)

    #print(l.dump_json() + "\n")



if __name__ == "__main__":
    main(sys.argv[1:])
