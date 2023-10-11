#include <stdio.h>
#include <string.h>
#include "better_hashtable.h"

typedef struct _entry
{
    char *key;
    void *obj;
    struct _entry *next;
} entry;

struct _hash_table
{
    uint32_t size; // number of elements
    hashfunction *hash;
    cleanupfunction *cleanup;
    entry **elemnets;
};

/**
 * @brief index of the key in the hash table
 * 
 * @param ht : hash table
 * @param key : key
 * @return size_t : index of the key
 */
static size_t hash_table_index(hash_table *ht, const char *key)
{
    return ht->hash(key, strlen(key)) % ht->size;
}

/**
 * @brief Hash table constructor
 * 
 * @param size : size of the hash table
 * @param hf : hash function
 * @param cf : cleanup function. If this value is NULL, then the default free function will be used.
 * @return hash_table* : pointer to the hash table
 */
hash_table *hash_table_create(uint32_t size, hashfunction *hf, cleanupfunction *cf)
{
    hash_table *ht = malloc(sizeof(hash_table));
    ht->size = size;
    ht->hash = hf;
    if (cf)
    {
        ht->cleanup = cf;
    }
    else
    {
        ht->cleanup = free;
    }
    ht->elemnets = calloc(sizeof(entry *), ht->size); // It will be zero initialized
    return ht;
}

/**
 * @brief Hash table destructor
 * 
 * @param ht : hash table
 */
void hash_table_destroy(hash_table *ht)
{
    for (uint32_t i = 0; i < ht->size; ++i)
    {
        while (ht->elemnets[i])
        {
            entry *tmp = ht->elemnets[i];
            ht->elemnets[i] = ht->elemnets[i]->next;
            free(tmp->key);
            ht->cleanup(tmp->obj);
            free(tmp);
        }
    }

    free(ht->elemnets);
    free(ht);
}

/**
 * @brief Print function for the hash table 
 * 
 * @param ht : hash table
 */
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

/**
 * @brief Print function for the hash table without NULL entries
 * 
 * @param ht : hash table
 */
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

/**
 * @brief Insert function for the hash table
 * 
 * @param ht : hash table
 * @param key : key
 * @param obj : object
 * @return true : if the insertion was successful
 * @return false : if the insertion was unsuccessful
 */
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
    e->key = strdup(key);

    // e->key = malloc(strlen(key) + 1);
    // strcpy(e->key, key);

    // Insert entry
    e->next = ht->elemnets[index];
    ht->elemnets[index] = e;
    return true;
}

/**
 * @brief Lookup function for the hash table
 * 
 * @param ht : hash table
 * @param key : key for the lookup
 * @return void* : pointer to the object
 */
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

/**
 * @brief Delete function for the hash table
 * 
 * @param ht : hash table
 * @param key : key for the deletion
 * @return void* : pointer to the object
 */
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
