
srcs = [
	'lxtermc.c',
	'cfg.c',
	'app.c',
	'win.c',
	'tab.c',
	'vte.c'
]

exe = executable(
	meson.project_name(),
	srcs,
	lxtcresources,
	dependencies: gtkdeps,
	install: true)
