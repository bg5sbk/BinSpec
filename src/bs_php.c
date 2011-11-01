#include <stdio.h>

#include "bs_php.h"

static void gen_pkg_list(FILE *file, bs_pkg_list *pkgs, int level);
static void gen_def_list(FILE *file, bs_def_list *defs, int level);
static void gen_col_list(FILE *file, bs_col_list *cols, int level);
static void gen_enum_list(FILE *file, bs_enum_list *pkgs, int level);
static void gen_enum_item_list(FILE *file, bs_enum_item_list *items, int level);
static char *get_php_type(bs_type type);

void bs_gen_php(bs_doc *doc, FILE *file) {
    fprintf(file,
"<?php\n"
"  class bs_type {\n"
"    const int8  = %d;\n"
"    const int16 = %d;\n"
"    const int32 = %d;\n"
"    const int64 = %d;\n\n"
"    const uint8  = %d;\n"
"    const uint16 = %d;\n"
"    const uint32 = %d;\n"
"    const uint64 = %d;\n\n"
"    const list8  = %d;\n"
"    const list16 = %d;\n"
"    const list32 = %d;\n"
"    const list64 = %d;\n\n"
"    const string8  = %d;\n"
"    const string16 = %d;\n"
"    const string32 = %d;\n"
"    const string64 = %d;\n\n"
"    const enumtype  = %d;\n"
"  }\n\n"
"  class bs_doc {\n"
"    public $pkgs;\n"
"    public $enums;\n\n"
"    function __construct($pkgs, $enums) {\n"
"      $this->pkgs = $pkgs;\n"
"      $this->enums = $enums;\n"
"    }\n"
"  }\n\n"
"  class bs_pkg {\n"
"    public $id;\n"
"    public $name;\n"
"    public $enums;\n"
"    public $pkgs;\n"
"    public $defs;\n\n"
"    function __construct($id, $name, $enums, $pkgs, $defs) {\n"
"      $this->id = $id;\n"
"      $this->name = $name;\n"
"      $this->enums = $enums;\n"
"      $this->pkgs = $pkgs;\n"
"      $this->defs = $defs;\n"
"    }\n"
"  }\n\n"
"  class bs_def {\n"
"    public $id;\n"
"    public $name;\n"
"    public $cols;\n\n"
"    function __construct($id, $name, $cols) {\n"
"      $this->id = $id;\n"
"      $this->name = $name;\n"
"      $this->cols = $cols;\n"
"    }\n"
"  }\n\n"
"  class bs_col {\n"
"    public $name;\n"
"    public $type;\n"
"    public $ref_name;\n"
"    public $cols;\n\n"
"    function __construct($name, $type, $ref_name, $cols) {\n"
"      $this->name = $name;\n"
"      $this->type = $type;\n"
"      $this->ref_name = $ref_name;\n"
"      $this->cols = $cols;\n"
"    }\n"
"  }\n\n"
"  class bs_enum {\n"
"    public $name;\n"
"    public $type;\n"
"    public $items;\n\n"
"    function __construct($name, $type, $items) {\n"
"      $this->name = $name;\n"
"      $this->type = $type;\n"
"      $this->items = $items;\n"
"    }\n"
"  }\n\n"
"  class bs_enum_item {\n"
"    public $name;\n"
"    public $value;\n\n"
"    function __construct($name, $value) {\n"
"      $this->name = $name;\n"
"      $this->value = $value;\n"
"    }\n"
"  }\n\n"
"  function bs_get_doc() {\n"
"    return new bs_doc(array(\n",
    BS_TI8, BS_TI16, BS_TI32, BS_TI64,
    BS_TU8, BS_TU16, BS_TU32, BS_TU64,
    BS_TL8, BS_TL16, BS_TL32, BS_TL64,
    BS_TS8, BS_TS16, BS_TS32, BS_TS64,
    BS_TE
    );

    gen_pkg_list(file, doc->root_pkg->pkgs, 0);

    fprintf(file, 
"    ));\n"
"  }\n"
"?>\n"
    );
}

#define TAB() { fprintf(file, "      "); int i_; for (i_ = 0; i_ < level; i_ ++) fprintf(file, "  "); }

