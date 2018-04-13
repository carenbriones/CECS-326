/* shmc1.cpp */

#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <semaphore.h>

using namespace std;

// Pointer to class struct
CLASS *class_ptr;

// Substitutes to "/semaphore" when SEM_NAME is used
#define SEM_NAME "/semaphore"

// Memory pointer
void *memptr;
// Process name
char *pname;
// Shared memory id, return value
int	shmid, ret;

// Prototypes
void rpterror(char *), srand(), perror(), sleep();
void sell_seats();

int main(int argc, char* argv[])
{
	// If argument count is less than 2
	if (argc < 2) {
		// Formatted output of first argument in argv is written to stderr
		fprintf (stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}

	// First argument in argument vector is assigned to pname
	pname = argv[0];
	// Read formatted data from second argument in argument vector
	sscanf (argv[1], "%d", &shmid);
	// Maps shared memory of shared memory id into address space specified by the system
	// Address that it is stored in is stored into memptr
	memptr = shmat (shmid, (void *)0, 0);
	// If address is -1, memory was not mapped
	if (memptr == (char *)-1 ) {
		// Displays error message
		rpterror ("shmat failed");
		exit(2);
	}
	// Places the memory segment into the class pointer
	class_ptr = (struct CLASS *)memptr;
	// Sells seats for the process
	sell_seats();
	// Unmaps the memory segment at the memory pointer,
	// stores 0 in ret if unmapping was successful
	ret = shmdt(memptr);

	// Unlinks the semaphore because it isn't used at this point
	sem_unlink(SEM_NAME);
	exit(0);
}

void sell_seats()
{
	//
	int all_out = 0;
	// Sets getpid() as seed for sequence of pseudo-random numbers
	srand ( (unsigned) getpid() );

	// Opens semaphore with SEM_NAME for read-write access
	sem_t *sem = sem_open(SEM_NAME, O_RDWR);

	// Displays error message if opening semaphore failed
	if (sem == SEM_FAILED)
	{
		cout << "Opening child process semaphore failed." << endl;
	}

	while ( !all_out) { /* loop to sell all seats */
		// Waits for a semaphore to be available; uses and decrements it
		sem_wait(sem);

		// If the number of seats left is greater than zero,
		if (class_ptr->seats_left > 0) {
			// Suspends execution for a time from 1-5 seconds
			sleep ( (unsigned)rand()%5 + 1);
			// Decrements the number of seats left
			class_ptr->seats_left--;
			// Suspends execution for a time from 1-5 seconds
			sleep ( (unsigned)rand()%5 + 1);
			// Displays that the program sold a seat, displays
			// number of seats left
			cout << pname << " SOLD SEAT -- "
					<< class_ptr->seats_left << " left" << endl;
		}

		else {
			// Increments all_out to exit loop when there are no seats
			all_out++;
			// Displays that there are no seats left
			cout << pname << " sees no seats left" << endl;
		}
		// Suspends execution for a time from 1-10 seconds
		sleep ( (unsigned)rand()%10 + 1);

		// Increments semaphore
		sem_post(sem);
	}
}

void rpterror(char* string)
{
	// Declaration of character array where error message will be stored
	char errline[50];
	// Stores formatted string of error into errline
	sprintf (errline, "%s %s", string, pname);
	// Displays system error message
	perror (errline);
}
