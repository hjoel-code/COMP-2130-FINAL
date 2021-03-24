#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>



int main() {

    struct User {
        int uid;

    };

    srand(time(0));
    int n = random () % 26; 

    return 0;
}