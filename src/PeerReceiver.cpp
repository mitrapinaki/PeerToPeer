/*
 * PeerReceiver.cpp
 *
 *  Created on: 24-Apr-2018
 *      Author: pinaki
 *      This runs is a separate thread and open a port to receive
 *      other peer incomming connection
 */
#include "color.h"
#include "PeerReceiver.h"
using namespace std;

PeerReceiver::PeerReceiver(std::string ReceiverName,std::string ReceiverPort){

	this->ReceiverName = ReceiverName;
	this->ReceiverPort = ReceiverPort;

}
void PeerReceiver::StartReceiver(){

	if (chdir(SERVER_SHARED_DIR) <0)
		cerr<< "ERROR Shared Dir  " << strerror(errno)<<"\n";
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		std::cerr << "ERROR opening socket" << strerror(errno)<<"\n";
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(ReceiverPort.c_str());
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		std::cerr << "ERROR on binding" << std::endl;
	listen(sockfd, 10);
	clilen = sizeof(cli_addr);

	cout << "---- PEER Receiver started at Port ---- " << portno << endl;
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		std::cerr << "ERROR on accept" << strerror(errno)<<"\n";;

	char cmd[MAX_COMMAND_LEN];
	recv(newsockfd, cmd, MAX_COMMAND_LEN, 0);
	while (strcmp(cmd, "exit")) {
		if (!strcmp(cmd, "ls")) {
			listfiles(newsockfd);
		} else{
			//cout << "File to upload is " << cmd << endl;
			FileReaderAndReceiver(cmd, newsockfd);
		}

		memset(cmd, 0, sizeof(cmd));
		recv(newsockfd, cmd, MAX_COMMAND_LEN, 0);
	}
	close(newsockfd);
	close(sockfd);
}

void PeerReceiver::FileReaderAndReceiver(char* file_name, int socket_id) {

	unsigned char buff[MAX_BUFFER_LEN + 3];
	memset(buff, 0, sizeof(buff));
	FILE *fp = fopen(file_name, "rb");
	if (fp == NULL) {
		cerr <<"File open error\n";
		return;
	}

	if (strlen(file_name) >= MAX_BUFFER_LEN) {
		cerr << "ERROR: Please use a filename less than 256 characters\n";
		fclose(fp);
		return;
	}

	// Length
	int SIZE = GetFilesize(fp);
	char snum[5];
	sprintf(snum, "%d", SIZE);

	// Write all
	memset(buff, 0, sizeof(buff));
	strcat(buff, file_name);
	strcat(buff, "|");
	strcat(buff, snum);
	printf("FILE_NAME_DATA: %s\n",buff);
	if (strlen(snum) + strlen(file_name) + 1 > MAX_PACKET_CHUNK_LEN) {
		cerr << "Name + Size length exceeded. Error may occur.\n";
	}

	send(socket_id, buff, MAX_PACKET_CHUNK_LEN, 0);

	int R;
	memset(buff, 0, sizeof(buff));
	while ((R = fread(buff, sizeof(char), MAX_PACKET_CHUNK_LEN, fp))) {
		if (send(socket_id, buff, MAX_PACKET_CHUNK_LEN, 0) < 0) {
			perror("Sending Error");
		};
		bzero(buff, MAX_PACKET_CHUNK_LEN);
	}

	memset(buff, 0, sizeof(buff));
	fclose(fp);
}

int PeerReceiver::GetFilesize(FILE* fileid) {
	fseek(fileid, 0L, SEEK_END);
	int sz = ftell(fileid);
	fseek(fileid, 0L, SEEK_SET);
	return sz;
}

void PeerReceiver::listfiles(int socket_id) {
	char buff[MAX_BUFFER_LEN];
	memset(buff, 0, sizeof(buff));
	listCommand(buff);

	// Send length of data
	char snum[200];
	memset(snum, 0, sizeof(snum));
	int size=strlen(buff);
	sprintf(snum, "%d", size);
	send(socket_id, snum, MAX_PACKET_CHUNK_LEN, 0);

	// Send all data
	int S,sent=0;
	while(sent < size) {
		S = send(socket_id, buff + sent, MAX_PACKET_CHUNK_LEN, 0);
		if(S < 0) {
			cerr << "listfiles sending error \n";
		}
		sent += S;
	};
}


void PeerReceiver::listCommand(char buffer2[]) {
	int pipefd[2];
	pipe(pipefd);
	char buffer[MAX_BUFFER_LEN];
	memset(buffer, 0, sizeof(buffer));
	int pid=fork();
	if(pid==0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		char *args[] = { "ls", "-a1", NULL };
		execvp(args[0], &args[0]);
		exit(EXIT_SUCCESS);
	}
	else {
		// parent
		close(pipefd[1]);  // close the write end of the pipe in the parent
		while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
		{
		}
	}
	strcpy(buffer2, buffer);	// Copy into the given buffer
	return;
}
