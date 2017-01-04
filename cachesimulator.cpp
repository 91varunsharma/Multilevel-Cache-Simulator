/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <stdio.h>
#include <map>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
    unsigned long L1blocksize;
    unsigned long L1setsize;
    unsigned long L1size;
    unsigned long L2blocksize;
    unsigned long L2setsize;
    unsigned long L2size;
};

/* you can define the cache class here, or design your own data structure for L1 and L2 cache*/

class cache {
    config config_params;
    unsigned long setIndexBit_L1, tag_L1, blockOffset_L1, setIndexBit_L2, tag_L2, blockOffset_L2;
    vector<vector<unsigned long> > cache_L1;
    vector<vector<unsigned long> > cache_L2;
    vector<vector<unsigned long> > validBit_L1, validBit_L2;


    public :
        cache(config config_param){
            this->config_params = config_param;

            this->blockOffset_L1 = (unsigned long)log2(config_params.L1blocksize);
            this->blockOffset_L2 = (unsigned long)log2(config_params.L2blocksize);

            this->setIndexBit_L1 = (unsigned long)log2((config_params.L1size)*(pow(2,10))/(config_params.L1blocksize*config_params.L1setsize));
            this->setIndexBit_L2 = (unsigned long)log2((config_params.L2size)*(pow(2,10))/(config_params.L2blocksize*config_params.L2setsize));

            this->tag_L1 = 32 - setIndexBit_L1 - blockOffset_L1;
            this->tag_L2 = 32 - setIndexBit_L2 - blockOffset_L2;

            unsigned long indexRow_L1 = (unsigned long)pow(2, setIndexBit_L1);
            unsigned long indexRow_L2 = (unsigned long)pow(2, setIndexBit_L2);

            this->validBit_L1.resize(this->config_params.L1setsize);
            for(int i=0;i<this->config_params.L1setsize;i++){
                this->validBit_L1[i].resize(indexRow_L1);
            }

            this->validBit_L2.resize(this->config_params.L2setsize);
            for(int i=0;i<this->config_params.L2setsize;i++){
                this->validBit_L2[i].resize(indexRow_L2);
            }


            unsigned long blockSize_L1 = (unsigned long)pow(2, blockOffset_L1);
            unsigned long blockSize_L2 = (unsigned long)pow(2, blockOffset_L2);

            this->cache_L1 = createCache(config_params.L1setsize, indexRow_L1, blockSize_L1);
            this->cache_L2 = createCache(config_params.L2setsize, indexRow_L2, blockSize_L2);
        }

        

        vector<string> getBits(bitset<32> addr) {
            string stringAddr = addr.to_string();
            vector<string> bits(6);
            bits[0] = stringAddr.substr(0, this->tag_L1);
            bits[1] = stringAddr.substr(this->tag_L1, this->setIndexBit_L1);
            bits[2] = stringAddr.substr(this->tag_L1+this->setIndexBit_L1, this->blockOffset_L1);

            bits[3] = stringAddr.substr(0, this->tag_L2);
            bits[4] = stringAddr.substr(this->tag_L2, this->setIndexBit_L2);
            bits[5] = stringAddr.substr(this->tag_L2+this->setIndexBit_L2,this->blockOffset_L2);
            return bits;
        }

        vector<vector<unsigned long> > createCache(unsigned long &way, unsigned long &indexRow, unsigned long &blockSize){
            vector<vector<unsigned long> > myCache;

            myCache.resize((unsigned long)way);
            for(int i=0; i<way;i++) {
                myCache[i].resize(indexRow);

            }

            return myCache;
        }

    vector<vector<unsigned long> > getL1Cache(){
        return this->cache_L1;
    }

    vector<vector<unsigned long> > getL2Cache(){
        return this->cache_L2;
    }

    vector<vector<unsigned long> > getValidL1(){
        return this->validBit_L1;
    }

    vector<vector<unsigned long> > getValidL2(){
        return this->validBit_L2;
    }





};

map<unsigned long, bool> isWaysFull(vector<vector<unsigned long> > &L_cache, unsigned long way_L, long index_L){

    map<unsigned long, bool> cacheMap;

    for(unsigned long i=0;i<way_L;i++){

        cacheMap[i] = L_cache[i][index_L] != 0;
    }
    
    return cacheMap;
}

