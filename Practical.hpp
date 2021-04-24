// 处理连接的函数
#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<iostream>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>
#include<vector>
using namespace std;
const MAX_BUF_SIZE = 10;
void HandleSEND_RECV(int sockfd);
void PrintfSocketAddress(struct sockaddr * address);
int SetupTCPConnect(char *node,char *service);
int setupTCPServerSocket(char* port);
void setNonBlock(int sockfd);
void addfd(int epfd,int sockfd,bool enableET);
void dealET(struct epoll_event* events,int num,int listenfd,vector<int> clntfd);