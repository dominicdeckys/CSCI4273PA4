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

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    logger l("main()");
    if (argc < 2) {
        l.log(dfs, "Usage: ./dfs <port>");
        return 0;
    }
    return 0;
}

