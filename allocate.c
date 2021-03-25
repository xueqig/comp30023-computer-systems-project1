#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node
{
    int arr_time;
    int id;
    int exe_time;
    int rem_time;
    char is_par;
    struct node *next;
} node_t;

typedef struct
{
    node_t *head;
    int id;
    int tot_rem_time;
    int proc_rem;
    int is_occup;
} queue_t;

queue_t *init_queue();
node_t *new_node(int arr_time, int id, int exe_time, int rem_time, char is_par);
void enqueue(queue_t *q, int arr_time, int id, int exe_time, int rem_time, char is_par);
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

                queue_t *q = init_queue(0);
                while (nth_process < num_processes_data && processes_data[nth_process][0] == cur_time)
                {
                    enqueue(q, processes_data[nth_process][0], processes_data[nth_process][1], processes_data[nth_process][2], processes_data[nth_process][2], processes_data[nth_process][3]);
                    nth_process++;
                }
                print_queue(q);
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
    q->is_occup = 0;
    return q;
}

node_t *new_node(int arr_time, int id, int exe_time, int rem_time, char is_par)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->arr_time = arr_time;
    temp->id = id;
    temp->exe_time = exe_time;
    temp->rem_time = rem_time;
    temp->is_par = is_par;
    temp->next = NULL;
    return temp;
}

void enqueue(queue_t *q, int arr_time, int id, int exe_time, int rem_time, char is_par)
{
    node_t *start = q->head;

    // Create a new LL node
    node_t *temp = new_node(arr_time, id, exe_time, rem_time, is_par);

    // If queue is empty, then new node is head
    if (q->head == NULL)
    {
        q->head = temp;
    }
    // The head has greater rem_time than new node.
    // So insert new node before head node and change head node.
    else if (q->head->rem_time > rem_time || (q->head->rem_time == rem_time && q->head->id > id))
    {
        // Insert New Node before head
        temp->next = q->head;
        q->head = temp;
    }
    else
    {
        // Traverse the list and find a position to insert new node
        while (start->next != NULL &&
               (start->next->rem_time < rem_time ||
                (start->next->rem_time == rem_time && start->next->id > id)))
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
    node_t *start = q->head;

    if (q->head == NULL)
    {
        printf("queue is empty\n");
        return;
    }

    printf("id: %d, rem_time: %d, is_par: %c\n", start->id, start->rem_time, start->is_par);
    while (start->next != NULL)
    {
        start = start->next;
        printf("id: %d, rem_time: %d, is_par: %c\n", start->id, start->rem_time, start->is_par);
    }
}