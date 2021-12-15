#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "includes/sock.h"
#include <pthread.h>
#include <csignal>
#include <chrono>
#include <string>
#include <iostream>
#include "userCreation.h"

#ifdef Win32
#include <cygwin/signal.h>
#endif

void *getMessagesFromClient(void *vargp);
void *sendMessagesToClient(void *vargp);
void sigInterruptHandler(int sigNum);
int client_fd;
bool closeWindow = false;

void report(const char* msg, int terminate) {
    perror(msg);
    if (terminate) exit(-1); /* failure */
}

int main() {
    system("clear");
    int fd = socket(AF_INET,     /* network versus AF_LOCAL */
                    SOCK_STREAM, /* reliable, bidirectional, arbitrary payload size */
                    0);          /* system picks underlying protocol (TCP) */
    if (fd < 0) report("socket", 1); /* terminate */

    /* bind the server's local address in memory */
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));          /* clear the bytes */
    saddr.sin_family = AF_INET;                /* versus AF_LOCAL */
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* host-to-network endian */
    saddr.sin_port = htons(PortNumber);        /* for listening */

    if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
        report("bind", 1); /* terminate */

    /* listen to the socket */
    if (listen(fd, MaxConnects) < 0) /* listen for clients, up to MaxConnects */
        report("listen", 1); /* terminate */

    fprintf(stderr, "Listening on port %i...\n", PortNumber);

//    std::string name = "Jean";
//    std::string surname = "Borg";
//    std::string username = "jdawg";
//    std::string password = "12345";
//    createUser(name, surname, username, password);

    auto previousTime = std::chrono::system_clock::now().time_since_epoch().count();
    /* a server traditionally listens indefinitely */
    while (!closeWindow) {
        struct sockaddr_in caddr; /* client address */
        int len = sizeof(caddr);  /* address length could change */

        client_fd = accept(fd, (struct sockaddr*) &caddr, reinterpret_cast<socklen_t *>(&len));  /* accept blocks */
        if (client_fd < 0) {
            report("accept", 0); /* don't terminate, though there's a problem */
            continue;
        }

        while(true)
        {
            auto currentTime = std::chrono::system_clock::now().time_since_epoch().count();
            if(currentTime - previousTime >= 1000/1) {
                pthread_t get_messages_thread_id;
                pthread_create(&get_messages_thread_id, NULL, getMessagesFromClient, NULL);
                //pthread_join(get_messages_thread_id, NULL);

                pthread_t send_messages_thread_id;
                pthread_create(&send_messages_thread_id, NULL, sendMessagesToClient, NULL);
                //pthread_join(send_messages_thread_id, NULL);

                //detects ctrl c
                sighandler_t sigHandler = signal(SIGINT, sigInterruptHandler);
                previousTime = currentTime;
            }
        }
    }
    return 0;
}

void *getMessagesFromClient(void *vargp)
{
    char buffer[BuffSize + 1];
    memset(buffer, '\0', sizeof(buffer));
    if (read(client_fd, buffer, sizeof(buffer)) > 0)
    {
        // puts(buffer);
        printf("Client: %s", buffer);
    }
    return NULL;
}

void *sendMessagesToClient(void *vargp)
{
    char textToSend[20];
    fgets(textToSend, 20, stdin);
    write(client_fd, textToSend, strlen(textToSend));

    return NULL;
}

void sigInterruptHandler(int sigNum)
{
    close(client_fd); /* break connection */
    closeWindow = true;
}