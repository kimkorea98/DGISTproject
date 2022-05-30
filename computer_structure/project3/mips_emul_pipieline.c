#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define TRUE 1
#define FALSE 0
#define TEXTADDR 0x400000
#define DATAADDR 0x10000000
//file information

FILE* fp;
int tp, m = 0, d = 0, p = 0, n = 0; //option checker 
int inst_size = 0;
int data_size = 0;
//PC
int PC = TEXTADDR;
int pc;
// register 32
int reg[32];
//instruction memory
struct INSTM {
	char inst[33];
	int loc;
}instm[200];
int im_size = 0;
//data memory
struct DATAM {
	char data[33];
	int loc;
}datam[200];
int dm_size = 0;
//alu_forward type

struct IF_ID_stateReg {
	int pPC; //pipleline PC
	int PC; //pc + 4
	//inst info
	char op[7] ; //inst[31-26]
	char rs[6] ;//inst[25-21]
	char rt[6] ; //inst[20-16]
	char rd[6] ; //inst[15-11]
	char sh[6] ; //inst[11-6]
	char fc[7] ; //inst[5-0]
	char im[17] ; //inst[15-0]
	char tg[27] ; //inst[25-0]

	int stall; //for stall
}IF_IDi, IF_IDo = {};
struct ID_EX_stateReg {
	int pPC; //pipleline PC
	int PC; //pc + 4
	int jump;
	int branch;
	//data
	int read_data1 ;
	int read_data2 ;
	int write_addr1;
	int write_addr2;
	int rs;
	int rt;
	int imm ;
	//control info
	int ALUOP;//func code
	int Jump; //j, jr, jal uncondition branch
	int Branch; // condition branch
	int PCSrc; //branch condition
	int MemRead; //read Mem, lw, lb
	int MemtoReg; //send Mem data, lw, lb
	int MemWrite; //write mem, sw, sb
	int ALUSrc; // choice alu source between reg_data with imm
	int RegWrite; //write reg
	int RegDst; //choise write regi rt, rd
	int Shift; 
	int Lui;

	int stall; //for stall
}ID_EXi, ID_EXo = {};
struct EX_MEM_stateReg{
	int pPC; //pipleline PC
	int PC; //pc + 4
	int branch;
	//data
	int read_data2;
	int write_addr;
	int ALU_value;
	int ALU_zero;
	//control info
	int Branch;
	int PCSrc; //branch condition
	int RegWrite;
	int MemRead;
	int MemtoReg;
	int MemWrite;
}EX_MEMi, EX_MEMo = {};
struct MEM_WB_stateReg {
	int pPC; //pipleline PC
	int PC; //pc + 4
	int read_data;
	int write_addr;
	int ALU_value;
	//control info 
	int MemtoReg;
	int RegWrite;
}MEM_WBi, MEM_WBo = {};

typedef struct ALU_VALUES {
	int v1;
	int v2;
}ALU_VAL;
struct HAZARD_control {
	int L; //ld hazard
	int J; //j hazard
	int B; // ATP branch
	int Bf1; // ANTP fail
	int Bf2; // ATP fail

}hazard_ctrl;

int last_pPC = 0;
int last_addr = 0;
int last_data = 0;
int last_RegWrite = 0;

