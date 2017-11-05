#ifndef Server_H_
#define Server_H
typedef struct {
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;

#endif // Server_H
