#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "header.h"

struct memory_struct
{
	char *var;
	char *value;
};

struct memory_struct shellmemory[1000];
struct PCB_struct *head = NULL;
struct PCB_struct *current = NULL;

// Shell memory functions

void scheduler(char *policy)
{
	if (strcmp(policy, "FCFS") == 0)
		return FCFS(-1);
	if (strcmp(policy, "SJF") == 0)
		return SJF();
	if (strcmp(policy, "RR") == 0)
		return RR();
	if (strcmp(policy, "AGING") == 0)
		return Aging();
}

void FCFS(int instruct_amount)
{
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL)
	{
		run_process(current->PID, instruct_amount);
		previous = current;
		current = current->next;
		clean_mem(previous->PID);
		remove_process(previous->PID);
	}
	return;
}

void SJF()
{
	sortPCB_length();
	FCFS(-1);
}

void sortPCB_length()
{
	struct PCB_struct *current = head;
	struct PCB_struct *upcoming = NULL;
	struct PCB_struct *temp = malloc(sizeof(struct PCB_struct));
	while (current != NULL)
	{
		upcoming = current->next;
		while (upcoming != NULL)
		{
			if (current->length > upcoming->length)
			{
				swap(temp, current, upcoming);
			}
			upcoming = upcoming->next;
		}
		current = current->next;
	}
	return;
}

void swap(struct PCB_struct *temp, struct PCB_struct *current, struct PCB_struct *upcoming)
{
	temp->PID = current->PID;
	temp->mem_start = current->mem_start;
	temp->mem_current = current->mem_current;
	temp->mem_end = current->mem_end;
	temp->length = current->length;
	temp->score = current->score;
	current->PID = upcoming->PID;
	current->mem_start = upcoming->mem_start;
	current->mem_current = upcoming->mem_current;
	current->mem_end = upcoming->mem_end;
	current->length = upcoming->length;
	current->score = upcoming->score;
	upcoming->PID = temp->PID;
	upcoming->mem_start = temp->mem_start;
	upcoming->mem_current = temp->mem_current;
	upcoming->mem_end = temp->mem_end;
	upcoming->length = temp->length;
	upcoming->score = temp->score;
	return;
}

void RR()
{
	FCFS_subroutine(2);
	struct PCB_struct *current = head;
	struct PCB_struct *previous = NULL;
	while (head != NULL)
	{
		while (current != NULL)
		{
			if (current->mem_end < current->mem_current)
			{
				int PID = current->PID;
				current = current->next;
				clean_mem(PID);
				remove_process(PID);
				continue;
			}
			current = current->next;
		}
		FCFS_subroutine(2);
		current = head;
	}
	return;
}

void FCFS_subroutine(int instruct_amount)
{
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL)
	{
		run_process(current->PID, instruct_amount);
		previous = current;
		current = current->next;
	}
	return;
}

void Aging()
{
	while (head != NULL)
	{
		if (head->mem_end < head->mem_current)
		{
			int PID = head->PID;
			clean_mem(PID);
			remove_process(PID);
			continue;
		}
		run_process(head->PID, 1);
		struct PCB_struct *current = head->next;
		while (current != NULL)
		{
			current->score--;
			if (current->score < 0)
			{
				current->score = 0;
			}
			if (current->mem_end < current->mem_current)
			{
				int PID = current->PID;
				current = current->next;
				clean_mem(PID);
				remove_process(PID);
				continue;
			}
			current = current->next;
		}
		struct PCB_struct *traverse = head;
		struct PCB_struct *newLow = head;
		struct PCB_struct *temp = malloc(sizeof(struct PCB_struct));
		while (traverse != NULL)
		{
			if (traverse->score < newLow->score)
			{
				newLow = traverse;
			}
			traverse = traverse->next;
		}
		if (newLow->score != head->score)
		{
			remove_process(newLow->PID);
			add_process_to_list(newLow);
			swap(temp, head, newLow);
		}
	}
	return;
}

void add_process_to_list(struct PCB_struct *process)
{
	if (head == NULL)
	{
		head = process;
		return;
	}
	struct PCB_struct *current = head;
	while (current->next != NULL)
	{
		current = current->next;
	}
	current->next = process;
	return;
}

void remove_process(int target_PID)
{
	struct PCB_struct *previous = NULL;
	struct PCB_struct *current = head;
	while (current != NULL)
	{
		if (current->PID == target_PID && current == head)
		{
			head = head->next;
			current->next = NULL;
			return;
		}
		if (current->PID == target_PID && current->next == NULL)
		{
			previous->next = NULL;
			current->next = NULL;
			return;
		}
		if (current->PID == target_PID && current->next != NULL)
		{
			previous->next = current->next;
			current->next = NULL;
			return;
		}
		previous = current;
		current = current->next;
	}
	return;
}

void run_process(int target_PID, int amount_instruct)
{
	struct PCB_struct *process = head;
	while (process != NULL)
	{
		if (process->PID == target_PID)
		{
			break;
		}
		process = process->next;
	}
	int end;
	int remaining = process->mem_end - process->mem_current + 1;
	if (amount_instruct == -1 || remaining <= amount_instruct)
	{
		end = process->mem_end;
	}
	else
	{
		end = process->mem_current - 1 + amount_instruct;
	}
	for (int i = process->mem_current; i <= end; i++)
	{
		char *num = malloc(sizeof(char));
		sprintf(num, "%d", i);
		parseInput(shellmemory[i].value);
		process->mem_current++;
	}
	return;
}

void mem_init()
{
	int i;
	for (i = 0; i < 1000; i++)
	{
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

void clean_mem(int target_PID)
{
	struct PCB_struct *process = head;
	while (process != NULL)
	{
		if (process->PID == target_PID)
		{
			break;
		}
		process = process->next;
	}
	for (int i = process->mem_start; i <= process->mem_end; i++)
	{
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
	return;
}

int mem_set_command(char *line_num, char *line_command)
{
	for (int i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0)
		{
			shellmemory[i].var = strdup(line_num);
			shellmemory[i].value = strdup(line_command);
			return i;
		}
	}
	return -1;
}

char *mem_get_command(char *line_num)
{
	for (int i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, line_num) == 0)
		{
			return strdup(shellmemory[i].value);
		}
	}
	return NULL;
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in)
{

	int i;

	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	// Value does not exist, need to find a free spot.
	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0)
		{
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	return;
}

// get value based on input key
char *mem_get_value(char *var_in)
{
	int i;

	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{
			return strdup(shellmemory[i].value);
		}
	}
	return "Variable does not exist";
}