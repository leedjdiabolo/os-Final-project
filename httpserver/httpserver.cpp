#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<vector>
#include<iostream>
#include<fstream>
#include<signal.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<netdb.h>
using namespace std;
#define buffer_size 10001

void err_dump(const char *m);
int readline(int fd,char *ptr,int maxlen);
void handle_request(int sockfd);
char socket_buffer[10001];

struct request_arg{
 	string ip;
 	string port;
 	string filename;
}; 

vector<struct request_arg> cgi_arg;

int main(int argc,char **argv){

	int sockfd,newsockfd,clilen,childpid;
	struct sockaddr_in cli_addr,serv_addr;

	int SERV_TCP_PORT = 4000;
	if( argc > 1)
		SERV_TCP_PORT = atoi(argv[1]);
	
	signal (SIGCHLD,SIG_IGN);
	//[socket]
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_dump("server: can't open stream socket");
	//[reuse same port]
	//int fd;
	int reuseaddr = 1;
    socklen_t reuseaddr_len;
	reuseaddr_len = sizeof(reuseaddr);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, reuseaddr_len);
	//[bind]
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_TCP_PORT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		err_dump("server: can't bind local address");

	listen(sockfd, 5);

	for ( ; ; ) {
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
		if (newsockfd < 0)
			err_dump("server: accept error");
		if ( (childpid = fork()) < 0)
			err_dump("server: fork error");
		else if (childpid == 0) { /* child process */ /* close original socket */
			close(sockfd);
			/* process the request */
			handle_request(newsockfd);
			shutdown(newsockfd,2);
			exit(0);
		}
		close(newsockfd); /* parent process */
	 }

	return 0;
}

void handle_request(int sockfd){
	int n;
	//memset
	memset(socket_buffer,0,sizeof(socket_buffer));
	n=readline(sockfd,socket_buffer,10000);
	string request = socket_buffer;
	while(n=readline(sockfd,socket_buffer,10000)){
		string rubbish = socket_buffer;
		if(rubbish.find("\r\n") == 0){
			break;
		}
	}
	cout << request << endl;
	//parse
	int pos;
	string r_get,token;
	string filename;
	pos = request.find(' ');
	r_get = request.substr(0, pos);
	request.erase(0, pos+1);
	pos = request.find(' ');
	request = request.substr(1, pos);
	//cout<<request<<endl;
	pos = request.find('?');
	if(pos != string::npos){
		filename = request.substr(0, pos);
		request.erase(0, pos+1);
		request = request.substr(0,request.size()-1);
	}else if((pos = request.find(' ')) != string::npos){
		filename = request.substr(0, pos);
		request = "";
	}

	//
	if(strcmp(r_get.c_str(),"GET") == 0){
		cout<<"GET"<<endl;
		if(filename.find(".htm") != string::npos){
			FILE* htm_file;
			filename = "/home/pjhiah/public_html/" +filename;
			htm_file = fopen(filename.c_str(),"r");
			if(htm_file == NULL){
				write(sockfd,"HTTP/1.1 404 Not Found\r\n",24);
			}else{
				write(sockfd,"HTTP/1.1 200 OK\r\n",17);
				write(sockfd,"Content-Type: text/html\r\n\r\n",28);
				memset(socket_buffer,0,sizeof(socket_buffer));
				fread (socket_buffer ,1 , 10000 , htm_file);
				string html_content = socket_buffer;
				write(sockfd,html_content.c_str(),html_content.size());
				fclose(htm_file);
			}	
		}else if(filename.find(".cgi") != string::npos){
			write(sockfd,"HTTP/1.1 200 OK\r\n",17);
			filename = "/home/pjhiah/public_html/cgi/" +filename;
			setenv("REQUEST_METHOD", r_get.c_str(), 1);
			setenv("QUERY_STRING" , request.c_str(), 1);
			setenv("SCRIPT_NAME" , filename.c_str(), 1);

			cout<<r_get<<' '<<filename<<' '<<request<<endl;
			if(fork() == 0){
				dup2(sockfd,STDOUT_FILENO);
				execl(filename.c_str(),NULL);//must NULL
				//exit(0);
			}else{
				int exit_status;
				wait(&exit_status);
			}
		}
	}
	
}

void err_dump(const char* m){
	perror(m);
	exit(1);
}

int readline(int fd,char *ptr,int maxlen)
{
	int n,rc;
	char c;
	*ptr = 0;
	for(n=1;n<maxlen;n++)
	{
		if((rc=read(fd,&c,1)) == 1)
		{
			*ptr++ = c;
			if(c=='\n')  break;
		}
		else if(rc==0)
		{
			if(n==1)     return(0);
			else         break;
		}
		else
			return(-1);
	}
	return(n);
}
