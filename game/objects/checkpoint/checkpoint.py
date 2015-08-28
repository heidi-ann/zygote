import operator
import os
"""
In Python comments,
could define some standard which the C++ code can use to determine things about it handles 
the python code
"""

"""
eg. We could be able to write:
"""
#__ import_object  game_object/GameObject
"""
The game code, at runtime, could recognise the "#__"
and replace it with:
"""
import sys
sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/..')
from game_object import GameObject
"""
As the GameObject is in the base objects folder.
"""


"""
"""
class Checkpoint(GameObject):
    def initialise(self):
        self.set_sprite("")
        self.set_visible(True)
        self.dialogue = []

    def player_walked_on(self, player_object):
        self.get_engine().run_callback_list_sequence(self.dialogue)

