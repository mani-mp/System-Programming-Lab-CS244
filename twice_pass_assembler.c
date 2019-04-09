//two_pass_assembler.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


char line[256], label[8], opcode[8], operand[8], operand2[8], code[8];
char line_mem[100] = {0000}, instr_count, count_reg[8] = "R31", accumulator[8]="R1";
int memory=0;

/*This function converts a decimal number to binary*/
void printBits(size_t const size, void const * const ptr, bool isbyte) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    if(isbyte == true){ /*If isbyte is true, size is in bytes, else size is in bits*/
	    for (i=size-1;i>=0;i--){
	        for (j=7;j>=0;j--){
	            byte = (b[i] >> j) & 1;
	            printf("%u", byte);
	        }
	    }
	}	
	else{
		for (i=size/8;i>=0;i--){
	        for (j=7;j>=0;j--){
	            byte = (b[i] >> j) & 1;
	            if(i==0){
	            	if(j<size%9)
	            		printf("%u", byte);
	        	}
	        	else
	        		printf("%u", byte);
	        }
	    }
	}
    printf(" ");
}

/*This function converts hexdecimal numbers to decimals*/
int hextodec(char *hexVal) {    
    int len = strlen(hexVal); 
    int base = 1;
    int dec_val = 0;
    for (int i=len-1; i>=0; i--) 
    {    
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base;  
            base = base * 16; 
        }
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base;  
            base = base*16; 
        } 
        else if (hexVal[i]>='a' && hexVal[i]<='f') 
        { 
            dec_val += (hexVal[i] - 87)*base;  
            base = base*16; 
        } 
    }     
    return dec_val; 
} 

/*this function removes extra characters from string*/
char* strip(char *str){
	int i=0, j;
	while(str[i]!='\0'){
		if (str[i]==','||str[i]=='\n'||str[i]=='\t'||str[i]==' '){
			j=i;
			while(str[j]!='\0'){
				str[j] = str[j+1];
				j++;
			}
		}
		else{
			i++;
		}
	}
	return str;
}

/*function to check if string is register*/
bool isregister(char *str){
	return str[0] == 'R';
}

/*function to check if string is label*/
bool islabel(char *str){
	int i;
	for (i = 0; i < sizeof(str)-1; ++i){
		if(str[i]==':')
			return true;
	}	
	
	return false;
}

/*divides string into components - labels, opcodes, hex numbers*/
void split_line(){
	char buff[8], word1[8], word2[8], word3[8], word4[8];
	int i,j=0,count=0;
	label[0]=opcode[0]=operand[0]=operand2[0]='\0';
	word1[0]=word2[0]=word3[0]=word4[0]='\0';
	for(i=0;line[i]!='\0';i++){
		if(line[i]!=' ')
		buff[j++]=line[i];
		else
		{
		buff[j]='\0';
		strcpy(word4,word3);
		strcpy(word3,word2);
		strcpy(word2,word1);
		strcpy(word1,buff);
		j=0;
		count++;
		}
	}
	buff[j]='\0';
	strcpy(word4,word3);
	strcpy(word3,word2);
	strcpy(word2,word1);
	strcpy(word1,buff);
	
	switch(count) //switch case to decide types of words
	{
	case 0:
	if(islabel(word1))
		strcpy(label, word1);
	else
		strcpy(opcode,word1);
	break;

	case 1:
	if(islabel(word2)){
		strcpy(label, word2);
		strcpy(opcode, word1);
	}
	else{
		strcpy(opcode,word2);
		strcpy(operand,word1);
	}
	break;

	case 2:
	if (islabel(word3))
	{
		strcpy(label,word3);
		strcpy(opcode,word2);
		strcpy(operand,word1);
	}	
	else{
		strcpy(opcode,word3);
		strcpy(operand,word2);
		strcpy(operand2,word1);
	}
	break;

	case 3:
	strcpy(label, word4);
	strcpy(opcode, word3);
	strcpy(operand, word2);
	strcpy(operand2, word1);
	break;
	
	default:
	printf("Too many operands\n");
	exit(0);
	}
}

