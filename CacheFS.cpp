//
//  CacheFS.cpp
//  os4
//
//  Created by Eyal Silberman on 30/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#include "CacheFS.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
//=============================
//DEFINES
//=============================
#define FAIL -1


//=============================
//GLOBALS
//=============================
int bulk_size;







//=============================
//HELPER FUNCTIONS
//=============================



int getBulkSize()
{
    struct stat fi;
    stat("/tmp", &fi);
    int blkSize = fi.st_blksize;
    return blkSize;
}

void errorHandler(int value, string func)
{
    if (value == FAIL)
    {
        string error = "function " + func + " failed. Exiting.";
        cerr << error << endl;
        exit(0);
    }
}




int CacheFS_init(int blocks_num, cache_algo_t cache_algo,
                 double f_old , double f_new  )
{
    bulk_size = getBulkSize();
    cout << bulk_size << endl;
    return 0;
}


int CacheFS_destroy()
{
    return 0;
}


int CacheFS_open(const char *pathname)
{
    
    return 0;
}

int CacheFS_close(int file_id)
{
    
    return 0;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    
    return 0;
}

int CacheFS_print_cache (const char *log_path)
{
    
    return 0;
}


int CacheFS_print_stat (const char *log_path)
{
    
    return 0;
}

