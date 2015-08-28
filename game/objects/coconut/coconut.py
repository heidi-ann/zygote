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

sys.path.insert(1, os.path.dirname(os.path.realpath(__file__)) + '/../properties/bagable')
from bagable import Bagable
"""
As the GameObject is in the base objects folder.
"""


"""
"""
class Coconut(GameObject, Bagable):
    weight = 0

    def set_weight(self, new_weight):
        self.weight=new_weight

    def get_weight(self):
        return self.weight

    """ IMPLEMENT THIS IN BAGABLE """
    def player_action(self, player_object):
        #add self to player's bag
        #make self non-solid 
        #make self invisible
        return

    def drop_from_bag():
        #make
        return
