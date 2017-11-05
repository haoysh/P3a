#include "shm_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

#define SHM_NAME "youn_wangh"
// ADD NECESSARY HEADERS

// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;
void* shm_ptr;
stats_t *pstat;
void exit_handler(int sig) 
{
    // ADD
    //printf("Terminated\n");
    munmap(shm_ptr, getpagesize());
    shm_unlink(SHM_NAME);
    exit(1);
}

int main(int argc, char *argv[]) 
{
  // ADD
  
  // Creating a new shared memory segment
  int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	
  ftruncate(fd_shm, getpagesize());
  void* shm_ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
  // Initializing mutex
  pthread_mutexattr_init(&mutexAttribute);
  pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
  mutex = (pthread_mutex_t*)shm_ptr;
  pthread_mutex_init(mutex, &mutexAttribute);
  int maxClients = getpagesize()/64 - 1;
  struct sigaction act;
  act.sa_handler = exit_handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  int count = 0;
  
  for(int i = 1; i <= maxClients; i++){
    pstat = (stats_t*)(shm_ptr + i * 64);
    pthread_mutex_lock(mutex);
    pstat->pid = -1;
    pthread_mutex_unlock(mutex);
  }
  
  stats_t *pstatch;
  while (1) 
  {
    // ADD
    sleep(1);
    for(int i = 1; i <= maxClients; i++){
      pstatch = (stats_t*)(shm_ptr + i * 64);
      if(pstatch->pid != -1){
        printf("%d, pid : %d, birth : %s, elapsed : %d s %.4f ms, %s\n", count, pstatch->pid, pstatch->birth, pstatch->elapsed_sec, pstatch->elapsed_msec, pstatch->clientString);
      }
    }
    printf("\n");
    count++;
    
    
  }
  
    
    return 0;
}
