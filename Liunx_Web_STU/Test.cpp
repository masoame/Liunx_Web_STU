#include"Function.h"

int buffer_recv[1024];

int Test()
{
	int tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	sockaddr_in local_addr;
	local_addr.sin_addr.s_addr = 0;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = ntohs(9898);

	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(udpsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	setsockopt(tcpsock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));


	bind(tcpsock, (sockaddr*)&local_addr, sizeof(sockaddr));
	perror("code:");

	bind(udpsock, (sockaddr*)&local_addr, sizeof(sockaddr));
	perror("code:");

	listen(tcpsock, 4);
	perror("code:");

	socklen_t* addr_len;

	sockaddr_in client_socket;
	socklen_t sock_len = sizeof(client_socket);
	int client_socketfd = accept(tcpsock, (sockaddr*)&client_socket, &sock_len);
	perror("code:");

	sockaddr_in addr_in = { 0 };
	socklen_t socklen = sizeof(sockaddr);
	int ret;

	ret = recvfrom(udpsock, buffer_recv, 1024, 0, (sockaddr*)&addr_in, &socklen);

	perror("code:");
	printf("%s:%d\n", inet_ntoa(addr_in.sin_addr), addr_in.sin_port);
	printf("%s:%d\n", inet_ntoa(client_socket.sin_addr), client_socket.sin_port);

	sleep(1);
	sendto(udpsock, "it is udp connect ip", 21, 0, (sockaddr*)&addr_in, sizeof(sockaddr));
	sleep(1);
	sendto(udpsock, "it is tcp connect ip", 21, 0, (sockaddr*)&client_socket, sizeof(sockaddr));


	close(client_socketfd);
	close(tcpsock);
	close(udpsock);
	return 0;
}