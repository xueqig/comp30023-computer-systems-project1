/* 
COMP30023 Project 1. 
The program is a simulator that allocates CPU to the running processes.
The program is written by Xueqi Guan, studentID: 1098403. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define NUM_DATA_TYEP 4
#define ARR_TIME_IDX 0
#define PID_IDX 1
#define EXE_TIME_IDX 2
#define IS_PAR_IDX 3
#define NUM_PROC 1000
#define BUF_LEN 100

typedef struct process
{
    int arr_time;
    int pid;
    double sub_pid;
    int exe_time;
    int rem_time;
    char is_par;
    int num_sub_proc;
    struct process *next;
} process_t;

typedef struct
{
    process_t *head;
    int id;
    int cur_pid;
    int tot_rem_time;
    int proc_rem;
    int num_fin_proc;
    int tot_tat;
    double max_toh;
    double tot_toh;
} cpu_t;

int read_input_file(int proc_data[][NUM_DATA_TYEP]);
cpu_t *init_cpu();
process_t *new_process(int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc);
void add_proc(cpu_t *cpu, int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc);
void rmv_proc(cpu_t *cpu, int cur_time);
int cal_num_sub_proc(int cust_skd, int num_cpus, int exe_time);
int cal_num_fin_proc(int num_cpus, cpu_t *cpus[], int fin_proc_and_sub_proc[], int tot_num_fin_proc_and_sub_proc);
int run_process(cpu_t *cpu, int cur_time);
int check_proc_fin(int fin_proc_and_sub_proc[], int pid, int num_sub_proc, char is_par);
void sort_proc_data(int proc_data[][NUM_DATA_TYEP], int tot_num_proc);
void sort_cpu_idx(cpu_t *cpus[], int index[], int num_cpus);
void print_cpu(cpu_t *cpu);

int main(int argc, char **argv)
{
    int option, num_cpus, i, j;
    int cust_skd = 0, tot_tat = 0, cur_time = 0;
    int nth_proc = 0, tot_num_proc = 0, tot_num_fin_proc = 0, tot_num_fin_proc_and_sub_proc = 0;
    int fin_proc_and_sub_proc1[NUM_PROC], fin_proc_and_sub_proc2[NUM_PROC];
    int proc_data[NUM_PROC][NUM_DATA_TYEP];
    double tot_toh = 0, max_toh = 0;

    while ((option = getopt(argc, argv, "p:f:c:")) != -1)
    {
        switch (option)
        {
        case 'p':
            num_cpus = atoi(optarg);
            break;
        case 'f':
            // read input file and store processes data in proc_data
            tot_num_proc = read_input_file(proc_data);
        case 'c':
            // implement customised schedule if -c flag is provided
            cust_skd = 1;
        default:
            break;
        }
    }

    // sort proc data
    sort_proc_data(proc_data, tot_num_proc);

    cpu_t *cpus[num_cpus];
    for (i = 0; i < num_cpus; i++)
    {
        cpus[i] = init_cpu(i);
    }

    for (i = 0; i < NUM_PROC; i++)
    {
        fin_proc_and_sub_proc1[i] = -1;
        fin_proc_and_sub_proc2[i] = -1;
    }

    while (tot_num_fin_proc < tot_num_proc)
    {
        // add process to cpu
        while (nth_proc < tot_num_proc && proc_data[nth_proc][ARR_TIME_IDX] == cur_time)
        {
            int indexes[num_cpus];
            // sort cpu indexes based on total remaining time
            sort_cpu_idx(cpus, indexes, num_cpus);
            if (proc_data[nth_proc][IS_PAR_IDX] == 'n')
            {
                add_proc(cpus[indexes[0]], proc_data[nth_proc][ARR_TIME_IDX], proc_data[nth_proc][PID_IDX], proc_data[nth_proc][PID_IDX], proc_data[nth_proc][EXE_TIME_IDX], proc_data[nth_proc][EXE_TIME_IDX], proc_data[nth_proc][IS_PAR_IDX], 0);
            }
            else
            {
                // calculate how many subprocesses
                int k = cal_num_sub_proc(cust_skd, num_cpus, proc_data[nth_proc][EXE_TIME_IDX]);
                for (i = 0; i < k; i++)
                {
                    add_proc(cpus[indexes[i]], proc_data[nth_proc][ARR_TIME_IDX], proc_data[nth_proc][PID_IDX], proc_data[nth_proc][PID_IDX] + (i * 0.1), proc_data[nth_proc][EXE_TIME_IDX], ceil((double)proc_data[nth_proc][EXE_TIME_IDX] / k) + 1, proc_data[nth_proc][IS_PAR_IDX], k);
                }
            }
            nth_proc++;
        }

        // update total number of finished processes
        tot_num_fin_proc += cal_num_fin_proc(num_cpus, cpus, fin_proc_and_sub_proc1, tot_num_fin_proc_and_sub_proc);

        // execute process
        for (i = 0; i < num_cpus; i++)
        {
            // check if the head process is finished
            if (cpus[i]->head && cpus[i]->head->rem_time == 0)
            {
                fin_proc_and_sub_proc2[tot_num_fin_proc_and_sub_proc++] = cpus[i]->head->pid;

                if (check_proc_fin(fin_proc_and_sub_proc2, cpus[i]->head->pid, cpus[i]->head->num_sub_proc, cpus[i]->head->is_par))
                {
                    // update tah, toh and max_toh
                    int tat = cur_time - cpus[i]->head->arr_time;
                    double toh = roundf(((double)tat / cpus[i]->head->exe_time) * 100) / 100;

                    tot_tat += tat;
                    tot_toh += toh;

                    if (toh > max_toh)
                    {
                        max_toh = toh;
                    }
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", cur_time, cpus[i]->head->pid, nth_proc - tot_num_fin_proc);
                }

                rmv_proc(cpus[i], cur_time);
            }
        }

        for (i = 0; i < num_cpus; i++)
        {
            run_process(cpus[i], cur_time);
        }
        cur_time++;
    }

    printf("Turnaround time %.f\n", ceil((double)tot_tat / tot_num_proc));
    printf("Time overhead %.2f %.2f\n", max_toh, roundf(tot_toh / tot_num_proc * 100) / 100);
    printf("Makespan %d\n", cur_time - 1);
    return 0;
}

/* read input file and return the total number of processes */
int read_input_file(int proc_data[][NUM_DATA_TYEP])
{
    FILE *input_file;
    char buff[BUF_LEN];
    int tot_num_proc = 0;

    if ((input_file = fopen(optarg, "r")))
    {
        // read input file and store processes data in proc_data
        while (fgets(buff, 200, (FILE *)input_file))
        {
            char *token;
            int num_tokens = 0;

            // breaks the string str into multiple tokens using space
            token = strtok(buff, " ");

            while (token != NULL)
            {
                // the first three items are number
                if (num_tokens < 3)
                {
                    proc_data[tot_num_proc][num_tokens++] = atoi(token);
                }
                // the last item is a char
                else
                {
                    proc_data[tot_num_proc][num_tokens++] = token[0];
                }
                token = strtok(NULL, " ");
            }
            tot_num_proc++;
        }
        fclose(input_file);
    }
    return tot_num_proc;
}

