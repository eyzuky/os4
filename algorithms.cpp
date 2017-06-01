//
//  algorithms.cpp
//  os4
//
//  Created by Eyal Silberman on 31/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#include "algorithms.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

CacheAlgo::CacheAlgo(int blocks_num, int block_size): blocks_num(blocks_num), block_size(block_size)
{
    //allocate our cache memory.
    cache_memory = (char*)calloc(blocks_num, block_size);
    //TODO: Check with other people that char * is good to use as the data type for the cache memory.
    
    //init our memory with null. look at function get_block_for_writing and how it searches for empty blocks.
    memset(cache_memory, '\0', sizeof(char)*block_size * blocks_num);
    cout << sizeof(cache_memory) << endl;
    
    if (cache_memory == NULL)
    {
        cerr << "Failed to allocate memory for cache\n";
    }
    cout << "Allocated " << blocks_num *  block_size << " memory for the cache" << endl;
    cout << "cache algo constructor finish" << endl;
}


CacheAlgo::~CacheAlgo()
{
    free(cache_memory);
    cout << "cache algo destructor finish" << endl;

}

void CacheAlgo::new_file(int fd, const char* pathname, off_t file_size)
{
    //allocate array of bulk_struct for the new file
    bulk_struct_array bulk_array = new bulk_struct[(int)(file_size / block_size) + 1];
    cout << "=====================\n";
    cout << "file: " << pathname << endl;
    cout << "fd: " << fd << endl;
    cout << "Size: " << file_size << endl;
    cout << "bulks allocated: " << (int)(file_size / block_size) + 1 << endl;
    cout << "=====================\n\n";
    //have access to the array using the pathname -> array map.
    pathname_to_bulks[pathname] = bulk_array;
    //have access to the pathname using the fd->pathname map. this 2 maps will work together. Why?
    //because the user may open the same file twice, getting 2 fd's that point to the same file with different relative paths. so multiple fd's may be mapped to the same pathname, and then we only open the file once.
    fd_to_pathname[fd] = pathname;
    get_block_for_writing();
    
}
//call this function to get a free block! if there is no free block available, we will need to remove a block from the cache (declared remove_block_from_cache(). Each algorithm defines this function for its own method.
char * CacheAlgo::get_block_for_writing()
{
    char * ptr = cache_memory;
    char * end = cache_memory + blocks_num * block_size;
    while(ptr < end)
    {
        if (*ptr == '\0')
        {
            cout << "found memory" << endl;
            return ptr;
        } else {
            cout << "incrementing" << endl;
            ptr += block_size;
        }
    }
    cout << "didn't find memory, returning NULL " << endl;
    return NULL;
}
bulk_struct CacheAlgo::get_block(int fd, int block_in_file)
{
    
    bulk_struct_array file_array = pathname_to_bulks[fd_to_pathname[fd]];
    return file_array[block_in_file];
}


//each algorithm implements this function for removing a block when no block is found by function get_block_for_writing().
//return value is a pointer to the block begin in the cache memory.
char * CacheAlgo::remove_block_from_cache(){return nullptr;}

char * LRUAlgo::remove_block_from_cache()
{
    cout << "I am in remove block from cache of algo: LRU" << endl;
    return nullptr;
}

char * LFUAlgo::remove_block_from_cache()
{
    cout << "I am in remove block from cache of algo: LFU" << endl;

    return nullptr;
}

char * FBRAlgo::remove_block_from_cache()
{
    cout << "I am in remove block from cache of algo: FBR" << endl;

    return nullptr;
}

