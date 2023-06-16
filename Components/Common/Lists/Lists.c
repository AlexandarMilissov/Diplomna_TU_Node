#include "Lists.h"
#include <stdlib.h>

List_I* NewList(void)
{
    List_I* list = (List_I*)malloc(sizeof(List_I));

    list->count = 0;
    list->current = NULL;
    list->first = NULL;
    list->last = NULL;

    return list;
}

void Add(List_I* list, void* item)
{
    ListItem* listItem = (ListItem*)malloc(sizeof(ListItem));
    listItem->item = item;

    if(0 == list->count)
    {
        list->current = listItem;
        list->first = listItem;
        list->last = listItem;
    }
    else
    {
        listItem->previous              = list->current;
        listItem->next                  = list->current->next;
        list->current->next              = listItem;
        list->current->next->previous    = listItem;
    }

    list->count++;
}

void Remove(List_I* list)
{
    if(0 == list->count)
    {
        return;
    }
    list->count--;

    if(0 == list->count)
    {
        free((void*)list->first);
        list->current = NULL;
        list->first = NULL;
        list->last = NULL;
    }
    else
    {
        list->current->previous->next = list->current->next;
        list->current->next->previous = list->current->previous;

        if(list->first == list->current)
        {
            list->first = list->current->next;
        }
        if(list->last == list->current)
        {
            list->last = list->current->previous;
        }
        ListItem* listItem = list->current;
        list->current = list->current->next;
        free(listItem);
    }
}

void Next(List_I* list)
{
    list->current = list->current->next;
}

void Previous(List_I* list)
{
    list->current = list->current->previous;
}

void First(List_I* list)
{
    list->current = list->first;
}

void Last(List_I* list)
{
    list->current = list->last;
}

void* This(List_I* list)
{
    return list->current->item;
}