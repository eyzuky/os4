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
    
    CacheFS_init(25, LFU, 0.1, 0.1);
    int fd_big_file = CacheFS_open("/tmp/OS4/File"); //TODO: uncomment in the awuarijum
    int fd_small_file = CacheFS_open("/tmp/OS4/FileForTest");
//    int fd_big_file = CacheFS_open("/Users/eyalsilberman/Desktop/os4/os4/os4/File");
//    int fd_small_file = CacheFS_open("/Users/eyalsilberman/Desktop/os4/os4/os4/FileForTest");
    void * buffer = malloc(12000);
    cout << (char*)buffer << endl;
    CacheFS_pread(fd_big_file, buffer, 12000, 0);
    cout << " buffer : " << (char*)buffer << endl;
    CacheFS_destroy();
}

