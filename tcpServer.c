// Charles "Blue" Kent Hartsell III

// Includes and defines
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#define MAX 80
#define SA struct sockaddr

// Function for client->server datastream
int revWrite(int connfd) {
  // Setup
  char buff[MAX];
  int n, rret;
  FILE *fp;
  bzero(buff, MAX);

  // get ASCII name for client side file
  // Read line from client, copy to buffer
  rret = read(connfd, buff, sizeof(buff));
  if(rret < 0) {
    printf("REVWRITE: Read error, exiting...\n");
    // send ERR to client
    return -1;
  }

  // SEND ACK

  // create server side file (filename: SERVERSIDE_clientFileName.txt)
  fp = fopen(strcat("SERVERSIDE_", buff), "w");

  // receive lines from client and copy to file
  while(1) {
    // Zero buffer
    bzero(buff, MAX);
    // Read from client, copy to buffer
    rret = read(connfd, buff, sizeof(buff));
    if(rret < 0) {
      printf("REVWRITE: Read error, exiting...\n");
      // send ERR to client
      return -1;
    }

    // SEND ACK

    // If EOF not reached, continue. If reached, exit
    if(buff[0] != EOF && buff != NULL && buff[0] != '\n') {
      // Reverse the line (copied this code from an old assignment I did)
      int len = strlen(buff);
      char str[len];
      strcpy(buff, str);
      int left = 0;
      int right = len - 1;
      for(int i = left; i < right; i++) {
        char temp = str[i];
        str[i] = str[right];
        str[right] = temp;
        right--;
      }
      puts(str);

      // Write to file
      fputs(buff, fp);
    } else {
      // EOF reached, done!
      fclose(fp); return 0;
    }
  }

  // this code should never be reached. error, return -1
  fclose(fp);
  return -1;
}

// main driver
int main(int argc, char *argv[]) {
  int sockfd, connfd, len, port;
  struct sockaddr_in servaddr, cli;



  // Tag
  printf("\n\nAUTHOR: tcpServer.c is not authorized for use without explicit permission from its author.\n\n");

  // set port
  if(argc != 2) {
    printf("ERROR: The correct format is ./tcpServer <portNumber>\n");
    exit(0);
  } else {
    port = atoi(argv[1]);
    printf("SERVER: Using port %i...\n", port);
  }

  // create and verify socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) {
    // creation failed
    printf("SERVER: Socket creation failed...\n");
    exit(0);
  } else {
    printf("SERVER: Socket successfully created...\n");
  }
  // zero out servaddr
  bzero(&servaddr, sizeof(servaddr));

  // assign IP and port
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // bind socket to IP and verify
  if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("SERVER: Socket bind failed...\n");
    exit(0);
  } else {
    printf("SERVER: Socket bind success...\n");
  }

  // Listen for client
  if((listen(sockfd, 5)) != 0) {
    printf("SERVER: Listen failed...\n");
    exit(0);
  } else {
    printf("SERVER: Listening...\n");
  }
  len = sizeof(cli);

  // Accept data from client
  connfd = accept(sockfd, (SA*)&cli, &len);
  if(connfd < 0) {
    printf("SERVER: Accept failed...\n");
    exit(0);
  } else {
    printf("SERVER: Client accepted...\n");
  }

  // Function for client/server data transfer
  revWrite(connfd);

  // send DONE to client

  // Close Socket
  close(sockfd);

  // Goodbye message
  printf("SERVER: Exiting. Have a nice day!\n");

  // catch-all exit
  return 0;
}
