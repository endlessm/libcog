# Copyright 2018 Endless Mobile, Inc.

project('cog', 'cpp', 'c', version: '0.0.0', license: 'LGPL2.1+',
    meson_version: '>= 0.42.0',
    default_options: ['c_std=c99', 'warning_level=3'])

gnome = import('gnome')
pkg = import('pkgconfig')

version_components = meson.project_version().split('.')
api_version = version_components[0]
api_name = '@0@-@1@'.format(meson.project_name(), api_version)
libtool_version = '0.0.0'
namespace_name = 'Cog'

# Build options

jasmine_report_argument = ''
junit_reports_dir = get_option('jasmine_junit_reports_dir')
if junit_reports_dir != ''
    jasmine_report_argument = '--junit=@0@/${log/.log/.js.xml}'.format(junit_reports_dir)
endif

# Dependencies

glib = dependency('glib-2.0')
gobject = dependency('gobject-2.0')
gio = dependency('gio-2.0')
aws_core = dependency('aws-cpp-sdk-core', version: '>=0.12')
cognito_idp = dependency('aws-cpp-sdk-cognito-idp', version: '>=0.12')

subdir('cog')

# Dependency object for gtk-doc and tests to use
main_library_dependency = declare_dependency(dependencies: [glib, gobject, gio],
    include_directories: include, link_with: main_library)

# Data files

install_data('overrides/@0@.js'.format(namespace_name),
    install_dir: join_paths(get_option('datadir'), 'gjs-1.0', 'overrides'))

# Generated Files

config = configuration_data()
config.set_quoted('COG_VERSION', meson.project_version())
configure_file(configuration: config, output: 'config.h')

requires = ['glib-2.0', 'gobject-2.0', 'gio-2.0']
requires_private = ['aws-cpp-sdk-core', 'aws-cpp-sdk-cognito-idp']
pkg.generate(filebase: api_name, libraries: [main_library],
    description: 'GObject wrapper for AWS Cognito SDK',
    name: meson.project_name(), subdirs: api_name, requires: requires,
    requires_private: requires_private,
    url: 'http://endlessm.github.io/libcog',
    version: meson.project_version())

subdir('test')

if get_option('documentation')
    subdir('docs')
endif

# Summary

message('\n'.join([
    '@0@ @1@ (@2@)'.format(meson.project_name(), meson.project_version(),
        api_name),
    '-------------------',
    'Options:',
    '     Documentation: @0@'.format(get_option('documentation')),
    '  Test reports dir: @0@'.format(get_option('jasmine_junit_reports_dir')),
    '',
    'Directories:',
    '    Install prefix: @0@'.format(get_option('prefix')),
    ''
]))
