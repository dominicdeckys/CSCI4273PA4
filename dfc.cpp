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

struct quadShort {
    short s1 = -1;
    short s2 = -1;
    short s3 = -1;
    short s4 = -1;
};

struct fileParts {
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;
};

using namespace std;


struct sockaddr_in buildAddressObject(string hostname, string port) {
    logger l("buildAddressObject()");
    l.log(debug, "building " + hostname + ":" + port);
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
    serveraddr.sin_port = htons(stoi(port));
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

/**
 * Requests a list of broken files from a single server
 * @param server
 * @return 
 */
vector<brokenFile> doListSingular(short server) {
    logger l("doListSingular()");
    int n;
    char buf[BUFSIZE];
    vector<brokenFile> files;
    if (!serverStatus[server]) {
        l.log(debug, "server offline " + to_string(server));
        return files;
    }

    string msg = "dfc list";
    send(sockets[server], msg.c_str(), msg.length(), 0);

    while (true) {
        bzero(buf, BUFSIZE);
        if ((n = recv(sockets[server], buf, BUFSIZE, 0)) <= 0) {
            l.log(error, "Connection with server lost: DFS" + to_string(server + 1));
            serverStatus[server] = false;
            //TODO remove function
            return files;
        }
        buf[n] = '\0';
        string full(buf);
        vector<string> args = split(full, ' ');
        l.log(debug, stringVector(args));

        if (args.size() < 4) {
            l.log(warn, "unknown input received");
            break;
        }
        if (args[1] == "listitem") {
            l.log(debug, "adding item to list");
            brokenFile f = {args[2], (short) stoi(args[3]), server};
            files.push_back(f);
            string msg2 = "dfc good";
            send(sockets[server], msg2.c_str(), msg2.length(), 0);
        }
        else if (args[1] == "listdone") {
            l.log(info, "end of list reached for DFS" + to_string(server + 1));
            break;
        }
        else {
            l.log(warn, "unknown input received");
            break;
        }

    }
    return files;
}

/**
 * Requests the broken files from all of the servers
 * @return 
 */
vector<brokenFile> getAllBrokenFiles() {
    vector<brokenFile> brokenFiles;
    for (int t = 0; t < 4; t++) {
        vector<brokenFile> vecc = doListSingular(t);
        brokenFiles.insert(brokenFiles.end(), vecc.begin(), vecc.end());
    }
    return brokenFiles;
}

/**
 * Gets the broken files from all the server, determines which ones can be completed, and displays to the user.
 */
void doList() {
    logger l("doList()");
    int n;
    char buf[BUFSIZE];
    map<string, fileParts> files;
    
    //get the broken files from each server and puts them in one list
    vector<brokenFile> brokenFiles = getAllBrokenFiles();
    
    //figure out which files are complete and which are not
    for (brokenFile f: brokenFiles) {
        if (f.server < 0)
            continue;
        string name = f.name;
        switch (f.part) {
            case 1:
                files[name].b1 = true;
                break;
            case 2:
                files[name].b2 = true;
                break;
            case 3:
                files[name].b3 = true;
                break;
            case 4:
                files[name].b4 = true;
                break;
            default:
                break;
        }
    }
    
    for (brokenFile f: brokenFiles)
        l.log(debug, "Name: " + f.name + " Part: " + to_string(f.part) + " Server: " + to_string(f.server));
    
    l.log(dfc, "Files:");
    //iterate through the map and print the files
    for(map<string, fileParts>::iterator it = files.begin(); it != files.end(); it++) {
        string output = it->first;
        fileParts p = it->second;
        //if and of the bools are false then the file is incomplete;
        if (!p.b1 || !p.b2 || !p.b3 || !p.b4)
            output += " [incomplete]";
        l.log(dfc, output);
    }
    l.log(debug, "done");
    
}

void doPutSingular(int connfd, string filename, short part, char * buf, int start, int end) {
    logger l("doPutSingular");
    int n;
    l.log(debug, "sending name " + filename + " connfd " + to_string(connfd) + " start " + to_string(start) + " end " + to_string(end));
    string msg = "dfc put " + filename + " " + to_string(part);
    
    send(connfd, msg.c_str(), msg.length(), 0);
}

void doPut(string filename) {
    //todo
}

bool authenticate(int connfd) {
    logger l("authenticate()");
    int n;
    char buf[BUFSIZE];
    string msg = "dfc " + user + " " + pass;
    if ((n = send(connfd, msg.c_str(), msg.length(), 0)) <= 0) {
        l.log(info, "failed sending authentication message to server");
        return false;
    }
    if ((n = recv(connfd, buf, BUFSIZE, 0)) <= 0 || n >= BUFSIZE) {
        l.log(info, "never received confirmation from the server");
        return false;
    }
    buf[n] = '\0';
    string confirm(buf);
    if (confirm == authfail) {
        l.log(info, "server returned authentication fail");
        return false;
    }
    else if (confirm == authsuc) {
        l.log(info, "server returned authentication success");
        return true;
    }
    else {
        l.log(info, "server returned invalid response");
        return false;
    }
    
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
            l.log(dfc, "DFS" + to_string(t + 1) + " appears to be down, ignoring until program restart");
            downservers++;
            serverStatus[t] = false;
        }
        else if (authenticate(sockets[t])){
            l.log(dfc, "Authenticated and connected to DFS" + to_string(t + 1));
            serverStatus[t] = true;
        }
        else {
            l.log(dfc, "Authentication failed for DFS" + to_string(t + 1));
            downservers++;
            serverStatus[t] = false;
            close(sockets[t]);
        }
    }
    //change to 4
    if (downservers >= 5) {
        l.log(error, "All servers ether down or failed to authenticate, client cannot proceed");
        return 0;
    }
    
    string line;
    
    while (true) {
        l.log(dfc, "Enter a command:");
        l.log(dfc, "list");
        l.log(dfc, "put <file name>");
        l.log(dfc, "get <file name>");
        
        cin >> line;
        
        vector<string> input = split(line, ' ');
        if (input[0] == "list") {
            doList();
        }
        else if (input[0] == "put") {
            //todo
        }
        else if (input[0] == "get") {
            //todo
        }
        else {
            l.log(error, "Input not recognized! Try again and use lowercase for the command");
        }
    }
    
    return 0;
}

