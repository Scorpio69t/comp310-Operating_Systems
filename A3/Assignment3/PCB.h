#include <stdio.h>
#include <stdlib.h>

struct PCB_struct
{
	int PID;
	int step_start;
	int step_current;
	int step_end;
	int length;
	int score;
	char location[100];
	struct PCB_struct *next;
	int pages[FRAMESIZE];
};