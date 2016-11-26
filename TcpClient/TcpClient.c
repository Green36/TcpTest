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

#define PORT "5000"

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
        return -1;
    }

    for (ai = res; ai; ai = ai->ai_next) {
        sockaddr_print("connect...", ai->ai_addr, ai->ai_addrlen);
        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sockfd < 0)
            return -1;
        if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        // ok
        sockaddr_print("connected", ai->ai_addr, ai->ai_addrlen);
        break;
    }
    freeaddrinfo(res);
    return sockfd;
}


int main()
{
    int sockfd;
    char ch;

    char hostname[128];

    gethostname(hostname , sizeof(hostname) );
    printf("%s\n", hostname);

    sockfd = connect_to_server(hostname, PORT);
    if (sockfd < 0) {
        perror("client");
        return 1;
    }

    ch = 'A';
    write(sockfd, &ch, 1);
    read(sockfd, &ch, 1);
    printf("char from server = '%c'\n", ch);

    close(sockfd);
    return 0;
}

