import os, sys, commands
from distutils.core import setup, Extension

print 'INFO: building judy static library...'

a, b = commands.getstatusoutput('(cd judy/src; COPT=\'-O3 -fPIC\' ./sh_build)')

if a != 0:
	sys.exit(b)

setup(
	name = 'Judy Python API',
	version = '1.0',
	maintainer = 'Arni Mar Jonsson',
	maintainer_email = 'arnimarj@gmail.com',
	description = 'A Python wrapper for Judy arrays, which provide fast and space-efficient integer mappings and integer sets, along with ranged ordered iterations',
	url = 'http://code.google.com/p/py-judy/',

	classifiers = [
		'Development Status :: 4 - Production',
		'Environment :: Other Environment',
		'Intended Audience :: Developers',
		'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
		'Operating System :: POSIX',
		'Programming Language :: C',
		'Programming Language :: Python',
		'Programming Language :: Python :: 2.6',
		'Programming Language :: Python :: 2.7',
		'Topic :: Database',
		'Topic :: Software Development :: Libraries'
	],

	packages = ['judy'],
	package_dir = {'judy': ''},

	ext_modules = [
		Extension('judy',
			include_dirs = [ './judy/src', '/usr/include' ],
			library_dirs = [ '/usr/lib' ],

			sources = [
				'judy.c',
				'judy_int_object_map.c',
				'judy_int_set.c',
				'utils.c'
			],

			extra_compile_args = ['-pedantic', '-std=c99', '-Wall', '-Wno-strict-prototypes', '-g', '-D_GNU_SOURCE', '-O2'],
			extra_link_args = ['-L./judy/src', '-Bstatic', '-lJudy']
		)
	]
)
