#include "common.h"

#include "miniml.tab.h"

extern int yyparse(void);

void main_init(void);
void main_free(void);

AstList *prog = NULL;


int main(int argc, char *argv[]) {
    // TODO: add option flags...
   
    main_init();
    atexit(main_free);

    switch (yyparse()) {
    case 0:
        // successful
        break;
    case 1:
        fprintf(stderr, "Invalid input\n");
        exit(EXIT_FAILURE);
    case 2:
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    default:
        fprintf(stderr, "Unknown error\n");
        exit(EXIT_FAILURE);
    }

    
    alist_print(prog);
    printf("\n\n");
    

    Environment *global_env = env_init(); 

    // usage example for visitor functions
    ListIterator progIter;
    list_iterate(&prog, &progIter);
    while (list_iter_has_more(&progIter)) {
        Ast *expr = list_iter_next(&progIter);

        NameList *names;
        Type *type = visit_type(global_env, expr, &names);

        if (!type_equ(type, terror)) { // if no error, evaluate
            Value *value = visit_eval(global_env, expr);

            if (list_length(names) == 1) {
                int name = *(int*) list_data(names);
                global_env = env_make(name, type, value, global_env);
                env_print(global_env);
            } else {
                Environment *start = global_env;
                global_env = env_addlist(names, type->typeTuple, value->valTuple, global_env);

                env_printrange(global_env, start);
            }
        }
    }

    return EXIT_SUCCESS;
}

void main_init(void) {
    names_init();
}

void main_free(void) {
    alist_free(prog);
    names_free();
}
