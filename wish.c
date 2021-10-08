#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
void readlines(){

}
int main(int argc, char *argv[]){
    
    char buffer[32];
    char *b = buffer;
    size_t bufsize = 32;
    // size_t characters;
    if (argc == 1){
        
        while(1){
            printf("wish>");
            // characters = 
            getline(&b,&bufsize,stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            char *string = strdup(buffer);
            char *command  = strsep(&string, " ");
            // printf("%zu characters were read.\n",characters);
            // printf("You typed: '%s'\n",buffer);
            if(strcmp(command, "exit")==0 ){
                exit(0);
            }
            else if (strcmp(command, "ls")==0){
                int rc = fork();
                if (rc < 0) { // fork failed; exit
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                } else if (rc == 0) { // child (new process)
                    char *myargs[3];
                    myargs[0] = strdup("ls"); // program: "ls" 
                    int i = 1;
                    while(command){
                        command  = strsep(&string, " ");
                        myargs[i] = command; 
                        i++;
                    }
                    myargs[i] = NULL;
                    execvp(myargs[0], myargs);
                 } else { // parent goes down this path (main)
                    wait(NULL);                   
                }              
            }
            else{
                
            }
        }
    }
    return 0;
}