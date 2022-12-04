#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 100;

int help();
int quit();
int badcommand();
int toomanyarg();
int set(char* command_args[], int args_size);
int print(char* var);
int run(char* script);
int echo (char* token);
int my_ls ();
int badcommandFileDoesNotExist();
static int comparator(const void* string1, const void* string2);

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		return badcommand();
	}


	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 2) return badcommand();	
		if ( 7 < args_size) return toomanyarg();	
		return set(command_args, args_size);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);

	} else if (strcmp(command_args[0], "echo")==0) {
		if (args_size != 2) return badcommand();
		return echo(command_args[1]);
	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size != 1) return badcommand();
		return my_ls();
	} else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n \
echo VAR/TEXT      	Print string or value of variable\n \
my_ls			Print files and folders in current directory";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

int toomanyarg(){
	printf("%s\n", "Bad Command: Too many tokens");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int set(char* command_args[], int args_size){
	char *link = "=";
	char buffer[1000];
	char *var = command_args[1];
	char value[500]= "";
	strcpy(value, command_args[2]);
	for (int i=3; i<args_size; i++){ 
		char convert[99] = "";
		strcpy(convert, command_args[i]);
		char string[101] = " ";
		strcat(string, convert);
		strcat(value, string);

	}
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}

int echo (char* token){
	char var_name[100];
	if (token[0] == '$'){
		strncpy(var_name, token+1, 100);
	} else {
		printf("%s \n", token);
		return 0;	
	}
	if (strcmp(mem_get_value(var_name), "Variable does not exist")==0) {
		printf("\n");
		return 0;
	} else {
		printf("%s \n", mem_get_value(var_name));
		return 0;
	}
	return 1;
}

int my_ls() {
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	char *list[100];
	int counter = 0;
	if (d) {
		while ((dir=readdir(d)) != NULL) {
			if (dir->d_name[0] == '.') {
			 	continue;
			}
			list[counter]=dir->d_name;
			counter++;
		}
	}
	closedir(d);
	qsort(list, counter, sizeof(const char*), comparator);
	for (int i =0; i<counter; i++) printf("%s \n", list[i]); 
	return 0;
}

static int comparator(const void* string1, const void* string2) {
	return strcmp(*(const char**)string1, *(const char**)string2);
}