static void gen_pkg_list(FILE *file, bs_pkg_list *pkgs, int level) {
    int i;
    for (i = 0; i < pkgs->len; i++) {
        bs_pkg *pkg = bs_pkg_list_get(pkgs, i);

        TAB();
        fprintf(file, "new bs_pkg(%d, '%s', array(", pkg->id, pkg->name);

        if (pkg->enums->len > 0) {
            fprintf(file, "\n");
            gen_enum_list(file, pkg->enums, level + 1);
            TAB();
        }

        fprintf(file, "), array(");

        if (pkg->pkgs->len > 0) {
            fprintf(file, "\n");
            gen_pkg_list(file, pkg->pkgs, level + 1);
            TAB();
        }

        fprintf(file, "), array(");
        
        if (pkg->defs->len > 0) {
            fprintf(file, "\n");
            gen_def_list(file, pkg->defs, level + 1);    
            TAB();
        }

        fprintf(file, "))");

        if (i != pkgs->len - 1)
            fprintf(file, ",");

        fprintf(file, "\n");
    }
}

static void gen_enum_list(FILE *file, bs_enum_list *enums, int level) {
    int i;
    for (i = 0; i < enums->len; i ++) {
        bs_enum *em = bs_enum_list_get(enums, i);

        TAB();
        fprintf(file, "new bs_enum('%s', %s, array(", em->name, get_php_type(em->type));

        if (em->items->len > 0) {
            fprintf(file, "\n");
            gen_enum_item_list(file, em->items, level + 1);
            TAB();
        }

        fprintf(file, "))");

        if (i != enums->len - 1)
            fprintf(file, ",");

        fprintf(file, "\n");
    }
}

static void gen_enum_item_list(FILE *file, bs_enum_item_list *items, int level) {
    int i;
    for (i = 0; i < items->len; i ++) {
        bs_enum_item *item = bs_enum_item_list_get(items, i);

        TAB();
        fprintf(file, "new bs_enum_item('%s', %ld)", item->name, item->value);

        if (i != items->len - 1)
            fprintf(file, ",");

        fprintf(file, "\n");
    }
}

static void gen_def_list(FILE *file, bs_def_list *defs, int level) {
    int i;
    for (i = 0; i < defs->len; i++) {
        bs_def *def = bs_def_list_get(defs, i);

        TAB();
        fprintf(file, "new bs_def(%d, '%s', array(", def->id, def->name);
       
        if (def->cols->len > 0) {
            fprintf(file, "\n");
            gen_col_list(file, def->cols, level + 1);
            TAB();
        }
        
        fprintf(file, "))");

        if (i != defs->len - 1)
            fprintf(file, ",");

        fprintf(file, "\n");
    }
}

static void gen_col_list(FILE *file, bs_col_list *cols, int level) {
    int i;
    for (i = 0; i < cols->len; i++) {
        bs_col *col = bs_col_list_get(cols, i);

        TAB();
        fprintf(file, "new bs_col('%s', %s, '%s', array(", 
            col->name, 
            get_php_type(col->type), 
            col->ref_name == NULL ? "" : col->ref_name
        );

        if (col->cols->len > 0) {
            fprintf(file, "\n");
            gen_col_list(file, col->cols, level + 1);
            TAB();
        }

        fprintf(file,"))");

        if (i != cols->len - 1)
            fprintf(file, ",");

        fprintf(file, "\n");
    }
}

char *get_php_type(bs_type type) {
    switch (type) {
        case BS_TI8:  return "bs_type::int8";
        case BS_TI16: return "bs_type:int16";
        case BS_TI32: return "bs_type::int32";
        case BS_TI64: return "bs_type::int64";
        case BS_TU8:  return "bs_type::uint8";
        case BS_TU16: return "bs_type::uint16";
        case BS_TU32: return "bs_type::uint32";
        case BS_TU64: return "bs_type::uint64";
        case BS_TL8:  return "bs_type::list8";
        case BS_TL16: return "bs_type::list16";
        case BS_TL32: return "bs_type::list32";
        case BS_TL64: return "bs_type::list64";
        case BS_TS8:  return "bs_type::string8";
        case BS_TS16: return "bs_type::string16";
        case BS_TS32: return "bs_type::string32";
        case BS_TS64: return "bs_type::string64";
        case BS_TE:   return "bs_type::enumtype";
    }
    return "";
}
