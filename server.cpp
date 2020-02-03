#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
// The use of sockets in this program were derived from the notes of Professor Carlos Rincon
using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

string getword(char a, string word)
{
    int len=word.length();
    string newword;
    for(int i=0;i<len;i++)
    {
        if(word[i]!=a)
            newword += word[i];
    }
    return newword;
}

string getcode(char a, string word)
{
    int len=word.length();
    string newsymbol="";
    for(int i=0;i<len;i++)
    {
        if(word[i]!=a) {
            newsymbol+='0';
        }
        else
            newsymbol+='1';
    }
    return newsymbol;
}

string converttostring(char* buffer)
{
    string newstr="";
    int size=strlen(buffer);
    for(int i=0;i<size;i++)
    {
        newstr+=buffer[i];
    }
    return newstr;
}

void dostuff(int sock)
{
	int n;
    char buffer[100];
    n = read(sock,buffer,100);
    if (n < 0) error("ERROR reading from socket");
    string input=converttostring(buffer);
    int sz=input.length()-1;
    input=input.substr(0,sz);
    char ch=buffer[sz];
    string code=getcode(ch,input);
    string word=getword(ch,input);
    bzero(buffer,100);
    strcpy (buffer, code.c_str());
    n = write(sock,buffer,100);
    if (n < 0) error("ERROR writing to socket");
    bzero(buffer,100);
    strcpy (buffer, word.c_str());
    n = write(sock,buffer,100);
    if (n < 0) error("ERROR writing to socket");
    //cout<<code<<" "<<word<<endl;
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
    signal(SIGCHLD, fireman);      
    while (1) {
        newsockfd = accept(sockfd, 
              (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0)  {
            close(sockfd);
            //cout<<"Child was born"<<endl;
            dostuff(newsockfd);
            exit(0);
        }
         else close(newsockfd);
    } /* end of while */
    return 0; /* we never get here */
}
