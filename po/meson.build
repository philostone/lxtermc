i18n = import('i18n')

arg_fmt = '-DGETTEXT_PACKAGE="@0@"'
add_project_arguments(arg_fmt.format(meson.project_name()), language: 'c')
i18n.gettext(meson.project_name())
