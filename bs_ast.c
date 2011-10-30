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

bs_doc *bs_doc_new() {
    bs_doc *doc = (bs_doc *)malloc(sizeof(bs_doc));

    doc->pkgs = bs_pkg_list_new(20);

    return doc;
}

void bs_doc_free(bs_doc *doc) {
    bs_pkg_list_free(doc->pkgs);
    free(doc);
}

bs_pkg *bs_pkg_new(bs_doc *doc, int id, char *name, size_t name_len) {
    bs_pkg *pkg = (bs_pkg *)malloc(sizeof(bs_pkg));
    
    char *name2 = (char *)malloc(name_len);
    
    pkg->pkgs = bs_pkg_list_new(10);
    pkg->defs = bs_def_list_new(20);
    pkg->enums = bs_enum_list_new(5);
    pkg->name = strncpy(name2, name, name_len);
    pkg->name_len = name_len - 1;
    pkg->doc  = doc;
    pkg->id   = id;

    return pkg;
}

void bs_pkg_free(bs_pkg *pkg) {
    bs_pkg_list_free(pkg->pkgs);
    bs_def_list_free(pkg->defs);

    free(pkg->name);
    free(pkg);
}

bs_def *bs_def_new(bs_pkg *pkg, int id, char *name, size_t name_len) {
    bs_def *def = (bs_def *)malloc(sizeof(bs_def));
  
    char *name2 = (char *)malloc(name_len);

    def->cols = bs_col_list_new(20);
    def->name = strncpy(name2, name, name_len);
    def->name_len = name_len - 1;
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

bs_col *bs_col_new(bs_def *def, char *name, size_t name_len, bs_type type) {
    bs_col *col = (bs_col *)malloc(sizeof(bs_col));

    char *name2 = (char *)malloc(name_len);

    col->cols = bs_col_list_new(5);
    col->name = strncpy(name2, name, name_len);
    col->name_len = name_len - 1;
    col->doc  = def->doc;
    col->def  = def;
    col->type = type;

    return col;
}

void bs_col_free(bs_col *col) {
    bs_col_list_free(col->cols);

    free(col->name);
    free(col);
}

bs_enum *bs_enum_new(char *name, size_t name_len, bs_type type) {
    bs_enum *em = (bs_enum *)malloc(sizeof(bs_enum));

    char *name2 = (char *)malloc(name_len);

    em->items = bs_enum_item_list_new(5);
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

bs_enum_item *bs_enum_item_new(char *name, size_t name_len, long value) {
    bs_enum_item *item = (bs_enum_item *)malloc(sizeof(bs_enum_item));

    char *name2 = (char *)malloc(name_len);

    item->name = strncpy(name2, name, name_len);
    item->name_len = name_len;
    item->value = value;

    return item;
}

void bs_enum_item_free(bs_enum_item *item) {
    free(item->name);
    free(item);
}

