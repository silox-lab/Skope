cmake_minimum_required(VERSION 3.23)

project(skope VERSION 0.1
    DESCRIPTION "skope: A local home for your ideas, documentation, and memories."
    LANGUAGES C
)

find_package(ICU REQUIRED COMPONENTS uc i18n)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

add_executable(skope main.c)

target_include_directories(skope PUBLIC
    include/
    include/database/
)

file(GLOB_RECURSE SKOPE_SOURCES CONFIGURE_DEPENDS "src/*.c")

add_library(skope_src STATIC ${SKOPE_SOURCES})

target_link_libraries(skope PRIVATE
    sqlite3
    skope_src
)

target_link_libraries(skope_src PUBLIC
    ICU::uc
    ICU::i18n
    PkgConfig::CJSON
)
