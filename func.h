#ifndef __FUNC_H__
#define __FUNC_H__

//////////////////////////////////////////////
// File Name     : func.h                   //
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
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFSIZE    1024
#define PORTNO      38030   

//permission 777
#define RWXRWXRWX (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH| S_IXOTH)

char* getHomeDir(char* home);
char *sha1_hash(char *input_url, char *hashed_url);
int Check_Hit(char * buf,char * hashed, char* root);
//static void handler();
void CommunicateClient(int client_fd, struct sockaddr_in client_addr);

char* getIPAddr(char* addr);


#endif