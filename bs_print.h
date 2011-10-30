#ifndef _BS_PRINT_H_
#define _BS_PRINT_H_

#include "bs_ast.h"

void bs_print_doc(bs_doc *doc, int level);
void bs_print_def_list(bs_def_list *list, int level);
void bs_print_col_list(bs_col_list *list, int level);
void bs_print_enum_list(bs_enum_list *list, int level);
void bs_print_type(bs_type type);

#endif
