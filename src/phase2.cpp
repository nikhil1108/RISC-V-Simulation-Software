/*
// The project is developed as part of Computer Architecture class
// Project Name: Functional Simulator for subset of RISCV Processor

Developer's Name: 
Abhishek Balu: 2018csb1065
Dilpreet Singh: 2018csb1085
Nikhil: 2018csb1108
Rasanpreet Singh: 2018csb1173

Date:23-03-2020
*/

#include <bits/stdc++.h>
#define lli long long int
using namespace std;
//CONSTANT......
#define max 100
#define max1 100

#define OPCOD_I1 3   // LOAD INSTRUCTION
#define OPCOD_I2 19  // ANDI SHIFTI ORI
#define OPCOD_I3 27  // SHIFTW ANDIMMEDIATE WORD
#define OPCOD_I4 103 // JALR

# define max2 100

#define OPCOD_U1 23 // AUIPC
#define OPCOD_U2 55 // LUI

#define OPCOD_S1 35 //STORE DOUBLE STORE HALF SORE WORD

#define OPCOD_R1 51 //ADD SUB
#define OPCOD_R2 59 // ADDWORD SUBWORD

#define OPCOD_SB1 99 //BRANCHJUMP

#define OPCOD_UJ 111 //JUMPANDLINK
struct cache
{
	bool valid_bit;
	unsigned char data[4];  // block with size of 8 bytes or simply  2 words
	unsigned int tag;// with fifteen bits
	cache(){
		valid_bit=0;
	}
};
cache Cache[1<<14];
unsigned int PC=0; //programm counter
# define SELECT 1
int memaccess;
int conflictmiss,coldmiss; //
int instreg; //instruction register
int immediate;
unsigned char memory[1 << 24]; //Processor Memory
int regArray[32] = {0};//array of size 32
unsigned int returnAddress; //Return Address in case of jal/jalr
int cycleCount = 0;//cycle count initialized to zero
int RA, RB, RZ, RY, RM; //Interstage Buffers
unsigned int addressA, addressB;//

int addressC;				//destination register


