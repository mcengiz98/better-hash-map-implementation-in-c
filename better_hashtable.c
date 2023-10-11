#include <stdio.h>
#include <string.h>
#include "better_hashtable.h"

typedef struct _entry
{
    char *key;
    void *obj;
    struct _entry *next;
} entry;

typedef struct _hash_table
{
    uint32_t size; // number of elements
    hashfunction *hash;
    entry **elemnets;
} hash_table;

static size_t hash_table_index(hash_table *ht, const char *key)
{
    return ht->hash(key, strlen(key)) % ht->size;
}

hash_table *hash_table_create(uint32_t size, hashfunction *hf)
{
    hash_table *ht = malloc(sizeof(hash_table));
    ht->size = size;
    ht->hash = hf;
    ht->elemnets = calloc(sizeof(entry *), ht->size); // It will be zero initialized
    return ht;
}

void hash_table_destroy(hash_table *ht)
{
    free(ht->elemnets);
    free(ht);
}

void hash_table_print(hash_table *ht)
{
    printf("Hash Table:\n");
    for (uint32_t i = 0; i < ht->size; ++i)
    {
        if (ht->elemnets[i] == NULL)
        {
            printf("\t%i\t---\n", i);
        }
        else
        {
            printf("\t%i", i);
            entry *tmp = ht->elemnets[i];
            while (tmp != NULL)
            {
                printf("\t\"%s\"(%p) - ", tmp->key, tmp->obj);
                tmp = tmp->next;
            }
            printf("\n");
        }
    }
}

void hash_table_print_wo_null(hash_table *ht)
{
    printf("Hash Table:\n");
    for (uint32_t i = 0; i < ht->size; ++i)
    {
        if (ht->elemnets[i] != NULL)
            printf("\t%i", i);
        entry *tmp = ht->elemnets[i];
        while (tmp != NULL)
        {
            printf("\t\"%s\"(%p) - ", tmp->key, tmp->obj);
            tmp = tmp->next;
        }
        printf("\n");
    }
}

bool hash_table_insert(hash_table *ht, const char *key, void *obj)
{
    if (key == NULL || obj == NULL || ht == NULL)
    {
        return false;
    }
    size_t index = hash_table_index(ht, key);
    if (hash_table_lookup(ht, key) != NULL)
    {
        return false;
    }

    // Create new entry
    entry *e = malloc(sizeof(entry));
    e->obj = obj;
    e->key = malloc(strlen(key) + 1);
    strcpy(e->key, key);

    // Insert entry
    e->next = ht->elemnets[index];
    ht->elemnets[index] = e;
    return true;
}

void *hash_table_lookup(hash_table *ht, const char *key)
{
    if (key == NULL || ht == NULL)
    {
        return false;
    }
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elemnets[index];
    while (tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        tmp = tmp->next;
    }
    if (tmp == NULL)
    {
        return NULL;
    }
    return tmp->obj;
}

void *hash_table_delete(hash_table *ht, const char *key)
{
    if (key == NULL || ht == NULL)
    {
        return false;
    }
    size_t index = hash_table_index(ht, key);

    entry *tmp = ht->elemnets[index];
    entry *prev = NULL;
    while (tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL)
    {
        return NULL;
    }
    if (prev == NULL)
    {
        // deleting the head of the list
        ht->elemnets[index] = tmp->next;
    }
    else
    {
        // deleting from somewhere else
        prev->next = tmp->next;
    }
    void *result = tmp->obj;
    free(tmp);
    return result;
}
