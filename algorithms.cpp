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
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

bool should_cout = true;
CacheAlgo::CacheAlgo(const int blocks_num, const int block_size): blocks_num(blocks_num), block_size(block_size)
{
    //allocate our cache memory.
    const int blocks_num_Ar = blocks_num;
    const int block_size_Ar = block_size;
    cache_memory = new char[blocks_num_Ar][block_size_Ar]; // CONTINUE FRM HEREEEEEEE
    for ( int i = 0; i < blocks_num; i++){
        cache_memory[i] = '\0';
    }
//    cache_memory = (char*)calloc(blocks_num, block_size);
    //TODO: Check with other people that char * is good to use as the data type for the cache memory.
    
    //init our memory with null. look at function get_block_for_writing and how it searches for empty blocks.
   // memset(cache_memory, '\0', sizeof(char)*block_size * blocks_num);
    if(should_cout)
    {
        cout << sizeof(cache_memory) << endl;
    }
    if (cache_memory == NULL)
    {
        cerr << "Failed to allocate memory for cache\n";
    }
    if(should_cout)
    {
        cout << "Allocated " << blocks_num *  block_size << " memory for the cache" << endl;
        cout << "cache algo constructor finish" << endl;
    }
}

CacheAlgo::~CacheAlgo()
{
    free(cache_memory);
    if(should_cout)
    {
     cout << "cache algo destructor finish" << endl;
    }
}

void CacheAlgo::new_file(int fd, const char* pathname, off_t file_size)
{
    //allocate array of bulk_struct for the new file
    bulk_struct_array bulk_array = new bulk_struct[(int)(file_size / block_size) + 1];
    for (int i = 0; i < (int)(file_size / block_size) + 1; i++)
    {
        bulk_array[i].pointer_to_memory = nullptr; //this way we know that this bulk is not loaded to the cache yet
        cout << "bulk_array[i]  : " << &(bulk_array[i]) << endl;

    }
    if(should_cout)
    {
        cout << "=====================\n";
        cout << "file: " << pathname << endl;
        cout << "fd: " << fd << endl;
        cout << "Size: " << file_size << endl;
        cout << "bulks allocated: " << (int)(file_size / block_size) + 1 << endl;
        cout << "=====================\n\n";
    }
    //have access to the array using the pathname -> array map.
    pathname_to_bulks[pathname] = bulk_array;
    //have access to the pathname using the fd->pathname map. this 2 maps will work together. Why?
    //because the user may open the same file twice, getting 2 fd's that point to the same file with different relative paths. so multiple fd's may be mapped to the same pathname, and then we only open the file once.
    fd_to_pathname[fd] = pathname;
}

//call this function to get a free block! if there is no free block available, we will need to remove a block from the cache (declared remove_block_from_cache(). Each algorithm defines this function for its own method.
int CacheAlgo::get_block_for_writing()
{
    for ( int i = 0; i < blocks_num; i++){
        if (cache_memory[i] == '\0'){
            return i;
        }
    }
    cout << "didn't find memory, returning NULL " << endl;
    return FAIL;
}

bulk_struct CacheAlgo::get_block(int fd, int block_in_file)
{
    bulk_struct_array file_array = pathname_to_bulks[fd_to_pathname[fd]];
    return file_array[block_in_file];
}

