#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */
#include <fstream>
#include <iostream>

using namespace std;


//brad14@csu.fullerton.edu
//alec.battisti@csu.fullerton.edu

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";

message msgReceiver;
message msgSender;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	         2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		 3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V object (i.e. message queues, shared memory, and sempahores)
		    is unique system-wide among all System V objects. Two objects, on the other hand,
		    may have the same key.
	 */

	 //Task 1
	 ofstream outFile("keyfile.txt");
	 outFile << "Hello world";
	 outFile.close();

	 //Task 2
/*
	 The ftok() function shall return a key based on path and id that is
	 usable in subsequent calls to msgget() and shmget().

	 key_t - is used to generate a key

*/
	 key_t shm_keygen = ftok("keyfile.txt", 'a');
	 if(shm_keygen < 0){
		 cout << "Error in ftok key generation." << endl;
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

/*
	Before you can use a shared memory segment, you have to attach yourself to it using shmat().

	shmid is shared memory id.

	shmaddr specifies specific address to use.
	If the addr argument is zero (0), the kernel tries to find an unmapped region.
	An address can be specified, 	but is typically only used to facilitate proprietary
	hardware or to resolve conflicts with other apps

	Syntax: void *shmat(int shmid ,void *shmaddr ,int shmflg);
*/

	/* TODO: Attach to the shared memory */
	sharedMemPtr = shmat(shmid,NULL,0);
	if(sharedMemPtr < 0){
		cout << "Error in shared memory attach." << endl;
		exit(1);
	}
	else{
		cout << "Shared memory attach successful!" << endl;
	}

	/* TODO: Create a message queue */
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */

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
 * The main loop
 */
void mainLoop()
{
	int mRcv = 0;
	/* The size of the mesage */
	int msgSize = 0;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	else{
		cout << "File opened successful" << endl;
	}
	msgSize++;

    /* TODO: Receive the message and get the message size. The message will
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */


	while(msgSize != 0)
	{
/*
    The msgrcv() function shall read a message from the queue associated
    with the message queue identifier specified by msqid and place it in
    the user-defined buffer pointed to by msgp.

    The argument msgp points to a user- defined buffer that contains first a field of
    type long specifying  the type of the message, and then a data portion that holds the data
    bytes of the message.

		Syntax: msgrcv (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)
*/

		cout << "Reading in message..." << endl;
		mRcv = msgrcv(msqid, &msgReceiver, sizeof(msgReceiver), SENDER_DATA_TYPE, 0);
		if(mRcv < 0){
			cout << "Error in message receiver" << endl;
			exit(1);
		}
		else{
			cout << "Message received successful!" << endl;
		}

		msgSize = msgReceiver.size; //Get the message size

		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}

			/* TODO: Tell the sender that we are ready for the next file chunk.
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case).
 			 */

			 cout << "Ready for the next file chunk." << endl;

			 msgSender.mtype = RECV_DONE_TYPE;
			 msgSender.size = 0;


/*
	The first argument to msgsnd is our queue identifier, returned by a previous call to msgget.
	The second argument, msgp, is a pointer to our redeclared and loaded message buffer.
	The msgsz argument contains the size of the message in bytes, excluding the length of the message type (4 byte long).
	The msgflg argument can be set to 0 (ignored)

	Syntax: msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
*/
			int msgSend = 0;
			msgSend =  msgsnd(msqid, &msgSender, 0, 0);
			 if(msgSend < 0){
				 cout << "Error in empty message send" << endl;
			 }
			 else{
				 cout << "Empty message sent sucessful" << endl;
			 }

		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
			cout << "File closed." << endl;
		}
	}
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{

/*
		When you’re done with the shared memory segment, your program should
		detach itself from it using shmdt().

		Syntax: int shmdt(void *shmaddr);
*/

		/* TODO: Detach from shared memory */
		shmdt(sharedMemPtr);


/*
		shmctl() performs the control operation on shared memory specified by cmd on the System V shared memory segment
		whose identifier is given in shmid.

		msgmctl() performs the control operation on msg queue specified by cmd on the System V shared memory segment
		whose identifier is given in msqid.

		The IPC_RMID command doesn't actually remove a segment from the kernel. Rather, it marks the segment for removal.

		The buf argument is a pointer to a shmid_ds structure, defined in <sys/shm.h>
		If we are unable to copy the internal buffer, -1 is returned to the calling function. If all went well, a value of 0(zero)
		is returned.


		Syntax: shmctl( int shmid, int cmd, struct shmid_ds *buf)
		Syntax: msgctl ( int msgqid, int cmd, struct msqid_ds *buf )
*/

		/* TODO: Deallocate the shared memory chunk */
		shmctl(shmid, IPC_RMID, 0);


		/* TODO: Deallocate the message queue */
		msgctl(msqid, IPC_RMID, 0);

}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{

	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */
	 signal(SIGINT, ctrlCSignal);

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
