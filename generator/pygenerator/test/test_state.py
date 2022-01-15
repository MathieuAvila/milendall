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

    def test_append_check(self):
        my_state = state.StateList()
        self.assertFalse(my_state.has_state(state.LevelState.Instantiated))
        my_state.add_state(state.LevelState.Instantiated)
        self.assertTrue(my_state.has_state(state.LevelState.Instantiated))
        self.assertFalse(my_state.has_state(state.LevelState.Personalized))
        my_state.add_state(state.LevelState.Personalized)
        self.assertTrue(my_state.has_state(state.LevelState.Instantiated))
        self.assertTrue(my_state.has_state(state.LevelState.Personalized))

    def test_save_read_void(self):
        """ test save 0"""
        my_state = state.StateList()
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [])

    def test_save_read_1(self):
        """ test save 1"""
        my_state = state.StateList()
        my_state.current.append(state.LevelState.Instantiated)
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [state.LevelState.Instantiated])

    def test_save_read_2(self):
        """ test save 2"""
        my_state = state.StateList()
        my_state.current.append(state.LevelState.Instantiated)
        my_state.current.append(state.LevelState.New)
        my_state.save("/tmp/state.txt")
        my_state = state.StateList("/tmp/state.txt")
        logger.info("%s" % my_state.current)
        self.assertEqual(my_state.current, [state.LevelState.Instantiated, state.LevelState.New])

if __name__ == '__main__':
    unittest.main()
