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

// Create enum of error types
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM, LERR_MOD_ZERO };

// Create enum of lval types
enum { LVAL_NUM, LVAL_ERR };

// Define lval (Lisp Value) struct
typedef struct {
    int type;
    long num;
    int err;
} lval;

// Create number lval type
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

// Create error lval type
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

// Print lval
void lval_print(lval v) {
    
    switch (v.type) {
        // If lval type is number print num then break from switch
        case LVAL_NUM:
            printf("%li", v.num);
            break;
            
        // If lval type is err
        case LVAL_ERR:
            // Check error type and print
            if (v.err == LERR_DIV_ZERO) {
                printf("Error: Can't divide by zero");
            }
            
            if (v.err == LERR_BAD_OP) {
                printf("Error: Invalid operator");
            }
            
            if (v.err == LERR_BAD_NUM) {
                printf("Error: Invalid number");
            }
            
            if (v.err == LERR_MOD_ZERO) {
                printf("Error: Can't use modulus with zero");
            }
            
        break;
    }
    
}

// Print lval
void lval_println(lval v) {
    lval_print(v);
    putchar('\n');
}

// Determine which if lval is error or number
// If the lval is an error, determine error and print
// Otherwise solve expression
lval eval_op(lval x, char* op, lval y) {
    
    // If value is error, return error
    if (x.type == LVAL_ERR) {
        return x;
    }
    
    if (y.type == LVAL_ERR) {
        return y;
    }
    
    // If value is a number peform math
    if (strcmp(op, "+") == 0) {
        return lval_num(x.num + y.num);
    }
    
    if (strcmp(op, "-") == 0) {
        return lval_num(x.num - y.num);
    }
    
    if (strcmp(op, "*") == 0) {
        return lval_num(x.num * y.num);
    }
    
    if (strcmp(op, "/") == 0) {
        // If y.num is zero return error
        return y.num == 0
            ? lval_err(LERR_DIV_ZERO)
            : lval_num(x.num / y.num);
    }
    
    if (strcmp(op, "%") == 0) {
        // If y.num is zero return error
        return y.num == 0
            ? lval_err(LERR_MOD_ZERO)
            : lval_num(x.num % y.num);
    }
    
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    
    // If the string's tag is a number, check for error in conversion
    // If not, return
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE 
            ? lval_num(x)
            : lval_err(LERR_BAD_NUM);
    }
    
<<<<<<< HEAD
    // The operator is always going to be the second child 
    // Notice how we check children at index 1 
=======
    /* The operator is always going to be the second child 
       Notice how we check children at index 1 */
>>>>>>> d0d9d947823c987c2539c5a061e972f3bee36c35
    char* op = t->children[1]->contents; 
    
    // Store the third child in x
    lval x = eval(t->children[2]);
    
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
            lval result = eval(r.output);
            lval_println(result);
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
