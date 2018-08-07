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
    'cog-client.h',
    'cog-init.h',
    'cog-macros.h',
    'cog-utils.h'
]
private_headers = [
    'cog-boxed-private.h',
    'cog-utils-private.h',
]
sources = [
    'cog-client.cpp',
    'cog-init.cpp',
    'cog-utils.cpp',
]

generated_boxed_types = [
    'cog-analytics-metadata',
    'cog-authentication-result',
    'cog-code-delivery-details',
    'cog-mfa-option',
    'cog-new-device-metadata',
    'cog-user-context-data',
]
genboxed = find_program('genboxed.py')
generated_boxed_headers = []
foreach base : generated_boxed_types
    infile = base + '.def.yaml'
    c_out = base + '.cpp'
    h_out = base + '.h'
    target = custom_target(base, build_by_default: true,
        command: [genboxed, '@INPUT@', '@OUTDIR@'],
        input: infile, output: [c_out, h_out],
        install: true,
        install_dir: [
            false,
            join_paths('include', api_name, meson.project_name())
        ])
    sources += target[0]
    generated_boxed_headers += target[1]
endforeach

include = include_directories('..')

enum_sources = gnome.mkenums_simple('cog-enums',
    decorator: 'COG_AVAILABLE_IN_ALL',
    header_prefix: '#include "cog/cog-macros.h"', install_header: true,
    install_dir: join_paths(get_option('includedir'), api_name),
    sources: installed_headers)

main_library = library('@0@-@1@'.format(meson.project_name(), api_version),
    enum_sources, sources, installed_headers, private_headers,
    generated_boxed_headers,
    cpp_args: ['-DG_LOG_DOMAIN="@0@"'.format(namespace_name),
        '-DCOMPILING_LIBCOG'],
    dependencies: [glib, gobject, gio, aws_core, cognito_idp],
    include_directories: include, install: true,
    link_depends: 'lib@0@.map'.format(meson.project_name()),
    soversion: api_version, version: libtool_version)

introspection_sources = [
    sources,
    enum_sources,
    installed_headers,
    generated_boxed_headers,
]

gnome.generate_gir(main_library, extra_args: ['--warn-all', '--warn-error'],
    identifier_prefix: 'Cog', include_directories: include,
    includes: ['GLib-2.0', 'GObject-2.0', 'Gio-2.0'],
    install: true, namespace: namespace_name, nsversion: api_version,
    sources: introspection_sources, symbol_prefix: 'cog')

install_headers(installed_headers,
    subdir: join_paths(api_name, meson.project_name()))
