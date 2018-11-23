/************************************************************************
Library Headers
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
/***********************************************************************
Constants
***********************************************************************/
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32
/************************************************************************
Function Declarations
************************************************************************/
char *qstrchr (const char *s, int c);
char *progScanner(FILE *input);
char *qstrcpy (char *dest, const char *src);
char *qstrncpy (char *dest, const char *src, size_t n);
char *regNumberConverter(char *instr);
struct instruction parser(char *inst);
void IF(struct instruction inst);
void ID();
void EX();
void MEM();
void WB();
int getLines (FILE *input);

/***************************************************************************
Enumerations
***************************************************************************/
enum OPCODE {ADD, SUB, ADDI, MUL, LW, SW, BEQ, HALT};
enum REGISTERS {
  ZERO, AT, V0, V1, A0, A1, A2, A3,
  T0, T1, T2, T3, T4, T5, T6, T7,
  S0, S1, S2, S3, S4, S5, S6, S7,
  T8, T9, K0, K1, GP, SP, FP, RA
};
/**************************************************************************
Structures
***************************************************************************/
struct instruction {
    int  opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};

struct latch {
  int opcode;
  int rs, rt, rd, immediate;
  int result;
};

/*************************************************************************
Global Variables
**************************************************************************/
typedef int bool;    // defining boolean values for latches
#define true 0
#define false 1

int pgm_c = 0; //program counter
int sim_cycle = 0; //clock
long stgcycle = 0; //keeps track of cycles in an individual stage
int lines = 0; //number of instructions in IM
struct latch IFID, IDEX, EXMEM, MEMWB; //latches between stages
struct instruction *IMEM; //instruction memory
bool IFID_flag, IDEX_flag, EXMEM_flag, MEMWB_flag,  WB_flag = true; //pipeline latch flags: true is low, false is high
int nops = 0; //keeps track of stalling instructions
long IF_useful, ID_useful, EX_useful, MEM_useful, WB_useful;
double IF_util, ID_util, EX_util, MEM_util, WB_util;
int m; //cycles needed for multiplication in EX
int n; //cycles needed for all other EX operations
int c; //cycles needed to access memory
int mips_reg[REG_NUM]; //array of MIPS registers
int DMEM[2048]; //data memory






int main (int argc, char *argv[]){
	int sim_mode = 0;//mode flag, 1 for single-cycle, 0 for batch
	int i;//for loop counter

	int test_counter = 0;
	FILE *input = NULL;
	FILE *output = NULL;
	printf("The arguments are:");

	for(i = 1;i < argc; i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1]) == 0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1]) == 0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}

		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");

	}

	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input == NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output == NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode == 1){
		for (i = 0; i < REG_NUM; i++){
			mips_reg[i]=0;
		}
	}
  int j = 0;
  int count = 0;
	lines = getLines(input);
  IMEM = malloc(sizeof(struct instruction) * lines);
  fclose(input);
  input = fopen(argv[5], "r");
  char *instruct;
  for(j = 1; j <=lines; j++){
    instruct = progScanner(input);
    instruct = regNumberConverter(instruct);
    IMEM[count] = parser(instruct);
    ++count;
  }

  IF_useful = 0;
  ID_useful = 0;
  EX_useful = 0;
  MEM_useful = 0;
  WB_useful = 0;

  while ((sim_cycle - 4) <= (lines - 1)) {
      if (sim_cycle - 4 >= 0) { WB(); }
      if (sim_cycle - 3 >= 0) { MEM(); }
      if (sim_cycle - 2 >= 0) { EX(); }
      if (sim_cycle - 1 >= 0) { ID(); }
      IF(IMEM[pgm_c / 4]);
      if(sim_mode == 1){
		printf("cycle: %d register value: ",sim_cycle);
		for (i = 1; i < REG_NUM; i++){
			printf("%d  ",mips_reg[i]);
		}
		printf("program counter: %d\n",pgm_c);
		printf("press ENTER to continue\n");
		while(getchar() != '\n');
  }
}
    int usefulcycles = IF_useful + ID_useful + EX_useful + MEM_useful+ WB_useful;
    IF_util = (double) IF_useful / usefulcycles;
    ID_util = (double) ID_useful / usefulcycles;
    EX_util = (double) EX_useful / usefulcycles;
    MEM_util = (double) MEM_useful / usefulcycles;
    WB_util = (double) WB_useful / usefulcycles;

    if(sim_mode == 0){
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
                             IF_util, ID_util, EX_util, MEM_util, WB_util);
                     // add the (double) stage_counter/sim_cycle for each
                     // stage following sequence IF ID EX MEM WB

		fprintf(output,"register values ");
		for (i = 1; i < REG_NUM; i++){
			fprintf(output,"%d  ",mips_reg[i]);
		}
		fprintf(output,"%d\n",pgm_c);

	}
	//close input and output files at the end of the simulation
	fclose(input);
	fclose(output);
	return 0;
}

