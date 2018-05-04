/*
* 화일명 : my_assembler_00000000.c
* 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
* 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
* 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
*/

/*
*
* 프로그램의 헤더를 정의한다.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20160273.h"

/* ----------------------------------------------------------------------------------
* 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
* 매계 : 실행 파일, 어셈블리 파일
* 반환 : 성공 = 0, 실패 = < 0
* 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
*		   또한 중간파일을 생성하지 않는다.
* ----------------------------------------------------------------------------------
*/
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}
	for (int i = 0; i < MAX_LINES; i++) { //파일을 만들기 위한 문자열 선언
		output[i] = (char *)malloc(sizeof(char) * 50);
	}
	for (int i = 0; i < MAX_LINES; i++) { //파일을 만들기 위한 문자열 선언
		ob_output[i] = (char *)malloc(sizeof(char) * 50);
	}
	if (assem_pass1() < 0) {
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	//	make_opcode_output("output_20160273.txt");
	make_symtab_output("symtab_20160273.txt");
	//	make_opcode_output(NULL);
	//	make_symtab_output("symtab_20160273.txt");
	//	assem_pass2();
	//	make_objectcode_output("output_20160273");

	if (assem_pass2() < 0) {
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}
	make_objectcode_output("output.txt");
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
* 매계 : 없음
* 반환 : 정상종료 = 0 , 에러 발생 = -1
* 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
*		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
*		   구현하였다.
* ----------------------------------------------------------------------------------
*/
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
* 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
*        생성하는 함수이다.
* 매계 : 기계어 목록 파일
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
*
*	===============================================================================
*		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
*	===============================================================================
*
* ----------------------------------------------------------------------------------
*/
int init_inst_file(char *inst_file)
{
	//inst_table 동적할당
	for (int i = 0; i < MAX_INST; i++) {
		inst_table[i] = (inst *)malloc(sizeof(inst));
		inst_table[i]->instname = (char *)malloc(sizeof(char) * 10);
		inst_table[i]->format = (char *)malloc(sizeof(char) * 10);
		inst_table[i]->opcode = (char *)malloc(sizeof(char) * 10);
		inst_table[i]->num_operand = (char *)malloc(sizeof(char) * 10);
	}
	FILE * file;
	char *temp[4] = { NULL, }; //토큰을 넣어놓을 임시변수
	for (int i = 0; i < 4; i++) {
		temp[i] = (char *)malloc(sizeof(char) * 10);
	}
	char * save = (char *)malloc(sizeof(char) * 10); // strtok을 활용할 때 쓰는 변수
	char * i_str = (char *)malloc(sizeof(char) * 10);

	int errno;

	file = fopen("inst.data", "r");
	if (file != NULL) {
		char strTemp[30];
		char *pStr;
		while (!feof(file))
		{
			pStr = fgets(strTemp, sizeof(strTemp), file);
			strcpy(i_str, pStr); // pStr 훼손을 방지하기 위해 strtok를 사용할 문자열을 i_str에 복사

			save = strtok(i_str, "\t"); //strtok 한 것을 save 에 넣음
			strcpy(temp[0], save);//strtok 한 것을 temp[0] 에 넣음
			strcpy(inst_table[inst_index]->instname, temp[0]); //temp[0] = 명령어 이름

			save = strtok(NULL, "\t");
			strcpy(temp[1], save);
			strcpy(inst_table[inst_index]->format, temp[1]); // temp[1] = 명령어 형식

			save = strtok(NULL, "\t");
			strcpy(temp[2], save);
			strcpy(inst_table[inst_index]->opcode, temp[2]); // temp[2] = opcode

			save = strtok(NULL, "\t");
			strcpy(temp[3], save);
			strcpy(inst_table[inst_index]->num_operand, temp[3]); //temp[3] = 오퍼랜드 개수
			save = strtok(NULL, "\t");

			inst_index++;
		}
		fclose(file);
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
* 매계 : 어셈블리할 소스파일명
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : 라인단위로 저장한다.
*
* ----------------------------------------------------------------------------------
*/
int init_input_file(char *input_file)
{
	for (int i = 0; i < MAX_LINES; i++) {
		input_data[i] = (char *)malloc(sizeof(char) * 50);
	}
	char *input_str = (char *)malloc(sizeof(char) * 50);
	FILE * file;
	file = fopen("input.txt", "r");
	int errno;

	if (file != NULL) {
		char strTemp[50];
		char *pStr;
		while (!feof(file))
		{
			pStr = fgets(strTemp, sizeof(strTemp), file); //한 줄 씩 잘라서 input_data 에 넣음
			strcpy(input_str, pStr);
			strcpy(input_data[line_num], input_str);
			line_num++;
		}

		fclose(file);
	}

	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
*        패스 1로 부터 호출된다.
* 매계 : 파싱을 원하는 문자열
* 반환 : 정상종료 = 0 , 에러 < 0
* 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
* ----------------------------------------------------------------------------------
*/
int token_parsing(char *str)
{
	if (str == NULL) {
		sscanf("%s", "\0");
	}
	int count_com = 0;
	int i = token_line;
	int op = 0;

	token_table[i] = (token *)calloc(1, sizeof(token));
	token_table[i]->label = (char *)calloc(10, sizeof(char));
	token_table[i]->operator = (char *)calloc(30, sizeof(char));
	for (int j = 0; j < MAX_OPERAND; j++) {
		token_table[i]->operand[j] = (char *)calloc(30, sizeof(char));
	}

	token_table[i]->comment = (char *)calloc(30, sizeof(char));
	char * token_str = (char *)malloc(sizeof(char) * 30); // str을 복사하기 위해 선언된 변수
	char * save = (char *)malloc(sizeof(char) * 30); // 탭을 기준으로 strtok 를 사용하기 위해 선언된 변수
	char * toto = (char *)malloc(sizeof(char) * 30); //콤마를 기준으로 strtok 를 사용하기 위해 선언된 변수
	int token_count = 0;
	strcpy(token_str, str); // str 훼손을 방지하기 위해 strtok를 사용할 문자열을 token_str에 복사


	char *zero = (char *)malloc(sizeof(char) * 30);

	lit_table[i].addr = 1;
	strcpy(toto, str);		// strtok를 사용할 문자열을 token_str에 복사
	save = strtok(token_str, "\t");		// strtok 를 통해
	while (save != NULL) {				// 탭을 기준으로 
		save = strtok(NULL, "\t");	// 토큰들을 잘라주고
		token_count++;				// 토큰 개수를 셉니다.
	}

	if (strstr(str, ",")) {				// strstr로 콤마가 들어가 있는 문자열을 찾고
		toto = strtok(toto, ",");		// strtok 를 통해
		while (toto != NULL) {			// 콤마를 기준으로
			toto = strtok(NULL, ",");	// 토큰들을 잘라주고
			count_com++;				// 콤마 개수를 셉니다
		}
	}
	tocount[token_line] = token_count;	// 각 문자열 하나 당 토큰 개수를 저장합니다 ( tocount 배열은 output 함수로 파일만들 때 사용 )

	switch (token_count) {				// 토큰 개수를 이용한 switch - case 문
	case 1: if (str[0] == '.') {		// 토큰 개수가 1개이고, 문자열 시작이 . 이라면 .을 label 에 넣어줍니다
		strcpy(token_table[token_line]->operator, "");
		sscanf(input_data[token_line], "%s\t%s", token_table[token_line]->label, token_table[token_line]->operator);
		strcpy(output[token_line], input_data[token_line]);
	}
			else {						// 아니라면 토큰 하나를 operator 에 넣어줍니다.
				strcpy(token_table[token_line]->label, "");
				sscanf(input_data[token_line], "%s", token_table[token_line]-> operator);
				strcpy(output[token_line], input_data[token_line]);
			}
			break;
	case 2:								// 토큰 개수가 2개이고, 문자열 시작이 . 이라면 .을 label에, 나머지 토큰은 comment에 넣어줍니다.
		if (strstr(input_data[token_line], ".")) {
			strcpy(token_table[token_line]->operator, "");
			sscanf(input_data[token_line], "%s\t%s\t%[^\r\n]", token_table[token_line]->label, token_table[token_line]->operator, token_table[token_line]->comment);
			strcpy(output[token_line], input_data[token_line]);
		}
		else if (str[0] == '\t') {	// 토큰 개수가 2개이고, 문자열 처음이 탭이라면 label은 없다는 뜻
			strcpy(token_table[token_line]->label, "");
			sscanf(input_data[token_line], "%s", token_table[token_line]->operator); //먼저 operator를 가져와 
			op = search_opcode(token_table[token_line]->operator);
			if (op >= 0 && strstr(inst_table[op]->num_operand, "0")) {				 //inst_table의 오퍼랜드 개수가 0인것을 확인
				sscanf(input_data[token_line], "%s\t\t%[^\r\n]", token_table[token_line]->operator, token_table[token_line]->comment);
				strcpy(output[token_line], input_data[token_line]);
			}
			else if (count_com == 3) {	// 그 때의 comma 로 나눈 문자열 개수가 3개라면 ,를 두번 써서 각 operand[0],[1],[2] 에 넣어줌
				sscanf(input_data[token_line], "%s\t%[^,],%[^,],%s", token_table[token_line]->operator, token_table[token_line]->operand[0], token_table[token_line]->operand[1], token_table[token_line]->operand[2]);
				strcpy(output[token_line], input_data[token_line]);
			}
			else if (count_com == 2) {	// 그 때의 comma 로 나눈 문자열 개수가 3개라면 ,를 두번 써서 각 operand[0],[1],[2] 에 넣어줌
				sscanf(input_data[token_line], "%s\t%[^,],%s",
					token_table[token_line]->operator, token_table[token_line]->operand[0], token_table[token_line]->operand[1]);
				strcpy(output[token_line], input_data[token_line]);
			}
			else {					// comma도 없고, label이 없으면 operator, operand[0]에 저장
				sscanf(input_data[token_line], "%s\t%s", token_table[token_line]->operator, token_table[token_line]->operand[0]);
				strcpy(output[token_line], input_data[token_line]);

			}
		}
		else {	// 문자열 처음이 탭이면 label과 operator 에 저장
			sscanf(input_data[token_line], "%s\t%s", token_table[token_line]->label, token_table[token_line]->operator);
			strcpy(output[token_line], input_data[token_line]);

		}
		break;
	case 3: if (str[0] == '\t') { // label 이 없을 때
		strcpy(token_table[token_line]->label, "");
		if (count_com == 2) {	// operand 에 콤마가 있을 때
			sscanf(input_data[token_line], "%s\t%[^,],%s\t%[^\r\n]", token_table[token_line]->operator, token_table[token_line]->operand[0], token_table[token_line]->operand[1], token_table[token_line]->comment);
			strcpy(output[token_line], input_data[token_line]);
		}
		else {					// operand 에 콤마가 없을 때
			strcpy(token_table[token_line]->operand[1], "");
			sscanf(input_data[token_line], "%s\t%s\t%[^\r\n]", token_table[token_line]->operator, token_table[token_line]->operand[0], token_table[token_line]->comment);
			sscanf(input_data[token_line], "%s %s %[^\r\n]", token_table[token_line]->operator, token_table[token_line]->operand[0], token_table[token_line]->comment);
			strcpy(output[token_line], input_data[token_line]);
		}
	}
			else {				// label 이 있을 때
				sscanf(input_data[token_line], "%s\t%s\t%s",
					token_table[token_line]->label, token_table[token_line]->operator, token_table[token_line]->operand[0]);
				strcpy(output[token_line], input_data[token_line]);
			}
			break;
	case 4: {	// label, operator, operand, comment가 다 있을 때
		sscanf(input_data[token_line], "%s\t%s\t%s\t%[^\r\n]", token_table[token_line]->label, token_table[token_line]->operator,token_table[token_line]->operand[0], token_table[token_line]->comment);
		strcpy(output[token_line], input_data[token_line]);
	}
			break;
	default:
		return -1;
		break;
	}
	if (strstr(token_table[token_line]->operator,"+") && (!strcmp(token_table[token_line]->operand[1], "X"))) {  //4형식 중, operand[1]에 x가 있으면 operand[2]로 넣어줌
		strcpy(token_table[token_line]->operand[2], token_table[token_line]->operand[1]);
		strcpy(token_table[token_line]->operand[1], "");
	}
	if (strstr(token_table[token_line]->operator,"CSECT")) { //CSECT 이나오면 섹션 구분하기
		section++;

	}
	token_table[token_line]->sec_addr = section;

	sym_table[token_line].sec_addr = token_table[token_line]->sec_addr;
	if (strstr(token_table[token_line]->operand[0], "=")) { // 오퍼랜드에 "="이 있으면 리터럴 테이블에 오퍼랜드 값 넣어주기
		strcpy(lit_table[token_line].literal, token_table[token_line]->operand[0]);
		lit_table[token_line].sec_addr = section;
	}

	token_line++;
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
* 매계 : 토큰 단위로 구분된 문자열
* 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
* 주의 :
*
* ----------------------------------------------------------------------------------
*/
int search_opcode(char *str)
{
	int realopcode = -1;

	for (int i = 0; i < inst_index; i++) {
		if (str[0] == '+') { //operator와 inst_table의 명령어이름과 비교
			str = str++;
		}
		else if (!strcmp(str, inst_table[i]->instname)) { //operator 에 4형식일 때(+) +를 건너뛰고 검색
			return i;
		}
		else if (str == "\0") {
			realopcode = -1;
		}
	}
	return realopcode;
}


/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	for (int i = 0; i < line_num; i++) { //line_num (전체 input.txt) 길이 만큼 토큰파싱하기
		token_parsing(input_data[i]);
	}
	//	int op = 0;
	int save = 0;
	for (int i = 0; i < token_line; i++) {

		token_table[i]->addr = locctr;
		if (!strcmp(token_table[i]->operator,"START")) {  //START일때, 심볼테이블에 토큰테이블의 LABEL 넣어주기
			token_table[i]->obcode = -1;
			strcpy(sym_table[i].symbol, token_table[i]->label);

			continue;
		}
		else if (!strcmp(token_table[i]->operator,"END")) { //END일때, 
															//		token_table[i]->obcode = -1;
			for (int j = 0; j < i; j++) {
				if (!strcmp(lit_table[j].literal, token_table[j]->operand[0])) { //리터럴이라면, 현재 주소값 저장하고, endaddr 배열에서 [2]에 넣어주기
					save = locctr;
					endaddr[2] = save + 1;

				}
			}
			token_table[i]->addr = 0; //END를 만나면 0으로 초기화
			continue;
		}
		else if (strstr(token_table[i]->operator,"EXT") || (!strcmp(token_table[i]->label, "."))) {
			token_table[i]->obcode = -1;
			continue;
		}
		myopcode = search_opcode(token_table[i]->operator);
		if (myopcode >= 0) {
			token_table[i]->addr = locctr;
			if (strstr(inst_table[myopcode]->format, "1")) { //1형식일때,
				locctr += 1;
			}
			else if (strstr(inst_table[myopcode]->format, "2")) { //2형식일때, 
				locctr += 2;
			}
			else if (!strcmp(inst_table[myopcode]->format, "3/4")) {  //3형식 중
				if (strchr(token_table[i]->operator,'+')) { //4형식일때,
					locctr += 4;
				}
				else { //3형식일 때
					locctr += 3;
				}
			}

			if (!strcmp(token_table[i]->label, "")) { //심볼테이블에 넣어줘야할 지 확인
				strcpy(sym_table[i].symbol, "\0");
			}
			else {
				strcpy(sym_table[i].symbol, token_table[i]->label);
				sym_table[i].addr = token_table[i]->addr;
			}
		}
		else {
			token_table[i]->addr = locctr;
			if (!strcmp(token_table[i]->operator,"RESW")) { //RESW
				token_table[i]->obcode = -1;
				locctr += atoi(token_table[i]->operand[0]) * 3;
			}
			else if (!strcmp(token_table[i]->operator,"RESB")) { //RESB
				token_table[i]->obcode = -1;
				locctr += atoi(token_table[i]->operand[0]);

			}
			else if (!strcmp(token_table[i]->operator,"CSECT")) { //CSECT
				token_table[i]->obcode = -1;
				for (int j = 0; j < i; j++) {
					if (!strcmp(lit_table[j].literal, token_table[j]->operand[0])) {
						save = locctr;
						if (token_table[i]->sec_addr == 1) { //현재 섹션이 1이라할때, 섹션 0에 현재 주소값
							endaddr[0] = save;
						}
						else if (token_table[i]->sec_addr == 2) { //현재 섹션이 2라 할때, 섹션 1에 현재주소값 저장
							endaddr[1] = save;
						}
					}
				}
				locctr = 0; //CSECT 를 만났으니 0으로 초기화해서 섹션 구분함
				token_table[i]->addr = locctr;
			}
			else if (!strcmp(token_table[i]->operator,"BYTE")) { //BYTE일때, 
				locctr += 1;
			}
			else if (!strcmp(token_table[i]->operator,"WORD") || (!strcmp(token_table[i]->operator,"LTORG"))) { //WORD 나 LTORG 일때 , 3씩 증가
				locctr += 3;
			}
			if (!strcmp(token_table[i]->label, "")) { //심볼테이블에 넣을 지 확인
				strcpy(sym_table[i].symbol, "\0");
			}
			else {
				strcpy(sym_table[i].symbol, token_table[i]->label);
				sym_table[i].addr = token_table[i]->addr;

			}
			if (!strcmp(token_table[i]->operator,"EQU")) { //EQU 중
				token_table[i]->obcode = -1;
				if (strstr(token_table[i]->operand[0], "-")) { // 오퍼랜드에 - 가 있을 때
					int loc1 = 0;
					int loc2 = 0;
					bufT = strtok(token_table[i]->operand[0], "-"); // '-'를 기준으로 자른 앞 토큰을 bufT 에 저장
					bufH = strtok(NULL, "-");
					strcpy(token_table[i]->operand[0], bufT);
					strcpy(token_table[i]->operand[1], bufH); // '-'를 기준으로 자른 뒤 토큰을 bufF 에 저장
					for (int j = 0; j < token_line; j++) {
						if (!strcmp(token_table[i]->operand[0], sym_table[j].symbol)) { // 심볼테이블의 심볼과 비교 후,
							loc1 = token_table[j]->addr;
						}
						if (!strcmp(token_table[i]->operand[1], sym_table[j].symbol)) {
							loc2 = token_table[j]->addr;
						}
					}
					token_table[i]->addr = loc1 - loc2; // bufT - bufH 
					sym_table[i].addr = token_table[i]->addr; //심볼테이블에 저장
				}
			}
		}
		if (!strcmp(token_table[i]->operator, "LTORG")) { //LTORG에다 리터럴 주소 저장해줌 (따로 * 라인을 만들지 않음)
			token_table[i]->obcode = -1;
			for (int j = 0; j < i; j++) {
				if (!strcmp(lit_table[j].literal, token_table[j]->operand[0])) {
					lit_table[j].addr = token_table[i]->addr;
				}
			}
		}
	}
	for (int i = 0; i < token_line; i++) { // LTORG가 나오지 않았지만 "="이 있고, 주소가 지정되지않은 리터럴처리
		if (token_table[i]->operand[0] != NULL) {
			if (strstr(lit_table[i].literal, "=") && (lit_table[i].addr == 0)) {
				int j = i;
				for (; (token_table[j]->addr != 0); j++) {
					lit_table[i].addr = save;
				}
			}
		}

	}

	return 0;
}
/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	FILE *make;
	if (file_name == NULL) // NULL일 때는 파일 생성하지않고 출력만
		make = stdout;
	else {
		make = fopen(file_name, "w");
		printf("%s 파일이 생성되었습니다 \n", file_name);
	}
	for (int i = 0; i < line_num; i++) {
		myopcode = search_opcode(token_table[i]->operator); //operator를 tsearch_opcode 를 통해 opcode 검색
		if (myopcode == -1) { //operator 가 지시어일 때(명령어테이블에 없을 때)
			if (tocount[i] == 4) { //label , operator, operand, comment 다 있다면
				strcpy(token_table[i]->comment, "");
				sprintf(input_data[i], "%s\t%s\t%s\t%s\n", token_table[i]->label, token_table[i]->operator,token_table[i]->operand[0], token_table[i]->comment);
				strcpy(output[i], input_data[i]);
			}
			else
				strcpy(output[i], input_data[i]);
		}
		else {
			strcpy(token_table[i]->comment, "");
			switch (tocount[i]) { // 아까 토큰파싱함수처럼 token 개수를 세기
			case 2: { //2개면 operator , operand , opcode 넣어주기
				sprintf(input_data[i], "\t%s\t%s\t%s\n", token_table[i]->operator, token_table[i]->operand[0], inst_table[myopcode]->opcode);
				if (strstr(inst_table[myopcode]->num_operand, "0")) {
					sprintf(input_data[i], "\t%s\t%s\t%s\n", token_table[i]->operator,token_table[i]->comment, inst_table[myopcode]->opcode);
				}
				strcpy(output[i], input_data[i]);
			}
					break;
			case 3: {
				if (strstr(inst_table[myopcode]->num_operand, "2")) { //3개인데 operand 개수가 2개면 operator , operand[0],[1] , opcode 넣어주기
					sprintf(input_data[i], "\t%s\t%s\,%s\t%s\n", token_table[i]->operator,token_table[i]->operand[0], token_table[i]->operand[1], inst_table[myopcode]->opcode);
				}
				else
					sprintf(input_data[i], "\t%s\t%s\t%s\n", token_table[i]->operator,token_table[i]->operand[0], inst_table[myopcode]->opcode);
				strcpy(output[i], input_data[i]);
			}
					break;
			case 4: { //4개면 operator , operand , opcode 넣어주기
				sprintf(input_data[i], "%s\t%s\t%s\t%s\n",
					token_table[i]->label, token_table[i]->operator,token_table[i]->operand[0], inst_table[myopcode]->opcode);
				strcpy(output[i], input_data[i]);
			}
					break;
			default:
				break;
			}
		}
		fwrite(output[i], strlen(output[i]), 1, make); //make 파일에 쓰기	
	}
	fputs("\n", make);
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/
void make_symtab_output(char *file_name) {
	FILE *file;
	if (file_name == NULL)
		return -1;
	else {
		file = fopen(file_name, "w");
		printf("%s 파일이 생성되었습니다.\n", file_name);
	}
	for (int i = 0; i < token_line; i++) {
		if (strcmp(sym_table[i].symbol, "\0")) {
			fprintf(file, "%s\t\t\t%X\n", sym_table[i].symbol, sym_table[i].addr);
		}
	}
}
/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	int opindex = 0;
	int TA = 0;
	int PC = 0;
	char *temp = "";
	char *temp2 = (char *)malloc(sizeof(char) * 30);
	char *temp_cut = (char *)malloc(sizeof(char) * 30);
	char *temping = (char *)malloc(sizeof(char) * 30);
	char for2 = 0; // 2형식 레지스터값을 처리해줄 때 사용하는 변수입니다

				   /* assem_pass2 에서 기계어 코드로 바꾸는 과정을 진행합니다 기계어 코드는 토큰 테이블의 obcode 라는 멤버 변수에 저장합니다 */

	char *bw = NULL; //byte word 저장 변수
	for (int i = 0; i < token_line; i++) {
		opindex = search_opcode(token_table[i]->operator);
		if (opindex >= 0) {
			if (strstr(inst_table[opindex]->format, "2")) { //2형식일 때 레지스터마다 처리
				token_table[i]->obcode = (int)(strtol((inst_table[opindex]->opcode), NULL, 16)) << 8; //opcode 를 먼저 밀어주고
				for (int j = 0; j < 2; j++) { //num_operand 개수가 1개나 2개일 때에 따라서 각 레지스터에 따라 값을 넣어주고
					if (token_table[i]->operand[j] != NULL) {

						if (!strcmp(token_table[i]->operand[j], "A"))
							for2 = 0;
						else if (!strcmp(token_table[i]->operand[j], "X"))
							for2 = 1;
						else if (!strcmp(token_table[i]->operand[j], "L"))
							for2 = 2;
						else if (!strcmp(token_table[i]->operand[j], "B"))
							for2 = 3;
						else if (!strcmp(token_table[i]->operand[j], "S"))
							for2 = 4;
						else if (!strcmp(token_table[i]->operand[j], "T"))
							for2 = 5;
						else if (!strcmp(token_table[i]->operand[j], "F"))
							for2 = 6;
						else if (!strcmp(token_table[i]->operand[j], "PC"))
							for2 = 8;
						else if (!strcmp(token_table[i]->operand[j], "SW"))
							for2 = 9;
						if (j == 0) //첫번째로 들어가는 레지스터는 밀어줍니다
							for2 = (for2 << 4);
					}
					token_table[i]->obcode |= for2;

				}

			}
			if (!strcmp(inst_table[opindex]->format, "3/4")) {
				if (strstr(token_table[i]->operator,"+")) { //4형식일때 처리
					token_table[i]->obcode = (int)(strtol((inst_table[opindex]->opcode), NULL, 16)) << 24;
					token_table[i]->nixbpe = (NN + II + EE);
					if (!strcmp(token_table[i]->operand[2], "X")) { // operand에 X가 있을때,
						token_table[i]->nixbpe += XX; // nixbpe 중 x값을 증가합니다
						token_table[i]->obcode += (token_table[i]->nixbpe) << 20;
					}
					else {
						token_table[i]->obcode += (token_table[i]->nixbpe) << 20;
					}
				}
				else {
					token_table[i]->obcode = (int)(strtol((inst_table[opindex]->opcode), NULL, 16)) << 16;
					if (strchr(token_table[i]->operand[0], '#')) { //직접
						token_table[i]->nixbpe = II; // nixbpe 중 i 값만 증가
						token_table[i]->obcode += (token_table[i]->nixbpe) << 12;

						temp = strtok(token_table[i]->operand[0], "#");
						token_table[i]->obcode += atoi(temp);
					}
					else if (strchr(token_table[i]->operand[0], '@')) { //간접
						token_table[i]->nixbpe = (NN + PP); // nixbpe 중 n, p 값만 증가
						token_table[i]->obcode += (token_table[i]->nixbpe) << 12;
						temp = strtok(token_table[i]->operand[0], "@");
						for (int j = 0; j < token_line; j++) {
							if (!strcmp(temp, sym_table[j].symbol)) {
								TA = (sym_table[j].addr); // pc relative 를 위해 TA 구하고, 
							}
						}
						PC = (token_table[i + 1]->addr); // PC 값을 구해서 obcode 에 넣어줍니다
						token_table[i]->obcode += (TA - PC);

					}
					else if (!strcmp(lit_table[i].literal, token_table[i]->operand[0])) { //리터럴 처리해주는 경우
						strcpy(temping, lit_table[i].literal); //먼저 temp 에 리터럴 값을 복사하고
						lit = strtok(temping, "'"); // "'" 을 기준으로 잘라줍니다. ----> ex) =C'EOF' 를 "'"를 통해 2번잘라주면 EOF 가 나옴
						lit = strtok(NULL, "'"); // 그 값(EOF)을 lit 에 저장합니다.

						if (strstr(lit_table[i].literal, "=C")) { // 문자일 때,
							lit_cleng = strlen(lit);
							if (lit != NULL) {
								for (int j = 0; j < strlen(lit); j++) { // lit 값의 길이만큼
									if (j == 0) { // 0 이면 첫번째 공간으로 밀어주고
										lit_save |= lit[j] << 16;
									}
									else if (j == 1) { // 1이면 두번째 공간으로 밀어주고
										lit_save |= lit[j] << 8;
									}
									else {
										lit_save |= lit[j];
									}

									if (lit == NULL) {
										break;
									}
								}

							}
							C_save = lit_save;
						}
						else if (strstr(lit_table[i].literal, "=X")) { //정수일 때,
							lit_xleng = strlen(lit);
							if (lit != NULL) {
								for (int j = 0; j < strlen(lit); j++) {
									lit_save = lit[j];
									if (lit == NULL) {
										break;
									}
								}

							}
							X_save = lit_save;
						}
						token_table[i]->nixbpe = (NN + II + PP);  // nixbpe 중 n, i, p 값을 증가시키고
						token_table[i]->obcode += (token_table[i]->nixbpe) << 12;
						if (!strcmp(token_table[i]->operand[0], "")) { // 오퍼랜드가 없을 때, 주소값을 모르니까 0으로 바꿔줌 -----> ex) RSUB 오퍼랜드 x
							token_table[i]->obcode &= 127 << 16; // 넣어놓은 opcode 와 ni 값을 제외하고 0으로 바꿔줌
							rsub = token_table[i]->obcode;
						}
						token_table[i]->obcode += (token_table[i]->nixbpe) << 12;
						for (int j = 0; j < token_line; j++) {
							if (!strcmp(lit_table[j].literal, token_table[i]->operand[0])) { // 리터럴일때, 타겟 어드레스 만들기
								TA = (lit_table[j].addr);
							}
						}
						PC = (token_table[i + 1]->addr); //피씨 어드레스 생성
						token_table[i]->obcode += (TA - PC);
						for (int j = i; token_table[j]->addr == 0; j++) { // j 가 주소가 0인 곳을 만나기 전까지 LTORG를 찾는 과정 (다른 섹션으로 구분되기 전까지)
							if (!strcmp(token_table[j]->operator,"LTORG")) { //LTORG 있으면 LTORG 오브젝트 코드에 저장
								token_table[j]->obcode = lit_save; //리터럴 값을 저장함

							}
							else { //맨끝에저장해야함

								j = token_line;  // 미완성
								token_table[j - 1]->obcode = lit_save;
							}
						}

					}
					else {
						token_table[i]->nixbpe = (NN + II + PP); //경우의 수 다 제외하고 일반적인 3형식 
						token_table[i]->obcode += (token_table[i]->nixbpe) << 12;
						for (int j = 0; j < token_line; j++) {
							if (!strcmp(sym_table[j].symbol, token_table[i]->operand[0])) { //심볼이랑 오퍼랜드가 같을 때
								TA = sym_table[j].addr; //타겟 어드레스는 심볼의 어드레스
							}
						}
						PC = (token_table[i + 1]->addr);
						if (TA <= PC) { //PC 값이 TA 보다 클때 하는 과정
							token_table[i]->obcode += ((TA - PC) & 0x00000FFF);
						}
						else { //아니면 보통의 과정으로 처리
							token_table[i]->obcode += (TA - PC);
						}
					}

				}

			}
		}

		else if (!strcmp(token_table[i]->operator,"BYTE")) { // operator 가 바이트일 때, 

			strcpy(temp, token_table[i]->operand[0]);
			bw = strtok(temp, "'"); // 리터럴 문자와 같은 방법으로 "'"를 기준으로 잘라줌
			bw = strtok(NULL, "'");
			token_table[i]->obcode = (int)(strtol((bw), NULL, 16));
		}
		else if (!strcmp(token_table[i]->operator,"WORD")) { //WORD 일때 경우의 수 
			int now_s;
			now_s = token_table[i]->sec_addr;
			for (int j = 0; j<token_line; j++) {
				if (token_table[j]->sec_addr == now_s) {
					if (strcmp(sym_table[j].symbol, token_table[i]->operand[0]) || (strcmp(sym_table[j].symbol, token_table[i]->operand[1]))) {
						token_table[i]->obcode = 0;
					}
				}
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	int finish_T = 0;
	if (file_name == NULL)
		return -1;
	//		object_output = stdout;
	else {
		object_output = fopen(file_name, "w");
		printf("%s 파일이 생성되었습니다.\n", file_name);
	}

	for (int i = 0; i < token_line; i++) {
		if (token_table[i]->sec_addr == 0) { //섹션 번호 0일때
			section_HDR(i); //H , D, R 작성
			section_T(i); //T (텍스트부분) 작성
		}
	}
	section_M(finish_T); //M 작성
	fprintf(object_output, "\nE%06X", sym_table[0].addr); //섹션 번호 0 의 끝을 알림

	finish_T++; // 섹션 번호 증가와 동일한 의미
	for (int i = 0; i < token_line; i++) {
		if (token_table[i]->sec_addr == 1) { //섹션 번호 1일때
			section_HDR(i); //H, D, R 작성
			section_T(i); //T(텍스트 부분) 작성
		}
	}
	section_M(finish_T); //M 작성
	fprintf(object_output, "\nE"); //섹션 번호 1의끝을 알림

	finish_T++;
	for (int i = 0; i < token_line; i++) { //섹션 번호 2일 때
		if (token_table[i]->sec_addr == 2) {
			section_HDR(i);
			section_T(i);
		}
	}
	section_M(finish_T);
	fprintf(object_output, "\nE\n");
	fclose(object_output);
}
void section_HDR(int i) {  //처음 주소가 0인 심볼을 만났을 때, 
	if (!strcmp(sym_table[i].symbol, "COPY") || (!strcmp(sym_table[i].symbol, "RDREC")) || (!strcmp(sym_table[i].symbol, "WRREC"))) {
		if (!strcmp(sym_table[i].symbol, "COPY")) {
			fprintf(object_output, "H%s\t%06X", sym_table[i].symbol, sym_table[i].addr); // 미리 저장해놓았던 시작주소와 섹션의 전체 길이 
			fprintf(object_output, "%06X", endaddr[0]);
		}
		else if (token_table[i]->sec_addr == 1) {
			leng = 0;
			fprintf(object_output, "\nH%s\t%06X", sym_table[i].symbol, sym_table[i].addr); // 미리 저장해놓았던 시작주소와 섹션의 전체 길이 
			fprintf(object_output, "%06X", endaddr[1]);
		}
		else if (token_table[i]->sec_addr == 2) {
			leng = 0;
			fprintf(object_output, "\nH%s\t%06X", sym_table[i].symbol, sym_table[i].addr); // 미리 저장해놓았던 시작주소와 섹션의 전체 길이 
			fprintf(object_output, "%06X", endaddr[2]);
		}
	}
	if (!strcmp(token_table[i]->operator,"EXTDEF")) { //EXTDEF 이 나오면 
		fprintf(object_output, "\nD");
		for (int j = 0; j < token_line; j++) { // 지금 섹션의 심볼들과 오퍼랜드를 비교하고 파일에 값을 넣어줌
			if (!strcmp(token_table[i]->operand[0], sym_table[j].symbol))
				fprintf(object_output, "%s%06X", token_table[i]->operand[0], sym_table[j].addr);
		}
		for (int j = 0; j < token_line; j++) {
			if (!strcmp(token_table[i]->operand[1], sym_table[j].symbol))
				fprintf(object_output, "%s%06X", token_table[i]->operand[1], sym_table[j].addr);
		}
		for (int j = 0; j < token_line; j++) {
			if (!strcmp(token_table[i]->operand[2], sym_table[j].symbol))
				fprintf(object_output, "%s%06X", token_table[i]->operand[2], sym_table[j].addr);
		}
	}

	if (!strcmp(token_table[i]->operator,"EXTREF")) { //EXTREF 가 나오면
		fprintf(object_output, "\nR");
		for (int j = 0; token_table[i]->operand[j] != NULL; j++) {
			fprintf(object_output, "%s", token_table[i]->operand[j]); // 오퍼랜드 개수만큼 EXTREF 를 정의해줌 
		}
		fprintf(object_output, "\n");
	}

}
void section_T(int i) {
	int check = 32;
	char *str = "";
	opop = search_opcode(token_table[i]->operator);
	int op = 0;
	op = opop;

	if (!strcmp(sym_table[i].symbol, "FIRST")) { //처음 주소가 0인 심볼을 만났을 때의 처리 과정, 
		fprintf(object_output, "T%06X1D", sym_table[i].addr);
	}
	else if (token_table[i]->sec_addr == 1) {
		if ((token_table[i]->addr == 0) && (opop != -1)) {
			fprintf(object_output, "T%06X1D", sym_table[i].addr);
		}
	}
	else if (token_table[i]->sec_addr == 2) {
		if ((token_table[i]->addr == 0) && (opop != -1)) {
			fprintf(object_output, "T%06X1C", sym_table[i].addr);
		}
	}
	leng = cal_leng(i, opop, leng);
	if (opop >= 0) { //INST_TABLE 의 OPCODE 가 있는 아이들 중
		if (leng >= 30) {  //leng 결과에 따라 라인 길이 변경
			if (token_table[i]->sec_addr == 0) {
				fprintf(object_output, "\nT%06X0D", token_table[i]->addr);
				leng = 0;
			}
			else if (token_table[i]->sec_addr == 1) {
				fprintf(object_output, "\nT%06X0E", token_table[i]->addr);
				leng = 0;
			}
		}
		else if (!strcmp(token_table[i]->operator,"RESW")) { //RESW 만나면 개행
			fprintf(object_output, "\n");
		}
		if (strstr(inst_table[opop]->format, "2")) { //2형식이면 기계어코드 4개만 작성
			fprintf(object_output, "%04X", token_table[i]->obcode);
		}
		else if (!strcmp(token_table[i]->operand[0], "")) { //RSUB 일때, 위에서 한 rsub 변수 넣어주기  (오퍼랜드가 0인)
			fprintf(object_output, "%06X", rsub);
		}
		else {
			fprintf(object_output, "%06X", token_table[i]->obcode); //그 외의 것 넣어주기
		}

	}
	else {
		if (token_table[i]->obcode != -1) {
			if (!strcmp(token_table[i]->operator,"BYTE")) {
				fprintf(object_output, "%X", token_table[i]->obcode);
			}
			else if (!strcmp(token_table[i]->operator,"END")) { // 리터럴 처리해주는 곳 !!!
				fprintf(object_output, "%02c", X_save);

			}
			else
				fprintf(object_output, "%06X", token_table[i]->obcode);
		}
		if (!strcmp(token_table[i]->operator,"LTORG")) { // 리터럴 처리해주는 곳 !!!
			for (int j = 0; j < token_line; j++) {
				if (strstr(token_table[j]->operand[0], "=C")) {
					fprintf(object_output, "\nT%06X%02X%06X", token_table[i]->addr, 3, C_save);


				}
			}
		}
	}

}
int cal_leng(int i, int op, int leng) { //leng 길이 계산해주는 곳
	if (op >= 0) {
		if (strstr(inst_table[op]->format, "2")) {
			leng += 2;
		}
		else if (!strcmp(inst_table[op]->format, "3/4")) {
			if (strstr(token_table[i]->operand[0], "+")) {
				leng += 4;
			}
			else {
				leng += 3;
			}
		}
	}
	else {
		if (!strcmp(token_table[i]->operator,"LTORG") || (!strcmp(token_table[i]->operator,"WORD"))) {
			leng += 3;
		}
		else if (!strcmp(token_table[i]->operator, "BYTE")) {
			leng += 1;
		}
	}
	return leng;
}
void section_M(int finish_T) {
	for (int i = 0; i<token_line; i++) {
		if (token_table[i]->sec_addr == finish_T) {
			if (strchr(token_table[i]->operator, '+')) { // 오퍼랜드의 첫 번째 글자가 + 라면
				fprintf(object_output, "\nM"); //4형식이므로 M에 값을 나타냄
				token_table[i]->addr += 1;

				fprintf(object_output, "%06X05", token_table[i]->addr);
				fprintf(object_output, "+%s", token_table[i]->operand[0]);

			}
			else if (!strcmp(token_table[i]->operator, "WORD")) {
				if (strstr(token_table[i]->operand[0], "-")) { // 오퍼랜드에 - 가 있을 때
					int loc1 = 0;
					int loc2 = 0;
					bufT = strtok(token_table[i]->operand[0], "-"); // '-'를 기준으로 자른 앞 토큰을 bufT 에 저장
					bufH = strtok(NULL, "-");
					strcpy(token_table[i]->operand[0], bufT);
					strcpy(token_table[i]->operand[1], bufH); // '-'를 기준으로 자른 뒤 토큰을 bufF 에 저장
				}
				//WORD일 때도 M (BUFEND - BUFFER 가 있기에 M에 값을 써줌) 
				fprintf(object_output, "\nM%06X06+%s", token_table[i]->addr, token_table[i]->operand[0]);
				fprintf(object_output, "\nM%06X06-%s", token_table[i]->addr, token_table[i]->operand[1]);


			}
		}
	}

}
