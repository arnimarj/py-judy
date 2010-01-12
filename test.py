#!/usr/bin/python

import sys, time

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

J = judy.JudyIntObjectMap()
P = dict()

measure('judy', J)
measure('dict', P)
