project('skope', 'c')

deps = [dependency('libcjson'), dependency('sqlite3'), dependency('icu-uc'), dependency('icu-i18n'), dependency('criterion')]

subdir('src')
test_sources = sources + []
sources += files(
  'main.c'
)

incdir = include_directories('include')

executable('skope', sources,
   dependencies: deps,
   include_directories: incdir
 )

subdir('tests/src')
test_incdir = include_directories('tests/include')

test('scope_unit', executable('scope_unit_bin', scope_unit_sources,
    dependencies: deps,
    include_directories: test_incdir))

test('scopefile_unit', executable('scopefile_unit_bin', scopefile_unit_sources,
    dependencies: deps,
    include_directories: test_incdir))

test('entity_unit', executable('entity_unit_bin', entity_unit_sources,
    dependencies: deps,
    include_directories: test_incdir))

test('relation_unit', executable('relation_unit_bin', relation_unit_sources,
    dependencies: deps,
    include_directories: test_incdir))
