/*
 * PeerSender.h
 *
 *  Created on: 24-Apr-2018
 *      Author: pinaki
 */

#ifndef PEERSENDER_H_
#define PEERSENDER_H_

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


/*const int MAX_BUFFER_LEN =5000;
const int MAX_COMMAND_LEN = 200;
const int MAX_PACKET_CHUNK_LEN = 1024;*/


class PeerSender{
private:
	std::string SenderName ="";
	int SenderPort=0;
	int sockfd=0,portno=0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256]={0};
	char ip[INET_ADDRSTRLEN]={0};
	unsigned char *receive=NULL;
	void GetPrompt(char *cmd);
	void GetListOfFiles(char**,int);
	void ReceiveAndDownload(int SOCKET);

public:
	PeerSender(){};
	void RegisterPeer();
	void FileDownload();

};



#endif /* PEERSENDER_H_ */
