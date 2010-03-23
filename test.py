#!/usr/bin/python

import sys, time, random, cStringIO

p = sys.path

try:
	sys.path = ['/home/arni/py-judy/build/lib.cygwin-1.7.1-i686-2.6']
	import judy
finally:
	sys.path = p

def measure(prefix, D):
	X = 0

	t_0 = time.time()

	for i in xrange(1000000):
		D[i] = 1

	t_1 = time.time()

	for i in xrange(1000000):
		X += D[i]

	t_2 = time.time()

	print '%s: %.2f   %.2f' % (prefix, t_1 - t_0, t_2 - t_1)
	print '   %i bytes' % (D.__sizeof__(),)

def test_sizeof():
	import pointless
	J = judy.JudyIntObjectMap()

	J[00] = pointless.PointlessPrimVector('u32', xrange(1000))
	J[10] = pointless.PointlessPrimVector('u32', xrange(10000))
	J[20] = pointless.PointlessPrimVector('u32', xrange(100000))

	print '__sizeof__()      ', J.__sizeof__()
	print '__value_sizeof()__', J.__value_sizeof__()

def test_print():
	J = judy.JudyIntObjectMap()
	P = { }

	random.seed(0)
	k = [random.randint(0, 10000) for i in xrange(10)]
	v = ['a', ['a'], [{},{'a':'b'}], 'k', u'arni', 1.00001, 7, 2, 1, 10]

	for K, V in zip(k, v):
		J[K] = V
		P[K] = V

	for i in xrange(10000000):
		buffer = cStringIO.StringIO()
		print >> buffer, repr(J)
		print >> buffer, repr(P)
		print repr(J)
		print repr(P)

def print_usage_exit():
	sys.exit('usage: ./test.py (--measure | --test-sizeof)')

def main():
	if len(sys.argv) != 2:
		print_usage_exit()

	if sys.argv[1] == '--measure':
		J = judy.JudyIntObjectMap()
		P = dict()

		measure('judy', J)
		measure('dict', P)
	elif sys.argv[1] == '--test-sizeof':
		test_sizeof()
	elif sys.argv[1] == '--test-print':
		test_print()
	else:
		print_usage_exit()

if __name__ == '__main__':
	main()