//finds first instance of a character in a string, implemented
//to work on quark.
char *qstrchr (const char *s, int c) {
  for (; *s != '\0'; ++s)
    if (*s == c)
      return (char *) s;

  return NULL;
}

//copies the source into the destination, implemented to work on quark
char *qstrcpy (char *dest, const char *src) {
  size_t i;
  for (i = 0; src[i] != '\0'; ++i)
    dest[i] = src[i];

  dest[i] = src[i];
  return dest;
}

char *qstrncpy (char *dest, const char *src, size_t n) {
  size_t i;
  for (i = 0; src[i] != '\0'; ++i)
    dest[i] = src[i];

  for (; i < n; ++i)
    dest[i] = '\0';

  return dest;
}

int getLines (FILE *input) {
  char *lp = NULL;
  size_t len = 0;
  int lines = 0;

  while (getline(&lp, &len, input) > 0) {
    if (*lp != '\n') ++lines;
  }
  return lines;
}

char *progScanner(FILE *input){
  char *inst = malloc(100 * sizeof(char)), *p1, *p2;

  if(!fgets(inst, 100, input))
    return (char*) NULL;

//remove new line
  if ((p1 = qstrchr(inst, '\n')))
    *p1 = '\0';

//remove commas and replace with spaces
  p1 = inst;
  while((p1 = qstrchr(p1, ',')))
    *p1 = ' ';

//remove tabs and replace with spaces
  p1 = inst;
  while((p1 = qstrchr(p1, '\t')))
    *p1 = ' ';

//ensure parenthesis are balanced
 int rparent = 0;
  int lparent = 0;
  p1 = inst;
    while(*p1 != '\0'){

        if(*p1 == '(')
          lparent++;

      else if(*p1 == ')')
          rparent++;
          assert(rparent <= lparent);
          ++p1;
    }

    assert(rparent == lparent);

    //convert parenthesiss to spaces
    p1 = inst;
    while((p1 = qstrchr(p1, '(')))
      *p1 = ' ';

    p1 = inst;
    while((p1 = qstrchr(p1, ')')))
      *p1 = ' ';

    //remove all extra spaces from instruction
    p1 = inst;
    while((p1 = qstrchr(p1, ' '))){
      if( p1 == inst || *(p1+1) == ' ' || *(p1+1) == '\0')
        p1 = qstrcpy(p1, p1+1);
        else ++p1;

    }

return inst;
}

//convert register names to equivalent register numbers
/*quark would not accept a loop based implementation due to
persistent segmentation fault */

