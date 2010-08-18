#!/usr/bin/python

from twisted.trial import unittest

import weakref, gc
import judy

# this is a subset of the Python dict-tests

class JudyIntObjectMapTest(unittest.TestCase):
	def test_empty_constructor(self):
		self.assertEqual(list(judy.JudyIntObjectMap().iteritems()), list({}.iteritems()))

	def test_keys(self):
		j = judy.JudyIntObjectMap()
		self.assertEqual(list(j.iterkeys()), [])
		j = {2: 'foo', 4: 'bar'}
		k = list(j.iterkeys())
		self.assert_(2 in j)
		self.assert_(4 in j)
		self.assert_(2 in k)
		self.assert_(4 in k)
		self.assertRaises(TypeError, j.iterkeys, None)

	def test_bool(self):
		self.assert_(not {})
		self.assert_({1: 2})
		self.assert_(bool({}) is False)
		self.assert_(bool({1: 2}) is True)

	def test_values(self):
		j = judy.JudyIntObjectMap()
		self.assertEqual(list(j.itervalues()), [])
		j[1] = 2
		self.assertEqual(list(j.itervalues()), [2])
		self.assertRaises(TypeError, j.itervalues, None)
		self.assertRaises(TypeError, j.itervalues, 1, None)
		self.assertRaises(TypeError, j.itervalues, None, 1)

	def test_items(self):
		j = judy.JudyIntObjectMap()
		self.assertEqual(list(j.iteritems()), [])

		j[1] = 'foo'
		self.assertEqual(list(j.iteritems()), [(1, 'foo')])

		self.assertRaises(TypeError, j.iteritems, None)
		self.assertRaises(TypeError, j.iteritems, 1, None)
		self.assertRaises(TypeError, j.iteritems, None, 1)

	def test_contains(self):
		j = judy.JudyIntObjectMap()
		self.assert_(not ('a' in j))
		self.assert_('a' not in j)
		j[1] = 'a'
		j[2] = 'b'
		self.assert_(not ('a' in j))
		self.assert_('a' not in j)
		self.assert_(1 in j)
		self.assert_(2 in j)
		self.assert_(0 not in j)

		self.assertRaises(TypeError, j.__contains__)

	def test_len(self):
		j = judy.JudyIntObjectMap()
		self.assertEqual(len(j), 0)
		j[1] = 'a'
		j[2] = 'b'
		self.assertEqual(len(j), 2)

	def test_clear(self):
		j = judy.JudyIntObjectMap()
		j[1] = 'a'
		j[2] = 'b'
		j[3] = 'c'
		j.clear()
		self.assertEqual(dict(j.iteritems()), {})
		self.assertRaises(TypeError, j.clear, None)

#	def test_update(self):
#		d = {}
#		d.update({1:100})
#		d.update({2:20})
#		d.update({1:1, 2:2, 3:3})
#		self.assertEqual(d, {1:1, 2:2, 3:3})
#
#		d.update()
#		self.assertEqual(d, {1:1, 2:2, 3:3})
#
#		self.assertRaises((TypeError, AttributeError), d.update, None)
#
#		class SimpleUserDict:
#			def __init__(self):
#				self.d = {1:1, 2:2, 3:3}
#			def keys(self):
#				return self.d.keys()
#			def __getitem__(self, i):
#				return self.d[i]
#		d.clear()
#		d.update(SimpleUserDict())
#		self.assertEqual(d, {1:1, 2:2, 3:3})
#
#		class Exc(Exception): pass
#
#		d.clear()
#		class FailingUserDict:
#			def keys(self):
#				raise Exc
#		self.assertRaises(Exc, d.update, FailingUserDict())
#
#		class FailingUserDict:
#			def keys(self):
#				class BogonIter:
#					def __init__(self):
#						self.i = 1
#					def __iter__(self):
#						return self
#					def next(self):
#						if self.i:
#							self.i = 0
#							return 'a'
#						raise Exc
#				return BogonIter()
#			def __getitem__(self, key):
#				return key
#		self.assertRaises(Exc, d.update, FailingUserDict())
#
#		class FailingUserDict:
#			def keys(self):
#				class BogonIter:
#					def __init__(self):
#						self.i = ord('a')
#					def __iter__(self):
#						return self
#					def next(self):
#						if self.i <= ord('z'):
#							rtn = chr(self.i)
#							self.i += 1
#							return rtn
#						raise StopIteration
#				return BogonIter()
#			def __getitem__(self, key):
#				raise Exc
#		self.assertRaises(Exc, d.update, FailingUserDict())
#
#		class badseq(object):
#			def __iter__(self):
#				return self
#			def next(self):
#				raise Exc()
#
#		self.assertRaises(Exc, {}.update, badseq())
#
#		self.assertRaises(ValueError, {}.update, [(1, 2, 3)])


