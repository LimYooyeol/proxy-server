//////////////////////////////////////////////
// File Name     : func.c                   //
// Date          : 2021/ 05 / 10            //
// Os            : Ubuntu 16.04 LTS 64bits  //
// Autor         : Lim Yooyoel              //
// Student ID    : 2017202072               //
//////////////////////////////////////////////
#include "func.h"

//permission 777
#define RWXRWXRWX (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH| S_IXOTH)



///////////////////////////////////////////////////////////////
// getHomeDir                                                //
// ==============================                            //
// Input : char* -> home,                                    //
//         home : string to record path of home directory    //
// Output : char * home,                                     //
//          path of home directory in char array             //
// Purpos : Get path of home directory                       //
///////////////////////////////////////////////////////////////
char* getHomeDir(char* home){
    //Get user information and home directory
    struct passwd* usr_info = getpwuid(getuid()); //get user information
    strcpy(home, usr_info -> pw_dir); //get home directory
    //End of get user information and home directory

    return home; //return home directory
}


//////////////////////////////////////////////////////////////////////
// sha1_hash                                                        //
// ==============================                                   //
// Input : char* -> input_url,                                      //
//         input_url : user input, original string to be hashed     //
//         char* -> hashed_url,                                     //    
//         hashed_url : char array to record hashed value           //
// Output : char * sha1_hash,                                       //
//          hash derived by hashing input_url by SHA1               //
// Purpos : hash input url by SHA1                                  //
//////////////////////////////////////////////////////////////////////
char *sha1_hash(char *input_url, char *hashed_url){
    unsigned char hashed_160bits[20]; //hashed value 
    char hashed_hex[41];              //hashed value in hexadecimal form
    int i; 

    //SHA1(origianl string, length of original string, hashed string)
    SHA1(input_url, strlen(input_url) , hashed_160bits); //hashing

    for(i = 0 ; i < sizeof(hashed_160bits); i++){ 
        sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]); //save hashed value in hexadecimal form
    } //end of for

    strcpy(hashed_url, hashed_hex); //copy

    return hashed_url; //return hashed url
}


//////////////////////////////////////////////////////
// Check_Hit                                        //
// ==============================================   //
// Input :  char * -> buf                           //
//          buf : string to store Hit url           //
//          char * -> hashed                        //
//          hashed : hashed url input               //
//          char * -> root                          //
//          root : path of cache root directory     //
// Output : 0 -> Miss, 1 -> Hit, -1 : error         //
//                                                  //
// Purpose : check if input is miss or hit          //
//                                                  //
//////////////////////////////////////////////////////
int Check_Hit(char * buf,char * hashed, char* root){
    struct dirent* d;   //cache root directory entity
    DIR * dp;           //cache root directory
    
    //open cache root directory
    if((dp = opendir(root)) == NULL){
        printf("Log Directory open error"); //open error
        return -1;
    } //end of open
        
    while(d = readdir(dp)){ //read cache directory entities
        char cachedir[100];
        strcpy(cachedir, root);
        
        // open sub directories in cache root
        DIR * cache_dp = opendir(strcat(cachedir, d->d_name));
        if(cache_dp == NULL){
            printf("Log Directory open error");
            return -1;
        }
        struct dirent* cache_d;
        // end of open sub directories

        //check hit
        while(cache_d = readdir(cache_dp)){ //read entities in sub directories
            if(strcmp(cache_d->d_name, hashed + 3*sizeof(char)) == 0){ //if hashed url already in cache
                //record name in buf
                char dir[100];
                strcpy(dir, d->d_name);
                strcat(dir,  "/");
                strcat(dir, cache_d->d_name);
                strcpy(buf, dir);
                //end of record

                return 1; //Hit
            }
        }
        //end of check hit
    }
    //end of read

    //miss
    return 0;
}

///////////////////////////////////////////////////
// CommunicateClient                             // 
// ============================================  //
// purpose : implement communication with client //
// paramter : char * - > addr,                   //
//            text url.                          //
// return :  dotted IP address                   //              
///////////////////////////////////////////////////
char* getIPAddr(char* addr){
    struct hostent* hent;
    char* haddr;
    int len = strlen(addr);

    //get host by name and extract dotted IP address
    if((hent = (struct hostent*)gethostbyname(addr))!= NULL){
        haddr = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));
    }
    
    //return dotted IP address
    return haddr;
}
