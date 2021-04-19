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


//Task 1: Assembly Code to Machine Code
#include <bits/stdc++.h>     // It will include all the libraries of c++
#include <unordered_map>
using namespace std;
#define lli long long int

//to store variables and addresses defined in .data segment
struct data{
	string var,hexaddress;
};

//User defined data type: to store line no of each label
struct labelData{
	 lli lineNumber;
	 string label;
};

//Function to convert integer number to binary string 
string dec2Binary(lli decNum, int length){
	
	int l=length;
	lli positiveDecNum;
       if(decNum<0)
	   positiveDecNum=decNum*(-1);   // If the number is negative then make it positive

	   else
	       positiveDecNum=decNum;

	string binaryNum="";
	while(l--)
		binaryNum=binaryNum+"0";     // Will initialize binaryNum to 0 (l times 0 , 000____l)
	
	int i=0;
	
	for( ; positiveDecNum>0; positiveDecNum=positiveDecNum/2) {
       		binaryNum[length-i-1]=positiveDecNum%2+'0'; 
       		 i++; 
    	}
	if(decNum<0){
    		int j=0;
			while(j<length) {
			 
			  if(binaryNum[j]=='1')
    				binaryNum[j]='0';
    			else
    				binaryNum[j]='1';

				j++;
			  }
    		
			int carry=1;
    		for(int j=length-1;j>=0;j--){
    			if( binaryNum[j]=='0' && carry==1 ){
    				binaryNum[j]='1';
				break;
			}
    			else if( binaryNum[j]=='1' && carry==1)
    				binaryNum[j]='0';
		}
	}
    return binaryNum;
}

//Function to create map between binary number and its equivalent hexadecimal 
void createMap(unordered_map<string, char> *bh){ 
    (*bh)["0000"] = '0'; 
    (*bh)["0001"] = '1'; 
    (*bh)["0010"] = '2'; 
    (*bh)["0011"] = '3'; 
    (*bh)["0100"] = '4'; 
    (*bh)["0101"] = '5'; 
    (*bh)["0110"] = '6'; 
    (*bh)["0111"] = '7'; 
    (*bh)["1000"] = '8'; 
    (*bh)["1001"] = '9'; 
    (*bh)["1010"] = 'A'; 
    (*bh)["1011"] = 'B'; 
    (*bh)["1100"] = 'C'; 
    (*bh)["1101"] = 'D'; 
    (*bh)["1110"] = 'E'; 
    (*bh)["1111"] = 'F'; 
}

 
//Functiom to convert binary string to hexadecimal string 
string bin2Hex(string bin){ 
    int l=bin.size(); 
    int t=bin.find_first_of('.');    // if bin=11.0001 then t=2, if there is no '.' then b will be -1
    
	int len_left;    // length of string before '.'
    if(t!=-1)
    len_left=t;
    else
    len_left=l;
	
	int i;
    
	// add min 0's in the beginning to make 
    // left substring length divisible by 4  
    for(i=1;i<=(4-len_left%4)%4;i++) 
        bin='0'+bin;   
    
	if(t!=-1){
        int len_right=l-len_left-1; 
        for(i=1;i<=(4-len_right%4)%4;i++) 
            bin=bin+'0';
    }
    unordered_map<string, char> bin_hex_map; 
    createMap(&bin_hex_map); 
      
    i=0; 
    string hex= "";
    while(1){ 
        hex=hex+bin_hex_map[bin.substr(i, 4)]; 
        i=i+4; 
        if(i==bin.size()) 
        	break;
        if(bin.at(i)=='.'){ 
            hex += '.'; 
            i++; 
        }
    } 
    return hex;     
}