/*Extracts the memory location corresponding to label from symbol Table*/
char* extract_symbol(char *symb){
	char buff[8];
	int i, j=0;
	FILE *sym_fpr = fopen("symTable.txt", "r");
	while(fgets(line, sizeof(line), sym_fpr)!=NULL){
		for(i=0;line[i]!=' ';i++){
			buff[i]=line[i];	
		}	
		buff[i++]='\0';
		if(strcmp(buff, symb)==0){
			while(line[i]!='\0'){
				code[j++]=line[i++];	
			}
			code[j] = '\0';
			return strip(code);
		}
	}	
	return NULL;
}

/*Extracts the operation code corresponding to operation label from op Table*/
char* extract_opcode(char *opcd, char* filename){
	char buff[8];
	int i, j=0;
	FILE *op_fpr = fopen(filename, "r");
	instr_count = 0;
	while(fgets(line, sizeof(line), op_fpr)!=NULL){
		for(i=0;line[i]!=' ';i++){
			buff[i]=line[i];	
		}	
		buff[i++]='\0';
		if(strcmp(buff, opcd)==0){
			while(line[i]!='\0'){
				code[j++]=line[i++];	
			}
			code[j] = '\0';
			return strip(code);
		}
		instr_count++;
	}
	return NULL;
}

/*Extracts pseudo opcodes*/
char* extract_pseudop(char *psop){
	int i;
	FILE *psop_fpr = fopen("pseudoplookupTable.txt", "r");
	while(fgets(line, sizeof(line), psop_fpr)!=NULL){
		for(i=0;line[i]!=' ';i++){
			code[i]=line[i];	
		}	
		code[i]='\0';
		if(strcmp(code, psop)==0){
			return strip(code);
		}
	}
	return NULL;
}

/*Function which reads assembly file and creates symbol and op Tables*/
void pass_one(){
	//declaration of file object and other variables
	FILE *fp = fopen("test.txt", "r");
	FILE *sym_fp = fopen("symTable.txt", "w");
	FILE *op_fp = fopen("opTable.txt", "w");
	FILE *op_fpr = fopen("oplookupTable.txt", "r");
	int bits, bytes, op_in[15] = {0}, line_count = 0;	
	memory = 0;
	fgets(line, sizeof(line), fp);
	if(strcmp(strip(line), "START")!=0){
		printf("Syntax Error: Program must start with START\n");
		exit(0);
	}

	//reads all lines of program
	while(fgets(line, sizeof(line), fp)!=NULL){
		split_line();//splits sentence into elements
	
		line_mem[line_count++] = memory;
		bits=0;
		if (label[0]!='\0'){
			fprintf(sym_fp, "%s %04x\n", label, memory);
		}

		//calculation of memory location of each sentence in program
		if(opcode[0]!='\0'){
			if (strcmp(strip(opcode), "LOOP")==0){
				memory += 25/8 + 1;
				line_mem[line_count++] = memory;
				bits += 4;
			}
			else if (strcmp(strip(opcode), "MUL")==0){
				bits += 9;
			}
			else{
				bits += 4;
			}
		}
		if(operand[0]!='\0'){
			if(islabel(operand)){
				bits += 16;
			}
			if(isregister(operand)){
				bits+=5;
			}
			else{
				bits +=16;
			}
		}
		if(operand2[0]!='\0'){
			if(isregister(operand2)){
				bits+=5;
			}
			else{
				bits +=16;
			}
		}
		if(bits%8==0)
			memory += bits/8;
		else
			memory += bits/8 + 1;
		
		if (opcode!='\0'){
			//checks whether opcode is valid
			if(strcmp(strip(opcode), "LOOP")==0){
				extract_opcode("SUB", "oplookupTable.txt");
				if(op_in[instr_count]==0){
			 		fprintf(op_fp, "%s %s\n", "SUB", code);
			 		op_in[instr_count] = 1;
			 	}
			 	extract_opcode("JNZ", "oplookupTable.txt");
			 	if(op_in[instr_count]==0){
			 		fprintf(op_fp, "%s %s\n", "JNZ", code);
			 		op_in[instr_count] = 1;
			 	}
			}
			else if(extract_opcode(strip(opcode), "oplookupTable.txt")!=NULL){
				if(op_in[instr_count]==0){
			 		fprintf(op_fp, "%s %s\n", opcode, code);
			 		op_in[instr_count] = 1;
			 	}
			}
			else if(strcmp(strip(opcode), "END")==0){
				break;
			}
			else{
				 	printf("Invalid opcode\n");
					exit(0);
			}
		}
	}
	fclose(op_fpr);
	fclose(op_fp);
	fclose(sym_fp);
	fclose(fp);
}

