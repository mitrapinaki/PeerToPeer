/*
 * P2Pmain.cpp
 *
 *  Created on: 24-Apr-2018
 *      Author: pinaki
 */
#include "PeerReceiver.h"
#include "PeerSender.h"
#include <thread>
#include <unistd.h>
#include <memory>
#include <exception>
using namespace std;

//The PeerReceiver Thread act like a server
void StartThePeerReceiver(string PeerReceiverName,string PeerReceiverPort){

	std::unique_ptr<PeerReceiver> rec(new PeerReceiver(PeerReceiverName,PeerReceiverPort));
	rec->StartReceiver();
}

//The PeerSender Thread will connect to other peer
void StartThePeerSender(){
	sleep(1);
	std::unique_ptr<PeerSender> send(new PeerSender());
	//PeerSender* send = new PeerSender();
	send->RegisterPeer();
	send->FileDownload();
}

int main(){
	string PeerReceiverName ="";
	string PeerReceiverPort ="";
	cout << "Enter the peer hostname \n";
	cin >> PeerReceiverName;
	cout << "Enter the peer receiver port \n";
	cin >> PeerReceiverPort;

	thread PeerReceiver(StartThePeerReceiver,PeerReceiverName,PeerReceiverPort);
	thread StartTheSender(StartThePeerSender);
	cout <<" ---START---\n";
	PeerReceiver.join();
	StartTheSender.join();
	cout << "---END---" << endl;
	return 0;
}
