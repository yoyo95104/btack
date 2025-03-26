#ifndef THREADS_H
#define THREADS_H

#include <bluetooth/bluetooth.h>

typedef struct{
	bdaddr_t addr;
	char message;
	int thread_id;
}thread_args_t;

void *thread_function(void *arg);
int run_threads(bdaddr_t *addr , const char msg);

#endif