#	def test_fromkeys(self):
#		self.assertEqual(dict.fromkeys('abc'), {'a':None, 'b':None, 'c':None})
#		d = {}
#		self.assert_(not(d.fromkeys('abc') is d))
#		self.assertEqual(d.fromkeys('abc'), {'a':None, 'b':None, 'c':None})
#		self.assertEqual(d.fromkeys((4,5),0), {4:0, 5:0})
#		self.assertEqual(d.fromkeys([]), {})
#		def g():
#			yield 1
#		self.assertEqual(d.fromkeys(g()), {1:None})
#		self.assertRaises(TypeError, {}.fromkeys, 3)
#		class dictlike(dict): pass
#		self.assertEqual(dictlike.fromkeys('a'), {'a':None})
#		self.assertEqual(dictlike().fromkeys('a'), {'a':None})
#		self.assert_(type(dictlike.fromkeys('a')) is dictlike)
#		self.assert_(type(dictlike().fromkeys('a')) is dictlike)
#		class mydict(dict):
#			def __new__(cls):
#				return UserDict.UserDict()
#		ud = mydict.fromkeys('ab')
#		self.assertEqual(ud, {'a':None, 'b':None})
#		self.assert_(isinstance(ud, UserDict.UserDict))
#		self.assertRaises(TypeError, dict.fromkeys)
#
#		class Exc(Exception): pass
#
#		class baddict1(dict):
#			def __init__(self):
#				raise Exc()
#
#		self.assertRaises(Exc, baddict1.fromkeys, [1])
#
#		class BadSeq(object):
#			def __iter__(self):
#				return self
#			def next(self):
#				raise Exc()
#
#		self.assertRaises(Exc, dict.fromkeys, BadSeq())
#
#		class baddict2(dict):
#			def __setitem__(self, key, value):
#				raise Exc()
#
#		self.assertRaises(Exc, baddict2.fromkeys, [1])
#
#		# test fast path for dictionary inputs
#		d = dict(zip(range(6), range(6)))
#		self.assertEqual(dict.fromkeys(d, 0), dict(zip(range(6), [0]*6)))

#	def test_copy(self):
#		d = {1:1, 2:2, 3:3}
#		self.assertEqual(d.copy(), {1:1, 2:2, 3:3})
#		self.assertEqual({}.copy(), {})
#		self.assertRaises(TypeError, d.copy, None)

	def test_get(self):
		j = judy.JudyIntObjectMap()
		self.assert_(j.get('c') is None)
		self.assertEqual(j.get('c', 3), 3)
		j[1] = 'a'
		j[2] = 'b'
		self.assert_(j.get(3) is None)
		self.assertEqual(j.get(3, 3), 3)
		self.assertEqual(j.get(2), 'b')
		self.assertEqual(j.get(1, 3), 'a')
		self.assertRaises(TypeError, j.get)
		self.assertRaises(TypeError, j.get, None, None, None)

#	def test_setdefault(self):
#		# dict.setdefault()
#		d = {}
#		self.assert_(d.setdefault('key0') is None)
#		d.setdefault('key0', [])
#		self.assert_(d.setdefault('key0') is None)
#		d.setdefault('key', []).append(3)
#		self.assertEqual(d['key'][0], 3)
#		d.setdefault('key', []).append(4)
#		self.assertEqual(len(d['key']), 2)
#		self.assertRaises(TypeError, d.setdefault)
#
#		class Exc(Exception): pass
#
#		class BadHash(object):
#			fail = False
#			def __hash__(self):
#				if self.fail:
#					raise Exc()
#				else:
#					return 42
#
#		x = BadHash()
#		d[x] = 42
#		x.fail = True
#		self.assertRaises(Exc, d.setdefault, x, [])

