#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)

enum cacheResType {MISS=0, HIT=1};

struct CacheLine{
	bool valid;
	unsigned int tagAndIndex;
	CacheLine(){
		valid = false;
		tagAndIndex = 0;
	}
}

class Cache{
	public:
		Cache(int lineSizeInBytes =0, int ways = 0){
			lineSizeInBytes = lineSizeInBytes;
			ways = ways;
			setSizeInBytes = lineSizeInBytes*8*ways;
			numberOfSets = CACHE_SIZE/setSizeInBytes;
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
		// A spceial case is when numberOfSets = 1, in which case the set is fully associative and index is 0
		// Another special case is when ways = 1, in which case the set is direct mapped.
		bool read(unsigned int addr){
			// Someone pls check my math
			unsigned int index = (addr/lineSizeInBytes)%numberOfSets;
			unsigned int tag = addr/(lineSizeInBytes*numberOfSets);
			// special case for fully associative
			if(numberOfSets == 1){
				index = 0;
			}
			// special case for direct mapped
			if(ways == 1){
				if(myCache[index][0].valid && myCache[index][0].tagAndIndex == tag){
					return true;
				}
				else{
					// miss due to cold start or conflict
					myCache[index][0].valid = true;
					myCache[index][0].tagAndIndex = tag;
					return false;
				}
			}
			else{
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
				// FIFO set replacement policy
				myCache[index][0].tagAndIndex = tag;
				return false;
			}
		}
	private:
		vector<vector<CacheLine>> myCache;
		int setSizeInBytes;
		int numberOfSets;
		int lineSizeInBytes;
		int ways;
}

/* The following implements a random number generator */
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr=0;
	return  rand_()%(24*1024);
}

unsigned int memGen3()
{
	return rand_()%(DRAM_SIZE);
}

unsigned int memGen4()
{
	static unsigned int addr=0;
	return (addr++)%(4*1024);
}

unsigned int memGen5()
{
	static unsigned int addr=0;
	return (addr++)%(1024*64);
}

unsigned int memGen6()
{
	static unsigned int addr=0;
	return (addr+=32)%(64*4*1024);
}


// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr)
{	
	// This function accepts the memory address for the memory transaction and 
	// returns whether it caused a cache miss or a cache hit

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr)
{	
	// This function accepts the memory address for the read and 
	// returns whether it caused a cache miss or a cache hit

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}
char *msg[2] = {"Miss","Hit"};

#define		NO_OF_Iterations	100		// Change to 1,000,000
int main()
{
	unsigned int hit = 0;
	cacheResType r;
	
	unsigned int addr;
	cout << "Direct Mapped Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen2();
		r = cacheSimDM(addr);
		if(r == HIT) hit++;
		cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "Hit ratio = " << (100*hit/NO_OF_Iterations)<< endl;
}