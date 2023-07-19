#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(16*1024)

struct CacheLine{
	bool valid = false;
	unsigned int tagAndIndex = 0;
};

class Cache{
	public:
		Cache(int lineSizeInBytes, int numberOfWays){
			lineSize = lineSizeInBytes;
			ways = numberOfWays;
			setSizeInBytes = lineSizeInBytes*ways;
			numberOfSets = (CACHE_SIZE)/setSizeInBytes;
			for(int i = 0; i<numberOfSets;i++){
				// initialise each set
				vector<CacheLine> mySet;
				for(int j = 0; j<ways;j++){
					// initialise each line
					CacheLine myLine;
					mySet.push_back(myLine);
				}
				myCache.push_back(mySet);
			}

		}
		// A function that will attempt to read a given DRAM address from the cache. 
		// If it is found return true, else add it to its correct place in myCache and return false
		bool read(unsigned int addr){
			unsigned int index = (addr/lineSize)%numberOfSets;
			unsigned int tag = addr/(lineSize*numberOfSets);
			
				for(int i = 0; i<ways;i++){
					if(myCache[index][i].valid && myCache[index][i].tagAndIndex == tag){
						return true;
					}
				}
				// miss due to cold start or capacity
				// find the first invalid line and replace it
				for(int i = 0; i<ways;i++){
					if(!myCache[index][i].valid){
						myCache[index][i].valid = true;
						myCache[index][i].tagAndIndex = tag;
						return false;
					}
				}
				// if we get here, we have a capacity miss and all lines are valid
				// FIFO set replacement policy (pop the first item in the set and push the new one to the end)
				myCache[index].erase(myCache[index].begin());
				CacheLine myLine;
				myLine.valid = true;
				myLine.tagAndIndex = tag;
				myCache[index].push_back(myLine);
				return false;

		}
	private:
		vector<vector<CacheLine>> myCache;
		int setSizeInBytes;
		int numberOfSets;
		int lineSize;
		int ways;
};

int sequentialMemoryGenerator(){
    static int addr = 0;
    return addr++;
}

// Main will test the hit rate for caches of increasing line size using addreses from sequentialMemoryGenerator()

int main(){
    int lineSize = 4;
    int numberOfWays = 1;
    int numberOfHits = 0;
    int numberOfMisses = 0;
    int numberOfAddresses = 1000000;
    int numberOfIterations = 5;
    for(int i = 0; i<numberOfIterations;i++){
        Cache myCache(lineSize,numberOfWays);
        for(int j = 0; j<numberOfAddresses;j++){
            if(myCache.read(sequentialMemoryGenerator())){
                numberOfHits++;
            }else{
                numberOfMisses++;
            }
        }
        cout << "Line size: " << lineSize << " Number of ways: " << numberOfWays << endl << "Expected hit rate: "<< (double)(lineSize-1)/(double)(lineSize) << " Hit rate: " << (double)numberOfHits/(double)(numberOfHits+numberOfMisses) << endl;
        lineSize*=2;
        numberOfHits = 0;
        numberOfMisses = 0;
    }
    // Print a horizontal line
    cout << "----------------------------------------" << endl;
    // We will now do the same but for increasing numbers of ways
    lineSize = 4;
    numberOfWays = 1;
    numberOfHits = 0;
    numberOfMisses = 0;
    for(int i = 0; i<numberOfIterations;i++){
        Cache myCache(lineSize,numberOfWays);
        for(int j = 0; j<numberOfAddresses;j++){
            if(myCache.read(sequentialMemoryGenerator())){
                numberOfHits++;
            }else{
                numberOfMisses++;
            }
        }
        cout << "Line size: " << lineSize << " Number of ways: " << numberOfWays << endl << "Expected hit rate: "<< (double)(lineSize-1)/(double)(lineSize) << " Hit rate: " << (double)numberOfHits/(double)(numberOfHits+numberOfMisses) << endl;
        numberOfWays*=2;
        numberOfHits = 0;
        numberOfMisses = 0;
    }
    
    return 0;
}
