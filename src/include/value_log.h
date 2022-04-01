#include <stdio.h>

#ifndef VALUE_LOG_H
#define VALUE_LOG_H

struct ValueLog {
    FILE *file;
    size_t head;
    size_t tail;
};

struct ValueLog *ValueLog_new(const char *filename, size_t head, size_t tail);

int ValueLog_append(struct ValueLog *log, size_t *pos, const char *key, size_t key_len, const char *value,
                    size_t value_len);

int ValueLog_get(const struct ValueLog *log, char **value, size_t *value_len, size_t value_loc);

int ValueLog_sync(const struct ValueLog *log);

void ValueLog_free(struct ValueLog *log);

#endif /* VALUE_LOG_H */