import os, sys, commands
from distutils.core import setup, Extension

print 'INFO: building judy static library...'

if sys.maxint == 2**63-1:
	CFLAGS = '-O3 -fPIC -DJU_64BIT'
elif sys.maxint == 2**31-1:
	CFLAGS = '-O3 -fPIC'
else:
	sys.exit('bad sys.maxint')

a, b = commands.getstatusoutput('(cd judy/src; COPT=\'%s\' sh ./sh_build)' % (CFLAGS,))

if a != 0:
	sys.exit(b)

setup(
	name = 'judy',
	version = '1.0',
	maintainer = 'Arni Mar Jonsson',
	maintainer_email = 'arnimarj@gmail.com',
	description = 'A Python wrapper for Judy arrays, which provide fast and space-efficient integer mappings and integer sets, along with ranged ordered iterations',
	url = 'http://code.google.com/p/py-judy/',
	download_url = 'http://py-judy.googlecode.com/files/judy.tar.gz',

	classifiers = [
		'Development Status :: 5 - Production/Stable',
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
