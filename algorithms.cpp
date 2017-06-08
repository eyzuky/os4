//
//  algorithms.cpp
//  os4
//
//  Created by Eyal Silberman on 31/05/2017.
//  Copyright © 2017 Eyal Silberman. All rights reserved.
//

#include "algorithms.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <vector>
#include <algorithm>
#include <fstream>

bool should_cout = true;
CacheAlgo::CacheAlgo(int blocks_num, int block_size): blocks_num(blocks_num), block_size(block_size)
{
    misses = 0;
    hits = 0;
    //allocate our cache memory.
    blockAvailability = new block_availability[blocks_num];
    cache_memory = new char* [blocks_num];
    for (int i = 0; i < blocks_num; ++i)
    {
        cache_memory[i] = (char*) aligned_alloc(block_size,block_size); // do not change the aligned allov the pread works only with it
        blockAvailability[i] = FREE;
    }

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
    for (int i = 0; i < blocks_num; ++i)
    {
        free (cache_memory[i]);
    }
    free(cache_memory); // todo delete properlly
    free (blockAvailability);
    if(should_cout)
    {
     cout << "cache algo destructor finish" << endl;
    }
}
//over-writen funcs
int CacheAlgo::remove_block_from_cache() {return FAIL;}

bool CacheAlgo::block_use(int index) {return true;}

//functional
void CacheAlgo::new_file(int fd, const char* pathname, off_t file_size)
{
    //allocate array of bulk_struct for the new file
    int array_size = (int)(file_size / block_size) + 1;
    vector<Block*> block_vec;
    for (int i = 0; i < array_size; i++){
        block_vec.push_back(new Block);
        block_vec.back()->cache_index = FAIL;
    }
    if(should_cout)
    {
        cout << "=====================\n";
        cout << "file: " << pathname << endl;
        cout << "fd: " << fd << endl;
        cout << "Size: " << file_size << endl;
        cout << "block size: " << block_size << endl;
        cout << "bulks allocated: " << (int)(file_size / block_size) + 1 << endl;
        cout << "=====================\n\n";
    }
//    cout << "~~~~~~~~~~~~ printing map ~~~~~~~~~~~~~" << endl;
//    for(auto item: block_vec)
//    {
//        cout << item->cache_index << endl;
//    }
//    cout << "~~~~~~~~~~~~============== ~~~~~~~~~~~~~" << endl;
    //have access to the array using the pathname -> array map.
    pathname_to_bulks[pathname] = block_vec;
    //have access to the pathname using the fd->pathname map. this 2 maps will work together. Why?
    //because the user may open the same file twice, getting 2 fd's that point to the same file with different relative paths. so multiple fd's may be mapped to the same pathname, and then we only open the file once.
    fd_to_pathname[fd] = pathname;
}

//call this function to get a free block! if there is no free block available, we will need to remove a block from the cache (declared remove_block_from_cache(). Each algorithm defines this function for its own method.
int CacheAlgo::get_block_for_writing()
{
    for ( int i = 0; i < blocks_num; i++){
        if (blockAvailability[i] == FREE){
            return i;
        }
    }
    cout << "didn't find memory, returning NULL " << endl;
    return FAIL;
}

Block * CacheAlgo::get_block(int fd, int block_in_file)
{
    //block_struct** file_array = pathname_to_bulks[fd_to_pathname[fd]];
    vector<Block*> file_array = pathname_to_bulks[fd_to_pathname[fd]];
    return file_array.at(block_in_file);
}

