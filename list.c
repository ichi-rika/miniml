#include "common.h"

ListEntry *list_new(ListValue value) {
    ListEntry *list = NULL;
    return list_append(&list, value);
}

ListEntry *list_last_entry(ListEntry *list) {
    if (list == NULL) return NULL;

    ListEntry *next;

    do {
        next = list_next(list);
        if (next == NULL) break;

        list = next;
    } while (list != NULL);

    return list;
}

ListValue list_last_data(ListEntry *list) {
    return list_data(list_last_entry(list));
}

void list_foreach(ListEntry *list, ListConsumer func) {
    ListIterator it;
    list_iterate(&list, &it);
    
    while (list_iter_has_more(&it)) {
        func(list_iter_next(&it));
    }
}

void list_print(ListEntry *list, ListPrintFunc func, const char *pref, const char *delim, const char *suff) {
    ListIterator it;
    list_iterate(&list, &it);

    if (pref != NULL) printf("%s", pref);

    while (list_iter_has_more(&it)) {
        func(list_iter_next(&it));

        if (list_iter_has_more(&it)) {
            printf("%s", delim);
        }
    }

    if (suff != NULL) printf("%s", suff);
}

void *nmalloc(int name) {
    int *p = malloc(sizeof(int));
    if (p == NULL) return NULL;
    
    *p = name;
    return p;
}
