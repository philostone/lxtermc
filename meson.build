
project('lxtermc', 'c',
	version: '0.0.1',			# still just gettings things to work
	meson_version: '>= 1.2.0',		# same as for glib-2.0
	default_options: [
		'buildtype=release',
		'warning_level=3',
	]
)

ver_str = '-DLXTERMC_VERSION="@0@"'
add_project_arguments(
	ver_str.format(meson.project_version()),
	language: 'c'
)

datadir_str = '-DLXTERMC_DATA_DIR="@0@"'
add_project_arguments(
	datadir_str.format(get_option('prefix')/get_option('datadir')),
	language: 'c'
)

gtkdeps = [
	dependency('gtk4', version: '>=4.0'),
	dependency('vte-2.91-gtk4', version: '>=0.74.2')
]

subdir('po')

# this is where the main program is, execute last
subdir('src')
