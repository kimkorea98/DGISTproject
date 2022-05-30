// System Programming, 2022, DGIST
// Written by Yeseong Kim for Homework 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	int count;
	int fsize;
	char dat[20];
	char* orig_buf;
	char* comp_buf;
	char temp[100];

	int * fsize_list;
	fsize_list = malloc(argc-2);
	
	fpw = fopen(argv[1], "w"); 
		

	//data compress
	for( int i = 2; i <argc ; i ++){
		fp  = fopen(argv[i], "r");
		if(fp == NULL){
			printf("there is no  file\n");
			exit(1);
		}
		fseek(fp, 0, SEEK_END);
		fsize  = ftell(fp);
		//printf("fsize %d\n", fsize); 
		orig_buf = malloc(fsize+1);
		comp_buf = malloc(fsize*1.05);
		memset(orig_buf, 0, fsize+1);
		memset(comp_buf, 0, fsize*1.05);
		fseek(fp, 0, SEEK_SET);
		count  = fread(orig_buf, fsize, 1, fp);

		//printf("%s \n size: %d, count: %d\n", orig_buf, fsize, count);
		int comp_size = fastlz_compress(orig_buf, fsize, comp_buf);
		//printf("%d bytes are compressed to %d\n", fsize, comp_size);
		fsize_list[i-2] = comp_size;
		
		count = fwrite(comp_buf, comp_size, 1,  fpw);
		fsize  = ftell(fpw);
		//printf("input size %d, %d\n", count, fsize);
		//printf("compress : %s size %d\n" , comp_buf, (int)sizeof(comp_buf));

		free(orig_buf);
		free(comp_buf);

	}
	//metadata
	fputs("\n", fpw);
	for (int j =0; j <argc-2; j++){
		sprintf(temp, "%s, %d,", argv[j+2], fsize_list[j]);
		fputs(temp, fpw);
	}	
	
	sprintf(temp, "%d\n", argc-2);
	fputs(temp, fpw);
	fseek(fpw, 0, SEEK_END);
	fsize  = ftell(fpw);
	//printf("size %d\n", fsize);
	free(fsize_list);
	
	printf("Compression is done\n");
	//
	//ust_for_your_understanding();

	return 0;
}