//following are the control signals
int ALU_OP, B_SELECT, PC_SELECT, INC_SELECT, Y_SELECT;
int MEM_READ;
int MEM_WRITE;
int RF_WRITE;
# define SELECT 1
// READ WRITE CACHE
int readWriteCache(int MEM_READ, int MEM_WRITE, int address=0, int data_w=0)
{
	int MOD =0;        // initializing MOD = 0
	MOD = 1<<14;       // left shift by 14
	unsigned int temp;
	temp = address>>2; // right shift by 2
	unsigned int Index ;
	Index = temp%MOD;
	unsigned int Tag ;
	Tag = temp>>14;
	unsigned int i1;
	 i1 = (address)%4;
	unsigned int i2;
	i2 = (address+1)%4;
	unsigned int i3;
	i3 = (address+2)%4;
	unsigned int i4;
	 i4 = (address+3)%4;
	if(MEM_READ>0)      //check if memory read is greater than zero
	{
		memaccess = memaccess+1; //increment memaccess by 1
		if(Cache[Index].valid_bit==1 && Cache[Index].tag==Tag)
		{
			if(MEM_READ==1)
			{
				int a = (int)Cache[Index].data[i1];
				return a;

			}

			if(MEM_READ==2)
			{
				int x = Cache[Index].data[i1];
				int x1;
				x1 = (int)((Cache[Index].data[i2])<<8);
				x = x + x1 ;
				return x;
			}

			if(MEM_READ==3)
			{
		      	int x = Cache[Index].data[i1];
				int y;
				int x1;
				x1=(int)Cache[Index].data[i2]<<8;
				int x2;
				x2 = (int)Cache[Index].data[i3]<<16;
				int x3;
				x3=(int)Cache[Index].data[i4]<<24;
				x = x + x1;
				x = x + x2;
				x = x + x3;
	            y=x;
				return y;

			}
		}

		else if((MEM_READ!=1) && (MEM_READ!=2) && (MEM_READ!=3))
		{
			int x1 = Cache[Index].valid_bit;
			if( x1==0)
			{
				int y1= coldmiss;
				y1++;
				coldmiss=y1; // increment coldmisses by 1
			}
			if(x1==1 && Cache[Index].tag!=Tag)
			{
				int y2=conflictmiss;
				y2++;
				conflictmiss = y2;// increment connflictmisses by 1
			}
             int c1,c2,c3,c4;
			 c1==memory[address];
			 c2=memory[address+1];
			 c3=memory[address+2];
			 c4= memory[address+3];
			Cache[Index].valid_bit=1;
			Cache[Index].tag = Tag;
			Cache[Index].data[i1]=c1;
			Cache[Index].data[i2]=c2;
			Cache[Index].data[i3]=c3;
			Cache[Index].data[i4]=c4;
			int y0= MEM_READ;
			if(y0 == 1)
				return memory[address];
			if(y0 == 2)
			{
				 memory[address]= memory[address] +( (int)memory[address + 1] << 8);
				return memory[address];
			}
			if(y0==3)
			{
				 memory[address]+= (int)memory[address+1]<<8;
				  memory[address]+= (int)memory[address+2]<<16;
				  memory[address]+= (int)memory[address+3]<<24;
				return memory[address];
			}
		}
	}
	else
	{
		int h1;
		h1=MEM_WRITE;
		int d1,d2,d3,d4;
			 d1==memory[address];
			 d2=memory[address+1];
			 d3=memory[address+2];
			 d4= memory[address+3];

		if(h1==1)
		{
			memory[address]=data_w;
			if(Cache[Index].valid_bit!=0)
			{
                Cache[Index].tag = Tag;
				Cache[Index].data[i1]=data_w;

			}
			else
			{
				Cache[Index].valid_bit=1;
				Cache[Index].tag = Tag;
				Cache[Index].data[i1]=data_w;
				Cache[Index].data[i2]=d2;
				Cache[Index].data[i3]=d3;
				Cache[Index].data[i4]=d4;
			}
		}
		if(h1==2)
		{
			memory[address] = data_w & ((1 << 8) - 1);//256-1=255
			int d1;
			d1= data_w>>8;
			memory[address + 1] = d1;
			if(Cache[Index].valid_bit==0)
			{
				Cache[Index].valid_bit=1;
				Cache[Index].tag = Tag;
				Cache[Index].data[i1]=data_w;
				Cache[Index].data[i2]=d2;
				Cache[Index].data[i3]=d3;
				Cache[Index].data[i4]=d4;
			}
			else
			{
				Cache[Index].tag = Tag;
				Cache[Index].data[i1]=d1;
				Cache[Index].data[i2]=d2;
			}
		}
		if(MEM_WRITE==3)
		{
			int  setb8;
			int h;
			h = (1 << 8) - 1;// 256-1= 255 by left shifting
			setb8 = h;
			memory[address] = data_w & setb8;
			int h1, h2 , h3;
			h1=(data_w & (setb8 << 8)) >> 8;
			h2 = (data_w & (setb8 << 16)) >> 16;
			h3=data_w >> 24;

			memory[address + 1] = h1;
			memory[address + 2] = h2 ;
			memory[address + 3] = h3;

			if(Cache[Index].valid_bit==0) //bool cache :: valid_bit
			{

				Cache[Index].valid_bit=1;
				Cache[Index].tag = Tag;
				Cache[Index].data[i1]=data_w & setb8;
				Cache[Index].data[i2]=h1;
				Cache[Index].data[i3]=h2;
				Cache[Index].data[i4]=h3;
			}
			else // the else part if cache[Index].valid_bit!=0 then
			{
				Cache[Index].tag = Tag;
				Cache[Index].data[i1]=data_w & setb8;
				Cache[Index].data[i2]=h1;
				Cache[Index].data[i3]=h2;
				Cache[Index].data[i4]=h3;
			}
		}
	}
	return 0;
}
// FUNCTION
// READ AND WRITE BACK IN  REGISTER FILE
// CALL IN DECODE STAGE
// CALL IN WRITE BACK TO REGISTER FILE

