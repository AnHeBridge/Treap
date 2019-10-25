# -*- coding: utf-8 -*-

# 树堆 
import random, sys
import copy



class DirectionEnum(object):
	Direction_Left = 0
	Direction_Right = 1


class InvalidNode(object):
	pass


class TreapNode(object):
	def __init__(self, key, value):

		global cursor
		self._key = key
		self._value = value
		self._child_node = [None, None]
		self._priority = random.randint(0,10)
		self._size = 1

	def __str__(self):
		leftkey = rightkey = -1
		if self._child_node[0] is not None:
			leftkey = self._child_node[0]._key
		if self._child_node[1] is not None:
			rightkey = self._child_node[1]._key
		return '<key,value,priority:{},{},{},{},{},{}>'.format(self._size, self._key, self._value, self._priority,
															   leftkey, rightkey)

	def __repr__(self):
		return self.__str__()

	def copy_from_other(self, other_node):
		self._key = other_node._key
		self._value = other_node._value
		if self._child_node[0] is not other_node._child_node[0] :
			self._child_node[0] = other_node._child_node[0]
		if self._child_node[1] is not other_node._child_node[1] :
			self._child_node[1] = other_node._child_node[1]
		self._priority = other_node._priority
		self._size = other_node._size

	def maintain(self):
		self._size = 1
		if self._child_node[0] is not None:
			self._size = self._size + self._child_node[0]._size
		if self._child_node[1] is not None:
			self._size = self._size + self._child_node[1]._size

	def Rotate(self, rotate_direct):
		opposite_node = self._child_node[rotate_direct ^ 1]
		self._child_node[rotate_direct ^ 1] = copy.deepcopy(opposite_node._child_node[rotate_direct])
		if opposite_node._child_node[rotate_direct] is None :
			opposite_node._child_node[rotate_direct] = TreapNode(None,None)

		opposite_node._child_node[rotate_direct].copy_from_other(self)

		self.maintain()
		opposite_node.maintain()
		self.copy_from_other(opposite_node)

	def Insert(self, key, value):
		direct = DirectionEnum.Direction_Right
		if self._key > key:
			direct = DirectionEnum.Direction_Left

		if self._child_node[direct] is None:
			self._child_node[direct] = TreapNode(key, value)
		else:
			self._child_node[direct].Insert(key, value)

		if self._child_node[direct]._priority > self._priority:
			self.Rotate(direct ^ 1)

		self.maintain()


class Treap(object):
	def __init__(self):
		self._root = None

	@property
	def size(self):
		if not self._root:
			return 0
		return self._root._size

	def Insert(self, key, value):
		if self._root is None:
			self._root = TreapNode(key, value)
		else:
			self._root.Insert(key, value)

	def Kth(self, treap_node, k):
		if treap_node._child_node[0] is None and k == 1:
			return treap_node
		if treap_node._child_node[0] is not None and treap_node._child_node[0]._size == k - 1:
			return treap_node
		if treap_node._child_node[0] is not None and treap_node._child_node[0]._size > k - 1:
			return self.Kth(treap_node._child_node[0], k)

		k = k - 1
		if treap_node._child_node[0] is not None:
			k = k - treap_node._child_node[0]._size
		return self.Kth(treap_node._child_node[1], k)

		return None







