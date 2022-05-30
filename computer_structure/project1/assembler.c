#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define TRUE 1
#define FALSE 0
#define TextAddr 0x400000
#define DataAddr 0x10000000

struct OPTAB {
	char name[8];
	int code;
	char format;
}optab[] = {
{"ADDIU",0x09,'I'}, {"ADDU", 0x21, 'R'}, {"AND", 0x24,'R'}, {"ANDI", 0xc, 'I'},
{"BEQ", 0x04, 'I'}, {"BNE", 0x05, 'I'}, {"J", 0x02, 'J'}, {"JAL", 0x03, 'J'},
{"JR", 0x08, 'R'}, {"LUI", 0xf, 'I'}, {"LW",0x23, 'I'}, {"LB", 0x20, 'I'},
{"NOR", 0x27,'R'}, {"OR", 0x25,'R'}, {"ORI", 0xd, 'I'}, {"SLTIU", 0xb,'I'}, 
{"STLU", 0x2b,'R'}, {"SLL", 0x00,'R'},{"SRL", 0x02,'R'}, {"SW", 0x2b,'I'}, 
{"SB", 0x28, 'I'}, {"SUBU", 0x23, 'R'},{"LA" , -1,'U'} 	
};

struct LABTAB {
	char name[10];
	int loc;
}labtab[200];
int labtab_size = 0;

struct INSTTAB{
	char label[11];
	int loc;
	int opcode;
	int type;

	int rs;
	int rt;
	int rd;
	int shamt;
	int funct;
	int imm;
	int offset;
	char target[11];
}insttab[1024];
int insttab_size = 0;

struct DATTAB{
	char label[11];
	int loc;
	char direc[11];
	int idata;
	char sdata[11];
}dattab[1024];
int dattab_size = 0;


FILE *file;
int code_len;

char label[10],direc[10], opcode[10], r1[10], r2[10], r3[10], r[10];
char rs[10], rt[10], rd[10], target[10];
int imm, offset, shamt, type;
int iopcode, ilabel, iaddr;
int loc;

void openFile(char *s){
	if((file = fopen(s, "r")) == NULL ){
		printf("no file");
	}
}

void makeFile(char *s){
	char name[20];
	if(strlen(s) >19){
		printf("file name is too long\n");
		exit(1);
	}
	
	if(file ==NULL){
		printf("can't make file\n");
		exit(1);
	}
	strncpy(name, s, strlen(s)-2);
	strcat(name, ".o");
	file = fopen(name, "w");
	
}

// type changer, binary hex deci changer
void strupr(char str[]){
	int i=0;
	while(str[i]){
		char *intToBinary(int i) {
			  static char s[32 + 1] = { '0', };
			    int count = 32;
			   
			      do { s[--count] = '0' + (char) (i & 1);
				             i = i >> 1;
					       } while (count);
			      
			        return s;
		}if(str[i]>='a' && str[i]<='z'){
			str[i]=str[i]-32;
		}
		i++;			                 
	}
}

char *toBi(int i, int size) {
	char *s = malloc(size*sizeof(char));	
	    int count = size;
	      do { s[--count] = '0' + (char) (i & 1);
		             i = i >> 1;
			       } while (count);
	        return s;
}

char *biToin( char* binary){
	int num = 0, i;
	char s[11];
	for(i =0; binary[i]; i++){
		num = (num <<1) + binary[i] -'0';
	}

	
	printf("0x%x\n", num);
	sprintf(s, "0x%x\n", num);
	fwrite(s, strlen(s), 1, file);
}
// searching function
int searchOp(char *opcode){
	int i;
	for (i = 0; i <sizeof(optab)/sizeof(optab[0]); i++){
		strupr(opcode);
		if(strcmp(opcode, optab[i].name) ==0){
			iopcode = i;
			return TRUE;
		}
	}
	return FALSE;
}

