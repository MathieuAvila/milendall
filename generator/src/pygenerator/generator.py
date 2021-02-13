#!/usr/bin/python

import level
import sys, getopt
import os


def help():
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
    print("   Output: level-complete.json")
    print()
    print("generator.py room-instantiation    -i level-directory -r room-id")
    print("   Instantiate a given room structure based on level file")
    print("   Input: level-user.json      for room-id")
    print("   Input: level-complete.json  for other rooms")
    print("   Output: level-complete.json")
    print()
    print("generator.py level-graph           -i level-directory")
    print("   Generate a graphviz representation of the level.")
    print("   This will try to run 'dot -Tpng level.graph -olevel.png' to get a PNG image.")
    print("   Input: level-user.json")
    print("   Output: level.graph")
    print("   Output: level.png")
    print()
    print("generator.py level-structure       -i level-directory")
    print("   Instantiate rooms structure based on level file")
    print("   Input: level-complete.json")
    print("   Output: room-$roomid.json (for each room)")
    print()
    print("generator.py room-structure        -i level-directory -r room-id")
    print("   Instantiate a given room structure based on level file and room-id")
    print("   This will replace all previously chosen parameters for the given room")
    print("   Input: level-complete.json      for room-id")
    print("   Output: room-$roomid.json       for each room")
    print()
    print("generator.py level-dressing        -i level-directory")
    print("   Perform dressing for all rooms")
    print("   Input: room-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for each room)")
    print()
    print("generator.py room-dressing        -i level-directory -r room-id")
    print("   Perform dressing for a given room")
    print("   Input: room-$roomid.json")
    print("   Output: roomdress-$roomid.json (for each room)")
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
    print("generator.py level-finalize        -i level-directory")
    print("   Generate final format. This generates gltf format files for rooms with extra-metadata")
    print("   If the dressing has not  been done, work at the structure level (ugly)")
    print("   Input: roomdress-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for each room)")
    print()
    print("generator.py room-finalize        -i level-directory -r room-id")
    print("   Generate a visual representation of a given room at the dressing or structure level.")
    print("   If the dressing phase has not been done, will generate visual based on structure.")
    print("   This is helpful to check that structure is fine before choosing dressing.")
    sys.exit(0)

directory = ''
room = ''
    
def check_level_user():
    if directory == '':
        print("Error, expected directory with level-user.json file. See --help for info.")
        sys.exit(1)
    if not "level-user.json" in os.listdir(directory):
        print("Error, expected directory with level-user.json file. See --help for info.")
        sys.exit(1)
    return level.Level(directory+"/level-user.json")

def main(argv):
    global directory, room
    try:
        opts,args = getopt.getopt(argv,"hi:r:",["directory=","room="])
    except getopt.GetoptError:
        help()
    for opt, arg in opts:
        if opt == '-h':
            help()
        elif opt in ("-i", "--directory"):
            directory = arg
        elif opt in ("-r", "--room"):
            room = arg
    print(args)
    print(opts)
    print('Input directory is "%s"' % directory)
    print('Selected room is "%s"' % room)

    if  (len(args) != 1):
        print("Error, expected action. See --help for info.")
        sys.exit(1)
    action = args[0]

    if action == "level-graph":
        l = check_level_user()
        graph_file = directory + "/level.graph"
        png_file = directory + "/level.png"
        l.dump_graph(graph_file)
        os.system("dot -Tpng " + graph_file + " -o" + png_file)
    elif action == "level-instantiation":
        l = check_level_user()
        print(l.dump_json() + "\n")
    else:
        print("Error, action '" + action + "' unknown. See --help for info.")
        sys.exit(1)

    #print(l.dump_json() + "\n")



if __name__ == "__main__":
   main(sys.argv[1:])
