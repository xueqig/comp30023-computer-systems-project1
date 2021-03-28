#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

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
    int tot_tat;    // total turnaround time
    double max_toh; // max time overhead
    double tot_toh; // total time overhead

} cpu_t;

cpu_t *init_queue();
process_t *new_process(int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc);
void enqueue(cpu_t *cpu, int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc);
void dequeue(cpu_t *cpu, int cur_time);
int run_process(cpu_t *cpu, int cur_time);
void sort_cpu_idx(cpu_t *cpus[], int index[], int num_cpus);
void print_queue(cpu_t *cpu);

int main(int argc, char **argv)
{
    int option, num_cpus;
    FILE *input_file;
    char buff[200];

    int proc_data[100][4];
    int nth_proc = 0;
    int tot_num_proc = 0;
    int tot_num_fin_proc = 0;
    int tot_num_fin_proc_and_sub_proc = 0;
    int fin_proc_and_sub_proc[100];

    int tot_tat = 0;
    double tot_toh = 0;
    double max_toh = 0;

    int i, j;

    int cur_time = 0;

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

    cpu_t *cpus[num_cpus];
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
            int indexes[num_cpus];
            // sort cpu indexes based on total remaining time
            sort_cpu_idx(cpus, indexes, num_cpus);
            if (proc_data[nth_proc][3] == 'n')
            {
                enqueue(cpus[indexes[0]], proc_data[nth_proc][0], proc_data[nth_proc][1], proc_data[nth_proc][1], proc_data[nth_proc][2], proc_data[nth_proc][2], proc_data[nth_proc][3], 0);
            }
            else
            {
                for (i = 0; i < 2; i++)
                {
                    enqueue(cpus[indexes[i]], proc_data[nth_proc][0], proc_data[nth_proc][1], proc_data[nth_proc][1] + (i * 0.1), proc_data[nth_proc][2], ceil((double)proc_data[nth_proc][2] / 2) + 1, proc_data[nth_proc][3], 2);
                }
            }
            nth_proc++;
        }

        // execute process
        for (i = 0; i < num_cpus; i++)
        {
            // check if the head process is finished
            if (cpus[i]->head && cpus[i]->head->rem_time == 0)
            {
                fin_proc_and_sub_proc[tot_num_fin_proc_and_sub_proc++] = cpus[i]->head->pid;

                if (cpus[i]->head->is_par == 'n')
                {
                    tot_num_fin_proc++;

                    // update tah, toh and max_toh
                    int tat = cur_time - cpus[i]->head->arr_time;
                    double toh = roundf(((double)tat / cpus[i]->head->exe_time) * 100) / 100;

                    printf("pid=%d,tat=%d,toh=%.3f\n", cpus[i]->head->pid, tat, toh);

                    tot_tat += tat;
                    tot_toh += toh;

                    if (toh > max_toh)
                    {
                        max_toh = toh;
                    }
                    // printf("tot_num_fin_proc: %d\n", tot_num_fin_proc);
                    // printf("tot_num_proc: %d\n", tot_num_proc);
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", cur_time, cpus[i]->head->pid, nth_proc - tot_num_fin_proc);
                    dequeue(cpus[i], cur_time);
                    // check if all subprocesses are finished
                }
                else
                {
                    int num_fin_sub_proc = 0;
                    for (j = 0; j < 100; j++)
                    {
                        if (fin_proc_and_sub_proc[j] == cpus[i]->head->pid)
                        {
                            num_fin_sub_proc++;
                        }
                    }
                    if (num_fin_sub_proc == cpus[i]->head->num_sub_proc)
                    {
                        tot_num_fin_proc++;
                        // update tah, toh and max_toh
                        int tat = cur_time - cpus[i]->head->arr_time;
                        double toh = roundf(((double)tat / cpus[i]->head->exe_time) * 100) / 100;

                        printf("pid=%d,tat=%d,toh=%.3f\n", cpus[i]->head->pid, tat, toh);

                        tot_tat += tat;
                        tot_toh += toh;

                        if (toh > max_toh)
                        {
                            max_toh = toh;
                        }
                        // printf("tot_num_fin_proc: %d\n", tot_num_fin_proc);
                        // printf("tot_num_proc: %d\n", tot_num_proc);
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", cur_time, cpus[i]->head->pid, nth_proc - tot_num_fin_proc);
                    }
                    dequeue(cpus[i], cur_time);
                }
            }
        }
        for (i = 0; i < num_cpus; i++)
        {
            run_process(cpus[i], cur_time);
        }
        // if (cur_time < 70)
        // {
        //     printf("cur_time: %d\n", cur_time);
        // }
        cur_time++;
    }

    // for (i = 0; i < num_cpus; i++)
    // {
    //     tot_tat += cpus[i]->tot_tat;
    //     tot_toh += cpus[i]->tot_toh;
    //     if (cpus[i]->max_toh > max_toh)
    //     {
    //         max_toh = cpus[i]->max_toh;
    //     }
    // }

    printf("Turnaround time %.f\n", ceil((double)tot_tat / tot_num_proc));
    printf("Time overhead %.2f %.2f\n", max_toh, roundf(tot_toh / tot_num_proc * 100) / 100);
    printf("Makespan %d\n", cur_time - 1);
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

void enqueue(cpu_t *cpu, int arr_time, int pid, double sub_pid, int exe_time, int rem_time, char is_par, int num_sub_proc)
{
    // printf("pid=%d add to cpu=%d\n", pid, cpu->id);

    process_t *start = cpu->head;

    // Create a new process
    process_t *temp = new_process(arr_time, pid, sub_pid, exe_time, rem_time, is_par, num_sub_proc);

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

    // int tat = cur_time - cpu->head->arr_time;
    // double toh = roundf(((double)tat / cpu->head->exe_time) * 100) / 100;

    // cpu->tot_tat += tat;
    // cpu->tot_toh += toh;

    // if (toh > cpu->max_toh)
    // {
    //     cpu->max_toh = toh;
    // }

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

    // if (cur_time < 70)
    // {
    //     printf("run cpu: %d\n", cpu->id);
    // }

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
            j = j - 1;
        }
        rem_time[j + 1] = key1;
        indexes[j + 1] = key2;
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