int searchLab(char *label){
	int i;
	
	if(labtab_size ==0){
		return FALSE;
	}
	else{
		for( i =0; i<labtab_size; i++){
			if(strcmp(label, labtab[i].name) ==0){
				ilabel = i;
				return TRUE;
			}
		}
	}
}
int searchAddr(int addr){
	int i;
	if(addr >0x10000000){
		for( i =0; i<dattab_size; i++){
			if(addr== dattab[i].loc){
				iaddr = i;
				return TRUE;
			}
		}

	}
	else{
		for( i =0; i<insttab_size; i++){
			if(addr== insttab[i].loc){
				iaddr = i;
				return TRUE;
			}
		}

	}
}
// code analysis
void getCode(char* line){
	char *tok;
	char *ptr1, *ptr2, *ptr3;
	int a, check = FALSE, count = 0;
	strcpy(opcode, ""); strcpy(label, ""); strcpy(direc, ""); strcpy(target, "");
	strcpy(r1, ""); strcpy(r2, ""); strcpy(r3, "");
	strcpy(rs, ""); strcpy(rt, ""); strcpy(rd, "");
	imm = 0; shamt =0; offset =0; //현재 상태 초기화 
	for(tok = strtok(line, " \t\n(),"); tok != NULL;){
		a = isdigit(tok[0]);
		if(!a && !check){ // atoi는 문자일 경우 0 반환
			ptr1 = strchr(tok, ':');
			ptr2 = strchr(tok, '.');
			if(ptr1 !=NULL){
				strcpy(label, tok);
				tok = strtok(NULL," \t\n,");
				
			}
			else if(ptr2 !=NULL){
				ptr3 = strchr(tok, 'w');
				if(ptr3 !=NULL){
				}
				strcpy(direc, tok);
				tok = strtok(NULL," \t\n,");
			}
			else{
				strcpy(opcode, tok);
				tok = strtok(NULL, " \t\n,");
				check = TRUE;
			}
		}
		else if(!a){
			if(!count){
				strcpy(r1, tok);
				tok = strtok(NULL, " \t\n(),");
				count ++;
			}
			else if(count ==1){
				strcpy(r2, tok);
				tok = strtok(NULL, " \t\n(),");
				count ++;
			}
			else{
				strcpy(r3, tok);
				tok = strtok(NULL, " \t\n(),");
			}
		}
		else{
			if(strchr(tok,'x') !=NULL){
				strcpy(r3, tok);
				imm = strtol(tok, NULL, 16);
				tok = strtok(NULL, " \t\n");
					
			}

			else{
				imm = atoi(tok);	
				tok = strtok(NULL, " \t\n");
			}
		}
	}
}
//store in memory
void storeCode(){
	int a = atoi(&rs[1]);
	int b = atoi(&rt[1]);
	int c = atoi(&rd[1]);
	
	
	strcpy(insttab[insttab_size].label,label);
	strcpy(insttab[insttab_size].target,target);
	insttab[insttab_size].opcode = optab[iopcode].code;
	insttab[insttab_size].rs = a;
	insttab[insttab_size].rt = b;
	insttab[insttab_size].rd = c;
	insttab[insttab_size].shamt = shamt;
	insttab[insttab_size].offset = offset;
	insttab[insttab_size].imm = imm;
	insttab[insttab_size].loc = TextAddr + insttab_size*4;
	insttab[insttab_size].type = type;
	loc = TextAddr + insttab_size*4;
	if(strlen(label)!=0){
		searchLab(label);
		labtab[ilabel].loc = TextAddr + insttab_size*4;
	}
	insttab_size++;


}

void storeData(){

	strcpy(dattab[dattab_size].label,label);
	strcpy(dattab[dattab_size].direc,direc);
	strcpy(dattab[dattab_size].sdata,r3);
	dattab[dattab_size].idata = imm;
	dattab[dattab_size].loc = DataAddr + dattab_size*4;
	if(strlen(label)!=0){
		searchLab(label);
		labtab[ilabel].loc = DataAddr + dattab_size*4;
	}
	dattab_size++;
}

