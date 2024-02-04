i18n = import('i18n')

pkg_fmt = '-DGETTEXT_PACKAGE="@0@"'
msg_fmt = '-DLOCALE_DIR="@0@"'
add_project_arguments(
	pkg_fmt.format(meson.project_name()),
	msg_fmt.format(get_option('prefix') / get_option('localedir')),
	language: 'c'
)
i18n.gettext(meson.project_name(),
	preset: 'glib',
	args: '--from-code=UTF-8'
)
