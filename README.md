# wish-shell

This shell supports the following built in commands.

exit: When the user types exit, shell exits

cd: cd always take one argument (0 or >1 args is signaled as an error). It changes directory to the path supplied by the user

path: The path command takes 0 or more arguments, with each argument separated by whitespace from the others. A typical usage would be like this: wish> path /bin /usr/bin, which would add /bin and /usr/bin to the search path of the shell. If the user sets path to be empty, then the shell won't be able to run any programs (except built-in commands). To run your user programs, please specify the path!

loop: This is a special built-in command! When you type:
prompt> loop 10 somecommand
the wish shell will run somecommand ten times.
For example, if the user types echo hello as the command (to run the program /bin/echo , which just prints out whatever the arguments to it are), the shell should do this:  
prompt> loop 5 echo hello  
hello   
hello  
hello   
hello  
hello  
prompt>  
You can also specify a $loop variable. Thus, the user should then be able to do type:   
prompt> loop 5 echo hello $loop 
hello 1  
hello 2 
hello 3  
hello 4  
hello 5   
prompt>  

To run in batch mode, type:  
./wish filename