/* initialise cpu */
cpu_t *init_cpu(int id)
{
    cpu_t *cpu = (cpu_t *)malloc(sizeof(cpu_t));
    cpu->head = NULL;
    cpu->id = id;
    cpu->cur_pid = -1;
    cpu->tot_rem_time = 0;
    cpu->proc_rem = 0;
    cpu->num_fin_proc = 0;
    cpu->tot_tat = 0;
    cpu->max_toh = 0;
    cpu->tot_toh = 0;
    return cpu;
}

/* create a new process */
process_t *new_process(int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc)
{
    process_t *temp = (process_t *)malloc(sizeof(process_t));
    temp->arr_time = arr_time;
    temp->pid = pid;
    temp->sub_pid = sub_pid;
    temp->exe_time = exe_time;
    temp->rem_time = rem_time;
    temp->is_par = is_par;
    temp->num_sub_proc = num_sub_proc;
    temp->next = NULL;
    return temp;
}

/* add process to cpu */
void add_proc(cpu_t *cpu, int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc)
{
    process_t *start = cpu->head;

    // create a new process
    process_t *temp = new_process(arr_time, pid, sub_pid, exe_time, rem_time, is_par, num_sub_proc);

    // if cpu is empty, then new process is head
    if (cpu->head == NULL)
    {
        cpu->head = temp;
    }
    // the head has greater rem_time than new process
    // so insert new process before head process and change head process
    else if (cpu->head->rem_time > rem_time || (cpu->head->rem_time == rem_time && cpu->head->pid > pid))
    {
        // insert new process before head
        temp->next = cpu->head;
        cpu->head = temp;
    }
    else
    {
        // traverse the list and find a position to insert new process
        while (start->next != NULL &&
               (start->next->rem_time < rem_time ||
                (start->next->rem_time == rem_time && start->next->pid < pid)))
        {
            start = start->next;
        }

        // either at the ends of the list or at required position
        temp->next = start->next;
        start->next = temp;
    }
    cpu->tot_rem_time += rem_time;
    cpu->proc_rem++;
}

/* remove process from cpu */
void rmv_proc(cpu_t *cpu, int cur_time)
{
    // if cpu is empty, return
    if (cpu->head == NULL)
    {
        return;
    }

    // if cpu is not empty
    cpu->proc_rem--;
    cpu->num_fin_proc++;

    // store previous head and move head one node ahead
    process_t *temp = cpu->head;
    cpu->head = cpu->head->next;

    free(temp);
}

