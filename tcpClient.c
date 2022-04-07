// Charles "Blue" Kent Hartsell III

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <arpa/inet.h>
#define MAX 80
#define SA struct sockaddr

// function for sending file
int revSend(int sockfd, char *filename) {
  char *buff = NULL;
  char name[strlen(filename)];
  size_t buffsize = sizeof(buff);
  int n;

  // open file
  FILE *fp = fopen(filename, "r");
  if(fp == NULL) {
    printf("REVSEND: File failed to open, exiting...\n");
    return -1;
  }

  // clear buffer
  buff = NULL;

  // write filename to socket
  write(sockfd, filename, (strlen(filename) * sizeof(filename)));

  // loop through file
  while(getline(&buff, &buffsize, fp) > 0) {
    // write buffer to socket
    write(sockfd, buff, sizeof(buff));

    // clear buffer
    buff = NULL;
  }

  // close file and exit function
  free(buff);
  fclose(fp);
  return 0;
}

// main driver
int main(int argc, char *argv[]) {
  int sockfd, connfd, port;
  struct sockaddr_in servaddr, cli;

  // set port
  if(argc != 4) {
    printf("ERROR: The correct format is ./tcpClient <IP> <port> <filename>\n");
    exit(0);
  }

  // create and verify socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) {
    printf("CLIENT: Socket creation failed...\n");
    exit(0);
  } else {
    printf("CLIENT: Socket successfully created...\n");
  }
  bzero(&servaddr, sizeof(servaddr));

  // assign IP and port
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]); // in SEED, server IP is 10.8.0.1
  servaddr.sin_port = htons(atoi(argv[2]));

  // connect client socket to server socket
  if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("CLIENT: Connection with the server failed...\n");
    exit(0);
  } else {
    printf("CLIENT: Connected to server...\n");
  }

  // function for sending file
  revSend(sockfd, argv[3]); // argv[3] is filename

  // close the socket
  close(sockfd);
}
