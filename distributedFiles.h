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
#include <string>
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
#include <vector>

using namespace std;

enum Severity {
    debug, info, warn, error, dfc, dfs
};

class logger {
    
private:
    string name;
    Severity currSev;
    
    string sevToString (Severity sev) {
        switch (sev) {
            case debug:
                return "DEBUG";
            case info:
                return "INFO";
            case warn:
                return "WARN";
            case error:
                return "ERROR";
            case dfc:
                return "DFC";
            case dfs:
                return "DFS";
            default:
                return "BAD BAD";
        }
    }
    
public:
    
    const static Severity progSev = debug;
    
    logger(string namee) {
        name = namee;
        currSev = info;
    }
    
    void setSeverity(Severity sev) {
        currSev = sev;
    }
    
    void log(Severity sev, string msg) {
        if (sev >= progSev)
            cout << sevToString(sev) << ": " << name << ": " << msg << endl;
    }
    
    void log(string msg) {
        log(currSev, msg);
    }
};

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

string stringVector(vector<string> v) {
    string sol = "Vector: ";
    for (string s: v) {
        sol += s + "_";
    }
    return sol;
}

/*
 * Splits a string by delimeter
 */
vector<string> split(string s, char delim) {
    vector<string> sol;
    int start = 0;
    for (int t = 0; t < s.length(); t++) {
        if(s[t] == delim) {
            if (t <= start)
                start++;
            else {
                sol.push_back(s.substr(start, t - start));
                start = t + 1;
            }
        }
    }
    if (start < s.length())
        sol.push_back(s.substr(start, s.length()));
    return sol;
}



#endif /* DISTRIBUTEDFILES_H */

