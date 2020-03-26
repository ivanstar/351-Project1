Ivan Tu         ivanstar@csu.fullerton.edu       Section 02
Bradley Keizer  brad14@csu.fullerton.edu         Section 02
Alec Battisti   alec.battisti@csu.fullerton.edu  Section 02

Language: C++
Extra credit not implemented

How to execute:
  1. in a terminal window, use the make command to compile the program
  2. in a separate terminal window, execute the receiver using ./recv
  3. in the original terminal window execute the sender using ./sender keyfile.txt

Execution example:
  
  in terminal window 1:
    user@user-VirtualBox:~/Desktop/351-Project1-master$ ./sender keyfile.txt
    ftok key generation successful!
    Shared memory allocation successful!
    Shared memory attach successful!
    Creating message queue successful!
    
   in terminal window 2:
    user@user-VirtualBox:~/Desktop/351-Project1-master$ ./recv 
    ftok key generation successful!
    Shared memory allocation successful!
    Shared memory attach successful!
    Creating message queue successful!
    File opened successful
    Reading in message...
    Message received successful!
    Ready for the next file chunk.
    Empty message sent successful
    Reading in message...
    Message received successful!
    File closed.

Contributions:
 Ivan: recv.cpp, sender.cpp, Makefile
 Bradley: Design of Assignment 1.odt
 Alec: README.txt
  
