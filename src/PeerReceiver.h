/*
 * Peer.h
 *
 *  Created on: 24-Apr-2018
 *      Author: pinaki
 */

#ifndef PEERRECEIVER_H_
#define PEERRECEIVER_H_
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>
#include <thread>
#include <iostream>

class PeerReceiver{

private:
	std::string ReceiverName ="";
	std::string ReceiverPort ="";
	int sockfd=0, newsockfd=0, portno=0;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[256]={0};
	void FileReaderAndReceiver(char*,int);
	int GetFilesize(FILE*);
	void listfiles(int);
	void listCommand(char[]);


public:
	PeerReceiver(std::string ,std::string);
	void StartReceiver();
};


#endif /* PEERRECEIVER_H_ */
