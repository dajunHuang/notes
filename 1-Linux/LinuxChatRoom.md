---
title: Linux Socket 聊天室
date: 2022/03/20
---

# 服务器端

## socket()函数

返回创建的套接字，即一个`int`数，发生错误返回`-1`。函数原型如下：

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int socket(int af, int type, int protocol);
```

`af`为地址族`Address Family`，也就是 IP 地址类型，常用的有`AF_INET`和`AF_INET6`。
`type`为数据传输方式，常用的有`SOCK_STREAM`和`SOCK_DGRAM`。
`protocol`表示传输协议，常用的有`IPPROTO_TCP`和`IPPTOTO_UDP`。

因为`SOCK_STREAM`、`SOCK_GGRAM`分别和TCP、UDP对应，使用IPV4地址、`SOCK_STREAM`创建套接字有以下两种方法。

```cpp
int mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //IPPROTO_TCP表示TCP协议
int mSocket = socket(AF_INET, SOCK_STREAM, 0);
```

## bind()函数

错误的时候返回-1。函数的原型为：

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //in_addr_t结构体
int bind(int sock, struct sockaddr *addr, socklen_t addrlen);  //Linux
```

`sock`为 socket 文件描述符，`addr`为`sockaddr`结构体变量的指针，`addrlen`为`addr`变量的大小，可由`sizeof()`计算得出。

服务器上把套接字和本机IP与特定端口绑定的方法如下：

```cpp
struct sockaddr_in servaddr;

memset(&servaddr, 0, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_port = htons(PORT);
//servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY即0.0.0.0，linux下表示的是一个服务器上所有的网卡
if(bind(mSocket, (struct sockaddr* ) &servaddr, sizeof(servaddr))==-1)
{
    perror("bind");
    exit(1);
}
```

### sockaddr_in和sockaddr结构体

`sockaddr`结构体如下所示：

```cpp
struct sockaddr{
    sa_family_t  sin_family;   //地址族（Address Family），也就是地址类型
    char         sa_data[14];  //IP地址和端口号
};
```

`sockaddr_in`结构体如下所示：

```cpp
struct sockaddr_in{
    sa_family_t     sin_family;   //地址族（Address Family），也就是地址类型
    uint16_t        sin_port;     //16位的端口号
    struct in_addr  sin_addr;     //32位IP地址
    char            sin_zero[8];  //不使用，一般用0填充
};


struct in_addr{
    in_addr_t  s_addr;  //32位的IP地址
};
```

下图是`sockaddr`与`sockaddr_in`的对比（括号中的数字表示所占用的字节数）：

<img src="LinuxChatRoom/addrin.jpg" style="zoom: 80%;" />

可以认为，`sockaddr`是一种通用的结构体，可以用来保存多种类型的IP地址和端口号，而`sockaddr_in`是专门用来保存 IPv4 地址的结构体。

### 本机转换

网络字节顺序是大端序`big-endian`，所以填充`sockaddr_in`的属性要把主机字节序的数据转换为网络字节序，用到的系列函数如下：

```cpp
#include <arpa/inet.h>　
uint32_t htonl(uint32_t hostlong);//Host to Network Long
uint16_t htons(uint16_t hostshort);//Host to Network Short
uint32_t ntohl(uint32_t netlong);//Network to Host Long
uint16_t ntohs(uint16_t netshort);//Network to Host Short

in_addr_t inet_addr(const char *cp);//字符串IP转in_addr中32位IP
int inet_pton(int af, const char *src, void *dst);//将ip地址转换成网络字节序后的结果存储在dst指向的 struct in_addr 结构体中 s_addr 成员
int inet_aton(const char *cp, struct in_addr *inp);//字符串存到in_addr结构体
char *inet_ntoa(struct in_addr in);//in_addr结构体转字符串IP
```