char *regNumberConverter(char *inst){
  char *p1 = inst;
  while((p1 = qstrchr(p1, '$'))){
    int i;
    ++p1;

    //translates the $zero register
    if(strncmp(p1,"zero",4) == 0){
      *p1++ = '0';
      qstrcpy(p1, p1+3);
    }

    else if(strncmp(p1, "at", 2) == 0){
      *p1++ = '1';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "v0", 2) == 0){
      *p1++ = '2';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "v1", 2) == 0){
      *p1++ = '3';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "a0", 2) == 0){
      *p1++ = '4';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "a1", 2) == 0){
      *p1++ = '5';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "a2", 2) == 0){
      *p1++ = '6';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "a3", 2) == 0){
      *p1++ = '7';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "t0", 2) == 0){
      *p1++ = '8';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "t1", 2) == 0){
      *p1++ = '9';
      qstrcpy(p1, p1+1);
    }

    else if(strncmp(p1, "t2", 2) == 0){
      qstrncpy(p1, "10", 2);
    }

    else if(strncmp(p1, "t3", 2) == 0){
      qstrncpy(p1, "11", 2);
    }

    else if(strncmp(p1, "t4", 2) == 0){
      qstrncpy(p1, "12", 2);
    }

    else if(strncmp(p1, "t5", 2) == 0){
      qstrncpy(p1, "13", 2);
    }

    else if(strncmp(p1, "t6", 2) == 0){
      qstrncpy(p1, "14", 2);
    }

    else if(strncmp(p1, "t7", 2) == 0){
      qstrncpy(p1, "15", 2);
    }

    else if(strncmp(p1, "s0", 2) == 0){
      qstrncpy(p1, "16", 2);
    }

    else if(strncmp(p1, "s1", 2) == 0){
      qstrncpy(p1, "17", 2);
    }

    else if(strncmp(p1, "s2", 2) == 0){
      qstrncpy(p1, "18", 2);
    }

    else if(strncmp(p1, "s3", 2) == 0){
      qstrncpy(p1, "19", 2);
    }

    else if(strncmp(p1, "s4", 2) == 0){
      qstrncpy(p1, "20", 2);
    }

    else if(strncmp(p1, "s5", 2) == 0){
      qstrncpy(p1, "21", 2);
    }

    else if(strncmp(p1, "s6", 2) == 0){
      qstrncpy(p1, "22", 2);
    }

    else if(strncmp(p1, "s7", 2) == 0){
      qstrncpy(p1, "23", 2);
    }

    else if(strncmp(p1, "t8", 2) == 0){
      qstrncpy(p1, "24", 2);
    }

    else if(strncmp(p1, "t9", 2) == 0){
      qstrncpy(p1,"25", 2);
    }

    else if(strncmp(p1, "k0", 2) == 0){
      qstrncpy(p1, "26", 2);
    }

    else if(strncmp(p1, "k1", 2) == 0){
      qstrncpy(p1, "27", 2);
    }

    else if(strncmp(p1, "gp", 2) == 0){
      qstrncpy(p1, "28", 2);
    }

    else if(strncmp(p1, "sp", 2) == 0){
      qstrncpy(p1, "29", 2);
    }

    else if(strncmp(p1, "fp", 2) == 0){
      qstrncpy(p1, "30", 2);
    }
    else if(strncmp(p1, "ra", 2) == 0){
      qstrncpy(p1, "31", 2);
    }


  }
  return inst;
}

struct instruction parser(char *inst){
  char **tokens = malloc(sizeof(char*) * 4);
  int count = 0;
  char *lp, *rp;
  rp = qstrchr(inst, ' ');
  lp = inst;
  struct instruction output;

  do{
    char *str = malloc(sizeof(char) * (rp - lp + 1));
    qstrncpy(str, inst, rp - lp);
    str[rp - lp] = '\0';
    tokens[count] = str;
    inst = rp + 1;
    ++count;
  }
    while((rp = qstrchr(inst, ' ')));
    tokens[count++] = inst;

