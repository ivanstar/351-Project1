#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */
#include <fstream>

using namespace std;
/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* TODO:
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	    2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V objest (i.e. message queues, shared memory, and sempahores)
		    is unique system-wide among all SYstem V objects. Two objects, on the other hand,
		    may have the same key.
	 */

	 ofstream outFile("keyfile.txt");
	 outFile << "Hello world";
	 outFile.close();

/*
	 	 The ftok() function shall return a key based on path and id that is
	 	 usable in subsequent calls to msgget() and shmget().

	 	 key_t - is used to generate a key
*/
	 key_t shm_keygen;
	 shm_keygen = ftok("keyfile.txt", 'a');
	 if(shm_keygen < 0){
	 	cout <<  "Error in ftok key generation." << endl;
	 	exit(1);
	 }
	 else{
	 	cout << "ftok key generation successful!" << endl;
	 }

	 //Task 3
/*
	 	shmget() returns an identifier for the shared memory segment.

	 	The first argument to shmget() is the key value (in our case returned by a call to ftok()).
	 	This key value is then compared to existing key values that exist within the kernel for other
	 	shared memory segments. At that point, the open or access operation is dependent upon the
	 	contents of the shmflg argument.

	 	If IPC_CREAT is used alone, shmget() either returns the segment identifier for a newly created segment,
	 	or returns the identifier for a segment which exists with the same key value.

	 	Syntax: int shmget(key_t key, int size, int shmflg)
*/

	 /* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	 shmid = shmget(shm_keygen, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	 if(shmid < 0){
	  cout << "Error in shared memory allocation." << endl;
	  exit(1);
	 }
	 else{
	  cout << "Shared memory allocation successful!" << endl;
	 }

	 /* TODO: Attach to the shared memory */
/*
	 	Before you can use a shared memory segment, you have to attach yourself to it using shmat().

	 	shmid is shared memory id.

	 	shmaddr specifies specific address to use.
	 	If the addr argument is zero (0), the kernel tries to find an unmapped region.
	 	An address can be specified, 	but is typically only used to facilitate proprietary
	 	hardware or to resolve conflicts with other apps

	 	Syntax: void *shmat(int shmid ,void *shmaddr ,int shmflg);
*/
	 sharedMemPtr = shmat(shmid,NULL,0);
	 if(sharedMemPtr < 0){
	  cout << "Error in shared memory attach." << endl;
	  exit(1);
	 }
	 else{
	  cout << "Shared memory attach successful!" << endl;
	 }

	 /* TODO: Create a message queue */
/*
	 	msgget(): either returns the message queue identifier for a newly created message
	 	queue or returns the identifiers for a queue which exists with the same key value.

	 	The first argument to msgget() is the key value (in our case returned by a call to ftok()).
	 	This key value is then compared to existing key values that exist within the kernel for other message queues.
	 	At that point, the open or access operation is dependent upon the contents of the msgflg argument.

	 	Syntax: msgget(key_t key, int msgflg);
*/
	 msqid = msgget(shm_keygen, 0666 | IPC_CREAT); // msgget creates a message queue
	 if(msqid < 0){
	  cout << "Error in creating message queue." << endl;
	  exit(1);
	 }
	 else{
	  cout << "Creating message queue successful!" << endl;
	 }

}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	shmdt(sharedMemPtr);
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");


	/* A buffer to store message we will send to the receiver. */
	message msgReceiver;

	/* A buffer to store message received from the receiver. */
	message msgSender;

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory.
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((msgSender.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}


		/* TODO: Send a message to the receiver telling him that the data is ready
 		 * (message of type SENDER_DATA_TYPE)
 		 */

		 /*
		 	The first argument to msgsnd is our queue identifier, returned by a previous call to msgget.
		 	The second argument, msgp, is a pointer to our redeclared and loaded message buffer.
		 	The msgsz argument contains the size of the message in bytes, excluding the length of the message type (4 byte long).
		 	The msgflg argument can be set to 0 (ignored)

		 Syntax: msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
		 */

		msgSender.mtype = SENDER_DATA_TYPE;
		msgsnd(msqid, &msgSender, sizeof(msgSender), 0);

		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us
 		 * that he finished saving the memory chunk.
 		 */

		 /*
		 		The msgrcv() function shall read a message from the queue associated
		     with the message queue identifier specified by msqid and place it in
		     the user-defined buffer pointed to by msgp.

		 		The argument msgp points to a user- defined buffer that contains first a field of
		 		type long specifying  the type of the message, and then a data portion that holds the data
		     bytes of the message.

		 		Syntax: msgrcv (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
		 */

		msgrcv(msqid, &msgReceiver, 0, RECV_DONE_TYPE, 0);

	}


	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0.
	  */

		msgSender.mtype = SENDER_DATA_TYPE;
		msgSender.size = 0;
		msgsnd(msqid, &msgSender, sizeof(msgSender), 0);


	/* Close the file */
	fclose(fp);

}


int main(int argc, char** argv)
{

	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}

	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);

	/* Send the file */
	send(argv[1]);

	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
