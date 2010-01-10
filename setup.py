import os, sys
from distutils.core import setup, run_setup, Extension

setup(name = 'Judy Python API',
	version = '1.0',
	maintainer = 'Arni Mar Jonsson',
	maintainer_email = 'arnimarj@gmail.com',
	description = 'Judy Python/C API',
	#url = 'http://code.google.com/p/py-pointless/',

	packages = ['judy'],
	package_dir = {'judy': ''},

	ext_modules = [
		Extension('judy',
			include_dirs = ['/usr/include' ],
			library_dirs = ['/usr/lib' ],

			sources = [
				# python stuff
				'judy.c',

				# pypointless
				'judy_int_object_map.c',
				# 'judy_int_set.c',
			],

			#extra_compile_args = ['-pedantic', '-std=c99', '-Wall', '-Wno-strict-prototypes', '-g', '-D_GNU_SOURCE', '-O2', '-DNDEBUG'],
			extra_compile_args = ['-pedantic', '-std=c99', '-Wall', '-Wno-strict-prototypes', '-g', '-D_GNU_SOURCE', '-O0'],
			extra_link_args = ['-Bstatic', '-lJudy', '-Bdynamic']
		)
	]
)
