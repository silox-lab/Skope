project('skope', 'c')

deps = [dependency('libcjson'), dependency('sqlite3'), dependency('icu-uc'), dependency('icu-i18n')]

subdir('src')
sources += files(
  'main.c'
)

incdir = include_directories('include')

executable('skope', sources,
   dependencies: deps,
   include_directories: incdir
 )
