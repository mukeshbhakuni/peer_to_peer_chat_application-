
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>


typedef struct clientInfo
{
    int clientSocket;
    struct sockaddr_in clientAddress;
}clientInfo;

typedef struct listenerInfo
{
    int listenerSocket,connSocket;
    struct sockaddr_in listenerAddress, clientAddr;
}listenerInfo;


clientInfo *clSocket(char *ipaddr, int port)
{
    clientInfo *sock;
    sock=(clientInfo *) malloc (sizeof(clientInfo));
    memset(&sock->clientAddress,'0',sizeof(sock->clientAddress));
    sock->clientSocket = socket(AF_INET,SOCK_STREAM,0);

        if (sock -> clientSocket == -1)
        {
            printf("\nunable to initialise client");
            exit(1);
        }
    sock->clientAddress.sin_family = AF_INET; 
    sock->clientAddress.sin_port = htons(port); // network byte order 
    sock->clientAddress.sin_addr.s_addr = inet_addr(ipaddr); 
    //memcpy(&sock->clientAddress.sin_addr.s_addr,inet_addr(ipaddr),sizeof(inet_addr(ipaddr)));
    return sock;
}



listenerInfo *ltSocket(int port)
{
 listenerInfo *sock;
 sock=(listenerInfo *)malloc(sizeof(listenerInfo));
 sock -> listenerSocket=socket(AF_INET,SOCK_STREAM,0);

 if(sock -> listenerSocket==-1)
 {
   perror("unable to initialise listener");
   exit(1);
 }
 
 memset(&sock -> listenerAddress,'0',sizeof(sock -> listenerAddress));   

 sock->listenerAddress.sin_family = AF_INET;
 sock->listenerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
 sock->listenerAddress.sin_port=htons(port);

 if(bind(sock -> listenerSocket, (struct sockaddr *) &sock -> listenerAddress , sizeof(sock -> listenerAddress) ) == -1)
 {
     printf("unable to bind listenerSocket");
     exit(1);
 }

 if(listen(sock -> listenerSocket,50) == -1)
 {
    printf("unable to listen ");
    exit(1);
 }

 return sock;
}


char *fetchIPaddr()
{
  FILE *fp;
  static char uname[40],ipaddr[40];
  int cnt=1,n;
  fp = fopen("addressBook","r+");
  while(fscanf(fp,"%s %s",uname,ipaddr)==2)
  { 
    printf("%d <-> %s\n",cnt,uname);
    cnt++;
  }
  fclose(fp);
  printf("select reciever :");
  scanf("%d",&n);
  cnt = 1;
  fp = fopen("addressBook","r+");
  while(fscanf(fp,"%s %s",uname,ipaddr))
  { 
    if (cnt == n)
    {
      n = strlen(ipaddr);
      ipaddr[n] = '\0';
      return ipaddr;
    } 
    cnt++;
  }
  fclose(fp);
}

int main()
{

  int pid,k;
   char uname[40],buffer[1024],msg[1024];
   char *ipaddr;

   system("clear");

   printf("Enter uname _| ");
   scanf("%s",uname);

   pid = fork();
   if (pid == 0) 
   {
       
     clientInfo *sock;
     while(1)
     { 
       ipaddr = fetchIPaddr();
       sock = clSocket(ipaddr,55555);
       
       k = (connect(sock->clientSocket , (struct sockaddr *) &sock->clientAddress , sizeof(sock->clientAddress)));
       printf("hello");
       if(k!=-1)
       {
         memset(msg,'\0',1024);
         memset(buffer,'\0',1024);
         printf("type message  _| ");
         scanf("%s",msg);
         strcat(buffer,msg);
         strcat(buffer ," |from| ");
         strcat(buffer,uname);
         write(sock->clientSocket,buffer,1024); 
       }
       close(sock->clientSocket);
     }
   }

   else 
   {
     int len;  
     listenerInfo *sock;
     sock=ltSocket(55555);
     len = sizeof(sock->clientAddr);
     while(1)
     {
       if ((sock->connSocket=accept(sock->listenerSocket , (struct sockaddr *) &sock->clientAddr  , &len )) != -1)
       {
         memset(msg,'\0',1024);
         read(sock->connSocket,msg,1024); 
         printf("%s\n",msg);
       }
       close(sock->connSocket);
     }
   }
}

