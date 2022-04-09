cmake_minimum_required(VERSION 3.10)

project(WiscKey VERSION 1.0.0 LANGUAGES C)

add_library(wisckey STATIC src/wisckey.c src/memtable.c src/value_log.c src/wal.c src/sstable.c)
set_target_properties(wisckey PROPERTIES PUBLIC_HEADER src/include/wisckey.h)
set_target_properties(wisckey PROPERTIES VERSION ${PROJECT_VERSION})

include(GNUInstallDirs)
install(TARGETS wisckey LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

enable_testing()

add_subdirectory(tests)
