/**Author: Rajesh Sakhamuru
 * Version: May 25, 2018
 *
 * killer.c -
 *     This program, when run concurrently with itself, through user input
 *     will communicate with both instances of itself through a variety of
 *     specified signals sent between the instances.
 *     The possible signals are:
 *	  		1 = SIGTSTP
 *			2 = SIGCONT
 *			3 = SIGTERM
 *			4 = SIGUSR1
 *			5 = SIGUSR2
 *
 *  Created on: May 22, 2018
 *      Author: Rajesh Sakhamuru
 */
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



static pid_t myPid;
static pid_t otherPid;
/** indicates process is running */
static bool running = true;

/**
 * Handle SIGTSTP as request to stop/suspend process. Similar to
 * SIGSTOP, but enables application to intercept and handle the
 * signal.
 */
void SIGTSTP_handler(int sig)
{
	printf("Received SIGTSTP: pausing process %d\n", myPid);
	pause();
}


/**
 * Handle SIGCONT by reprompting the user with potential options for
 * signals to be sent to the other process.
 */
void SIGCONT_handler(int sig)
{
	printf("0 = quit\n1 = SIGTSTP\n2 = SIGCONT\n3 = SIGTERM\n4 = SIGUSR1\n5 = SIGUSR2\n");
	printf("Enter a number between 0 and 5: \n");
}

/**
 * Handle SIGTERM as request to terminate the process normally.
 * Similar to SIGKILL but enables application to intercept and
 * handle the signal.  This handler Sets running to false and
 * terminates the process.
 */
void SIGTERM_handler(int sig)
{
	printf("Received SIGTERM: terminating normally\n");
	running = false;
	exit(EXIT_SUCCESS);
}

/**
 * Handle SIGUSR1, an application specific signal. This handler
 * prints the curent process ID.
 */
void SIGUSR1_handler(int sig)
{
	printf("Recieved SIGUSR1:: My process ID is %d\n", myPid);
}

/**
 * Handle SIGUSR2, an application specific signal. This handler
 * handler prints the other process ID.
 */
void SIGUSR2_handler(int sig)
{
	printf("Received SIGUSR2:: Other process ID is: %d\n", otherPid);
}

int main()
{
	// setbuf(stdout, NULL);
	// Register signal and signal handler
	signal(SIGTSTP, SIGTSTP_handler);
	signal(SIGCONT, SIGCONT_handler);
	signal(SIGTERM, SIGTERM_handler);
	signal(SIGUSR1, SIGUSR1_handler);
	signal(SIGUSR2, SIGUSR2_handler);

	myPid = getpid();
	int userIn = 0;

	printf("My process ID is %d.\n", myPid);
	printf("Enter other process ID: ");
	scanf("%d", &otherPid);
	printf("Other process ID is %d.\n",otherPid);

	while(running)
	{
		printf("0 = quit\n1 = SIGTSTP\n2 = SIGCONT\n3 = SIGTERM\n4 = SIGUSR1\n5 = SIGUSR2\n");
		printf("Enter a number between 0 and 5: \n");
		scanf("%i", &userIn);

		if (userIn <=5 && userIn >=0)
		{
			switch(userIn)
			{
			case 0:
				return EXIT_SUCCESS;
			case 1:
				if (kill(otherPid, SIGTSTP) == -1)
				{
					printf("Process %d no longer available\n", otherPid);
					exit(EXIT_FAILURE);
				}
				break;
			case 2:
				if(kill(otherPid, SIGCONT) == -1)
				{
					printf("Process %d no longer available\n", otherPid);
					exit(EXIT_FAILURE);
				}
				break;
			case 3:
				if (kill(otherPid, SIGTERM) == -1)
				{
					printf("Process %d no longer available\n", otherPid);
					exit(EXIT_FAILURE);
				}
				break;
			case 4:
				if (kill(otherPid, SIGUSR1) == -1)
				{
					printf("Process %d no longer available\n", otherPid);
					exit(EXIT_FAILURE);
				}
				break;
			case 5:
				if (kill(otherPid, SIGUSR2) == -1)
				{
					printf("Process %d no longer available\n", otherPid);
					exit(EXIT_FAILURE);
				}
				break;
			}
		}
	}
}
