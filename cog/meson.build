# Copyright 2018 Endless Mobile, Inc.

version = configuration_data()
version.set('COG_MAJOR_VERSION', version_components[0])
version.set('COG_MINOR_VERSION', version_components[1])
version.set('COG_MICRO_VERSION', version_components[2])
version_h = configure_file(configuration: version,
    input: 'cog-version.h.in', output: 'cog-version.h')

installed_headers = [
    'cog.h',
    version_h,
    'cog-init.h',
    'cog-macros.h',
]
private_headers = [
]
sources = [
    'cog-init.cpp',
]

include = include_directories('..')

main_library = library('@0@-@1@'.format(meson.project_name(), api_version),
    sources, installed_headers, private_headers,
    cpp_args: ['-DG_LOG_DOMAIN="@0@"'.format(namespace_name),
        '-DCOMPILING_LIBCOG'],
    dependencies: [glib, gobject, aws_core, cognito_idp],
    include_directories: include, install: true,
    link_depends: 'lib@0@.map'.format(meson.project_name()),
    soversion: api_version, version: libtool_version)

introspection_sources = [sources, installed_headers]

gnome.generate_gir(main_library, extra_args: ['--warn-all', '--warn-error'],
    identifier_prefix: 'Cog', include_directories: include,
    includes: ['GLib-2.0', 'GObject-2.0'],
    install: true, namespace: namespace_name, nsversion: api_version,
    sources: introspection_sources, symbol_prefix: 'cog')

install_headers(installed_headers,
    subdir: join_paths(api_name, meson.project_name()))
