#include <stdio.h>
#include <pthread.h>

void *my_pthread(void *arg)
{
	int retval = 0;
	pthread_attr_t attr;
	struct sched_param param;
	size_t stacksize;
	int detachstate;
	int scope;
	int inherit;
	int policy;

	if(pthread_attr_init(&attr) == 0)
	{
		if(pthread_attr_getstacksize(&attr, &stacksize) == 0)
		{
			printf("StackSize:%d\n",stacksize);
		}

		if(pthread_attr_getdetachstate(&attr, &detachstate) == 0)
		{
			if(detachstate == PTHREAD_CREATE_JOINABLE)
				printf("Detachstate:PTHREAD_CREATE_JOINABLE\n");
			if(detachstate == PTHREAD_CREATE_DETACHED)
				printf("Detachstate:PTHREAD_CREATE_DETACHED\n");
		}

		if(pthread_attr_getscope(&attr, &scope) == 0)
		{
			if(scope == PTHREAD_SCOPE_PROCESS)
				printf("Scope:PTHREAD_SCOPE_PROCESS\n");
			if(scope == PTHREAD_SCOPE_SYSTEM)
				printf("Scope:PTHREAD_SCOPE_SYSTEM\n");
		}

		if(pthread_attr_getinheritsched(&attr, &inherit) == 0)
		{
			if(inherit == PTHREAD_INHERIT_SCHED)
				printf("InheritSche:PTHREAD_INHERIT_SCHED\n");
			if(inherit == PTHREAD_EXPLICIT_SCHED)
				printf("InheritSche:PTHREAD_EXPLICIT_SCHED\n");
		}

		if(pthread_attr_getschedpolicy(&attr, &policy) == 0)
		{
			if(policy == SCHED_FIFO)
				printf("SchedPolicy:SCHED_FIFO\n");
			if(policy == SCHED_RR)
				printf("SchedPolicy:SCHED_RR\n");
			else
				printf("SchedPolicy:SCHED_OTHER\n");
		}

		if(pthread_attr_getschedparam(&attr, &param) == 0)
		{
			printf("SchedPriority:%d\n",param.sched_priority);
		}

		pthread_attr_destroy(&attr);
	}
	pthread_exit(&retval);
}

int main()
{
	int count;
	pthread_t thread;
	int *retval;

	if(pthread_create(&thread, NULL, my_pthread, (void *)NULL) != 0)
	{
		printf("Count not create thread!\n");
		return -1;
	}

	if(pthread_join(thread, (void**)(&retval)) != 0)
	{
		printf("No thread to join!\n");
		return -2;
	}
	return 0;

}