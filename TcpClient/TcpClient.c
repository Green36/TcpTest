
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

#define PORT "5000"

#define DEBUG_LOG printf("%s(%d)\n", __FUNCTION__, __LINE__);
#define ERROR_RETURN \
    { \
        printf("%s(%d)\n", __FUNCTION__, __LINE__ ); \
        goto ErrorReturn; \
    }


void sock_print(char* str, int family, int socktype)
{

}

void sockaddr_print(char* str, struct sockaddr* addr, socklen_t len)
{

}

// サーバに接続する。
int connect_to_server(
        const char* hostname,  // IPv4 or IPv6ホスト名
        const char* service)   // ポート番号（の文字列）
{
    int sockfd;
    int err;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4/IPv6両対応
    hints.ai_socktype = SOCK_STREAM;
    // serviceはポート番号でなければならない。AI_NUMERICSERV を指定しなければ、
    // 'pop'などでもよい。
    hints.ai_flags = AI_NUMERICSERV;

    err = getaddrinfo(hostname, service, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo(): %s\n", gai_strerror(err));
        ERROR_RETURN;
    }

    for (ai = res; ai; ai = ai->ai_next) {
        sockaddr_print("connect...", ai->ai_addr, ai->ai_addrlen);
        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sockfd < 0) ERROR_RETURN;

        if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
            close(sockfd);
            printf("%s(%d) connect error\n", __FUNCTION__, __LINE__ );
            sockfd = -1;
            continue;
        }
        // ok
        sockaddr_print("connected", ai->ai_addr, ai->ai_addrlen);
        break;
    }
    freeaddrinfo(res);
    return sockfd;

ErrorReturn:
    return -1;
}


int main()
{
    int sockfd;
    char buff[255];
    char hostname[128];

    gethostname(hostname , sizeof(hostname) );
    printf("%s\n", hostname);

    for(;;)
    {
        sockfd = connect_to_server(hostname, PORT);
        if (sockfd < 0) {
            perror("client");
            return 1;
        }

        memset(buff, 0x00, sizeof(buff));
        printf("> ");
        if( scanf("%s", buff ) == EOF ) ERROR_RETURN;
        if( write(sockfd, buff, sizeof(buff)) < 0 ) ERROR_RETURN;
        if( read(sockfd, buff, sizeof(buff)) < 0 ) ERROR_RETURN;
        printf("from server = '%s'\n", buff);
    }

    close(sockfd);
    return 0;

ErrorReturn:
    perror("client");
    close(sockfd);
    return -1;
}

