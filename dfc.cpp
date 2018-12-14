/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dfc.cpp
 * Author: ddeckys
 *
 * Created on December 13, 2018, 9:59 PM
 */

#include "distributedFiles.h"

/*
 * Constants
 */
struct sockaddr_in serveraddrList[4];
int sockets[4];
bool serverStatus[4];
string user, pass;

using namespace std;

struct sockaddr_in buildAddressObject(string hostname, string port) {
    logger l("buildAddressObject()");
    /* gethostbyname: get the server's DNS entry */
    struct sockaddr_in serveraddr;
    struct hostent *server;
    bzero((char *) &serveraddr, sizeof(serveraddr));
    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
        l.log(warn, "DNS lookup failed");
        return serveraddr;
    }

    /* build the server's Internet address */
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = stoi(port);
    return serveraddr;
}

bool readConfiguration () {
    logger l("readConfiguration()");
    if (!fileExists("dfc.conf"))
        return false;
    
    ifstream file("dfc.conf");
    string line;
    
    for (int t = 0; t < 4; t++) {
        if (!getline(file, line)) {
            l.log(warn, "Missing line " + to_string(t + 1));
            return false;
        }
        else {
            l.log(debug, "Read " + line);
            vector<string> spl = split(line, ' ');
            l.log(debug, stringVector(spl));
            if (spl.size() != 3){
                l.log(warn, "spl.size != 3");
                return false;
            }
            if (spl[0] != "Server") {
                l.log(warn, "spl[0] != Server");
                return false;
            }
            if (spl[1].length() < 4 || (spl[1][3] - '0') != t + 1) {
                l.log(warn, "DFS servers must be listed in order");
                return false;
            }
            vector<string> spl2 = split(spl[2], ':');
            l.log(debug, stringVector(spl2));
            if (spl2.size() != 2) {
                l.log(warn, "spl2.size != 2");
                return false;
            }
            serveraddrList[t] = buildAddressObject(spl2[0], spl2[1]);
        }
    }//end build addresses
    
    if (!getline(file, line)) {
        l.log(warn, "Missing username line");
        return false;
    }
    vector<string> spl = split(line, ' ');
    l.log(debug, stringVector(spl));
    if (spl.size() != 2 || spl[0] != "Username") {
        l.log(warn, "username line incorrectly formatted");
        return false;
    }
    
    user = spl[1];
    if (!getline(file, line)) {
        l.log(warn, "Missing password line");
        return false;
    }
    spl = split(line, ' ');
    l.log(debug, stringVector(spl));
    if (spl.size() != 2 || spl[0] != "Password") {
        l.log(warn, "password line incorrectly formatted");
        return false;
    }
    pass = spl[1];
    return true;
}


/*
 * 
 */
int main(int argc, char** argv) {
   
    logger l("main()");
    if (!readConfiguration()) {
        l.log(error, "There was a problem reading your configuration file dfc.conf, "
        "are you sure it exists and is set up properly? If you're unsure refer to the readme"
        " or run the program with warnings enabled");
        return 0;
    }
    
    //open sockets for each server and attempt to connect
    int downservers = 0;
    for (int t = 0; t < 4; t++){
        sockets[t] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[t] < 0)
            l.log(error, "error opening socket " + to_string(t));
        
        if (connect(sockets[t], (struct sockaddr *)&serveraddrList[t], sizeof(serveraddrList[t])) < 0) {
            l.log(dfc, "DFS" + to_string(t + 1) + " is down, ignoring until program restart");
            downservers++;
            serverStatus[t] = false;
        }
        else
            serverStatus[t] = true;
    }
    if (downservers >= 4) {
        l.log(error, "All servers down, client cannot proceed");
        return 0;
    }
    
    
    return 0;
}