char* toBi(int i, int size) {
	char* s = malloc(size * sizeof(char));
	int count = size;
	do {
		s[--count] = '0' + (char)(i & 1);
		i = i >> 1;
	} while (count);
	return s;
}
int signExtend() {
	int a;
	int b = strtol(IF_IDo.im, NULL, 2);
	if (IF_IDo.im[0] == '1') {
		a = 0xffff0000 | b;
	}
	else {
		a = b;
	}
	return a;
}
void preprocess() {
	char line[33];
	int count = 0;
	while (fgets(line, sizeof(line), fp) != NULL) {
		//printf("%s" , line); //print object file
		if (count == 0) {
			inst_size = (int)strtol(line, NULL, 16)/4; //real byte
		}
		else if (count == 1) {
			data_size = (int)strtol(line, NULL, 16)/4; //real byte
		}
		else if (count < inst_size + 2) {
			strcpy(instm[im_size].inst, toBi((int)strtol(line, NULL, 16), 32));
			instm[im_size].loc = TEXTADDR + 4 * im_size;
			im_size++;
		}
		else {
			strcpy(datam[dm_size].data, toBi((int)strtol(line, NULL, 16), 32));
			datam[dm_size].loc = DATAADDR + 4 * dm_size;
			dm_size++;
		}
		count++;
	}

}
//dec
void controlUnit() {
	int opcode = strtol(IF_IDo.op, NULL, 2);
	int func = strtol(IF_IDo.fc, NULL, 2);
	//printf("opcode : %d, 0x%x, func: %d\n", opcode, opcode, func);
	ID_EXi.Jump = FALSE;
	ID_EXi.Branch = FALSE;
	ID_EXi.MemRead = FALSE;
	ID_EXi.MemtoReg = FALSE;
	ID_EXi.MemWrite = FALSE;
	ID_EXi.RegWrite = TRUE;
	ID_EXi.Shift = FALSE;
	ID_EXi.Lui = FALSE;

	if (opcode == 0) { // r type;
		ID_EXi.ALUSrc = FALSE;
		ID_EXi.RegDst = TRUE; //use rd
		if (func == 0x21) { //ADDU
			ID_EXi.ALUOP = 4; // add
		}
		else if (func == 0x23) {//SUBU
			ID_EXi.ALUOP = 5;//sub
		}
		else if (func == 0x24) {//AND
			ID_EXi.ALUOP = 0;//and

		}
		else if (func == 0x27) {//NOR
			ID_EXi.ALUOP = 2; //nor
		}
		else if (func == 0x25) {//OR
			ID_EXi.ALUOP = 1; //or 
		}
		else if (func == 0x2b) {//SLTU
			ID_EXi.ALUOP = 8;
		}
		else if (func == 0x0) {//SLL
			ID_EXi.ALUOP = 6;
			ID_EXi.Shift = TRUE;
		}
		else if (func == 2) {//SRL
			ID_EXi.ALUOP = 7;
			ID_EXi.Shift = TRUE;
		}
		else if (func == 8) {//JR
			ID_EXi.Jump = TRUE;
			ID_EXi.RegWrite = FALSE;
		}
	}
	else {
		ID_EXi.ALUSrc = TRUE;
		ID_EXi.RegDst = FALSE;
		if (opcode == 9) {//ADDIU
			ID_EXi.ALUOP = 4;//add
		}
		else if (opcode == 0xc) {//ANDI
			ID_EXi.ALUOP = 0;//and
		}
		else if (opcode == 0xd) {//ORI
			ID_EXi.ALUOP = 1;//or
		}

		else if (opcode == 0xf) {//LUI
			ID_EXi.ALUOP = 1;//or;
			ID_EXi.Lui = TRUE;
		}
		else if (opcode == 0xb) {
			ID_EXi.ALUOP = 8;//slt
		}
		else if (opcode == 0x23 || opcode == 0x20 || opcode == 0x2b || opcode == 0x28) {
			ID_EXi.ALUOP = 4;//add
			if (opcode == 0x23 || opcode == 0x20) {// lw lb
				ID_EXi.MemtoReg = TRUE;
				if (opcode == 0x23) {
					ID_EXi.MemRead = 1; //lw
				}
				else {
					ID_EXi.MemRead = 2; //lb
				}
			}
			else {//sw sb
				ID_EXi.RegWrite = FALSE;
				if (opcode == 0x2b) {
					ID_EXi.MemWrite = 1;//sw
				}
				else {
					ID_EXi.MemWrite = 2;//sb
				}
			}
		}
		else if (opcode == 4 || opcode == 5) {//BEQ, BNE
			ID_EXi.Branch = TRUE;
			ID_EXi.RegWrite = FALSE;
			ID_EXi.ALUSrc = FALSE;
			ID_EXi.ALUOP = 5; //sub
			if (opcode == 4) { //BEQ
				ID_EXi.PCSrc = FALSE;
			}
			else { //BNE
				ID_EXi.PCSrc = TRUE;
			}
		}
		else if (opcode == 2 || opcode == 3) {//Jump
			ID_EXi.Jump = TRUE;
			if (opcode == 2) {//j
				ID_EXi.RegWrite = FALSE;
			}
			else {
				ID_EXi.RegWrite = TRUE;

			}

		}
	}
	//printf("Mem read = %d, Mem write = %d\n", MemRead, MemWrite);
}
void hazardUnit_ID() {
	//printf("---------------------hazard---------------------\n");
	//printf("ID_EXi.Branch %d\n", ID_EXi.Branch);
	//lw, at decoding
	if (ID_EXo.MemRead && ((ID_EXo.rt == (int)strtol(IF_IDo.rs, NULL, 2)) || (ID_EXo.rt == (int)strtol(IF_IDo.rt, NULL, 2)))) {
		//stall the pipeline
		hazard_ctrl.L = TRUE;
		//printf("load hazard occur\n");
	}
	else if (ID_EXi.Jump) {
		hazard_ctrl.J = TRUE;
		//printf("jump hazard occur\n");
	}
	// branch 
	else if (ID_EXi.Branch) {
		//at decoding
		//printf("---------------------branch---------------------tp : %d\n", tp);
		if (tp) {//atp
			hazard_ctrl.B = TRUE;
			//printf("branch atp hazard occur at decode\n");
		}
	}
}
void hazardUnit_MEM() {
	//printf("---------------------hazard---------------------\n");
	if (EX_MEMo.Branch) {
		//at exe
		//printf("check pc  0x%x, %d\n", PC, pc);
		//printf("pcs %d, alu %d\n", ID_EXo.PCSrc ,  EX_MEMi.ALU_value);

		if ((EX_MEMo.PCSrc && EX_MEMo.ALU_value) || (!EX_MEMo.PCSrc && !EX_MEMo.ALU_value)) {//bne , beq match
			//printf("branch hazard occur antp\n");//antp
			if(!tp) hazard_ctrl.Bf1 = TRUE;
		}
		else if ((!EX_MEMo.PCSrc && EX_MEMo.ALU_value) || (EX_MEMo.PCSrc && !EX_MEMo.ALU_value)) {//bne , beq unmatch
			//printf("branch hazard occur atp\n");//atp
			if(tp) hazard_ctrl.Bf2 = TRUE;
		}
	}
}
//exe
ALU_VAL forwardUnit(ALU_VAL alu_val) {
	//printf("-----------forwardUnit in EXE-----------\n");
	//printf("EX_MEMo.write_addr : %d, MEM_WBo.write_addr : %d last_data : %d\n", EX_MEMo.write_addr, MEM_WBo.write_addr, last_data);
	//printf("ID_EXo.rs : %d, ID_EXo.rt : %d\n", ID_EXo.rs, ID_EXo.rt);
	/*
	if (last_RegWrite && (last_addr != 0)) {
		if (last_addr == ID_EXo.rs) {
			alu_val.v1 = last_data;
			//printf("rs is same with WB\n");
		}
		if (last_addr == ID_EXo.rt) {
			alu_val.v2 = last_data;
			//printf("rt is same with WB\n");
		}
	}
	*/
	if (EX_MEMo.RegWrite && (EX_MEMo.write_addr != 0)) {
		if ((EX_MEMo.write_addr == ID_EXo.rs) && !ID_EXo.Shift) {//shift should be false
			alu_val.v1 = EX_MEMo.ALU_value;
			//printf("rs is same with EXE\n");
		}
		if (EX_MEMo.write_addr == ID_EXo.rt) {
			alu_val.v2 = EX_MEMo.ALU_value;
			//printf("rt is same with EXE\n");
		}
	}
	if (MEM_WBo.RegWrite && (MEM_WBo.write_addr != 0)) {
		if ((EX_MEMo.write_addr != ID_EXo.rs) && (MEM_WBo.write_addr == ID_EXo.rs) && !ID_EXo.Shift) {
			if (MEM_WBo.MemtoReg) {
				alu_val.v1 = MEM_WBo.read_data;
				//printf("rs is same with MEM\n");
			}
			else {
				alu_val.v1 = MEM_WBo.ALU_value;
				//printf("rs is same with MEM\n");
			}
		}
		if ((EX_MEMo.write_addr != ID_EXo.rt) && (MEM_WBo.write_addr == ID_EXo.rt)) {
			if (MEM_WBo.MemtoReg) {
				alu_val.v2 = MEM_WBo.read_data;
				//printf("rt is same with MEM\n");
			}
			else {
				alu_val.v2 = MEM_WBo.ALU_value;
				//printf("rt is same with MEM\n");
			}
		}
	}
	//printf("-----------forwardUnit in end-----------\n");
	return alu_val;
}
void ALU(ALU_VAL alu_val) {
	//printf("---------------ALU---------------\n");

	if (ID_EXo.ALUOP == 0) { //and 0000
		EX_MEMi.ALU_value = alu_val.v1 & alu_val.v2;
	}
	else if (ID_EXo.ALUOP == 1) {//or 0001
		EX_MEMi.ALU_value = alu_val.v1 | alu_val.v2;
		if (ID_EXo.Lui == TRUE) {
			EX_MEMi.ALU_value = alu_val.v2 << 16;
		}
	}
	else if (ID_EXo.ALUOP == 2) {//nor 0010
		EX_MEMi.ALU_value = ~(alu_val.v1 | alu_val.v2);

	}
	else if (ID_EXo.ALUOP == 4) {//add 0100 
		EX_MEMi.ALU_value = alu_val.v1 + alu_val.v2;

	}
	else if (ID_EXo.ALUOP == 5) {//sub 0101
		EX_MEMi.ALU_value = alu_val.v1 - alu_val.v2;
	}
	else if (ID_EXo.ALUOP == 6) {//left shift
		EX_MEMi.ALU_value = alu_val.v2 << alu_val.v1;
	}
	else if (ID_EXo.ALUOP == 7) {//right shift
		EX_MEMi.ALU_value = alu_val.v2 >> alu_val.v1;
	}
	else if (ID_EXo.ALUOP == 8) {//set on less than 1000
		EX_MEMi.ALU_value = alu_val.v1 < alu_val.v2;
	}

	//printf("v1, v2: %x, %x\n", alu_val.v1 ,  alu_val.v2);
	//printf("EX_MEMi.ALU_value : %d\n", EX_MEMi.ALU_value);


}
//datapath
void IFetch() {
	//printf("---------------ifetch---------------\n");
	IF_IDi.pPC = PC; //pipeline PC
	IF_IDi.PC =	 PC + 4; //real PC
	strncpy(IF_IDi.op, instm[pc].inst, 6);
	strncpy(IF_IDi.rs, instm[pc].inst + 6, 5);
	strncpy(IF_IDi.rt, instm[pc].inst + 11, 5);
	strncpy(IF_IDi.rd, instm[pc].inst + 16, 5);
	strncpy(IF_IDi.sh, instm[pc].inst + 21, 5);
	strncpy(IF_IDi.fc, instm[pc].inst + 26, 6);
	strncpy(IF_IDi.im, instm[pc].inst + 16, 16);
	strncpy(IF_IDi.tg, instm[pc].inst + 6, 26);
	if (!strcmp(instm[pc].inst, "")) {
		IF_IDi.pPC = 0;
		IF_IDi.PC = PC;
	}
	//printf("op =%s, rs =%s, rt =%s, rd =%s, sh =%s, fc =%s \n", op, rs, rt, rd, sh, fc); 	
	//printf("op =%s, rs =%s, rt =%s, im =%s\n", op, rs, rt, im);
}
void Dec() {
	//printf("---------------decode---------------\n");
	ID_EXi.pPC = IF_IDo.pPC;
	ID_EXi.PC = IF_IDo.PC;
	ID_EXi.jump = 0;
	ID_EXi.branch = 0;
	ID_EXi.write_addr1 = (int)strtol(IF_IDo.rt, NULL, 2);
	ID_EXi.write_addr2 = (int)strtol(IF_IDo.rd, NULL, 2);
	ID_EXi.rs = (int)strtol(IF_IDo.rs, NULL, 2);
	ID_EXi.rt = (int)strtol(IF_IDo.rt, NULL, 2);
	ID_EXi.imm = signExtend();
	//control unit
	controlUnit();
	//harzard unit
	hazardUnit_ID();

	int read_addr1 = ID_EXi.rs;
	int read_addr2 = ID_EXi.rt;
	//printf("rs %d, rt %d\n", ID_EXi.rs, ID_EXi.rt);
	

	//branch cal
	if (ID_EXi.Branch == TRUE) {
		ID_EXi.branch = ID_EXi.PC +4*(int)strtol(IF_IDo.im, NULL, 2);
	}

	//chose read data 1, 2 , register access
	
	ID_EXi.read_data1 = reg[read_addr1]; //rs
	if (ID_EXi.Shift) {// shift rt, rd shmt, this is rtype so dont use imm
		ID_EXi.read_data1 = strtol(IF_IDo.sh, NULL, 2);// shift than rs
	}
	ID_EXi.read_data2 = reg[read_addr2];//rt
	//jump task
	if (ID_EXi.Jump == TRUE) {
		int jump;
		jump = strtol(IF_IDo.tg, NULL, 2);
		jump <<= 2; //address = 4*target
		jump = ((IF_IDo.PC >> 28) << 28) | jump;
		//printf("-------------jump = %x, PC = %x----------------\n", jump, PC);
		
		if (ID_EXi.RegWrite == TRUE) { //jal
			//printf("regwrite %d\n", RegWrite);
			ID_EXi.jump = jump;
			ID_EXi.read_data2 = IF_IDo.PC;//?? divide 4?
			ID_EXi.write_addr1 = 31;
		}
		else if (ID_EXi.RegWrite == FALSE) {//jr
			if (ID_EXi.RegDst == TRUE) {
				ID_EXi.PC = reg[31];//ra  target?? PC?
				jump = reg[31];
			}
			else {//J
				ID_EXi.jump = jump;
			}
		}
		pc += (jump - PC)/4;//change PC
		PC = TEXTADDR + pc * 4;
		//printf("-------------jump = %x, PC = %x----------------\n", ID_EXi.jump, PC);
	}


	//printf("rdat1 %d, rdat2 %d\n", ID_EXi.read_data1, ID_EXi.read_data2);
	//printf("read addr1 : %d, read addr2 : %d, write addr : %d\n", read_addr1, read_addr2, write_addr);
	//printf("read data1 : %d, read data2 : %d\n", read_data1, read_data2);
}
void Exec() {
	//printf("---------------exec---------------\n");

	EX_MEMi.pPC = ID_EXo.pPC;
	EX_MEMi.PC = ID_EXo.PC;
	EX_MEMi.branch = ID_EXo.branch;
	//control signal
	EX_MEMi.RegWrite = ID_EXo.RegWrite;
	EX_MEMi.Branch = ID_EXo.Branch;
	EX_MEMi.PCSrc = ID_EXo.PCSrc;
	EX_MEMi.MemRead = ID_EXo.MemRead;
	EX_MEMi.MemtoReg = ID_EXo.MemtoReg;
	EX_MEMi.MemWrite = ID_EXo.MemWrite;
	EX_MEMi.read_data2 = ID_EXo.read_data2; //in case of MemWrite
	EX_MEMi.write_addr = 0; // need it?
	//alu val struct
	ALU_VAL alu_val;
	alu_val.v1 = ID_EXo.read_data1;//rs or shamf
	alu_val.v2 = ID_EXo.read_data2;//rt
	//send write addr
	if (ID_EXo.RegWrite) {
		if (ID_EXo.RegDst) {
			//printf("ID_EXo.write_addr2 : %d\n", ID_EXo.write_addr2);
			EX_MEMi.write_addr = ID_EXo.write_addr2; // general
		}
		else {
			//printf("ID_EXo.write_addr1 : %d\n", ID_EXo.write_addr1);
			EX_MEMi.write_addr = ID_EXo.write_addr1; // store
		}
	}
	//forward Unit for alu cal
	//printf("a1 : %d, a2 : %d\n", alu_val.v1, alu_val.v2);
	alu_val = forwardUnit(alu_val);//rs
	EX_MEMi.read_data2 = alu_val.v2;
	//printf("a1 : %d, a2 : %d\n", alu_val.v1, alu_val.v2);
	//imm is exist
	if (ID_EXo.ALUSrc == TRUE) {
		alu_val.v2 = ID_EXo.imm;
	}
	//ALU
	if (ID_EXo.Jump){
		EX_MEMi.ALU_value = ID_EXo.read_data2; //jal address
	}
	else {
		ALU(alu_val);
	}


	//printf("PC = %x\n",PC);
	//printf("read_data1 : %d, read_data2 :  %d, ALUOP : %d, alu_result : %d\n", read_data1, read_data2, ALUOP, alu_result);

}
void Mem() {
	//printf("---------------mem---------------\n");

	MEM_WBi.pPC = EX_MEMo.pPC;
	MEM_WBi.PC = EX_MEMo.PC;
	MEM_WBi.ALU_value = EX_MEMo.ALU_value;
	MEM_WBi.read_data = EX_MEMo.read_data2;
	MEM_WBi.MemtoReg = EX_MEMo.MemtoReg;
	MEM_WBi.RegWrite = EX_MEMo.RegWrite;
	MEM_WBi.write_addr= EX_MEMo.write_addr;
	int temp; // for calculate
	int address = (EX_MEMo.ALU_value >> 2) << 2;
	int res = EX_MEMo.ALU_value - address;
	int i;

	//branch hazard
	hazardUnit_MEM();
	// find addr
	if (EX_MEMo.MemRead || EX_MEMo.MemWrite) {
		for (i = 0; i < data_size; i++) {
			if (datam[i].loc == address) {
				//printf("data access to %d\n", address);
				temp = (int)strtol(datam[i].data, NULL, 2); //LW
				//printf("temp : 0x%x, res : %d, add : 0x%x\n", temp, res, address);
				break;
			}
		}
	}
	if(EX_MEMo.MemRead == 2) {//LB
		if (res == 0) {
			temp &= 0xff000000;
			temp >>= (8 * (3 - res));
		}
		else if (res == 1) {
			temp &= 0x00ff0000;
			temp >>= (8 * (3 - res));
		}
		else if (res == 2) {
			temp &= 0x0000ff00;
			temp >>= (8 * (3 - res));
		}
		else if (res == 3) {
			temp &= 0x000000ff;
			temp >>= (8 * (3 - res));
		}
		//printf("byte read : %x\n", temp3);
	}
	else if(EX_MEMo.MemWrite) {
		//printf("data write, write data : %d\n", write_data3);
		//printf("data write \n");
		if (EX_MEMo.MemWrite == 1) {
			strcpy(datam[i].data, toBi(EX_MEMo.read_data2, 32));
		}
		else if (EX_MEMo.MemWrite == 2) {
			if (res == 0) {
				temp ^= (temp & 0xff000000);
			}
			else if (res == 1) {
				temp ^= (temp & 0x00ff0000);
			}
			else if (res == 2) {
				temp ^= (temp & 0x0000ff00);
			}
			else if (res == 3) {
				temp ^= (temp & 0x000000ff);
			}
			//EX_MEMi.read_data2 == write data
			EX_MEMo.read_data2 <<= (8 * (3 - res));
			EX_MEMo.read_data2 |= temp;
			//printf("data write, write data : %x\n", EX_MEMo.read_data2	);
			strcpy(datam[i].data, toBi(EX_MEMo.read_data2, 32));
		}
		//printf("save data : %s\n", toBi(write_data3, 8));
	}
	//printf("temp :  %x\n", temp);
	MEM_WBi.read_data = temp; //

	//printf("MEM_WBi.ALU_value : %x\n", MEM_WBi.ALU_value);

	//printf("MEM_WBi.read_data : %x\n", MEM_WBi.read_data);
}
void WB() {
	//printf("---------------wb---------------\n");
	last_pPC = MEM_WBo.pPC;
	int write_data = MEM_WBo.ALU_value;
	if (MEM_WBo.MemtoReg == TRUE) {
		write_data = MEM_WBo.read_data;
	}
	else {
		write_data = MEM_WBo.ALU_value;
	}
	if (MEM_WBo.RegWrite == TRUE) {
		reg[MEM_WBo.write_addr] = write_data;
	}
	last_addr = MEM_WBo.write_addr;
	last_data = write_data;
	last_RegWrite = MEM_WBo.RegWrite;
}
void update() {
	//printf("pc , inst_size : %d, %d\n", pc, inst_size);
	if (hazard_ctrl.B) {
		//printf("bf branch, pc : 0x%x, 0x%x\n", ID_EXi.branch, PC);//atp 
		pc += (ID_EXi.branch - PC - 4) / 4;
		memset(&IF_IDi, 0, sizeof(struct IF_ID_stateReg));
		hazard_ctrl.B = FALSE;
	}
	else if (hazard_ctrl.Bf1) { //antp fail change to the branch PC
		//printf("bf1 branch, pc : 0x%x, 0x%x\n", EX_MEMo.branch, PC);
		pc += (EX_MEMo.branch - PC-4) / 4;
		memset(&IF_IDi, 0, sizeof(struct IF_ID_stateReg));
		memset(&ID_EXi, 0, sizeof(struct ID_EX_stateReg));
		memset(&EX_MEMi, 0, sizeof(struct EX_MEM_stateReg));
		hazard_ctrl.Bf1 = FALSE;
	}
	else if (hazard_ctrl.Bf2) { //atp fail change to the origin PC
		//printf("bf2 branch, pc : 0x%x, 0x%x\n", EX_MEMo.branch, PC);
		pc += (EX_MEMo.PC - PC-4) / 4;//?
		memset(&IF_IDi, 0, sizeof(struct IF_ID_stateReg));
		memset(&ID_EXi, 0, sizeof(struct ID_EX_stateReg));
		hazard_ctrl.Bf2 = FALSE;
	}
	else if (hazard_ctrl.J) {
		pc -= 1;
		memset(&IF_IDi, 0, sizeof(struct IF_ID_stateReg));
		hazard_ctrl.J = FALSE;
		//printf("check--------------------\n");
	}
	else if (hazard_ctrl.L) {
		pc -= 1;
		memset(&ID_EXi, 0, sizeof(struct ID_EX_stateReg));
		IF_IDi.stall = TRUE;
		hazard_ctrl.L = FALSE;
	}

	if (!IF_IDi.stall)IF_IDo = IF_IDi; IF_IDi.stall = 0;
	if(!ID_EXi.stall)ID_EXo = ID_EXi; IF_IDi.stall = 0;
	if (!ID_EXi.stall)EX_MEMo = EX_MEMi; IF_IDi.stall = 0;
	MEM_WBo = MEM_WBi;
	//printf("pc , inst_size : %d, %d\n", pc, inst_size);
}

