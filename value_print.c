#include <stdio.h>

#include "names.h"
#include "symbols.h"
#include "ast.h"
#include "values.h"


void value_print(value_t *value) {
    switch (value->type) {
    case et_unit:
        printf("unit = ()");
        break;
    case et_int:
        printf("int = %d", value->valInt);
        break;
    case et_float:
        printf("float = %g", value->valFloat);
        break;
    case et_bool:
        printf("bool = %s", (value->valBool ? "true" : "false"));
        break;
    case et_string:
        printf("string = %s", escape(value->valString));
        break;
    case et_natfun1:
    case et_natfun2:
        printf("fun = <native-fun>");
        break;
    case et_fun:
        //TODO: use actual type
        printf("fun = <fun>");
        break;
    case et_tuple:
        //TODO: use actual type
        printf("tuple = ");
        vlist_print(value->valTuple);
        break;
    default:
        printf("<unknown>");
        break;
    }
}

void value_ptprint(int name, value_t *value) {
    if (name == NO_NAME) {
        printf("-");
    } else {
        printf("val %s", names_getnm(name));
    }
    printf(" : ");
    
    value_print(value);
    
    printf("\n");
}
