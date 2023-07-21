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

int memoryGenWithStep(int addr,int step){
    return (addr+=step);
}
// We will now compare the cache preformance as step size increases
int main(){
    Cache myCache(64,1);
    int hits = 0;
    int misses = 0;
    int count = 0;
    int addr = 0;
    for(int step = 1; step<65;step++){
        while(count++ < 1000000){
            if(myCache.read(addr)){
                hits++;
            }else{
                misses++;
            }
            addr = memoryGenWithStep(addr,step);
        }
        cout << "Step size: " << step << endl;
        int calculatedMisses = (step-1.0)*(floor((64.0/(double)step)))+64%step;
        int calculatedHits = ceil(64.0/((double)step));
        int virtualLineSize = floor(64.0/((double)step+64%step));
        cout << "Expected hit rate: " << (double) (calculatedHits)/(double(calculatedHits+calculatedMisses)) << "\t";
        cout << "Hit rate: " << (double)hits/(double)(hits+misses) << endl;
        hits = 0;
        misses = 0;
        count = 0;
        addr = 0;
    }
    
    return 0;
}