#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int main() {
    void sep(char *str) {
        char *tok;

        tok = strtok(str, ":");

        while (tok != NULL) {
            printf("%s\n", tok);
            tok = strtok(NULL, ":");
        }
    }

    void updateClient(char *init) {
        char ch[3];
        char *token;
        char *tok;

        token = strtok(init, ":");
        while (token != NULL) {
            if (strlen(token) <= 3) {
                printf("%s\n",token);
                strcat(ch,token);
            } else {
                int len = strlen(token);
                printf("%s - %d\n", token, len);
            }
            token = strtok(NULL, ":");
        }
    }

    char str[30];
    
    strcpy(str,"Hop:He,ll,o:This:is:me"); 
    
    
    printf("%s\n", str);

    updateClient(str);


    return 0;
}