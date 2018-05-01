/*
* my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define NN 0b00100000
#define II 0b00010000
#define XX 0b00001000
#define BB 0b00000100
#define PP 0b00000010
#define EE 0b00000001
/*
* instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
* 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
* 라인 별로 하나의 instruction을 저장한다.
*/
struct inst_unit {
	char *instname;
	char *format;
	char *opcode;
	char *num_operand;
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

/*
* 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
* operator는 renaming을 허용한다.
* nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
*/
struct token_unit {
	int addr;
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char nixbpe;
	int obcode;
	
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

int tocount[MAX_LINES] = { NULL, }; // output 함수로 파일만들 때 사용, 각 문자열 하나 당 token 개수들을 저장함 
char * output[MAX_LINES];			// output 에 출력할 문자열 생성
int myopcode; //search_opcode 함수 사용 시에 사용하는 변수
/*
* 심볼을 관리하는 구조체이다.
* 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
*/
struct symbol_unit {
	char symbol[10];
	int addr;
};
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int locctr;

struct literal_unit{
	char literal[10];
	int addr;
	int sec_addr;
};
typedef struct literal_unit literal;
literal lit_table[MAX_LINES];
static int section;
char *lit = "";
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* 추후 프로젝트에서 사용하게 되는 함수*/
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
void make_symtab_output(char *file_name);