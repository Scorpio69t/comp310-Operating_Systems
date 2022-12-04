#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "PCB.h"
#include "frame.h"



struct memory_struct{
	char *var;
	char *value;
};


struct memory_struct framememory[FRAMESIZE];
struct memory_struct varmemory[VARSIZE];
int framesize = FRAMESIZE;
int varsize = VARSIZE;
struct PCB_struct *head = NULL;
struct PCB_struct *current = NULL;
struct frame *head_frame = NULL;
// Shell memory functions


void scheduler(char * policy) {
	if (strcmp(policy, "FCFS")==0) return FCFS(-1);
	if (strcmp(policy, "SJF")==0) return SJF();
	if (strcmp(policy, "RR")==0) return RR();
	if (strcmp(policy, "AGING")==0) return Aging();
}

void FCFS(int instruct_amount){
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL) {
		run_process(current->PID,instruct_amount);
		previous = current;
		current = current->next;
		clean_mem(previous->PID);
		remove_process(previous->PID);
	}
	return; 
}

void SJF(){
	sortPCB_length();
	FCFS(-1);
}

void sortPCB_length(){
	struct PCB_struct *current = head;  
	struct PCB_struct *upcoming = NULL;  
    struct PCB_struct *temp = malloc(sizeof(struct PCB_struct)+(sizeof(int)*FRAMESIZE)/3);
	while(current != NULL) {  
        upcoming = current->next;  
        while(upcoming != NULL) {  
			if(current->length > upcoming->length) {  
				swap(temp, current, upcoming); 
			}  
			upcoming = upcoming->next;  
		}  
        current = current->next;  
    }
	return;      
    }   

void swap(struct PCB_struct *temp, struct PCB_struct *current, struct PCB_struct *upcoming){
	temp->PID=current->PID;
	temp->step_start=current->step_start;
	temp->step_current=current->step_current;
	temp->step_end=current->step_end;
	temp->length=current->length;
	temp->score=current->score;
	for (int i=0;i<framesize;i++){
		temp->pages[i]=current->pages[i];
	}
	current->PID=upcoming->PID;
	current->step_start=upcoming->step_start;
	current->step_current=upcoming->step_current;
	current->step_end=upcoming->step_end;
	current->length=upcoming->length;
	current->score=upcoming->score;
	for (int i=0;i<framesize;i++){
		current->pages[i]=upcoming->pages[i];
	}
	upcoming->PID=temp->PID;
	upcoming->step_start=temp->step_start;
	upcoming->step_current=temp->step_current;
	upcoming->step_end=temp->step_end;
	upcoming->length=temp->length;
	upcoming->score=temp->score;
	for (int i=0;i<framesize;i++){
		upcoming->pages[i]=temp->pages[i];
	}
	return;
}

void Aging(){
	while (head != NULL){
		if (head->step_end <= head->step_current) {
				int PID = head->PID;
				clean_mem(PID);
				remove_process(PID);
				continue;
		}
		run_process(head->PID,1);
		struct PCB_struct *current = head->next;
		while (current != NULL) {
			current->score--;
			if (current->score<0){
				current->score=0;
			}
			if (current->step_end <= current->step_current) {
				int PID = current->PID;
				current= current->next;
				clean_mem(PID);
				remove_process(PID);
				continue;
			}
			current=current->next;
		}
		struct PCB_struct *traverse = head;
		struct PCB_struct *newLow = head;
		struct PCB_struct *temp = malloc(sizeof(struct PCB_struct)+(sizeof(int)*FRAMESIZE)/3);;
		while (traverse != NULL) {
			if (traverse->score<newLow->score){
				newLow = traverse;
			}
			traverse = traverse->next;
		}
		if (newLow->score != head->score) {
			remove_process(newLow->PID);
			add_process_to_list(newLow);
			swap(temp, head, newLow);
		}
	}
	return;
}

void RR(){
	initialize_frame_order();
	FCFS_subroutine(2);
	struct PCB_struct *current = head;  
	struct PCB_struct *previous = NULL;
	while (head != NULL) {
		while (current != NULL) {
			if (current->step_end<current->step_current) {
				int PID = current->PID;
				current= current->next;
				remove_process(PID);
				continue;
			}
			current= current->next;
		}
		FCFS_subroutine(2);
		current = head;
	}
	resetframes();
	return;
}

void FCFS_subroutine(int instruct_amount){
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL) {
		run_process(current->PID,instruct_amount);
		previous = current;
		current = current->next;
	}
	return; 
}

void add_process_to_list(struct PCB_struct *process){
	if (head == NULL) {
		head = process;
		return;
	}
	struct PCB_struct *current = head;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = process;
	return;
}

void remove_process(int target_PID) {
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL) {
		if (current->PID == target_PID && current == head) {
			head = head->next;
			current->next = NULL;
			return;
		}
		if (current->PID == target_PID && current->next == NULL) {
			previous->next = NULL;
			current->next = NULL;
			return;
		}
		if (current->PID == target_PID && current->next != NULL) {
			previous->next = current->next;
			current->next = NULL;
			return;
		}
		previous = current;
		current = current->next;
	}
	return;
}

void run_process(int target_PID, int amount_instruct){
	struct PCB_struct *process = head;
	while (process != NULL) {
		if (process->PID == target_PID) {
			break;
		}
		process=process->next;
	}
	int end;
	int remaining = process->step_end-process->step_current;
	if (amount_instruct == -1 || remaining < amount_instruct){
		end = process->step_end;
	} else {
		end = process->step_current-1+amount_instruct;
	}
	for (int i=process->step_current; i<=end; i++) {
		int page_of_step = i/3;
		int frame_location = process->pages[page_of_step];
		int offset = i%3;
		if (frame_location == -1){
			page_fault(process);
			return;
		}
		parseInput(framememory[frame_location+offset].value);
		update_frame_order(frame_location);
		process->step_current++;
	}
	return;
}

