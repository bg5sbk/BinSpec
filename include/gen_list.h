#ifndef _BSS_LIST_H_
#define _BSS_LIST_H_

#define LIST_DEF(LIST, T)             \
typedef struct {                      \
  T   *items;                         \
  int  cap;                           \
  int  len;                           \
} LIST;                               \
                                      \
LIST *LIST##_new(int cap);            \
                                      \
void LIST##_free(LIST *list);         \
                                      \
int LIST##_add(LIST *list, T item);   \
                                      \
void LIST##_del(LIST *list, int pos); \
                                      \
T LIST##_get(LIST *list, int pos);    \


#define LIST_IMP_(LIST, T, ALLOC, REALLOC, FREE, ERROR, FREE_ITEMS, TFREE) \
LIST *LIST##_new(int cap) {                                                \
    LIST *list = (LIST *)(ALLOC)(sizeof(LIST));                            \
    list->items = (T *)(ALLOC)(sizeof(T *) * cap);                         \
    list->cap   = cap;                                                     \
    list->len   = 0;                                                       \
    return list;                                                           \
}                                                                          \
                                                                           \
void LIST##_free(LIST *list) {                                             \
    FREE_ITEMS                                                             \
    (FREE)(list->items);                                                   \
    (FREE)(list);                                                          \
}                                                                          \
                                                                           \
int LIST##_add(LIST *list, T item) {                                       \
    if (list->len == list->cap) {                                          \
        int new_cap = list->cap * 2;                                       \
        list->items = (T *)(REALLOC)(list->items, sizeof(T) * new_cap);    \
        if (list->items == NULL) {                                         \
            (ERROR)(#LIST" realloc failed!\n");                            \
            exit(1);                                                       \
        }                                                                  \
        list->cap = new_cap;                                               \
    }                                                                      \
    int pos = list->len;                                                   \
    list->items[pos] = item;                                               \
    list->len += 1;                                                        \
    return pos;                                                            \
}                                                                          \
                                                                           \
void LIST##_del(LIST *list, int pos) {                                     \
    if (pos >= list->len)                                                  \
        return;                                                            \
                                                                           \
    TFREE(list->items[pos]);                                               \
                                                                           \
    int i;                                                                 \
                                                                           \
    for (i = pos + 1; i < list->len; i ++) {                               \
        list->items[i - 1] = list->items[i];                               \
    }                                                                      \
                                                                           \
    list->len -= 1;                                                        \
}                                                                          \
                                                                           \
T LIST##_get(LIST *list, int pos) {                                        \
    if (pos >= list->len)                                                  \
        return 0;                                                          \
                                                                           \
    return list->items[pos];                                               \
}                                                                          \


#define LIST_FREE_ITEMS(TFREE)                    \
    int i = 0;                                    \
    for(i = 0; i < list->len; i ++) {             \
        TFREE(list->items[i]);                    \
    }                                             \


#define LIST_IMP(LIST, T)                         \
    LIST_IMP_(                                    \
        LIST, T, malloc, realloc, free, printf, , \
    )                                             \


#define LIST_IMP_P(LIST, T, TFREE)                \
    LIST_IMP_(                                    \
        LIST, T, malloc, realloc, free, printf,   \
        LIST_FREE_ITEMS(TFREE), TFREE             \
    )                                             \

#endif
