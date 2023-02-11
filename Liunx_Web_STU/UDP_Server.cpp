#include"Function.h"
#include<pthread.h>

char buffer[1024];
int ret;

int UDP_Server()
{
	int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in local_addr;

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = 0;
	local_addr.sin_port = htons(9898);
	ret = bind(sock_fd, (sockaddr*)&local_addr, sizeof(sockaddr_in));
	if (ret < 0) {
		printf("端口绑定失败");
		return -1;
	}

	//用于存储两个客户端地址
	sockaddr_in client_addr[2];
	socklen_t client_t[2] = { sizeof(sockaddr_in),sizeof(sockaddr_in) };
	
	while (true) {
		recvfrom(sock_fd, buffer, 1024, 0, (sockaddr*)client_addr, client_t);
		printf("one client was link\n");

		recvfrom(sock_fd, buffer, 1024, 0, (sockaddr*)(client_addr + 1), client_t + 1);
		printf("one client was link\n");

		printf("%s :%d\n", inet_ntoa(client_addr[0].sin_addr), htons(client_addr[0].sin_port));
		printf("%s :%d\n", inet_ntoa(client_addr[1].sin_addr), htons(client_addr[1].sin_port));

		sleep(3);
		ret = sendto(sock_fd, client_addr + 1, sizeof(sockaddr), 0, (sockaddr*)client_addr, sizeof(sockaddr));
		if (ret < 0) {
			printf("文件发送失败\n");
		}
		ret = sendto(sock_fd, client_addr, sizeof(sockaddr), 0, (sockaddr*)(client_addr + 1), sizeof(sockaddr));
		if (ret < 0) {
			printf("文件发送失败\n");
		}
		printf("two client link successed\n");
	}
	

	close(sock_fd);
	return 0;
}