/*
 * Modified by: Rajesh Sakhamuru
 * Version: 6/4/2019
 *
 * manage_threads.c
 *
 * This program is a very simple demonstration of
 * thread scheduling. After creating the specified
 * number of threads, it manages the threads round-
 * robin fashion (0, 1, 2, .. n-1, 0, 1, 2, ..),
 * selecting each thread in turn. That selected
 * thread's function prints a running message at
 * 1-second intervals during its time slice, while
 * other threads silently loop and wait their
 * turn. A SIGINT handler sets a running flag to
 * false, causing the threads to terminate and
 * thread management to complete. It then waits
 * for the threads to terminate before exiting.
 *
 * @since 2019-05-29
 * @author Philip Gust
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

/** Thread currently selected */
volatile pthread_t selected_thread = 0; // used by multiple threads

/** Flag used to indicate whether to continue running */
volatile bool running = true;  // used by multiple threads

/**
 * Handle SIGINT as request to stop managing threads.
 * Prints status message that the signal was received
 * and sets running to false.
 *
 * @param sig the signal (unused)
 */
void SIGINT_handler(int sig)
{
	// your code here
	printf("Received SIGINT: notifying threads to stop.\n");
	running = false;
}

/**
 * This thread function sleeps for 1-sec intervals
 * while running is true. If this is the selected
 * thread, it also prints the pid and its thread
 * number to stdout during each interval. Before
 * returning, it prints a termination message with
 * its thread number, then returns NULL to indicate
 * that nothing is returned.
 *
 * @param param pointer-encoded thread number
 * 	  (NULL+thread_no)
 * @return NULL (unused)
 */
void *runner(void *param)
{
	pid_t myPid = getpid();
	int tid = param - NULL;
	pthread_t tidKey = tid + NULL;

	if (tid == 0)
	{
		sleep(1);
	}

	while (running)
	{
		if (running == false)
		{
			break;
		}
		if (tidKey == selected_thread)
		{
			printf("pid %d: thread %d is running.\n", myPid, tid);
			sleep(1);
		}
	}
	pthread_exit(NULL);
}

/**
 * Create nthread threads on the runner function
 * and places them in the threads array. Passes a
 * pointer-encoded thread number (NULL+thread_no)
 * as data to each created thread. Stops creating
 * if pthread_create returns non-zero result.
 *
 * @param nthreads number of threads to create
 * @param threads the created threads
 * @return number of threads actually created
 */
unsigned int createThreads(unsigned int nthreads, pthread_t threads[])
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int count = 0;

	sleep(1);
	for (int i = 0; i < nthreads; i++)
	{
		int status = pthread_create(&threads[i], &attr, runner, NULL + i);
		if (status != 0)
		{
			fprintf(stderr, "thread not created: error=%d", status);
			return EXIT_FAILURE;
		}
		count++;
	}

	return count;
}

/**
 * Manage threads in round-robin fashion
 * (0, 1, 2, .. n-1, 0, 1, 2, ..) until
 * running is false. For each each thread,
 * set selected_thread, and let it run for
 * timeslice 1-second intervals (sleep(1)),
 * or until running is false.
 *
 * @param timeslice the time each thread runs
 * @param nthreads number of threads to manage
 * @param threads threads to manage
 */
void manageThreads(unsigned int timeslice, unsigned int nthreads,
		const pthread_t threads[])
{
	while (running)
	{
		for (int i = 0; i < nthreads; i++)
		{
			if (running == false)
			{
				break;
			}
			selected_thread = i + NULL;
			for (int t = 0; t < timeslice; t++)
			{
				if (running == false) // if SIGINT called during timeslice
				{
					break;
				}
				sleep(1);
			}
		}
	}
}

/**
 * Wait for the threads to terminate, and
 * returns then number of threads that did
 * terminate with pthread_join() status 0.
 *
 * @param nthreads number of threads to wait on
 * @param threads threads to wait on
 */
unsigned int waitForThreads(unsigned int nthreads, const pthread_t threads[])
{
	int count = 0;
	for (int i = 0; i < nthreads; i++)
	{
		printf("pid %d: thread %d terminating\n", getpid(), i);
		int err = pthread_join(threads[i], NULL);
		if (err != 0)
		{
			printf("ERROR CREATING THREAD: pthread_join error return: %d\n",
					err);
			break;
		}
		printf("thread %d exited with status %d\n", i, err);
		count++;
	}

	return count; // temporary
}

/**
 * Creates a number of threads indicated by
 * argv[1], manages threads in a round-robin
 * fashion. Management stops and the threads
 * terminate on receiving a SIGINT which sets
 * running to false.
 *
 * @param argc the number of arguments (must be 2)
 * @param argv the arguments
 * 	(argv[1] is the number of threads to create)
 */
int main(int argc, char *argv[])
{
	// buffer stdio by line
	setlinebuf(stdout);

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s n_threads\n", argv[0]);
		return EXIT_FAILURE;
	}

	// get number of threads to create
	int nthreads = atoi(argv[1]);
	if (nthreads <= 0)
	{
		fprintf(stderr, "n_threads must be > 0\n");
		return EXIT_FAILURE;
	}

	// handler to set running flag false
	signal(SIGINT, SIGINT_handler);

	// create threads to schedule
	printf("Creating %d threads\n", nthreads);
	pthread_t threads[nthreads];
	int ncreated = createThreads(nthreads, threads);
	if (ncreated < nthreads)
	{
		printf("Only %d of %d threads created -- terminating\n", ncreated,
				nthreads);
		return EXIT_FAILURE;
	}

	// manage threads
	printf("Managing %d threads\n", nthreads);
	manageThreads(3, nthreads, threads);  // each gets ~3 secs

	// wait for threads to terminate
	printf("Waiting for %d threads to terminate\n", nthreads);
	int nterm = waitForThreads(nthreads, threads);

	// report success of all threads terminated
	printf("%d threads have terminated -- exiting\n", nterm);
	return (nterm == nthreads) ? EXIT_SUCCESS : EXIT_FAILURE;
}
