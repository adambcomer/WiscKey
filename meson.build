project('WiscKey', 'c', default_options : ['c_std=c17'])

### Library ###
include = include_directories('include')

lib = library('wisckey', ['src/wisckey.c', 'src/memtable.c', 'src/wal.c', 'src/sstable.c', 'src/value_log.c'], include_directories : include, version : '1.0.0', soversion : '1')

### Tests ###
memtable_test = executable('memtable_test', 'tests/memtable_test.c', link_with : lib, include_directories : include)
test('memtable_test', memtable_test)

wal_test = executable('wal_test', 'tests/wal_test.c', link_with : lib, include_directories : include)
test('wal_test', wal_test)

sstable_test = executable('sstable_test', 'tests/sstable_test.c', link_with : lib, include_directories : include)
test('sstable_test', sstable_test)

value_log_test = executable('value_log_test', 'tests/value_log_test.c', link_with : lib, include_directories : include)
test('value_log_test', value_log_test)
