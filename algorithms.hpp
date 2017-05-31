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

class CacheAlgo
{
    //TODO - Declare virtual functions that all the classes will implement. Then we use polymorphism on them
    // and in CacheFS.cpp the code will not change for different algorithms.
    
    
};
//This class is for managing algo LEAST RECENTLY USED
class LRUAlgo: public CacheAlgo
{
public:
    
    LRUAlgo(int blocks_num){}
    ~LRUAlgo(){}
    
private:
    int blocks_num;
    
    
};

//This class is for managing also LEAST FREQUENTLY USED
class LFUAlgo: public CacheAlgo
{
public:
    LFUAlgo(int blocks_num){}
    ~LFUAlgo(){}
    
private:
    int blocks_num;
    
};

//Hybrid policy
class FBRAlgo: public CacheAlgo
{
public:
    FBRAlgo(int blocks_num, double f_old, double f_new){}
    ~FBRAlgo(){}

private:
    int blocks_num;
    double f_old;
    double f_new;
    
};











#endif /* algorithms_hpp */
