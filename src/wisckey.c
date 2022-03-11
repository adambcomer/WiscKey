#include <stdlib.h>
#include "include/wisckey.h"
#include "include/memtable.h"

struct WiscKeyDB
{
    char *dir;
    struct MemTable *memtable;
};

struct WiscKeyDB *WiscKeyDB_new(char *dir)
{
    struct WiscKeyDB *db = malloc(sizeof(struct WiscKeyDB));
    db->dir = dir;
    db->memtable = MemTable_new();

    return db;
}

size_t WiscKeyDB_get(struct WiscKeyDB *db, char *ptr, char *key, size_t key_length)
{
    struct MemTableRecord *m_record = MemTable_get(db->memtable, key, key_length);
    if (m_record != NULL)
    {
        ptr = m_record->key;
        return m_record->key_len;
    }

    return 0;
}

int WiscKeyDB_set(struct WiscKeyDB *db, char *key, char *value, size_t key_length, size_t value_length)
{
    return 0;
}

int WiscKeyDB_delete(struct WiscKeyDB *db, char *key, size_t key_length)
{
    return 0;
}