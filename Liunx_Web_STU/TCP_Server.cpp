#include"Function.h"

#include<iostream>
#include<string.h>

#include<sys/epoll.h>
#include<netinet/in.h>
#include<map>

#define Connect_MAX (1024)

int TCP_Server()
{
	int epfd = epoll_create1(0);
	if (epfd < 0) {
		perror("epoll create failed :");
		return -1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket create failed :");
		return -1;
	}

	sockaddr_in adrr;
	adrr.sin_family = AF_INET;																//使用IPV4地址
	adrr.sin_addr.s_addr = htonl(INADDR_ANY);												//htonl进行大端和小端的转化
	adrr.sin_port = htons(9898);

	int ret = bind(sockfd, (sockaddr*)&adrr, sizeof(adrr));
	if (ret < 0) {
		printf("bind failed \n");
		return -1;
	}

	ret = listen(sockfd, 5);
	if (ret < 0) {
		printf("listen failed \n");
		return -1;
	}

	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
	if (ret < 0) {
		printf("epoll_ctl failed \n");
		return -1;
	}

	std::map<int, Client>Clinet_group;
	epoll_event evs[Connect_MAX];

	while (true) {																				//循环检测信息
		int n = epoll_wait(epfd, evs, Connect_MAX, -1);

		if (n < 0) {
			printf("epoll_wait failed \n");
			break;
		}

		for (int i = 0; i < n; i++) {															//遍历epoll响应的事件
			int fd = evs[i].data.fd;

			if (fd == sockfd) {																	//如果等于socket表示有新客户端进行连接
				sockaddr_in client_socket;
				socklen_t sock_len = sizeof(client_socket);
				int client_socketfd = accept(sockfd, (sockaddr*)&client_socket, &sock_len);
				if (client_socketfd < 0) {
					printf("connect error \n");
					continue;
				}
				printf("[%s: %d] is success connect\n", inet_ntoa(client_socket.sin_addr),htons(client_socket.sin_port));

				epoll_event ev_cilent;
				ev_cilent.events = EPOLLIN;
				ev_cilent.data.fd = client_socketfd;

				ret = epoll_ctl(epfd, EPOLL_CTL_ADD, client_socketfd, &ev_cilent);
				if (ret < 0) {
					printf("epoll_ctl error \n");
					continue;
				}

				Client new_client;
				new_client.name = "";
				new_client.Sockfd = client_socketfd;
				Clinet_group[client_socketfd] = new_client;
			}

			else																				//如果是客户端有请求
			{
				char buffer[1024];
				int len = read(fd, buffer, 1024);
				//printf("len=%d\n", len);

				if (len < 0) {																	//读取错误
					continue;
				}

				else if (len == 0) {															//读取到长度为空断开连接
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);									//删除epoll中的字符
					close(fd);																	//关闭文件识别符
					printf("[%s] client quit\n", Clinet_group[fd].name.c_str());
					Clinet_group.erase(fd);														//将文件识别符删掉

				}
				else {																			//长度大于0
					std::string mesg(buffer, len);												//创建新字符串
					if (Clinet_group[fd].name.empty())											//名字为空则先加载为名字
					{
						Clinet_group[fd].name = mesg;
						printf("client success to add:%s\n", mesg.c_str());
					}
					else																		//名字不为空
					{
						std::string msg = '[' + Clinet_group[fd].name + "]: " + mesg;
						for (auto& a : Clinet_group)
						{
							if (a.first != fd) {
								write(a.first, &msg[0], msg.size());
							}
						}
						printf("%s\n", &msg[0]);
					}
				}
			}
		}
	}
	close(epfd);
	close(sockfd);
	return 0;
}