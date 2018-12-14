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
#define LISTENQ 50 /*maximum number of client connections */
#define BUFSIZE 4096

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

/*
 * Constants
 */
const static string dfc_msg = "dfc";
const static string authfail = dfc_msg + " authfail";
const static string authsuc = dfc_msg + " authsuccess";

enum Severity {
    debug, info, warn, error, dfc, dfs
};

template <class K, class V>
class dMap {
private:
    map<K, V> m;
    
public:
    
    int size() {
        return m.size();
    }
    
    /*
     * Inserts and updates if value already exists
     */
    void insert(K key, V value) {
        typename map<K, V>::iterator it = m.find(key);
        if (it != m.end()) {
            it->second = value;
        }
        else
            m.insert({key, value});
    }
    
    void add(K key, V value) {
        insert(key, value);
    }
    
    /*
     * Gets the value for the key, returns NULL if it does not exist
     */
    V get(K key) {
        typename map<K, V>::iterator it = m.find(key);
        if (it != m.end()) {
            return it->second;
        }
        else
            return NULL;
    }
    
    bool contains(const K k) {
        typename map<K, V>::iterator it = m.find(k);
        if (it != m.end()) {
            return true;
        }
        else
            return false;
    }
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
        if (sev >= progSev) {
            cout << sevToString(sev) << ": ";
            if (sev != dfc && sev != dfs)
                cout << name << ": ";
            cout << msg << endl;
        }
    }
    
    void log(string msg) {
        log(currSev, msg);
    }
    
    void printCharVector(vector<char> v) {
        cout << sevToString(currSev) << ": ";
        for (int t = 0; t < v.size(); t++)
            cout << v[t];
        cout << endl;
    }
};

static bool fileExists (string name) {
    struct stat buf;
    return (stat (name.c_str(), &buf) == 0);
}

static string getMd5(string s) {
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)s.c_str(), (unsigned long)s.length(), (unsigned char *)md5);
    char finalString[33]; //I guess the size is 33
    
    for (int i = 0; i < 16; i++)
        sprintf(&finalString[i*2], "%02x", (unsigned int)md5[i]);
    return finalString;
}

static string stringVector(vector<string> v) {
    string sol = "Vector: ";
    for (string s: v) {
        sol += s + "_";
    }
    return sol;
}

/*
 * Reads all of the bites of a file
 * This function is from
 * https://codereview.stackexchange.com/questions/22901/reading-all-bytes-from-a-file
 */
static vector<char> ReadAllBytes(char const* filename)
{
    ifstream ifs(filename, ios::binary|ios::ate);
    ifstream::pos_type pos = ifs.tellg();

    vector<char>  result(pos);

    ifs.seekg(0, ios::beg);
    ifs.read(&result[0], pos);

    return result;
}

/**
 * Writes all bytes to the file, overwrites any file that is there
 * @param file
 * @return 
 */
static void WriteAllBytes(char const* filename, vector<char> bytes) {
    ofstream ofs(filename, ios::binary|ios::trunc);
    
    ofs.write(bytes.data(), bytes.size());
    ofs.close();
}

/*
 * Splits a file into four nearly equal parts
 * returns a map where the int is the size of each part
 */
static map<int, vector<char>> splitFileInto4(vector<char> file) {
    //todo
}

/*
 * Splits a string by delimeter
 */
static vector<string> split(string s, char delim) {
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

