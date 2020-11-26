#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define PORT 8000


int main(int argc, char const *argv[])
{
  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char *hello = "Hello from client";
  char buffer[1024] = {0};
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
  // which is meant to be, and rest is defined below

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Converts an IP address in numbers-and-dots notation into either a
  // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
  if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
  {
    printf("\nConnection Failed \n");
    return -1;
  }
  else
  {
    printf("\nconnected to server\n");
  }
  while(1)
  {
    printf("client> ");

    //variables for taking input
    size_t size_i = 100;
    char* input_str = (char *)malloc(size_i*sizeof(char));
    char *token_arr[1000];

    //taking input
    if(getline(&input_str,&size_i,stdin)==-1)
    {
      printf("error\n");
      return 0;
    }
    send(sock , input_str , strlen(input_str) , 0 );  // send the input string
    int i = 0,j=0;
    token_arr[j] = strtok(input_str, " \n\t\r");
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
      float percent;
      char sizestr[1024]={0};

      read(sock,sizestr,1024);
      int sizer = atoi(sizestr);

      char *hi="recieved the sizee";
      send(sock,hi,sizeof(hi),0);
      int fp_output=open(token_arr[i],O_CREAT | O_WRONLY,0600);
      if(fp_output==-1)
      {
        perror("program");
        return 0;
      }

      char fbuff[10000];
      int left_size=sizer%10000;
      int block=0;
      for(int i=(sizer/10000);i>0;i--)
      {
        percent=(10000*(block))*100/sizer;
        printf("\rthe percent of file written : %0.2f",percent);
        fflush(stdout);
        read( sock , fbuff, 10000);
        write(fp_output,fbuff,10000);
        block++;
      }
      for(int i=0;i<left_size;i++)
      {
        percent=(((10000*(block))+(float)(i+1))*100)/sizer;

        printf("\rthe percent of file written : %0.2f",percent);
        fflush(stdout);
      }
      printf("\n");
      read(sock,fbuff,left_size);
      write(fp_output,fbuff,left_size);
    }


  }



  return 0;
}
