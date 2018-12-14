/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   distributedFiles.h
 * Author: ddeckys
 *
 * Created on December 13, 2018, 10:10 PM
 */

#ifndef DISTRIBUTEDFILES_H
#define DISTRIBUTEDFILES_H

#include <sys/stat.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include  <signal.h>
#include <netdb.h>
#include <ctype.h>
#include <pthread.h>
#include <map>
#include <openssl/md5.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <list>

using namespace std;

bool fileExists (string name) {
    struct stat buf;
    return (stat (name.c_str(), &buf) == 0);
}

string getMd5(string s) {
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)s.c_str(), (unsigned long)s.length(), (unsigned char *)md5);
    char finalString[33]; //I guess the size is 33
    
    for (int i = 0; i < 16; i++)
        sprintf(&finalString[i*2], "%02x", (unsigned int)md5[i]);
    return finalString;
}


#endif /* DISTRIBUTEDFILES_H */

