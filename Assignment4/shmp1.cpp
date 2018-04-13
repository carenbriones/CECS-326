/* shmp1.cpp */

#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

// Class number is 1001, Date is 120186, Title is "Operating Systems",
// and seats left starts at 15
CLASS myclass = { "1001", "120186", "Operating Systems", 15 };

// Substitutes to 3 whenever NCHILD is used
#define NCHILD	3
// Substitutes to "/semaphore" when semName is used
#define SEM_NAME "/lel"

// Prototypes
int	shm_init( void * );
void	wait_and_wrap_up( int [], void *, int );
void	rpterror( char *, char * );

int main(int argc, char *argv[])
{
	// Declaration of int array size 3, counter, shared memory id, and semaphore id
	int child[NCHILD], i, shmid, semid;
	// Declaration of shared memory pointer
	void	*shm_ptr;
	// Declaration of asynchronous shared memory id, asynchronous semaphore id, and process name
	char	ascshmid[10], ascsemid[10], pname[14];

	sem_t *sem;
	// Copies first value in argv into pname
	strcpy(pname, argv[0]);
	// Initializes the shared memory into the address of the pointer, stores id into shared memory id
	shmid = shm_init(shm_ptr);
	// Stores the formatted shared memory id into asynchronous shared memory id
	sprintf(ascshmid, "%d", shmid);
	// Creates a semaphore if a semaphore with SEM_NAME doesn't already exist,
	// Allows creator and group associated to open semaphore in read and write mode
	sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 1);

	if (sem == SEM_FAILED)
	{
		cout << "Opening semaphore failed." << endl;
	}

	// Will loop 3 times to make 3 different processes
	for (i = 0; i < NCHILD; i++) {
		// Creation of a child process
		child[i] = fork();
		switch (child[i]) {

		// If no child process is created
		case -1:
			// Displays an error message
			rpterror ("fork failure", pname);
			exit(1);

			// If fork was successful
		case 0:
			// Writes the formatted number into pname
			sprintf (pname, "shmc%d", i+1);
			// Child process is replaced with shmc1
			execl("shmc1", pname, ascshmid, (char *)0);
			// Displays an error if child process's program is not replaced
			perror ("execl failed");
			exit (2);
		}
	}
	// Waits until child exits and removes/destroys child
	wait_and_wrap_up (child, shm_ptr, shmid);

	// Unlinks and destroys semaphore after it's done being used
	sem_unlink(SEM_NAME);
	sem_destroy(sem);
}

int shm_init(void *shm_ptr)
{
	int	shmid;
	// Gets shared memory id--shared memory segment is created in this case
	// ftok() created an IPC identifier, shmget() returns a shared memory id from that,
	// and that shared memory id is stored into shmid
	shmid = shmget(ftok(".",'u'), sizeof(CLASS), 0600 | IPC_CREAT);

	// If shmget() did not return a shared memory id; can fail for many reasons
	if (shmid == -1) {
		// Displays error message
		perror ("shmget failed");
		exit(3);
	}

	// Maps shared memory segment associated with shmid into an address space
	// chosen by the system itself
	shm_ptr = shmat(shmid, (void * ) 0, 0);

	// If shmat() did not return an address space; can fail for many reasons
	if (shm_ptr == (void *) -1) {
		// Displays error message
		perror ("shmat failed");
		exit(4);
	}

	// Copies memory area in myclass into shm_ptr
	memcpy (shm_ptr, (void *) &myclass, sizeof(CLASS) );

	// Returns shared memory id of memory that is initialized
	return (shmid);
}

void wait_and_wrap_up(int child[], void *shm_ptr, int shmid)
{
	// Declaration of waiting process, counter, and
	// active child begins at 3
	int wait_rtn, w, ch_active = NCHILD;

	// While the active child number is greater than 0
	// or while there still is an active child
	while (ch_active > 0) {
		// Blocks the calling process until a child exits or a signal is received
		wait_rtn = wait( (int *)0 );

		// Loops 3 times
		for (w = 0; w < NCHILD; w++)
			// If process id matches terminated process id
			if (child[w] == wait_rtn) {
				// Decrements the active child
				ch_active--;
				break;
			}
	}

	// Displays that parent is being removed from shared memory
	cout << "Parent removing shm" << endl;
	// Unmaps the shared memory segment mapped at the shm_ptr address
	shmdt (shm_ptr);
	// Removes and destroys shared memory segment of shmid
	shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);
	exit (0);
}

void rpterror(char *string, char *pname)
{
	// Declaration of character array where error message will be stored
	char errline[50];
	// Stores formatted error string into errline
	sprintf (errline, "%s %s", string, pname);
	// Displays error message
	perror (errline);
}
