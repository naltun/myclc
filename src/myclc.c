#include "../libs/mpc.h"

// Compile these functions if compiling on a Windows
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// readline function
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

// add_history function
void add_history(char*, unused) {}

// If compiling on *nix, include and use from editline
#else
#include <editline/readline.h>
#endif

// Create enum of lval typeS
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

// Define lval (Lisp Value) struct
typedef struct lval {
    int type;
    long num;
    char* err;
    char* sym;
    int count;
    struct lval** cell;
} lval;

// Pointer to Number lval type
lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num  = x;
    return v;
}


// Pointer to Error lval type
lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err  = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

// Pointer to Symbol lval type
lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym  = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

// Pointer to empty Sexpr lval type
lval* lval_sexpr(void) {
    lval* v  = malloc(sizeof(lval));
    v->type  = LVAL_SEXPR;
    v->count = 0;
    v->cell  = NULL;
    return v;
}

// Delete lval and release memory function
void lval_del(lval* v) {
    switch (v->type)
    {
        case LVAL_NUM: break;

        // If v->type is Error or Symbol then free the string data
        case LVAL_ERR:
            free(v->err);
            break;

        case LVAL_SYM:
            free(v->sym);
            break;

        // If v->type is Sexpr then delete all internal elements
        case LVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }
            // Free allocated memory containing the pointer
            free(v->cell);
        break;
    }

    // Free the memory allocated to lval (locally as 'v')
    free(v);
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

// Takes a single element from the S-Expression at [i] and shifts the remaining list backwards. Returns the value [i] without deleting it
lval* lval_pop(lval* v, int i) {
    // Finds the item at [i]
    lval* x = v->cell[i];

    // Shifts the memory after [i]
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));

    // Decrease the count of items in the list
    v->count--;

    // Finally, reallocate the memory used
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++)
    {
        // Print Value
        lval_print(v->cell[i]);

        // If the last element is trailing space then don't print
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }

    putchar(close);
}

// Construct what to print (see following function 'lval_println')
void lval_print(lval* v) {
    switch (v->type)
    {
        case LVAL_NUM:
            printf("%li", v->num);
            break;

        case LVAL_ERR:
            printf("Error: %s", v->err);
            break;

        case LVAL_SYM:
            printf("%s", v->sym);
            break;

        case LVAL_SEXPR:
            lval_expr_print(v, '(', ')');
            break;
    }
}

// Print lval
void lval_println(lval* v) {
    lval_print(v);
    putchar('\n');
}

// Functions similarly to eval_op function
// Takes a single lval* which represents a list of all arguments which need operation
lval* builtin_op(lval* a, char* op) {

    // Check all arguments are numbers
    for (int i = 0; i < a->count; i++)
    {
        if (a->cell[i]->type != LVAL_NUM)
        {
            lval_del(a);
            return lval_err("Cannot operate on a non-number!");
        }
    }

    // Pops the first element of the list
    lval* x = lval_pop(a, 0);

    // If there are no arguments and operation is '-' then perform unary negation
    if ((strcmp(op, "-") == 0) && a->count == 0) { x->num = -x->num; }

    // While there are elements remaining, continue
    while (a->count > 0)
    {
        // Pop element
        lval* y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0)
        {
            if (y->num == 0)
            {
                lval_del(x);
                lval_del(y);
                x = lval_err("Cannot divide by zero!");
                break;
            }

            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* lval_eval(lval* v);

// Evaluates S-expression
lval* lval_eval_sexpr(lval* v) {
    // Evaluate Sexpr's children
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // Errors
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
    }

    // If Expression is empty
    if (v->count == 0) { return v; }

    // If Expression is single
    if (v->count == 1) { return lval_take(v, 0); }

    // Checks that the first element is a Symbol
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM)
    {
        lval_del(f);
        lval_del(v);
        return lval_err("S-Expression does not begin with a symbol!");
    }

    // Call builtin_op function with an operator
    lval* result = builtin_op(v, f->sym);
    lval_del(f);
    return result;
}

// Evaluates lval type
lval* lval_eval(lval* v) {
    // S-expressions
    if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
    // All other lval types
    return v;
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ?
        lval_num(x) : lval_err("Invalid number!");
}

lval* lval_read(mpc_ast_t* t) {
    // If lval type is Symbol or Number return conversion to Symbol or Number
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

    // If > or Sexpr then create an empty list
    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

    // Fill list with any valid expression (expr)
    for (int i = 0; i < t->children_num; i++)
    {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex")  == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

int main(int argc, char** argv) {

    // syntax parsers
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr  = mpc_new("sexpr");
    mpc_parser_t* Expr   = mpc_new("expr");
    mpc_parser_t* MyCLC  = mpc_new("myclc");

    // MyCLC language definition
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                            \
        number : /-?[0-9]+/ ;                    \
        symbol : '+' | '-' | '*' | '/' | '%' ;   \
        sexpr  : '(' <expr>* ')' ;               \
        expr   : <number> | <symbol> | <sexpr> ; \
        myclc  : /^/ <expr>* /$/ ;               \
    ",
    Number, Symbol, Sexpr, Expr, MyCLC);

    puts("MyCLC -- My Command-line Lisp Calculator\nDeveloped by Noah Altunian (github.com/naltun/)\n");
    puts("Press ctrl+C to Exit\n");

    while (1) {

        // Output prompt and retrieve user input
        char* input = readline(">> ");
        
        // if input is 'exit' or 'quit', then exit with a status of 0
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") ==0) { exit(0); }

        // add user input to input history
        add_history(input);

        // Parse user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, MyCLC, &r)) {
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        } else {
            // If parse is not successful, print and delete Error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // Undefine and delete parsers
    mpc_cleanup(5, Number, Symbol, Sexpr, Expr, MyCLC);

    return 0;
}