/* calculate number of subprocesses */
int cal_num_sub_proc(int cust_skd, int num_cpus, int exe_time)
{
    int k;
    if (cust_skd == 0)
    {
        for (k = num_cpus; k > 0; k++)
        {
            if (exe_time / k >= 1)
            {
                break;
            }
        }
    }
    else
    {
        k = num_cpus;
    }
    return k;
}

/* calculate number of finished processes */
int cal_num_fin_proc(int num_cpus, cpu_t *cpus[], int fin_proc_and_sub_proc[], int tot_num_fin_proc_and_sub_proc)
{
    int i, num_fin_proc = 0;

    for (i = 0; i < num_cpus; i++)
    {
        if (cpus[i]->head && cpus[i]->head->rem_time == 0)
        {
            fin_proc_and_sub_proc[tot_num_fin_proc_and_sub_proc++] = cpus[i]->head->pid;

            if (check_proc_fin(fin_proc_and_sub_proc, cpus[i]->head->pid, cpus[i]->head->num_sub_proc, cpus[i]->head->is_par))
            {
                num_fin_proc++;
            }
        }
    }
    return num_fin_proc;
}

/* execute process and print execution message */
int run_process(cpu_t *cpu, int cur_time)
{
    if (cpu->proc_rem == 0)
    {
        return 0;
    }

    if (cpu->head->pid != cpu->cur_pid)
    {
        if (cpu->head->is_par == 'n')
        {
            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", cur_time, cpu->head->pid, cpu->head->rem_time, cpu->id);
        }
        else
        {
            printf("%d,RUNNING,pid=%.1f,remaining_time=%d,cpu=%d\n", cur_time, cpu->head->sub_pid, cpu->head->rem_time, cpu->id);
        }
        cpu->cur_pid = cpu->head->pid;
    }
    cpu->head->rem_time--;
    cpu->tot_rem_time--;

    return 1;
}

/* check if process is finished */
int check_proc_fin(int fin_proc_and_sub_proc[], int pid, int num_sub_proc, char is_par)
{
    if (is_par == 'n')
    {
        return 1;
    }

    // if process is parallelisable, need to check if all subprocesses are finished
    int num_fin_sub_proc = 0, i = 0;

    for (i = 0; i < NUM_PROC; i++)
    {
        if (fin_proc_and_sub_proc[i] == pid)
        {
            num_fin_sub_proc++;
        }
    }

    if (num_fin_sub_proc == num_sub_proc)
    {
        return 1;
    }
    return 0;
}

/* sort process data based on execution time and process id */
void sort_proc_data(int proc_data[][NUM_DATA_TYEP], int tot_num_proc)
{
    int i, j, key_arr_time, key_pid, key_exe_time, key_is_par;

    for (i = 1; i < tot_num_proc; i++)
    {
        key_arr_time = proc_data[i][0];
        key_pid = proc_data[i][1];
        key_exe_time = proc_data[i][2];
        key_is_par = proc_data[i][3];

        j = i - 1;

        while (j >= 0 && proc_data[j][0] == key_arr_time && (proc_data[j][2] > key_exe_time || (proc_data[j][2] == key_exe_time && proc_data[j][1] > key_pid)))
        {
            proc_data[j + 1][0] = proc_data[j][0];
            proc_data[j + 1][1] = proc_data[j][1];
            proc_data[j + 1][2] = proc_data[j][2];
            proc_data[j + 1][3] = proc_data[j][3];
            j--;
        }
        proc_data[j + 1][0] = key_arr_time;
        proc_data[j + 1][1] = key_pid;
        proc_data[j + 1][2] = key_exe_time;
        proc_data[j + 1][3] = key_is_par;
    }
}

/* sort cpu based on time left to complete all processes */
void sort_cpu_idx(cpu_t *cpus[], int indexes[], int num_cpus)
{
    int rem_time[num_cpus];
    int i, j, key1, key2;

    for (i = 0; i < num_cpus; i++)
    {
        rem_time[i] = cpus[i]->tot_rem_time;
        indexes[i] = i;
    }

    for (i = 1; i < num_cpus; i++)
    {
        key1 = rem_time[i];
        key2 = indexes[i];
        j = i - 1;

        while (j >= 0 && rem_time[j] > key1)
        {
            rem_time[j + 1] = rem_time[j];
            indexes[j + 1] = indexes[j];
            j--;
        }
        rem_time[j + 1] = key1;
        indexes[j + 1] = key2;
    }
}

/* print out cpu data */
void print_cpu(cpu_t *cpu)
{
    process_t *start = cpu->head;

    if (cpu->head == NULL)
    {
        printf("cpu is empty\n");
        return;
    }

    printf("pid: %d, rem_time: %d, is_par: %c\n", start->pid, start->rem_time, start->is_par);
    while (start->next != NULL)
    {
        start = start->next;
        printf("pid: %d, rem_time: %d, is_par: %c\n", start->pid, start->rem_time, start->is_par);
    }
    printf("id: %d, proc_rem: %d\n", cpu->id, cpu->proc_rem);
}
