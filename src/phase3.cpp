/* Task 3: Pipelining

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of RISCV Processor

Developer's Name: 
Abhishek Balu: 2018csb1065
Dilpreet Singh: 2018csb1085
Nikhil: 2018csb1108
Rasanpreet Singh: 2018csb1173

Date:28-04-2020
*/

#include <bits/stdc++.h>
using namespace std;

#define lli long long int
#define FETCH_STAGE 1
#define DECODE_STAGE 2
#define EXECUTE_STAGE 3
#define MEM_STAGE 4
#define WB_STAGE 5

#define OPCODE_S1 35 //for sd, sw, sl, sh

#define NO_DATA_DEPEND 0
#define DATA_DEPEND_RA 1
#define DATA_DEPEND_RB 2
#define DATA_DEPEND_RA_RB 3
#define DATA_DEPENED_MtoM 4

#define OFF 0
#define ON 1
#define TRUE 0
#define FALSE 1

#define OPCODE_R1 51 //for add, sub, and etc
#define OPCODE_R2 59 // for addw, subw etc

#define OPCODE_SB1 99 //for branch jump

#define OPCODE_UJ 111 //for jal

#define OPCODE_U1 23 // for auipc
#define OPCODE_U2 55 // for lui

#define OPCODE_I1 3   //for load
#define OPCODE_I2 19  // for shift, ori, andi
#define OPCODE_I3 27  // for shiftw and addiw
#define OPCODE_I4 103 //for jalr
#define select 1

unsigned char memory[1 << 24]; //Processor Memory
int regArray[32] = {0};

int cycleCount,total_instructions;
double CPI;

int stalls;
int stalls_data_hazard;
int stalls_control_hazard;

int data_hazard;
int control_hazard;
int branch_mispredictions;

int isLoadInstruction;
int isPrevLoadInstruction ;
int aluInstructions;
int controlInstructions;
int dataTransferInstructions;

bool knob1, knob2, knob3, knob4, knob5;
int n, m, k;

int readWriteMemory(int MEM_READ, int MEM_WRITE, int address = 0, int data_w = 0)
{
	if(MEM_READ > 0)
	{
	    if (MEM_READ == 1 && select == 1)   //lb
			return memory[address];
	    
		int data = memory[address];
		    data = data + ((int)memory[address + 1] << 8);

	    if (MEM_READ == 2)   //lw
             return data;
		
		else if (MEM_READ == 3)   //lh
		 {   
			data = data + ((int)memory[address + 2] << 16);
			data = data + ((int)memory[address + 3] << 24);
			return data;
		}
		
	}
	if (MEM_WRITE == 1)  //sb
	   memory[address] = data_w;
	
	else if (MEM_WRITE == 2 && select == 1)  //sh
	{   int setb8 = (1 << 8) - 1;
		memory[address] = data_w & setb8;
		memory[address + 1] = data_w >> 8;
	}
	else if (MEM_WRITE == 3)  //sw
	{ 
		int setb8 = (1 << 8) - 1;
		memory[address] = data_w & setb8;
		memory[address + 1] = (data_w & (setb8 << 8)) >> 8;
		memory[address + 2] = (data_w & (setb8 << 16)) >> 16;
		memory[address + 3] = data_w >> 24;
	}
	return 0;
}  
//End of readWriteMemory


//Print stats in stats.txt file
void stats_print()
{   stalls = stalls_control_hazard + stalls_data_hazard;
	total_instructions = aluInstructions + dataTransferInstructions + controlInstructions;
	control_hazard = controlInstructions;
	CPI = (double)cycleCount / (double)total_instructions;
	fstream fileWriting;
	fileWriting.open("stats.txt", ios::out);
	fileWriting << "----------------------------------------------------------------------" << endl;
	fileWriting << "1.Total Number Of Cycles                 :  " << cycleCount << endl;
	fileWriting << "2.Total Instructions Executed            :  " << total_instructions << endl;
	fileWriting << "3.CPI                                    :  " << CPI << endl << endl;
	fileWriting << "4.Number of Data-Transfer Instructions   :  " << dataTransferInstructions << endl;
	fileWriting << "5.Number of ALU Instruction              :  " << aluInstructions << endl;
	fileWriting << "6.Number of Control Instructions         :  " << controlInstructions << endl << endl;
	fileWriting << "7.Total Stalls                           :  " << stalls << endl << endl;
	fileWriting << "8.Number of Data Hazards                 :  " << data_hazard << endl;
	fileWriting << "9.Number of Control Hazards              :  " << control_hazard << endl << endl;
	fileWriting << "10.Number of Branch Mis-predictions      :  " << branch_mispredictions << endl;
	fileWriting << "11.Number of Stalls due Data Hazard      :  " << stalls_data_hazard << endl;
	fileWriting << "12.Number of Stalls due Control Hazard   :  " << stalls_control_hazard << endl;
	fileWriting << "----------------------------------------------------------------------" << endl;
	
	fileWriting.close();
}
//End of stats_print()

//Prints all register file & their value
void printRegisterFile()
{
	fstream fileWriting;
	fileWriting.open("registerFile.txt", ios::app);
	fileWriting << "----------------------------------------------------------------------" << endl;
	fileWriting << "CYCLE NUMBER\t\t:\t" << cycleCount << endl;
	int i=0;
	while(i < 10){ 
		fileWriting << "REGISTER x" << i << "\t\t\t:\t" << regArray[i] << endl;
		i++;
	}
	while(i < 32){
		fileWriting << "REGISTER x" << i << "\t\t:\t" << regArray[i] << endl;
		i++;
	}
	fileWriting << "----------------------------------------------------------------------" << endl;

	fileWriting.close();
}
//Check data dependency Execute to Execute

//Prints memory that has been alloted with data or instruction!
void printMemory()
{
	fstream fileWriting;
	fileWriting.open("memory.txt", ios::out);
	fileWriting << "----------------------------------------------------------------------" << endl;
	int i = 0;
	while(i < 1 << 22){ 
		if (memory[i] != '\0')
			fileWriting << i << "\t" << (int)memory[i] << endl;
		i++;
	}	
	fileWriting << "----------------------------------------------------------------------" << endl;
	fileWriting.close();
}

// Pipelined

class pipelined
{
  public:
	struct Buffer_IF_ID
	{
		int en2,IR;
		bool en;
		unsigned int PC;
		
		Buffer_IF_ID()
		{
			IR = 0;
			PC = 0;
			en = 0;
			en2 = 1;
		}
	};

	struct Buffer_ID_EX
	{
		int RA,RB,RZ;  // for branch instructions
	    int addressA, addressB;
		int addressC;
		int immediate;
		int ALU_OP, B_SELECT, PC_SELECT, INC_SELECT, Y_SELECT;
		unsigned int PC;
		int MEM_READ,MEM_WRITE,RF_WRITE;
		bool isBranchInstruction, branchTaken, isLoad, isStore, isJAL_JALR, isALU;
		unsigned int returnAddress;
		int en2;
		bool en;
		Buffer_ID_EX()
		{
			en = 0;
			en2 = 1;
			isStore = FALSE;
			isLoad = FALSE;
			isJAL_JALR = FALSE;
			isALU = FALSE;
			branchTaken = FALSE;
			isBranchInstruction = FALSE;
		}
	};

	struct Buffer_EX_MEM
	{
		unsigned int PC;
		int RA,RB,RZ;
	    int addressA, addressB;
		int addressC;
		int immediate;
		int PC_SELECT, INC_SELECT, Y_SELECT;
		int MEM_READ, MEM_WRITE, RF_WRITE;
		bool isBranchInstruction, branchTaken, isStore, isLoad, isALU, isJAL_JALR;
		unsigned int returnAddress;
		int en2;
		bool en;
		
		Buffer_EX_MEM()
		{
			isStore = FALSE;
			isLoad = FALSE;
			isALU = FALSE;
			isJAL_JALR = FALSE;
			branchTaken = FALSE;
			isBranchInstruction = FALSE;
			en2 = 1;
			en = 0;
			
		}
	};

	struct Buffer_MEM_WB
	{
		unsigned int PC;
		int addressC;
		int RF_WRITE;
		int RY;
		int en2;
		bool en;
		bool isBranchInstruction, branchTaken, isALU, isLoad, isStore, isJAL_JALR;
		Buffer_MEM_WB()
		{
			isALU = FALSE;
			isLoad = FALSE;
			isStore = FALSE;
			isJAL_JALR = FALSE;
			isBranchInstruction = FALSE;
			branchTaken = FALSE;
			en = 0;
			en2 = 1;
		}
	};
    Buffer_MEM_WB buffer_MEM_WB;
	Buffer_IF_ID buffer_IF_ID;
	Buffer_EX_MEM buffer_EX_MEM;
	Buffer_ID_EX buffer_ID_EX;
	
