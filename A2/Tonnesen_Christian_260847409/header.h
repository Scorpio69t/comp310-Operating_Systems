#include <stdio.h>
#include <stdlib.h>

struct PCB_struct{
	int PID;
	int mem_start;
	int mem_current;
	int mem_end;
	int length;
	int score;
	struct PCB_struct *next;
};

extern struct PCB_struct *head;
extern struct PCB_struct *current;