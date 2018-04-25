# PeerToPeerApp

## About
Peer2Peer is a simple application built using socket programming in C++ which is capable of transferring files over TCP. Since TCP is used file verification(hashing) is not considered.

## Design
The P2P communication is achieved by two threads running under a main thread.
One thread acts as a server and other thread acts as a client running under a single process.
Thus if a new process is started it can communicate with each other. The hostname and port
is needed to make the connection.

## Major Files:
`P2Pmain.cpp`:
The main startup file, resposible to create two threads.

`PeerReceiver.cpp`
The peer receiver or the server thread, resposible to accept and provide files to other peers.

`PeerSender.cpp`
The peer sender or the client thread, resposible to connecte to other peers and give commands to download specific file.

##  OutPutLog 
In the ReadMe.doc

## Compilation
Go to the Debug folder and run 
`make all` to compile
`make clean` to clean

##  How To Test:
A sample test is provided under the “Test” folder.
The `Test` directory contains a simple structure of a P2P setup with 2 peers, Peer1 and Peer2. Each of them have a shared directory named, `shared`. Each of them have a executable named `PeerToPeer` which can be used to create a connection between each other. 

## Environment/Tools:
A Linux ubuntu desltop:
Linux 4.4.0-119-generic #143-Ubuntu SMP Mon Apr 2 16:08:24 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux
Compiler : C++14
Editor: Eclipse

## Limitation:
The compiler warnings need to be fixed
Error and Exception handling can be improved 
More test case and different file format need to be tested.
Unit test case addition


