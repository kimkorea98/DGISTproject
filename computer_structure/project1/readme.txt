Computer Architecture Project1
							
				
Project goal 
MIPS 기반의 assembly file(*.s)을 binary file로 변환한 object file(*.o)을 출력

Environment 
wsl2 ubuntu 20.04.4 

Execution
gcc –o runfile file_name(*.c)로 컴파일한 뒤 ./runfile <assembly file> 명령어 수행. output으로 assembly file(*.s)명에 해당되는 object file(*.o) 생성 및 예상 결과 출력 

Code Description
//file load and save
void openFile(char* s) : assembly file을 불러옴
void makeFile(char* s) : object file을 생성
//type change
void strupr(char str[]) : Capital letter로 변환
char *toBi(int t, int size) : int type의 decimal를 char* type binary로 변환
char *biToin(char* binary) : char* type binary를 char* type heximal로 변환
//search 
int searchOp(char *opcode) : 해당 opcode의 정보를 structure에서 탐색 
int searchLab(char *label) : 해당 label의 정보를 structure에서 탐색 
int searchAddr(int addr) : 해당 address의 정보를 structure에서 탐색 
//read and analysis the line
void getCode(char *line) : file의 해당 line에 대한 assembly code 분석 및 load
//store
void storeCode() : 현재 main에 정의된 instruction를 가상 메모리 0x400000에 저장
void storeDate() : 현재 main에 정의된 value를 가상 메모리 0x400000에 저장

//main
void checkFile(FLIE *file) : 불러온 파일을 확인 및 가상 메모리에 데이터 load
void process() : load된 데이터 처리 및 출력 