void loadTag(vector<vector<unsigned long> > &L_cache, vector<vector<unsigned long> > &L_validBit, unsigned long &way_L,unsigned long &wayNumber, long &index_L, long tag_L){

    L_cache[wayNumber][index_L] = (unsigned long)tag_L;
    L_validBit[wayNumber][index_L] = 1;

}

unsigned long incCounter (unsigned long &way_L, unsigned long &wayNumber){

return (wayNumber+1)%way_L;

}

int main(int argc, char* argv[]){



    config cacheconfig;
    ifstream cache_params;
    string dummyLine;

    cache_params.open("cacheconfig.txt");

        while (!cache_params.eof())  // read config file
        {
            cache_params >> dummyLine;
            cache_params >> cacheconfig.L1blocksize;
            cache_params >> cacheconfig.L1setsize;
            cache_params >> cacheconfig.L1size;
            cache_params >> dummyLine;
            cache_params >> cacheconfig.L2blocksize;
            cache_params >> cacheconfig.L2setsize;
            cache_params >> cacheconfig.L2size;
        }

    
    if(cacheconfig.L1setsize == 0){
        cacheconfig.L1setsize = cacheconfig.L1size*1024/cacheconfig.L1blocksize;
    }
    if(cacheconfig.L2setsize == 0){
        cacheconfig.L2setsize = cacheconfig.L2size*1024/cacheconfig.L2blocksize;
    }

    cache cacheObj(cacheconfig);



        // Implement by you:
        // initialize the hierarchy cache system with those configs
        // probably you may define a Cache class for L1 and L2, or any data structure you like
    vector<vector<unsigned long> > L1_cache = cacheObj.getL1Cache();
    vector<vector<unsigned long> > L2_cache = cacheObj.getL2Cache();

    vector<vector<unsigned long> > validBit_L1 = cacheObj.getValidL1();
    vector<vector<unsigned long> > validBit_L2 = cacheObj.getValidL2();

    unsigned long sizeL1Way = L1_cache.size();
    unsigned long sizeL2Way = L2_cache.size();
    unsigned long row_L1 = L1_cache[0].size();
    unsigned long row_L2 = L2_cache[0].size();

    


    int L1AcceState = 0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState = 0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;


        ifstream traces;
        ofstream tracesout;
        string outname;
        outname=string("traceout.txt");  

        traces.open("trace.txt");
        tracesout.open(outname.c_str());  

        string line;
        string accesstype;  // the Read/Write access type from the memory trace;
        string xaddr;       // the address from the memory trace store in hex;
        unsigned int addr;  // the address from the memory trace store in unsigned int;
        bitset<32> accessaddr; // the address from the memory trace store in the bitset;

        vector<unsigned long> counter_L1, counter_L2;
        unsigned long temp;
        counter_L1.resize(row_L1);
        counter_L2.resize(row_L2);
        int count=0;

        if (traces.is_open() && tracesout.is_open()) {
            while (getline(traces, line)) {   // read mem access file and access Cache
                count++;

                istringstream iss(line);
                if (!(iss >> accesstype >> xaddr)) { break; }

                stringstream saddr(xaddr);
                saddr >> std::hex >> addr;

                accessaddr = bitset<32>(addr);


                vector<string> bits = cacheObj.getBits(accessaddr);
                string strTagL1 = bits[0];
                string strIndexL1 = bits[1];
                string strOffsetL1 = bits[2];
                string strTagL2 = bits[3];
                string strIndexL2 = bits[4];
                string strOffsetL2 = bits[5];

                long  index_L1, index_L2;
                char * ptr;
                long  tag_L1= strtol(strTagL1.c_str(), & ptr, 2);
                long  tag_L2= strtol(strTagL2.c_str(), & ptr, 2);

                if(strIndexL1 != ""){
                    index_L1= strtol(strIndexL1.c_str(), & ptr, 2);
                }
                else{
                    index_L1 = 0;
                }

                if(strIndexL2 != ""){
                    index_L2= strtol(strIndexL2.c_str(), & ptr, 2);
                }
                else{
                    index_L2 = 0;
                }


                unsigned long hit_L1=0, hit_L2=0;

               

                //tag bit matching on L1
                for(unsigned long i=0; i<sizeL1Way; i++){
                    //way checking
                    if(L1_cache[i][index_L1] == tag_L1 && validBit_L1[i][index_L1]==1){
                        // hit

                        hit_L1=1;
                        break;
                    }
                    else{
                        //miss
                        hit_L1=0;
                    }
                }

                if(hit_L1== 0){
                    // goto L2

                    for(unsigned long i=0; i<sizeL2Way; i++){
                        //way checking
                        if(L2_cache[i][index_L2] == tag_L2){
                            // hit

                            hit_L2=1;
                            break;
                        }
                        else{
                            //miss

                            hit_L2=0;
                        }
                    }

                }

                map<unsigned long, bool> isFull;

                // access the L1 and L2 Cache according to the trace;
                if (accesstype.compare("R") == 0) {
                    //Implement by you:
                    // read access to the L1 Cache,
                    //  and then L2 (if required),
                    //  update the L1 and L2 access state variable;

                    if(hit_L1==0 && hit_L2 ==0){

                        L1AcceState = RM;
                        L2AcceState = RM;

                        //Goto main memory and fetch the data of that tag
                        //update the L2 cache

                        //checking which L2 Ways are empty
                        isFull = isWaysFull(L2_cache, sizeL2Way, index_L2);
                        temp = 0;

                        //counter_L2 will be having the Way number in which we have to load the data
                        //checking whether Way number provided by counter is empty or not
                        while(isFull[counter_L2[index_L2]] && temp<sizeL2Way){
                            counter_L2[index_L2] = (++counter_L2[index_L2])%sizeL2Way;
                            ++temp;
                        }

                        // Finally loading Tag onto L2 cache
                        loadTag(L2_cache, validBit_L2, sizeL2Way,counter_L2[index_L2], index_L2, tag_L2);
                        counter_L2[index_L2] = incCounter(sizeL2Way, counter_L2[index_L2]);


                        // update the L1 cache
                        isFull = isWaysFull(L1_cache, sizeL1Way, index_L1);
                        temp = 0;

                        while(isFull[counter_L1[index_L1]] && temp<sizeL1Way){
                            counter_L1[index_L1] = (++counter_L1[index_L1])%sizeL1Way;
                            ++temp;
                        }

                        loadTag(L1_cache, validBit_L1, sizeL1Way, counter_L1[index_L1], index_L1, tag_L1);
                        counter_L1[index_L1] = incCounter(sizeL1Way, counter_L1[index_L1]);
                    }
                    else if(hit_L1 == 0 && hit_L2 == 1){

                        L1AcceState = RM;
                        L2AcceState = RH;

                        //when there is a read miss in L1 and read hit in L2, the L1 cache might have to evict some data to make room for the data returned by the L2 cache.
                        //If the evicted data is dirty, this will result in a write access to L2. If the write access to L2 results in a hit no data will be evicted/replaced.
                        //If it results in a miss, the data will be forwarded to main memory without changing the L2 cache state since the L2 cache is a write-no-allocate cache

                        //put content of l2 into l1
                        loadTag(L1_cache, validBit_L1, sizeL1Way, counter_L1[index_L1], index_L1, tag_L1);
                        counter_L1[index_L1] = incCounter(sizeL1Way, counter_L1[index_L1]);


                    }
                    else if(hit_L1 == 1){

                        L1AcceState = RH;
                        L2AcceState = NA;


                    }

                    isFull = isWaysFull(L1_cache, sizeL1Way, index_L1);

                   

                } else {
                    //Implement by you:
                    // write access to the L1 Cache,
                    //and then L2 (if required),
                    //update the L1 and L2 access state variable;

                    if (hit_L1 == 0 && hit_L2 == 0) {

                        L1AcceState = WM;
                        L2AcceState = WM;

                        //

                    } else if (hit_L1 == 0 && hit_L2 == 1) {
                        L1AcceState = WM;
                        L2AcceState = WH;

                        //Do not allocate any space on L1. Pass the write access to L2
                        //Goto L2 cache and update the data array with the new data on that tag


                    } else if (hit_L1 == 1) {
                        L1AcceState = WH;
                        L2AcceState = NA;
                        // update the data array on this tag at L1


                    }

                    
                }


                tracesout << L1AcceState << " " << L2AcceState
                          << endl;  // Output hit/miss results for L1 and L2 to the output file;


            }
            traces.close();
            tracesout.close();
        } else cout << "\n Unable to open trace or traceout file ";




    return 0;
}