void page_fault(struct PCB_struct *process){
	if (head->next==NULL){
		add_frame(process);
		return;
	}
	remove_process(process->PID);
	add_process_to_list(process);
	add_frame(process);
	return;
}


void add_frame(struct PCB_struct *process){
	FILE *p = fopen(process->location,"rt");
	if(p == NULL){
		return badcommandFileDoesNotExist();
	}
	int newstart=0;
	char line[1000];
	char *end_of_file = fgets(line,999,p);
	for (int i=0;i<process->step_current;i++){
		end_of_file = fgets(line,999,p);
	}
	
	for (int i=0;i<3;i++){
		char *line_num = malloc(sizeof(char));
		sprintf(line_num, "%d",process->step_current+1);
		newstart=mem_set_command_init(line_num,line,i,newstart);
		if (newstart==-1){
			full_frame(process);
			add_frame(process);
			break;
		}
		if (i==0){
			process->pages[(process->step_current+1)/3]=newstart;
		}
		if (end_of_file == NULL) {
			break;
		}
		fgets(line,999,p);
	}
	fclose(p);
	return;
}

void mem_init(){
	int i;
	for (i=0; i<framesize; i++){		
		framememory[i].var = "none";
		framememory[i].value = "none";
	}
	for (i=0; i<varsize; i++){		
		varmemory[i].var = "none";
		varmemory[i].value = "none";
	}
}

int mem_set_command_init(char *line_num, char *line_command, int num, int newstart) {
	if (newstart == -1) {
		newstart = 0;
	}
	for (int i=newstart; i<framesize; i++){
		if (strcmp(framememory[i].var, "none") == 0){
			if ((num%3==0)) {
				if (i%3==1) {
					i++;
					continue;
				}
				if (i%3==2) {
					continue;
				}
			}
			framememory[i].var = strdup(line_num);
			framememory[i].value = strdup(line_command);
			return i;
		} 
	}
	return -1;
}

void full_frame(struct PCB_struct *process){
	struct frame *current_frame = head_frame;
	while (current_frame->next != NULL){
		current_frame = current_frame->next;
	}
	int frame_to_evict = current_frame->frame_num;
	struct PCB_struct *current = head;
	struct PCB_struct *evicted_process = NULL;
	while (current != NULL) {
		int i=0;
		while(current->pages[i]!=-1){
			if (current->pages[i]==frame_to_evict) {
				current->pages[i]==-1;
				evicted_process = current;
				break;
			}
			i++;
		}
		current = current->next;
	}
	int new_page = process->step_current/3;
	process->pages[new_page]=frame_to_evict;
	update_evicted_process(frame_to_evict);
	return;
}

void update_evicted_process(int frame_num){
	update_frame_order(frame_num);
	printf("Page fault! Victim page contents:\n");
	for (int i=frame_num;i<frame_num+3;i++) {
		printf("%s",framememory[i].value);
		framememory[i].var="none";
		framememory[i].value="none";
	}
	printf("End of victim page contents \n");
	return;
}

void initialize_frame_order(){
	struct PCB_struct *current = head;
	while (current != NULL) {
		int i=0;
		while(current->pages[i]!=-1){
			add_frame_order(current->pages[i]);
			i++;
		}
		current = current->next;
	}
	return;
}

void add_frame_order(int frame_num){
	if (head_frame == NULL) {
		head_frame = malloc(sizeof(struct frame));
		head_frame->frame_num = frame_num;
		head_frame->next = NULL;
		return;
	}
	struct frame *new_frame = malloc(sizeof(struct frame));
	struct frame *old_frame_head = head_frame;
	new_frame->frame_num = frame_num;
	head_frame = new_frame;
	new_frame->next = old_frame_head;
	return;
}

void update_frame_order(int frame_num){
	struct frame *previous = NULL;
	struct frame *current = head_frame;
	while (current != NULL) {
		if (current->frame_num == frame_num && current == head_frame) {
			head_frame = head_frame->next;
			current->next = NULL;
			break;
		}
		if (current->frame_num == frame_num && current->next == NULL) {
			previous->next = NULL;
			current->next = NULL;
			break;
		}
		if (current->frame_num == frame_num && current->next != NULL) {
			previous->next = current->next;
			current->next = NULL;
			break;
		}
		previous = current;
		current = current->next;
	}
	add_frame_order(frame_num);
	return;
}

void resetmem(){
	for (int i=0; i<varsize; i++){		
		varmemory[i].var = "none";
		varmemory[i].value = "none";
	}
}

void resetframes(){
	for (int i=0; i<framesize/3; i++) {
		framememory[i].var = "none";
		framememory[i].value = "none";
	}
	head_frame = NULL;
	int check = mkdir("backing",0777);
	if (check) {
		system("exec rm -r backing");
	}
	return;
}
// Set key value pair
void mem_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<varsize; i++){
		if (strcmp(varmemory[i].var, var_in) == 0){
			varmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<varsize; i++){
		if (strcmp(varmemory[i].var, "none") == 0){
			varmemory[i].var = strdup(var_in);
			varmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<varsize; i++){
		if (strcmp(varmemory[i].var, var_in) == 0){
			return strdup(varmemory[i].value);
		} 
	}
	return "Variable does not exist";

}


void clean_mem(int target_PID){
	struct PCB_struct *process = head;
	while (process != NULL) {
		if (process->PID == target_PID) {
			break;
		}
		process=process->next;
	}
	for (int i=process->step_start; i<=process->step_end; i++) {
		framememory[i].var = "none";
		framememory[i].value = "none";
	}
	return;
}