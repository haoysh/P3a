#include "shm_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define SHM_NAME "youn_wangh"
// ADD NECESSARY HEADERS

void* shm_ptr;
stats_t *pstat;
pthread_mutex_t* mutex;

void exit_handler(int sig) {
  // critical section begins
  pthread_mutex_lock(mutex);
  pstat->pid = -1;
  pthread_mutex_unlock(mutex);
	// critical section ends
  exit(1);
}

int main(int argc, char *argv[]) {
  struct timeval  start, end;
  gettimeofday(&start, NULL);    
  if(argc == 1){
    fprintf(stderr, "Error: Needs client name\n");
    exit(1);
  }
  int fd_shm = shm_open(SHM_NAME, O_RDWR, 0); 
  if (fd_shm == -1){
     exit(1);
  }
  void *shm_ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
  if(shm_ptr == MAP_FAILED){
     exit(1);
  }
  
  time_t t = time(NULL);
  struct sigaction act;
  act.sa_handler = exit_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  mutex = (pthread_mutex_t*)shm_ptr;
  char* time_string = ctime(&t);
  int len_of_new_line = strlen(ctime(&t)) - 1;
  time_string[len_of_new_line] = '\0';
  int maxClients = getpagesize()/64 - 1;
  stats_t* pstatch;
  int flag = 0;
  int curr_pid = getpid();
  for(int i = 1; i <= maxClients; i++){
    pstat = (stats_t*)(shm_ptr + i * 64);
    pthread_mutex_lock(mutex);
    if(pstat->pid == -1){
      pstat->pid = curr_pid;
      pthread_mutex_unlock(mutex);
      flag = 1;
      break;
    }
    pthread_mutex_unlock(mutex); 
  }
  if(!flag){
    fprintf(stderr, "Maximum number of clients handled by the server exceeded\n");
    exit(1);
  }
  strcpy(pstat->birth, time_string);
  strcpy(pstat->clientString, argv[1]);
  
  while (1) {
    // Print active clients
    gettimeofday(&end, NULL);
    pstat->elapsed_sec = end.tv_sec - start.tv_sec;
    pstat->elapsed_msec = (double)((end.tv_usec - start.tv_usec) / (double)1000);
    //printf("elapsed sec: %d elapsed msec: %.4f\n", pstat->elapsed_sec, pstat->elapsed_msec);
    sleep(1);
    
    printf("Active clients : ");
    for(int i = 1; i <= maxClients; i++){
      pstatch = (stats_t*)(shm_ptr + i * 64);
      if(pstatch->pid != -1){
        printf("%d ", pstatch->pid);
      }
    }
    printf("\n");
    
  }
  return 0;
}
