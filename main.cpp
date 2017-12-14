#include <iostream>
#include <fstream>
#include <stdlib.h> // atoi()
#include <arpa/inet.h> // for socket()
#include <unistd.h> // fork() & close()
#include <sys/socket.h> // accept()
#include <sys/stat.h> // write file
#include <fcntl.h> // write file
#include <sys/types.h> // accept() & shared memory
#include <sys/ipc.h> // shared memory
#include <sys/shm.h> // shared memory
#include <sys/wait.h> // wait()
#include <signal.h>

#include <cstring>
#include <map>
#include <vector>

using namespace std;

#define MAX_INPUT_LENGTH 11111
#define PERMS 0666

// adjustable value
int server_port = 8000; //server port number
int backlog = 30; // the maximum number to which the queue of pending connections
string start_string="****************************************\n** Welcome to the information server. **\n****************************************\n";

int readline(int fd,char *ptr,int maxlen);
int all_space(string command); // return 0 if the input command is all space
vector<string> parser(string command);

int start_while_loop_for_accept_input(int client_sockfd);

// custom_command
void pwd(int client_sockfd,vector<string> input_vector);
void search_string(int client_sockfd,vector<string> input_vector);
void search_file(int client_sockfd,vector<string> input_vector);

int main(int argc, char* argv[], char *envp[]){
    int sockfd;
    struct sockaddr_in server_addr;  //structure for IPv4

    // initial
    setenv("PATH","bin:.",1);
    chdir("user_profile/");
    if(argc == 2){ server_port = atoi(argv[1]);} // set server port

    // ----------------------------------------
    // socket()
    // ----------------------------------------
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // AF_INET = IPv4 , SOCK_STREAM = use TCP protocol
        cout << "ERROR opening socket" << endl;
        return 0;
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; //
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //htonl = Host to Network Long,  INADDR_ANY = any IP
    server_addr.sin_port = htons(server_port); //set server's port , htons = host to network short

    // ----------------------------------------
    // bind()
    // ----------------------------------------
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (struct sockaddr *) &server_addr, sizeof(server_addr)); //  manipulate options for the socket referred to by the file descriptor sockfd
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        cout << "ERROR on binding" << endl;
        return 0;
    }
    // ----------------------------------------
    // listen()
    // ----------------------------------------
    listen(sockfd, backlog); // backlog = the maximum number to which the queue of pending connections
    
    // ----------------------------------------
    // signal settings
    // ----------------------------------------
    signal (SIGCHLD,SIG_IGN); // make zombie process will not be generated

    // ----------------------------------------
    // accept()
    // ----------------------------------------
    int client_sockfd,child_pid;
    socklen_t client_length;
    struct sockaddr_in client_addr;

    cout<<"=============="<<endl;
    cout<<" Server Ready "<<endl;
    cout<<"=============="<<endl;

    while(1){
        client_length = sizeof(client_addr);
        client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_length); // process will suspend here for waiting some client to connect

        if( client_sockfd < 0 ){
            cout << "ERROR when accept a connection" << endl;
        }
        else{
            // get client ip & port
            string ip = string(inet_ntoa(client_addr.sin_addr));
            int port = ntohs(client_addr.sin_port);

            // print at server
            cout<<"A new client connect to server, client ip:"<<ip<<" , client port:"<<port<<endl;
            
            // send start_string & "% " to client 
            send(client_sockfd, start_string.c_str(), (int)strlen(start_string.c_str()), 0);
            send(client_sockfd, "% ", (int)strlen("% "), 0);

            // fork()
            if( (child_pid = fork()) < 0 ){
                cout << "ERROR when fork a child to handle a new client" << endl;
            }
            else if(child_pid == 0) {
                // close original (parent) socket
                close(sockfd);
                
                // start work
                start_while_loop_for_accept_input(client_sockfd);

                //end
                cout<<"A client disconnect with server, client ip:"<<ip<<" , client port:"<<port<<endl;
                exit(0);
            }
            else{
                close(client_sockfd);
            }
        }
    }
    return 0;
}

int start_while_loop_for_accept_input(int client_sockfd){

    while(1){
        //initial
        char input_buffer[MAX_INPUT_LENGTH];
        memset(input_buffer, 0, sizeof(input_buffer)); // clear array

        //receive from client
        if(readline(client_sockfd,input_buffer,sizeof(input_buffer)) < 0){
            cout << "Error when readline()" << endl;
        }
        else{
            // input turn to string
            string input_string(input_buffer);
            input_string = input_string.substr(0,input_string.size()-2);
            
            // if input nothing or space only
            if(all_space(input_string)){
                send(client_sockfd, "% ", (int)strlen("% "), 0);
                continue;
            }
            // make command split with one space only
            vector<string> input_vector = parser(input_string);

            // -------- debug use,you can comment out this part if you dont need --------
            cout<<"sockfd --> "<<client_sockfd<<": ";
            for(int i=0;i<input_vector.size();i++){
                cout<<"\""<<input_vector[i]<<"\" ";
            }
            cout<<endl;
            // -------- debug use,you can comment out this part if you dont need --------

            if(input_vector[0] == "exit"){
                return 0;
            }
            else if(input_vector[0] == "pwd"){
                pwd(client_sockfd,input_vector);
            }
            else if(input_vector[0] == "search"){
				if( input_vector.size() == 4 && input_vector[2] == "in" ){
					search_string(client_sockfd,input_vector);
				}
				else if( input_vector.size() == 5 && input_vector[2] == "in" && input_vector[3] == "dir" ){
					search_file(client_sockfd,input_vector);
				}
				else{
					string output_string = "Invalid use of command [search].\n";
               		send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
				}
            }
            else if(input_vector[0] == "XXX"){
                // add your code here
                // add your code here
                // add your code here
            }
            else if(input_vector[0] == "XXX"){
                // add your code here
                // add your code here
                // add your code here
            }
            else if(input_vector[0] == "XXX"){
                // add your code here
                // add your code here
                // add your code here
            }
            else if(input_vector[0] == "XXX"){
                // add your code here
                // add your code here
                // add your code here
            }
            else if(input_vector[0] == "XXX"){
                // add your code here
                // add your code here
                // add your code here
            }
            else{
                string output_string = "Unknown command: [" + input_vector[0] + "].\n";
                send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
            }
        }

        // print % and ready the next recv
        send(client_sockfd, "% ", (int)strlen("% "), 0);
    }
}

