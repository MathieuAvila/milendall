#!/usr/bin/python

"""
Command-line program
"""

from __future__ import annotations

from typing import Sequence

import sys
import getopt
import os
import logging
import selector_regular

import level
from state import LevelState

logging.basicConfig()
logger = logging.getLogger("generator")
logger.setLevel(logging.INFO)

def my_help() -> None:
    """
    Print help and exit
    """
    print("generator.py : Integrated tool to build levels.")
    print("               It works at various layers of generation to help control the output")
    print()
    print("Generation steps are: " + " ".join(LevelState.__members__.keys()))
    print()
    print("generator.py level-directory [-h] [-o dir] [-f] [-s start_step] [-e end-step] [-r room] [-p] [-P] [-v]")
    print()
    print("By default, it will generate all steps from the most advanced to")
    print("the final one, and finish with generating the final content.")
    print()
    print("-h : This help text.")
    print("-f : Force generation of the given steps, even if step is aleady generated.")
    print("-o : Output directory.")
    print("-s : Start at this step.")
    print("-e : End at this step.")
    print("-r : Only this room.")
    print("-p : Generate intermediate previews for steps that provides them.")
    print("-P : Only generate intermediate previews for steps that provides them.")
    print("     Won't generate the steps themselves.")
    print("-v : Increase verbosity")
    print()


    sys.exit(0)

def check_level(directory: str, filename: str) -> level.Level:
    """ Get level, check it loads and return it"""
    if directory == '':
        print("Error, expected directory. See --help for info.")
        sys.exit(1)
    if filename not in os.listdir(directory):
        print("Error, expected directory with ", filename, "file. See --help for info.")
        sys.exit(1)
    selector = selector_regular.SelectorRegular()
    my_level = level.Level(selector)
    my_level.load(directory, LevelState.Story)
    return my_level

def check_level_user(directory: str) -> level.Level:
    """Get a user defined level"""
    return check_level(directory,"level-user.json")

def check_level_instance(directory: str) -> level.Level:
    """Get a user instanced level"""
    return check_level(directory,"level-instance.json")

def main(argv: Sequence[str]) -> None:

    preview = False
    force = False
    only_preview = False
    start_step: LevelState | None = None
    end_step: LevelState | None = None
    output_dir = None

    try:
        opts,args = getopt.getopt(argv,"ho:fs:e:r:pPv")
    except getopt.GetoptError:
        my_help()
    for opt, arg in opts:
        if opt == '-h':
            my_help()
        elif opt in ("-o"):
            output_dir = arg
            print("Option: Output set to: %s" % output_dir)
        elif opt in ("-r"):
            logger.debug("Option: Room %s", arg)
        elif opt in ("-s"):
            start_step = LevelState.__members__[arg]
        elif opt in ("-e"):
            end_step = LevelState.__members__[arg]
        elif opt in ("-f"):
            force = True
            print("Option: Force generation.")
        elif opt in ("-p"):
            preview = True
            print("Option: Preview")
        elif opt in ("-P"):
            print("Option: Only preview")
            only_preview = True
        elif opt in ("-v"):
            logging.getLogger().setLevel(logging.DEBUG)
            logger.debug("Set verbose to debug")

    if len(args) != 1:
        logger.error("Error, expected level directory. See --help for info.")
        sys.exit(1)
    directory = args[0]
    if output_dir is None:
        output_dir = directory

    selector = selector_regular.SelectorRegular()
    my_level = level.Level(selector)
    try:
        states = my_level.read_state_list(directory)
    except Exception as e:
        logger.error("Invalid directory or unreadable level state. ERROR=%s" % str(e))
        sys.exit(1)
    logger.info("Level has states: %s" % states)

    if start_step is None:
        start_step = max(states.current)
    if end_step is None:
        end_step = LevelState.Finalize
    logger.info("Need to open at step:%s and generate up to:%s" % (start_step, end_step))

    try:
        my_level.load(directory, start_step)
    except Exception as e:
        logger.error("Unable to load level at this step. ERROR=%s" % str(e), exc_info=True)
        sys.exit(1)

    for step in LevelState:
        if step > start_step and step <= end_step:
            logger.info("Apply step: %s" % step.name)
            if states.has_state(step) and not force:
                logger.warning("Step is already generated, and force is not set. Will exit.")
                sys.exit(1)
            if not only_preview:
                my_level.run_step(step)
                my_level.save(output_dir)
                if step == LevelState.Finalize:
                    my_level.finalize(output_dir, preview)
            if preview or only_preview:
                my_level.preview(output_dir)



if __name__ == "__main__":
    main(sys.argv[1:])