//Function to extract RS1, RS2 & RD of R type instructions
string Rtype(string &mcodeInstBin, string &line, int i, string &rd, string &rs1, string &rs2){
	
	while((i<line.size()) && line[i]!='x'){
		if(line[i]!=' '){
			cout<<"Invalid Register\n";
			return "error";
		}
		i++;
	}
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	if(line[i]!='9' && line[i]!='8' && line[i]!='7' && line[i]!='6' && line[i]!='5' && line[i]!='4' && line[i]!='3' && line[i]!='2' && line[i]!='1' && line[i]!='0'){
		cout<<"Invalid Register\n";
		return "error";
	}

	int temp=0;

	while(line[i]!=' ' && line[i]!=','){
		if(line[i]!='9' && line[i]!='8' && line[i]!='7' && line[i]!='6' && line[i]!='5' && line[i]!='4' && line[i]!='3' && line[i]!='2' && line[i]!='1' && line[i]!='0'){
			cout<<"Invalid Register\n";
			return "error";
		}
		temp=temp*10+(int)(line[i]-'0');
		i++;
	}
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	rd=dec2Binary(temp, 5);

	temp=0;
	while(i<line.size() && line[i]!='x'){
		if(line[i]!=',' && line[i]!=' '){
			cout<<"Invalid Register\n";
			return "error";
		}
		i++;
	}
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	if(line[i]!='9' && line[i]!='8' && line[i]!='7' && line[i]!='6' && line[i]!='5' && line[i]!='4' && line[i]!='3' && line[i]!='2' && line[i]!='1' && line[i]!='0'){
		cout<<"Invalid Register\n";
		return "error";
	}
	while(line[i]!=' ' && line[i]!=','){
		if(line[i]!='9' && line[i]!='8' && line[i]!='7' && line[i]!='6' && line[i]!='5' && line[i]!='4' && line[i]!='3' && line[i]!='2' && line[i]!='1' && line[i]!='0'){
			cout<<"Invalid Register\n";
			return "error";
		}
		temp=temp*10+(int)(line[i]-'0');
		i++;
	}
	if(temp>31){
		cout<<"Invalid Register\n";
		return "error";
	}
	rs1=dec2Binary(temp, 5);

	temp=0;
	while(line[i]!='x' && i<line.size()){
		if(line[i]!=' ' && line[i]!=','){
			cout<<"Invalid Register\n";
			return "error";
		}
		i++;
	}
	if(line[i]!='x'){
		cout<<"Invalid Register"<<endl;
		return "error";
	}
	i++;
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Register\n";
		return "error";
	}

	while (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
			temp = temp * 10 + (int)(line[i] - '0');
			i++;
     }
		
	if(temp>=32){
		cout<<"Invalid Register"<<endl;
		return "error";
	}
	rs2=dec2Binary(temp, 5);
	return "valid";
}


