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
    
    CacheFS_init(3, LFU, 0.1, 0.1);
    CacheFS_open("/Users/eyalsilberman/Desktop/os4/os4/os4/File");
    CacheFS_open("/Users/eyalsilberman/Desktop/os4/os4/os4/FileForTest");
    CacheFS_destroy();
}

