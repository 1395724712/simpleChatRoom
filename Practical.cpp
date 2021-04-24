#include"Practical.hpp"

void HandleSEND_RECV(int sockfd){
    /*
    * 一边接收，一边发送
    */
    int recvNum;
    char bufNum[1000];

    recvNum = recv(sockfd,bufNum,1000,0);

    while(recvNum>0){
        //收到后再送回去
        int sendNum = send(sockfd,bufNum,recvNum,0);
        if(sendNum!=recvNum)
            cerr<<"发送失败"<<endl;

        bufNum[recvNum] = '\0';
        cout<<"收到的数据为：\t"<<bufNum<<endl;
        //继续接收
        recvNum = recv(sockfd,bufNum,1000,0);
    }

    if(recvNum==0)
        cout<<"链接中止"<<endl;
    else if(recvNum<0)
        cerr<<"链接出现问题"<<endl;

    close(sockfd);

    return;
}

void PrintfSocketAddress(struct sockaddr * address)
{
    //将address中的地址和端口号输出

    //1 首先检查address的有效性
    if(address==nullptr)
        cerr<<"地址无效"<<endl;

    //2 确定地址是那一族
    char dst[INET6_ADDRSTRLEN];
    char *src;
    int port;

    switch(address->sa_family){
        case AF_INET:
            if(inet_ntop(address->sa_family,&(((struct sockaddr_in*)address)->sin_addr.s_addr),dst,INET6_ADDRSTRLEN)==NULL){
                cerr<<"inet_ntop 转化失败"<<endl;
            }
            port = ntohs(((struct sockaddr_in*)address)->sin_port);
            break;
        case AF_INET6:
            if(inet_ntop(address->sa_family,&(((struct sockaddr_in6*)address)->sin6_addr.s6_addr),dst,INET6_ADDRSTRLEN)==NULL){
                cerr<<"inet_ntop 转化失败"<<endl;
            }
            port = ntohs(((struct sockaddr_in6*)address)->sin6_port);
            break;
        default:
            cerr<<"输入内容有误"<<endl;
    }

    //3 转化网址为本地格式

    cout<<"网址为："<<dst<<endl;
    cout<<"端口为："<<port<<endl;
    return;
}

int SetupTCPConnect(char *node,char *service){
    /*
    * Author:wh
    * Date:20210409
    * Description:根据输入的网络节点名称和服务（端口号）链接至目标节点
    * Resolution:1 声明所需地址的协议族和协议
    * 2 获取网络地址
    * 3 选一个能用的创建套接子并链接
    */
    // 1 声明所需的网络节点名称和服务
    struct addrinfo hints;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //2 获取网络地址
    struct addrinfo *res;
    getaddrinfo(node,service,&hints,&res);

    //3 循环选择一个能用的创建套接字并链接
    struct addrinfo *cur = res;
    int sockfd;
    while(cur!=nullptr){
        //3.1 创建socket
        sockfd = socket(cur->ai_family,cur->ai_socktype,cur->ai_protocol);
        if(sockfd<0)
        {
            cur = res->ai_next;
            continue;
        }
        //3.2 链接
        int rtnVal = connect(sockfd,cur->ai_addr,cur->ai_addrlen);
        if(rtnVal<0){
            cur = res->ai_next;
            continue;
        }

        return sockfd;
    }

    freeaddrinfo(res);

    return -1;
}

int setupTCPServerSocket(char* port){
    /*
    * Author:wh
    * Date:20210417
    * Description:根据输入的端口号创建可用的套接字，
    * 并绑定到可用的网络，之后返回套接字用作select多路复用
    */
    //1 获得可用的网络地址
    struct addrinfo *hints = new struct addrinfo;
    struct addrinfo *res;
    // memset(hints,0,sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_protocol = IPPROTO_TCP;
    int rtnVal = getaddrinfo(nullptr,port,hints,&res);
    if(rtnVal!=0)
        cerr<<"getaddrinfo失败,在端口："<<port<<endl;

    //2 创建可用的套接字,并绑定到可用的网址和端口上
    int sockfd;
    struct addrinfo *cur = res;
    for(;cur!=nullptr;cur = cur->ai_next){
        sockfd = socket(cur->ai_family,cur->ai_socktype,cur->ai_protocol);
        if(sockfd<0)
            continue;

        rtnVal = bind(sockfd,cur->ai_addr,cur->ai_addrlen);
        if(rtnVal!=0)
            continue;

        rtnVal = listen(sockfd,3);
        if(rtnVal==0)
            break;
    }

    if(cur==nullptr)
        cerr<<"套接字创建失败，在端口: "<<port<<endl;

    free(hints);
    freeaddrinfo(res);
    return sockfd;
}