#ifndef WISCKEY_H
#define WISCKEY_H

struct WiscKeyDB;

struct WiscKeyDB *WiscKeyDB_new(char *dir);

size_t WiscKeyDB_get(struct WiscKeyDB *db, char *ptr, char *key, size_t key_length);

int WiscKeyDB_set(struct WiscKeyDB *db, char *key, char *value, size_t key_length, size_t value_length);

int WiscKeyDB_delete(struct WiscKeyDB *db, char *key, size_t key_length);

#endif /* WISCKEY_H */
