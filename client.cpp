#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
using namespace std;

struct command
{
    char letter;
    string word="";
    string symbol;
    int sockfd;
};

void error(char* msg)
{
    perror(msg);
    exit(0);
}

void printword(string word)
{
    string newword;
    int len=word.length();
    for(int i=0;i<len;i++)
    {
        if(int(word.at(i))==10)
            newword+="<EOL>";
        else
            newword+=word.at(i);
    }
    cout<<"Remaining Message:   "<<newword<<endl;
}
void printcode(string word,char letter)
{
    string newword;
    int len=word.length();
    for(int i=0;i<len;i++)
    {
        if(int(word.at(i))==10)
            newword+="<EOL>";
        else
            newword+=word.at(i);
    }
    if(int(letter)==10)
        cout<<"Symbol <EOL> code:   "<<newword<<endl;
    else
        cout<<"Symbol "<<letter<<" code:       "<<newword<<endl;
}

void *sendtoserver(void *cmd){
    //FIX THIS FUNCTION AND FIGURE OUT HOW TO APPEND INPUT AND LETTER INTO 1
    cout<<"CONNECTED"<<endl;
    struct command *cmdp = (struct command *)cmd;
    int n;
    char letter=cmdp->letter;
    int sockfd=cmdp->sockfd;
    string word=cmdp->word;
    string symbol=cmdp->symbol;
    string letterthenword=letter+word;
    cout<<letterthenword<<endl;
    char buffer[letterthenword.size() + 1];
    letterthenword.copy(buffer, sizeof(letterthenword) + 1);
    buffer[letterthenword.size()] = '\0';
    n = write(sockfd,buffer,sizeof(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    word="";
    for (int i = 0; i < sizeof(buffer); i++) {
        word = word + buffer[i];
    }
    bzero(buffer, 256);
    n = read(sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    symbol="";
    for (int i = 0; i < sizeof(buffer); i++) {
        symbol = symbol + buffer[i];
    }
    cmdp->symbol=symbol;
    cmdp->word=word;
    cout<<symbol<<" "<<word<<endl;
    close(sockfd);
    /*
    word.copy(buffer,sizeof(word)+1);
    n = write(sockfd,buffer,sizeof(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer, sizeof(buffer));
    n = read(sockfd,buffer,sizeof(buffer));
    if (n < 0)
        error("ERROR reading from socket");
    cmdp->word=convertTostring(buffer,(sizeof(buffer)/sizeof(char)));
    bzero(buffer, sizeof(buffer));
    if (n < 0)
        error("ERROR reading from socket");
    cmdp->symbol=convertTostring(buffer,(sizeof(buffer)/sizeof(char)));
    bzero(buffer, sizeof(buffer));
    close(sockfd);
     */
}

int main(int argc, char *argv[])
{
    //Connect to the socket
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    //Find the frequencies of each character
    string input;
    char ch;
    while(cin.get(ch))
    {
        input+=ch;
    }
    char arr[62];//array of ascii values
    int freq[62];//array of frequencies for each character+<EOL>
    int c = 0;
    for (int i = 0; i < 63; i++) { // LOADS IN VALUES FOR EACH ASCII CHARACTER BESIDES <EOL>
        if(i==0){
            arr[i] = char(10);
        }else if (i < 11) {
            arr[i] = char(i + 47);
        } else if (i < 37) {
            arr[i] = char(i + 54);
        } else if (i < 63) {
            arr[i] = char(i + 60);
        }
        freq[i]=0;
    }
    int z=input.length(); //FINDS FREQUENCY OF EACH CHARACTER
    for (int i = 0; i < 63; i++) { //Looks through each character
        for (int j = 0; j < input.length(); j++){ //looks through the entire input
            if (arr[i] == input.at(j))
                freq[i]++;
        }
        if (freq[i] != 0) {
            c++;
        }
    }
    int ar[c][2];//CREATES A TWO DIMENSIONAL ARRAY CONTAINING THE CHARACTER'S FREQUENCY AND ASCII NUMBER RESPECTIVELY
    //for(int i=0;i<63;i++)
    //  for(int j=0;j<2;j++)
    //    ar[i][j]=0;
    int d = 0;
    for (int i = 0; i < 63; i++) {
        if (freq[i] != 0) {
            ar[d][0] = freq[i];
            ar[d][1] = int(arr[i]);
            d++;
        }
    }
    int a[d][2];
    int mx;
    int g = d;
    cout<<endl;
    for (int i = 0; i < d; i++) { //finds the current maximum value amongst the current array and loads it into the new array so the new array is in order.
        mx = 0;
        for (int j = 0; j < g; j++) {
            if (ar[j][0] > ar[mx][0])
                mx = j;
            else if (ar[j][0] == ar[mx][0] && ar[j][1] < ar[mx][1])
                mx = j;
        }
        a[i][0] = ar[mx][0];
        a[i][1] = ar[mx][1];
        if(a[i][1]==10)
            cout<<"<EOL> frequency = "<<a[i][0]<<endl;
        else
            cout << char(a[i][1]) << " frequency = " << a[i][0] << endl;
        for (int h = mx; h < g - 1; h++) { // Should replace the newly added column from arr with values 0 0
            ar[h][0] = ar[h + 1][0];
            ar[h][1] = ar[h + 1][1];
            ar[g][0] = 0;
            ar[g][1] = 0;
        }
        g--;
    }
    int numchar=d;
    // Create Threads
    pthread_t tid[numchar];
    struct command cmd[numchar];
    cmd[0].word=input;
    for(int i=0;i<numchar;i++)
    {
        if(i!=0)
            cmd[i].word=cmd[i-1].word;
        cmd[i].letter=char(a[i][1]);
        cmd[i].sockfd=sockfd;
        if(pthread_create(&tid[i],NULL,sendtoserver,&cmd[i]))
        {
            fprintf(stderr,"Error creating thread\n");
            return 1;
        }
        pthread_join(tid[i],NULL);
    }
    for(int i=0;i<numchar;i++)
        pthread_join(tid[i],NULL);

    //Print Compressed words
    for(int i=0;i<numchar;i++)
    {
        if(i!=(numchar-1)) {
            printcode(cmd[i].symbol,cmd[i].letter);
            printword(cmd[i].word);
        }
        else
            printword(cmd[i].symbol);
    }
    cout<<"Hello World"<<endl;
    return 0;
}
Client
