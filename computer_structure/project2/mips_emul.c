#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define TRUE 1
#define FALSE 0
#define TEXTADDR 0x400000
#define DATAADDR 0x10000000

FILE *file;
int inst_size = 0;
int data_size = 0;

//PC
int PC;
int pc;

//instruction fetch
char op[7]="";
char rs[6]="";
char rt[6]="";
char rd[6]="";
char sh[6]="";
char fc[7]="";
char im[17]="";
char tg[27]="";

// register 32
int reg[32];

//control signal
int ALUOP;
int Jump;
int Branch;
int PCSrc;
int MemRead;
int MemtoReg;
int MemWrite;
int ALUSrc;
int RegWrite;
int RegDst;
int Shift;
int Lui;
//register input
int read_addr1;
int read_addr2;
int write_addr;
int write_data;
int read_data1;
int read_data2;
int alu_result;

int read_addr3;
int write_data3;
int read_data3;
int branch_PC;

struct INSTM{
	char inst[33];
	int loc;
}instm[200];
int im_size = 0;


struct DATAM{
	char data[33];
	int loc;
}datam[200];
int dm_size = 0;

char *toBi(int i, int size) {
	char *s = malloc(size*sizeof(char));	
	int count = size;
	do { s[--count] = '0' + (char) (i & 1);
		i = i >> 1;
	} while (count);
	return s;
}

void openFile(char *s){
	if((file = fopen(s, "r")) == NULL ){
		printf("no file");		
	}
}

int signExtend(){
	int a;
	int b = strtol(im, NULL, 2);
	if(im[0] =='1'){
		a = 0xffff0000|b;
	}
	else{
		a=b;
	}
	return a;
}

void preprocess(FILE *file){
	char line[100];
	int num;
	char *stop;
	int count = 0;
	int i = 0;
	while(fgets(line, sizeof(line), file) != NULL){
		//printf("%s" , line); //print object file
		num  = strtol(line,NULL, 16);
		if (count ==0){
			inst_size = num/4;
			count++;
		}
		else if (count ==1){
			data_size = num/4;
			count++;
		}
		else if(count < inst_size+2){	
			strcpy(instm[im_size].inst,toBi(num, 32));
			instm[im_size].loc = TEXTADDR + 4*im_size;
			im_size ++;
			count++;
		}
		else{
			strcpy(datam[dm_size].data,toBi(num, 32));
			datam[dm_size].loc = DATAADDR + 4*dm_size;
			dm_size ++;
			count++;
		}
	}
}

void controlUnit(){
	int opcode = strtol(op, NULL, 2);
	int func = strtol(fc, NULL, 2);
	//printf("opcode : %d, 0x%x, func: %d\n", opcode, opcode, func);
		Jump = FALSE;
		Branch = FALSE;
		MemRead = FALSE;
		MemtoReg = FALSE;
		MemWrite = FALSE;
		RegWrite = TRUE;
		Shift = FALSE;
		Lui = FALSE;
	
	if(opcode == 0){ // r type;
		ALUSrc = FALSE;
		RegDst = TRUE;
		if(func == 0x21){ //ADDU
			ALUOP = 4; // add
		}
		else if(func == 0x23){//SUBU
			ALUOP = 5;//sub
		}	
		else if(func == 0x24){//AND
			ALUOP = 0;//and

		}	
		else if(func == 0x27){//NOR
			ALUOP = 2; //nor
		}	
		else if(func == 0x25){//OR
			ALUOP = 1; //or 
		}	
		else if(func == 0x2b){//SLTU
			ALUOP = 8;
		}
		else if(func == 0x0){//SLL
			ALUOP = 6;
			Shift = TRUE;
		}
		else if(func == 2){//SRL
			ALUOP =7;
			Shift = TRUE;
		}	
		else if(func == 8){//JR
			Jump = TRUE;
			RegWrite = FALSE;

		}

	}
	else{
		ALUSrc = TRUE;
		RegDst = FALSE;
		if(opcode == 9){//ADDIU
			ALUOP = 4;//add
		}
		else if(opcode == 0xc){//ANDI
			ALUOP = 0;//and
		}
		else if(opcode == 0xd){//ORI
			ALUOP = 1;//or
		}

		else if(opcode == 0xf){//LUI
			ALUOP = 1;//or;
			Lui = TRUE;
		}
		else if (opcode == 0xb){
			ALUOP = 8;//slt
		}
		else if(opcode ==0x23 || opcode==0x20 || opcode == 0x2b || opcode == 0x28){
			ALUOP = 4;//add
			if(opcode  ==0x23 || opcode ==0x20){// lw lb
				MemtoReg = TRUE;
				if(opcode == 0x23){
					MemRead =1; //lw
				}
				else{
					MemRead =2; //lb
				}
			}
			else {//sw sb
				RegWrite = FALSE;
				if(opcode == 0x2b){
					MemWrite = 1;//sw
				}
				else{
					MemWrite = 2;//sb
				}
			}
		}
		else if(opcode == 4 || opcode == 5){//BEQ, BNE
			Branch = TRUE;
			RegWrite = FALSE;
			ALUSrc = FALSE;
			ALUOP = 5; //sub
			if(opcode == 4){ //BEQ
				PCSrc = FALSE;
			}
			else{ //BNE
				PCSrc = TRUE;
			}
		}
		else if(opcode == 2 || opcode ==3){//Jump
			Jump = TRUE;
			if(opcode ==2){//j
				RegWrite = FALSE;
			}
			else{
				RegWrite = TRUE;

			}

		}	
	}
	//printf("Mem read = %d, Mem write = %d\n", MemRead, MemWrite);
}

