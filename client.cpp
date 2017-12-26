#include<iostream>    //cout
#include<stdlib.h>
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<string>  //string
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<netdb.h> //hostent
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int sock; // server socket number

int main(int argc,char *argv[]){
    int port = 8000;
    string address = "localhost";

    sock = socket(AF_INET , SOCK_STREAM , 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    if(inet_addr(address.c_str()) == -1){ //if input is not ip
        string hostname = address;
        struct hostent *he;
        struct in_addr **addr_list;
        int i;
        he = gethostbyname( hostname.c_str() );  
        addr_list = (struct in_addr **) he->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++) 
            address = inet_ntoa(*addr_list[i]);
    }
    server.sin_addr.s_addr = inet_addr( address.c_str() );
    int status = connect(sock , (struct sockaddr *)&server , sizeof(server));
    if (status >= 0)
        cout << "Connected to server\n";
    else 
        cout << "Init Error\n";

    fd_set readfds;
    int maxfd;
    string tmp;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(fileno(stdin), &readfds);
        maxfd = max(sock, fileno(stdin));

        select(maxfd+1,&readfds,NULL,NULL,NULL);
        if( FD_ISSET(sock, &readfds) ){
            string buffer;
            char buff[1025];
            int len;
            while( len = read(sock, buff, 1024), len > 0 ) {
                buff[len] = '\0';
                buffer += buff;
                if ( len < 1024 ) {
                    break;
                }
            }
            if(len == 0) exit(1);
            if(buffer == "password: ") {
                char *pass = getpass(buffer.c_str());
                send(sock, pass, strlen(pass), 0);
            } else 
                cout << buffer << flush;
            buffer = "";
        }
        if( FD_ISSET(fileno(stdin), &readfds) ){
            getline(cin,tmp);
            tmp += "\n";
            send(sock, tmp.c_str(), tmp.size(), 0);
        }
    }

}
