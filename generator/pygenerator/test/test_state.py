"""
test level
"""

import unittest
import state
import logging

logger = logging.getLogger("test_state")
logger.setLevel(logging.INFO)

class TestState(unittest.TestCase):

    def test_init_void(self):
        """ test void init"""
        my_state = state.StateList()
        self.assertEqual(my_state.current, [])

    def test_save_read_void(self):
        """ test save"""
        my_state = state.StateList()
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [])

    def test_save_read_1(self):
        """ test save"""
        my_state = state.StateList()
        my_state.current.append(state.LevelState.Instantiated)
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [state.LevelState.Instantiated])

    def test_save_read_2(self):
        """ test save"""
        my_state = state.StateList()
        my_state.current.append(state.LevelState.Instantiated)
        my_state.current.append(state.LevelState.New)
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [state.LevelState.Instantiated, state.LevelState.New])

if __name__ == '__main__':
    unittest.main()
