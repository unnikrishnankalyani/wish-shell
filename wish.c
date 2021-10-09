#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct Path{
  char *pathname;
  struct Path *next;
}Path;

Path *paths;

void printerror(){
    //printf("verify %s\n",command);
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void checkinpath(char *command, char *string){
    Path *temp = paths;
    
    while (temp){
        char *c = (char *)malloc(sizeof(temp->pathname));
        strcpy(c, temp->pathname);
        strcat(c, command);
        int x = access(c, X_OK);
        
        if(x ==0){
            int rc = fork();
            if (rc < 0) { // fork failed; exit
                fprintf(stderr, "fork failed\n");
                exit(1);
            } else if (rc == 0) { // child (new process)
                char *myargs[3];
                myargs[0] = strdup(command); // program: "ls" d
                
                int i = 1;
                while(command){
                    command  = strsep(&string, " ");
                    // printf("command %s\n",command);
                    if (command){
                        if(*command!=0){
                            myargs[i] = command; 
                            i++;
                        }
                    }
                }
                myargs[i] = NULL;
                // for (i=0;myargs[i]!=NULL;i++){
                //     printf("myargs[i], %s\n", myargs[i]);
                // }
                execv(c, myargs);
                } else { // parent goes down this path (main)
                wait(NULL);  
                return;                 
            }  
        }
        temp = temp->next;
    }
    printerror();
}

void addtopath(char *string){
    char *command  = strsep(&string, " ");
    Path *temp;
    int init = 1;
    while(command){
        //create node
        Path *new = (Path*)malloc(sizeof(Path));
        command[strcspn(command, "\n")] = 0; //manage \n to avoid weird errors
        char *c = "/";
        
        new->pathname = (char *)malloc(sizeof(command)+1);
        strcpy(new->pathname,command);
        strcat(new->pathname, c);
        new->next = NULL;
        
        if(init){
            paths = new;
            temp = paths;
        } else {
        temp->next = new;
        temp = temp->next;
        }
        init = 0;
        command  = strsep(&string, " ");
   
    }
    
}

void cd(char *string){
    char *newdir  = strsep(&string, " ");
    while(newdir){
        newdir  = strsep(&string, " ");
        if (newdir){
            if(*newdir!=0){
                printerror();
                return;
            }
        }
    }
    chdir(newdir);
}

void process_command(char *buffer){
    buffer[strcspn(buffer, "\n")] = 0;
    char *string = strdup(buffer);
    char *command  = strsep(&string, " ");
    if(strcmp(command, "exit")==0 ){
        exit(0);
    }
    else if (strcmp(command, "cd")==0){
        cd(string);
    }
    else if (strcmp(command, "path")==0){
        addtopath(string);
    }
    else{
        checkinpath(command, string);
    }
}

void execute_file(char *file_){
  FILE *fptr;
  if((fptr = fopen(file_,"r"))){
    char *line;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fptr)) != -1) {
        process_command(line);
    }
    if(line)
      free(line);
    fclose(fptr);
  }
  else{
      exit(1);
  }
  return;
}

int main(int argc, char *argv[]){
    char buffer[32];
    char *b = buffer;
    size_t bufsize = 32;
    if (argc == 1){
        while(1){
            printf("wish>");
            getline(&b,&bufsize,stdin);
            
            process_command(buffer);
        }
    } else {
        execute_file(argv[1]);
    }
    
    return 0;
}