cmake_minimum_required(VERSION 3.23)

project(wikit VERSION 0.1
    DESCRIPTION "wikit: A local home for your ideas, documentation, and memories."
    LANGUAGES C
)

find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

add_executable(wikit main.c)

target_include_directories(wikit PUBLIC
    include/
    include/database/
)

file(GLOB_RECURSE WIKIT_SOURCES CONFIGURE_DEPENDS "src/*.c")

add_library(wikit_src STATIC ${WIKIT_SOURCES})

target_link_libraries(wikit PRIVATE
    sqlite3
    wikit_src
)

target_link_libraries(wikit_src PUBLIC
    ICU::uc
    ICU::i18n
    PkgConfig::CJSON
)