void IFetch(int pc){
	//printf("ifetch\n");
	strncpy(op, instm[pc].inst, 6);
	strncpy(rs, instm[pc].inst+6,5 );
	strncpy(rt, instm[pc].inst+11, 5);
	strncpy(rd, instm[pc].inst+16, 5);
	strncpy(sh, instm[pc].inst+21, 5);
	strncpy(fc, instm[pc].inst+26, 6);
	strncpy(im, instm[pc].inst+16, 16);
	strncpy(tg, instm[pc].inst+6, 26);
	//printf("op =%s, rs =%s, rt =%s, rd =%s, sh =%s, fc =%s \n", op, rs, rt, rd, sh, fc); 	
	//printf("op =%s, rs =%s, rt =%s, im =%s\n", op, rs, rt, im);
}

void Dec(int pc){
	//control unit
	//printf("dec\n");
	controlUnit();
	
	read_addr1 = strtol(rs, NULL, 2);
	read_addr2 = strtol(rt, NULL, 2);
	//jump task
	if(Jump == TRUE){
		int target;
		target = strtol(tg, NULL, 2);
		target <<= 2;
		target = ((PC>>28)<<28)|target;
		if(RegWrite == TRUE){ //jal
			//printf("regwrite %d\n", RegWrite);
			reg[31] = PC;// ra
			PC = target;
		}
		else if(RegWrite == FALSE){//jr
			if(RegDst == TRUE){
		       		PC = reg[31];//ra
			}
			else{//J
				PC = target;
			}
		}
		//printf("jump, PC = %x\n", PC);
	}
	if(Branch ==TRUE){
		branch_PC = strtol(im, NULL, 2);
		branch_PC = PC +4*branch_PC;
		//printf("branch = %x\n", branch_PC);		
	}


	//chose write data rt, rd
	if(RegWrite == TRUE){
		if(RegDst == TRUE){
			write_addr = strtol(rd, NULL, 2);
		}
		else{
			write_addr = strtol(rt, NULL, 2);
		}
	}
	//chose read data 1, 2
	read_data1 = reg[read_addr1];
	if(Shift == TRUE){
		read_data1 = strtol(sh, NULL, 2);
	}	
	read_data2 = reg[read_addr2];	
	
	
	//printf("read addr1 : %d, read addr2 : %d, write addr : %d\n", read_addr1, read_addr2, write_addr);
	//printf("read data1 : %d, read data2 : %d\n", read_data1, read_data2);
	

}

void Exec(int i){
	//printf("exec\n");
	write_data3 = read_data2; //in case of MemWrite
	if(ALUSrc== TRUE){//imm is exist
		if( ALUOP ==6 ||ALUOP ==7){ //case of shift
			read_data1 = signExtend();
		}
		else{
			
			read_data2 = signExtend();
		}	
	}
	//printf("last ead data1 : %d, read data2 : %d ", read_data1, read_data2);

	if(ALUOP == 0){ //and 0000
		alu_result = read_data1&read_data2;		
	}
	else if(ALUOP == 1){//or 0001
		alu_result = read_data1|read_data2;		
		if(Lui == TRUE){
		alu_result = read_data2<<16;
		}
	}
	else if(ALUOP == 2){//nor 0010
		alu_result = ~(read_data1|read_data2);		

	}
	else if(ALUOP == 4){//add 0100 
		alu_result = read_data1+read_data2;		

	}
	else if(ALUOP == 5){//sub 0101
		alu_result = read_data1-read_data2;		
	}
	else if(ALUOP == 6){//left shift
		alu_result =  read_data2<<read_data1;
	}
	else if(ALUOP == 7){//right shift
		alu_result = read_data2>>read_data1;
	}
	else if(ALUOP== 8){//set on less than 1000
		alu_result = read_data1<read_data2;		
	}
	//branch
	if(Branch == TRUE){
		//printf("PCSrc %d, alu %d\n", PCSrc, alu_result );
		PCSrc = PCSrc&&alu_result;
		if (PCSrc == TRUE){
			PC= branch_PC;
		}	
	}
	//printf("PC = %x\n",PC);
	//printf("read_data1 : %d, read_data2 :  %d, ALUOP : %d, alu_result : %d\n", read_data1, read_data2, ALUOP, alu_result);

}
void Mem(int i){
	//printf("mem\n");
	int res;
	read_addr3 = (alu_result>>2)<<2;
	res = alu_result - read_addr3;
	// find addr
	if(MemRead || MemWrite){
		for(int i = 0; i < data_size; i++){
			if( datam[i].loc == read_addr3){
				//printf(" data is accessed to %x read addr %x, memread = %d, memwrite = %d, res = %d\n", read_addr3, alu_result,  MemRead, MemWrite, res);
				read_data3 = strtol(datam[i].data, NULL, 2); //LW
				if(MemRead != FALSE){
					//printf("data read\n");
					//printf("byte read : %x\n", read_data3);
					if(MemRead == 2){//LB
						if(res ==0){
							read_data3 &= 0xff000000;
							read_data3 >>=(8*(3-res));
						}
						else if(res ==1){
							read_data3 &= 0x00ff0000;
							read_data3 >>=(8*(3-res));
						}
						else if(res ==2){
							read_data3 &= 0x0000ff00;
							read_data3 >>=(8*(3-res));
						}
						else if(res ==3){
							read_data3 &= 0x000000ff;
							read_data3 >>=(8*(3-res));
						}
						//printf("byte read : %x\n", read_data3);
					}
				}
				else if(MemWrite != FALSE){
					//printf("data write, write data : %d\n", write_data3);
					if(MemWrite == 1){
						strcpy(datam[i].data, toBi(write_data3, 32));
					}
					else if(MemWrite ==2){
						if(res ==0){
							read_data3^=(read_data3&0xff000000);
						}
						else if(res ==1){
							read_data3^=(read_data3&0x00ff0000);
						}
						else if(res ==2){
							read_data3^=(read_data3&0x0000ff00);
						}
						else if(res ==3){
							read_data3^=(read_data3&0x000000ff);
						}
						write_data3 <<=(8*(3-res));
						write_data3 |=read_data3;
						strcpy(datam[i].data, toBi(write_data3, 32));
						
					
					}		
					//printf("save data : %s\n", toBi(write_data3, 8));
				}
			}

		}
	}

}
void WB(int i){
	//printf("wb\n");
	if(MemtoReg == TRUE){
		write_data = read_data3;
	}
	else{
		write_data = alu_result;
	}

	if(RegWrite ==TRUE){
		reg[write_addr] = write_data;	
	}
}