//Function to extract rs, rd and immediate values
string Itype(string &line, string &immediate, int ISubType, string &rs1, int i, string &rd){
	
   // read destination register
	while (line[i] != 'x' && i<line.size() && line[i] !='s'){
		if(line[i]!=' '){
			cout<<"Invalid Register\n";
			return "error";
		}
		i++;
	}
	if(line[i]!='x' && line[i]!='s'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;

	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9' && line[i]!='p'){
		cout<<"Invalid Register\n";
		return "error";
	}

	 int temp = 0;

	while(line[i]!=' ' && line[i]!=','){

		if(line[i]=='p'){
			temp=2;
			i++;
			break;
		}
		if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
			cout<<"Invalid Register\n";
			return "error";
		}
		temp=temp*10+(int)(line[i]-'0');
		i=i+1;
	}
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	rd = dec2Binary(temp, 5);

	temp = 0;
	if(ISubType == 0 ){ //immediate value

		temp = 0;
		//read source register
		
	while (line[i] != 'x' && i<line.size()){
		if(line[i]=='s')
		  break;
		if(line[i]!=' ' && line[i]!=','){
			cout<<"Invalid Register\n";
			return "error";
		}
		i++;
	 }
	  
		i++;
		if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9' && line[i]!='p'){
			cout<<"Invalid Register\n";
			return "error";
		}
		for(; line[i]!=',' && line[i]!=' '; i++){

			if(line[i]=='s' || line[i]=='p'){
			temp=2;
			i++;
			break;
		  }
			if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
				cout<<"Invalid Register\n";
				return "error";
			}
			temp = temp * 10 + (int)(line[i] - '0');
		}
		if(temp>=32){
			cout<<"Invalid Register\n";
			return "error";
		}
		rs1 = dec2Binary(temp, 5);

		int flag1=0;

		// skip spaces or commas
		while (i<line.size() && (line[i]==' ' || line[i]==',')){
			i++;
		}
		//for negative offset
		if(line[i]=='-'){
			flag1=1;
			i++;
		}
		//read offset
		if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
			cout<<"Invalid Instruction Format\n";
			return "error";
		}
		temp = 0;
		while (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
			temp = temp * 10 + (int)(line[i] - '0');
			i++;
		}
		if(flag1==1){           // For negative offset
			temp=temp*-1;
		}
		
		if(temp<-2048){
			cout<<"Out of Range Immediate\n";
			return "error";
		}
		if(temp>2047){
			cout<<"Out of Range Immediate\n";
			return "error";
		}
		immediate = dec2Binary(temp, 12);
		return "valid";
	}
	else if(ISubType == 1){	//with offset
		temp = 0;
		//skip spaces and commas
		while (line[i]==',' || line[i]==' '){
			i++;
		}
		//calculate offset
		if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
			cout<<"Invalid Instruction Format\n";
			return "error";
		}
		while (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
			temp = temp * 10 + (int)(line[i] - '0');
			i++;
		}
		if(temp<-2048||temp>2047){
			cout<<"Out of Range Immediate\n";
			return "error";
		}
		immediate = dec2Binary(temp, 12);

		//read source register
		while(line[i]==' '){
			i++;
		}
		if(line[i]!='('){
			cout<<"Invalid Instruction Format\n";
			return "error";
		}
		i++;
		if(line[i]!='x' && line[i]!='s'){
			cout<<"Invalid Instruction Format\n";
			return "error";
		}
		temp = 0;
		i++;
		if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9' && line[i]!='p'){
			cout<<"Invalid Instruction Format\n";
			return "error";
		}
		while (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9' || line[i] == 'p'){
			if(line[i]=='p'){
				temp=2;
				i++;
				break;
			}
			temp = temp * 10 + (int)(line[i] - '0');
			i=i+1;
		}
		if(temp<-16 && temp>15){
			cout<<"Out of Range Immediate"<<endl;
			return "error";
		}
		rs1 = dec2Binary(temp, 5);
		return "valid";
	}
}


//Function to extract data fied of S type
string Stype(string &line, string &immediate, int i, string &rs1, string &rs2){
	
	// skip spaces
	while (line[i]==' '){
		i++;
	}
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	int temp;
	temp=0;
	
	while (line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
		temp = temp * 10 + (int)(line[i] - '0');
		i++;
	}
	
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	
	rs2 = dec2Binary(temp, 5);
	
	while(line[i]==',' || line[i]==' '){
		i++;
	}
	
	int flag1=0;   // For negative offset

	if(line[i]=='-'){
		flag1=1;
		i++;
	}
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Instruction Format\n";
		return "error";
	}
	temp=0;
	while(line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
		temp = temp*10 + (int)(line[i]-'0');
		i++;
	}
	
	if(flag1==1){
		temp=temp*-1;
	}
	if(temp>2047){
		if(temp<-2048){
		cout<<"Invalid Immediate\n";
		return "error";
		}
	}	
	
	immediate = dec2Binary(temp,12);
	for( ; line[i]==' '; i++){
		
	}
	if(line[i]!='('){
		cout<<"Invalid Instruction Format\n";
		return "error";
	}
	i++;
	while(line[i]==' '){
		i++;
	}
	if(line[i]!='x' && line[i]!='s'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9' && line[i]!='p'){
		cout<<"Invalid Register\n";
		return "error";
	}
	temp=0;
	while(line[i] == '0' || line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9' || line[i] == 'p'){
		if(line[i]=='p'){
			temp=2;
			i++;
			break;
		}
		temp = temp*10 + (int)(line[i]-'0');
        i++;
	}
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	rs1 = dec2Binary(temp,5);
	return "valid";
}

