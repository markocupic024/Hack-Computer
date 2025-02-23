#include "symbol_table.h"

static uint32_t hash_string(char *key, size_t length);

static void symbol_table_adjustCapacity(symbol_table_ts *table, size_t capacity);

static entry_ts *symbol_table_findEntry(entry_ts *entries, size_t capacity, char *key, uint32_t hash);

void symbol_table_init(symbol_table_ts *table)
{
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void symbol_table_free(symbol_table_ts *table)
{
    if (table->entries)
    {
        for (size_t i = 0; i < table->count; i++)
        {
            free(table->entries[i].key);
        }
        free(table->entries);
    }
}

bool symbol_table_set(symbol_table_ts *table, char *key, size_t value)
{
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD)
    {
        size_t new_capacity = table->capacity ? table->capacity * 2 : 2;
        symbol_table_adjustCapacity(table, new_capacity);
    }
    uint32_t hash = hash_string(key, strlen(key));
    entry_ts *entry = symbol_table_findEntry(table->entries, table->capacity, key, hash);
    bool is_new_key = entry->key == NULL;
    if (is_new_key)
        table->count++;
    entry->key = strdup(key);
    entry->value = value;
    return is_new_key;
}

bool symbol_table_get(symbol_table_ts *table, char *key, size_t *value)
{
    if (table->count == 0)
        return false;

    uint32_t hash = hash_string(key, strlen(key));
    entry_ts *entry = symbol_table_findEntry(table->entries, table->capacity, key, hash);
    if (entry->key == NULL)
        return false;

    *value = entry->value;
    return true;
}

// FNV-1a hashing function
static uint32_t hash_string(char *key, size_t length)
{
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < length; i++)
    {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

static void symbol_table_adjustCapacity(symbol_table_ts *table, size_t capacity)
{
    entry_ts *entries = malloc(sizeof(entry_ts) * capacity);
    if (entries)
    {
        for (size_t i = 0; i < capacity; i++)
        {
            entries[i].key = NULL;
            entries[i].value = 0;
        }

        for (size_t i = 0; i < table->capacity; i++)
        {
            entry_ts *entry = &table->entries[i];
            if (entry->key == NULL)
                continue;

            uint32_t hash = hash_string(entry->key, strlen(entry->key));
            entry_ts *dest = symbol_table_findEntry(entries, capacity, entry->key, hash);
            dest->key = entry->key;
            dest->value = entry->value;
        }

        free(table->entries);
        table->entries = entries;
        table->capacity = capacity;
    }
}

static entry_ts *symbol_table_findEntry(entry_ts *entries, size_t capacity, char *key, uint32_t hash)
{
    uint32_t index = hash % capacity;

    for (;;)
    {
        entry_ts *entry = &entries[index];
        if (entry->key == NULL || !strcmp(entry->key, key))
        {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}