void memState(int m1, int m2) {
	printf("Memory content [0x%x..0x%x]:\n---------------------\n", m1, m2);
	int i, j;
	if (m1 >= DATAADDR) {
		i = (m1 - DATAADDR)/4;
		j = (m2 - DATAADDR)/4;

		for (i; i <= j; i++) {
			printf("0x%x 0x%x\n", DATAADDR + 4 * i, (int)strtol(datam[i].data, NULL, 2));
		}
	}
	else {
		i = (m1 - TEXTADDR) / 4;
		j = (m2 - TEXTADDR) / 4;

		for (i; i <= j; i++) {
			
			printf("0x%x: 0x%x\n", TEXTADDR + 4 * i, (int)strtol(instm[i].inst, NULL, 2));
		}
	}
	printf("\n");
}
void regState() {
	printf("Current regsiter values:\n%s\nPC: 0x%x\n", "--------------------", PC);
	printf("Registers:\n");
	for (int i = 0; i < 32; i++) {
		printf("R%d: 0x%x\n", i, reg[i]);
	}
	printf("\n");
}
void pipeState() {
	int pPC[5] = { IF_IDo.pPC, ID_EXo.pPC, EX_MEMo.pPC, MEM_WBo.pPC, last_pPC };
	printf("Current pipeline PC state\n");
	printf("{");
	for (int i = 0; i < sizeof(pPC) / sizeof(int); i++) {
		if (pPC[i] != 0) {
			printf("0x%x", pPC[i]);

		}
		if (i == 4) {
			printf("}\n\n"); 
			break;
		}
		printf("|");
	}
	//printf("{0x%x|0x%x|0x%x|0x%x|0x%x}\n\n", IF_IDo.pPC, ID_EXo.pPC, EX_MEMo.pPC, MEM_WBo.pPC, last_pPC );
}
void sRegstate(){
	printf("Current state register\n");
	printf("		IF_ID,	 ID_EX,	  EX_MEM,	MEM_WB		end_PC\n");
	printf("pPC	: 0x%8X, 0x%8X, 0x%8X, 0x%8X, 0x%8X\n", IF_IDo.pPC, ID_EXo.pPC, EX_MEMo.pPC, MEM_WBo.pPC, last_pPC);
	printf("PC	: 0x%8X, 0x%8X, 0x%8X, 0x%8X\n", IF_IDo.PC, ID_EXo.PC, EX_MEMo.PC, MEM_WBo.PC);
	printf("jump	:	None, 0x%8x,	None,	None\n", ID_EXo.jump);
	printf("branch	:	None, 0x%8x, 0x%8x,	None\n", ID_EXo.branch, EX_MEMo.branch);
	//control info
	printf("ALUOP	:	None,	%8d,	None,	None\n", ID_EXo.ALUOP);
	printf("ALUSrc	:	None,	%8d,	None,	None\n", ID_EXo.ALUSrc);
	printf("Jump	:	None,	%8d,	None,	None\n", ID_EXo.Jump);
	printf("Branch	:	None,	%8d, %8d,	None\n", ID_EXo.Branch, EX_MEMo.Branch);
	printf("PCSrc	:	None,	%8d,	None,	None\n", ID_EXo.PCSrc);
	printf("MemRead	:	None,	%8d, %8d,	None\n",ID_EXo.MemRead, EX_MEMo.MemRead);
	printf("MemWrite:	None,	%8d, %8d,	None\n", ID_EXo.MemWrite, EX_MEMo.MemWrite);
	printf("MemtoReg:	None,	%8d, %8d, %8d\n", ID_EXo.MemtoReg, EX_MEMo.MemtoReg, MEM_WBo.MemtoReg);
	printf("RegWrite:	None,	%8d, %8d, %8d\n", ID_EXo.RegWrite, EX_MEMo.RegWrite, MEM_WBo.RegWrite);
	printf("RegDst	:	None,	%8d,	None,	None\n", ID_EXo.RegDst);
	printf("Shift	:	None,	%8d,	None,	None \n", ID_EXo.Shift);
	printf("Lui     :	None,	%8d,	None,	None\n", ID_EXo.Lui);
	//data
	printf("readdat1:	None,	0x%8x,	None,	None\n", ID_EXo.read_data1);
	printf("readdat2:	None,	0x%8x,	None,	None\n", ID_EXo.read_data2);
	printf("rs     :	None,	%8d,	None,	None\n", ID_EXo.rs);
	printf("rt     :	None,	%8d,	None,	None\n", ID_EXo.rt);
	printf("imm     :	None,	%8d,	None,	None\n", ID_EXo.imm);

	printf("readdat2:	None,	    None,  %8d,	None\n", EX_MEMo.read_data2);
	printf("ALU_val :	None,	    None,  %8d, %8d\n", EX_MEMo.ALU_value, MEM_WBo.ALU_value);
	printf("ALU_zero:	None,	    None,  %8d,	None\n", EX_MEMo.ALU_zero);
	printf("wReg    :	None,	    None,  %8d, %8d\n", EX_MEMo.write_addr, MEM_WBo.write_addr);
	printf("read_dat:	None,	    None,	None,%8d\n", MEM_WBo.read_data);
}

