#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// typedef struct
// {
//     int arr_time;
//     int id;
//     int exe_time;
//     char is_par;
// } process_t;

int main(int argc, char **argv)
{
    char *filename;
    int option, proessors;

    opterr = 0;

    while ((option = getopt(argc, argv, "p:f:")) != -1)
        switch (option)
        {
        case 'p':
            proessors = atoi(optarg);
            printf("proessors = %d\n", proessors);
            break;
        case 'f':
            filename = optarg;
            printf("filename = %s\n", filename);
            break;
        default:
            return 0;
        }
    return 0;
}