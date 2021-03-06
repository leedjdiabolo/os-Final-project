#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<vector>
#include<iostream>
#include<fstream>
#include<signal.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<errno.h>
using namespace std;

#define buffer_size 10001

void front();
void end();
void connection();
void job();
void json_output(char *tmp);
int readline(int fd,char *ptr,int maxlen);
int contain_prompt ( char* line );
int recv_msg(int from);
int drop_msg(int drop);

int write_enable;
int conn;
int nfds;
int sockfd;
int flag;
int fail;
char socket_buffer[10001];
string uid;
string upass;

fd_set rfds;/* readable file descriptors*/
fd_set wfds;/* writable file descriptors*/
fd_set ars;/* active file descriptors*/
fd_set aws; /* active file descriptors*/

int main(){
	front();
	write_enable = 0;
	conn = 0;
	flag = 0;
	fail = 1;
	nfds= FD_SETSIZE;
	FD_ZERO(&rfds); FD_ZERO(&wfds); FD_ZERO(&ars); FD_ZERO(&aws);
	int pos;
	string squery = getenv("QUERY_STRING");
	pos = squery.find('=');
	squery.erase(0,pos+1);
	pos = squery.find('&');
	uid = squery.substr(0,pos) + "\r\n";
	pos = squery.find('=');
	squery.erase(0,pos+1);
	upass = squery + "\r\n";

	connection();
	if(conn == 1)
		job();

	end();
	return 0;
}
//[ { filetype : 0, filename : xxx }]
void front(){
	cout << "Content-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
	cout << "{ \"response\" : ";
}

void end(){
	if(fail){
		cout << " 0 ";
	}else cout << " 1 ";
	cout << "}";
}

void connection(){

	string s_ip = "139.59.237.167";

	int SERVER_PORT;
	struct sockaddr_in client_sin;
	struct hostent *he;
	//int unsend = 0;

	if((he=gethostbyname(s_ip.c_str())) == NULL) {
		fprintf(stderr, "Invalid IP Address\n");
		exit(1);
	}

	SERVER_PORT = 8000;

	//handle socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&client_sin, 0, sizeof(client_sin)); 
	client_sin.sin_family = AF_INET;
	client_sin.sin_addr = *((struct in_addr *)he->h_addr); 
	client_sin.sin_port = htons(SERVER_PORT);

	if ( connect(sockfd, (struct sockaddr *)&client_sin,sizeof(client_sin)) < 0) {
		if (errno != EINPROGRESS) {
			fprintf(stderr, "failed\n");
			return;
		}
	}
	conn = 1;
}

void job(){
	int n,error;
	if (getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,(socklen_t*) &n) < 0 ||error != 0) {
		// non-blocking connect failed
		fprintf(stderr, "oh no!\n");
		return;
	}
	while( write_enable == 0){
		drop_msg(sockfd);
	}
	fprintf(stderr, "ok\n");
	memset(socket_buffer,0,sizeof(socket_buffer));
	//write command to server
	write(sockfd, uid.c_str(), uid.size());
	drop_msg(sockfd);
	write(sockfd, upass.c_str(), upass.size());
	if(recv_msg(sockfd) == 0)
		cout<< "0 ";

	string cmd = "exit\r\n";
	write(sockfd, cmd.c_str(), cmd.size());

	while( write_enable == 0 && n!=0){
		n = drop_msg(sockfd);
	}
	close(sockfd);
	return;
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
			if(c==' '&& *(ptr-2) =='%'){  break; }
			if(c==' '&& *(ptr-2) ==':'){  break; }
			if(c=='\n')  break;
		}
		else if(rc==0)
		{
			if(n==1)     return(0);
			else         break;
		}
		else{
			return(-1);
		}
	}
	return(n);
}

int contain_prompt ( char* line ){
	int i, prompt = 0 ;
	for (i=0; line[i]; ++i) {
		switch ( line[i] ) {
		case '%' : prompt = 1 ; break;
		case ' ' : if ( prompt ) {
			return 1;
		}
		default: prompt = 0;
		}
	}
	return 0;
} 

int recv_msg(int from){
	char buf[3000],*tmp;
	int sum_len = 0;
	int len,i;
	
	if((len=readline(from,buf,sizeof(buf)-1)) <0) return -1;
	buf[len] = 0; 
	string buff = buf;
	if ( contain_prompt(buf) )
		write_enable = 1 ;
	else if (buff.find("username: ") !=string::npos){
		write_enable = 1;
	}
	else json_output(buf);
	
	fflush(stdout); 
	return len;
}

void json_output(char *tmp){
	string x = tmp;
	if(x.find("Access Granted") !=string::npos)
		fail = 0;
	else fail = 1;
}

int drop_msg(int from){
	char buf[3000],*tmp;
	int len,i;
	if((len=readline(from,buf,sizeof(buf)-1)) <0) return -1;
	buf[len] = 0; 

	string buff=buf;
	if ( contain_prompt(buf) )
		write_enable = 1 ;
	else if (buff.find("username: ") !=string::npos){
		write_enable = 1;
	}else if(buff.find("password: ") !=string::npos){
		write_enable = 1;
	}
	fprintf(stderr, "%s\n", buf);
	fflush(stdout); 
	return len;
}
