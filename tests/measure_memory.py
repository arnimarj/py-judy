#!/usr/bin/python

import sys, random, urllib
import judy

# mapping memory excluding values
def judy_io_map_mem(j):
	return j.__sizeof__()

def python_io_map_mem(d):
	return d.__sizeof__() + sum(k.__sizeof__() for k in d.iterkeys())

# mapping memory excluding values
def judy_ii_map_mem(j):
	return j.__sizeof__()

def python_ii_map_mem(d):
	return d.__sizeof__() + sum(k.__sizeof__() + v.__sizeof__() for k, v in d.iteritems())

# set memory
def judy_i_set_mem(j):
	return j.__sizeof__()

def python_i_set_mem(s):
	return s.__sizeof__() + sum(k.__sizeof__() for k in s)

# race-organizer
def mem_measure(kv_sequence_func, sequence_name):
	# our x-values
	x = [int((1.2)**i) for i in xrange(50, 60, 1)]

	# and our measured values
	m_j_io = [ ]
	m_p_io = [ ]

	# m_j_ii = [ ]
	# m_p_ii = [ ]

	m_j_i = [ ]
	m_p_i = [ ]

	# for each race
	for N, n in enumerate(x):
		print 'N', N, n
		# our candidates
		j_io = judy.JudyIntObjectMap()
		p_io = dict()

		# j_ii = judy.JudyIntIntMap()
		# p_ii = dict()

		j_i = judy.JudyIntSet()
		p_i = set()

		# some value to put in io mappings
		io_v = 'foobar'

		for k, v in kv_sequence_func(n):
			j_io[k] = io_v
			p_io[k] = io_v

			# j_ii[k] = v
			# p_ii[k] = v

			j_i.add(k)
			p_i.add(k)

		m_j_io.append(judy_io_map_mem(j_io))
		m_p_io.append(python_io_map_mem(p_io))

		# m_j_ii.append(judy_ii_map_mem(j_ii))
		# m_p_ii.append(python_ii_map_mem(p_ii))

		m_j_i.append(judy_i_set_mem(j_i))
		m_p_i.append(python_i_set_mem(p_i))

	if sys.maxint == 2**63-1:
		bits = 64
	elif sys.maxint == 2**31-1:
		bits = 32
	else:
		raise ValueError('I miss my bits')

	print format_graph(x, m_j_io, m_p_io, 'int/object map',    sequence_name, bits)
	# emit_graph(x, m_j_ii, m_p_ii, 'int/int maps',    sequence_name, bits)
	print format_graph(x, m_j_i,  m_p_i,  'int set',           sequence_name, bits)

def format_graph(x, y_j, y_p, title, name, bits):
	y_j = [float(v) / (1024**2) for v in y_j]
	y_p = [float(v) / (1024**2) for v in y_p]

	x = ['%.fK' % (float(v) / 1000.0,) for v in x]

	url = 'http://chart.apis.google.com/chart'
	p = {
		'chxl': '0:|%s|1:#keys|2:|MBs' % ('|'.join(map(str, x)),),
		'chxp': '|1,50|2,200',
		'chxs': '1,676767,11.5,0,lt,676767',
		'chxt': 'x,x,y',
		'chbh': 'a',
		'chs':  '600x450',
		'cht':  'bvg',
		'chco': 'A2C180,3D7930',
		'chds': '0,%i' % (max(max(y_j), max(y_p)),),
		'chd':  't:%s|%s' % (','.join(map(str, y_j)), ','.join(map(str, y_p))),
		'chdl': 'Judy %s|Python %s' % (title, title),
		'chg':  '-1,0,0,4',
		'chtt': name,
	}

	return '%s?%s' % (url, urllib.urlencode(p))

def main():
	# our integer range
	i_min = 0
	i_max = sys.maxint * 2 + 1

	# non-duplicate k/v pairs
	def random_seq(n):
		random.seed(0)
		s = judy.JudyIntSet()

		def unique_i():
			while True:
				v = random.randint(i_min, i_max)

				if v in s:
					continue

				s.add(v)
				return v

		for i in xrange(n):
			k = unique_i()
			v = unique_i()
			yield k, v

	def half_random_half_cont_seq(n):
		random.seed(0)
		s = judy.JudyIntSet()

		def unique_i():
			while True:
				v = random.randint(i_min, i_max)

				if v in s:
					continue

				s.add(v)
				return v

		contig_i = random.randint(i_min, i_max - n//2)

		for i in xrange(n // 2):
			k_a = random.randint(i_min, i_max)
			k_b = contig_i + i

			v_a = random.randint(i_min, i_max)
			v_b = random.randint(i_min, i_max)

			yield k_a, v_a
			yield k_b, v_b

	def cont_seq(n):
		random.seed(0)
		s = judy.JudyIntSet()

		contig_i = random.randint(i_min, i_max - n//2)

		for i in xrange(n):
			k = contig_i + i
			v = random.randint(i_min, i_max)
			yield k, v

	# full-random
	# mem_measure(random_seq, 'random keys')

	# half-contiguous, half-random
	# mem_measure(half_random_half_cont_seq, 'half random keys, half contiguous keys)

	# full-contiguous
	mem_measure(cont_seq, 'contiguous keys')

if __name__ == '__main__':
	main()
