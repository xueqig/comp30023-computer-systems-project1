#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct process
{
    int arr_time;
    int pid;
    int exe_time;
    int rem_time;
    char is_par;
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
    int tot_tat;    // total turnaround time
    double max_toh; // max time overhead
    double tot_toh; // total time overhead

} cpu_t;

cpu_t *init_queue();
process_t *new_process(int arr_time, int pid, int exe_time, int rem_time, char is_par);
void enqueue(cpu_t *cpu, int arr_time, int pid, int exe_time, int rem_time, char is_par);
void dequeue(cpu_t *cpu, int cur_time);
int run_process(cpu_t *cpu, int cur_time);
void sort_cpus(cpu_t *cpus[], int num_cpus);
void print_queue(cpu_t *cpu);

int main(int argc, char **argv)
{
    int option, num_cpus;
    FILE *input_file;
    char buff[200];

    int proc_data[100][4];
    int tot_num_proc = 0;
    int nth_proc = 0;
    int tot_num_fin_proc = 0;

    int i;

    int cur_time = 0;

    cpu_t *cpus[num_cpus];

    while ((option = getopt(argc, argv, "p:f:")) != -1)
    {
        switch (option)
        {
        case 'p':
            num_cpus = atoi(optarg);
            break;
        case 'f':
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
        default:
            break;
        }
    }

    for (i = 0; i < num_cpus; i++)
    {
        cpus[i] = init_queue(i);
    }

    // cpu_t *cpu = init_queue(0);
    while (tot_num_fin_proc < tot_num_proc)
    {
        // add process to queue
        while (nth_proc < tot_num_proc && proc_data[nth_proc][0] == cur_time)
        {
            if (proc_data[nth_proc][3] == 'n')
            {
                sort_cpus(cpus, num_cpus);
                enqueue(cpus[0], proc_data[nth_proc][0], proc_data[nth_proc][1], proc_data[nth_proc][2], proc_data[nth_proc][2], proc_data[nth_proc][3]);
                nth_proc++;
            }
        }

        // execute process
        for (i = 0; i < num_cpus; i++)
        {
            // check if the head process is finished
            if (cpus[i]->head->rem_time == 0)
            {
                tot_num_fin_proc++;
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", cur_time, cpus[i]->head->pid, tot_num_proc - tot_num_fin_proc);
                dequeue(cpus[i], cur_time);
            }
            run_process(cpus[i], cur_time);
        }
        cur_time++;
    }

    // printf("Turnaround time %.0f\n", (double)cpu->tot_tat / cpu->tot_num_proc);
    // printf("Time overhead %.2f %.2f\n", cpu->max_toh, (double)cpu->tot_toh / cpu->tot_num_proc);
    // printf("Makespan %d\n", cur_time);
    return 0;
}

cpu_t *init_queue(int id)
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

process_t *new_process(int arr_time, int pid, int exe_time, int rem_time, char is_par)
{
    process_t *temp = (process_t *)malloc(sizeof(process_t));
    temp->arr_time = arr_time;
    temp->pid = pid;
    temp->exe_time = exe_time;
    temp->rem_time = rem_time;
    temp->is_par = is_par;
    temp->next = NULL;
    return temp;
}

void enqueue(cpu_t *cpu, int arr_time, int pid, int exe_time, int rem_time, char is_par)
{
    process_t *start = cpu->head;

    // Create a new process
    process_t *temp = new_process(arr_time, pid, exe_time, rem_time, is_par);

    // If queue is empty, then new process is head
    if (cpu->head == NULL)
    {
        cpu->head = temp;
    }
    // The head has greater rem_time than new process.
    // So insert new process before head process and change head process.
    else if (cpu->head->rem_time > rem_time || (cpu->head->rem_time == rem_time && cpu->head->pid > pid))
    {
        // Insert New process before head
        temp->next = cpu->head;
        cpu->head = temp;
    }
    else
    {
        // Traverse the list and find a position to insert new process
        while (start->next != NULL &&
               (start->next->rem_time < rem_time ||
                (start->next->rem_time == rem_time && start->next->pid > pid)))
        {
            start = start->next;
        }

        // Either at the ends of the list or at required position
        temp->next = start->next;
        start->next = temp;
    }
    cpu->tot_rem_time += rem_time;
    cpu->proc_rem++;
}

void dequeue(cpu_t *cpu, int cur_time)
{
    // If queue is empty, return
    if (cpu->head == NULL)
    {
        return;
    }

    // if queue is not empty
    cpu->proc_rem--;
    cpu->num_fin_proc++;

    int tat = cur_time - cpu->head->arr_time;
    double toh = (double)tat / cpu->head->exe_time;
    cpu->tot_tat += tat;
    cpu->tot_toh += ((double)tat / cpu->head->exe_time);
    if (toh > cpu->max_toh)
    {
        cpu->max_toh = toh;
    }

    // Store previous head and move head one node ahead
    process_t *temp = cpu->head;
    cpu->head = cpu->head->next;

    free(temp);
}

int run_process(cpu_t *cpu, int cur_time)
{
    if (cpu->proc_rem == 0)
    {
        return 0;
    }

    if (cpu->head->pid != cpu->cur_pid)
    {
        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", cur_time, cpu->head->pid, cpu->head->rem_time, cpu->id);
        cpu->cur_pid = cpu->head->pid;
    }
    cpu->head->rem_time--;
    cpu->tot_rem_time--;

    return 1;
}

void sort_cpus(cpu_t *cpus[], int num_cpus)
{
    int i, j;
    for (i = 0; i < num_cpus; i++)
    {
        for (j = i + 1; j < num_cpus; j++)
        {
            if (cpus[i]->tot_rem_time > cpus[j]->tot_rem_time ||
                (cpus[i]->tot_rem_time == cpus[j]->tot_rem_time && cpus[i]->id > cpus[j]->id))
            {
                cpu_t *temp = cpus[i];
                cpus[i] = cpus[j];
                cpus[j] = temp;
            }
        }
    }
}

void print_queue(cpu_t *cpu)
{
    process_t *start = cpu->head;

    if (cpu->head == NULL)
    {
        printf("queue is empty\n");
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