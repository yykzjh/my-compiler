#include<bits/stdc++.h>
using namespace std;
#define registerNum 7
//�������ṹ 
struct token {
    string name;
    string type;     //2Ϊ��ʱ������3Ϊ���֣�4Ϊ�ַ���1Ϊ����ʱ����,5Ϊ������ʶ��,6Ϊ������ 
    token(){
     type = "0";
 }
    token(string _name,string _type){
     name = _name;
     type = _type;
 }
};

//��Ԫʽ�ṹ��
struct Quaternion {
    int block;           //�������ʶ
    int type;            //����-0 1 2 3
    string op;           //����
    token num1;         //������1
    token num2;         //������2
    token num3;          //���
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
string getR(string lsR){//��ʱ��������Ĵ��� 
	if(!eptRegister.empty()){//����п��мĴ��� 
		string cmp=eptRegister.front();
		eptRegister.pop_front();
		vleRegister.push_front(cmp);//ȡ��һ��������ֵ�Ĵ���ͷ 
		
//		cout<<" cmp[1]"<<cmp[1];
		int k=(int)cmp[1]-48;// 
//		cout<<" k: "<<k<<endl; 
		mapR[k-1]=lsR;//����ʱ�����ͼĴ�����Ӧ�������Ĵ���Ϊ$1-7�� 
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

string lsFindR(string lsname){//������ʱ��������Ӧ�ļĴ��� 
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

Quaternion trans_R(Quaternion x){//�� ��ʱ��������Ϊ�Ĵ����� 
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
	
	//�Ƚ������ gt gl ge 
	else if(x.op=="gt"){
		Quaternion y=trans_R(x);
//		cout<<"gt��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
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
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
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
//		cout<<"eq��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
			string cmp1="eqli "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}
		else{
			string cmp1="eql "+y.num3.name+" "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
		}
	}
	
	//�߼������
	else if(x.op=="and"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"and��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
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
//		cout<<"and��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
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
//		cout<<"and��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
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
//		cout<<"xor��yֵ��"<<y.num3.name<<endl;
		if(x.num1.type=="3"||x.num2.type=="3"||x.num1.type=="4"||x.num2.type=="4"){//�ж��Ƿ����� 
			string cmp1="xori "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);
		} 
		else{
			string cmp1="xor "+y.num3.name+" "+y.num1.name+" "+y.num2.name;
			objcode.push_back(cmp1);			
		} 
	}
	
	//����ָ�� 
	else if(x.op=="sub"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"sub��yֵ��"<<y.num3.name<<endl;
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
//		cout<<"add��yֵ��"<<y.num3.name<<endl;
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
//		cout<<"mul��yֵ��"<<y.num3.name<<endl;
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
//		cout<<"mul��yֵ��"<<y.num3.name<<endl;
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

	//������ת 
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
//		cout<<"if��yֵ��"<<y.num1.name<<endl;
		string cmp1="beq "+y.num1.name+" "+y.num2.name+" "+y.num3.name;
		objcode.push_back(cmp1);
	}
	
	//����ת�� 
	else if(x.op=="mov"){//subi addi
		Quaternion y=trans_R(x);
//		cout<<"mov��yֵ��"<<y.num1.name<<endl;
		if(x.num1.type=="3"||x.num1.type=="4"){
			string cmp1="li "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
			}
		else {
			string cmp1="mov "+y.num2.name+" "+y.num1.name;
			objcode.push_back(cmp1);
			}
	}
	//������� 
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
 	//���Ŀ�����
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
	
	//���Ŀ����뵽�ļ� 
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
	initRegister();//��ʼ�� 
	inputQuaternions(in);
	toTarget(out);
//	outputQuaternions(out);
	 cout<<endl;
//  for(int i=0;i<registerNum;i++)
//  cout<<"$"<<i+1<<"��"<<mapR[i]<<" ";
	return 0;
}
