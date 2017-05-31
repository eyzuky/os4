//
//  main.cpp
//  os4
//
//  Created by Eyal Silberman on 30/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#include <iostream>
#include "CacheFS.h"
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    CacheFS_init(6, FBR, 0.2, 0.4);
    return 0;
}
