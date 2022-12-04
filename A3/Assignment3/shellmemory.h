void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int mem_set_command_init(char *line_num, char *line_command, int num, int newstart);
void add_process_to_list(struct PCB_struct *process);
void remove_process(int target_PID);
void run_process(int target_PID, int amount_instruct);
void resetmem();
