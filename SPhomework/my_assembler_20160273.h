/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
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
* instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
* ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
* ���� ���� �ϳ��� instruction�� �����Ѵ�.
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
* ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
*/
char *input_data[MAX_LINES];
static int line_num;

int label_num;

/*
* ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
* operator�� renaming�� ����Ѵ�.
* nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
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

int tocount[MAX_LINES] = { NULL, }; // output �Լ��� ���ϸ��� �� ���, �� ���ڿ� �ϳ� �� token �������� ������ 
char * output[MAX_LINES];			// output �� ����� ���ڿ� ����
int myopcode; //search_opcode �Լ� ��� �ÿ� ����ϴ� ����
/*
* �ɺ��� �����ϴ� ����ü�̴�.
* �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
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

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
void make_symtab_output(char *file_name);