//Function to extract values of SB type
string SBtype(string &line, string &immediate, lli i, lli currentLineNumber, vector<labelData> &labelArray, string &rs1, string &rs2 ){

	int flag=0;  //To check whether such label exist or not
	int temp=0;   
	lli labelOffset;
	string label="";
	
	int flag1=0;
	
  while(line[i]==' ')
			i++;
	
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Register\n";
		return "error";
	}
	
	while(line[i]=='0'||line[i]=='1'||line[i]=='2'||line[i]=='3'||line[i]=='4'||line[i]=='5'||line[i]=='6'||line[i]=='7'||line[i]=='8'||line[i]=='9'){
		temp=temp*10+(int)(line[i]-'0');
		i++;
     }

	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	
	rs1=dec2Binary(temp, 5);
	
	while(line[i]==',' || line[i]==' ')
		i++;
	
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	temp=0;
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Register\n";
		return "error";
	}
	while(line[i]=='0'||line[i]=='1'||line[i]=='2'||line[i]=='3'||line[i]=='4'||line[i]=='5'||line[i]=='6'||line[i]=='7'||line[i]=='8'||line[i]=='9'){
		temp=temp*10+(int)(line[i]-'0');
		i++;
     }
	
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	
	rs2=dec2Binary(temp, 5);
	temp=0;
	i++;
	while(line[i]==',' || line[i]==' ')
		i++;

   for(; i<line.length(); i++){
		if(line[i]==' ') 
			break;
		label+=line[i]; 
	}
	//cout<<label.length();
    i=0;
	while(i<labelArray.size()){
		if(labelArray[i].label==label){
			labelOffset=(labelArray[i].lineNumber-currentLineNumber)*4;
			if(labelOffset<-1024)
			   if(labelOffset>2048){
				cout<<"out of range offset\n";
				return "error";
			}
			flag=1;
			immediate=dec2Binary(labelOffset, 12);
		}
		i++;
	}
    
	if(flag==0){
		cout<<"ERROR: Incorrect label\n";
		return "error";
	}
	return "valid";
}

//Function to extract values of UJ type
string UJtype(string &line, lli currentLineNumber, string &immediate, vector<labelData> &labelArray, string &rd, lli i){
	lli labelOffset;
	int flag=0;//To check whether such label exist or not
	
	string label="";
	while(line[i]==' ')
		i++;
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	if(line[i]!='9' && line[i]!='8' && line[i]!='7' && line[i]!='6' && line[i]!='5' && line[i]!='4' && line[i]!='3' && line[i]!='2' && line[i]!='1' && line[i]!='0'){
		cout<<"Invalid Register\n";
		return "error";
	}

	 int temp=0;
	
	while(line[i]=='9'||line[i]=='8'||line[i]=='7'||line[i]=='6'||line[i]=='5'||line[i]=='4'||line[i]=='3'||line[i]=='2'||line[i]=='1'||line[i]=='0'){
		temp=temp*10+(int)(line[i]-'0');
		i++;
     }
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	rd=dec2Binary(temp, 5);
	
	while(line[i]==',' || line[i]==' ')
		i++;

	for( ; i<line.length(); i++){
		if(line[i]==' ') 
			break;
		label+=line[i]; 
	}
      i=0;
	  
	while(i<labelArray.size()){
		if(labelArray[i].label==label){
			labelOffset=(labelArray[i].lineNumber-currentLineNumber)*4;
			if(labelOffset<-262144)
			  if(labelOffset>262143){
				cout<<"out of range label\n";
				return "error";
			}
			immediate=dec2Binary(labelOffset, 20);
			flag=1;
		}
		 i++;
    }
	if(flag==0){
		cout<<"ERROR: Incorrect label\n";
		return "error";
	}
	return "valid";
}

