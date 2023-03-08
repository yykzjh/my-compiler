#include<bits/stdc++.h>
using namespace std;
#define registerNum 7
//操作数结构 
struct token {
    string name;
    string type;     //2为临时变量，3为数字，4为字符，1为非临时变量,5为程序块标识名,6为变量名 
    token(){
     type = "0";
 }
    token(string _name,string _type){
     name = _name;
     type = _type;
 }
};

//四元式结构体
struct Quaternion {
    int block;           //基本块标识
    int type;            //类型-0 1 2 3
    string op;           //操作
    token num1;         //操作数1
    token num2;         //操作数2
    token num3;          //结果
    Quaternion(string _op,token _num1=token(),token _num2=token(),token _num3=token()){
     op = _op;
     num1 = _num1;
     num2 = _num2;
     num3 = _num3;
 }
};

vector<Quaternion> quaternionArray;
vector<string> objcode;
vector<string> def;
list<string> eptRegister;
list<string> vleRegister;
string mapR[registerNum];

void initRegister(){
	for(int i=0;i<registerNum;i++){
		string cmp="$"+to_string(i+1);
		eptRegister.push_back(cmp);
	}
}
string getR(string lsR){//临时变量请求寄存器 
	if(!eptRegister.empty()){//如果有空闲寄存器 
		string cmp=eptRegister.front();
		eptRegister.pop_front();
		vleRegister.push_front(cmp);//取出一个放入有值寄存器头 
		
//		cout<<" cmp[1]"<<cmp[1];
		int k=(int)cmp[1]-48;// 
//		cout<<" k: "<<k<<endl; 
		mapR[k-1]=lsR;//将临时变量和寄存器对应起来，寄存器为$1-7； 
		return cmp;
	}
	else{
		string cmp=vleRegister.back();
		vleRegister.pop_back();
		vleRegister.push_front(cmp);
		
//		cout<<" cmp[1]"<<cmp[1];
		int k=(int)cmp[1]-48;// 
//		cout<<" k: "<<k<<endl; 
		mapR[k-1]=lsR;
		
		return cmp;
	}
}

string lsFindR(string lsname){//返回临时变量所对应的寄存器 
//	cout<<"isfind get lsname "<<lsname<<endl;
	for(int i=0;i<registerNum;i++){
		if(mapR[i]==lsname){
			int j=i+1;
			string jj=to_string(j);
			string cmp="$"+jj;
//			cout<<i<<jj<<" cmp:"<<cmp<<endl;
			return cmp;
		}
	}
}

string readR(string lsname){
	string x;
	x=lsFindR(lsname) ;
//	cout<<" lsFindR: "<<x<<endl;
	for(list<string>::iterator iter = vleRegister.begin();iter != vleRegister.end();iter++)  
	  {  
	    if(*iter==x) {
	    	vleRegister.push_front(*iter);
	    	vleRegister.erase(iter);
	    	return *iter;
		}  
	  }  
}

Quaternion trans_R(Quaternion x){//将 临时变量名改为寄存器名 
//	cout<<x.num1.name<<endl;
	Quaternion y(x.op,x.num1,x.num2,x.num3);
//	cout<<y.num3.type<<endl;
	if(x.op=="gt"||x.op=="lt"||x.op=="eq"||x.op=="if_f"||x.op=="sub"||x.op=="add"||x.op=="mul"||x.op=="div"||x.op=="and"){
		if(x.num1.type=="2")
		y.num1.name=readR(x.num1.name);
		if(x.num2.type=="2")
		y.num2.name=readR(x.num2.name);
		if(x.num3.type=="2")
		y.num3.name=getR(x.num3.name);
	}
	if(x.op=="mov"){
		if(x.num1.type=="2")
		y.num1.name=readR(x.num1.name);
		if(x.num2.type=="2")
		y.num2.name=getR(x.num2.name);
	}
	if(x.op=="mul1"){
		if(x.num3.type=="2")
		y.num3.name=getR(x.num3.name);
	}
//	cout<<y.num3.name<<endl;
	return y;
} 

void outputQuaternions(ofstream &out)
{
	for(auto x:quaternionArray){
		out<<x.op<<endl;
		out<<x.num1.type<<' '<<x.num1.name<<endl;
		out<<x.num2.type<<' '<<x.num2.name<<endl;
		out<<x.num3.type<<' '<<x.num3.name<<endl;
	}
}