	int PC_of_stalledStageEtoE= INT_MAX;
	int PC_of_stalledStageMtoE = INT_MAX;
	int PC_of_stalledStageMtoM = INT_MAX;
	int prevPC = -1;

//Call in decode stage & Writeback Stage
	void readWriteRegFile(int stage)
	{
		if (select==1)
		{
			if (stage == WB_STAGE)
				{
					int a1=buffer_MEM_WB.RF_WRITE;
					if(a1==1)
					{
					  if(buffer_MEM_WB.addressC)
					  regArray[buffer_MEM_WB.addressC]=buffer_MEM_WB.RY;
					}
				}
			
			if (stage == DECODE_STAGE)
			{
				int a3=buffer_ID_EX.addressA;
				int a4=buffer_ID_EX.addressB;
				if(a3<32)
				    buffer_ID_EX.RA=regArray[a3];
				if(a4<32)
				   buffer_ID_EX.RB=regArray[a4];
			}
		}
	}

	/*Instruction Address Generator
    returns returnAddress*/
	
  lli iag(int stage, lli immediate = 0, int INC_SELECT = 0, int PC_SELECT = 0)
	{
		if (stage == FETCH_STAGE && select == 1)  //call from fetch
		   return buffer_IF_ID.PC + 4;
		

		if (stage == DECODE_STAGE)   //call from decode
		{ 
			lli PC_Temp = buffer_ID_EX.PC + 4;
			
		   if(PC_SELECT !=0)
			 {
			   if (INC_SELECT == 1)
				  buffer_ID_EX.PC = buffer_ID_EX.PC + immediate;
			   else
				  buffer_ID_EX.PC = buffer_ID_EX.PC + 4;
			 }
			else
			   buffer_ID_EX.PC = buffer_ID_EX.RZ;
			
			return PC_Temp;
		}
		
		if (stage == EXECUTE_STAGE && select==1)
		{ //call from alu
			lli PC_Temp = buffer_EX_MEM.PC;
			
			if(PC_SELECT != 0)
			 {
			    if(INC_SELECT == 1)
				   buffer_EX_MEM.PC = buffer_EX_MEM.PC + immediate;
				else
					buffer_EX_MEM.PC = buffer_EX_MEM.PC + 4;
			 }
		  else
			 buffer_EX_MEM.PC = buffer_EX_MEM.RZ + 4;
				
			return PC_Temp;
		}
	}
	//End of function iag

