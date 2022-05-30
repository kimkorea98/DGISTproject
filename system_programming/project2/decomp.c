// System Programming, 2022, DGIST
// Written by Yeseong Kim for Homework 2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "lib_fastlz/fastlz.h"

void just_for_your_understanding() {
	unsigned char orig_buf[512];

	// 0. Initialize the buffer with some values
	int i;
	for (i = 0; i < 512; ++i) {
		orig_buf[i] = i % 100;
	}


	// 1. Try compression:
	// It compresses the data in orig_buf to another buffer (comp_buf.)
	// You need to give the size of the data compressed.
	// You will get the size of the compressed data as the return value.
	// Note that the compressed size might be larger than the original.
	unsigned char comp_buf[1024];
	int comp_size = fastlz_compress(orig_buf, 512, comp_buf); 
	printf("512 bytes are compressed to %d\n", comp_size);


	// 2. Try decompression:
	// It decompresses the data in comp_buf to decomp_buf.
	// You need to give the compressed data size of comp_buf.
	// You should also give the decomp_buf size, here 2048 bytes,
	// to specify the maximum size of the output buffer.
	unsigned char decomp_buf[2048];
	int decomp_size = fastlz_decompress(
			comp_buf, comp_size, decomp_buf, 2048);
	printf("%d bytes are decompressed to %d\n", decomp_size, comp_size);

	// 3. Check all things are correctly decompressed
	if (decomp_size != 512) {
		printf("Something went wrong. ");
		printf("The decompressed size differs from ");
		printf("the original buffer size.\n");
		return;
	}
	for (i = 0; i < 512; ++i) {
		if (orig_buf[i] != decomp_buf[i]) {
			printf("Something went wrong. ");
			printf("The decompressed data differ from ");
			printf("the original buffer.\n");
			return;
		}
	}

	printf("Succeed!\n");
}

int main(int argc, char* argv[]) {
	
	FILE* fp;
	FILE* fpw;

	int cnt = 3;
	int decomp_size;
	int result;
	int fsize;
	char fname[64]; //file name is less then 64 byte
	char* comp_buf;
	char decomp_buf[200000];
	char temp[128];
	char temp2[128];
	char num[10];
	
	fp = fopen(argv[1], "r");
	//find last line
	while ( strcmp(temp, "\n") != 0){ 
		//printf("%s", temp);
		fseek(fp, -cnt, SEEK_END);
        	fgets(temp, sizeof(temp), fp);
		cnt++;
	}

	//get last line
        fgets(temp, sizeof(temp), fp);
	printf("last sentense : %s", temp);
	strcpy(temp2, temp);
	fseek(fp, 0, SEEK_SET);
	
	//tokenize
	char* ptr = strtok(temp, " ,");
	while(ptr !=  	NULL){
		strcpy(num, ptr);
		ptr = strtok(NULL, " ,");
	}
	//printf("last sentense : %s\n", temp2);
	printf("of source files: %s\n", num);
	
	//make directory
	int nResult = mkdir(argv[2], 0777);
	if(nResult ==0){
		//printf("directory file is maden\n");
	}
	else{
		//printf("error is occured when directory file is making\n");
	}

	
	// file decompree
	ptr = strtok(temp2, " ,");
	cnt = 0;
	while(ptr != NULL){
		if(cnt%2 == 0){
			strcpy(fname, ptr);
		}
		else{
			fsize = atoi(ptr);
		}
		if(cnt %2 ==1){
			printf("reading: %s\n", fname);
			sprintf(temp, "./data/%s", fname);
			fpw = fopen(temp, "w");
			
			comp_buf = malloc(fsize+1);
			result = fread(comp_buf, fsize, 1, fp);
			
			//printf("read error %d, %d, %s \n", (int)ftell(fp), fsize, comp_buf);
			decomp_size = fastlz_decompress(comp_buf, fsize, decomp_buf, 200000);
			//printf("check decomp_size : %d \n decompressed sentence :%s\n", decomp_size, decomp_buf);
			fwrite(decomp_buf, decomp_size, 1, fpw);
			

			fclose(fpw);


		}
		ptr = strtok(NULL, " , ");
		cnt++;
	}
	
	fclose(fp);
	printf("Decompression is done\n");
	return 0;

}
