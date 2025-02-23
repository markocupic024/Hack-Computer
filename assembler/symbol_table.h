#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define TABLE_MAX_LOAD 0.75

typedef struct
{
    char *key;
    size_t value;
} entry_ts;

typedef struct
{
    size_t count;
    size_t capacity;
    entry_ts *entries;
} symbol_table_ts;

void symbol_table_init(symbol_table_ts *table);

void symbol_table_free(symbol_table_ts *table);

bool symbol_table_set(symbol_table_ts *table, char *key, size_t value);

bool symbol_table_get(symbol_table_ts *table, char *key, size_t *value);

#endif