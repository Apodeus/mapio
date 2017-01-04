#define _XOPEN_SOURCE 600

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
	struct timeval tv;

	gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
	tv.tv_sec -= 3600UL * 24 * 365 * 46;

	return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

struct s_event{

	void* param_event;
	unsigned long done_time;
	unsigned long timer;

	struct s_event* next;
};

typedef struct s_event* event;

event root_event;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void add_new_event(event e)
{
	event prev_event = NULL;
	event current_event = root_event;
	while (current_event!= NULL && current_event->next != NULL && e->done_time > current_event->done_time)
	{
		prev_event = current_event;
		current_event = current_event->next;
	}

	if (root_event == NULL)
	{
		fprintf(stderr, "root found null\n");
		root_event = e;
	}

	if (prev_event != NULL)
	{
		fprintf(stderr, "%s\n", "prev not null");
		prev_event->next = e;
	}
	
	if (current_event != NULL)
	{
		fprintf(stderr, "%s\n", "current not null");
		e->next = current_event;
	}
}

void handler()
{
	pthread_mutex_lock(&lock);
	unsigned long date = root_event->done_time;
	while(root_event != NULL && root_event->done_time == date)
	{
		fprintf(stderr, "sdl_push_event(%p) appelée au temps %ld\n", root_event->param_event, get_time());
		sdl_push_event(root_event->param_event);
		fprintf(stderr, "Done.\n");
		event prev_event = root_event;
		fprintf(stderr, "prev_event created\n");
		if (root_event->next != NULL)
			root_event = root_event->next;
		else
			root_event = NULL;
		fprintf(stderr, "root moved forward\n");
		free(prev_event);
		fprintf(stderr, "Free done.\n");
	}

	fprintf(stderr, "%s\n", "unlocking...");
	pthread_mutex_unlock(&lock);
	fprintf(stderr, "%s\n", "unlocked");
}

void daemon()
{
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGALRM);

	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	while(1)
	{
		fprintf(stderr, "Running...\n");
		sigaction(SIGALRM, &sa, NULL);
		sigsuspend(&set);
		fprintf(stderr, "___");
	}
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
	pthread_t d;
	//lock = PTHREAD_MUTEX_INITIALIZER;
	root_event = NULL;

	pthread_create(&d, NULL, (void*) &daemon, NULL);



	return 1; // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
	pthread_mutex_lock(&lock);
	event e = (event) malloc(sizeof(struct s_event));
	e->param_event = param;
	e->done_time = get_time() + delay * 1000;
	e->timer = delay;
	e->next = NULL;
	fprintf(stderr, "Event fait avec delay=%u.\n", delay);
	add_new_event(e);
	fprintf(stderr, "Event placé, liste :\n");
	event current_event = root_event;
	while(current_event != NULL && current_event->next != NULL)
	{
		fprintf(stderr, " done_time : %lu;\n", current_event->done_time);
		current_event = current_event->next;
	}
	if (current_event == NULL)
		fprintf(stderr, "%s\n", "<null>");
	fprintf(stderr, "%s\n", "C'est tout");
	struct itimerval timer;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = (root_event->done_time - get_time()) / 1000000;
	timer.it_value.tv_usec = ((root_event->done_time - get_time()) % 1000000);
	fprintf(stderr, "Fixation du timer pour dans %ld s %ld us\n", timer.it_value.tv_sec, timer.it_value.tv_usec);
	fprintf(stderr, "%d\n",setitimer(ITIMER_REAL, &timer, NULL));
	pthread_mutex_unlock(&lock);
}

#endif
