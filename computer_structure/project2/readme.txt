Computer Architecture Project2
												
Project goal 
MIPS 인스트럭션을 실행할 수 있는 MIPS 에뮬레이터를 구현. MIPS 기반의 바이너리 object file(*.o)을 input으로 받으며 입력된 바이너리를 통해 각 인스트럭션을 수행하고 register 및 memory에 data를 load한다.

Environment 
wsl2 ubuntu 20.04.4 

Execution
gcc –o runfile file_name(*.c)로 컴파일한 뒤 ./runfile [-m addr1:addr2] [-d] [n- num_instruction] input file 명령어 수행. -m는 메모리 주소 범위addr1~addr2) 내용을 출력하며 시작주소는 text section의 경우 0x400000, data section의 경우 0x10000000이다. -d는 한 인스트럭션이 수행될 때마다 모든 레지스터의 값을 출력한다. -n의 경우 num_instruction 개수만큼 인스터럭션이 수행된다.

Code Description
STRUCT INSTM : instruction memory
STRUCT DATAM : data memory
//file load and save
void openFile(char* s) : assembly file을 불러옴
preprocess(FILE) : instruction과 data를 memory에 save
//type change
char *toBi(int t, int size) : int type의 decimal를 char* type binary로 변환
int signExtend() : imm 값을 signExtendtion 
//stage
void controlUnit(int pc) : instr[31-26] 부분을 opcode 받아 ALU Control signal을 설정한다. 
void IFetch(int pc) : text memory에 있는 instruction을 fetch하여 decode 
void Dec(int pc) : text memory에 있는 instruction 받아서 regist에서 read
void Exec(int pc) : ALU control signal을 바탕으로 ALU에서 연산 수행
void Mem(int pc) : load와 store을 위해 data memory에 access
void WB(int pc) : data memory에 받은 data를 register에 write
//print code
void imTest(int m1, int m2) : text memory의 m1~m2 addr 및 instruction 출력
void dmTest(int m1, int m2) : data memory의 m1~m2 addr 및 data 출력
void regTest(int pc)

