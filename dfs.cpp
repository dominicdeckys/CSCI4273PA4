/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dfs.cpp
 * Author: ddeckys
 *
 * Created on December 13, 2018, 10:00 PM
 */

#include "distributedFiles.h"

/*
 * Constants
 */
short int num;
int listenfd;
dMap<string, string> users;

using namespace std;

bool readConfiguration () {
    logger l("readConfiguration()");
    if (!fileExists("dfs.conf"))
        return false;
    
    ifstream file("dfs.conf");
    string line;
    
    while (getline(file, line)) {
        l.log(debug, line);
        vector<string> str = split(line, ' ');
        if (str.size() != 2) {
            l.log(warn, "User not configured correctly");
        }
        users.insert(str[0], str[1]);
    }
    
    if (users.size() <= 0)
        l.log(warn, "There are no users");
}

void ctrlC (int sig) {
    logger l("ctrlC()");
    int val = 1;
    close(listenfd);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void*) &val, sizeof(val));
    l.log("Closing the socket!");
    exit(0);
}

void exitGracefully(int connfd) {
    close(connfd);
    pthread_exit(NULL);
}

void * listenToClient (void * arg) {
    logger l("listenToClient()");
    int connfd = (int)*((int*)arg);
}

/*
 * 
 */
int main(int argc, char** argv) {
    logger l("main()");
    
    //check arguments
    if (argc < 3) {
        l.log(dfs, "Usage: ./dfs <server #> <port>");
        return 0;
    }
    num = stoi(argv[1]);
    if (num < 1 || num > 4) {
        l.log(error, "Server number invalid");
        return 0;
    }
    
    if(!readConfiguration()) {
        l.log(error, "Configuration file not set up correctly or does not exist.");
        return 0;
    }
    
    
    //legacy code from PA3
    int connfd, openSockets;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    signal(SIGINT, ctrlC);

    //creation of the socket
    listenfd = socket (AF_INET, SOCK_STREAM, 0);

    //setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &o, sizeof(int));

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((unsigned short)atoi(argv[1]));
    
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    l.log("Server running...waiting for connections.");
    openSockets = 0;
    pthread_t thread;
    
    pthread_attr_t attr;

    if (pthread_attr_init(&attr) != 0) {
        l.log(error, "Error in pthread_attr_init()");
        return 0;
    }

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        l.log(error, "Error in pthread_attr_setdetachstate()");
        return 0;    
    }
    signal (SIGPIPE, SIG_IGN);
    while(true) {

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (pthread_create(&thread , &attr, &listenToClient, (void*) &connfd ) != 0) {
            l.log(error, "error creating pthread");
        }
        l.log("Socket " + to_string(openSockets++) + " opened with connfd " + to_string(connfd));

    }
    close(listenfd);
    
    return 0;
}