/*Function which converts assembly code to machine language with help of symbol and op Tables*/
void pass_two(){
	//declaration of file object and other variables
	FILE *fp = fopen("test.txt", "r");
	printf("ADDRESS INSTRUCTION\n");
	int line_count = 0, num_operand=0;
	char operand_new[8];

	fgets(line, sizeof(line), fp); 
	if(strcmp(strip(line), "START")!=0){
		printf("Syntax Error: Program must start with START\n");
		exit(0);
	}
	
	//reads all lines of program
	while(fgets(line, sizeof(line), fp)!=NULL){

		split_line();//splits program into elements
		if(strcmp(strip(opcode), "END")!=0){
			printf("%04x    ", line_mem[line_count++]);
		}
		if(opcode[0]!='\0'){
			if (strcmp(strip(opcode), "LOOP")==0){
				printf("%4s ", extract_opcode("SUB", "opTable.txt"));
				sscanf(count_reg, "%*[^0123456789]%d", &num_operand);
				printBits(5, &num_operand, 0);
				num_operand = hextodec("0001");
				printBits(2, &num_operand, 1);
				printf("\n");
				printf("%04x    ", line_mem[line_count++]);
				printf("%4s ", extract_opcode("JNZ", "opTable.txt"));
			}
			else if(strcmp(strip(opcode), "MUL")==0){
				printf("%4s ", extract_opcode(strip(opcode), "opTable.txt"));
				sscanf(accumulator, "%*[^0123456789]%d", &num_operand);
				printBits(5, &num_operand, 0);
			}
			else if(strcmp(strip(opcode), "END")==0){
				break;
			}
			else{
				printf("%4s ", extract_opcode(strip(opcode), "opTable.txt"));
			}

		}
		if(operand[0]!='\0'){
			strcpy(operand_new, strip(operand));
			strcat(operand_new, ":");
			if(extract_symbol(operand_new)!=NULL){
				strcat(operand, ":");
				num_operand = hextodec(extract_symbol(operand));
				printBits(2,&num_operand,1);
			}
			else if(isregister(operand)){
				sscanf(operand, "%*[^0123456789]%d", &num_operand);
				printBits(5, &num_operand, 0);

			}
			else{
				operand[sizeof(operand)-1] = '\0';
				num_operand = hextodec(operand);
				printBits(2, &num_operand, 1);
			}
		}
		if(operand2[0]!='\0'){
			if(isregister(operand2)){
				sscanf(operand2, "%*[^0123456789]%d", &num_operand);
				printBits(5, &num_operand, 0);
			}
			else{
				operand2[sizeof(operand2)-1] = '\0';
				num_operand = hextodec(operand2);
				printBits(2, &num_operand, 1);

			}
		}
		printf("\n");
	}
	fclose(fp);

}

int main(){
	freopen("output.o", "a+", stdout);
	pass_one();//runs first pass of assembler
	pass_two();//runs second pass of assembler
	return 0;
}