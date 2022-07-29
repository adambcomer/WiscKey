cmake_minimum_required(VERSION 3.10)

project(WiscKey VERSION 1.0.0 LANGUAGES C)

option(ASAN "Build with the Address Sanitizer" OFF)

add_library(wisckey STATIC src/wisckey.c src/memtable.c src/value_log.c src/wal.c src/sstable.c)
if(ASAN)
    target_compile_options(wisckey PUBLIC -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(wisckey PUBLIC -fsanitize=address -fno-omit-frame-pointer -g)
endif(ASAN)
set_target_properties(wisckey PROPERTIES PUBLIC_HEADER src/include/wisckey.h)
set_target_properties(wisckey PROPERTIES VERSION ${PROJECT_VERSION})

include(GNUInstallDirs)
install(TARGETS wisckey LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

enable_testing()

add_subdirectory(tests)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET docs/doxygen-awesome.css)
    if(EXISTS "${CMAKE_SOURCE_DIR}/docs/header.html")
        set(DOXYGEN_HTML_HEADER docs/header.html)
    endif()
    set(DOXYGEN_EXCLUDE README.md)
    set(DOXYGEN_PROJECT_NAME "WiscKey Database")
    set(DOXYGEN_PROJECT_BRIEF "Simple LSM-Tree Key-Value Database")

    doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR})
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
