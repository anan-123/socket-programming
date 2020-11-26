#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};


  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // This is to lose the pesky "Address already in use" error message
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
      perror("setsockopt");
      exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
    sizeof(address))<0)
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
    (socklen_t*)&addrlen))<0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    while(1)
    {
      char hi[1024]={0};
      valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer

      int j=0;
      char *token_arr[1000];
      token_arr[j] = strtok(buffer, " \n\t\r");
      while(token_arr[j]!=NULL)
      {
        j++;
        token_arr[j] = strtok(NULL, " \n\t\r");
      }
      if(strcmp(token_arr[0],"exit")==0)
      {
        return 0;
      }
      for(int i=1;i<j;i++)
      {

        int fp_input=open(token_arr[i],O_RDONLY);
        if(fp_input==-1)
        {
          perror("file opening error:");
          return 0;
        }
        //getting the size of the input file
        struct stat sb;
        fstat(fp_input,&sb);
        int size=sb.st_size;

        char sizestr[1000];
        sprintf(sizestr,"%d",size);
        send(new_socket,sizestr,sizeof(sizestr),0);
        read(new_socket,hi,1024);

        char fbuf[10000];
        int left_size=size%10000;
        int block=0;
        for(int i=(size/10000);i>0;i--)
        {
          read(fp_input,fbuf,10000);
          write(new_socket , fbuf , 10000);

        }

        read(fp_input,fbuf,left_size);
        write(new_socket , fbuf , left_size);


      }
    }

    return 0;
  }
