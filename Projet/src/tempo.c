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

void handler()
{
	fprintf(stderr, "%ld\n", pthread_self());
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
	pthread_create(&d, NULL, &daemon, NULL);
	return 0; // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
	// TODO
}

#endif
