/************************************************************************
Library Headers
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
/************************************************************************
Function Declarations
************************************************************************/
char *qstrchr (const char *s, int c);
char *progScanner(FILE *input);
char *qstrcpy (char *dest, const char *src);
char *qstrncpy (char *dest, const char *src, size_t n);
char *regNumberConverter(char *instr);
struct instruction parser(char *inst);

/***************************************************************************
Enumerations
***************************************************************************/
enum OPCODE {ADD, SUB, ADDI, MUL, LW, SW, BEQ, HALT};

/**************************************************************************
Structures
***************************************************************************/
struct instruction {
    enum OPCODE opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};




int main(int argc, char **argv){
  FILE *input = fopen("/Users/rickbrownscomputer/Desktop/inst.txt", "r");
  char *inst;
  char *inst2;
  struct instruction inst3;
  inst = progScanner(input);
  inst2 = regNumberConverter(inst);
  inst3 = parser(inst2);

  printf("%d", inst3.opcode);



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
char *regNumberConverter(char *inst){
  char *p1 = inst;
  /*char *regNames[] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0",
  "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3",
  "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};*/
/*  char *regNumbers[] = {"0","1", "2", "3", "4", "5", "6", "7","8", "9",
  "10", "11", "12", "13", "14", "15", "16", "17", "18",
  "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "31", "32"}; */

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

 else if(strcmp(tokens[0], "lw") == 0 || strcmp(tokens[0], "sw") == 0){
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
