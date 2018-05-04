/*
* my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
*
*/
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define NN 0b00100000 //nixbpe �� n
#define II 0b00010000 //nixbpe �� i
#define XX 0b00001000 //nixbpe �� x
#define BB 0b00000100 //nixbpe �� b
#define PP 0b00000010 //nixbpe �� p
#define EE 0b00000001 //nixbpe �� e
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
	int sec_addr; //���� ���� ��ȣ ---------- > ������ 0(COPY), 1(RDREC), 2(WRREC) �� ����
	int addr; // ���� ������ �ּ��Ҵ�
	int obcode; // �ٲ��� ���� �ڵ带 �����ϴ� �� 
	char *label;
	char *operator;
	char *operand[MAX_OPERAND];
	char *comment;
	char nixbpe;
};
int * leng_ob;
int leng_obcd;
typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;
int lit_save; // strtok�� �̿��� �߶��� ��ū�� �����ϴ� ��
int C_save; //���ͷ��� ������ �� ���ͷ�����
int X_save; //���ͷ��� ������ �� ���ͷ� ����
int tocount[MAX_LINES] = { NULL, }; // output �Լ��� ���ϸ��� �� ���, �� ���ڿ� �ϳ� �� token �������� ������ 
char * output[MAX_LINES];			// output �� ����� ���ڿ� ����
char *ob_output[MAX_LINES];
int myopcode; //search_opcode �Լ� ��� �ÿ� ����ϴ� ����

			  /*
			  * �ɺ��� �����ϴ� ����ü�̴�.
			  * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
			  */
static int section; //���� ���� 

struct symbol_unit {
	char symbol[10];
	int addr;
	int sec_addr;
};
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
static int locctr;

struct literal_unit {
	char literal[10];
	int addr;
	int sec_addr;
	char *lit;
};
typedef struct literal_unit literal;
literal lit_table[MAX_LINES];

int endaddr[3] = { 0, };
char *bufT = ""; //���� ��
char *bufH = ""; //���� ����
char *lit = "";
FILE *object_output;
int leng = 0; // �ƿ�ǲ�ڵ忡�� ���� ���̰���Ҷ�
int opop = 0; // �ƿ�ǲ �ڵ��Լ����� OPCODE ����� ��
int lit_cleng = 0; //���ͷ��� ������ ���� ���� ���
int lit_xleng = 0; //���ͷ��� ������ ���� ���� ���
int rsub = 0; // RSUB �϶�, ���� �ڵ� ����
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
void section_HDR(int i);
void section_T(int i);
void section_M(int finish_T);
int cal_leng(int i, int op, int leng);