#	def test_popitem(self):
#		# dict.popitem()
#		for copymode in -1, +1:
#			# -1: b has same structure as a
#			# +1: b is a.copy()
#			for log2size in range(12):
#				size = 2**log2size
#				a = {}
#				b = {}
#				for i in range(size):
#					a[repr(i)] = i
#					if copymode < 0:
#						b[repr(i)] = i
#				if copymode > 0:
#					b = a.copy()
#				for i in range(size):
#					ka, va = ta = a.popitem()
#					self.assertEqual(va, int(ka))
#					kb, vb = tb = b.popitem()
#					self.assertEqual(vb, int(kb))
#					self.assert_(not(copymode < 0 and ta != tb))
#				self.assert_(not a)
#				self.assert_(not b)
#
#		d = {}
#		self.assertRaises(KeyError, d.popitem)

#	def test_pop(self):
#		# Tests for pop with specified key
#		d = {}
#		k, v = 'abc', 'def'
#		d[k] = v
#		self.assertRaises(KeyError, d.pop, 'ghi')
#
#		self.assertEqual(d.pop(k), v)
#		self.assertEqual(len(d), 0)
#
#		self.assertRaises(KeyError, d.pop, k)
#
#		# verify longs/ints get same value when key > 32 bits (for 64-bit archs)
#		# see SF bug #689659
#		x = 4503599627370496L
#		y = 4503599627370496
#		h = {x: 'anything', y: 'something else'}
#		self.assertEqual(h[x], h[y])
#
#		self.assertEqual(d.pop(k, v), v)
#		d[k] = v
#		self.assertEqual(d.pop(k, 1), v)
#
#		self.assertRaises(TypeError, d.pop)
#
#		class Exc(Exception): pass
#
#		class BadHash(object):
#			fail = False
#			def __hash__(self):
#				if self.fail:
#					raise Exc()
#				else:
#					return 42
#
#		x = BadHash()
#		d[x] = 42
#		x.fail = True
#		self.assertRaises(Exc, d.pop, x)

	def test_repr(self):
		j = judy.JudyIntObjectMap()
		self.assertEqual(repr(j), '{}')
		j[1] = 2
		self.assertEqual(repr(j), '{1: 2}')
		j = judy.JudyIntObjectMap()
		j[1] = j
		self.assertEqual(repr(j), '{1: {...}}')

		class Exc(Exception):
			pass

		class BadRepr(object):
			def __repr__(self):
				raise Exc()

		d = {1: BadRepr()}
		self.assertRaises(Exc, repr, d)

#	def test_le(self):
#		self.assert_(not ({} < {}))
#		self.assert_(not ({1: 2} < {1L: 2L}))
#
#		class Exc(Exception): pass
#
#		class BadCmp(object):
#			def __eq__(self, other):
#				raise Exc()
#			def __hash__(self):
#				return 42
#
#		d1 = {BadCmp(): 1}
#		d2 = {1: 1}
#		try:
#			d1 < d2
#		except Exc:
#			pass
#		else:
#			self.fail('< didn't raise Exc')

	def test_tuple_keyerror(self):
		# SF #1576657
		j = judy.JudyIntObjectMap()
		try:
			j[(1,)]
		except KeyError, e:
			self.assertEqual(e.args, ((1,),))
		else:
			self.fail('missing KeyError')

	def test_container_iterator(self):
		# Bug #3680: tp_traverse was not implemented for dictiter objects
		class C(object):
			pass

		iterators = (judy.JudyIntObjectMap.iteritems, judy.JudyIntObjectMap.itervalues, judy.JudyIntObjectMap.iterkeys)

		for i in iterators:
			obj = C()
			ref = weakref.ref(obj)
			container = judy.JudyIntObjectMap()
			container[1] = obj
			obj.x = i(container)
			del obj, container
			gc.collect()
			self.assert_(ref() is None, 'Cycle was not collected')

	def test_container_refs(self):
		# Bug #3680: tp_traverse was not implemented for dictiter objects
		class C(object):
			pass

		obj = C()

		j = judy.JudyIntObjectMap()
		j[1] = obj
		j[2] = j

		ref = weakref.ref(obj)

		obj.x = j

		del obj, j
		gc.collect()

		self.assert_(ref() is None, 'Cycle was not collected')

'''
from test import mapping_tests

class GeneralMappingTests(mapping_tests.BasicTestMappingProtocol):
	type2test = dict

class Dict(dict):
	pass

class SubclassMappingTests(mapping_tests.BasicTestMappingProtocol):
	type2test = Dict

def test_main():
	test_support.run_unittest(
		GeneralMappingTests,
		SubclassMappingTests,
	)

'''