后四个函数解析见[IP地址转换函数](https://blog.csdn.net/Lurora0/article/details/117413828)。

## listen()函数

如果需要等待接收一个请求并处理它们，就要先listen()、再accept()。listen函数原型如下：

```cpp
int listen(int sockfd, int backlog);
```

`sockfd`是调用`socket()`返回的套接字文件描述符。`backlog`是在进入队列中允许的连接数目，进入的连接是在队列中一直等待直到你接受，它们的数目限制于队列的长度。如果将`backlog`的值设置为`SOMAXCONN`，就由系统来决定请求队列长度。

监听上面绑定的端口并设置队列长度为20的方法如下：

```cpp
if(listen(mSocket, 20) == -1)
{
    perror("listen");
    exit(1);
}
```

## accept()函数

当套接字处于监听状态时，可以通过`accept()`函数来接收客户端请求并返回一个新的套接字。它的原型为：

```cpp
#include <sys/socket.h>
int accept(int sock, struct sockaddr *addr, socklen_t *addrlen);
```

`sock`为服务器端套接字，`addr`为服务器`sockaddr_in`结构体变量，`addrlen`为参数`addr`的长度，可由`sizeof()`求得。

`accept()`会阻塞程序执行（后面代码不能被执行），直到有新的请求到来。

本例中`getConn()`函数实现方法如下

```cpp
std::list<int> connList;//用list来存放套接字，没有限制套接字的容量就可以实现一个server跟若干个client通信
void getConn()
{
    while(1)
    {
        int conn = accept(mSocket, (struct sockaddr*)&servaddr, &len);
        connList.push_back(conn);
        //printf("%d\n", conn);
        printf("玩家 %d 进入房间 \n",conn);
    }
}
```

## send() and recv()函数

这两个函数用于流式套接字或者数据报套接字的通讯，发生错误时返回-1。函数原型如下：

```cpp
int send(int sockfd, const void *msg, int len, int flags);
int recv(int sockfd, void *buf, int len, unsigned int flags);
```

`sockfd`发送或接收数据的套接字，`msg`是指向发送或接收数据的指针，`len`是数据的长度（注意不是由`sizeof()`获得），`flags`设置为`0`。

服务器端接收到一个用户发送的数据后，把消息广播到聊天室的程序实现如下所示：

```cpp
void sendMsg(int sender,char* msg,int length)
{
    char buf[1024];
    memset(buf, 0 ,sizeof(buf));
    sprintf(buf,"玩家%d:%s",sender ,msg);
    std::list<int>::iterator it;
    for(it=connList.begin(); it!=connList.end(); ++it)
    {
        //给其他人发送消息
        if (sender != *it)
        {
            send(*it, buf, strlen(buf), 0);
        }
    }
}
```

## select()函数

这部分应该置于上一节之前，因为服务器本来就是先从多路IO接收接收消息然后才广播出去的，但是因为理解比较困难，放在靠后的位置。函数原型如下：

```cpp
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
struct timeval {
    int tv_sec; /* seconds */
    int tv_usec; /* microseconds */
};
int select(int numfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);
```

### 参数解释

`maxfdp`：被监听的文件描述符的总数，它比所有文件描述符集合中的文件描述符的最大值大1，因为文件描述符是从0开始计数的；

`fd_set* readfds`是指向`fd_set`结构的指针，这个集合中应该包括文件描述符，我们是要监视这些文件描述符的读变化的，即我们关心是否可以从这些文件中读取数据了，如果这个集合中有一个文件可读，`select()`就会返回一个大于0的值，表示有文件可读，如果没有可读的文件，则根据`timeout`参数再判断是否超时，若超出`timeout`的时间，`select()`返回`0`，若发生错误返回负值。可以传入`NULL`值，表示不关心任何文件的读变化。

`fd_set* writefds`是指向`fd_set`结构的指针，这个集合中应该包括文件描述符，我们是要监视这些文件描述符的写变化的，即我们关心是否可以向这些文件中写入数据了，如果这个集合中有一个文件可写，`select()`就会返回一个大于0的值，表示有文件可写，如果没有可写的文件，则根据`timeout`再判断是否超时，若超出`timeout`的时间，`select()`返回`0`，若发生错误返回负值。可以传入`NULL`值，表示不关心任何文件的写变化。

`fe_set* errorfds`同上面两个参数的意图，用来监视文件错误异常。

`timeout`:用于设置`select`函数的超时时间，即告诉内核`select`等待多长时间之后就放弃等待。

第一：若将`NULL`以形参传入，即不传入时间结构，就是将`select()`置于阻塞状态，一定等到监视文件描述符集合中某个文件描述符发生变化为止；
第二：若将时间值设为`0`秒`0`毫秒，就变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回`0`，有变化返回一个正值；
第三：`timeout`的值大于`0`，这就是等待的超时时间，即`select`在`timeout`时间内阻塞，超时时间之内有事件到来就返回了，否则在超时后不管怎样一定返回，返回值同上述。

返回值：超时返回`0`;失败返回`-1`；成功返回大于`0`的整数，这个整数表示就绪描述符的数目。

用户首先将需要进行IO操作的`socket`添加到`select()`中，然后阻塞等待`select()`系统调用返回。当数据到达时，`socket()`被激活，`select()`函数返回。用户线程正式发起`read()`请求，读取数据并继续执行。

### 相关宏

下面是`select()`函数相关的常见的几个宏：

```cpp
#include <sys/select.h>   
int FD_ZERO(int fd, fd_set *fdset);   //一个 fd_set类型变量的所有位都设为 0
int FD_CLR(int fd, fd_set *fdset);  //清除某个位时可以使用
int FD_SET(int fd, fd_set *fd_set);   //设置变量的某个位置位
int FD_ISSET(int fd, fd_set *fdset); //测试某个位是否被置位
```

### 使用范例

当声明了一个文件描述符集后，必须用`FD_ZERO`将所有位置零。之后将我们所感兴趣的描述符所对应的位置位，操作如下：

```cpp
fd_set rset;   
int fd;   
FD_ZERO(&rset);   
FD_SET(fd, &rset);   
FD_SET(stdin, &rset);
```

然后调用`select()`函数，拥塞等待文件描述符事件的到来；如果超过设定的时间，则不再等待，继续往下执行。

```cpp
select(fd+1, &rset, NULL, NULL,NULL);
```

`select()`返回后，用`FD_ISSET`测试给定位是否置位：

```cpp
if(FD_ISSET(fd, &rset)   
{ 
    ... 
    //do something  
}
```

本例中服务器实现从客户端接收消息的程序如下，我觉得还有利用`fdset`属性的更好的实现方法。

```cpp
void getData()
{
    struct timeval tv;
    tv.tv_sec = 10;//设置倒计时时间
    tv.tv_usec = 0;
    while(1)
    {
        std::list<int>::iterator it;
        for(it=connList.begin(); it!=connList.end(); ++it)
        {
            fd_set rfds;
            int maxfd = 0;
            int retval = 0;

            FD_ZERO(&rfds);
            FD_SET(*it, &rfds);
            if(maxfd < *it)
                maxfd = *it;

            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
            
            if(retval == -1)
                printf("select error\n");
            else if(retval != 0)
            {
                char buf[1024];
                memset(buf, 0 ,sizeof(buf));
                int len = recv(*it, buf, sizeof(buf), 0);
                if (len > 0) {
                    printf("收到 %d的消息 %s", *it ,buf);
                    sendMsg(*it, buf, len);
                }
            }
        }
    }
}
```

## 其它

本例中服务器还用到多线程来实现同时检测连接和消息的到来。

```cpp
std::thread t(getConn);
t.detach();
std::thread t2(getData);
t2.detach();
```

# 客户端

## connect()函数

用来建立连接，它的原型为：

```cpp
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sock, struct sockaddr *serv_addr, socklen_t addrlen);  //Linux
```

参数和`bind()`函数差不多，不过要注意`serv_addr `是要连接服务器的结构体。

客户机使用`connect()`函数连接服务器的方法如下：

```cpp
    sock_cli = socket(AF_INET,SOCK_STREAM, 0);  //定义sockfd

    struct sockaddr_in servaddr;    //定义sockaddr_in
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  //连接服务器，成功返回0，错误返回-1
    {
        perror("connect");
        exit(1);
    }
```

## 客户端发送和接收消息

步骤和服务器端类似，注意理解描述符集中的`0`是`stdin`即可

```cpp
    while(1){ 
        FD_ZERO(&rfds); /*把可读文件描述符的集合清空*/
        FD_SET(0, &rfds);   /*把标准输入的文件描述符加入到集合中*/
        FD_SET(sock_cli, &rfds);   /*把当前连接的文件描述符加入到集合中*/
        if(maxfd < sock_cli)     /*找出文件描述符集合中最大的文件描述符*/
            maxfd = sock_cli;
        
        tv.tv_sec = 10; /*设置超时时间*/
        tv.tv_usec = 0;
        
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);   /*等待聊天*/
        if(retval == -1){
            printf("select出错，客户端程序退出\n");
            break;
        }else if(retval == 0){
            continue;   //printf("客户端没有任何输入信息，并且服务器也没有信息到来，waiting...\n");
        }else{
            if(FD_ISSET(sock_cli,&rfds)){    /*服务器发来了消息*/
                char recvbuf[BUFFER_SIZE];
                int len;
                len = recv(sock_cli, recvbuf, sizeof(recvbuf),0);
                if(len > 0)
                {
                    printf("%s",recvbuf);
                }
                memset(recvbuf, 0, sizeof(recvbuf));
            }
            if(FD_ISSET(0, &rfds)){  /*用户输入信息了,开始处理信息并发送*/
                char sendbuf[BUFFER_SIZE];
                fgets(sendbuf, sizeof(sendbuf), stdin);
                send(sock_cli, sendbuf, strlen(sendbuf),0); //发送
                memset(sendbuf, 0, sizeof(sendbuf));
            }
        }
    }
```

# 完整代码

服务器端：

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <thread>
#include <list>
#include <iostream>

#define PORT 8606
#define IP "127.0.0.1"

int mSocket;
struct sockaddr_in servaddr;
socklen_t len;
std::list<int> connList;    //用list来存放套接字，没有限制套接字的容量就可以实现一个server跟若干个client通信

void sendMsg(int sender,char* msg,int length)
{
    char buf[1024];
    memset(buf, 0 ,sizeof(buf));
    sprintf(buf,"玩家%d:%s",sender ,msg);
    std::list<int>::iterator it;
    for(it=connList.begin(); it!=connList.end(); ++it)
        if (sender != *it)  //给其他人发送消息
            send(*it, buf, strlen(buf), 0);
}

void getConn()
{
    while(1)
    {
        int conn = accept(mSocket, (struct sockaddr*)&servaddr, &len);
        connList.push_back(conn);
        printf("玩家 %d 进入房间 \n",conn);
    }
}

void getData()
{
    struct timeval tv;
    tv.tv_sec = 10;//设置倒计时时间
    tv.tv_usec = 0;
    while(1)
    {
        std::list<int>::iterator it;
        for(it=connList.begin(); it!=connList.end(); ++it)
        {
            fd_set rfds;
            int maxfd = 0;
            int retval = 0;

            FD_ZERO(&rfds);
            FD_SET(*it, &rfds);
            if(maxfd < *it)
                maxfd = *it;

            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
            
            if(retval == -1)
                printf("select error\n");
            else if(retval != 0)
            {
                char buf[1024];
                memset(buf, 0 ,sizeof(buf));
                int len = recv(*it, buf, sizeof(buf), 0);
                if (len > 0) {
                    printf("收到 %d的消息 %s", *it ,buf);
                    sendMsg(*it, buf, len);
                }
            }
        }
    }
}

int main()
{
    std::cout<<"1.创建socket"<<std::endl;
    mSocket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);    //设置服务器监听的端口

    //如果是局域网，这里可以使用IP127.0.0.1
    //servaddr.sin_addr.s_addr = inet_addr(IP);
    //如果部署到linux服务器 这里使用htonl(INADDR_ANY)
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout<<"2.绑定socket和端口号"<<std::endl;
    if(bind(mSocket, (struct sockaddr* ) &servaddr, sizeof(servaddr))==-1)
    {
        perror("bind");
        exit(1);
    }
    std::cout<<"3.监听端口号"<<std::endl;

    if(listen(mSocket, 20) == -1)
    {
        perror("listen");
        exit(1);
    }
    len = sizeof(servaddr);

    std::thread t(getConn);
    t.detach();//detach的话后面的线程不同等前面的进程完成后才能进行，如果这里改为join则前面的线程无法判断结束，就会
    //一直等待，导致后面的线程无法进行就无法实现操作

    std::thread t2(getData);
    t2.detach();

    while(1);

    return 0;
}
```

客户端：

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MYPORT  8606
#define BUFFER_SIZE 1024
int main()
{
    int sock_cli;
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd;

    sock_cli = socket(AF_INET,SOCK_STREAM, 0);  //定义sockfd

    struct sockaddr_in servaddr;    //定义sockaddr_in
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  ///服务器端口
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1127.0.0.1");  ///服务器ip

    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)  //连接服务器，成功返回0，错误返回-1
    {
        perror("connect");
        exit(1);
    }

    maxfd = 0;  
    while(1){ 
        FD_ZERO(&rfds); /*把可读文件描述符的集合清空*/
        FD_SET(0, &rfds);   /*把标准输入的文件描述符加入到集合中*/
        FD_SET(sock_cli, &rfds);   /*把当前连接的文件描述符加入到集合中*/
        if(maxfd < sock_cli)     /*找出文件描述符集合中最大的文件描述符*/
            maxfd = sock_cli;
        
        tv.tv_sec = 10; /*设置超时时间*/
        tv.tv_usec = 0;
        
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);   /*等待聊天*/
        if(retval == -1){
            printf("select出错，客户端程序退出\n");
            break;
        }else if(retval == 0){
            continue;   //printf("客户端没有任何输入信息，并且服务器也没有信息到来，waiting...\n");
        }else{
            if(FD_ISSET(sock_cli,&rfds)){    /*服务器发来了消息*/
                char recvbuf[BUFFER_SIZE];
                int len;
                len = recv(sock_cli, recvbuf, sizeof(recvbuf),0);
                if(len > 0)
                {
                    printf("%s",recvbuf);
                }
                memset(recvbuf, 0, sizeof(recvbuf));
            }
            if(FD_ISSET(0, &rfds)){  /*用户输入信息了,开始处理信息并发送*/
                char sendbuf[BUFFER_SIZE];
                fgets(sendbuf, sizeof(sendbuf), stdin);
                send(sock_cli, sendbuf, strlen(sendbuf),0); //发送
                memset(sendbuf, 0, sizeof(sendbuf));
            }
        }
    }
    close(sock_cli);
    return 0;
}
```

**参考：[C++知识分享： Socket 编程详解，万字长文](https://zhuanlan.zhihu.com/p/405416697)、[htons()和htonl()函数](https://www.cnblogs.com/rexienk/p/4858710.html)、[C/C++ socket编程教程：1天玩转socket通信技术](http://c.biancheng.net/cpp/socket/)、[IP地址转换函数inet_addr(),inet_aton(),inet_ntoa(),int_pton()](https://blog.csdn.net/Lurora0/article/details/117413828)、[socket编程之select](https://blog.csdn.net/weixin_41010318/article/details/80257177)、[socket通信中select函数的使用和解释](https://www.cnblogs.com/renyuan/p/5100184.html)。**
