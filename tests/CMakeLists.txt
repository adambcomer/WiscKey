
add_executable(MemTableTests test_memtable.c)
target_link_libraries(MemTableTests wisckey)
add_test(NAME MemTableTests COMMAND MemTableTests)

add_executable(ValueLogTests test_value_log.c)
target_link_libraries(ValueLogTests wisckey)
add_test(NAME ValueLogTests COMMAND ValueLogTests)

add_executable(WALTests test_wal.c)
target_link_libraries(WALTests wisckey)
add_test(NAME WALTests COMMAND WALTests)