    assert(count == 4);
    assert(tokens[1][0] == '$');
    tokens[1] = &tokens[1][1];
if(strcmp(tokens[0], "add") == 0 || strcmp(tokens[0], "sub") == 0 ||strcmp(tokens[0], "mul") == 0){
    if(strcmp(tokens[0], "add") == 0)
      output.opcode = ADD;

    else if(strcmp(tokens[0], "sub") == 0)
      output.opcode = SUB;

    else if(strcmp(tokens[0], "mul") == 0)
      output.opcode = MUL;

    output.rd = atoi(tokens[1]);

    assert(tokens[2][0] == '$');
    output.rs = atoi(&tokens[2][1]);
    assert(tokens[3][0] == '$');
    output.rt = atoi(&tokens[3][1]);
  }

  else if(strcmp(tokens[0], "beq") == 0){
    output.opcode = BEQ;
    output.rs = atoi(tokens[1]);
    assert(tokens[2][0] == '$');
    output.rt = atoi(&tokens[2][1]);
    output.immediate = atoi(tokens[3]);

    assert(output.immediate >= 0);
    assert(output.immediate < 0xFFFF);

  }

 else if(strcmp(tokens[0], "addi") == 0){
   output.opcode = ADDI;
   output.rt = atoi(tokens[1]);
   assert(tokens[2][0] == '$');
   output.rs = atoi(&tokens[2][1]);
   output.immediate = atoi(tokens[3]);
   assert(output.immediate >= 0);
   assert(output.immediate < 0xFFFF);


 }

 else if((strcmp(tokens[0], "lw") == 0) || (strcmp(tokens[0], "sw") == 0)){
   if(strcmp(tokens[0], "lw") == 0)
     output.opcode = LW;

    if(strcmp(tokens[0], "sw") == 0)
      output.opcode = SW;

    output.rt = atoi(tokens[1]);

    assert(tokens[2][0] != '$');
    output.immediate = atoi(tokens[2]);

    assert(tokens[3][0] == '$');
    output.rs = atoi(&tokens[3][1]);

    assert(output.immediate >= 0);
    assert(output.immediate < 0xFFFF);




 }

  return output;
}

void IF(struct instruction inst){
  if((sim_cycle <= lines) && IFID_flag == true){
    if(c == stgcycle){      //fetching an instruction from memory takes c cycles
      if(nops > 0){          //if there are stalls in the pipeline... wait
        IFID.opcode = HALT;
        IFID.immediate = 0;
        IFID.rd = 0;
        IFID.rt = 0;
        --nops;
      }
      else{ //if no stalls in the pipeline store contents of instruction in latch
        IFID.opcode = inst.opcode;
        IFID.immediate = inst.immediate;
        IFID.rs = inst.rs;
        IFID.rt = inst.rt;
        IFID.rd = inst.rd;
        ++IF_useful;  //this is a useful cycle
        pgm_c +=4;   //increment pc by four
      }
      ++sim_cycle; //increment simulation cycles
      stgcycle = 0; //re-set stage cycles
      IFID_flag = false; //set the IFID latch flag high
      WB_flag = true; //set WB flag low

    }
    else ++stgcycle;
  }
}

void ID(){
  if((sim_cycle - 1 <= lines) && IDEX_flag == true){
    if(stgcycle == 1){
      if(IFID.opcode == ADD || IFID.opcode == ADDI || IFID.opcode == BEQ ||
      IFID.opcode == SUB || IFID.opcode == MUL || IFID.opcode == LW){
        nops = 3;
        lines += 3;
      }
      ++ID_useful;
      IDEX.opcode = IFID.opcode;
      IDEX.rs = IFID.rs;
      IDEX.immediate = IFID.immediate;
      IDEX.rd = IFID.rd;
      IDEX.rs = IFID.rs;
      IDEX_flag = false;
      IFID_flag = true;
      stgcycle = 0;
      if(sim_cycle >= lines){
        ++sim_cycle;
        WB_flag = true;
      }
    }
    else ++stgcycle;
  }
}

