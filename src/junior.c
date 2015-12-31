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
#include <editline/history.h>
#endif

// Determine which operator to use by comparing operator string to arithmetic sign
long eval_op(long x, char* op, long y) {
    
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    if (strcmp(op, "%") == 0) { return x % y; }
    
    return 0;
    
}

long eval(mpc_ast_t* t) {
    
    // If the string's tag is a number, return directly 
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }
    
    /* The operator is always going to be the second child 
       Notice how we check children at index */
    char* op = t->children[1]->contents; 
    
    // Store the third child in x
    long x = eval(t->children[2]);
    
    // Iterate through the remaining children
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }
    
    return x;
}

int main(int argc, char** argv) {
    
    // syntax parsers 
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Junior   = mpc_new("junior");
    
    // Junior language definition 
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                      \
        number   : /-?[0-9]+/ ;                            \
        operator : '+' | '-' | '*' | '/' | '%' ;           \
        expr     : <number> | '(' <operator> <expr>+ ')' ; \
        junior   : /^/ <operator> <expr>+ /$/ ;            \
    ",
    Number, Operator, Expr, Junior);
    
    puts("\n\tJunior\nDeveloped by Noah Altunian (github.com/naltun/)\n");
    puts("Press ctrl+C to Exit\n");
    
    while (1) {
        
        // Output prompt and retrieve user input 
        char* input = readline(">> ");
        
        // add user input to input history
        add_history(input);
        
        // Parse user input 
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Junior, &r)) {
        
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        
        } else {
            // If parse is not successful, print and delete Error 
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        
        free(input);
    }
    
    // Undefine and delete parsers 
    mpc_cleanup(4, Number, Operator, Expr, Junior);
    
    return 0;
}