int main(int argc, char* argv[]) {
	int maddr[2] = { 0 }; //range of memory address
	int n_num = 0; //-n option number of instruction
	//input option control
	for (int i = 1; i < argc; i++) {
		if (i == 1) {
			if (!strcmp(argv[i], "-atp")) {
				tp = 1; 
			}
			else if (!strcmp(argv[i], "-antp")) {
				tp = 0; 
			}
			else {
				printf("error : Please clarify the branch predictor \n");
				exit(1);
			}
		}
		
		else if (!strcmp(argv[i], "-m")) {
			m = TRUE;
			char* temp = argv[++i];
			strtok(temp, ":");
			maddr[0] = (int)strtol(temp, NULL, 16);
			temp = strtok(NULL, ":");
			maddr[1] = (int)strtol(temp, NULL, 16);
		}
		else if (!strcmp(argv[i], "-d")) {
			d = TRUE;
		}
		else if (!strcmp(argv[i], "-p")) {
			p = TRUE;
		}
		else if (!strcmp(argv[i], "-n")) {
			n = TRUE;
			n_num = atoi(argv[++i]);
		}
		else {
			fp = fopen(argv[i], "r");
		}
	}
	if (fp == NULL) {
		printf("error : There are no object file %s\n", argv[argc-1]);
		exit(2);
	}
	//inst, data memory allocated 
	preprocess(); 
	
	int count = 0;
	while (pc < inst_size+5) {// 5 or 4??
		if (n && (n_num == count))break;//n option break
		pc = (PC - TEXTADDR) / 4;
		IFetch();
		WB();
		Dec();
		Exec();
		Mem();
		//WB();

		update();
		count++;//cycle checker
		if(pc < inst_size)pc++;//no udapte
		PC = TEXTADDR + pc * 4;
		if (d) { //d option is True
			printf("=====Cycle %d =====\n\n", count);
			//sRegstate();
			if (p) {
				pipeState();
			}
			regState();
			if (m) { //m option is True
				memState(maddr[0], maddr[1]);
			}
		}
		if (!IF_IDo.pPC && !ID_EXo.pPC && !EX_MEMo.pPC && !MEM_WBo.pPC) {
			IFetch();
			WB();
			Dec();
			Exec();
			Mem();
			update();
			if (d) { //d option is True
				printf("=====Cycle %d =====\n\n", count);
				//sRegstate();
				if (p) {
					pipeState();
				}
				regState();
				if (m) { //m option is True
					memState(maddr[0], maddr[1]);
				}
			}

			break;
		}
	}

	if (!d) {//d option is False
		printf("=====Cycle %d =====\n\n", count);
		if (p) {
			pipeState();
		}
		regState();
		if (m) { //m option is True
			memState(maddr[0], maddr[1]);
		}
	}
	
	//printf("tp : %d, m : %d,  d : %d, p : %d, n_num : %d  \n", tp, m, d, p, n_num);

	//printf("what's happen pc :%d, inst_size%d\n", pc, inst_size);
	//`printf("m1 : 0x%x, m2 : 0x%x, d : %d, n : %d\n", ma[0], ma[1], dc, n);
}

