//
// http://www.nslabs.jp/socket.rhtml
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define BACKLOG 5
#define PORT "5000"

#define ERROR_RETURN \
    { \
        printf("%s(%d)\n", __FUNCTION__, __LINE__ ); \
        return -1; \
    }

void sock_print(char* str, int family, int socktype)
{

}

void sockaddr_print(char* str, struct sockaddr* addr, socklen_t len)
{

}


// 戻り値 <0   エラー
//        >=0  listenしているソケット
int tcp_listen(const char* service)
{ // サービス名 or ポート番号（の文字列）
    int err;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;    // AF_INET6は、IPv4/IPv6両方を受け付ける。
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;   // bind()する場合は指定する。

    // node = NULLのとき、INADDR_ANY, IN6ADDR_ANY_INIT に相当。
    err = getaddrinfo(NULL, service, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo(): %s\n", gai_strerror(err));
        return -1;
    }

    ai = res;
    sock_print("create socket", ai->ai_family, ai->ai_socktype);
    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) ERROR_RETURN;

    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) ERROR_RETURN;

    printf("set SO_REUSEADDR\n");

    if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) ERROR_RETURN;

    if (listen(sockfd, BACKLOG) < 0) ERROR_RETURN;

    sockaddr_print("listen succeeded", ai->ai_addr, ai->ai_addrlen);

    freeaddrinfo(res);
    return sockfd;
}

int test_server(){
    int sockfd;
    char buff[1024];

    sockfd = tcp_listen(PORT);
    if (sockfd < 0) {
        perror("server");
        exit(1);
    }

    printf("wait...\n");

    while (1) {
        int cs;
        struct sockaddr_storage sa;  // sockaddr_in 型ではない。
        socklen_t len = sizeof(sa);  // クライアントの情報を得る場合
        cs = accept(sockfd, (struct sockaddr*) &sa, &len);
        if (cs < 0)
        {
            if (errno == EINTR) continue;
            perror("accept");
            exit(1);
        }

        printf("accepted.\n");
        sockaddr_print("peer", (struct sockaddr*) &sa, len);

        if (fork() == 0)
        {
            memset(buff, 0x00, sizeof(buff));

            // 子プロセス
            close(sockfd);

            if( read(cs, &buff, 1) < 0) ERROR_RETURN;
            if( write(cs, &buff, 1) < 0) ERROR_RETURN;

            close(cs);
            exit(0);
        }
        close(cs);
    }
    return 0;
}

int main()
{
    test_server();
    return 0;
}