int CacheAlgo::fetch_from_file(int fd, void *buf, int block_start, int block_end, off_t seek, int count)
{
    cout <<"\n###########################################" << endl;
    cout << "This is function fetch_from_file! " << endl;
    cout << "I got a buffer address to load the results to after I manage to fetch all the data " << endl;
    cout << " I have some more data: " << endl;
    cout << "file id: " << fd << endl;
    cout << "The first block of the file that I need: " << block_start << endl;
    cout << "The last block of the file that I need: " << block_end << endl;
    cout << "My start offset: " << seek << endl;
    cout << "How much should I read: " << count << endl;
    cout <<"###########################################" << endl;

    // this function tries to get most of the data from cache memory,
    // and if its not found it takes it from the actual file,
    // save bulks to cache (and removing bulks if needed).
    // Only the overriding of this function would actually be called
    int count_to_fetch = count;
    char* temp_buf = (char *)malloc(sizeof(((block_end-block_start)+1)*this->block_size));
    for(int i = block_start; i < block_end + 1; i++)
    {
        bulk_struct bulk = get_block(fd, i);
        if (bulk.pointer_to_memory == nullptr) {
            cout << "didn't find block: " << i << " in cache" << endl;
            //TODO: done :)
            //block not in cache, need to read it from the file and then append it, and save it in cache.
            //use functions: get_block_for_writing(). if it returns NULL,
            // it means there is no free block for writing,
            // so you need to remove a block depending on the algo that is currently being used.
            int to_read = get_block_for_writing();
            char * block_to_read = &cache_memory[to_read];
            if (block_to_read == NULL) {
                cout << "didn't find free block in cache" << endl;
                this->remove_block_from_cache();
                to_read = get_block_for_writing();
                cout << block_to_read << "freed block" << endl;
                if (to_read == FAIL) {
                    cout << "failed to delete bock from cache" << endl;
                    return FAIL;
                }
            }
            cout << block_to_read << "       <---         freed block" << endl;
            bulk.pointer_to_memory = block_to_read;
            cout << "bulk pointer: " << bulk.pointer_to_memory << endl;

            cout << "before pread" << endl;
            // copying the whole block
            off_t block_begin = lseek(fd, (i * block_size), SEEK_SET);
            pread(fd, bulk.pointer_to_memory, block_size , block_begin);
            cout << "after pread :    bulk pointer: " << *bulk.pointer_to_memory << endl;
            cout << "after pread" << endl;

        } else {
//            cout << "found block in cache: " << i << endl;
//            memcpy( &temp_buf[i*block_size], bulk.pointer_to_memory, block_size);// TODO: Debug this. make sure it works.-> added <string.h> is it good?
//            count_to_fetch -= block_size;
        }
        char * cast_temp = (char*) temp_buf;
        memcpy((&cast_temp[i*block_size]) , bulk.pointer_to_memory, (size_t)block_size);// TODO: Debug this. make sure it works.-> added <string.h> is it good?

//        if (i == block_start){
//            cout << "before memcpy" << endl;
             // copying only from seek to buf
//            memcpy(&temp_buf[i*block_size], bulk.pointer_to_memory, block_size);// TODO: Debug this. make sure it works.-> added <string.h> is it good?
//            count_to_fetch -= (block_size);
//            cout << "after memcpy" << endl;
//
//        } else if (i == block_end && count_to_fetch < block_size){
//             copying up to the end of the count to buf
//            memcpy( &temp_buf[i*block_size], bulk.pointer_to_memory, count_to_fetch);// TODO: Debug this. make sure it works.-> added <string.h> is it good?
//        }else {
//             copying a full block to buf
//            memcpy( &temp_buf[i*block_size], bulk.pointer_to_memory, block_size);// TODO: Debug this. make sure it works.-> added <string.h> is it good?
//            count_to_fetch -= block_size;
//        }

    }
    //now after we have all of our blocks in temp_buf, we need to extract the exact data the user asked for (from [begin, begin + count] which is all contained somewhere in our temp_buf. load this data into buf and the user gets what he asked for. also check how to return how much we wrote to his buffer since that is the return of this function.
    memcpy(buf, &temp_buf, count);// TODO: Debug this.
    return 0;
}

/*
int LFUAlgo::fetch_from_file(int fd, char *buf, int block_start, int block_end, int begin, int count)
{
    //this function tries to get most of the data from cache memory, and if its not found it takes it from the actual file, save bulks to cache (and removing bulks if needed).
    if(should_cout)
    {
        cout <<"\n###########################################" << endl;
        cout << "This is function fetch_from_file! " << endl;
        cout << "I got a buffer address to load the results to after I manage to fetch all the data " << endl;
        cout << " I have some more data: " << endl;
        cout << "file id: " << fd << endl;
        cout << "The first block of the file that I need: " << block_start << endl;
        cout << "The last block of the file that I need: " << block_end << endl;
        cout << "My start offset: " << begin << endl;
        cout << "How much should I read: " << count << endl;
    cout <<"###########################################" << endl;
        
    }
    char * temp_buf = (char*)malloc(sizeof(this->block_size));
    for(int i = block_start; i < block_end + 1; i++)
    {
        bulk_struct bulk = get_block(fd, i);
        if (bulk.pointer_to_memory == nullptr)
        {
            //TODO:
            //block not in cache, need to read it from the file and then append it, and save it in cache.
            //use functions:
            // get_block_for_writing(). if it returns NULL, it means there is no free block for writing, so you need to remove a block depending on the algo that is currently being used.
        } else {
            //TODO:
            //found the block in cache, need to append it
            memcpy( &temp_buf[i*block_size], bulk.pointer_to_memory, block_size);// TODO: Debug this. make sure it works.-> added <cstring> is it good?
        }
        //now after we have all of our blocks in temp_buf, we need to extract the exact data the user asked for (from [begin, begin + count] which is all contained somewhere in our temp_buf. load this data into buf and the user gets what he asked for. also check how to return how much we wrote to his buffer since that is the return of this function.
    }
    return 0;
}

int LRUAlgo::fetch_from_file(int fd, char *buf, int block_start, int block_end, int begin, int count)
{
    //this function tries to get most of the data from cache memory, and if its not found it takes it from the actual file, save bulks to cache (and removing bulks if needed).
    return 0;
}

int FBRAlgo::fetch_from_file(int fd, char *buf, int block_start, int block_end, int begin, int count)
{
    //this function tries to get most of the data from cache memory, and if its not found it takes it from the actual file, save bulks to cache (and removing bulks if needed).
    return 0;
}
*/

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