//MEMORY READ WRITE MEMORY INTERFACE
void readWriteRegFile(int RF_WRITE, int addressA, int addressB, int addressC)
{
	int reg;
	reg =RF_WRITE;
	if (reg == 1)
	{
		if (addressC)
			regArray[addressC] = RY; // RY INTERSTAGE BUFFER
		return;
	}

	RA = regArray[addressA]; // UPDATE INTERSTAGE BUFFER
	RB = regArray[addressB];
}//EOF READ AND WRITE REG FILE


//MEMORY READ WRITE MEMORY INTERFACE
int readWriteMemory(int MEM_READ, int MEM_WRITE, int address = 0, int data_w = 0 , int enable=1)
{
	int r1,r2,r3;
	r1=(int)memory[address + 1] << 8;
	r2=(int)memory[address + 2] << 16;
	r3=(int)memory[address + 3] << 24;
	if(MEM_READ < 0 )
	{
		return 0;
	}

	if (MEM_READ > 0 && enable==1)
	{
		if (MEM_READ == 3) //lb
           {
			   //lw
            int data = memory[address];
            data += r1;
            data += r2;
            data += r3;
            return data;
		   }
        else if (MEM_READ == 2)
        { //lh
            int data = memory[address];
            data += r1;
            return data;
        }
        else if (MEM_READ == 1)
        {
			return memory[address];
        }

	}
	if (MEM_WRITE == 1)// STORE BYTE
	{
		memory[address] = data_w;
	}
	else if (MEM_WRITE == 2)//STORE HALF
	{
		memory[address] = data_w & ((1 << 8) - 1);
		memory[address + 1] = data_w >> 8;
	}
	else if (MEM_WRITE == 3)// STORE WORD
	{
		int b1 = (1 << 8) - 1;
		int b2=b1<<8;
		int b3=b1<<16;
		memory[address] = data_w & b1;
		memory[address + 1] = (data_w & b2) >> 8;
		memory[address + 2] = (data_w & b3) >> 16;
		memory[address + 3] = data_w >> 24;
	}
	return 0;
}
// FINISHES THE READ WRITE MEMORY

// FUNCTION TO RETURN TH RETURNADDRESS
// ISTRUCTION ADDRESS GENERATOR

lli iag(int INC_SELECT, int PC_SELECT, lli immediate = 0)
{
	lli PC_Temp ;
	PC_Temp = PC + 4;
	if (PC_SELECT !=0)
	{
		if (INC_SELECT==1)
		{
			PC = PC  + immediate;
		}
		else
		    PC = PC + 4;
	}
	else
	{
		PC = RZ;
	}

	return PC_Temp;
}

