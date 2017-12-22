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
#include <string.h>
#include <cstring>
#include <map>
#include <vector>
#include <security/pam_appl.h> // login
#include <security/pam_misc.h> // login

using namespace std;

#define MAX_INPUT_LENGTH 11111
#define PERMS 0666

//username
int level = 0;
string username = "";

// adjustable value
int server_port = 8000; //server port number
int backlog = 30; // the maximum number to which the queue of pending connections
string start_string="****************************************\n** Welcome to the information server. **\n****************************************\n";

int readline(int fd,char *ptr,int maxlen);
int all_space(string command); // return 0 if the input command is all space
vector<string> parser(string command);

int start_while_loop_for_accept_input(int client_sockfd); 

// custom_command
void exec_command_directly_only(int client_sockfd,vector<string> input_vector);
int login(int client_sockfd);
void hide_show(int client_sockfd, vector<string> input_vector,int flag);
void compress_extract(int client_sockfd, vector<string> input_vector, int flag);
void search_string(int client_sockfd,vector<string> input_vector);
void search_file(int client_sockfd,vector<string> input_vector);
void show_space(int client_sockfd,vector<string>input_vector);
// login
struct pam_response *reply;
int function_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr)
{
    *resp = reply;
    return PAM_SUCCESS;
}


int main(int argc, char* argv[], char *envp[]){
    int sockfd;
    struct sockaddr_in server_addr;  //structure for IPv4

    // initial
    setenv("PATH","/tmp/bin:.",1);
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
            
            // fork()
            if( (child_pid = fork()) < 0 ){
                cout << "ERROR when fork a child to handle a new client" << endl;
            }
            else if(child_pid == 0) {
                // close original (parent) socket
                close(sockfd);
                
                //login
                if(login(client_sockfd) == 1){
                    string home = "/home/"+username+"/";
                    chdir(home.c_str());
                    cout << "Client Login success" << endl;

                    // start work
                    send(client_sockfd, "% ", (int)strlen("% "), 0);
                    start_while_loop_for_accept_input(client_sockfd);
                }
                else{
                    cout << "Client Login failed" << endl;
                }

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

            // detect absolute path
            int get_it = 0;
            for(int i=0;i<input_vector.size();i++){
                if(input_vector[i][0] == '/'){
                    get_it = 1;
                    break;
                }
            }
            if(get_it == 1){
                string output_string = "We don't accept absolute path\n";
                send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);    
                send(client_sockfd, "% ", (int)strlen("% "), 0);
                continue;
            }

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

            else if(input_vector[0] == "pwd" || input_vector[0] == "ls" || input_vector[0] == "cat" || input_vector[0] == "mv" || input_vector[0] == "touch" || input_vector[0] == "rm" || input_vector[0] == "cp" || input_vector[0] == "mkdir" ){
                exec_command_directly_only(client_sockfd,input_vector);
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

            else if(input_vector[0] == "hide"){
                if(input_vector.size()==2){
                    hide_show(client_sockfd,input_vector,0);
                }
                else{
                    string output_string = "Please use \"hide [filename]\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
            }
            else if(input_vector[0] == "show"){
                if (input_vector.size() == 2){
                    hide_show(client_sockfd, input_vector,1);
                }
                else{
                    string output_string = "Please use \"show [filename]\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
            }
            else if(input_vector[0] == "compress"){
                if (input_vector.size() == 3)                
                    compress_extract(client_sockfd, input_vector, 0);                
                else{
                    string output_string = "Please use \"compress [filename] [archive_name]\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
            }
            else if(input_vector[0] == "extract"){
                if (input_vector.size() == 2)                
                    compress_extract(client_sockfd, input_vector, 1);                
                else{
                    string output_string = "Please use \"extract [filename]\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
            }
            else if(input_vector[0] == "cd"){
                if(input_vector.size() == 1){
                    string home = "/home/"+username+"/";
                    chdir(home.c_str());
                }
                else if (input_vector.size() == 2){
                    if(input_vector[1].substr(0,2) == ".."){
                        if(level == 0){
                            string output_string = "You aren't able go that directory\n";
                            send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                        }
                        else{
                            chdir(input_vector[1].c_str());
                            level--;
                        }
                    }
                    else{
                        chdir(input_vector[1].c_str());
                        level++;
                    }
                }
                else{
                    string output_string = "Please use \"cd [target]\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
            }
	    else if (input_vector[0] == "space"){
                if (input_vector.size() == 1)
                    show_space(client_sockfd, input_vector);
                else{
                    string output_string = "Please use \"space\".\n";
                    send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
                }
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

void exec_command_directly_only(int client_sockfd,vector<string> input_vector){

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
        dup2(client_sockfd,STDERR_FILENO);
        dup2(client_sockfd,STDOUT_FILENO);
        execvp(input_vector[0].c_str(), temp_argv);
    }
    else{
        wait(NULL);
    }
}


int login(int client_sockfd){
    char buf[128];
    int len;
    string user, pass;
    send(client_sockfd, "username: ", 10, 0);
    len = read(client_sockfd, buf, 127);
    for(int i=0; i<len-2; i++) user += buf[i];
    memset(buf, 0, 128);
    send(client_sockfd, "password: ", 10, 0);
    len = read(client_sockfd, buf, 127);
    for(int i=0; i<len-2; i++) pass += buf[i];

    static struct pam_conv pam_conversation = { function_conversation, NULL };
    pam_handle_t*          pamh;
    int res = pam_start("lapsapSVC", user.c_str(), &pam_conversation, &pamh);
    if (res == PAM_SUCCESS) {
        reply = (struct pam_response *)malloc(sizeof(struct pam_response));
        reply[0].resp = strdup(pass.c_str());
        reply[0].resp_retcode = 0;
        res = pam_authenticate(pamh, 0);
    }
    if (res == PAM_SUCCESS) 
        res = pam_acct_mgmt(pamh, 0);
    if (res == PAM_SUCCESS){
        username = user;
        send(client_sockfd, "Correct\n", 8, 0);
    }
    else 
        send(client_sockfd, "Wrong\n", 6, 0);
    pam_end(pamh, res);

    if (res == PAM_SUCCESS) 
        return 1;
    else
        return 0;
}

void hide_show(int client_sockfd,vector<string> input_vector,int flag){
    //flag 0:hide the file  ex: hide test.html
    //flag 1:do not hide the file  ex:show .test.html
    string output_string; 
    string hide_filename = "." + input_vector[1];
    int f1 = access(input_vector[1].c_str(),F_OK);
    int f2 = access(hide_filename.c_str(),F_OK);
    if(f1 != 0 && f2 != 0){
        output_string = "File "+input_vector[1]+" not exist.\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
        return;
    }
    else if((flag == 0 && f2 == 0) ||(flag ==0 && f1==0 && input_vector[1][0]=='.')){
        output_string = "File " + input_vector[1] + " is already hidden.\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
        return;
    }
    else if(flag == 1 && f1 == 0){
        output_string = "File " + input_vector[1] + " is already showed up.\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
        return;
    }
    int pid;
    if ((pid = fork()) == -1) 
        cout << "Error when fork to run command" << endl;
    // child, run command
    else if (pid == 0){
        char **arg = new char *[5];
        string a = "mv";
        arg[0] = new char[3]; strcpy(arg[0],a.c_str());
        arg[1] = new char[input_vector[1].size()+2]; 
        string tmp;
        if(flag == 0){
            tmp = "." + input_vector[1];
            strcpy(arg[1], input_vector[1].c_str());
        }
        else{
            tmp = input_vector[1];
            strcpy(arg[1], hide_filename.c_str());
        }
        arg[2] = new char[tmp.size()+1]; strcpy(arg[2],tmp.c_str());
        arg[3] = NULL;
        execvp(a.c_str(), arg);
    }
    else{
        wait(NULL);
        
        if(flag==0)
            output_string = "Hide the file :" + input_vector[1] + "\n";
        else
            output_string = "Show the file :" + input_vector[1] + "\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
    }
}

void compress_extract(int client_sockfd,vector<string> input_vector,int flag){
    //flag 0:compress the file
    //flag 1:extract the file
    string output_string;
    int size = input_vector[1].size();
    if(access(input_vector[1].c_str(),F_OK) != 0){
        output_string = "File " + input_vector[1] + " not exist.\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
        return;
    }
    else if(flag == 1 && ( size <= 4  || input_vector[1].substr(size-4,4) !=".zip")){
        output_string = "File " + input_vector[1] + " is already extracted.\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
        return;
    }
    int pid;
    if((pid = fork()) == -1) 
        cout << "Error when fork to run command" << endl;
    // child, run command
    else if(pid == 0){
        dup2(client_sockfd,1);
        dup2(client_sockfd, 2);
        dup2(client_sockfd, 0);
        char **arg = new char *[5];   
        string a,b;
        if(flag==0){
            a = "zip";
            b = "-r";
        }
        else{
            a = "unzip";
        }
        arg[0] = new char[a.size()+1];strcpy(arg[0],a.c_str());
        
        if(flag == 0){
            arg[1] = new char[b.size()+1];strcpy(arg[1],b.c_str());            
            arg[2] = new char[input_vector[2].size()+1]; strcpy(arg[2],input_vector[2].c_str());
            arg[3] = new char[input_vector[1].size()+1]; strcpy(arg[3],input_vector[1].c_str());
            arg[4] = NULL;
        }
        else{
            arg[1] = new char[input_vector[1].size()+1]; strcpy(arg[1],input_vector[1].c_str());
            arg[2] = NULL;
        }
       
        execvp(a.c_str(), arg);
    }
    else{
        wait(NULL);
        
        if(flag==0)
            output_string = "Compress the file :" + input_vector[1] + "\n";
        else
            output_string = "Extract the file :" + input_vector[1] + "\n";
        send(client_sockfd, output_string.c_str(), (int)strlen(output_string.c_str()), 0);
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

void show_space(int client_sockfd,vector<string> input_vector) 
{
    int pid;
    int p[2];
    pipe(p);
    if((pid=fork())==-1)
        cout << "Error when fork to run command" << endl;
    else if(pid == 0){
        dup2(p[1],1);
        close(p[0]);
        close(p[1]);
        char **arg = new char *[3];
        string a = "du";
        string b = "-sh";
        arg[0] = new char[3];strcpy(arg[0],a.c_str());
        arg[1] = new char[4];strcpy(arg[1],b.c_str());
        arg[2] = NULL;
        execvp(a.c_str(), arg);
    }
    else{
        wait(NULL);
        char buf[100];
        int n = read(p[0],buf,100);
        close(p[0]);
        close(p[1]);
        char *pch;
        pch = strtok(strdup(buf),"M");
        char reply[100];
        sprintf(reply,"Current use: %sMB/100MB\n",pch);
        write(client_sockfd,reply,strlen(reply));
        memset(buf,0,100);
        memset(reply,0,100);
    }
}