int CacheAlgo::fetch_from_file(int fd, void *buf, int block_start, int block_end, off_t seek, off_t offset,  int count)
{
//    cout <<"\n###########################################" << endl;
//    cout << "This is function fetch_from_file! " << endl;
//    cout << "I got a buffer address to load the results to after I manage to fetch all the data " << endl;
//    cout << " I have some more data: " << endl;
//    cout << "file id: " << fd << endl;
//    cout << "The first block of the file that I need: " << block_start << endl;
//    cout << "The last block of the file that I need: " << block_end << endl;
//    cout << "My start offset: " << seek << endl;
//    cout << "How much should I read: " << count << endl;
//    cout <<"###########################################" << endl;

    // this function tries to get most of the data from cache memory,
    // and if its not found it takes it from the actual file,
    // save bulks to cache (and removing bulks if needed).
    // Only the overriding of this function would actually be called
    int count_to_fetch = count;
    int malloc_size = ((block_end-block_start)+1)*this->block_size;
    char* temp_buf = (char *)malloc(malloc_size);
    if(temp_buf == NULL)
    {
        cout << "TEMP_BUF MALLOC FAILS\n";
        return  FAIL;
    }
    int counter = 0;
    for(int i = block_start; i < block_end + 1; i++)
    {

        Block* currBlock = get_block(fd, i);
//        cout << "BLOCK FROM GET BLOCK: " << currBlock << " " << currBlock.cache_index << endl;
        if (currBlock->cache_index == FAIL) {
            cout << "didn't find block: " << i << " in cache" << endl;
            this->misses += 1;
            currBlock->block_in_file = i;
            currBlock->pathname = (char *) fd_to_pathname[fd];
            //block not in cache, need to read it from the file and then append it, and save it in cache.
            //use functions: get_block_for_writing(). if it returns NULL,
            // it means there is no free block for writing,
            // so you need to remove a block depending on the algo that is currently being used.
            int to_read = get_block_for_writing();
            cout << "to_read : " << to_read << endl;

            if (to_read == FAIL)
            {
                //if get block returned FAIL, it means we need to remove a block from cache. luckily enough, the function remove block from cache returns the index of the removed block, so we can just set it into to_read and continue regularly.
                to_read = this->remove_block_from_cache();
            }
            currBlock->cache_index = to_read;
            // copying the whole block
            size_t pread_val ;
            auto cache = cache_memory[currBlock->cache_index];
            pread_val = (size_t) pread(fd, (void*)cache, (size_t)block_size, (off_t)(i * block_size));
            if ( pread_val != block_size ){

                cout << "fail <<<<------>>>>> pread" << endl;
            } else{
                blockAvailability[currBlock->cache_index] = USED;

            }
        }else{
            this->hits += 1;
        }

        this->block_use(currBlock->cache_index);
        index_to_block[currBlock->cache_index] = currBlock;
        char * data = cache_memory[currBlock->cache_index];

        char * ptr = temp_buf + (counter*block_size);


        memcpy(ptr, data , (size_t)block_size);
        counter += 1;

    }
    //now after we have all of our blocks in temp_buf, we need to extract the exact data the user asked for (from [begin, begin + count] which is all contained somewhere in our temp_buf. load this data into buf and the user gets what he asked for. also check how to return how much we wrote to his buffer since that is the return of this function.
    count_to_fetch = (int)offset - (max(0,block_start) * block_size);
    char * temp_buf_ptr = temp_buf + count_to_fetch;
    memcpy(buf, temp_buf_ptr, (size_t)count);
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

LRUAlgo::LRUAlgo(int blocks_num, int block_size):CacheAlgo(blocks_num, block_size)
{
    this->blocks_num = blocks_num;
    this->block_size = block_size;
    lru_counters = new int[blocks_num];
    for (int i = 0; i < blocks_num; ++i)
    {
        lru_counters[i] = i;
    }
}

LFUAlgo::LFUAlgo(int blocks_num, int block_size): CacheAlgo(blocks_num, block_size)
{
    this->blocks_num = blocks_num;
    this->block_size = block_size;
    lfu_counters = new int[blocks_num];
    for (int i = 0; i < blocks_num; ++i)
    {
        lfu_counters[i] = 0;
    }
}

FBRAlgo::FBRAlgo(int blocks_num, int block_size, double f_old, double f_new): CacheAlgo(blocks_num, block_size)
{
    this->blocks_num = blocks_num;
    this->block_size = block_size;
    lfu_counters = new int[blocks_num];
    for (int i = 0; i < blocks_num; ++i)
    {
        lfu_counters[i] = 0;
    }
}

int LRUAlgo::remove_block_from_cache()
{
    cout << "I am in remove block from cache of algo: LFU" << endl;
    int index = 0;
    int min = lru_counters[0];
    for (int i = 0; i < blocks_num; ++i)
    {
        if (lru_counters[i] < min)
        {
            min = lru_counters[i];
            index = i;
        }
    }
    blockAvailability[index] = FREE;
    lru_counters[index] = 0;
    index_to_block[index]->cache_index = FAIL;
    return index;
}

int LFUAlgo::remove_block_from_cache()
{
    cout << "I am in remove block from cache of algo: LFU" << endl;
    int index = 0;
    int min = lfu_counters[0];

    for (int i = 0; i < blocks_num; ++i)
    {
        if (lfu_counters[i] < min)
        {
            min = lfu_counters[i];
            index = i;
        }
    }
    blockAvailability[index] = FREE;
    lfu_counters[index] = 0;
    index_to_block[index]->cache_index = FAIL;
    return index;
}

int FBRAlgo::remove_block_from_cache()
{}

bool LFUAlgo::block_use(int index) {
    if (index == FAIL)
    {
        return false;
    }

    lfu_counters[index] += 1;
    cout << "~~~~~~~~~~~~ block used printing all the counters!~~~~~~~~~~~~~`" << endl;
    for (int i = 0; i < blocks_num; ++i)
    {
        cout << lfu_counters[i] << "   ";
    }
    cout << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`" << endl;

    return true;
}

bool LRUAlgo::block_use(int index) {
    int score = lru_counters[index];
    lru_counters[index] = this->blocks_num;
    for (int i = 0; i < blocks_num; ++i)
    {
        if (lru_counters[i] >= score )
        {
            lru_counters[i] -= 1;
        }
    }
    return true;
}

bool FBRAlgo::block_use(int index) {

    return true;
}

//printing
void CacheAlgo::print_cache(const char *log_path) {return;}

int CacheAlgo::CacheFS_print_stat(const char *log_path) {

    fstream stream;
    try{
        stream.open(log_path, fstream::app);
    }
    catch(...){
        return FAIL;
    }
        stream << "Hits number: " << this->hits << "." << endl;
        stream << "Misses number: " << this->misses << "." << endl;
        cout <<  "Hits number: " << this->hits << "." << endl;
        cout << "Misses number: " << this->misses << "." << endl;
    return 0;
}

void LFUAlgo::print_cache(const char *log_path) {

    vector<pair<int,int>> printing_order_vec;
    for (int i = 0; i < blocks_num; ++i)
    {
        printing_order_vec.push_back(make_pair(lfu_counters[i], i));
    }
    sort(printing_order_vec.begin(), printing_order_vec.end(), [](pair<int,int> &left, pair<int,int> &right){
        return left.first > right.first;
    });

    fstream stream;
    try{
        stream.open(log_path, fstream::app);
    }
    catch(...){
        return ;
    }
    for (auto item: printing_order_vec)
    {
        int index = item.second;
        Block* cur =  index_to_block[index];
        stream << cur->pathname << " " << cur->block_in_file << endl;
        cout << cur->pathname << " " << cur->block_in_file << endl;
    }
    return;
}
void LRUAlgo::print_cache(const char *log_path) {

    vector<pair<int,int>> printing_order_vec;
    for (int i = 0; i < blocks_num; ++i)
    {
        printing_order_vec.push_back(make_pair(lru_counters[i], i));
    }
    sort(printing_order_vec.begin(), printing_order_vec.end(), [](pair<int,int> &left, pair<int,int> &right){
        return left.first > right.first;
    });

    fstream stream;
    try{
        stream.open(log_path, fstream::app);
    }
    catch(...){
        return ;
    }
    for (auto item: printing_order_vec)
    {
        int index = item.second;
        Block* cur =  index_to_block[index];
        if(cur != NULL)
        {
            stream << cur->pathname << " " << cur->block_in_file << endl;
            cout << cur->pathname << " " << cur->block_in_file << endl;
        }
    }

    return;

}
void FBRAlgo::print_cache(const char *log_path) {return;}

