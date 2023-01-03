//////////////////////////////////////////////
// File Name     : proxy.c                  //
// Date          : 2021/ 05 / 10            //
// Os            : Ubuntu 16.04 LTS 64bits  //
// Autor         : Lim Yooyoel              //
// Student ID    : 2017202072               //
//////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "func.h"

#define BUFFSIZE    1024
#define PORTNO      38030   


//////////////////////////////////////////////////////////////////////////
// handler                                                              //
// ==============================================================       //
// purpose : handle signal                                              //
//////////////////////////////////////////////////////////////////////////
static void handler(){
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}



time_t start_program;
int log_fd;
int sub_process;

///////////////////////////////////////////////////
// myhandler                                     // 
// ============================================  //
// purpose : set handler for SIGCHLD, SIGALRM    // 
///////////////////////////////////////////////////
static void myhandler(int signo){
    if(signo == SIGALRM){ // time over
        printf("응답 없음\n");

        exit(SIGALRM);
    } // end of time over
    if(signo == SIGINT){ //SIGINT
        // server total log
        time_t end_program;
        time(&end_program);
        
        char svr_msg[100];
        sprintf(svr_msg, "**SERVER** [TERMINATED] run time : %d sec. #sub process : %d\n", (int)(end_program- start_program), sub_process);
        write(log_fd, svr_msg, strlen(svr_msg));
        //  end of server total log
        exit(0);
    } //End of SIGINT
}


