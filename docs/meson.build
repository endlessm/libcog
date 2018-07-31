# Copyright 2018 Endless Mobile, Inc.

version_conf = configuration_data()
version_conf.set('VERSION', meson.project_version())

version_xml = configure_file(input: 'version.xml.in', output: 'version.xml',
  configuration: version_conf)

ignore_hfiles = [
]
main_xml = '@0@-docs.xml'.format(meson.project_name())

gnome.gtkdoc(meson.project_name(), dependencies: main_library_dependency,
    ignore_headers: ignore_hfiles, install: true, main_xml: main_xml,
    mkdb_args: ['--xml-mode', '--output-format=xml', '--source-suffixes=cpp,h'],
    scan_args: ['--rebuild-types'],
    src_dir: [
        join_paths(meson.source_root(), meson.project_name()),
        join_paths(meson.build_root(), meson.project_name()),
    ])

# Tests

# See https://github.com/mesonbuild/meson/issues/1844
# gtkdoc_check = find_program('gtkdoc-check')
# gtkdoc_tests_env = environment()
# gtkdoc_tests_env.set('DOC_MODULE', meson.project_name())
# gtkdoc_tests_env.set('DOC_MAIN_SGML_FILE', main_xml)
# gtkdoc_tests_env.set('SRCDIR', meson.current_source_dir())
# gtkdoc_tests_env.set('BUILDDIR', meson.current_build_dir())
# test('gtk-doc sanity check', gtkdoc_check, env: gtkdoc_tests_env)