void imTest(int m1, int m2){
	int i = (m1 -TEXTADDR)/4;
	int j = (m2 -TEXTADDR)/4;

	for (i ; i <= j; i++){
		printf("0x%X: 0x%X\n", TEXTADDR+4*i, (int)strtol(instm[i].inst, NULL, 2));
	}
}

void dmTest(int m1, int m2){
	int i = (m1-DATAADDR);
	int j = (m2-DATAADDR);

		for (i ; i<=j; i++){
			printf("0x%X 0x%X\n", DATAADDR+4*i, (int)strtol(datam[i].data,NULL,2));
		}
}
void regTest(int pc){
	printf("Current regsiter values:\n%s\nPC: 0x%x\n", "--------------------", TEXTADDR+4*pc);
	printf("Registers:\n");
	for(int i = 0; i < 32; i++){
		printf("R%d: 0x%x\n", i , reg[i]);
	}
	printf("\n");
}

int main(int argc, char*argv[]){
	char *m;
	int ma[2];
	int mc = FALSE;
	int dc = FALSE;
	int n =0;
	int count = 0;	
	char fname[20];

	for ( int i= 1 ; i< argc; i++){
		if (strcmp(argv[i], "-m")==0){
			mc=TRUE;
			m = strtok(argv[i+1], ":");
			ma[0] = strtol(m, NULL, 16);
			m = strtok(NULL, ":");
			ma[1] = strtol(m, NULL, 16);
		}
		else if(strcmp(argv[i],"-d")==0){
			dc = TRUE;
		}	
		else if(strcmp(argv[i],"-n")==0){
			n = atoi(argv[i+1]);
		}
		strcpy(fname, argv[argc-1]);
	}

	openFile(fname);
	preprocess(file);
	PC = TEXTADDR;
	pc = 0;
	while(pc < inst_size){
		if(dc == TRUE){
			regTest(pc);
			if(mc == TRUE){
				printf("Memory content [0x%x..0x%x]:\n---------------------\n", ma[0], ma[1]);
				if(ma[0]>= DATAADDR){
					dmTest(ma[0], ma[1]);
				}
				else if(ma[0]>= TEXTADDR){
					imTest(ma[0], ma[1]);
				}
				printf("\n");
			}		
		}
		if(count ==n ) {
			break;
		}
		pc = (PC-TEXTADDR)/4;
		pc++;
		PC = TEXTADDR+pc*4;

		IFetch(pc-1);
		Dec(pc-1);
		Exec(pc-1);
		Mem(pc-1);
		WB(pc-1);
		count++;
	}

	if(dc == FALSE){
		regTest(pc);
		if(mc== TRUE){
			printf("Memory content [0x%x..0x%x]:\n---------------------\n", ma[0], ma[1]);
			if(ma[0]>= DATAADDR){
				dmTest(ma[0], ma[1]);
			}
			else if(ma[0]>= TEXTADDR){
				imTest(ma[0], ma[1]);
			}
			printf("Wn");
		}
		
	}

	//`printf("m1 : 0x%x, m2 : 0x%x, d : %d, n : %d\n", ma[0], ma[1], dc, n);
}

