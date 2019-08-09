#ifndef HASHTABLE_H__
#define HASHTABLE_H__

#include <stdio.h>
#include <stdlib.h>
#endif

struct node
{
    int key;
    int val;
    struct node *next;
};

struct table
{
    int size;
    struct node **list;
};

static struct table *makeTable(int size)
{
    struct table *t = (struct table *)malloc(sizeof(struct table));
    t->size = size;
    t->list = (struct node **)malloc(sizeof(struct node *) * size);

    int i;
    for (i = 0; i < size; i++)
    {
        t->list[i] = NULL;
    }

    return t;
}

static int hashCode(struct table *t, int key)
{
    if (key < 0)
        return -(key % t->size);
    return key % t->size;
}

static void insert(struct table *t, int key, int val)
{
    int pos = hashCode(t, key);
    struct node *list = t->list[pos];

    struct node *cursor = list;
    while (cursor)
    {
        //If find the key replace the value
        if (cursor->key == key)
        {
            cursor->val = val;
            return;
        }
        // search the list until the end
        cursor = cursor->next;
    }
    //Key not found. Insert at the begining of the list collition list
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}

static int lookup(struct table *t, int key)
{
    int pos = hashCode(t, key);
    struct node *list = t->list[pos];

    struct node *cursor = list;
    while (cursor)
    {
        if (cursor->key == key)
            return cursor->val;
        cursor = cursor->next;
    }
    return -1;
}

static void printTable(struct table *t)
{
    int i;
    for (i = 0; i < t->size; i++)
    {
        char buffer[256];
        sprintf(buffer, "Pos: %d ", i);
        struct node *list = t->list[i];
        struct node *cursor = list;
        while (cursor)
        {
            sprintf(buffer, "%s [%d : %d]", buffer, cursor->key, cursor->val);
            cursor = cursor->next;
        }
        printf("%s\n", buffer);
    }
}