// file reading
void checkFile(FILE *file){
	char line[100];
	while(fgets(line, sizeof(line), file) != NULL){
		getCode(line);
		//printf("label : %s direc: %s opcode: %s r1: %s r2: %s r3: %s imm : %d\n", label, direc, opcode, r1, r2, r3, imm);
		if(strlen(label) != 0){
			strtok(label, ":");
			strcpy(labtab[labtab_size].name,label);
			labtab_size++;
		}
		if(searchOp(opcode)){
			if(optab[iopcode].format =='R'){
				if(!strcmp(optab[iopcode].name ,"ADDU")||//6
				!strcmp(optab[iopcode].name ,"AND")||
				!strcmp(optab[iopcode].name ,"NOR")||
				!strcmp(optab[iopcode].name ,"OR")||
				!strcmp(optab[iopcode].name ,"SLTU")||
				!strcmp(optab[iopcode].name ,"SUBU")){
					strcpy(rd, r1);
					strcpy(rs, r2);
					strcpy(rt, r3);
					type = 1; //0 rs rt rd 0 ip
					storeCode();			
				}
				else if(!strcmp(optab[iopcode].name ,"SLL")||
				!strcmp(optab[iopcode].name ,"SRL")){//2
					strcpy(rd, r1);
					strcpy(rt, r2);
					shamt = imm;
					type =2;//0 0 rt rd sh ip
					storeCode();
				}
				else{//jr
					strcpy(rs, r1);
					type =3;//0 rs 0 8
					storeCode();
				}


			}//9
			else if(optab[iopcode].format  =='I'){
				if(!strcmp(optab[iopcode].name ,"ADDIU")|| //4
				!strcmp(optab[iopcode].name ,"ANDI")||
				!strcmp(optab[iopcode].name ,"ORI")||
				!strcmp(optab[iopcode].name ,"SLTIU")){
					strcpy(rt, r1);
					strcpy(rs, r2);
					if((imm ==0) && (strlen(r3)!=0)){ 
						imm =atoi(r3);
					}
					
					type =4;//op rs rt imm
					storeCode();
					
				}
				else if(!strcmp(optab[iopcode].name ,"BEQ")||//6
				!strcmp(optab[iopcode].name ,"BNE")){
					strcpy(rs, r1);
					strcpy(rt, r2);
					strcpy(target, r3);
					type =5; //op rs rt offset
					storeCode();
					
				
				}
				else if(!strcmp(optab[iopcode].name ,"LW")||
				!strcmp(optab[iopcode].name ,"LB")||
				!strcmp(optab[iopcode].name ,"SW")||
				!strcmp(optab[iopcode].name ,"SB")){
					strcpy(rt, r1);
					strcpy(rs, r2);
					type =6;//op rs rt offset
					storeCode();
					
				}	
				else{//LUi	
					strcpy(rs, "0");
					strcpy(rt, r1);
					type =4; // op 0 rt imm
					storeCode();

					
				}			
			}//11
			
			else if(optab[iopcode].format == 'J'){
				strcpy(target , r1);
				type = 7;//op target
				storeCode();


			}//2
			else{//LA*
				char high[5] ="";
				char low[5] = "";
				char s[10];
				strcpy(r, "");
				strcpy(r, r1);//addr
				searchLab(r2);
				searchAddr(labtab[ilabel].loc);
				type = 4;
				sprintf(s, "%X\n",labtab[ilabel].loc);
				strncpy(high, s ,4);
				strncpy(low, s + 4,4);
				if(strtol(high, NULL, 16)){//lui
					searchOp("LUI");
					strcpy(rs, "0");
					strcpy(rt, r);
					strcat(r3, "0x");
					strcat(r3,high);
					imm = strtol(r3, NULL, 16);
					storeCode();
					
				}	
				if(strtol(low, NULL, 16)){// ori
					searchOp("ORI");
					strcpy(rt, r);
					strcpy(rs, r);
					strcpy(r3,"0x");
					strcat(r3,low);
					imm = strtol(r3, NULL, 16);
					storeCode();
				}
				
				
			}//1

		}
		else{
			if (strcmp(direc, ".word")==0){
		 		storeData();
			
			}
			else if(strlen(label)!=0){
				searchLab(label);
				labtab[ilabel].loc = loc+4;
				
			}
		}

	}
}
// output
void process(){
	int i;
	char s[11];	
	FILE *fp1;


	sprintf(s, "0x%x\n", 4*insttab_size);
  	printf("%s",s);
	fwrite(s, strlen(s), 1, file);
	sprintf(s, "0x%x\n", 4*dattab_size); // 십진수 숫자를, 16진수 문자열로 변환
  	printf("%s",s);
	fwrite(s, strlen(s), 1, file);
	

	for(i = 0; i<insttab_size; i++){
		if(insttab[i].type ==1){ //0 rs rt rd 0 ip
			char mcode[32] = "000000";
			strcat(mcode, toBi(insttab[i].rs,5));
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].rd,5));
			strcat(mcode, "00000");
			strcat(mcode, toBi(insttab[i].opcode,6));
			//printf("%s \n",mcode);
			biToin(mcode);	

		}	
		else if(insttab[i].type ==2){//0 0 rt rd sh ip
			char mcode[32] = "00000000000";
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].rd,5));
			strcat(mcode, toBi(insttab[i].shamt,5));
			strcat(mcode, toBi(insttab[i].opcode,6));
			//printf("%s \n",mcode);
			biToin(mcode);	

	
		}
		else if(insttab[i].type ==3){
			char mcode[32] = "000000";
			strcat(mcode, toBi(insttab[i].rs,5));
			strcat(mcode, "000000000000000");
			strcat(mcode, toBi(insttab[i].opcode,6));
			//printf("%s \n",mcode);
			biToin(mcode);	


		}
		else if(insttab[i].type ==4){//op rs rt imm
			char mcode[32] = "";
			strcat(mcode, toBi(insttab[i].opcode,6));
			strcat(mcode, toBi(insttab[i].rs,5));
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].imm,16));
			//printf("%s \n",mcode);
			biToin(mcode);	
		}
		else if(insttab[i].type ==5){ //op rs rt offset
			char mcode[32] = "";
			strcat(mcode, toBi(insttab[i].opcode,6));
			strcat(mcode, toBi(insttab[i].rs,5));
			strcat(mcode, toBi(insttab[i].rt,5));
			searchLab(insttab[i].target);
			strcat(mcode, toBi( (labtab[ilabel].loc -insttab[i].loc)/4 - 1 , 16));
			//printf("%X %X\n",labtab[ilabel].loc, insttab[i].loc);
			//printf("%s \n",mcode);
			biToin(mcode);	

	
		}
		else if(insttab[i].type ==6){
			char mcode[32] = "";
			strcat(mcode, toBi(insttab[i].opcode,6));
			strcat(mcode, toBi(insttab[i].rs,5));
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].imm,16));
			//printf("%s \n",mcode);
			biToin(mcode);	


		}
		else if(insttab[i].type ==7){////op target
			char mcode[32] = "";
			searchLab(insttab[i].target);
			strcat(mcode, toBi(insttab[i].opcode,6));
			strcat(mcode, toBi(labtab[ilabel].loc/4,26));
			//printf("%d\n", labtab[ilabel].loc);
			//printf("%s \n",mcode);
			biToin(mcode);	


		}
		else if(insttab[i].type ==7){
			char mcode[32] = "00000000000";
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].rd,5));
			strcat(mcode, toBi(insttab[i].shamt,5));
			strcat(mcode, toBi(insttab[i].opcode,6));
			//printf("%s \n",mcode);
			biToin(mcode);	


		}
			else if(insttab[i].type ==8){
			char mcode[32] = "00000000000";
			strcat(mcode, toBi(insttab[i].rt,5));
			strcat(mcode, toBi(insttab[i].rd,5));
			strcat(mcode, toBi(insttab[i].shamt,5));
			strcat(mcode, toBi(insttab[i].opcode,6));
			//printf("%s \n",mcode);
			biToin(mcode);	


		}
	
	}

	for(i =0; i<dattab_size; i++){
		sprintf(s, "0x%x\n", dattab[i].idata);
		printf("%s",s);
		fwrite(s, strlen(s), 1, file);
	
	}

}
// for test


