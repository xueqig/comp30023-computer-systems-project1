#include <stdio.h>
#include <stdlib.h>

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
void dequeue(queue_t *q);
void print_queue(queue_t *q);

int main()
{
    queue_t *q = init_queue(0);
    enqueue(q, 1, 5, 0, 30, 'n');
    enqueue(q, 2, 2, 0, 30, 'n');
    enqueue(q, 0, 1, 0, 30, 'n');
    dequeue(q);
    print_queue(q);
    printf("proc_rem: %d\n", q->proc_rem);
    printf("tot_rem_time: %d\n", q->tot_rem_time);
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
    temp->id = id;
    temp->rem_time = rem_time;
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

void dequeue(queue_t *q)
{
    // If queue is empty, return NULL.
    if (q->head == NULL)
    {
        return;
    }

    // Store previous head and move head one node ahead
    node_t *temp = q->head;
    q->head = q->head->next;

    q->tot_rem_time -= temp->rem_time;
    q->proc_rem--;
    q->is_occup = 1;

    free(temp);
}

void print_queue(queue_t *q)
{
    node_t *start = q->head;

    if (q->head == NULL)
    {
        printf("queue is empty\n");
        return;
    }

    while (start->next != NULL)
    {
        printf("id: %d, rem_time: %d\n", start->id, start->rem_time);
        start = start->next;
    }
    // print the last node in the queue
    printf("id: %d, rem_time: %d\n", start->id, start->rem_time);
}
