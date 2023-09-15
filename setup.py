import os
import subprocess
import sys

from setuptools import Extension, setup


def build_judy():
	print('INFO: building judy static library...')

	CC = os.environ.get('CC', 'cc')

	is_clang = False
	is_gcc_46 = False

	# test if CC is clang/gcc-4.6
	exitcode, output = subprocess.getstatusoutput('%s -v' % (CC,))

	if exitcode != 0:
		sys.exit(output)

	if 'clang' in output:
		print('INFO: compiler is Clang')
		is_clang = True
	elif 'gcc version 4.6' in output:
		print('INFO: compiler is GCC 4.6')
		is_gcc_46 = True

	# adding last two flags because of compiler and/or code bugs
	# see http://sourceforge.net/p/judy/mailman/message/32417284/
	assert(sys.maxsize in (2**63 - 1, 2**31 - 1))

	if is_clang or is_gcc_46:
		if sys.maxsize == 2**63 - 1:
			CFLAGS = '-DJU_64BIT -O0 -fPIC -fno-strict-aliasing'
		else:
			CFLAGS = '           -O0 -fPIC -fno-strict-aliasing'
	else:
		if sys.maxsize == 2**63 - 1:
			CFLAGS = '-DJU_64BIT -O0 -fPIC -fno-strict-aliasing -fno-aggressive-loop-optimizations'
		else:
			CFLAGS = '           -O0 -fPIC -fno-strict-aliasing -fno-aggressive-loop-optimizations'

	exitcode, output = subprocess.getstatusoutput('(cd judy-1.0.5/src; CC=\'%s\' COPT=\'%s\' sh ./sh_build)' % (CC, CFLAGS))

	if exitcode != 0:
		sys.exit(output)

	print(output)


if not os.path.isfile('./judy-1.0.5/src/libJudy.a'):
	build_judy()

extra_link_args = ['-L./judy-1.0.5/src', '-Bstatic', '-lJudy', '-Bdynamic', '-lm']
extra_compile_args = [
	'-I./judy-1.0.5/src',
	'-I./include',
	'-I/usr/local/include',
	'-Wall',
	'-Wno-strict-prototypes',
	'-g',
	'-D_GNU_SOURCE',
	'-O2',
	'-DNDEBUG',
#	'-fno-omit-frame-pointer',
#	'-pg',
#	'-fno-inline-functions',
#	'-fno-inline-functions-called-once',
#	'-fno-optimize-sibling-calls',
#	'-fno-inline'
]

setup(
	name='judy',
	version='1.0.18',
	maintainer='Arni Mar Jonsson',
	maintainer_email='arnimarj@gmail.com',
	description='A Python wrapper for Judy arrays, which provide fast and space-efficient integer mappings and integer sets, along with ranged ordered iterations',
	url='https://github.com/arnimarj/py-judy/',

	classifiers=[
		'Development Status :: 5 - Production/Stable',
		'Environment :: Other Environment',
		'Intended Audience :: Developers',
		'License :: OSI Approved :: GNU Library or Lesser General Public License (LGPL)',
		'Operating System :: POSIX',
		'Programming Language :: C',
		'Programming Language :: Python',
		'Programming Language :: Python :: 3',
		'Programming Language :: Python :: 3.8',
		'Programming Language :: Python :: 3.9',
		'Programming Language :: Python :: 3.10',
		'Programming Language :: Python :: 3.11',
		'Programming Language :: Python :: 3.12',
		'Topic :: Database',
		'Topic :: Software Development :: Libraries'
	],

	packages=['judy'],
	package_dir={'judy': 'src'},
	package_data={
		'judy': ['py.typed', '*.pyi'],
	},
	include_package_data=True,
	ext_modules=[
		Extension(
			'_judy',
			include_dirs=['./judy-1.0.5/src', './src-c'],
			library_dirs=['./judy-1.0.5/src'],

			sources=[
				'src-c/judy.c',
				'src-c/judy_int_object_map.c',
				'src-c/judy_int_set.c',
				'src-c/utils.c'
			],

			extra_compile_args=extra_compile_args,
			extra_link_args=extra_link_args
		)
	]
)