void test(FILE *file){
	const int max = 1024;
	char line[max];
	char *pline;
	while (!feof(file)){
		pline = fgets(line, max, file);
		printf("%s", pline);
	}
	printf("\n");
}

void labtabTest(){
	int i;
	for (i =0; i<labtab_size; i++){
		printf("%s %d\n", labtab[i].name, labtab[i].loc);
	}
}

void insttabTest(){
	int i;
	for (i =0; i<insttab_size; i++){
		printf("addr : %d opcode : %d type : %d rs: %d rt: %d rd: %d shamt %d offset : %d imm : %d target : %s label : %s\n",insttab[i].loc, insttab[i].opcode, insttab[i].type, insttab[i].rs, insttab[i].rt, insttab[i].rd, insttab[i].shamt, insttab[i].offset, insttab[i].imm, insttab[i].target, insttab[i].label  );
	}
	printf("total instruct : %d\n", insttab_size);

}void dattabTest(){
	int i;
	for (i =0; i<dattab_size; i++){
		printf("addr : %d label : %s direc: %s data : %d\n", dattab[i].loc, dattab[i].label, dattab[i].direc, dattab[i].idata );
	
	}
	printf("total data : %d\n",  dattab_size);

}



int main(int argc, char*argv[]){
	if(argc >2){
		printf("wrong file name\n");
		exit(1);
	}
	else if(argc == 1){
		printf("no file\n");
		exit(1);
	}
	else{
	openFile(argv[1]);
	checkFile(file);
	//test(file);
	//labtabTest();
	//insttabTest();
	//dattabTest();
	makeFile(argv[1]);	
	process();
	fclose(file);
	}
}
