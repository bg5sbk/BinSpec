#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bs_ast.h"
#include "bs_parse.h"

LIST_IMP_P(bs_pkg_list, bs_pkg *, bs_pkg_free);
LIST_IMP_P(bs_def_list, bs_def *, bs_def_free);
LIST_IMP_P(bs_col_list, bs_col *, bs_col_free);
LIST_IMP_P(bs_enum_list, bs_enum *, bs_enum_free);
LIST_IMP_P(bs_enum_item_list, bs_enum_item *, bs_enum_item_free);
LIST_IMP_P(bs_type_list, bs_type *, bs_type_free);

bs_doc *bs_doc_new() {
    bs_doc *doc = (bs_doc *)malloc(sizeof(bs_doc));

    doc->root_pkg = bs_pkg_new(doc, NULL, 0, NULL, 0);

    return doc;
}

void bs_doc_free(bs_doc *doc) {
    bs_pkg_free(doc->root_pkg);
    free(doc);
}

bs_pkg *bs_pkg_new(bs_doc *doc, bs_pkg *parent, int id, const char *name, size_t name_len) {
    bs_pkg *pkg = (bs_pkg *)malloc(sizeof(bs_pkg));
     
    pkg->pkgs = bs_pkg_list_new(10);
    pkg->defs = bs_def_list_new(20);
    pkg->enums = bs_enum_list_new(5);
    pkg->types = bs_type_list_new(2);

    if (name != NULL) {
        size_t name2_len = name_len + 1;
        char *name2 = (char *)malloc(name2_len);
        bzero(name2, name2_len);

        pkg->name = strncpy(name2, name, name_len);
        pkg->name_len = name_len;
    } else {
        pkg->name = NULL;
        pkg->name_len = 0;
    }

    pkg->doc  = doc;
    pkg->pkg  = parent;
    pkg->id   = id;

    return pkg;
}

void bs_pkg_free(bs_pkg *pkg) {
    bs_pkg_list_free(pkg->pkgs);
    bs_def_list_free(pkg->defs);
    bs_enum_list_free(pkg->enums);
    bs_type_list_free(pkg->types);

    free(pkg->name);
    free(pkg);
}

bs_def *bs_def_new(bs_pkg *pkg, int id, const char *name, size_t name_len) {
    bs_def *def = (bs_def *)malloc(sizeof(bs_def));
  
    def->cols = bs_col_list_new(20);

    size_t name2_len = name_len + 1;
    char *name2 = (char *)malloc(name_len + 1);
    bzero(name2, name2_len);

    def->name = strncpy(name2, name, name_len);
    def->name_len = name_len;

    def->doc  = pkg->doc;
    def->pkg  = pkg;
    def->id   = id;

    return def;
}

void bs_def_free(bs_def *def) {
    bs_col_list_free(def->cols);

    free(def->name);
    free(def);
}

bs_col *bs_col_new(const char *name, size_t name_len, const char *ref_name, size_t ref_name_len, bs_type_enum type) {
    bs_col *col = (bs_col *)malloc(sizeof(bs_col));

    col->cols = bs_col_list_new(5);

    size_t name2_len = name_len + 1;
    char *name2 = (char *)malloc(name_len + 1);
    bzero(name2, name2_len);

    col->name = strncpy(name2, name, name_len);
    col->name_len = name_len;

    if (ref_name != NULL) {
        size_t ref_name2_len = ref_name_len + 1;
        char *ref_name2 = (char *)malloc(ref_name_len + 1);
        bzero(ref_name2, ref_name2_len);

        col->ref_name = strncpy(ref_name2, ref_name, ref_name_len);
        col->ref_name_len = ref_name_len;
    } else {
        col->ref_name = NULL;
        col->ref_name_len = 0;
    }

    col->type = type;

    return col;
}

void bs_col_free(bs_col *col) {
    bs_col_list_free(col->cols);

    free(col->name);
    free(col->ref_name);
    free(col);
}

bs_enum *bs_enum_new(bs_pkg *pkg, const char *name, size_t name_len, bs_type_enum type) {
    bs_enum *em = (bs_enum *)malloc(sizeof(bs_enum));

    em->items = bs_enum_item_list_new(5);

    size_t name2_len = name_len + 1;
    char *name2 = (char *)malloc(name_len + 1);
    bzero(name2, name2_len);

    em->name = strncpy(name2, name, name_len);
    em->name_len = name_len;

    em->type = type;

    return em;
}

void bs_enum_free(bs_enum *em) {
    bs_enum_item_list_free(em->items);

    free(em->name);
    free(em);
}

bs_enum_item *bs_enum_item_new(const char *name, size_t name_len, long value) {
    bs_enum_item *item = (bs_enum_item *)malloc(sizeof(bs_enum_item));

    size_t name2_len = name_len + 1;
    char *name2 = (char *)malloc(name_len + 1);
    bzero(name2, name2_len);

    item->name = strncpy(name2, name, name_len);
    item->name_len = name_len;

    item->value = value;

    return item;
}

void bs_enum_item_free(bs_enum_item *item) {
    free(item->name);
    free(item);
}

bs_type *bs_type_new(bs_pkg *pkg, const char *name, size_t name_len) {
    bs_type *type = (bs_type *)malloc(sizeof(bs_type));

    type->pkg = pkg;

    type->cols = bs_col_list_new(5);

    size_t name2_len = name_len + 1;
    char *name2 = (char *)malloc(name_len + 1);
    bzero(name2, name2_len);

    type->name = strncpy(name2, name, name_len);
    type->name_len = name_len;

    return type;
}

void bs_type_free(bs_type *type) {
    bs_col_list_free(type->cols);
    free(type->name);
    free(type);
}
