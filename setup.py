from setuptools import Extension, setup


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
	version='1.0.19',
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
		'Programming Language :: Python :: 3.13',
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