void EX(){
  if((sim_cycle - 2 <= lines) && EXMEM_flag == true){
    if(stgcycle == m){ //indicates a multiplication
      if((IDEX.opcode = MUL)){
        EXMEM.result = mips_reg[IDEX.rs] * mips_reg[IDEX.rt];
        EXMEM.opcode = IDEX.opcode;
        EXMEM.immediate = IDEX.immediate;
        EXMEM.rs = IDEX.rs;
        EXMEM.rt = IDEX.rt;
        EXMEM.rd = IDEX.rd;
        ++EX_useful; //this was a useful cycle
        IDEX_flag = true;
        stgcycle = 0;
        EXMEM_flag = false;
        if(sim_cycle >= lines + 1){
          ++sim_cycle;
          WB_flag = true;
      }
      return;
    }
  }

  if(stgcycle == n) { //indicates a non multiplication
    EXMEM.opcode = IDEX.opcode;
    EXMEM.rd = IDEX.rd;
    EXMEM.rs = IDEX.rs;
    EXMEM.rt =IDEX.rt;
    EXMEM.immediate = IDEX.immediate;
    if(EXMEM.opcode != HALT)
      ++EX_useful;

    switch(IDEX.opcode){

      case HALT:
      break;

      case ADD:
      EXMEM.result = mips_reg[EXMEM.rs] + mips_reg[EXMEM.rt];
      break;

      case SUB:
      EXMEM.result = mips_reg[EXMEM.rs] - mips_reg[EXMEM.rt];
      break;

      case ADDI:
      EXMEM.result = mips_reg[EXMEM.rs] + EXMEM.immediate;
      break;

      case LW:
      EXMEM.result = mips_reg[EXMEM.rs] + EXMEM.immediate;
      break;

      case SW:
      EXMEM.result = mips_reg[EXMEM.rs] + EXMEM.immediate;
      break;

      case BEQ:
      EXMEM.result = mips_reg[EXMEM.rs] == mips_reg[EXMEM.rt];
      if(EXMEM.result && EXMEM.opcode == BEQ){
        pgm_c += 4 * EXMEM.immediate;
      }
      break;


    }

    IDEX_flag = true;
    EXMEM_flag = false;
    stgcycle = 0;

      if(sim_cycle >= lines+1){
        ++sim_cycle;
        WB_flag = true;
      }
    return;
    }
  ++stgcycle;
  }
}

void MEM(){
  if((sim_cycle - 3 <= lines) && MEMWB_flag == true){
    if (c == stgcycle){
      MEMWB.opcode = EXMEM.opcode;
      MEMWB.rs = EXMEM.rs;
      MEMWB.rd = EXMEM.rd;
      MEMWB.rt = EXMEM.rt;
      MEMWB.result = EXMEM.result;
      MEMWB.immediate = EXMEM.immediate;
      if(EXMEM.opcode == LW){
        mips_reg[EXMEM.rt] = DMEM[EXMEM.result];
        ++MEM_useful;
      }

      else if(EXMEM.opcode == SW){
        DMEM[EXMEM.result]= mips_reg[EXMEM.rt];
        ++MEM_useful;
      }

      WB_flag = false;
      EXMEM_flag = true;
      MEMWB_flag = false;
      stgcycle = 0;

      if(sim_cycle >= lines + 2){
        ++sim_cycle;
        WB_flag = true;
      }
    }
    else ++stgcycle;
  }
}

void WB(){
  if((sim_cycle - 4 <= lines) && WB_flag == true){
    if(stgcycle == 1){
      if(MEMWB.opcode == HALT || MEMWB.opcode == BEQ){}

      else if(MEMWB.opcode == ADD || MEMWB.opcode == SUB || MEMWB.opcode == MUL){
        mips_reg[MEMWB.rt] = MEMWB.result;
        ++WB_useful;
      }
      else if(MEMWB.opcode == ADDI){
        mips_reg[MEMWB.rt] = MEMWB.result;
        ++WB_useful;
      }
       stgcycle = 0;
       WB_flag = false;
       MEMWB_flag = true;
       if(sim_cycle >= lines + 3){
         ++sim_cycle;
         WB_flag = true;
       }
       else ++stgcycle;
    }
  }
}
