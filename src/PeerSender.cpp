/*
 * PeerSender.cpp
 *
 *  Created on: 24-Apr-2018
 *      Author: pinaki
 *      This runs is a separate thread and act as a client to connect
 *      to the other peer
 */

#include "PeerSender.h"
#include "color.h"
#include <errno.h>

using namespace std;

void PeerSender::RegisterPeer(){

	string hostname;
	int portno;
	cout << "Enter other Peer hostname to connect " << endl;
	cin >> hostname;
	cout << "Enter other Peer receiver port" << endl;
	cin >> portno;
	cin.get();
	this->SenderName=hostname;
	this->SenderPort=portno;
}

void PeerSender::FileDownload(){

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		cerr<< "ERROR opening socket\n";
	server = gethostbyname(SenderName.c_str());
	if (server == NULL) {
		cerr<< "ERROR, no such host\n";
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(SenderPort);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		cerr<< "ERROR connecting" << strerror(errno) << "\n";

	cout << "The list of files which can be downloaded are:\n";
	char cmd[MAX_COMMAND_LEN];
	strcpy(cmd,"ls"); //listing of server files
	while (strcmp(cmd, "exit")) {
		if (!strcmp(cmd, "ls")) {
			if (send(sockfd, "ls", MAX_COMMAND_LEN, 0) < 0) {
				cerr<< "ERROR sending" << strerror(errno) << "\n";;
			};
			GetListOfFiles(&receive, sockfd);
			cout<< receive <<"\n";
			free(receive);
		} else {
			cout <<"File Request to download is " << cmd  <<endl;
			if (send(sockfd, cmd, MAX_COMMAND_LEN, 0) < 0) {
				cerr << "getfile:send_request: Sending Error\n";
			}
			ReceiveAndDownload(sockfd);
		}
		cout << "Type the 'filename' to download or 'exit' to discontinue " << endl;
		GetPrompt(cmd);

	}
	if (send(sockfd, "exit", MAX_PACKET_CHUNK_LEN, 0) < 0) {
		cerr <<"Error send while exiting\n";
	}
	close(sockfd);
}

void PeerSender::GetPrompt(char *cmd) {
	printf("%s> %s", KGRN, RESET);
	scanf("%[^\n]", cmd);
	getchar();
	return;
}

void PeerSender::GetListOfFiles(char **buff, int socket_id) {
	char small_buff[MAX_BUFFER_LEN];
	memset(small_buff, 0, sizeof(small_buff));
	recv(socket_id, small_buff, MAX_PACKET_CHUNK_LEN, 0);
	int to_receive = atoi(small_buff);
	int R, received = 0;
	*buff = (unsigned char*) malloc(sizeof(unsigned char) * (to_receive + 100));
	memset(small_buff, 0, sizeof(small_buff));
	**buff = '\0';

	while (received < to_receive) {
		R = recv(socket_id, small_buff, MAX_PACKET_CHUNK_LEN, 0);
		if (R < 0) {
			printf("ERROR: Received Failed\n");
		}
		strncat(*buff, small_buff, strlen(small_buff) - 1);
		memset(small_buff, 0, sizeof(small_buff));
		received += R;
	}
	return;
}

void PeerSender::ReceiveAndDownload(int SOCKET) {
	unsigned char buffer[MAX_BUFFER_LEN + 4];
	unsigned char file_name[MAX_BUFFER_LEN];
	int file_data_len;

	memset(buffer, 0, sizeof(buffer));
	memset(file_name, 0, sizeof(file_name));

	/* Get File name + len, under 256 characters */
	if (recv(SOCKET, buffer, MAX_PACKET_CHUNK_LEN, 0) < 0) {
		cerr<<"ERROR: Reading file name\n";
		return;
	}
	printf("1: %s\n",buffer);
	char *end_pointer;
	char* ch = strtok_r(buffer, "|", &end_pointer);
	strncpy(file_name, ch, strlen(ch));
	strcat(file_name, "\0");
	ch = strtok_r(NULL, " ,", &end_pointer);
	file_data_len = atoi(ch);
	printf("%sFileName: %s%s\n", KRED, RESET, file_name);
	printf("%sFilesize: %s%d bytes\n", KRED, RESET, file_data_len);

	/* Create File */
	FILE *fp = fopen(file_name, "wb+");
	if (fp == NULL) {
		printf("File open error");
		return;
	}

	printf("%s-----------------------------------------------------%s\n", KYEL,
			RESET);
	int data_received = 0;
	char dd[10];
	int received = 0;
	while (received < file_data_len) {
		int R = recv(SOCKET, buffer, MAX_PACKET_CHUNK_LEN, 0);
		if (R < 0) {
			cerr<< "Error: While receiving\n";
		}
		if (!fputs(buffer, fp)) {
			cerr<< "ERROR: While saving to file\n";
		};
		data_received += R;
		memset(buffer, 0, sizeof(buffer));
		received += R;
	}
	if (received >= file_data_len) {
		printf("%sFile Length:%s nbytes %s%s%s\n", KRED, RESET, KGRN, TICK,
				RESET);
	} else {
		printf("%sCONCERN:%s File Length not matching%s\n", KRED, KYEL, RESET);
	}

	fclose(fp);
}


