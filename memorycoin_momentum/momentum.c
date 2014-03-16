#include <stdlib.h>
#include <string.h>
#include "momentum.h"
#include <openssl/sha.h>
#include <openssl/aes.h>

#define PSUEDORANDOM_DATA_SIZE 30 //2^30 = 1GB
#define PSUEDORANDOM_DATA_CHUNK_SIZE 6 //2^6 = 64 bytes
#define L2CACHE_TARGET 16 // 2^16 = 64K
#define AES_ITERATIONS 50

// useful constants
const unsigned int psuedoRandomDataSize=(1<<PSUEDORANDOM_DATA_SIZE);
const unsigned int cacheMemorySize = (1<<L2CACHE_TARGET);
const unsigned int chunks=(1<<(PSUEDORANDOM_DATA_SIZE-PSUEDORANDOM_DATA_CHUNK_SIZE));
const unsigned int chunkSize=(1<<(PSUEDORANDOM_DATA_CHUNK_SIZE));
const unsigned int comparisonSize=(1<<(PSUEDORANDOM_DATA_SIZE-L2CACHE_TARGET));

bool momentum_verify(const char *midHash, unsigned int a, unsigned int b ){
	
	//Basic check
	if( a > comparisonSize ) return false;
	
	//Allocate memory required
	unsigned char *cacheMemoryOperatingData;
	unsigned char *cacheMemoryOperatingData2;
	cacheMemoryOperatingData = (unsigned char *)malloc(cacheMemorySize+16);
	cacheMemoryOperatingData2 = (unsigned char *)malloc(cacheMemorySize);
	unsigned int* cacheMemoryOperatingData32 = (unsigned int*)cacheMemoryOperatingData;
	unsigned int* cacheMemoryOperatingData322 = (unsigned int*)cacheMemoryOperatingData2;
	
	unsigned char  hash_tmp[sizeof(midHash)];
	memcpy((char*)&hash_tmp[0], (char*)&midHash, sizeof(midHash) );
	unsigned int* index = (unsigned int*)hash_tmp;
	
	unsigned int startLocation = a * cacheMemorySize / chunkSize;
	unsigned int finishLocation = startLocation + (cacheMemorySize / chunkSize);
		
	//copy 64k of data to first l2 cache
	unsigned int i;
	for(i = startLocation;i < finishLocation;i++) {
		*index = i;
		SHA512((unsigned char*)hash_tmp, sizeof(hash_tmp), (unsigned char*)&(cacheMemoryOperatingData[(i-startLocation)*chunkSize]));
	}
	
	unsigned char key[32] = {0};
	unsigned char iv[AES_BLOCK_SIZE];
	int outlen1, outlen2;
	
	int j;
	for(j=0;j<AES_ITERATIONS;j++){
		
		//use last 4 bits as next location
		startLocation = (cacheMemoryOperatingData32[(cacheMemorySize / 4) - 1] % comparisonSize) * cacheMemorySize / chunkSize;
		finishLocation = startLocation + (cacheMemorySize / chunkSize);
		unsigned int i;
		for(i = startLocation;i < finishLocation;i++) {
			*index = i;
			SHA512((unsigned char*)hash_tmp, sizeof(hash_tmp), (unsigned char*)&(cacheMemoryOperatingData2[(i-startLocation)*chunkSize]));
		}

		//XOR location data into second cache
		for(i = 0;i < cacheMemorySize / 4;i++){
			cacheMemoryOperatingData322[i] = cacheMemoryOperatingData32[i] ^ cacheMemoryOperatingData322[i];
		}
		
		//AES Encrypt using last 256bits as key
		
		AES_KEY AESkey;
		AES_set_encrypt_key((unsigned char*)&cacheMemoryOperatingData2[cacheMemorySize-32], 256, &AESkey);			
		memcpy(iv,(unsigned char*)&cacheMemoryOperatingData2[cacheMemorySize-AES_BLOCK_SIZE],AES_BLOCK_SIZE);
		AES_cbc_encrypt((unsigned char*)&cacheMemoryOperatingData2[0], (unsigned char*)&cacheMemoryOperatingData[0], cacheMemorySize, &AESkey, iv, AES_ENCRYPT);
		
	}
	//use last X bits as solution
	unsigned int solution = cacheMemoryOperatingData32[(cacheMemorySize/4)-1]%comparisonSize;
	unsigned int proofOfCalculation = cacheMemoryOperatingData32[(cacheMemorySize/4)-2];
	
	//free memory
	free(cacheMemoryOperatingData);
	free(cacheMemoryOperatingData2);
	
	if(solution == 1968 && proofOfCalculation == b){
		return true;
	}
	
	return false;
}
