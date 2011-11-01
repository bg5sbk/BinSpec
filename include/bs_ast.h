#ifndef _BS_AST_H_
#define _BS_AST_H_

#include "gen_list.h"

typedef enum bs_type bs_type;

enum bs_type {
    /* integer */
    BS_TI8 = 130, BS_TI16, BS_TI32, BS_TI64,
    
    /* unsigned integer */
    BS_TU8, BS_TU16, BS_TU32, BS_TU64,

    /* string */
    BS_TS8, BS_TS16, BS_TS32, BS_TS64,

    /* list */
    BS_TL8, BS_TL16, BS_TL32, BS_TL64,

    /* enum */
    BS_TE
};

typedef struct bs_doc bs_doc;
typedef struct bs_pkg bs_pkg;
typedef struct bs_def bs_def;
typedef struct bs_col bs_col;
typedef struct bs_enum bs_enum;
typedef struct bs_enum_item bs_enum_item;

LIST_DEF(bs_pkg_list, bs_pkg *);
LIST_DEF(bs_def_list, bs_def *);
LIST_DEF(bs_col_list, bs_col *);
LIST_DEF(bs_enum_list, bs_enum *);
LIST_DEF(bs_enum_item_list, bs_enum_item *);

struct bs_doc {
    bs_pkg *root_pkg;
};

struct bs_pkg {
    bs_doc *doc;
    bs_pkg *pkg;

    int    id;
    char  *name;
    size_t name_len;

    bs_pkg_list  *pkgs;
    bs_def_list  *defs;
    bs_enum_list *enums;
};

struct bs_def {
    bs_doc  *doc;
    bs_pkg  *pkg;
    
    int    id;
    char  *name;
    size_t name_len;

    bs_col_list *cols;
};

struct bs_col {
    bs_doc  *doc;
    bs_def  *def;

    char    *name;
    size_t   name_len;

    char    *ref_name;
    size_t   ref_name_len;

    bs_type  type;

    bs_col_list *cols;
};

struct bs_enum {
    char    *name;
    size_t   name_len;
    bs_type  type;

    bs_enum_item_list *items;
};

struct bs_enum_item {
    char  *name;
    size_t name_len;
    long   value;
};

bs_doc *bs_doc_new();

void bs_doc_free(bs_doc *doc);

bs_pkg *bs_pkg_new(bs_doc *doc, bs_pkg *pkg, int id, char *name, size_t name_len);

void bs_pkg_free(bs_pkg *pkg);

bs_def *bs_def_new(bs_pkg *pkg, int id, char *name, size_t name_len);

void bs_def_free(bs_def *def);

bs_col *bs_col_new(bs_def *def, char *name, size_t name_len, char *ref_name, size_t ref_name_len, bs_type type);

void bs_col_free(bs_col *col);

bs_enum *bs_enum_new(bs_pkg *pkg, char *name, size_t name_len, bs_type type);

void bs_enum_free(bs_enum *em);

bs_enum_item *bs_enum_item_new(char *name, size_t name_len, long value);

void bs_enum_item_free(bs_enum_item *item);

#endif

