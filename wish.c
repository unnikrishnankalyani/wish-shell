#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct Path{
  char *pathname;
  struct Path *next;
}Path;

Path *paths;
void pathinit(){
    free(paths);
    paths = (Path*)malloc(sizeof(Path));
    paths->pathname = "/bin/";
    paths->next = NULL;
}

void printerror(){
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

void checkinpath(char *command, char *string, char *fileop){
    
    Path *temp = paths;

    char *fileextract = NULL;
    if (fileop){
        fileextract  = strsep(&fileop, " ");
        while(*fileextract==0){
            fileextract  = strsep(&fileop, " ");
            if(fileextract == NULL){
                printerror();
                return;
            }
        }
    }
    // printf("fileextract, %s\n", fileextract);
    
    while (temp){
        char *c = (char *)malloc(sizeof(temp->pathname));
        strcpy(c, temp->pathname);
        strcat(c, command);
        int x = access(c, X_OK);
        
        if(x ==0){
            int rc = fork();
            if (rc < 0) { 
                exit(1);
            } else if (rc == 0) { 
                if(fileextract){
                    char *filecpy = strsep(&fileop, " ");
                    while(filecpy){
                        if(*filecpy!=0){
                            // printf("mul files :%s",filecpy);
                            printerror();
                            return;
                        }
                        filecpy  = strsep(&fileop,  " ");
                    }
                    close(STDOUT_FILENO);
                    open(fileextract, O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);
                }
                

                char *myargs[10];
                int i = 0;
                while(command){
                    // printf("command %s\n",command);
                    if(*command!=0){
                        myargs[i] = command; 
                        i++;
                    }
                    command  = strsep(&string, " ");
                }
                myargs[i] = NULL;
                execv(c, myargs);
                } else { 
                wait(NULL);  
                return;                 
            }  
        }
        temp = temp->next;
    }
    printerror();
}

void addtopath(char *string){
    free(paths);
    paths = NULL;
    char *command  = strsep(&string, " ");
    Path *temp;
    int init = 1;
    while(command){
        //create node
        Path *new = (Path*)malloc(sizeof(Path));
        command[strcspn(command, "\n")] = 0; //manage \n to avoid weird errors
        char *c = "/";
        // printf("%s,%ld", command, strlen(command));
        new->pathname = (char *)malloc(strlen(command)+2);
        
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
    if(string == NULL){
        printerror();
        return;
    }
    char *newdir  = strsep(&string, " ");
    while (*newdir==0){
        newdir  = strsep(&string, " ");
        if(newdir==NULL){
            printerror();
            return;
        }
    }
    //handle multiple arguments
    char *newdircpy = strsep(&string, " ");
    while(newdircpy){
        if(*newdircpy!=0){
            // printf("newdir not empty :%s",newdircpy);
            printerror();
            return;
        }
        newdircpy  = strsep(&string,  " ");
    }
    
    //all good, change
    int status = chdir(newdir);
    if(status == -1){
        printerror();
    }
}

void process_command(char *buffer){
    if (buffer==NULL){
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0;

    char *fileop = strdup(buffer);

    char *string = strsep(&fileop, ">");

    // printf("string, %s\n", string);
    // printf("opfile, %s\n", fileop);

    int failbuiltin = 0;
    if(fileop!=NULL){
        failbuiltin = 1;
    }
    char *command  = strsep(&string, " ");
    
    while(*command==0){
        command  = strsep(&string, " ");
        if(command == NULL){
            return;
        }
    }

    if(strcmp(command, "exit")==0 ){
        if(string!=NULL || failbuiltin){
            printerror();
        }else{
            exit(0);
        }
        
    }
    else if (strcmp(command, "cd")==0){
        if(failbuiltin){
            printerror();
        }else{
            cd(string);
        }
    }
    else if (strcmp(command, "path")==0){
        if(failbuiltin){
            printerror();
        }else{
            addtopath(string);
        }
        
    }
    else{
        checkinpath(command, string, fileop);
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
    pathinit();
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