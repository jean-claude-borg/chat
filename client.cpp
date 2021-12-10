#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <chrono>
#include <pthread.h>
#include "sock.h"

void *getMessagesFromServer(void *vargp);
void *sendMessagesToServer(void *vargp);

int sockfd;

void report(const char* msg, int terminate) {
    perror(msg);
    if (terminate) exit(-1); /* failure */
}

int main() {
    system("clear");
    /* fd for the socket */
    sockfd = socket(AF_INET,      /* versus AF_LOCAL */
                    SOCK_STREAM,  /* reliable, bidirectional */
                    0);           /* system picks protocol (TCP) */
    if (sockfd < 0) report("socket", 1); /* terminate */

    /* get the address of the host */
    struct hostent *hptr = gethostbyname("127.0.0.1"); /* localhost: 127.0.0.1 */
    if (!hptr) report("gethostbyname", 1); /* is hptr NULL? */
    if (hptr->h_addrtype != AF_INET)       /* versus AF_LOCAL */
        report("bad address family", 1);

    /* connect to the server: configure server's address 1st */
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr =
            ((struct in_addr *) hptr->h_addr_list[0])->s_addr;
    saddr.sin_port = htons(PortNumber); /* port number in big-endian */

    if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
        report("connect", 1);

    /* Write some stuff and read the echoes. */
    puts("Server Connection Success...");

    auto previousTime = std::chrono::system_clock::now().time_since_epoch().count();
    while (true) {
        auto currentTime = std::chrono::system_clock::now().time_since_epoch().count();
        if (currentTime - previousTime >= 1000 / 1) {
            pthread_t get_messages_thread_id;
            pthread_create(&get_messages_thread_id, NULL, getMessagesFromServer, NULL);
            //pthread_join(get_messages_thread_id, NULL);

            pthread_t send_messages_thread_id;
            pthread_create(&send_messages_thread_id, NULL, sendMessagesToServer, NULL);
            //pthread_join(send_messages_thread_id, NULL);

            //detects ctrl c
            // sighandler_t sigHandler = signal(SIGINT, sigInterruptHandler);
            previousTime = currentTime;
        }
    }
    puts("Client done, about to exit...");
    close(sockfd); /* close the connection */
    return 0;
}

void *getMessagesFromServer(void *vargp)
{
    char buffer[BuffSize + 1];
    memset(buffer, '\0', sizeof(buffer));
    if (read(sockfd, buffer, sizeof(buffer)) > 0)
    {
        //puts(buffer);
        printf("Server: %s", buffer);
    }

    return NULL;
}

void *sendMessagesToServer(void *vargp)
{
    char textToSend[20];
    fgets(textToSend, 20, stdin);
    write(sockfd, textToSend, strlen(textToSend));

    return NULL;
}