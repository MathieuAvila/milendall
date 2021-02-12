#!/usr/bin/python

import level
import sys, getopt


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
    print("   Input: roomdress-$roomid.json")
    print("   Output: roomfinal-$roomid.json (for each room)")
    print()
    print("generator.py room-visualize        -i level-directory -r room-id")
    print("   Generate a visual representation of a given room at the dressing or structure level.")
    print("   If the dressing phase has not been done, will generate visual based on structure.")
    print("   This is helpful to check that structure is fine before choosing dressing.")
    sys.exit(0)

def main(argv):
    directory = ''
    room = ''
    try:
        opts,args = getopt.getopt(argv,"hi:r:",["directory=","room="])
    except getopt.GetoptError:
        help()
    for opt, arg in opts:
        if opt == '-h':
            help()
        elif opt in ("-i", "--directory"):
            directory = arg
        elif opt in ("-o", "--room"):
            room = arg
    print(args)
    print(opts)
    print('Input directory is "%s"' % directory)
    print('Selected room is "%s"' % room)

    l = level.Level("../../samples/simple_level.json")
    l.dump_graph("/tmp/sample.graph")
    #print(l.dump_json() + "\n")



if __name__ == "__main__":
   main(sys.argv[1:])
