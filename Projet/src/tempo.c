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


unsigned long max(unsigned long a, unsigned long b)
{
	if (a > b)
		return a;
	return b;
}

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
		root_event = e;

	
	
	if (current_event != NULL )
	{
		if (e->done_time > current_event->done_time)
		{
			current_event->next = e;
		}
		else 
		{
			e->next = current_event;
			if (prev_event != NULL)
			{
				prev_event->next = e;
			}else{
				root_event = e;
			}
		}
	}

}

void handler()
{
	//fprintf(stderr, "%ld\n", pthread_self());
	pthread_mutex_lock(&lock);
	unsigned long date = root_event->done_time;
	while(root_event != NULL && root_event->done_time/1000 == date/1000)
	{
		sdl_push_event(root_event->param_event);
		event prev_event = root_event;
		if (root_event->next != NULL)
			root_event = root_event->next;
		else
			root_event = NULL;
		
		free(prev_event);
	}
		if (root_event != NULL)
		{
			struct itimerval timer;
			timer.it_interval.tv_sec = 0;
			timer.it_interval.tv_usec = 0;
			unsigned long t = max(0, root_event->done_time);
			timer.it_value.tv_sec = (t - get_time()) / 1000000;
			timer.it_value.tv_usec = ((t - get_time()) % 1000000);
			setitimer(ITIMER_REAL, &timer, NULL);
		}

	pthread_mutex_unlock(&lock);
}

void daemon()
{
	//fprintf(stderr, "%ld\n", pthread_self());
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGALRM);

	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	while(1)
	{
		sigaction(SIGALRM, &sa, NULL);
		sigsuspend(&set);
	}
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
	//fprintf(stderr, "%ld\n", pthread_self());
	pthread_t d;
	root_event = NULL;
	pthread_create(&d, NULL, (void*) &daemon, NULL);
	return 1;
}


void timer_set (Uint32 delay, void *param)
{
	pthread_mutex_lock(&lock);
	event e = (event) malloc(sizeof(struct s_event));
	e->param_event = param;
	e->done_time = get_time() + delay * 1000;
	e->timer = delay;
	e->next = NULL;
	add_new_event(e);
	
	struct itimerval timer;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;
	unsigned long t = max(0, root_event->done_time);
			timer.it_value.tv_sec = (t - get_time()) / 1000000;
			timer.it_value.tv_usec = ((t - get_time()) % 1000000);
			setitimer(ITIMER_REAL, &timer, NULL);
	pthread_mutex_unlock(&lock);
}

#endif
