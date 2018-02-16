#include "type.h"


TYPE(ifelse) {
    Type *cond = visit_type(env, ifelse->exprIf.cond, NULL);
    checkerr(cond);

    checktypes(cond, tbool, "Condition type mismatch");
    
    Type *typeIf = visit_type(env, ifelse->exprIf.bIf, NULL);
    checkerr(typeIf);
    
    Type *typeElse = visit_type(env, ifelse->exprIf.bElse, NULL);
    checkerr(typeElse);

    checktypes(typeIf, typeElse, "If-else branch types mismatch");

    return typeIf;
}
