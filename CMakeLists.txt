cmake_minimum_required(VERSION 3.10)

project(WiscKey VERSION 1.0.0 LANGUAGES C)

add_library(wisckey STATIC src/wisckey.c src/memtable.c src/value_log.c)
set_target_properties(wisckey PROPERTIES PUBLIC_HEADER src/include/wisckey.h)

install(TARGETS wisckey LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)

enable_testing()

add_executable(MemTableTests tests/test_memtable.c)
target_link_libraries(MemTableTests wisckey)
add_test(NAME MemTableTests COMMAND MemTableTests)

add_executable(ValueLogTests tests/test_value_log.c)
target_link_libraries(ValueLogTests wisckey)
add_test(NAME ValueLogTests COMMAND ValueLogTests)
