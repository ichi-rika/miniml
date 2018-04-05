#ifndef _NAMES_H_
#define _NAMES_H_

#include <stddef.h>

#define NO_NAME 0

void names_init(void);
void names_free(void);

void names_settablecap(int capacity);

int names_getid(const char *name);
const char *names_getnm(int id);

int operator_id(int t);

/* list */

void nmlist_free(NameList *list);
void nmlist_print(NameList *list);

/* standard name */

extern int name_addi, name_addf;
extern int name_subi, name_subf;
extern int name_muli, name_mulf;
extern int name_divi, name_divf;
extern int name_equal;
extern int name_lt, name_lte;
extern int name_gt, name_gte;
extern int name_compare;
extern int name_and, name_or;
extern int name_print_string;
extern int name_print_int;
extern int name_print_float;
extern int name_print_bool;
extern int name_int_of_float;
extern int name_float_of_int;

#endif // _NAMES_H_