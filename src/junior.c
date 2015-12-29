#include <stdio.h>
#include <stdlib.h>

/* Compiles these functions if compiling on a Windows */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* readline function */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

/* add_history function */
void add_history(char*, unused) {}

/* If compiling on *nix, include and use from editline */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

int main(int argc, char** argv) {
    
    puts("\n\tJunior\nDeveloped by Noah Altunian (github.com/naltun/)\n");
    puts("Press ctrl+C to Exit\n");
    
    while (1) {
        
        /* Output prompt and retrieve input */
        char* input = readline(">> ");
        
        /* Add input to history (for retrieval with up/down arrows) */
        add_history(input);
        
        /* Return input */
        printf("%s\n", input);
        
        /* Free input memory */
        free(input);
        
    }
    
    return 0;
}