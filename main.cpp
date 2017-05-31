//
//  main.cpp
//  os4
//
//  Created by Eyal Silberman on 30/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#include <iostream>
#include "CacheFS.h"
using namespace std;
int main(int argc, const char * argv[]) {
    char * filename1 = "/Users/eyalsilberman/Desktop/os4/os4/os4/File";
    int fd1 = CacheFS_open(filename1);
    //read from file and print it
    void *buf[10];
    CacheFS_pread(fd1, &buf, 1000, 0);
    cout << (char*)buf << endl;
    
    int i  = CacheFS_init(3, FBR, 0.1, 0.1);
    cout << i << endl;
}

