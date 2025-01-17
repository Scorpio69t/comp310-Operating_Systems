#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "shellmemory.h"
#include "shell.h"
#include "header.h"

int MAX_ARGS_SIZE = 7;

int help();
int quit();
int badcommand();
int badcommandTooManyTokens();
int badcommandFileDoesNotExist();
int badcommandTooManyScripts();
int badcommandBadPolicy();
int badcommandMatching();
int outOfMemError();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int my_ls();
int echo();
int exec(int size, char * scripts[], char* policy);
int add_script(char * script, struct PCB_struct *process);


int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		if (strcmp(command_args[0], "set")==0 && args_size > MAX_ARGS_SIZE) {
			return badcommandTooManyTokens();
		}
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
		if (args_size < 3) return badcommand();
		char* value = (char*)calloc(1,150);
		char spaceChar = ' ';

		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], 30);
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 2) return badcommand();
		return my_ls();
	
	}else if (strcmp(command_args[0], "echo")==0) {
		if (args_size > 2) return badcommand();
		return echo(command_args[1]);
	}else if (strcmp(command_args[0], "exec")==0){
		if (args_size > 5)  {
			// for (int i=0;i<args_size;i++){
			// 	printf("%s \n",command_args[i]);
			// }
			badcommandTooManyScripts();
		}
		if (args_size < 3) return badcommandBadPolicy();
		char *policy = command_args[args_size-1];
		if (!((strcmp(policy, "FCFS" ) == 0) || (strcmp(policy, "SJF" ) == 0) || (strcmp(policy, "RR" ) == 0) || (strcmp(policy, "AGING" ) == 0))) return badcommandBadPolicy();
		if (args_size == 3) {
			return run(command_args[1]);
		}
		char *scripts[args_size-2];
		for(int i = 1; i < args_size-1; i++){
			scripts[i-1] = command_args[i];
		}
		int size = sizeof(scripts)/sizeof(scripts[0]);
		for (int i = 0; i<size; i++) {
			for (int j = 0; j<size; j++) {
				if (i==j) continue;
				if (strcmp(scripts[i],scripts[j])==0) return badcommandMatching();
			}
		}
		return exec(size, scripts, policy);
	} else return badcommand();
}

int help(){

char help_string[] = "COMMAND			DESCRIPTION\n \
help				Displays all the commands\n \
quit				Exits / terminates the shell with “Bye!”\n \
set VAR STRING			Assigns a value to shell memory\n \
print VAR			Displays the STRING assigned to VAR\n \
run SCRIPT.TXT			Executes the file SCRIPT.TXT\n \
echo VAR/TEXT      		Print string or value of variable\n \
my_ls					Print files and folders in current directory \n \
exec SCRIPT.TXT SCRIPT.TXT SCRIPT.TXT POLICY			Run scripts according to policy";

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

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}


int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int badcommandTooManyScripts(){
	printf("%s\n", "Bad command: Too many scripts");
	return 4;
}

int badcommandBadPolicy(){
	printf("%s\n", "Bad command: Invalid Policy");
	return 5;
}

int badcommandMatching(){
	printf("%s\n", "Bad command: Matching Files");
	return 6;
}

int outOfMemError(){
	printf("%s\n", "Out of memory: terminating command");
	return 7;
}


int set(char* var, char* value){
	char *link = "=";
	char buffer[1000];
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
	struct PCB_struct * process = malloc(sizeof(struct PCB_struct));
	process->mem_current=0;
	process->mem_end=0;
	process->mem_start=0;
	process->length=0;
	process->score=0;
	process->next=NULL;
	int errCode = add_script(script, process);
	if (errCode != 0) {
		return errCode;
	}
	add_process_to_list(process);
	run_process(process->PID,-1);
	clean_mem(process->PID);
	remove_process(process->PID);
	return 0;
}

int my_ls(){
	int errCode = system("ls | sort");
	return errCode;
}

int echo(char* var){
	if(var[0] == '$'){
		var++;
		printf("%s\n", mem_get_value(var)); 
	}else{
		printf("%s\n", var); 
	}
	return 0; 
}

int exec(int size, char *scripts[], char *policy){
	for (int i = 0; i<size; i++) {
		struct PCB_struct * process = malloc(sizeof(struct PCB_struct));
		process->PID=0;
		process->mem_current=0;
		process->mem_end=0;
		process->mem_start=0;
		process->length=0;
		process->score=0;
		process->next=NULL;
		int errCode = add_script(scripts[i], process);
		if (errCode != 0) {
			return errCode;
		}
		add_process_to_list(process);
	}
	scheduler(policy);
	return 0;
};

int add_script(char * script, struct PCB_struct *process) {
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file
	if(p == NULL){
		return badcommandFileDoesNotExist();
	}
	process->PID = rand();
	int mem_pos = -1;
	int i = 1;
	fgets(line,999,p);
	while(1) {
		char *num = malloc(sizeof(char));
		sprintf(num, "%d",i);
		mem_pos = mem_set_command(num, line);
		if (mem_pos == -1){
			return outOfMemError();
		}
		if (i==1) {
			process->mem_start = mem_pos;
			process->mem_current = mem_pos;
		}
		if(feof(p)){
			break;
		}
		memset(line, 0, sizeof(line));
		fgets(line,999,p);
		i++;
	}
	process->mem_end = mem_pos;
	process->length = process->mem_end - process->mem_start+1;
	process->score = process->length;
    fclose(p);
	return 0;
}