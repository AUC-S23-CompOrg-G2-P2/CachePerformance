#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(16*1024)

enum cacheResType {MISS=0, HIT=1};

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
		// A spceial case is when numberOfSets = 1, in which case the set is fully associative and index is 0
		// Another special case is when ways = 1, in which case the set is direct mapped.
		bool read(unsigned int addr){
			// Someone pls check my math
			unsigned int index = (addr/lineSize)%numberOfSets;
			unsigned int tag = addr/(lineSize*numberOfSets);
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
				// FIFO set replacement policy (pop the first item in the set and push the new one to the end)
				myCache[index].erase(myCache[index].begin());
				CacheLine myLine;
				myLine.valid = true;
				myLine.tagAndIndex = tag;
				myCache[index].push_back(myLine);
				return false;
				
				
			}
		}
	private:
		vector<vector<CacheLine>> myCache;
		int setSizeInBytes;
		int numberOfSets;
		int lineSize;
		int ways;
};

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

const char *msg[2] = {"Miss","Hit"};

#define		NO_OF_Iterations	1000000
int main()
{
	/*
	vector<Cache> experiment1;
	vector<Cache> experiment2;
	// experiment one will run for every possible line size (16 B, 32 B, 64 B, 128 B) with number of ways being 4
	for(int i = 0; i<4;i++){
		experiment1.push_back(Cache(16*pow(2,i),4));
	}
	// experiment two will run for every possible number of ways (1,2, 4, 8, 16) with line size being 32 B
	for(int i = 0; i<5;i++){
		experiment2.push_back(Cache(32,pow(2,i)));
	}
	// Now we will plot the hit ratio against line size for experiment 1
	cout << "Effect of line size on hit ratio\n";
	for(int i = 0; i<experiment1.size();i++){
		unsigned int hit = 0;
		unsigned int addr;
		for(int inst=0;inst<NO_OF_Iterations;inst++)
		{
			addr = memGen6();
			if(experiment1[i].read(addr))
				hit++;
			//r = static_cast<cacheResType>(experiment1[i].read(addr));
			//if(r == HIT) hit++;
			//cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
		}
		cout << "Hit ratio of experiment 1 line size "<< (int)16*pow(2,i)<<" B = " << fixed << setprecision(10) <<((double)100*hit/(NO_OF_Iterations)*1.0)<< "%"<<endl;
	}
	cout << "--------------------------------------------------\n";

	// Now we will plot the hit ratio against number of ways for experiment 2
	for(int i = 0; i<experiment2.size();i++){
		unsigned int hit = 0;
		cacheResType r;
		unsigned int addr;
		cout << "Effect of number of ways on hit ratio\n";
		for(int inst=0;inst<NO_OF_Iterations;inst++)
		{
			addr = memGen6();
			if(experiment2[i].read(addr))
				hit++;
			//cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
		}
		cout <<  "Hit ratio of experiment 2 number of ways "<< (int)(pow(2,i)) << " = "<< fixed << setprecision(10)  << ((double)(100*hit)/(double)NO_OF_Iterations)<< "%"<<endl;
	}
	cout << "--------------------------------------------------\n";
	*/
	for(int line_size=16; line_size<=128;line_size*=2){
		for(int i=1;i<=6;i++){
			Cache mycache(line_size,4);		// Create a cache of size 16B, 32B, 64B, 128B
			unsigned int hit = 0;
			
			// assign addr value from memGen number i using a switch case
			
			for(int inst=0;inst<NO_OF_Iterations;inst++)
			{
				unsigned int addr;
			switch (i)
			{
			case 1:
				addr = memGen1();
				break;
			case 2:
				addr = memGen2();
				break;
			case 3:
				addr = memGen3();
				break;
			case 4:
				addr = memGen4();
				break;
			case 5:
				addr = memGen5();
				break;
			case 6:
				addr = memGen6();
				break;
			default:
				break;
			}
				if(mycache.read(addr))
					hit++;
			}
			cout << "Hit ratio of experiment 1 line size "<< line_size<<" B and memGen"<<i<<" = " << fixed << setprecision(10) <<((double)100*hit/(NO_OF_Iterations)*1.0)<< "%"<<endl;
		}
	}
	// Now we will plot the hit ratio against number of ways for experiment 2
	for(int ways=1; ways<=16;ways*=2){
		for(int i=1;i<=6;i++){
			Cache mycache(32,ways);		// Create a cache of size 16B, 32B, 64B, 128B
			unsigned int hit = 0;
			
			// assign addr value from memGen number i using a switch case
			
			for(int inst=0;inst<NO_OF_Iterations;inst++)
			{
				unsigned int addr;
			switch (i)
			{
			case 1:
				addr = memGen1();
				break;
			case 2:
				addr = memGen2();
				break;
			case 3:
				addr = memGen3();
				break;
			case 4:
				addr = memGen4();
				break;
			case 5:
				addr = memGen5();
				break;
			case 6:
				addr = memGen6();
				break;
			default:
				break;
			}
				if(mycache.read(addr))
					hit++;
			}
			cout << "Hit ratio of experiment 2 number of ways "<< ways << " and memGen"<<i<<" = "<< fixed << setprecision(10)  << ((double)(100*hit)/(double)NO_OF_Iterations)<< "%"<<endl;
		}
	}
	
}