#pragma once
#include<sys/socket.h>
#include<sys/unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string>
extern int TCP_Server();
extern int UDP_Server();
extern int Test();

struct Client {
	std::string name;
	int Sockfd;
};