//Function to extract values of U type
string Utype(string &line, string &rd, string &immediate, lli i){
	
	while(line[i]==' ')
		i++;
	if(line[i]!='x'){
		cout<<"Invalid Register\n";
		return "error";
	}
	i++;
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Register\n";
		return "error";
	}

	int temp=0;
	while(line[i]=='9'||line[i]=='8'||line[i]=='7'||line[i]=='6'||line[i]=='5'||line[i]=='4'||line[i]=='3'||line[i]=='2'||line[i]=='1'||line[i]=='0'){
		temp=temp*10+(int)(line[i]-'0');
		i++;
    } 
	
	if(temp>=32){
		cout<<"Invalid Register\n";
		return "error";
	}
	rd=dec2Binary(temp, 5);
	
	while(line[i]==',' || line[i]==' ')
		i++;
	
	if(line[i]!='0' && line[i]!='1' && line[i]!='2' && line[i]!='3' && line[i]!='4' && line[i]!='5' && line[i]!='6' && line[i]!='7' && line[i]!='8' && line[i]!='9'){
		cout<<"Invalid Immediate"<<endl;
		return "error";
	}
	temp=0;
	while(line[i]=='0'|| line[i] == '1' || line[i] == '2' || line[i] == '3' || line[i] == '4' || line[i] == '5' || line[i] == '6' || line[i] == '7' || line[i] == '8' || line[i] == '9'){
		temp = temp * 10 + (int)(line[i] - '0');
		i++; 
    }
	
	if(temp<-524288)
	   if(temp>524287){
		cout<<"Invalid Immediate"<<endl;
		return "error";
	}
	immediate=dec2Binary(temp,20);
	return "valid";
}

/*Assembly to Machine Code
Input: each line of assembly code, line number, label array
Output: equivalent machine code(hexadecimal string)*/
string asm_mc(string line, vector<labelData> &labelArray, lli currentLineNumber){
	
	string instruction="";//first word of each assembly line
	string mcodeInstHex="";
	string mcodeInstBin="";
	string funct3="";
	string funct7="";
	string opcode="";
	string immediate="";
	string rs1="", rs2="", rd=""; 
	string type="";
	int i=0;
	int ISubType = 0;//subtypes for I-type instructions 0 for addi etc, 1 for instruction with offset

	
	for(; line[i]!=' ' && i<line.size(); i++){
		if(line[i]==':'){
			instruction+=line[i];
			break;
		}
		instruction+=line[i];
    }

	if(instruction[instruction.size()-1]==':'){
		type="LABEL";
		if(i<=line.size()-2){
			instruction="";
			i++;
			while(line[i]==' ')
				i++;
			for(; line[i]!=' '; i++)
				instruction+=line[i];
		}
	}
	//cout<<instruction<<endl;
	if(instruction=="add")
		opcode="0110011", funct3="000", funct7="0000000", type="R";

	if(instruction=="and")
		opcode="0110011", funct3="111", funct7="0000000", type="R";
		
	if(instruction=="or")
		opcode="0110011", funct3="110", funct7="0000000", type ="R";

	if(instruction=="sll")
		opcode="0110011", funct3="001", funct7="0000000", type ="R";
		
	if(instruction=="slt")
		opcode="0110011", funct3="010", funct7="0000000", type ="R";

    if(instruction=="sra")
		opcode="0110011", funct3="101", funct7="0100000", type ="R";
	
	if(instruction=="srl")
		opcode="0110011", funct3="101", funct7="0000000", type ="R";
	
	if(instruction=="sub")
		opcode="0110011", funct3="000", funct7="0100000", type ="R";
	
	if(instruction=="xor")
		opcode="0110011", funct3="100", funct7="0000000", type ="R";
	
	if(instruction=="mul")
		opcode="0110011", funct3="000", funct7="0000001", type ="R";
	
	if(instruction=="div")
		opcode="0110011", funct3="100", funct7="0000001", type ="R";
		
	if(instruction=="rem")
		opcode="0110011", funct3="110", funct7="0000001", type ="R";
		
    if(instruction == "lb")
		opcode = "0000011", funct3 = "000", type = "I", ISubType = 1;

	if(instruction == "lh")
		opcode = "0000011", funct3 = "001", type = "I", ISubType = 1;

	if(instruction == "lw")
		opcode = "0000011", funct3 = "010", type = "I", ISubType = 1;
	
	if(instruction == "ld")
		opcode = "0000011", funct3 = "011", type = "I", ISubType = 1;

    if(instruction == "addi")
		opcode = "0010011", funct3 = "000", type = "I";
	
    if(instruction == "ori")
		opcode = "0010011", funct3 = "110", type = "I";
	
	if (instruction == "andi")
		opcode = "0010011", funct3 = "111", type = "I";

	if (instruction == "sd")
		opcode = "0010011", funct3 = "011", type = "I";

	if (instruction == "jalr")
		opcode = "1100111", funct3 = "000", type = "I", ISubType = 1;
	
	if(instruction == "sw")
		opcode = "0100011", funct3 = "010", type = "S";

	if(instruction == "sh")
		opcode = "0100011", funct3 = "001", type = "S";

	if(instruction == "sb")
		opcode = "0100011", funct3 = "000", type = "S";

	if(instruction=="beq")
		opcode="1100011",funct3="000",type="SB";

	if(instruction=="bne")
		opcode="1100011",funct3="001",type="SB";

	if(instruction=="blt")
		opcode="1100011",funct3="100",type="SB";

	if(instruction=="bge")
		opcode="1100011",funct3="101",type="SB";

	if(instruction=="jal")
		opcode="1101111",type="UJ";
	

	if(instruction=="auipc")
		opcode="0010111",type="U";

	if(instruction=="lui")
		opcode="0110111", type="U";

	//To extract other data field according to instruction type
	if(type=="R"){
		string s = Rtype(mcodeInstBin, line, i, rd, rs1, rs2);
		if(s!="error")
			mcodeInstBin=funct7+rs2+rs1+funct3+rd+opcode;
	}
	
	else if(type=="S"){
		string s = Stype(line,immediate, i, rs1, rs2);
		
		string imm_4_0 = immediate.substr(7,5);
		string imm_11_5 = immediate.substr(0,7);
		if(s!="error")
			mcodeInstBin = imm_11_5+rs2+rs1+funct3+imm_4_0+opcode;
	}

	else if(type == "I"){
		string s = Itype(line, immediate, ISubType, rs1, i, rd );
		if(s=="error")
			mcodeInstBin="";
		else if(ISubType == 0 || ISubType == 1){
			mcodeInstBin = immediate + rs1 + funct3 + rd + opcode;
		}
	}
	else if(type=="UJ"){
		string s = UJtype(line, currentLineNumber, immediate, labelArray, rd, i);
		if(s!="error")
			mcodeInstBin = immediate[0]+immediate.substr(9,10)+immediate[8]+immediate.substr(0,8)+rd+opcode;
	}
	
	else if(type=="SB"){
		string s = SBtype(line, immediate, i, currentLineNumber, labelArray, rs1, rs2 );
		if(s!="error")
			mcodeInstBin=immediate[0]+immediate.substr(1,6)+rs2+rs1+funct3+immediate.substr(7,4)+immediate[0]+opcode;
	}
	
	else if(type=="U"){
		string s = Utype(line, rd, immediate, i);
		if(s!="error")
			mcodeInstBin = immediate+rd+opcode;
	}
	
	else if(type=="LABEL")
		return "labelDetected";
	
	else {
		cout<<"Unsupported Instruction"<<endl;
	}
	
	if(mcodeInstBin!="")
		mcodeInstHex="0x"+bin2Hex(mcodeInstBin);
	else
		mcodeInstHex="";
	return mcodeInstHex;
}

