#ifndef LISTS_H
#define LISTS_H

#include <stdint.h>

typedef struct ListItem
{
    void* item;
    struct ListItem* next;
    struct ListItem* previous;
}ListItem;

typedef struct Lists
{
    uint32_t count;
    ListItem* first;
    ListItem* current;
    ListItem* last;
}List_I;

List_I* NewList(void);
void Add(List_I*, void*);
void Remove(List_I*);
void Next(List_I*);
void Previous(List_I*);
void First(List_I*);
void Last(List_I*);
void* This(List_I*);

#endif