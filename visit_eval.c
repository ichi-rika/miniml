#include <stdio.h>
#include <stdlib.h>

#include "names.h"
#include "ast.h"
#include "symbols.h"

#include "values.h"
#include "environment.h"
#include "visit.h"

#define VEVAL(type) value_t *eval_##type (env_t *env, ast_t * type , int *nameptr)

VEVAL(var);
VEVAL(list);
VEVAL(funcall);
VEVAL(let);
VEVAL(ifelse);
VEVAL(tuple);

#define VERR(str) do { fprintf(stderr, str "\n"); return value_make_unit(); } while (false)
#define VERR2(str, ...) do { fprintf(stderr, str "\n", __VA_ARGS__); return value_make_unit(); } while (false)

#define setname(val) do { if (nameptr != NULL) *nameptr = val; } while (false)

// assumes all types are sound already
value_t *visit_eval(env_t *env, ast_t *expr, int *nameptr) {
   
    setname(NO_NAME); // default name

    switch (expr->type) {
    case e_unit:
        return value_make_unit();
    case e_int:
        return value_make_int(expr->exprInteger);
    case e_float:
        return value_make_float(expr->exprFloat);
    case e_bool:
        return value_make_bool(expr->exprBool);
    case e_string:
        return value_make_string(expr->exprString);
    case e_var:
        return eval_var(env, expr, nameptr);
    case e_block:
        return visit_eval(env, expr->exprBlock, nameptr);
    case e_list:
        return eval_list(env, expr, nameptr);
    case e_funcall:
        return eval_funcall(env, expr, nameptr);
    case e_let:
        return eval_let(env, expr, nameptr);
    case e_if:
        return eval_ifelse(env, expr, nameptr);
    case e_tuple:
        return eval_tuple(env, expr, nameptr);
    default:
        VERR("Evaluation not implemented yet");
    }

}


VEVAL(var) {
    while (env != NULL) {
        if (env->name == var->exprVariable)
            return env->value;
        env = env->next;
    }
    VERR2("Undefined name %s", names_getnm(var->exprVariable));
}

VEVAL(list) {
    value_t *value;

    astlist_t *expr = list->exprList;
    while (expr != NULL) {
        value = visit_eval(env, expr->elem, nameptr);
        expr = expr->next;
    }

    return value;
}

VEVAL(funcall) {
    value_t *func = visit_eval(env, funcall->exprFunCall.function, NULL);
    astlist_t *args = funcall->exprFunCall.args;

    switch (func->type) {
    case et_fun:
        // normal function
        break;
    case et_natfun1:
    {
        if (args->size > 1) {
            VERR("Too many arguments"); 
        }
        
        value_t *arg = visit_eval(env, args->elem, NULL);
        
        return (func->valNatfun1)(arg);
    }
    case et_natfun2:
    {
        if (args->size < 2) {
            VERR("Native functions cannot be curried");
        }
        if (args->size > 2) {
            VERR("Too many arguments");
        }

        value_t *arg1 = visit_eval(env, args->elem, NULL);
        value_t *arg2 = visit_eval(env, args->next->elem, NULL);

        return (func->valNatfun2)(arg1, arg2);
    }
    default:
        VERR("Expression is not a function and cannot be applied");
    }

    env_t *callsite = func->valFun.defsite;    
    namelist_t *params = func->valFun.params;

    if (args->size > params->size) {
        VERR("Too many arguments");
    }

    while (args != NULL && params != NULL) {
        value_t *value = visit_eval(env, args->elem, NULL);

        callsite = env_make(params->name, value, callsite);
        
        args = args->next;
        params = params->next;
    }

    if (params != NULL) { // partial currying
        return value_make_fun(callsite, params, func->valFun.body);
    } else {
        return visit_eval(callsite, func->valFun.body, nameptr);
    }
}

VEVAL(let) {
    namelist_t *names = let->exprLet.names;

    value_t *valExpr;

    if (let->exprLet.params == NULL) { // it's a variable binding
        // don't support recursive variables
        if (let->exprLet.rec) {
            VERR("Recursive variable bindings aren't supported");
        }

        valExpr = visit_eval(env, let->exprLet.expr, NULL);
    } else { // it's a function binding
        // a function is never evaluated before it's called
        // only pass the block
        
        // check that there is no invalid reference in the function body
        

        valExpr = value_make_fun(env, let->exprLet.params, let->exprLet.expr);

        // if it's recursive, the function exists from the start of the body
        if (let->exprLet.rec) {
            valExpr->valFun.defsite = env_make(
                names->name, valExpr, valExpr->valFun.defsite);
        }
    }

    if (let->exprLet.block != NULL) { // it's a let-in
        env_t *newEnv = env;

        if (names->next == NULL) { // single name
            newEnv = env_make(names->name, valExpr, newEnv);
        } else { // tuple binding
            // expr has to be a tuple as well, with the same length
            if (valExpr->type != et_tuple) {
                VERR("Tuple binding must match with a tuple expression");
            }
            if (names->size != valExpr->valTuple->size) {
                VERR("Tuple lengths must match in tuple binding");
            }
            
            vlist_t *elems = valExpr->valTuple;
            while (names != NULL) {
                newEnv = env_make(names->name, elems->elem, newEnv);

                names = names->next;
            }
        }
        
        return visit_eval(newEnv, let->exprLet.block, NULL);
    } else { // it's a global let
        // then set the value name
        setname(names->name);
        return valExpr;
    }
}

VEVAL(ifelse) {
    value_t *cond = visit_eval(env, ifelse->exprIf.cond, NULL);
    
    if (cond->valBool) {
        return visit_eval(env, ifelse->exprIf.bIf, NULL);
    } else {
        return visit_eval(env, ifelse->exprIf.bElse, NULL);
    }
}

VEVAL(tuple) {
    vlist_t *elems = NULL;
    astlist_t *exprs = tuple->exprTuple;

    while (exprs != NULL) {
        elems = vlist_make(visit_eval(env, exprs->elem, NULL), elems);
        exprs = exprs->next;
    }

    return value_make_tuple(vlist_rev(elems));
}

