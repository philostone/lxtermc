
project('lxtermc', 'c',
	version: '0.0.1',			# still just gettings things to work
	meson_version: '>= 1.2.0',		# same as for glib-2.0
	default_options: [
		'buildtype=release',
		'warning_level=3',
	]
)

gtkdeps = [dependency('gtk4', version: '>=4.0')]

subdir('po')

# this is where the main program is, execute last
subdir('src')