/*Keep track of labels in a vector
labelArray stores label & their line number */
void LineNumToLabel(vector<labelData> &labelArray){
	lli flag1=0, flag2=0;
	labelData tempLabel;
	lli lineNumber=0;
	string line="";
    int datasegment=0;
	fstream fileReading;
	fileReading.open("assemblyCode1.asm");
	
	while(getline(fileReading,line)){
		int i=0;
		string label="";
		flag1=0;
		if(line==".data")
			datasegment=1;
		if(line==".text")
			datasegment=0;
		
		if(line!=".text" && datasegment==0){
		if(line.size()==0 )
			continue;
		while(line[i]==' ')
			i++;
		for(; i<line.size(); i++){
			if(line[i]==':'){
				flag1=1;
				break;
			}
			label+=line[i];
		}
		if(flag1==1){
			if(i<line.size()){
				i++;
			while(line[i]==' ')
				i++;
			if(i<line.size()-1)
				flag2=1;
			}
		}
		
		if(flag1==1){
			tempLabel.label=label;
			tempLabel.lineNumber=lineNumber;
			labelArray.push_back(tempLabel);
		}
		if(flag1==1 && flag2==0)
			continue;

		lineNumber++;
	}
	}
	fileReading.close();
}

void removeComments(string str){
	fstream fileReading;
	fileReading.open(str,ios::in);  // Open for input operations.
	fstream fileWriting;
	fileWriting.open("assemblyCode1.asm",ios::out); // Open for output operations.
	string nline=""; // String which does not have comments 
	string line="";
	
	int i;
	while(getline(fileReading,line)){
		i=0;
		nline="";
		if(line[i]=='#')
			nline="";
		else{
			for(; i<line.size() ; i++){
				if(line[i]=='#')
					break;
				nline+=line[i];
			}
		}
		fileWriting<<nline<<endl;
	}
}
//Main File : File Read & Write
int main(){
	
	string hexInstructionAddress;
	lli instructionAddress=0;
	string binaryInstructionAddress;
	lli currentLineNumber=0;
	string assemblyLine;
	string machineLine="";
	string input;
	vector<labelData> labelArray;
	vector<data> varArray;
	
	cout<<"Enter the input file name"<<endl;
	cin>>input;
	removeComments(input);
	LineNumToLabel(labelArray);
	int datasegment=0;        //to check if .data is there or not
	int textsegment=0;
	long int* memory = new long int[1<<22];
	int memoryused=0;
	
	fstream fileReading;
	fstream fileWriting;
	fstream fileWriting2;
	fileReading.open("assemblyCode1.asm", ios::in);
	fileWriting.open("machineCode.mc", ios::out);
	fileWriting2.open("machineData.txt");
	int datal=1048576;//0x100000
	
	//To read input from Assembly Code File 
	while(getline(fileReading, assemblyLine)){
		cout<<assemblyLine;
		cout<<endl;
		if(assemblyLine==".data")
			datasegment=1;
		if(assemblyLine==".text"){
			textsegment=1;
			datasegment=0;
		}
		int i = 0;
		if(assemblyLine[0]==' '){
			while(assemblyLine[i]==' ')
				i++;
			assemblyLine = assemblyLine.substr(i,assemblyLine.size()-i);
		}

		if(assemblyLine.size() == 0){
			continue;
		}
		i=0;
		string size="";
		string var="";
		string value="";
		string address="";
		if(datasegment==1 && assemblyLine!=".data"){
			while(i<assemblyLine.size() && assemblyLine[i]!=':'){
				var+=assemblyLine[i];
				i++;
			}
			if(assemblyLine[i]!=':'){
				cout<<"ERROR"<<endl;
				goto EXIT;
			}
			i++;
			while(assemblyLine[i]==' ')
				i++;
			i++;
			for(; assemblyLine[i]!=' ' ; i++)
				size+=assemblyLine[i];
		    
			i++;
			while(i<assemblyLine.size()){
				value="";
				while(assemblyLine[i]=='9'||assemblyLine[i]=='8'||assemblyLine[i]=='7'||assemblyLine[i]=='6'||assemblyLine[i]=='5'||assemblyLine[i]=='4'||assemblyLine[i]=='3'||assemblyLine[i]=='2'||assemblyLine[i]=='1'||assemblyLine[i]=='0'){
					value+=assemblyLine[i];
					i++;
				}
				address = bitset<24>(datal).to_string();
				address = bin2Hex(address);
				machineLine = value+" 0x"+address+" "+size;
				if(size=="byte")
					datal++;
				else if(size=="word")
					datal+=4;
				data obj;
				obj.var=var;
				obj.hexaddress="0x"+address;
				varArray.push_back(obj);
				i++;
				fileWriting2<<machineLine<<endl;
			}
		}
		else if( assemblyLine!=".data" && assemblyLine!=".text"){
			machineLine=asm_mc(assemblyLine, labelArray, currentLineNumber);
		if(machineLine!="labelDetected"){
			if(machineLine!=""){
				binaryInstructionAddress=dec2Binary(instructionAddress, 32);
				instructionAddress+=4;
				hexInstructionAddress="0x"+bin2Hex(binaryInstructionAddress);
				machineLine=hexInstructionAddress+" "+machineLine;
				fileWriting<<machineLine<<endl;
				currentLineNumber++;
			}
		}
	}
}
	EXIT:
	fileWriting.close();
	fileWriting2.close();
	fileReading.close();
}



  