void toTarget(ofstream &out){
	cout<<"0 .text"<<endl;
	for(auto x:quaternionArray){
	if(x.op=="def"){
		string cmp1=x.num1.name+": .word"+"0";
		def.push_back(cmp1);
	}
	
	//比较运算符 gt gl ge 
	else if(x.op=="gt"){
		Quaternion y=trans_R(x);
//		cout<<"gt的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="slti "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}
		else{
			string cmp1="slt "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		} 
		/*        
		b   target      #  unconditional branch to program label target
        beq $t0,$t1,target #  branch to target if  $t0 = $t1
        blt $t0,$t1,target #  branch to target if  $t0 < $t1
        ble $t0,$t1,target #  branch to target if  $t0 <= $t1
        bgt $t0,$t1,target #  branch to target if  $t0 > $t1
        bge $t0,$t1,target #  branch to target if  $t0 >= $t1
        bne $t0,$t1,target #  branch to target if  $t0 <> $t1*/
		
	}
	else if(x.op=="lt"){
		Quaternion y=trans_R(x);
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="slti "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		}
		else{
			string cmp1="slt "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}	
	}
	else if(x.op=="eq"){
		Quaternion y=trans_R(x);
//		cout<<"eq的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="eqli "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}
		else{
			string cmp1="eql "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}
	}
	
	//逻辑运算符
	else if(x.op=="and"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"and的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="andi "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else{
			string cmp1="and "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);			
		} 
	}
	
	else if(x.op=="or"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"and的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="ori "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else{
			string cmp1="or "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);			
		} 
	}
	
	else if(x.op=="not"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"and的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="noti "+x.num3.name+" "+x.num1.name+" "+x.num2.name;
			objcode.push_back(cmp1);
		} 
		else{
			string cmp1="not "+x.num3.name+" "+x.num1.name+" "+x.num2.name;
			objcode.push_back(cmp1);			
		} 
	}
	
	else if(x.op=="xor"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"xor的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//判断是否含数字 
			string cmp1="xori "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else{
			string cmp1="xor "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);			
		} 
	}
	
	//算术指令 
	else if(x.op=="sub"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"sub的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){
			string cmp1="subi "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else {
			string cmp1="sub "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
	}
	
	else if(x.op=="add"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"add的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){
			string cmp1="addi "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else {
			
			string cmp1="add "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
	}
	else if(x.op=="mul"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"mul的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){
			string cmp1="muli "+y.num1.name+" "+y.num2.name;
			string cmp2="mflo "+y.num3.name;
			objcode.push_back(cmp1);
			objcode.push_back(cmp2);
		} 
		else {
			string cmp1="mul "+y.num1.name+" "+y.num2.name;
			string cmp2="mflo "+y.num3.name;
			objcode.push_back(cmp1);
			objcode.push_back(cmp2);
		} 
	}
	else if(x.op=="div"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"mul的y值："<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){
			string cmp1="divi "+y.num1.name+" "+y.num2.name;
			string cmp2="mflo "+y.num3.name;
			objcode.push_back(cmp1);
			objcode.push_back(cmp2);
		} 
		else {
			string cmp1="div "+y.num1.name+" "+y.num2.name;
			string cmp2="mflo "+y.num3.name;
			objcode.push_back(cmp1);
			objcode.push_back(cmp2);
		} 
	}

	//控制跳转 
	else if(x.op=="lab"){
		string cmp1=x.num1.name+" :";
		objcode.push_back(cmp1);
	}
	else if(x.op=="goto"){
		string cmp1="j "+x.num1.name;
		objcode.push_back(cmp1);
	}
	else if(x.op=="if_f"){
		Quaternion y=trans_R(x);
//		cout<<"if的y值："<<y.num1.name<<endl;
		string cmp1="beq "+y.num1.name+" "+y.num2.name+" "+y.num3.name;
		objcode.push_back(cmp1);
	}
	
	//数据转移 
	else if(x.op=="mov"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"mov的y值："<<y.num1.name<<endl;
		if(x.num1.type=="3"||x.num1.type=="4"){
			string cmp1="li "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
			}
		else {
			string cmp1="mov "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
			}
	}
	//输入输出 
	else if(x.op=="write"){
		string cmp1="li $v0 1";
		string cmp2="la $a0 "+x.num1.name;
		string cmp3="syscall";
		objcode.push_back(cmp1);
		objcode.push_back(cmp2);
		objcode.push_back(cmp3);	
	}
	else if(x.op=="read"){
		string cmp1="li $v0 5";
		string cmp2="syscall";
		string cmp3="la "+x.num1.name+" "+"$v0";
		objcode.push_back(cmp1);
		objcode.push_back(cmp2);
		objcode.push_back(cmp3);
	}
		

 }
 	//输出目标代码
 	int kk;
	for (kk=1; kk < (int)objcode.size()+1; kk++)
	{
		cout <<std::left<<setw(10)<< kk << std::left << setw(10) << objcode[kk-1] << endl;
	}
	cout<<kk<<" .data"<<endl; 
	kk++;
	for ( int i=kk ; i < (int)def.size()+objcode.size()+2; i++)
	{
		cout <<std::left<<setw(10)<< i << std::left << setw(10) << def[i-kk] << endl;
	}
	
	//输出目标代码到文件 
	for (kk=1; kk < (int)objcode.size()+1; kk++)
	{
		out <<std::left<<setw(10)<< kk << std::left << setw(10) << objcode[kk-1] << endl;
	}
	out<<kk<<" .data"<<endl; 
	kk++;
	for ( int i=kk ; i < (int)def.size()+objcode.size()+2; i++)
	{
		out <<std::left<<setw(10)<< i << std::left << setw(10) << def[i-kk] << endl;
	}
}


void inputQuaternions(ifstream &in)
{	
	string str;
	while(in>>str){
		Quaternion tmp(str);
		in>>tmp.num1.type;
		if(tmp.num1.type!="0")
			in>>tmp.num1.name;
		in>>tmp.num2.type;
		if(tmp.num2.type!="0")
			in>>tmp.num2.name;
		in>>tmp.num3.type;
		if(tmp.num3.type!="0")
			in>>tmp.num3.name;
		quaternionArray.push_back(tmp);
	}
}
int main(){
	ifstream in("quaternion_file.txt", ios::in);
	ofstream out("mips.txt", ios::out);
	initRegister();//初始化 
	inputQuaternions(in);
	toTarget(out);
//	outputQuaternions(out);
	 cout<<endl;
//  for(int i=0;i<registerNum;i++)
//  cout<<"$"<<i+1<<"："<<mapR[i]<<" ";
	return 0;
}
