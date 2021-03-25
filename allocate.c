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
    int tot_rem_time;
    int proc_rem;
} queue_t;

queue_t *init_queue();
process_t *new_process(int arr_time, int pid, int exe_time, int rem_time, char is_par);
void enqueue(queue_t *q, int arr_time, int pid, int exe_time, int rem_time, char is_par);
void print_queue(queue_t *q);

int main(int argc, char **argv)
{
    int option, proessors;
    FILE *input_file;
    char buff[200];

    int processes_data[100][4];
    int num_processes_data = 0;
    int nth_process = 0;

    int cur_time = 0;

    while ((option = getopt(argc, argv, "p:f:")) != -1)
    {
        switch (option)
        {
        case 'p':
            proessors = atoi(optarg);
            printf("proessors = %d\n", proessors);
            break;
        case 'f':
            if ((input_file = fopen(optarg, "r")))
            {
                // read input file and store processes data in processes_data
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
                            processes_data[num_processes_data][num_tokens++] = atoi(token);
                        }
                        // the last item is a char
                        else
                        {
                            processes_data[num_processes_data][num_tokens++] = token[0];
                        }
                        token = strtok(NULL, " ");
                    }
                    num_processes_data++;
                }
                fclose(input_file);

                // add process to queue
                queue_t *q = init_queue(0);
                while (nth_process < num_processes_data && processes_data[nth_process][0] == cur_time)
                {
                    enqueue(q, processes_data[nth_process][0], processes_data[nth_process][1], processes_data[nth_process][2], processes_data[nth_process][2], processes_data[nth_process][3]);
                    nth_process++;
                }
                print_queue(q);

                if (q->proc_rem > 0)
                {
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", cur_time, q->head->pid, q->head->rem_time, q->id);
                    q->head->rem_time--;
                }
            }
        default:
            break;
        }
    }

    return 0;
}

queue_t *init_queue(int id)
{
    queue_t *q = (queue_t *)malloc(sizeof(queue_t));
    q->head = NULL;
    q->id = id;
    q->tot_rem_time = 0;
    q->proc_rem = 0;
    return q;
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

void enqueue(queue_t *q, int arr_time, int pid, int exe_time, int rem_time, char is_par)
{
    process_t *start = q->head;

    // Create a new LL process
    process_t *temp = new_process(arr_time, pid, exe_time, rem_time, is_par);

    // If queue is empty, then new process is head
    if (q->head == NULL)
    {
        q->head = temp;
    }
    // The head has greater rem_time than new process.
    // So insert new process before head process and change head process.
    else if (q->head->rem_time > rem_time || (q->head->rem_time == rem_time && q->head->pid > pid))
    {
        // Insert New process before head
        temp->next = q->head;
        q->head = temp;
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
    q->tot_rem_time += rem_time;
    q->proc_rem++;
}

void print_queue(queue_t *q)
{
    process_t *start = q->head;

    if (q->head == NULL)
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
    printf("id: %d, proc_rem: %d\n", q->id, q->proc_rem);
}