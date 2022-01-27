#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

int main (int argc, char ** argv) {
    int i;
        // start from 1 because argv[0] is the name of the program
    for( i=1; i<argc-1; i++)
    {
        int pd[2];
        pipe(pd);

        if (!fork()) {
            dup2(pd[1], 1); // remap output back to parent
            char *argv_toexecvp[] = { argv[i], NULL }; // I assume the use is with commands without parameters
            execvp(argv[i],argv_toexecvp); //run the command execlp
            perror("exec");
            exit(1) ;
        }

        // remap output from previous child to input
        dup2(pd[0], 0);
        close(pd[1]);
    }
    char *argv_toexecvp[] = { argv[i], NULL };
    execvp(argv[i],argv_toexecvp); // run the last command execlp
    perror("exec");
    exit(1) ;
}