	//HERE STARTS THE STAGE 1=== "FETCH STAGE"
	void fetch(bool en)
	{
		buffer_IF_ID.en = en;
		
		if (select==1 && en == 0)
		     return;
		
       buffer_IF_ID.IR = readWriteMemory(3, 0, buffer_IF_ID.PC);
	   buffer_IF_ID.PC = iag(FETCH_STAGE);
	}//End of fetch
	

// HERE IS THE STAGE 2 
//STAGE 2 == DECODE STAGE "RA and RB will get their values "
	void decode()
	{
		if (buffer_IF_ID.en == 0)
			return;

		buffer_ID_EX.isLoad = FALSE;
        buffer_ID_EX.branchTaken = FALSE;
		buffer_ID_EX.isBranchInstruction = FALSE;
		buffer_ID_EX.isJAL_JALR = FALSE;
		buffer_ID_EX.isALU = FALSE;
		buffer_ID_EX.isStore = FALSE;
		

        unsigned int returnAddress; //Return Address in case of jal/jalr
		int addressA = 0;
		int  addressB = 0 ;
		int addressC;
		int IR = buffer_IF_ID.IR;
		unsigned int PC = buffer_IF_ID.PC;
		int immediate; // for immediate values
		int ALU_OP, B_SELECT, PC_SELECT=1, INC_SELECT=0, Y_SELECT=0;
		int MEM_READ, MEM_WRITE, RF_WRITE;

		unsigned int opcode = IR << 25;
		opcode >>= 25;
		unsigned int funct3 = IR << 17;
		funct3 >>= 29;
		unsigned int funct7;
		unsigned int a3 = IR >> 25;
		funct7=a3;
		isLoadInstruction = 0;
		if (opcode == OPCODE_I1 || opcode==OPCODE_I2 ||opcode==OPCODE_I3||opcode==OPCODE_I4)
		{
			if (opcode==OPCODE_I1)
			{
			 RF_WRITE = 1;
	         buffer_ID_EX.isLoad = TRUE;
			 isLoadInstruction = 1;
			 int b1= IR >> 20;
			 int imm = b1;
			immediate = imm;
			unsigned int b2 = IR<<12;
			b2 >>= 27;
			unsigned int rs1 = b2;
			addressA = rs1;
			unsigned int b3 = IR<<20;
			b3 >>= 27;
			unsigned int rd = b3;
			addressC = rd;
			B_SELECT = 1;
			ALU_OP = 0;
			Y_SELECT = 1;
			
			if (funct3 != 0)
			{
                if (funct3 == 1 && select ==1)
				MEM_READ = 2;
			 else if (funct3 == 2 && select==1)
				MEM_READ = 3;
			 else if (funct3 == 3 && select==1)
				MEM_READ = 4;
			}
			else if (funct3==0)
			    MEM_READ = 1;
		
			MEM_WRITE = 0;
			}

		else if (opcode == OPCODE_I2)
	 {
	    buffer_ID_EX.isALU = TRUE;
	    RF_WRITE = 1;
	    unsigned int od1,od2,od3;
        int imm;
		od1 = IR >> 20;
		imm=od1;
		immediate=imm;
        unsigned int rs1;
		od2 = IR << 12;
		rs1=od2;
        rs1 = rs1 >> 27;
		addressA = rs1;
        unsigned int rd;
		od3 = IR << 20;
		rd = od3;
        rd >>= 27;
		addressC = rd;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        if (funct3 != 0)
     {
          if (funct3 == 1 && select==1) //slli
        {
            unsigned int st = IR << 7;
            st = st >> 27;
            immediate = st;
            ALU_OP = 7;
        }

        else if (funct3 == 2) //slti
        {
			if (select==1)
            ALU_OP = 20;
        }

        else if (funct3 == 3) //sltiu
            ALU_OP = 20;
        
        else if (funct3 == 4) //xori
            ALU_OP = 8;
        
        else if (funct3 == 5) //srli
        {   unsigned int f1;
            unsigned int shamt;
			f1 = IR << 7;
			shamt=f1;
            shamt >>= 27;
            immediate = shamt;
            if (funct7 != 0)
                ALU_OP = 21;
            else
                ALU_OP = 19;
        }
        else if (funct3 == 6) //ori
            ALU_OP = 6;
        
        else if (funct3 == 7) //andi
            ALU_OP = 1;
       }

    else if(funct3==0) //addi
		     ALU_OP= 0;
		
     }

	else if (opcode == OPCODE_I3)
	{
		buffer_ID_EX.isALU = TRUE;
        int s1,s2,s3,s4;
	    s1=IR << 12;
        s2=IR << 20;
        RF_WRITE = 1;
        addressA = s1;
        addressA =addressA >> 27;
        addressC = s2;
        addressC = addressC >> 27;
        MEM_READ = 0;
        MEM_WRITE = 0;
        unsigned int shamt = IR << 7;
        shamt >>= 27;

        B_SELECT = 1;

        if (funct3 != 0) //addiw
        {
            if (funct3 == 1) //slliw
        {   ALU_OP = 7;
            immediate = shamt;
        }

        else if (funct3 == 5) //srliw,sraiw
        {
            immediate = shamt;
            if (funct7 != 0)
                ALU_OP = 21;
            else
                ALU_OP = 19;
        }
        }

        else
		{
			immediate=IR>>20;
			ALU_OP=0;
		}
     }

	else if (opcode == OPCODE_I4)
	{ //for jalr
		buffer_ID_EX.isJAL_JALR = FALSE;
		buffer_ID_EX.isBranchInstruction = TRUE;
		buffer_ID_EX.branchTaken = TRUE;
		
		RF_WRITE = 1;
        int imm = IR >> 20;
        unsigned int rs1 = IR << 12;
        rs1 =rs1 >> 27;
		addressA = rs1;
        unsigned int rd = IR << 20;
        rd >>= 27;
		addressC = rd;
		immediate = imm;
        PC_SELECT = 0;
        B_SELECT = 1;
        Y_SELECT = 2;
        ALU_OP = 22;
        MEM_READ = 0;
        MEM_WRITE = 0;

		}
		}
	else if (opcode == OPCODE_S1) //store
	{
		buffer_ID_EX.isStore = TRUE;
        int t1;
		t1= (1 << 5) - 1;
        int tmp;
        tmp=t1;
        tmp <<= 7;
        int imm1 = IR & tmp;
        imm1 =imm1 >> 7;
        int imm2 = IR >> 25;
		int t2;
		t2=(imm2 << 5);
        immediate = imm1 + t2;
        addressB = IR << 7;
        addressB = addressB>> 27;
        addressA = IR << 12;
        addressA >>= 27;
		MEM_READ = 0;
        B_SELECT = 1;
        RF_WRITE = 0;
        ALU_OP = 0;
        if (funct3!=0)
		{
			if(funct3 == 1 && select==1)
            MEM_WRITE = 2;
         else if (funct3 == 2 && select==1)
            MEM_WRITE = 3;
         else if (funct3 == 3&& select==1)
            MEM_WRITE = 4;
		}
        else if (funct3 == 0)
            MEM_WRITE = 1;
		}

	else if (opcode == OPCODE_U1) //auipc
	{
		buffer_ID_EX.isALU = TRUE;
        int u1,u2;
	    u1=IR>>12;
		u2=IR<<20;
        immediate = u1;
		u2>>=27;
        addressC = u2;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
        ALU_OP = 12;
    
	}

	else if (opcode == OPCODE_U2) //lui
	{
		buffer_ID_EX.isALU = TRUE;
        int o1;
		o1=IR>>12;
		immediate = o1;
        addressC = IR << 20;
        addressC =addressC>> 27;
        ALU_OP = 13;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
	}

   else if (opcode == OPCODE_R1 || opcode == OPCODE_R2)
	{
		buffer_ID_EX.isALU = TRUE;
        int k;
        unsigned int rs1 = IR << 12;
		rs1 >>= 27;
        unsigned int rs2 = IR << 7;
		rs2 >>= 27;
        unsigned int rd = IR << 20;
        rd = rd>> 27;
        RF_WRITE = 1;
        B_SELECT = 0;
        if (funct3 == 0)
        {
            if (funct7 != 0)
                { if (funct7 != 1)
                   ALU_OP = 18; //SUB
				   else
				   ALU_OP=25;//mul
				}
            else if(funct7==0)
                ALU_OP = 0; //add,addw
        }

        else if (funct3 == 2 || funct3 == 3)
            ALU_OP = 7; //slt,sltu

        else if (funct3 == 1)
            ALU_OP = 7; //sll,sllw

        else if (funct3 == 4)
        {
            if (funct7 != 1)
                ALU_OP = 8; //xor
            else
                ALU_OP = 29; //div
        }

        else if (funct3 == 5)
        {
            if (funct7 == 1)
                ALU_OP = 30; //divu
            else if (funct7 == 0)
                ALU_OP = 19; // srl.srlw
            else
                ALU_OP = 21; //sra,sraw
        }

        else if (funct3 == 6)
        {
            if (funct7 != 1)
                ALU_OP = 6; //or
            else
                ALU_OP = 31; //rem
        }

        else if (funct3 == 7)
        {
            if (funct7 != 1)
                ALU_OP = 1; //and
            else
                ALU_OP = 32; //remu
        }
        addressA = rs1;
        addressB = rs2;
        addressC = rd;
        MEM_READ = 0;
        MEM_WRITE = 0;
		}
	

	else if (opcode == OPCODE_UJ)
	{ // jal
	    buffer_ID_EX.isJAL_JALR = FALSE;
		buffer_ID_EX.isBranchInstruction = TRUE;
		buffer_ID_EX.branchTaken = TRUE;
			
        unsigned int rd = IR << 20;
        rd >>= 27;
        addressC = rd;
        bitset<20> tmp2(IR >> 12), res;
		int i=18;
        while ( i>=11)
      {  res[i] = tmp2[i-11];
	     i--;
	  }
	  res[10]=tmp2[8];
        for (int i = 9; i >= 0; --i)
            res[i] = tmp2[i + 9];
        int tmp1 = res.to_ulong();
		
		if (tmp2[19])
            tmp1 = tmp1 - (1 << 19);

		int h1= tmp1*2;
        immediate = h1;
        RF_WRITE = 1;
        B_SELECT = 0;
        INC_SELECT = 1;
        ALU_OP = -1;
        addressA = 0;
        addressB = 0;
        MEM_READ = 0;
        MEM_WRITE = 0;
        Y_SELECT = 2;
		
	}

	else if (opcode == OPCODE_SB1)
	{
		buffer_ID_EX.isBranchInstruction = TRUE;
        unsigned int rs1 ;
		unsigned int s1 = IR << 12;
		rs1=s1;
        rs1 =rs1>> 27;
        unsigned int rs2,s2 ;
	    s2= IR << 7;
		rs2=s2;
        rs2 =rs2 >> 27;
        unsigned int rd,s3 ;
		s3= IR << 20;
        rd = s3;
		rd=rd>> 27;
        int bit_11 = (rd & 1) << 10;
        int bit_1_4 = rd >> 1;
		int j=(funct7 >> 6);
        int bit_12 = j << 11;
		int j1=(bit_12 << 6);
        int bit_5_10 = (funct7 - j1) << 4;
        immediate = bit_1_4 | bit_5_10 | bit_11 | bit_12;
        immediate =immediate << 1;

        RF_WRITE = 0;
        B_SELECT = 0;
   // Execute moved to decode for stalling
			int InA = regArray[rs1];
			int InB = regArray[rs2];
			if (funct3!=1)
		{
			if (funct3 == 0 && select==1) //beq
            ALU_OP = 2;
           else if (funct3 == 4 && select==1) //blt
            ALU_OP = 4;
           else if(funct3 == 5 && select==1)   //bge
            ALU_OP = 3; 
           else if (funct3 == 6 && select==1) //bltu
            ALU_OP = 35;

           else if(funct3 == 7){   //bgeu
            ALU_OP = 34;}
		}

        else if (funct3 == 1) //bne Update task2.cpp
            ALU_OP = 5;
			
			addressA = rs1;
			addressB = rs2;
			addressC = 0;
			MEM_READ = 0;
			MEM_WRITE = 0;
		}
		if (addressA < 0 && select==1)
			addressA += 32;
		if (addressB < 0 && select ==1)
			addressB += 32;
		if (addressC < 0 && select==1)
			addressC += 32;

		buffer_ID_EX.PC = buffer_IF_ID.PC;
        buffer_ID_EX.addressA = addressA;
		buffer_ID_EX.addressB = addressB;
		buffer_ID_EX.addressC = addressC;
		buffer_ID_EX.immediate = immediate;
		buffer_ID_EX.B_SELECT = B_SELECT;
		buffer_ID_EX.PC_SELECT = PC_SELECT;
		buffer_ID_EX.INC_SELECT = INC_SELECT;
		buffer_ID_EX.Y_SELECT = Y_SELECT;
		buffer_ID_EX.ALU_OP = ALU_OP;
		buffer_ID_EX.RF_WRITE = RF_WRITE;
		
	    buffer_ID_EX.MEM_READ = MEM_READ;
		buffer_ID_EX.MEM_WRITE = MEM_WRITE;
		buffer_ID_EX.returnAddress = returnAddress;
		prevPC = buffer_IF_ID.PC;
		readWriteRegFile(DECODE_STAGE);
	}
	//End of decode

	// ALU
	
