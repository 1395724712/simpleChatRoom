// 处理连接的函数
#pragma once
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<arpa/inet.h>
using namespace std;
void HandleSEND_RECV(int sockfd);
void PrintfSocketAddress(struct sockaddr * address);
int SetupTCPConnect(char *node,char *service);
int setupTCPServerSocket(char* port);