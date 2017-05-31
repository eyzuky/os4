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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
using namespace std;
//=============================
//DEFINES
//=============================
#define FAIL -1


//=============================
//GLOBALS
//=============================
int bulk_size;
mode_t modes = O_SYNC | 0 | O_RDONLY;
int blocks_num_global;
cache_algo_t algo_global;
double f_old_global;
double f_new_global;

vector<int> fd_vec;


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
    // from here checking for some errors
    if (blocks_num <= 0)
    {
        cerr << "Blocks num must be positive" << endl;
        return FAIL;
    }
    if(cache_algo == FBR)
    {
        if(f_old < 0 || f_old > 1 || f_new < 0 || f_new > 1 || f_old + f_new > 1)
        {
            cerr << "Illegal parameters for FBR algorithm" << endl;
            return FAIL;
        }
    }
    // ----------------------------
    
    
    //Initialize global variables
    algo_global = cache_algo;
    f_new_global = f_new;
    f_old_global = f_old;
    blocks_num_global = blocks_num;
    //-----------------------------
    
    //Allocate resources here
    
    
    
    //--------------------------
    
    
    
    return 0;
}


int CacheFS_destroy()
{
    

    return 0;
}

//note - change 0 to O_DIRECT when working in the aquarium
int CacheFS_open(const char *pathname)
{
    int fd = open(pathname, modes);
    fd_vec.push_back(fd); //for now just saving fd's in a vector, more for testing than actual EX solution
    return fd;
}

int CacheFS_close(int file_id)
{
    
    int res = close(file_id);
    if (res == FAIL)
    {
        cerr << "Failed to close file " << endl;
        exit(FAIL);
    }
    return 0;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset)
{
    int res;
    off_t seek = lseek(file_id, offset, SEEK_SET);
    off_t fileLength = lseek(file_id, 0, SEEK_END);
    if (count + seek <= fileLength) //this case we are not getting to the file end when readin this chunk
    {
        res = pread(file_id, buf, count, seek);
    } else {
        off_t size = fileLength - seek;
        res = pread(file_id, buf, size, seek);
    }
    if(res == FAIL)
    {
        cerr << "failed to read file" << endl;
        exit(1);
    }
    return res;
    
    
}

int CacheFS_print_cache (const char *log_path)
{
    
    return 0;
}


int CacheFS_print_stat (const char *log_path)
{
    
    return 0;
}

