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


using namespace std;

bool readConfiguration () {
    if (!fileExists("dfc.conf"))
        return false;
}

/*
 * 
 */
int main(int argc, char** argv) {
    
    if (!readConfiguration()) {
        cout << "There was a problem reading your configuration file dfc.conf, are you sure it exists and is set up properly?\n";
        return 0;
    }
    
    return 0;
}

