#include <stdio.h>
#include <stdlib.h>

#include "bs_print.h"

#define TAB(level) { int i_; for(i_ = 0; i_ < level; i_ ++) printf("  "); }

void bs_print_doc(bs_doc *doc, int level) {
    int i;
    for (i = 0; i < doc->pkgs->len; i ++) {
        bs_pkg *pkg = bs_pkg_list_get(doc->pkgs, i);

        TAB(level);
        printf("pkg %s = %d\n", pkg->name, pkg->id);
        TAB(level);
        printf("{\n");

        bs_print_enum_list(pkg->enums, level + 1);
        bs_print_def_list(pkg->defs, level + 1);

        TAB(level);
        printf("}\n");
    }
}

void bs_print_enum_list(bs_enum_list *list, int level) {
    int i;
    for (i = 0; i < list->len; i ++) {
        bs_enum *em = bs_enum_list_get(list, i);

        TAB(level);
        printf("enum %s : ", em->name);

        bs_print_type(em->type);

        printf("\n");
        TAB(level);
        printf("{\n");

        int max_len = 0;
 
        int j;
        for (j = 0; j < em->items->len; j ++) {
            bs_enum_item *item = bs_enum_item_list_get(em->items, j);
            max_len = max_len < item->name_len ? item->name_len : max_len;
        }

        for (j = 0; j < em->items->len; j ++) {
            bs_enum_item *item = bs_enum_item_list_get(em->items, j);

            TAB(level + 1);
            printf("%s", item->name);

            int k;
            for (k = 0; k <= max_len - item->name_len; k ++) {
                printf(" ");
            }

            printf("= %ld", item->value);

            if (j != em->items->len - 1)
                printf(",");

            printf("\n");
        }

        TAB(level);
        printf("}\n\n");
    }
}

void bs_print_def_list(bs_def_list *list, int level) {
    int i;
    for (i = 0; i < list->len; i ++) {
        bs_def *def = bs_def_list_get(list, i);

        TAB(level);
        printf("def %s = %d\n", def->name, def->id);

        bs_print_col_list(def->cols, level + 1);

        printf("\n");

        if (i != list->len - 1)
            printf("\n");
    }
}

void bs_print_col_list(bs_col_list *list, int level) {
    TAB(level - 1);
    printf("{\n");

    int k;
    int max_len = 0;
    for (k = 0; k < list->len; k ++) {
        bs_col *col = bs_col_list_get(list, k);
        max_len = max_len < col->name_len ? col->name_len : max_len;
    }

    for (k = 0; k < list->len; k ++) {
        bs_col *col = bs_col_list_get(list, k);

        TAB(level);
        printf("%s", col->name);

        int i;
        for (i = 0; i <= max_len - col->name_len; i ++) {
            printf(" ");
        }

        printf(": ");

        bs_print_type(col->type);

        if (BS_TL8 <= col->type && col->type <= BS_TL64) {
            printf("\n");
            bs_print_col_list(col->cols, level + 1);
        }

        if (k != list->len - 1)
            printf(",");

        printf("\n");
    }

    TAB(level - 1);
    printf("}");
}

void bs_print_type(bs_type type) {
    switch (type) {
        case BS_TI8:  printf("int8");  break;
        case BS_TI16: printf("int16"); break;
        case BS_TI32: printf("int32"); break;
        case BS_TI64: printf("int64"); break;

        case BS_TU8:  printf("uint8");  break;
        case BS_TU16: printf("uint16"); break;
        case BS_TU32: printf("uint32"); break;
        case BS_TU64: printf("uint64"); break;

        case BS_TE8:  printf("enum8");  break;
        case BS_TE16: printf("enum16"); break;
        case BS_TE32: printf("enum32"); break;
        case BS_TE64: printf("enum64"); break;

        case BS_TL8:  printf("list8");  break;
        case BS_TL16: printf("list16"); break;
        case BS_TL32: printf("list32"); break;
        case BS_TL64: printf("list64"); break;

        case BS_TS8:  printf("string8");  break;
        case BS_TS16: printf("string16"); break;
        case BS_TS32: printf("string32"); break;
        case BS_TS64: printf("string64"); break;
    }
}
