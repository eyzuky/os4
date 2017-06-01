//
//  algorithms.hpp
//  os4
//
//  Created by Eyal Silberman on 31/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#ifndef algorithms_hpp
#define algorithms_hpp

#include <stdio.h>
#include <map>
using namespace std;


//------------------------------------------
//               TYPEDEFS                  -
//------------------------------------------
//holds memory of all the cache
typedef char * Cache_memory;

//holds bulk data
typedef struct bulk_struct
{
    int counter;
    void * pointer_to_memory;
}bulk_struct;
//holds array of bulk data, which everyfile will hold. if a bulk from a file is not in the cache, its associated index in the array will hold null value.
typedef bulk_struct * bulk_struct_array;
//map file descriptor to its array
typedef map<const char *, bulk_struct_array> map_pathname_to_bulk_array; //map file path to bulk array.
typedef map<int, const char*> map_fd_to_pathname; //since there might be multiple fd's on a path name, if the user opens the same file multiple times from different relative paths. so we always need to create the full path in CacheFS_open.. we get the pathname, get its full path and call new_file function with the FULL PATH ONLY
//-------------------------------------------


class CacheAlgo
{
    
public:
    
    CacheAlgo(int blocks_num, int block_size);
    
    ~CacheAlgo();
    
    char * get_block_for_writing(); //this function returns a pointer to empty space in the cache. If none is found, it returns null and the algo knows to remove a bulk from the cache.
    
    bulk_struct get_block(int fd, int block_in_file);
    
    virtual char * remove_block_from_cache();
    
    void new_file(int fd, const char * pathname, off_t file_size);
private:
    Cache_memory cache_memory; //this holds the actual content of the files. Should be initialized with size blocks_num * block_size.
    int blocks_num;
    int block_size;
    map_pathname_to_bulk_array pathname_to_bulks;
    map_fd_to_pathname fd_to_pathname;
    
};
//This class is for managing algo LEAST RECENTLY USED
class LRUAlgo: public CacheAlgo
{
public:
    
    LRUAlgo(int blocks_num, int block_size): CacheAlgo(blocks_num, block_size){}
    ~LRUAlgo(){}
    char * remove_block_from_cache();
private:
    void update_counters();
    int * LRU_Counters;
    
};

//This class is for managing also LEAST FREQUENTLY USED
class LFUAlgo: public CacheAlgo
{
public:
    LFUAlgo(int blocks_num, int block_size): CacheAlgo(blocks_num, block_size){}
    ~LFUAlgo(){}
    char * remove_block_from_cache();
private:
    int blocks_num;
    
};

//Hybrid policy
class FBRAlgo: public CacheAlgo
{
public:
    FBRAlgo(int blocks_num, int block_size, double f_old, double f_new): CacheAlgo(blocks_num, block_size){}
    ~FBRAlgo(){}
    char * remove_block_from_cache();
private:
    int blocks_num;
    double f_old;
    double f_new;
    
};











#endif /* algorithms_hpp */