// STAGE 1
// INSTRUCTION FEETCH
void fetch()
{
	instreg = readWriteCache(3, 0, PC);
	returnAddress = iag(0, 1);
}
// next stage
void decode()
{
  if(SELECT==-1)
  {
	  cout<<"error";
  }
  else
  {
	  unsigned int opcode,funct3,funct7;
	int e1 = instreg << 25;
	opcode=e1;
    opcode =opcode>> 25;
	int e2 = instreg << 17;
	funct3=e2;
    funct3 =funct3>> 29;
	int e3 = instreg >> 25;
	funct7=e3;
    PC_SELECT = 1;
    INC_SELECT = 0;
    Y_SELECT = 0;
	if(opcode==OPCOD_I1 || opcode==OPCOD_I2 || opcode==OPCOD_I3 || opcode==OPCOD_I4 )
	{
		if (opcode == OPCOD_I1 && SELECT==1)
      {
        RF_WRITE = 1;
		int p1,p2,p3;
        int imm = instreg >> 20;
		immediate = imm;
        unsigned int rs1;
		p1 = instreg << 12;
		rs1=p1;
        rs1 =rs1 >> 27;
		addressA = rs1;
        unsigned int rd;
		p2 = instreg << 20;
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
        MEM_WRITE = 0;

		}
		else
            MEM_READ = 1;

     }

     else if (opcode == OPCOD_I2 && SELECT == 1)
     {
        RF_WRITE = 1;
		int od1,od2,od3;
        int imm;
		od1 = instreg >> 20;
		imm=od1;
		immediate=imm;
        unsigned int rs1;
		od2 = instreg << 12;
		rs1=od2;
        rs1 =rs1>> 27;
		addressA = rs1;
        unsigned int rd;
		od3 = instreg << 20;
		rd=od3;
        rd >>= 27;
		addressC = rd;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        if (funct3 != 0 && SELECT==1)
     {

           if (funct3 == 1&& SELECT==1) //slli
        {
            unsigned int st = instreg << 7;
            st =st>> 27;
            immediate = st;
            ALU_OP = 7;
        }

        else if (funct3 == 2) //slti
        {
            ALU_OP = 20;
        }

        else if (funct3 == 3) //sltiu
        {
            ALU_OP = 20;
        }

        else if (funct3 == 4) //xori
        {
            ALU_OP = 8;
        }

        else if (funct3 == 5) //srli
        {   int f1;
            unsigned int shamt;
			f1 = instreg << 7;
			shamt=f1;
            shamt >>= 27;
            immediate = shamt;
            if (funct7 != 0)
                ALU_OP = 21;
            else
                ALU_OP = 19;
        }
        else if (funct3 == 6) //ori
        {
            ALU_OP = 6;
        }
        else if (funct3 == 7) //andi
        {
            ALU_OP = 1;
        }
      }

        else if(funct3==0) //addi
		{
			ALU_OP= 0;
		}
     }

     else if (SELECT==1 && opcode == OPCOD_I3)
     {
		 int s1,s2,s3,s4;
		 s1=instreg << 12;
         s2=instreg << 20;
        RF_WRITE = 1;
        addressA = s1;
        addressA =addressA >> 27;
        addressC = s2;
        addressC = addressC >> 27;
        MEM_READ = 0;
        MEM_WRITE = 0;
        unsigned int shamt = instreg << 7;
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
			immediate=instreg>>20;
			ALU_OP=0;
		}
     }

     else if (opcode == OPCOD_I4 && SELECT==1)
	 { //for jalr
        RF_WRITE = 1;
        int imm = instreg >> 20;
        unsigned int rs1 = instreg << 12;
        rs1 =rs1 >> 27;
		addressA = rs1;
        unsigned int rd = instreg << 20;
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

    else if (opcode == OPCOD_S1 && SELECT==1) // instructions for store
    {
        int t1;
		t1= (1 << 5) - 1;
        int tmp;
        tmp=t1;
        tmp <<= 7;
        int imm1 = instreg & tmp;
        imm1 =imm1 >> 7;
        int imm2 = instreg >> 25;
		int t2;
		t2=(imm2 << 5);
        immediate = imm1 + t2;
        addressB = instreg << 7;
        addressB =addressB>> 27;
        addressA = instreg << 12;
        addressA >>= 27;
		  MEM_READ = 0;
         B_SELECT = 1;
         RF_WRITE = 0;
         ALU_OP = 0;
        if (funct3!=0)
		{
			if(funct3 == 1 && SELECT==1)
            MEM_WRITE = 2;
         else if (funct3 == 2 && SELECT==1)
            MEM_WRITE = 3;
         else if (funct3 == 3&& SELECT==1)
            MEM_WRITE = 4;
		}
        else if (funct3 == 0)
            MEM_WRITE = 1;
    }

    else if (opcode == OPCOD_U1 && SELECT==1) //auipc
    {   int u1,u2;
	    u1=instreg>>12;
		u2=instreg<<20;
        immediate = u1;
		u2>>=27;
        addressC = u2;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
        ALU_OP = 12;
    }

    else if (opcode == OPCOD_U2 && SELECT==1) //lui
    {
		int o1;
		o1=instreg>>12;
		 immediate = o1;
         addressC = instreg << 20;
        addressC =addressC>> 27;
        ALU_OP = 13;
        B_SELECT = 1;
        MEM_READ = 0;
        MEM_WRITE = 0;
        RF_WRITE = 1;
    }

    else if (opcode == OPCOD_R1 || opcode == OPCOD_R2)
    {   int k;
        unsigned int rs1;
		int r1 = instreg << 12;
		rs1=r1;
        rs1 =rs1 >> 27;
        unsigned int rs2;
		int r2 = instreg << 7;
		rs2 = r2;
        rs2 = rs2>> 27;
        unsigned int rd = instreg << 20;
        rd =rd>> 27;
        RF_WRITE = 1;
        B_SELECT = 0;
        if (funct3 == 0)
        {
            if (funct7 != 0)
                { if (funct7 != 1)
                   ALU_OP = 18; //SUB
				   else
				   {
					   ALU_OP=25;//mul
				   }

				}
            else if(funct7==0)
                ALU_OP = 0; //add,addw
        }

        else if (funct3 == 3)
            ALU_OP = 7; //sltu

        else if (funct3 == 2)
            ALU_OP = 20; //slt

        else if (funct3 == 1)
            ALU_OP = 20; //sll,sllw

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

    else if (opcode == OPCOD_UJ && SELECT==1)
    {
         unsigned int rd = instreg << 20;
        rd >>= 27;
        addressC = rd;
        bitset<20> tmp2(instreg >> 12), res;
		int i=18;
        while ( i>=11)
      {  res[i] = tmp2[i-11];
	     i--;
	  }
	  res[10]=tmp2[8];
        for (int i = 9; i >= 0; --i)
            res[i] = tmp2[i + 9];
        int tmp1 = res.to_ulong();
		int kl;
		kl=tmp2[19];
        if (kl)
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

    else if (opcode == OPCOD_SB1 && SELECT==1)
    {
        unsigned int rs1 ;
		int s1= instreg << 12;
		rs1=s1;
        rs1 =rs1>> 27;
        unsigned int rs2 ;
		int s2= instreg << 7;
		rs2=s2;
        rs2 =rs2 >> 27;
        unsigned int rd ;
		int s3= instreg << 20;
        rd =s3;
		rd=rd>> 27;
        int bit_11 = (rd & 1) << 10;
        int bit_1_4 = rd >> 1;
		int j=(funct7 >> 6);
        int bit_12 =  j<< 11;
		int j1=(bit_12 << 6);
        int bit_5_10 = (funct7 - j1) << 4;
        immediate = bit_1_4 | bit_5_10 | bit_11 | bit_12;
        immediate =immediate << 1;

        RF_WRITE = 0;
        B_SELECT = 0;
        if (funct3!=1)
		{
			if (funct3 == 0 && SELECT==1) //beq
            ALU_OP = 2;
           else if (funct3 == 4 && SELECT==1) //blt
            ALU_OP = 4;
           else if(funct3 == 5 && SELECT==1){   //bge
            ALU_OP = 3; }
           else if (funct3 == 6 && SELECT==1) //bltu
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
    readWriteRegFile(0, addressA, addressB, addressC);
}
}

//End of decode

/* Arithmetic Logic Unit
Input: ALU_OP, MUXB select, immediate(if any)
(these input will be provided by decode stage)
Updates RZ */


void post_alu(int ALU_OP, int B,int immediate = 0,int enable=1)
{
    int InA = 0;
	int InB=B;

    //cout<<"XXXXXXXHELLO"<<endl;
    if(enable==1){

    InA=RA;
	if (enable==1&&ALU_OP == 0) //addi,load,
	{
		RZ = InA + InB;
	}

	else if (enable ==1&&ALU_OP == 1) //andi
		RZ = InA & InB;

	else if (enable ==1&&ALU_OP == 2) //beq
	{
		if (InA == InB)
		{
			INC_SELECT=1;
		//	INC_SELECT = 1;
			PC -= 4;
			iag(INC_SELECT, 1, immediate);
		}
	}
	else if (enable ==1&&ALU_OP == 3) //bge
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

	else if (enable ==1&&ALU_OP == 18) //sub
		RZ = InA - InB;

	else if (enable ==1&&ALU_OP == 8) //xori
		RZ = InA ^ InB;

	else if (enable ==1&&ALU_OP == 12) //auipc
	{
		RZ = PC - 4 + (InB << 12);
	}
	else if (enable ==1&&ALU_OP == 13) //lui
		RZ = InB << 12;

	else if (enable ==1&&ALU_OP == 19) //srli
		RZ = InA >> InB;

	else if (enable ==1&&ALU_OP == 20) //slti,sltiu
		RZ = (InA < InB) ? 1 : 0;

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

	else if (enable ==1&&ALU_OP == 29 || enable ==1&&ALU_OP == 30) //div, divu
		RZ = RA / RB;

	else if (enable ==1&&ALU_OP == 31 || enable ==1&&ALU_OP == 32) // rem, remu
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
    if(enable==-1){
        cout<<"error in pre_alu";

    }
    else{
            if (B_SELECT == 0){

		InB = RB;
	}
	else
		{InB = immediate;}
        post_alu(ALU_OP,InB,immediate,1);
    }

}

//end of ALU function
void starter(){
    coldmiss=0;
     memaccess=0;
	conflictmiss=0;

	}
/*Stage 4 */
void memoryStage(int enable,int Y_SELECT, int MEM_READ, int MEM_WRITE, int address = 0, int data = 0)
{
	int dataFromMem = readWriteCache(MEM_READ, MEM_WRITE, address, data);

		if(enable==1){
	if (Y_SELECT == 2)
		RY = returnAddress;}
		if(enable==1){
	if (Y_SELECT == 1)
		RY = dataFromMem;}
		if(enable==1){
	if (Y_SELECT == 0)
		RY = RZ;}

}

void updateMemory( int enable=1)
{
	string machineLine;
	string machineCode;
	fstream fileReading;

	map<char, int> hexadecimal;
    int i=0;
	while(i<=9){
    hexadecimal[i + '0'] = i;
	++i;
	}
    i=0;
	while(i<=6){
    hexadecimal[i + 'A'] = i + 10;
	++i;
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
		if (type == "byte"&&enable==1)
			readWriteCache(0, 1, address, value);
		else if (type == "halfword"&&enable==1)
			readWriteCache(0, 2, address, value);
		else if (type == "word"&&enable==1)
			readWriteCache(0, 3, address, value);
		else if (type == "doubleword"&&enable==1)
			readWriteCache(0, 4, address, value);
        }
	}
	fileReading.close();
}
	fileReading.open("machineCode.mc");
	lli address = 0;
	if(enable==1){
	while (getline(fileReading, machineLine))
	{
		lli value = 0;

		int i = 2; //So basically we have initially : 0x
				   //       hence we have  while (machineLine[i] != ' ')
				   //  hence    we  have      address = address * 16 + hexadecimal[machineLine[i++]];

		//      so this  i += 3; //between : 0x
	if(enable==1){	while (i < machineLine.length()){
			value = value * 16 ;
			value = value + hexadecimal[machineLine[i++]];}
if(enable==1){
		readWriteCache(0, 3, address, value);
		address += 4;}
	}}
	}
	fileReading.close();
}

//Prints memory that has been alloted with data or instruction!

//End of printMemory


//End of print RegisterFile

//Run Instructions: unpipelined
void runCode(int enable=1)
{int cho;


	if (enable==1)cout << "*---------------------------------------------------*" << endl;
	if (enable==1)cout << "Please press the 1 for the whole program to run " << endl;
	if (enable==1)cout << "Please press the 2 for the program to run step by step" << endl;
	cin >> cho;
	cout << "*--------------------------------------------------*" << endl;

    if (enable==1){
	switch (cho)
	{
	case 1:
	{
		while (2)
		{

			if (enable==1&& memory[PC + 3] == 0)
            {
                if(enable==0){cout<<"Error"<<endl;}
                else if(memory[PC] == 0 && memory[PC + 2] == 0 ){
                        if(memory[PC + 3] == 0&& memory[PC + 1] == 0){
                    break;}
                }
            }

            fetch();
			decode();
			if (enable==1){
			alu(ALU_OP,immediate, B_SELECT,1);
			memoryStage(1,Y_SELECT, MEM_READ, MEM_WRITE, RZ, RB);
			readWriteRegFile(RF_WRITE, 0, 0, addressC);
			cycleCount++;
			}
		}
		break;
	}
	case 2:
	{
		int pr_info = 0;
		while (3&&enable==1)
		{
			if(enable==1){cout << " 0 button ---> move on" << endl;}
			if(enable==1){cout << " 1 button ---> print register file " << endl;}
			if(enable==1){cout << " 2 button ---> print memory" << endl;}
			if(enable==1){cout << " 3 button ---> execute to end" << endl;}
			if(enable==1){cout << "*-------------------------------------------------*" << endl;}
			if(enable==1){cin >> pr_info;}
			if (enable==1&&pr_info == 1)
			{
				cout << "*----------REGISTER FILE-----------------*" << endl;
           int  i=0;
            while(i<32){
		cout << "REGISTER x" << i << "\t" << regArray[i] << endl;
		i++;
		}
	cout << "-------------------------------------------" << endl;
				continue;
			}
			else if (enable==1&&pr_info == 2)
			{
				cout << "*----------------mmry--------------------*"<< endl;
	 int  i=0;
            while(i<1<<22){
		if (memory[i] != '\0')
			cout << i << "\t" << (int)memory[i] << endl;
			i++;}
	cout << "-------------------------------------------" << endl;
				continue;
			}
			else if (enable==1&&pr_info == 0)
			{

				if (enable==1&& memory[PC + 3] == 0)
                {
                      if(enable==0){cout<<"Error"<<endl;}
                    else if(memory[PC] == 0 && memory[PC + 1] == 0 && memory[PC + 2] == 0)
                    {
                        break;
                    }
                }

				if(enable==1){
				fetch();
				decode();
				alu(ALU_OP,immediate, B_SELECT,1);
				memoryStage(1,Y_SELECT, MEM_READ, MEM_WRITE, RZ, RB);
				readWriteRegFile(RF_WRITE, 0, 0, addressC);
				cycleCount++;
			}
		}
			else if (enable==1&&pr_info == 3)
			{
				while (2&&enable==1)
				{
					if (enable==1&& memory[PC + 3] == 0)
                        {
                              if(enable==0){cout<<"Error"<<endl;}
                            else if(memory[PC + 1] == 0 && memory[PC + 2] == 0&&memory[PC] == 0){
						break;
						}}
						if(enable==1){
					fetch();
					if(enable==1){decode();}
					alu(ALU_OP,immediate ,B_SELECT,1);
					if(enable==1){

                            memoryStage(1,Y_SELECT, MEM_READ, MEM_WRITE, RZ, RB);
                    }
					readWriteRegFile(RF_WRITE, 0, 0, addressC);
					cycleCount++;
				}}
			break;
			}
		}
		break;
	}
	default:
		cout << "Wrong" << endl;
	}
}
}

//End of runCode

//main function
int main()
{

	starter();
	//initialize x2, x3
	regArray[2] = 0xFFFFFF;
	regArray[3] = 0x100000;


	updateMemory(1); //Update memory with data & instructions

	runCode(1);

	cout << "-------------Register File-----------------" << endl;
	 int  i=0;
            while(i<32){
		cout << "REGISTER x" << i << "\t" << regArray[i] << endl;
		i++;}
	cout << "-------------------------------------------" << endl;
	cout<<"Number of cold misses: "<<coldmiss<<endl;
	cout<<"Number of conflict misses: "<<conflictmiss<<endl;
	cout << "Number of Cycles = " << cycleCount << endl;
}