//////////////////////////////////////////////////////////////////////////
// main                                                                 //
// ==============================================================       //
// purpose : implement proxy server communicating with client           //
//////////////////////////////////////////////////////////////////////////
int main(){
    time(&start_program); // set start time
    


    struct sockaddr_in server_addr, client_addr;
    int socket_fd, client_fd;

    pid_t pid;

    // create socket
    if( (socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
        // if failed at open socket, print error and end program
        printf("Server : Can't open stream socket\n");
        return 0;
    } // end of create socekt

    // set socket information
    bzero( (char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);
    // end of set socket information

    // associate an address with the socket
    if( bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        // if failed binding, print error and end program
        printf("Server : Can't bind local address\n");
        close(socket_fd);
        return 0;
    }
    // end of associate an address with the socket

    listen(socket_fd, 10); // listen connection with clients
    signal(SIGCHLD, (void *)handler);
    signal(SIGINT, (void *)myhandler);

    // get path of log file and crate
    char LogRoot[300];
    getHomeDir(LogRoot); // get home directory path
    mkdir(strcat(LogRoot, "/logfile/"), RWXRWXRWX); // make directory ~/logfile/

    // open log file
    if( (log_fd = open(strcat(LogRoot, "logfile.txt"), O_WRONLY| O_CREAT | O_TRUNC , RWXRWXRWX)) < 0){
        printf("failed to open log file\n");
        return 0;
    }// end of open log file
    //End of get path and create

    while(1){ //accept connections
        struct in_addr inet_client_address;

        char buf[BUFFSIZE];

        char tmp[BUFFSIZE] = {0, };
        char method[20] = {0, };
        char url[BUFFSIZE] = {0, };

        char * tok = NULL; 

        // accept a client
        bzero((char*)&client_addr, sizeof(client_addr));
        int len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
        // end of accept a client

        // accept failure
        if(client_fd < 0){  
            printf("Server : accept failed  %d\n", getpid());
            close(socket_fd);
            return 0;
        } //end of accept failure

        pid = fork(); //fork child process
        sub_process ++;

        if( pid == -1){ //fork failure
            close(client_fd);
            close(socket_fd);
            continue;
        } //end of fork failure
        
        if( pid == 0){ // child process
            struct in_addr inet_client_address;

            char buf[BUFFSIZE];
            //int len, len_out;

            char tmp[BUFFSIZE] = {0, };
            char method[20] = {0, };
            char url[BUFFSIZE] = {0, };

            char * tok = NULL; 

            char hashed[41]; //hashed value

            inet_client_address.s_addr = client_addr.sin_addr.s_addr; //client address


            //set signal handler
            if(signal(SIGALRM, myhandler) == SIG_ERR){
                printf("signal() error ");
                return 0;
            }


            //Get path and make cache root
            char root[300]; //root directory to make cache directory and files
           
              
            getHomeDir(root); //get home direcotry path

            umask(0);  //set umask 000
            mkdir(strcat(root, "/cache/"), RWXRWXRWX); //make directory ~/cache/
            // end of get path and make cache root
        
            //read requests and parse url
            read(client_fd, buf, BUFFSIZE); //read requests

            strcpy(tmp, buf);

            tok = strtok(tmp, " ");
            char hostname[300];
            char *referer;
            char *ico;

            // get referer name, check if .ico request
            referer = strstr(buf, "Referer");
            ico = strstr(buf, ".ico");
            // end of get referer name, check if .ico request
            strcpy(method, tok);
            if(strcmp(method, "GET") == 0){
                // get url
                tok = strtok(NULL, " ");
                strcpy(url, tok);
                // end of get url
                
                // get hostname, add www. if not included
                int j = 0;
                if(strncmp("www.", tok+ 7, 4) != 0){
                    strcpy(hostname, "www."); 
                    j = 4;
                }
                for(int i = 7; tok[i] != '/'; i++){
                    hostname[j++] = tok[i];
                }
                // end of get hostname



                // ignore firefox
                if(strcmp(hostname, "www.detectportal.firefox.com") == 0){
                    exit(0);
                } // end of ignore firefox

                
                // print IP, PORT of accepted client
                printf("[%s : %d] client was connected\n", (char*)inet_ntoa(client_addr.sin_addr), client_addr.sin_port );

                // print request message from web browser
                puts("============================");
                printf("Request from [%s : %d]\n", inet_ntoa(inet_client_address), client_addr.sin_port);
                puts(buf);
                puts("============================\n");
                
            }
            else{
                exit(0); //ignore request except GET
            }
            //end of read request and pharse url
            
            // hash url and check hit or miss
            sha1_hash(url, hashed); //hashing url

            char Hitbuf[300];
            int Is_Hit = Check_Hit(Hitbuf, hashed, root);
            // end of hash url and check hit or miss

            time_t request_time;
            time(&request_time);
            struct tm * ltp = localtime(&request_time);
            
            char log_msg[100];
            char cur_time[100];
            strftime(cur_time, 100, "-[%Y/%m/%d, %H:%I:%S]", ltp); //current time in format


            if(Is_Hit == 0 ){ // MISS
                // log only when directly requested
                if(referer == NULL && ico == NULL){
                    sprintf(log_msg, "[MISS]%s-%s\n", url, cur_time);
                    write(log_fd, log_msg, strlen(log_msg));
                }// end lf log
                int socket_fd, len;
                struct sockaddr_in server_addr;
                char* IPAddr = getIPAddr(hostname); //get dotted IP address
                                
                char server_buf[BUFFSIZE];
                
                //Make cache directory by hashed value
                char dir[300]; //path of directory
                strcpy(dir, root);
                mkdir(strncat(dir, hashed, 3), RWXRWXRWX); //make directory at ~/cache/, name with first three letters of hashed vlaue
                //End of make directory

                //Make cache file
                char file[300]; //path of cache file
                strcpy(file,dir);
                strcat(file, "/");
                strcat(file, hashed + 3 * sizeof(char)); //file name is equal to hashed value excepts first three character
                
                int cache_fd;
                if(access(file , F_OK) < 0){ //create cache file
                    if((cache_fd = creat(file , RWXRWXRWX)) < 0){
                        printf("creat error for %s\n", file);
                    }
                } //end of create cache file
                //End of Make cache file

                // create socket to connect web server
                if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
                    printf("can't create socket.\n");
                    return 0;
                } // end of create socket
                
                //set socket information
                bzero(server_buf, sizeof(server_buf));
                bzero((char *)&server_addr, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_addr.s_addr = inet_addr(IPAddr);
                server_addr.sin_port = htons(80); //HTTP
                // end of set socket information

                // connect with web server
                if( connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
                    printf("%s can't connect\n", hostname);
                    return 0;
                } // end of connect with web server

                alarm(10);
                if(write(socket_fd, buf, sizeof(buf)) > 0){ // write response message from browser to server
                    int read_len;
                    while( (read_len = read(socket_fd, server_buf, sizeof(server_buf))) > 0){
                        //puts(server_buf);
                        alarm(0); 
                        write(client_fd, server_buf, read_len);
                        write(cache_fd, server_buf, read_len);
                        bzero(server_buf, sizeof(server_buf));
                    }
                }
                else{
                    printf("web server write failure");
                }
            } //End of MISS
            else if(Is_Hit == 1){ //HIT
                // log only when directly requested
                if(referer == NULL && ico == NULL){
                    sprintf(log_msg, "[HIT]%s-%s\n", url, cur_time);
                    write(log_fd, log_msg, strlen(log_msg));
                }// end lf log

                char * Cache_Contents = malloc(100000);

                char dir[300];
                char filename[300];
                strcpy(dir, root); // cache root directory
                strncat(dir, hashed, 3);

                strcpy(filename, dir);
                strcat(filename, "/");
                strcat(filename, hashed + 3*sizeof(char));

                if(access(filename, F_OK) < 0){
                    printf(" Cache File Not Exits \n");
                }
                int cache_fd = open(filename, O_RDONLY);
                while( read(cache_fd, Cache_Contents, sizeof(Cache_Contents)) > 0){
                    write(client_fd, Cache_Contents, sizeof(Cache_Contents));
                    bzero(Cache_Contents, sizeof(Cache_Contents));
                }
                free(Cache_Contents);
            } // End of Hit

            close(client_fd);
            exit(0);
        } // end of child process

        close(client_fd);
    }
    //end of accept connections
    
    

    close(socket_fd);

    return 0;
}