int readline(int fd,char *ptr,int maxlen){
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

int all_space(string command){
    int range = command.size();
    for(int i=0;i<range;i++){
        if(command[i] != ' '){
            return 0;
        }
    }
    return 1;
}

vector<string> parser(string command){
    
    // initial
    vector<string> result_vector;
    command+=" ";

    // parse all parameter (split by space)
    for(int i=0;i<command.size();i++){
        
        string temp_string;
        if(command[i] != ' '){
            while(1){
                if(command[i] != ' '){
                    temp_string = temp_string + command[i];
                    i++;
                }
                else{
                    break;
                }
            }
            result_vector.push_back(temp_string);
        }
    }

    return result_vector;
}

void pwd(int client_sockfd,vector<string> input_vector){

    // make argv (for argument)
    char** temp_argv = new char*[input_vector.size()+1];
    for(int i = 0; i<input_vector.size(); i++){

        string temp_string = input_vector[i];

        temp_argv[i] = new char[temp_string.size()+1];
        strcpy(temp_argv[i],temp_string.c_str());
    }
    temp_argv[input_vector.size()] = NULL;

    // exec pwd
    int pid;
    if((pid = fork()) == -1) {
        cout << "Error when fork to run command" << endl;
    }
    // child, run command
    else if(pid == 0){
        dup2(client_sockfd,STDOUT_FILENO);
        execvp(input_vector[0].c_str(), temp_argv);
    }
    else{
        wait(NULL);
    }
}

void search_string(int client_sockfd,vector<string> input_vector){
	
	int pid_grep ;
	if( (pid_grep = fork()) < 0 ){
		cout << "Error when fork to run command search" << endl;	
	}
	else if( pid_grep == 0 ){

		int pipefd[2];
		pipe(pipefd);
		int pid_cat ;

		if( (pid_cat = fork()) < 0 ){
			cout << "Error when fork to run command search" << endl;	
		}
		// doing cat
		else if( pid_cat == 0 ){

			// set pipe
			close(pipefd[0]);
			dup2(pipefd[1], 1);
			close(pipefd[1]);

			// make argv
			char** temp_argv = new char*[3];
			temp_argv[0] = (char*) "cat";
			temp_argv[1] = new char[input_vector[3].size()+1];
			temp_argv[2] = NULL;
			strcpy(temp_argv[1], input_vector[3].c_str());

			// execute
			if( execvp( temp_argv[0], temp_argv) < 0 ){
				perror("execvp()");
				exit(1);
			}
		}
		// doing grep
		else{
			// wait for cat
			wait(NULL);

			// set pipe
			close(pipefd[1]);
			dup2(pipefd[0],0);
			close(pipefd[0]);

			// set standard output
			dup2(client_sockfd, STDOUT_FILENO);

			// make argv
			char** temp_argv = new char*[3];
			temp_argv[0] = (char*) "grep";
			temp_argv[1] = new char[input_vector[1].size()+1];
			strcpy(temp_argv[1], input_vector[1].c_str());
			temp_argv[2] = NULL;

			// execute
			if( execvp( temp_argv[0], temp_argv) < 0 ){
				perror("execvp()");
				exit(1);
			}
		}
	}
	else{
		wait(NULL);
	}
}

void search_file(int client_sockfd,vector<string> input_vector){

	int pid;

	if( (pid = fork()) < 0 ){
		cout << "Error when fork to run command search" << endl;	
	}
	else if( pid == 0 ){
		
		// make argv
		char** temp_argv = new char*[5];
	
		input_vector[3] = input_vector[1];	
		input_vector[1] = input_vector[4];
		input_vector[0] = "find";		
		input_vector[2] = "-name";

		for(int i = 0; i < 4; i++){

        	string temp_string = input_vector[i];
        	temp_argv[i] = new char[temp_string.size()+1];
       		strcpy(temp_argv[i],temp_string.c_str());

    	}
   		temp_argv[4] = NULL;

		// set client as stdout
		dup2(client_sockfd,STDOUT_FILENO);

		// execute
		if( execvp( temp_argv[0], temp_argv) < 0 ){
			perror("execvp()");
			exit(1);
		}
	}
	else{
		wait(NULL);
	}
}