	void alu(int ALU_OP, int B_SELECT, int immediate = 0)
	{
		buffer_ID_EX.en = buffer_IF_ID.en;
		
		if (buffer_ID_EX.en == 0)
			return;
     
	   else{
        int PC_SELECT = buffer_ID_EX.PC_SELECT;
		int INC_SELECT = buffer_ID_EX.INC_SELECT;
		immediate = buffer_ID_EX.immediate;
		
		unsigned int returnAddress; //Return Address in case of jal/jalr
		int RZ;
		int RA = buffer_ID_EX.RA;
		int RB = buffer_ID_EX.RB;
		
		buffer_EX_MEM.RA = buffer_ID_EX.RA;
		buffer_EX_MEM.RB = buffer_ID_EX.RB;
		buffer_EX_MEM.isALU = buffer_ID_EX.isALU;
		buffer_EX_MEM.isLoad = buffer_ID_EX.isLoad;
        buffer_EX_MEM.PC_SELECT = buffer_ID_EX.PC_SELECT;
		buffer_EX_MEM.INC_SELECT = buffer_ID_EX.INC_SELECT;
		buffer_EX_MEM.addressC = buffer_ID_EX.addressC;
		buffer_EX_MEM.immediate = buffer_ID_EX.immediate;
		buffer_EX_MEM.Y_SELECT = buffer_ID_EX.Y_SELECT;
        buffer_EX_MEM.isStore = buffer_ID_EX.isStore;
		buffer_EX_MEM.isJAL_JALR = buffer_ID_EX.isJAL_JALR;
		buffer_EX_MEM.isBranchInstruction = buffer_ID_EX.isBranchInstruction;
		buffer_EX_MEM.branchTaken = buffer_ID_EX.branchTaken;
		
		buffer_EX_MEM.MEM_WRITE = buffer_ID_EX.MEM_WRITE;
		buffer_EX_MEM.RF_WRITE = buffer_ID_EX.RF_WRITE;
		buffer_EX_MEM.MEM_READ = buffer_ID_EX.MEM_READ;
		buffer_EX_MEM.addressA = buffer_ID_EX.addressA;
		returnAddress = buffer_ID_EX.returnAddress;
		buffer_EX_MEM.addressB = buffer_ID_EX.addressB;

		buffer_EX_MEM.PC = buffer_ID_EX.PC;
        int InA,InB;
		InA = RA;
		
		if (B_SELECT != 0)
			InB = immediate;
		else
			InB = RB;

		if (ALU_OP == 0) //addi,load
		      RZ = InA + InB;
		
		else if (ALU_OP == 1) //andi
			RZ = InA & InB;

		else if (ALU_OP == 2 && select == 1) //beq
		{
			if (InA == InB)
			{   
				PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}
		else if (ALU_OP == 3) //bge
		{
			if (InA >= InB)
			{   
				PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE,immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}

		else if (ALU_OP == 4) //blt
		{
			if (InA < InB)
			{   PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}

		else if (ALU_OP == 34) //bgeu
		{
			if ((unsigned)InA >= (unsigned)InB)
			{   PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}
      
	   else if (ALU_OP == 5) //bne
		{
			if (InA != InB)
			{   PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}

		else if (ALU_OP == 35) //bltu
		{
			if ((unsigned)InB > (unsigned)InA)
			{   PC_SELECT = 1;
				INC_SELECT = 1;
				iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
				buffer_EX_MEM.branchTaken = TRUE;
			}
		}

	
     else if (ALU_OP == 6) //ori
			RZ = InA | InB;

		else if (ALU_OP == 7) //slli
			RZ = InA << InB;
			
		else if (ALU_OP == 8) //xori
			RZ = InA ^ InB;
        
		else if (ALU_OP == 18) //sub
			RZ = InA - InB;
		
		else if (ALU_OP == 12) //auipc
		   {    int tmp = InB << 12;
			    RZ = buffer_ID_EX.PC + tmp - 4;
		   }
		
		else if (ALU_OP == 13) //lui
			RZ = InB << 12;

		else if (ALU_OP == 19) //srli
			RZ = InA >> InB;

		else if (ALU_OP == 20) //slti,sltiu
		{	 if(InA < InB) 
		       RZ = 1;
			  else
			    RZ=0;
		} 
       
	   else if (ALU_OP == 22) //jalr
		{
			buffer_EX_MEM.RZ = InA + InB;
			RZ = InA + InB;
			returnAddress = iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
		}

		else if (ALU_OP == 21) //sra, sraw
		{
			RZ = InA >> InB;
			RZ |= InA & (1 << 31);
		}
		
		
		else if (ALU_OP == 29 || ALU_OP == 30) //div, divu
			RZ = RA / RB;
		
		else if (ALU_OP == 25) //mul
			RZ = RA * RB;

        else if (ALU_OP == 31 || ALU_OP == 32) // rem, remu
			RZ = RA % RB;

		else if (ALU_OP == -1)   // jal
		{   PC_SELECT = 1;
			returnAddress = iag(EXECUTE_STAGE, immediate, INC_SELECT, PC_SELECT);
		}

		if (buffer_ID_EX.isStore == TRUE || buffer_ID_EX.isLoad == TRUE )
			dataTransferInstructions++;
		else if (buffer_ID_EX.isBranchInstruction == TRUE)
			controlInstructions++;	
		else if (buffer_ID_EX.isALU == TRUE)
			aluInstructions++;
		
        buffer_EX_MEM.RZ = RZ;
        buffer_EX_MEM.PC_SELECT = PC_SELECT;
		buffer_EX_MEM.returnAddress = returnAddress;
		buffer_EX_MEM.INC_SELECT = INC_SELECT;
		
	}
}
	//end of ALU function

	//Stage 4: Memory & RY get updated
    
	void memoryStage(int Y_SELECT, int MEM_READ, int MEM_WRITE, int address = 0, int data = 0)
	{
		buffer_EX_MEM.en = buffer_ID_EX.en;
		if (buffer_EX_MEM.en == 0)
			return;
		else{	
		buffer_MEM_WB.isALU = buffer_EX_MEM.isALU;
		buffer_MEM_WB.isStore = buffer_EX_MEM.isStore;
		buffer_MEM_WB.isLoad = buffer_EX_MEM.isLoad;
		buffer_MEM_WB.isJAL_JALR = buffer_EX_MEM.isJAL_JALR;
		buffer_MEM_WB.branchTaken = buffer_EX_MEM.branchTaken;
		buffer_MEM_WB.isBranchInstruction = buffer_EX_MEM.isBranchInstruction;
		
		int returnAdd = buffer_EX_MEM.returnAddress;
		int RY,dataFromMem;
		
		dataFromMem = readWriteMemory(MEM_READ, MEM_WRITE, address, data);
		
        if (Y_SELECT == 0)
			RY = buffer_EX_MEM.RZ;
		else if (Y_SELECT == 1)
			RY = dataFromMem;
		else if (Y_SELECT == 2)
			RY = returnAdd;
        
		buffer_MEM_WB.PC = buffer_EX_MEM.PC;
		buffer_MEM_WB.RY = RY;
		buffer_MEM_WB.addressC = buffer_EX_MEM.addressC;
		buffer_MEM_WB.RF_WRITE = buffer_EX_MEM.RF_WRITE;
			
    }
}
	//End of memoryStage

	//Stage 5: WriteBack
	void writeBack(int RF_WRITE, int addressC)
	{
		buffer_MEM_WB.en = buffer_EX_MEM.en;
		if (buffer_MEM_WB.en == 0)
			return;
		
		else
	      readWriteRegFile(WB_STAGE);
	}
	//End of writeBack

	void forward_dependency_EtoE()
	{
		if (buffer_EX_MEM.addressC == 0)
			return;
		if (buffer_EX_MEM.isJAL_JALR == FALSE){
		   if(buffer_EX_MEM.isALU == FALSE)
			  return;
		 }

		if (buffer_ID_EX.addressB == buffer_EX_MEM.addressC && buffer_ID_EX.addressA == buffer_EX_MEM.addressC)
		{   data_hazard++;
			buffer_ID_EX.RA = buffer_EX_MEM.RZ;
			buffer_ID_EX.RB = buffer_EX_MEM.RZ;
		}
		
		if (buffer_ID_EX.addressA == buffer_EX_MEM.addressC)
		{
			data_hazard++;
			buffer_ID_EX.RA = buffer_EX_MEM.RZ;
			return;
		}
		
		if (buffer_ID_EX.addressB == buffer_EX_MEM.addressC)
		{   
			data_hazard++;
			buffer_ID_EX.RB = buffer_EX_MEM.RZ;
			return;
		}
		return;
	}
	//End of forward_dependency_EtoE()

	//Check forward dependency Memory to Execute
	void forward_dependency_MtoE()
	{
		if (buffer_MEM_WB.addressC == 0)
			return;
    
	  else{
		if (buffer_ID_EX.addressB == buffer_MEM_WB.addressC && buffer_ID_EX.addressA == buffer_MEM_WB.addressC)
		{
			data_hazard++;
			buffer_ID_EX.RA = buffer_MEM_WB.RY;
			buffer_ID_EX.RB = buffer_MEM_WB.RY;
			return;
		}
		if (buffer_ID_EX.addressA == buffer_MEM_WB.addressC)
		{
			data_hazard++;
			buffer_ID_EX.RA = buffer_MEM_WB.RY;
			return;
		}
		if (buffer_ID_EX.addressB == buffer_MEM_WB.addressC)
		{
			data_hazard++;
			buffer_ID_EX.RB = buffer_MEM_WB.RY;
			return;
		}
		return;
	  }
	}
	//End of forward_dependency_MtoE()

	//Check forward dependency Memory to Execute for stalls
	void forward_dependency_MtoEStall()
	{
		if (buffer_MEM_WB.addressC == 0)
			return;
		
		else if (buffer_MEM_WB.isLoad == FALSE)
			return;
		
		else if (buffer_EX_MEM.isStore == TRUE)
			return;
		
		else{
			
			if (buffer_ID_EX.addressA == buffer_MEM_WB.addressC && buffer_ID_EX.addressB == buffer_MEM_WB.addressC)
		   {
			stalls_data_hazard++;
			data_hazard++;
		    cycleCount++;
			buffer_ID_EX.RA = buffer_MEM_WB.RY;
			buffer_ID_EX.RB = buffer_MEM_WB.RY;
			return;
		 }
		else if (buffer_ID_EX.addressA == buffer_MEM_WB.addressC)
		{
			stalls_data_hazard++;
			data_hazard++;
			cycleCount++;
			buffer_ID_EX.RA = buffer_MEM_WB.RY;
			return;
		}
		else if (buffer_ID_EX.addressB == buffer_MEM_WB.addressC)
		{
			stalls_data_hazard++;
			data_hazard++;
			cycleCount++;
			buffer_ID_EX.RB = buffer_MEM_WB.RY;
			return;
		}
		return;
	}
}
	//End of forward_dependency_MtoEStalls()

	//Check data dependency Memory to Memory
	void forward_dependency_MtoM()
	{
		if (buffer_MEM_WB.addressC == 0)
			return;
		else if (buffer_MEM_WB.isLoad == FALSE)
			return;

		else{
			if (buffer_EX_MEM.isStore == TRUE)
		   { // Load-Store Dependency
			if (buffer_EX_MEM.addressB == buffer_MEM_WB.addressC)
			  { data_hazard++;
				buffer_EX_MEM.RB = buffer_MEM_WB.RY;
				return;
			  }
			return;
		   }
	   }
	}
	//End of forward_dependency_MtoM()

	//Check data stalling Execute to Execute
	int stall_check_EtoE()
	{
		if (buffer_EX_MEM.addressC == 0)
			return NO_DATA_DEPEND;

		else if (buffer_ID_EX.addressA == buffer_EX_MEM.addressC && buffer_ID_EX.addressB == buffer_EX_MEM.addressC)
		{
			PC_of_stalledStageEtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RA_RB;
		}
		else if (buffer_ID_EX.addressA == buffer_EX_MEM.addressC)
		{
			PC_of_stalledStageEtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RA;
		}
	   else if (buffer_ID_EX.addressB == buffer_EX_MEM.addressC)
		{
			PC_of_stalledStageEtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RB;
		}
		 
		 return NO_DATA_DEPEND;
	}
	//End of stall_check_EtoE()

	//Check data stalling Memory to Execute
	int stall_check_MtoE()
	{
		if (buffer_MEM_WB.addressC == 0)
			return NO_DATA_DEPEND;

		else if (buffer_ID_EX.addressA == buffer_MEM_WB.addressC && buffer_ID_EX.addressB == buffer_MEM_WB.addressC)
		{
			PC_of_stalledStageMtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RA_RB;
		}

	    else if (buffer_ID_EX.addressA == buffer_MEM_WB.addressC)
		{
			PC_of_stalledStageMtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RA;
		}
		else if (buffer_ID_EX.addressB == buffer_MEM_WB.addressC)
		{
			PC_of_stalledStageMtoE = buffer_ID_EX.PC;
			return DATA_DEPEND_RB;
		}
		else
		   return NO_DATA_DEPEND;
	}
	//End of stall_check_MtoE()

	//Prints pipeline registers data
	void printPipelineRegisters()
	{
		fstream fileWriting;
		fileWriting.open("pipelineRegisters.txt", ios::app);
		cout << "--------------------------------------------------------------------" << endl;
		fileWriting << "Cycle       :    " << cycleCount << endl;
		fileWriting << "RA          :    " << buffer_ID_EX.RA << endl;
		fileWriting << "RB          :    " << buffer_ID_EX.RB << endl;
		fileWriting << "RZ          :    " << buffer_EX_MEM.RZ << endl;
		fileWriting << "RY          :    " << buffer_MEM_WB.RY << endl;
		cout << "--------------------------------------------------------------------" << endl;
		fileWriting.close();
	}
	//End of printpipelineRegisters

	//Run Instructions: pipelined
	void runCode()
	{
		bool en = 1;
		int branchDecisionMade = 0,instruction_number;       //instruction_number for knob5
		cout << "Knob 2 (For Data Forwarding Knob)           :  ";
		cin >> knob2;
		cout << "Knob 3 (For Register File Knob)             :  ";
		cin >> knob3;
		cout << "Knob 4 (For Pipeline Register Knob)         :  ";
		cin >> knob4;
		cout << "Knob 5 (For Instruction Knob)               :  ";
		cin >> knob5;
		if (knob5 == ON)
		{
			cout << "Instruction Number                      :  ";
			cin >> instruction_number;
			instruction_number = instruction_number << 2;
		}
		cout << "----------------------------------------------------------------------" << endl;
		int tmp2 = 0,flag=0;
		
		while (1)
		{
			cycleCount++;
			if (memory[buffer_IF_ID.PC] == 0 && memory[buffer_IF_ID.PC + 1] == 0 && memory[buffer_IF_ID.PC + 2] == 0 && memory[buffer_IF_ID.PC + 3] == 0)
				en = 0;

			if (knob2 == 0)
			{
               if (buffer_MEM_WB.en2 == 1)
				{
                   writeBack(buffer_MEM_WB.RF_WRITE, buffer_MEM_WB.addressC);
				}

				if (buffer_EX_MEM.en2 != 1)
				{
                    buffer_MEM_WB.en2 = 0;
					buffer_MEM_WB.addressC = 0;
					
				}
				else
				{
					buffer_MEM_WB.en2 = 1;
					memoryStage(buffer_EX_MEM.Y_SELECT, buffer_EX_MEM.MEM_READ, buffer_EX_MEM.MEM_WRITE, buffer_EX_MEM.RZ, buffer_EX_MEM.RB);
				}

				if (buffer_ID_EX.en2 != 1)
				{   buffer_EX_MEM.en2 = 0;
					buffer_EX_MEM.addressC = 0;
				}
				
				else
				{  buffer_EX_MEM.en2 = 1;
				   alu(buffer_ID_EX.ALU_OP, buffer_ID_EX.B_SELECT, buffer_ID_EX.immediate);
				}

				if (buffer_IF_ID.en2 < 1)
					buffer_IF_ID.en2++;

				if (branchDecisionMade == 0 && buffer_IF_ID.en2 == 1 )
				{ 
					buffer_ID_EX.en2 = 1;
					decode();
				}
				else
				{
					buffer_ID_EX.en2 = 0;
				}
				int dataDependencyMtoE = stall_check_MtoE();
				
				if (dataDependencyMtoE != NO_DATA_DEPEND)
				{   buffer_ID_EX.en2 = 0;
					buffer_IF_ID.en2 = 0;
					stalls_data_hazard = stalls_data_hazard + 1;
					data_hazard = data_hazard + 1;
				}
				
				int dataDependencyEtoE = stall_check_EtoE();
				if (dataDependencyEtoE != NO_DATA_DEPEND)
				{	
					buffer_ID_EX.en2 = 0;
					buffer_IF_ID.en2 = -1;
					stalls_data_hazard++;
					data_hazard++;
					tmp2=1;
				}
				if(dataDependencyEtoE != NO_DATA_DEPEND && tmp2==1)
				{   
					data_hazard--;
					tmp2=0;
				}
			
				if (buffer_IF_ID.en2 == 1)
				{   fetch(en);
					branchDecisionMade = 0;
				}

				if (buffer_EX_MEM.branchTaken == TRUE && buffer_EX_MEM.isBranchInstruction == TRUE && branchDecisionMade == 0)
				{   
					buffer_IF_ID.en2 = 0;
					buffer_IF_ID.PC = buffer_EX_MEM.PC - 4;
					buffer_ID_EX.en2 = 0;
					buffer_EX_MEM.isBranchInstruction = FALSE;
					buffer_EX_MEM.branchTaken = FALSE;
					stalls_control_hazard += 2;
					branch_mispredictions = branch_mispredictions + 1;
					branchDecisionMade = 1;
				}
			}
			else if (knob2 == 1)
			{
				if (buffer_MEM_WB.en2 == 1)
				
					writeBack(buffer_MEM_WB.RF_WRITE, buffer_MEM_WB.addressC);
				
				forward_dependency_MtoE();
				forward_dependency_MtoM();
				forward_dependency_EtoE();

				if (buffer_EX_MEM.en2 != 1)
				  buffer_MEM_WB.en2 = 0;
				else {
					memoryStage(buffer_EX_MEM.Y_SELECT, buffer_EX_MEM.MEM_READ, buffer_EX_MEM.MEM_WRITE, buffer_EX_MEM.RZ, buffer_EX_MEM.RB);
					buffer_MEM_WB.en2 = 1;
				}
					
               forward_dependency_MtoEStall();

				if (buffer_ID_EX.en2 != 1)
				   buffer_EX_MEM.en2 = 0;
					
			    else{
					alu(buffer_ID_EX.ALU_OP, buffer_ID_EX.B_SELECT, buffer_ID_EX.immediate);
					buffer_EX_MEM.en2 = 1;
				}
					
                buffer_ID_EX.en2 = 0;
				if (buffer_IF_ID.en2 == 1)
				{
					decode();
					buffer_ID_EX.en2 = 1;
				}
				
				fetch(en);
				buffer_IF_ID.en2 = 1;

				if ( buffer_EX_MEM.branchTaken == TRUE)
				  if(buffer_EX_MEM.isBranchInstruction == TRUE)
				{
					buffer_IF_ID.PC = buffer_EX_MEM.PC - 4;
					buffer_ID_EX.en2 = 0;
					buffer_IF_ID.en2 = 0;
				    buffer_EX_MEM.isBranchInstruction = FALSE;
					buffer_EX_MEM.branchTaken = FALSE;
					stalls_control_hazard = stalls_control_hazard + 2;
					branch_mispredictions++;
				}
			}
			if (knob3 == ON)
				printRegisterFile();

			if (knob4 == ON)
				printPipelineRegisters();

			if (knob5 == ON)
			{
				if (buffer_ID_EX.PC == instruction_number)
				{
					cout << "Cycle Number       :    " << cycleCount;
					cout << endl;
					cout << "RA                 :    " << buffer_ID_EX.RA << endl;
					cout << "RB                 :    " << buffer_ID_EX.RB << endl;
				}
				if (buffer_EX_MEM.PC == instruction_number)
				{
					cout << "Cycle Number       :    " << cycleCount << endl;
					cout << "RZ                 :    " << buffer_EX_MEM.RZ;
					cout << endl;
				}
				if (buffer_MEM_WB.PC == instruction_number)
				{
					cout << "Cycle Number       :    " << cycleCount << endl;
					cout << "RY                 :    " << buffer_MEM_WB.RY << endl;
				}
			}

			if (buffer_IF_ID.en == 0 && en == 0 && buffer_ID_EX.en == 0 && buffer_MEM_WB.en == 0 && buffer_EX_MEM.en == 0)
				break;

			if (knob2 == ON)
				cout << data_hazard;
		}
		// Inserted NOP

		if(knob2==ON)
			data_hazard = data_hazard + stalls_data_hazard;
		
		cycleCount = cycleCount - 2;
	    aluInstructions = aluInstructions - 1;
		if (knob3 == OFF)
			printRegisterFile();
		cout << "----------------------------------------------------------------------" << endl;
	}
	//End of runCode
};

class unpipelined
{
  public:
	unsigned int PC = 0;	    //Program Counter
    int IR, RA, RB, RZ, RY, RM; // Instruction Register & Interstage Buffers
	unsigned int addressA;
	unsigned int addressB;
	int immediate,addressC;				// for immediate values & destination register respectively
	unsigned int returnAddress;         //Return Address in case of jal/jalr

	//Control Signals
	int ALU_OP, B_SELECT, PC_SELECT, INC_SELECT, Y_SELECT;
	int MEM_READ, MEM_WRITE, RF_WRITE;

	//Call in decode stage & Writeback Stage
	void readWriteRegFile(int RF_WRITE, int addressA, int addressB, int addressC)
	{
		int reg;
	    reg = RF_WRITE;
	   if (reg == 1)
	  {
		if (addressC)
			regArray[addressC] = RY; // RY INTERSTAGE BUFFER
		return;
	  }

       RA = regArray[addressA]; // UPDATE INTERSTAGE BUFFER
	   RB = regArray[addressB];
	}
	//End of readWriteRegFile

	/*Instruction Address Generator
    returns returnAddress*/
	lli iag(int INC_SELECT, int PC_SELECT, lli immediate = 0)
	{
		lli PC_Temp = PC + 4;
		
		if(PC_SELECT != 0){
           if (INC_SELECT == 1)
				PC += immediate;
			else
				PC += 4;
		}
		else 
			PC = RZ;
		
	    return PC_Temp;
	}
	//End of function iag

	//Stage 1: Fetch Stage
	void fetch()
	{
		IR = readWriteMemory(3, 0, PC);
		returnAddress = iag(0, 1);
	}
	//end of fetch

	/* Stage 2: Decode Stage
    RA & RB will be updated after this stage */
void decode()
{
  if(select==-1)
  {  cout<<"error";
     return;
  }
  
	unsigned int opcode,funct3,funct7;
	opcode = IR << 25;
	opcode = opcode >> 25;
	funct3 = IR << 17;
    funct3 = funct3>> 29;
	unsigned int e = IR >> 25;
	funct7=e;
    PC_SELECT = 1;
    INC_SELECT = 0;
    Y_SELECT = 0;
	if(opcode==OPCODE_I1 || opcode==OPCODE_I2 || opcode==OPCODE_I3 || opcode==OPCODE_I4 )
	{
		if (opcode == OPCODE_I1 && select==1)
      {
        dataTransferInstructions++;
        RF_WRITE = 1;
		unsigned int p1,p2,p3;
        int imm = IR >> 20;
		immediate = imm;
        unsigned int rs1;
		p1 = IR << 12;
		rs1=p1;
        rs1 =rs1 >> 27;
		addressA = rs1;
        unsigned int rd;
		p2 = IR << 20;
		rd=p2;
        rd = rd>> 27;
		addressC = rd;
        B_SELECT = 1;
        ALU_OP = 0;
        Y_SELECT = 1;
        if (funct3 != 0)
		{
		 if (funct3 == 1)
            MEM_READ = 2;
         else if (funct3 == 2)
            MEM_READ = 3;
         else if (funct3 == 3)
            MEM_READ = 4;
        }
		else
            MEM_READ = 1;

		MEM_WRITE = 0;

     }

     else if (opcode == OPCODE_I2 && select == 1)
     {
         aluInstructions++;
        RF_WRITE = 1;
		unsigned int od1,od2,od3;
        int imm = IR >> 20;
	    immediate=imm;
        unsigned int rs1;
		od2 = IR << 12;
		rs1=od2;
        rs1 =rs1>> 27;
		addressA = rs1;
        unsigned int rd;
		od3 = IR << 20;
		rd=od3;
        rd >>= 27;
		addressC = rd;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        if (funct3 != 0)
       {
           if (funct3 == 1 && select==1) //slli
          {
            unsigned int st = IR << 7;
            st = st>> 27;
            immediate = st;
            ALU_OP = 7;
         }

        else if (select==1 && (funct3 == 2 || funct3 == 3)) //slti,sltiu
              ALU_OP = 20;
        
        else if (select==1 && funct3 == 4) //xori
            ALU_OP = 8;
        
        else if (funct3 == 5) //srli
        {   unsigned int st;
			st = IR << 7;
		    st >>= 27;
            immediate = st;
            if (funct7 != 0)
                ALU_OP = 21;
            else
                ALU_OP = 19;
        }
        else if (select==1&&funct3 == 6) //ori
             ALU_OP = 6;
        
        else if (select==1&&funct3 == 7) //andi
             ALU_OP = 1;
        
      }

        else if(funct3==0) //addi
		    ALU_OP= 0;
	}

     else if (select==1 && opcode == OPCODE_I3)
     {
		unsigned int s1,s2,s3,s4;
		s1=IR << 12;
        s2=IR << 20;
        RF_WRITE = 1;
        aluInstructions++;
        addressA = s1;
        addressA =addressA >> 27;
        addressC = s2;
        addressC = addressC >> 27;
        MEM_READ = 0;
        MEM_WRITE = 0;
        unsigned int shamt = IR << 7;
        shamt >>= 27;

        B_SELECT = 1;

      if (funct3 != 0) //addiw
        {
            if (funct3 == 1) //slliw
         {
            ALU_OP = 7;
            immediate = shamt;
         }

        else if (funct3 == 5) //srliw,sraiw
        {
            immediate = shamt;
            if (funct7 != 0)
                ALU_OP = 21;
            else
                ALU_OP = 19;
        }
        }

      else
		{
			immediate=IR>>20;
			ALU_OP=0;
		}
     }

     else if (opcode == OPCODE_I4 && select==1)
	 { //for jalr
        RF_WRITE = 1;
        controlInstructions++;
        int imm = IR >> 20;
        unsigned int rs1 = IR << 12;
        rs1 =rs1 >> 27;
		addressA = rs1;
        unsigned int rd = IR << 20;
        rd >>= 27;
		addressC = rd;
		immediate = imm;
        PC_SELECT = 0;
        B_SELECT = 1;
        Y_SELECT = 2;
        ALU_OP = 22;
        MEM_READ = 0;
        MEM_WRITE = 0;
     }

	}

    else if (opcode == OPCODE_S1 && select==1) // instructions for store
    {
        int t1 = 1 << 5;
        int tmp;
        dataTransferInstructions++;
        tmp = t1 - 1;
        tmp <<= 7;
        int imm1 = IR & tmp;
        imm1 = imm1 >> 7;
        int imm2 = IR >> 25;
		int t2;
		t2=(imm2 << 5);
        immediate = imm1 + t2;
        addressB = IR << 7;
        addressB = addressB >> 27;
        addressA = IR << 12;
        addressA >>= 27;
		  MEM_READ = 0;
         B_SELECT = 1;
         RF_WRITE = 0;
         ALU_OP = 0;
        if (funct3!=0)
		{
		 if(funct3 == 1)
            MEM_WRITE = 2;
         else if (funct3 == 2 && select==1)
            MEM_WRITE = 3;
         else if (funct3 == 3)
            MEM_WRITE = 4;
		}
        else if (funct3 == 0)
            MEM_WRITE = 1;
    }

    else if (opcode == OPCODE_U1 && select==1) //auipc
    {   int u1,u2;
        aluInstructions++;
	    u1=IR>>12;
		u2=IR<<20;
        immediate = u1;
		u2>>=27;
        addressC = u2;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
        ALU_OP = 12;
    }

    else if (opcode == OPCODE_U2 && select==1) //lui
    {
		int o1;
        aluInstructions++;
		o1=IR>>12;
		immediate = o1;
        addressC = IR << 20;
        addressC = addressC >> 27;
        ALU_OP = 13;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
    }

    else if (opcode == OPCODE_R1 || opcode == OPCODE_R2)
    {   int k;
        aluInstructions++;
        unsigned int rs1 = IR << 12;
		rs1 = rs1 >> 27;
        unsigned int rs2;
		rs2 = IR << 7;
	   rs2 = rs2 >> 27;
        unsigned int rd = IR << 20;
        rd =rd >> 27;
        RF_WRITE = 1;
        B_SELECT = 0;
        if (funct3 == 0 && select==1)
        {
            if(funct7 != 0)
                { 
				  if (funct7 != 1)
                    ALU_OP = 18; //SUB
				  else
				    ALU_OP=25;//mul
				}
            else if(select==1 && funct7==0)
                ALU_OP = 0; //add,addw
        }

        else if (select==1&&funct3 == 1)
            ALU_OP = 7; //sll,sllw

        else if (funct3 == 2 || funct3 == 3)
            ALU_OP = 20; //slt,sltu

       else if (select==1&&funct3 == 4)
         {
            if (funct7 != 1)
                ALU_OP = 8; //xor
            else
                ALU_OP = 29; //div
        }

        else if (select==1&&funct3 == 5)
        {
            if (funct7 == 1)
                ALU_OP = 30; //divu
            else if (funct7 == 0)
                ALU_OP = 19; // srl.srlw
            else
                ALU_OP = 21; //sra,sraw
        }

        else if (select==1&&funct3 == 6)
        {
            if (funct7 != 1)
                ALU_OP = 6; //or
            else
                ALU_OP = 31; //rem
        }

        else if (select==1&&funct3 == 7)
        {
            if (funct7 != 1)
                ALU_OP = 1; //and
            else
                ALU_OP = 32; //remu
        }
        addressA = rs1;
        addressB = rs2;
        addressC = rd;
        MEM_READ = 0;
        MEM_WRITE = 0;
    }

    else if (opcode == OPCODE_UJ && select==1)
    {
        controlInstructions++;
         unsigned int rd = IR << 20;
        rd >>= 27;
        addressC = rd;
        bitset<20> tmp2(IR >> 12), res;
		int i=18;
        while ( i>=11)
        {  res[i] = tmp2[i-11];
	       i--;
	    }
	  
	   res[10]=tmp2[8];
        
		for (int i = 9; i >= 0; --i)
            res[i] = tmp2[i + 9];
        int tmp1 = res.to_ulong();
		
		if (tmp2[19])
		 tmp1 = tmp1 - (1 << 19);

		int h1= tmp1*2;
        immediate = h1;
		addressA = 0;
        addressB = 0;
		MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
        B_SELECT = 0;
        INC_SELECT = 1;
        ALU_OP = -1;
        Y_SELECT = 2;
    }

    else if (opcode == OPCODE_SB1 && select==1)
    {
        controlInstructions++;
		
        unsigned int rs1,s1,s2,s3;
		s1= IR << 12;
		rs1=s1;
        rs1 =rs1>> 27;
        unsigned int rs2 ;
		s2= IR << 7;
		rs2=s2;
        rs2 =rs2 >> 27;
        unsigned int rd ;
	    s3= IR << 20;
        rd =s3;
		rd=rd>> 27;
        int bit_11 = (rd & 1) << 10;
        int bit_1_4 = rd >> 1;
		unsigned int j=(funct7 >> 6);
        int bit_12 =  j << 11;
		int j1=(bit_12 << 6);
        int bit_5_10 = (funct7 - j1) << 4;
        immediate = bit_1_4 | bit_5_10 | bit_11 | bit_12;
        immediate =immediate << 1;

        RF_WRITE = 0;
        B_SELECT = 0;
        if (funct3!=1)
		{
			if (funct3 == 0 && select==1) //beq
            ALU_OP = 2;
           else if (funct3 == 4 && select==1) //blt
            ALU_OP = 4;
           else if(funct3 == 5 && select==1)  //bge
            ALU_OP = 3; 
           else if (funct3 == 6 && select==1) //bltu
            ALU_OP = 35;
		   else if(select==1&&funct3 == 7)   //bgeu
            ALU_OP = 34;
		}

        else if (select==1&&funct3 == 1) //bne Update task2.cpp
            ALU_OP = 5;
       int impedence=0;
       if(select==1) addressA = rs1;
       if(select==1) addressB = rs2;

       MEM_WRITE = impedence;
       MEM_READ = impedence;
       addressC = impedence;

    }
    readWriteRegFile(0, addressA, addressB, addressC);
}
   //End of decode

	/* Arithmetic Logic Unit
    Input: ALU_OP, MUXB select, immediate(if any)
    (these input will be provided by decode stage)
    Updates RZ */

void post_alu(int ALU_OP, int B, int immediate = 0,int enable=1)
{
    int InA = 0;
	int InB=B;

    //cout<<"XXXXXXXHELLO"<<endl;
    if(enable==1){

      InA=RA;
	
	  if (ALU_OP == 0) //addi,load,
	    RZ = InA + InB;
	 
	 else if (ALU_OP == 1) //andi
		RZ = InA & InB;

	else if (enable ==1 && ALU_OP == 2) //beq
	{
		if (InA == InB)
		{
			INC_SELECT=1;
		    PC = PC - 4;
			iag(INC_SELECT, 1, immediate);
		}
	}
	else if (enable ==1 && ALU_OP == 3) //bge
	{
		if (InA >= InB)
		{
			INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}

	//bgeu
	else if(enable ==1&&ALU_OP == 34){
		if((unsigned) InA >= (unsigned) InB)
		{
			INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}

	//bltu
	else if(enable ==1&&ALU_OP == 35){
		if ((unsigned) InA < (unsigned) InB)
		{
			INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}

	else if (enable ==1&&ALU_OP == 4) //blt
	{
		if (InA < InB)
		{
			INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}

	else if (enable ==1&&ALU_OP == 5) //bne
	{
		if (InA != InB)
		{
			INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}

	else if (enable ==1&&ALU_OP == 6) //ori
		RZ = InA | InB;

	else if (enable ==1&&ALU_OP == 7) //slli
		RZ = InA << InB;

   else if (enable ==1&&ALU_OP == 8) //xori
		RZ = InA ^ InB;
	
	else if (enable ==1&&ALU_OP == 18) //sub
		RZ = InA - InB;

   else if (enable ==1&&ALU_OP == 12) //auipc
	{
		RZ = PC - 4 + (InB << 12);
	}
	else if (enable ==1&&ALU_OP == 13) //lui
		RZ = InB << 12;

	else if (enable ==1&&ALU_OP == 19) //srli
		RZ = InA >> InB;

	else if (enable ==1&&ALU_OP == 20){ //slti,sltiu
		 if(InA < InB)
		     RZ = 1;
		 else 
			 RZ=0;
	}
	
	else if (enable ==1&&ALU_OP == 21) //sra, sraw
	{
		RZ = InA >> InB;
		RZ |= InA & (1 << 31);
	}
	else if (enable ==1&&ALU_OP == 22) //jalr
	{
		RZ = InA + InB;
		PC -= 4;
		returnAddress = iag(0, PC_SELECT, immediate);
	}
	else if (enable ==1&&ALU_OP == 25) //mul
		RZ = RA * RB;

	else if (ALU_OP == 29 || ALU_OP == 30) //div, divu
		RZ = RA / RB;

	else if (enable ==1 && (ALU_OP == 31 || ALU_OP == 32)) // rem, remu
		RZ = RA % RB;

	else if (enable ==1&&ALU_OP == -1)
	{ // jal
		PC -= 4;
		returnAddress = iag(INC_SELECT, 1, immediate);
	}
}
}
void alu(int ALU_OP, int immediate ,int B_SELECT,int enable=1)
{
	int InB=0;
    if(enable==-1)
        cout<<"error in pre_alu";
   
    else{
         if (B_SELECT == 0)
			  InB = RB;
	     else
		   InB = immediate;
        post_alu(ALU_OP,InB,immediate,1);
    }

}
    //end of ALU function

	/*Stage 4: Memory & RY get updated
    Input: Y_SELECT, MEM_READ, MEM_WRITE, address from RZ/RM, data */
	void memoryStage(int Y_SELECT, int MEM_READ, int MEM_WRITE, int address = 0, int data = 0)
	{
	    int dataFromMem;
        dataFromMem = readWriteMemory(MEM_READ, MEM_WRITE, address, data);
      
		if (Y_SELECT == 2)
			RY = returnAddress;
		if (Y_SELECT == 1)
			RY = dataFromMem;
		if (Y_SELECT == 0)
			RY = RZ;
}
	//End of memoryStage

  //Stage 5: WriteBack
	void writeBack(int RF_WRITE, int addressC)
	{
		readWriteRegFile(RF_WRITE, 0, 0, addressC);
	}
	//End of writeBack
	//Run Instructions: unpipelined
	void runCode(int enable)
	{
	    fstream fileWriting;
        fileWriting.open("pipelineRegisters.txt", ios::out); //To clear data of existing pipelineRegisters.txt
		fileWriting.close();
		int instruction_number; //for knob5
        if(enable==1){
		cout << "Knob 3 (Register File Knob)             :  ";
		cin >> knob3;
		cout << "Knob 4 (Pipeline Register Knob)         :  ";
		cin >> knob4;
		cout << "Knob 5 (Instruction Knob)               :  ";
		cin >> knob5;
        }
		if (enable==1&&knob5 == ON)
		{
			cout << "Instruction Number                  : ";
			cin >> instruction_number;
			instruction_number = instruction_number << 2;
		}
		cout << "*****-------------------------------------------*****" << endl;

		while (2)
	   {
			if(enable==1){
			   if (memory[PC] == 0 && memory[PC + 1] == 0 && memory[PC + 2] == 0 && memory[PC + 3] == 0)
				break;
			}
			fetch();
			decode();
			alu(ALU_OP, B_SELECT, 1);
            memoryStage(Y_SELECT, MEM_READ, MEM_WRITE, RZ, RB);
			writeBack(RF_WRITE, addressC);
			cycleCount++;
			
            if (knob3 == ON){
               printRegisterFile();
                }
			if (enable==1&&knob4 == ON)
				{
				    fstream fileWriting;
		           fileWriting.open("pipelineRegisters.txt", ios::app);
		           fileWriting << "****---------------------------------------------------------****" << endl;
		           fileWriting << "Cycle\t:\t" << cycleCount << endl;
		           fileWriting << "RA\t\t:\t" << RA << endl;
		           fileWriting << "RB\t\t:\t" << RB << endl;
		           fileWriting << "RZ\t\t:\t" << RZ << endl;
		           fileWriting << "RY\t\t:\t" << RY << endl;
		           fileWriting << "****---------------------------------------------------------****" << endl;
		           fileWriting.close();
				}
			if (knob5 == ON)
			{
				if (enable==1 && PC == instruction_number)
				{
				    cout << "Number of cycles\t:\t" << cycleCount << endl;
					cout << "RA\t\t:\t" << RA << endl;
					cout << "RB\t\t:\t" << RB << endl;
					cout << "RZ\t\t:\t" << RZ << endl;
					cout << "RY\t\t:\t" << RY << endl;
				}
			}
		}
		
		if (enable==1 && knob3 == OFF)
		   printRegisterFile();
		
		cout << "******--------------------------------------------------*****" << endl;
	}
	//End of runCode
};

//Update memory with data & instructions
void updateMemory(int enable)
{
	string machineLine;
	string machineCode;
	fstream fileReading;

	map<char, int> hexadecimal;
    int i=0;
	while(i<=9){
      hexadecimal[i + '0'] = i;
	  i++;;
	}
    i=0;
	while(i<=6){
    hexadecimal[i + 'A'] = i + 10;
	i++;
	}
if(enable==1){
	fileReading.open("machineData.txt");
	while (getline(fileReading, machineLine))
	{
		lli value = 0, address = 0;
		string type = "";
		int i = 0;
		if(enable==1){
		while (machineLine[i] != ' ')
			value = value * 10 + (machineLine[i++] - '0');
		i = i + 3;
		}
		if(enable==1){
		while (machineLine[i] != ' ')
			address = address * 16 + hexadecimal[machineLine[i++]];
		i++;
		}
        if(enable==1){
		while (i < machineLine.length() && machineLine[i] != ' ')
			type += machineLine[i++];
        }
        if(enable==1){
		if (type == "byte")
			readWriteMemory(0, 1, address, value);
		else if (type == "halfword")
			readWriteMemory(0, 2, address, value);
		else if (type == "word")
			readWriteMemory(0, 3, address, value);
		else if (type == "doubleword")
			readWriteMemory(0, 4, address, value);
        }
	}
	fileReading.close();
}
	fileReading.open("machineCode.mc");
	lli address = 0;
	while (getline(fileReading, machineLine))
	{
		lli value = 0;
		int i = 2;
        while (i < machineLine.length())
		 value = value * 16 + hexadecimal[machineLine[i++]];
		readWriteMemory(0, 3, address, value);
		address += 4;
    }

    if(enable==0)
    {
        cout<<"Error in Update Memory Stage 2"<<endl;
    }
	else if(enable==1 && knob1 == ON)
	{
		lli value = 0x00000033;
        int i=0;
        while(i<2)
        {   readWriteMemory(0, 3, address, value);
			address += 4;
            i++;
		}
	}
	fileReading.close();
} 

//main function
int main()
{
	regArray[2] = 0xFFFFFF; //initialize x2, x3
	regArray[3] = 0x100000;

	fstream fileWriting; //Clear data of existing registerFile.txt
	fileWriting.open("registerFile.txt", ios::out);
	fileWriting.close();
	fileWriting.open("stats.txt", ios::out);  // Clear data of existing stats.txt
	fileWriting.close();
    cout << "*****------------------------------------------------------*****" << endl;
	cout << "To Turn OFF the KNOB , Press 0" << endl;
	cout << "To Turn ON the KNOB, Press 1" << endl;
	cout << "*****------------------------------------------------------*****" << endl;
	cout << "Knob 1 (Pipeline Knob)                  :  ";
    cin >> knob1;
	updateMemory(1);
	 //Update memory with data & instructions
	if (knob1 == ON)
	{
		pipelined execute_pipeline;
		execute_pipeline.runCode();
	}
	else
	{
		unpipelined execute_unpipeline;
		execute_unpipeline.runCode(1);
	}
	stats_print();
	printMemory();
}
//End of main
