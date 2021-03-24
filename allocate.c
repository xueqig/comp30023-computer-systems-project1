#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct process
{
    int arr_time;
    int id;
    int exe_time;
    int rem_time;
    char is_par;
} process_t;

process_t create_process(char str[]);
int get_arr_time(char str[]);

int main(int argc, char **argv)
{
    int option, proessors;
    FILE *input_file;
    char buff[200];

    char process_data[100][100];
    int num_process_data = 0;

    int time = 0;

    while ((option = getopt(argc, argv, "p:f:")) != -1)
        switch (option)
        {
        case 'p':
            proessors = atoi(optarg);
            printf("proessors = %d\n", proessors);
            break;
        case 'f':
            if ((input_file = fopen(optarg, "r")))
            {
                while (fgets(buff, 200, (FILE *)input_file))
                {
                    strcpy(process_data[num_process_data++], buff);
                }
            }
        default:
            return 0;
        }
    return 0;
}

int get_arr_time(char str[])
{
    char *token;
    token = strtok(str, " ");
    return atoi(token);
}

process_t create_process(char str[])
{
    process_t process;
    char *token;
    int tokens[4];
    int num_tokens = 0;

    // breaks the string str into multiple tokens using space
    token = strtok(str, " ");

    while (token != NULL)
    {
        // the first three items are number
        if (num_tokens < 3)
        {
            tokens[num_tokens++] = atoi(token);
        }
        // the last item is a char
        else
        {
            tokens[num_tokens++] = token[0];
        }
        token = strtok(NULL, " ");
    }

    process.arr_time = tokens[0];
    process.id = tokens[1];
    process.exe_time = tokens[2];
    process.rem_time = tokens[2];
    process.is_par = tokens[3];
    return process;
}

void print_process(int num_process, process_t processes[])
{
    int j = 0;
    printf("num_process %d\n", num_process);
    while (j < num_process)
    {
        process_t cur_process = processes[j];
        printf("arr_time: %d, id: %d, exe_time: %d, is_par: %c\n", cur_process.arr_time, cur_process.id, cur_process.exe_time, cur_process.is_par);
        j++;
    }
}
