#include<bits/stdc++.h>
using namespace std;

const int maxn=1e4+5;

//pro_numָ�����һ������ʽ�±� 
int pro_num;
//S�����ع��Ŀ�ʼ���� 
string S;
//NT_lines����ÿ�����ս��ַ��Ĳ���ʽ���ļ��� 
map<string, vector<int> > NT_lines;
//strl�ݴ����ʽ��ߵķ��ս���ţ�strr�ݴ����ʽ�ұߵĸ������� 
string strl,strr,str;
//pro_left����ÿ�в���ʽ��ߵķ��ս���� 
vector<string> pro_left;
//pro_right�������ʽ��� 
vector<string> pro_right[maxn];
//NT����Ϊ���ս���ż��ϣ�T����Ϊ�ս���ż��� 
set<string> NT,T;

//NT_first����������ŵ�first���� 
map<string, set<string> > NT_first;

//NT_folow����������ս���ŵ�follow���� 
map<string, set<string> > NT_follow; 

//������Ŀ�ṹitem 
struct item{
	int row;//ָ���ڼ��еĲ���ʽ
	int col;//ָ����ǰ����ʽ����״̬ 
	set<string> search;
	bool operator< (const item &itemp) const
	{
		return this->row==itemp.row?this->col<itemp.col:this->row<itemp.row;
	}
};
//������Ŀ���Ľ������
struct node{
	set<item> itemSet;
	bool haveR;
}; 
//������Ч��Ŀ����DFA�Ľ��DFANodes
node DFANodes[maxn];
//����DFA���ڽ�����DFAArc
int DFAArc[maxn][1005];
//totalCharacter�������з���
vector<string> totalCharacter;
//itemSetInd���浱ǰ����Ŀ��������
int itemSetInd;

//����action��洢�Ľṹ
struct actionUnit{
	char op;
	int to;
}; 
//����action��
actionUnit action[maxn][1005];
//����goto�� 
int goTo[maxn][1005];
//����action���goto��������� 
vector<string> actionCol,goToCol;

//LALR1�кϲ�ͬ��������Ŀ����Ƿ��Ѿ����µı�� 
int amendVis[maxn];


//�Լ��Ͻ��в����� 
void Union(set<string> &dest,set<string> src,int op=0){
    set<string>::iterator it;
    if(op==1){
    	src.erase("#");
	}
    it = src.begin();
    while(it != src.end()){
        dest.insert(*it);
        it++;
    }
}


//�����������޹��ķ� 
void readCFG(ifstream &fin)
{
	T.clear();
	NT.clear();
	NT_lines.clear();
	pro_left.clear();
	for(int i=0;i<maxn;i++){
		pro_right[i].clear();
	}
	NT.insert("S'"); 
	NT_lines["S'"].push_back(0);
	pro_left.push_back("S'");
	pro_num = 0;
	while(fin>>strl){
		NT.insert(strl);
		fin>>str;
		fin.get();
		getline(fin,strr);
		pro_left.push_back(strl);
		pro_num++;
		NT_lines[strl].push_back(pro_num);
		stringstream ss(strr);
		while(ss>>strr){
			if(strr=="|"){
				pro_num++;
				pro_left.push_back(strl);
				NT_lines[strl].push_back(pro_num);
			}
			else{
				if(strr!="��"){
					for(int i=0;i<strr.size();){
						if(strr[i]=='\\'){
							strr.erase(i,1);
						}
						else i++;
					} 
					pro_right[pro_num].push_back(strr);
				}  
			}
		}
	}
	//ȷ����ʼ���ź��ع�Ĳ���ʽ 
	S = "S'";
	pro_right[0].push_back(pro_left[1]); 
	//ȷ���ս���ŵļ��� 
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();j++){
			if(NT_lines.find(pro_right[i][j])==NT_lines.end()){
				T.insert(pro_right[i][j]);
			}
		}
	}
}


//���������
void printParseTable(ofstream & fout)
{
	fout<<pro_num<<endl;
	for(int i=0;i<=pro_num;i++){
		fout<<pro_left[i]<<' ';
	} 
	fout<<endl;
	for(int i=0;i<=pro_num;i++){
		fout<<pro_right[i].size()<<endl;
		for(int j=0;j<pro_right[i].size();j++){
			fout<<pro_right[i][j]<<' ';
		}
		fout<<endl;
	}
	fout<<itemSetInd<<endl;
	fout<<actionCol.size()<<endl;
	for(int i=0;i<actionCol.size();i++){
		fout<<actionCol[i]<<' ';
	}
	fout<<endl;
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<actionCol.size();j++){
			fout<<action[i][j].op<<action[i][j].to<<' ';
		}
		fout<<endl;
	}
	fout<<goToCol.size()<<endl;
	for(int i=0;i<goToCol.size();i++){
		fout<<goToCol[i]<<' ';
	}
	fout<<endl;
	for(int i=0;i<itemSetInd;i++){
		for(int j=0;j<goToCol.size();j++){
			fout<<goTo[i][j]<<' ';
		}
		fout<<endl;
	}
} 


//����ĳ����Ŀ���ϵ���Ŀ�հ�
void calClosure(node &p,int op)
{
	while(1){
		set<item> addItem;
		for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){
			if(it->col<pro_right[it->row].size()&&NT.find(pro_right[it->row][it->col])!=NT.end()){
				item itemp;
				itemp.col = 0;
				if(op!=0){
					int flag = 0;
					for(int i=(it->col)+1;i<pro_right[it->row].size();++i){
						if(NT_first[pro_right[it->row][i]].find("#")==NT_first[pro_right[it->row][i]].end()){
							Union(itemp.search,NT_first[pro_right[it->row][i]]);
							flag = 1;
							break;
						}
						else{
							Union(itemp.search,NT_first[pro_right[it->row][i]],1);
						}
					}
					if(flag==0){
						Union(itemp.search,it->search);
					}
				}
				for(int i=0;i<NT_lines[pro_right[it->row][it->col]].size();i++){
					itemp.row = NT_lines[pro_right[it->row][it->col]][i];
					if(op!=0){
						if(addItem.find(itemp)!=addItem.end()){
							auto iter = addItem.find(itemp);
							(const_cast<item*>(&(*iter))->search).insert(itemp.search.begin(),itemp.search.end());
						}
						else 
							addItem.insert(itemp);
					}
					else
						addItem.insert(itemp);
				}
			}
		}
		int prePSize = p.itemSet.size();
		p.itemSet.insert(addItem.begin(),addItem.end());
		if(p.itemSet.size()==prePSize) break;
	}
	for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){
		if(it->col>=pro_right[it->row].size()){
			p.haveR = true;
			break;
		}
	}
	return ;
}


//LALR1��ֻ�������������ıհ�
void justCalClosure(node &p)
{
	while(1){
		set<item> addItem;//ÿ�α������ӵ���Ŀ 
		for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){//���������� 
			if(it->col<pro_right[it->row].size()&&NT.find(pro_right[it->row][it->col])!=NT.end()){//����ú������ת���ַ��Ƿ��ս���� 
				item itemp;
				itemp.col = 0;
				int flag = 0;
				for(int i=(it->col)+1;i<pro_right[it->row].size();++i){//�µıհ���Ŀ�Ӷ�Ӧ�ĺ�����Ŀ������������ 
					if(NT_first[pro_right[it->row][i]].find("#")==NT_first[pro_right[it->row][i]].end()){
						Union(itemp.search,NT_first[pro_right[it->row][i]]);
						flag = 1;
						break;
					}
					else{
						Union(itemp.search,NT_first[pro_right[it->row][i]],1);
					}
				}
				if(flag==0){//����ú�����Ŀ������ȵ����з��ŵ�First���϶������շ��ţ��������ɵ�������Ҫ���ϸú�����Ŀ����������
					Union(itemp.search,it->search);
				}
				for(int i=0;i<NT_lines[pro_right[it->row][it->col]].size();i++){//���÷��ս���ŵ����в���ʱ����Ϊ��ʼ��Ŀ���뼯���� 
					itemp.row = NT_lines[pro_right[it->row][it->col]][i];
					if(addItem.find(itemp)!=addItem.end()){
						auto iter = addItem.find(itemp);
						(const_cast<item*>(&(*iter))->search).insert(itemp.search.begin(),itemp.search.end());
					}
					else 
						addItem.insert(itemp);
				}
			}
		}
		int prePSize = p.itemSet.size();
		p.itemSet.insert(addItem.begin(),addItem.end());
		if(p.itemSet.size()==prePSize) break;//������Ŀ���ϴ�С����ʱѭ������ 
	}
	for(auto it = p.itemSet.begin();it!=p.itemSet.end();++it){//�жϵ�ǰ������Ŀ���ϰ���������Լ��Ŀ 
		if(it->col>=pro_right[it->row].size()){
			p.haveR = true;
			break;
		}
	}
	return ;
}


//�ϲ�ͬ�����������������״̬�������������Ͻ����޸�
void amendDFS(int index,int now,string symbol)
{
	node p;
	p.haveR = false;
	//����ת���ַ����ɺ����� 
	for(auto it=DFANodes[index].itemSet.begin();it!=DFANodes[index].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()&&pro_right[it->row][it->col]==symbol){
			item itemp;
			itemp.row = it->row;
			itemp.col = (it->col)+1;
			itemp.search = it->search;
			p.itemSet.insert(itemp);
		}
	}
	justCalClosure(p);//����հ���Ŀ
	for(auto it=DFANodes[now].itemSet.begin(),iter=p.itemSet.begin();it!=DFANodes[now].itemSet.end();++it,++iter){
		const_cast<item*>(&(*it))->search = iter->search;
	}//���¾ɽ��������������ֱ�ӻ���㣬Ҫ��Ȼ�������DFS���Ҳ��� 
	amendVis[now] = 1; 
	for(int j=0;j<totalCharacter.size();j++){//���º�������������� 
		if(DFAArc[now][j]!=0&&amendVis[DFAArc[now][j]]==0){
			amendDFS(now,DFAArc[now][j],totalCharacter[j]);
		}
	}
}
 

//��ѯ�Ƿ��Ѿ������뵱ǰ�������Ŀ������ͬ���߿ɺϲ�����Ŀ���ϣ����з������н��ţ���û�з���0
int findEqual(const node &p,int op)
{
	for(int i=1;i<itemSetInd;++i){
		if(DFANodes[i].itemSet.size()==p.itemSet.size()){
			int flag = 0,LALRFlag=0;
			vector<set<string> > tmpMergeSearch(p.itemSet.size()); //��ʱ����������Ŀ���Ϻϲ��������������Ϻ�ļ��� 
			int tmpMSearchInd = 0; //ָ��ϲ������������ϵ��±� 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(p.itemSet.find(*it)==p.itemSet.end()){//����һ����Ŀ����ͬ��ֱ�ӷ�������Ŀ���� 
					flag=1;
					break;
				}
				else{//�������Ŀ�ҵ���ͬ�Ĵ��� 
					if(op!=0){//ֻ��LR1��LALR1�Ż��һ���жϴ��� 
						auto findedItem = p.itemSet.find(*it);
						int flagg = 0;
						if((findedItem->search).size()==(it->search).size()){
							for(auto iter = (it->search).begin();iter!=(it->search).end();++iter){//�ȶ������������Ƿ���ͬ 
								if((findedItem->search).find(*iter)==(findedItem->search).end()){
									flagg = 1;
									break;
								}
							}
						}
						else{
							flagg = 1;
						}
						if(op==1){ 
							if(flagg==1){//LR1�����������ϲ�ֱͬ�Ӿ�������Ŀ���ϲ���� 
								flag = 1;
								break;
							}
						} 
						else if(op==2){//LALR1����ȻҲҪ�ж������������벻��ͬ�������������ϴ��ڲ���ͬ��Ӱ��ϲ� 
							if(flagg==1) LALRFlag = 1;
							tmpMergeSearch[tmpMSearchInd].insert((it->search).begin(),(it->search).end());
							tmpMergeSearch[tmpMSearchInd++].insert((findedItem->search).begin(),(findedItem->search).end());
						}
					}
				}
			}
			if(op==0||op==1){
				if(flag==0){//LR0��SLR1��LR1��flag=0�����±�Ϊi�ļ�����Ŀ����Ŀ������ͬ 
					return i;
				}
			}
			else if(op==2){
				if(flag==0){//LALR1��flag=0������Ŀ�����г�ȥ���������ϵĲ�����ͬ 
					if(LALRFlag==0){//LALRFlag=0������Ϊi�ļ�����Ŀ����Ŀ������ͬ 
						return i;
					}
					else{//LALR1��Ŀ������ͬ�����������ϲ�ͬ�����ж��ܷ�ϲ� 
						set<string> sumStr;
						int sumSize = 0;
						int j = 0;
						for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end()&&j<tmpMSearchInd;++j,++it){
							if((it->col)>=pro_right[it->row].size()){
								sumStr.insert(tmpMergeSearch[j].begin(),tmpMergeSearch[j].end());
								sumSize += tmpMergeSearch[j].size();
							}
						}//�жϺϲ�����û�й�Լ-��Լ��ͻ 
						if(sumStr.size()==sumSize){//�ϲ���û�й�Լ-��Լ��ͻ 
							j = 0;
							for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end()&&j<tmpMSearchInd;++j,++it){
								const_cast<item*>(&(*it))->search = tmpMergeSearch[j];
							}//���ºϲ�ͬ��������������
							for(int j=0;j<maxn;++j){
								amendVis[j] = 0;
							}
							amendVis[i] = 1;
							for(int j=0;j<totalCharacter.size();j++){//���ºϲ���ͬ������������������ 
								if(DFAArc[i][j]!=0&&amendVis[DFAArc[i][j]]==0){
									amendDFS(i,DFAArc[i][j],totalCharacter[j]);
								}
							}
							return i;
						}
					}
				}
			}
		}
	}
	return 0;
} 
 

//����ĳ����Ŀ���϶�Ӧ��ĳ���ķ����ŵ�ת�ƺ���
int calGo(int index, string symbol,int op)
{
	node p;
	p.haveR = false;
	for(auto it=DFANodes[index].itemSet.begin();it!=DFANodes[index].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()&&pro_right[it->row][it->col]==symbol){
			item itemp;
			itemp.row = it->row;
			itemp.col = (it->col)+1;
			if(op!=0) itemp.search = it->search;
			p.itemSet.insert(itemp);
		}
	}
	calClosure(p,op);
	int equalInd = findEqual(p,op);
	int toCol = find(totalCharacter.begin(),totalCharacter.end(),symbol)-totalCharacter.begin();
	if(equalInd==0){
		DFANodes[itemSetInd] = p;
		DFAArc[index][toCol] = itemSetInd++;
	}
	else{
		DFAArc[index][toCol] = equalInd;
	}
	return equalInd;
}


//�ж�һ����Ŀ�������Ƿ�ȫ�ǹ�Լ��Ŀ
bool  judgeAllRe(const node &p)
{
	for(auto it=p.itemSet.begin();it!=p.itemSet.end();++it){
		if(it->col<pro_right[it->row].size()){
			return false;
		}
	}
	return true;
}


//�ӵ������Ŀ���Ͽ�ʼ��ɢ�γ�DFA
void DFS(int ind,int op)
{
	for(auto it=DFANodes[ind].itemSet.begin();it!=DFANodes[ind].itemSet.end();++it){
		if(it->col<pro_right[it->row].size()){
			int toCol = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
			if(DFAArc[ind][toCol]==0){
				int equalInd = calGo(ind, pro_right[it->row][it->col],op);
				if(equalInd==0&&!judgeAllRe(DFANodes[DFAArc[ind][toCol]])){
					DFS(DFAArc[ind][toCol],op);
				}
			}
		}
	}
} 


//����DFA
void produceDFA(int op)
{
	//��ʼ�����ݽṹ
	itemSetInd = 0;
	for(int i=0;i<maxn;i++){
		DFANodes[i].itemSet.clear();
		DFANodes[i].haveR = false;
		for(int j=0;j<105;j++){
			DFAArc[i][j] = 0;
		}
	}
	//���ս���źͷ��ս���źϲ���һ��
	totalCharacter.clear(); 
	totalCharacter.insert(totalCharacter.end(),T.begin(),T.end());
	NT.erase("S'");
	totalCharacter.insert(totalCharacter.end(),NT.begin(),NT.end());
	NT.insert("S'");
	//��ʼ���������Ŀ���ϵĽ��
	item itemp;
	itemp.row = 0;
	itemp.col = 0;
	if(op!=0) itemp.search.insert("$");
	DFANodes[itemSetInd].haveR = false;
	DFANodes[itemSetInd].itemSet.insert(itemp);
	calClosure(DFANodes[itemSetInd++],op);
	DFS(0,op);
}


//ÿ������ս���ŵ�First���� 
bool getFirstPerTurn()
{
	bool change = false;
	for(map<string, vector<int> >::iterator it = NT_lines.begin();it!=NT_lines.end();++it){
		set<string> first;
		for(int i=0;i<(it->second).size();i++){
			int flag = 0;
			for(int j=0;j<pro_right[(it->second)[i]].size();j++){
				if(NT_first[pro_right[(it->second)[i]][j]].find("#")==NT_first[pro_right[(it->second)[i]][j]].end()){
					Union(first,NT_first[pro_right[(it->second)[i]][j]]);
					flag = 1;
					break;
				}
				else{
					Union(first,NT_first[pro_right[(it->second)[i]][j]],1);
				}
			}
			if(flag==0){
				first.insert("#");
			}
		}
		if(first.size()>NT_first[(it->first)].size()) change=true;
		NT_first[(it->first)] = first;
		first.clear();
	}
	return change;
} 


//���������ż���First���� 
void getFirst(){
	NT_first.clear();
	for(set<string>::iterator it = T.begin();it!=T.end();it++){
		set<string> first;
		first.insert((*it));
		NT_first[(*it)] = first;
		first.clear();
	}
	while(1){
		if(!getFirstPerTurn()) break;
	}
}


//��ʼ��follow���ϣ���first�Ĳ��ּ��뵽follow��
void initFollow(){
	NT_follow[S].insert("$");
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();++j){
			if(NT.find(pro_right[i][j])!=NT.end()){
				for(int k=j+1;k<pro_right[i].size();k++){
					if(NT_first[pro_right[i][k]].find("#")==NT_first[pro_right[i][k]].end()){
						Union(NT_follow[pro_right[i][j]],NT_first[pro_right[i][k]]);
						break;
					}
					else{
						Union(NT_follow[pro_right[i][j]],NT_first[pro_right[i][k]],1);
					}
				}
			}
		}
	}
} 


//ÿ�ֶ����з��ս������Follow���� 
bool  getFollowPerTurn(){
	bool change = false;
	for(int i=0;i<=pro_num;i++){
		for(int j=0;j<pro_right[i].size();++j){
			if(NT.find(pro_right[i][j])!=NT.end()){
				int flag = 0;
				for(int k=j+1;k<pro_right[i].size();k++){
					if(NT_first[pro_right[i][k]].find("#")==NT_first[pro_right[i][k]].end()){
						flag = 1;
						break;
					}
				}
				if(flag==0){
					int pre_num = NT_follow[pro_right[i][j]].size();
					Union(NT_follow[pro_right[i][j]],NT_follow[pro_left[i]]);
					if(NT_follow[pro_right[i][j]].size()>pre_num) change = true;
				}
			}
		}
	}
	return change;
}


//�����з��ս���ŵ�Follow���� 
void getFollow()
{
	//�Բ���ʽ���ս���Ž����޸�
	int emptyStrFlag = 0;
	for(int i=0;i<=pro_num;++i){
		if(pro_right[i].size()==0){
			pro_right[i].push_back("#");
			emptyStrFlag = 1;
		}
	}
	if(emptyStrFlag) T.insert("#");
	//����First����
	getFirst();
	//��ʼ��Follow���� 
	NT_follow.clear();
	initFollow();
	//��ʼѭ����Follow���ϣ�ֱ��Follow���ϲ������仯 
	while(1){
		if(!getFollowPerTurn()) break;
	}
	//������ʽ���ս�����޸Ļ�ȥ 
	for(int i=0;i<=pro_num;++i){
		if(pro_right[i].size()==1&&pro_right[i][0]=="#"){
			pro_right[i].pop_back();
		}
	}
	if(emptyStrFlag) T.erase("#");
}


//ʶ��DFA�Ľṹ���ж���ʲô��������
int judgeLRMethod()
{
	//����һ����Ϊ�ж����ݵı�־λ
	int SLR1 = 0, LR1 = 0; 
	//������Ŀ���ϵĽ�㣬Ѱ�Ҵ��ڹ�Լ��Ŀ����Ŀ���ϲ������ж� 
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR&&DFANodes[i].itemSet.size()>1){
			SLR1 = 1;
			set<string> succeedStr;//����ǰ��Ŀ�����л�����Ŀ�ĺ�̷��ű�����һ��������
			set<string> sumStr;//��̷��ż��������й�Լ��Ŀ��Follow���ϵĲ��� 
			int sumSize = 0;//������Լ��Ŀ��߷��ս���ŵ�Follow���ϵĴ�С֮�� 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(it->col<pro_right[it->row].size()){
					succeedStr.insert(pro_right[it->row][it->col]);
				}
				else{
					sumSize += NT_follow[pro_left[it->row]].size();
					sumStr.insert(NT_follow[pro_left[it->row]].begin(),NT_follow[pro_left[it->row]].end()); 
				} 
			}
			sumSize += succeedStr.size();
			sumStr.insert(succeedStr.begin(),succeedStr.end());
			if(sumSize>sumStr.size()) LR1 = 1;
		}
	}
	//���жϱ�־λ���з��������ؽ����1��LR0��2��SLR1��3��LR1
	if(LR1==1){
		return 3;
	}
	else if(SLR1==1){
		return 2;
	}
	else{
		return 1;
	}
}


//�ж�LR1����LALR1�ķ��������ܲ�����
bool assessLR1_or_LALR1()
{
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR&&DFANodes[i].itemSet.size()>1){
			set<string> succeedStr;//����ǰ��Ŀ�����л�����Ŀ�ĺ�̷��ű�����һ��������
			set<string> sumStr;//��̷��ż��������й�Լ��Ŀ�����������ϵĲ��� 
			int sumSize = 0;//������Լ��Ŀ���������ϵĴ�С֮�� 
			for(auto it=DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){
				if(it->col<pro_right[it->row].size()){
					succeedStr.insert(pro_right[it->row][it->col]);
				}
				else{
					sumSize += (it->search).size();
					sumStr.insert((it->search).begin(),(it->search).end()); 
				} 
			}
			sumSize += succeedStr.size();
			sumStr.insert(succeedStr.begin(),succeedStr.end());
			if(sumSize>sumStr.size()){
				return false;
			}
		}
	}
	return true;
}


//����LR0������
void produceLR0Table()
{
	//posΪ�����ҵ����±�
	int pos; 
	//��ʼ��action���goto��������� 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//��ʼ��action���goto��
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	} 
	//�����ڽӾ�������LR0������
	for(int i=0;i<itemSetInd;++i){
		if(DFANodes[i].haveR){
			for(int j=0;j<actionCol.size();++j){
				action[i][j].op = 'r';
				action[i][j].to = (DFANodes[i].itemSet.begin())->row;
			}
		}
		else{
			for(int j=0;j<totalCharacter.size();j++){
				if(DFAArc[i][j]!=0){
					if(find(actionCol.begin(),actionCol.end(),totalCharacter[j])!=actionCol.end()){
						pos = find(actionCol.begin(),actionCol.end(),totalCharacter[j])-actionCol.begin();
						action[i][pos].op = 's';
						action[i][pos].to = DFAArc[i][j];
					}
					else{
						pos = find(goToCol.begin(),goToCol.end(),totalCharacter[j])-goToCol.begin();
						goTo[i][pos] = DFAArc[i][j];
					}
				}
			}
		}
	}
	//�Գ���״̬�����޸�
	for(int i=0;i<actionCol.size();i++){
		action[1][i].op = '\0';
		action[1][i].to = 0;
	}
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


//����SLR1������
void produceSLR1Table()
{
	//pos��possΪ�����ҵ����±�
	int pos,poss; 
	//��ʼ��action���goto��������� 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//��ʼ��action���goto��
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	} 
	//�����ڽӾ�������SLR1������
	for(int i=0;i<itemSetInd;++i){
		for(auto it = DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){//������Ŀ������������Ŀ 
			if(it->col>=pro_right[it->row].size()){//�ǹ�Լ��Ŀ 
				for(auto iter = NT_follow[pro_left[it->row]].begin();iter!=NT_follow[pro_left[it->row]].end();++iter){//�����ù�Լ��Ŀ��follow���� 
					pos = find(actionCol.begin(),actionCol.end(),(*iter))-actionCol.begin();
					action[i][pos].op = 'r';
					action[i][pos].to = it->row;
				}
			}
			else{//�ǻ�����Ŀ 
				poss = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
				if(find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])!=actionCol.end()){
					pos = find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])-actionCol.begin();
					action[i][pos].op = 's';
					action[i][pos].to = DFAArc[i][poss];
				}
				else{
					pos = find(goToCol.begin(),goToCol.end(),pro_right[it->row][it->col])-goToCol.begin();
					goTo[i][pos] = DFAArc[i][poss];
				}
			}
		} 
	}
	//�Գ���״̬�����޸�
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


//����LR1������
void produceLR1_or_LALR1Table()
{
	//pos��possΪ�����ҵ����±�
	int pos,poss; 
	//��ʼ��action���goto��������� 
	goToCol.assign(NT.begin(),NT.end());
	goToCol.erase(find(goToCol.begin(),goToCol.end(),"S'"));
	actionCol.assign(T.begin(),T.end());
	actionCol.push_back("$");
	//��ʼ��action���goto��
	for(int i=0;i<itemSetInd;++i){
		for(int j=0;j<1005;j++){
			action[i][j].op = '\0';
			action[i][j].to = 0;
			goTo[i][j] = 0;
		}
	}
	//�����ڽӾ�������LR1����LALR1������
	for(int i=0;i<itemSetInd;++i){
		for(auto it = DFANodes[i].itemSet.begin();it!=DFANodes[i].itemSet.end();++it){//������Ŀ������������Ŀ 
			if(it->col>=pro_right[it->row].size()){//�ǹ�Լ��Ŀ 
				for(auto iter = (it->search).begin();iter!=(it->search).end();++iter){//�����ù�Լ��Ŀ������������ 
					pos = find(actionCol.begin(),actionCol.end(),(*iter))-actionCol.begin();
					action[i][pos].op = 'r';
					action[i][pos].to = it->row;
				}
			}
			else{//�ǻ�����Ŀ 
				poss = find(totalCharacter.begin(),totalCharacter.end(),pro_right[it->row][it->col])-totalCharacter.begin();
				if(find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])!=actionCol.end()){
					pos = find(actionCol.begin(),actionCol.end(),pro_right[it->row][it->col])-actionCol.begin();
					action[i][pos].op = 's';
					action[i][pos].to = DFAArc[i][poss];
				}
				else{
					pos = find(goToCol.begin(),goToCol.end(),pro_right[it->row][it->col])-goToCol.begin();
					goTo[i][pos] = DFAArc[i][poss];
				}
			}
		} 
	}
	//�Գ���״̬�����޸�
	pos = find(actionCol.begin(),actionCol.end(),"$")-actionCol.begin();
	action[1][pos].op = 'a';
}


int main()
{ 
	//���ļ� 
	ifstream fin("grammar_rules.txt");
	ofstream fout("grammar_parse_table.txt");
	
	
	//�����������޹��ķ� 
	readCFG(fin);
	
	
	//����DFA 
	produceDFA(0);
	
	
	//���Follow����
	getFollow();
	
	
	//ʶ��DFA�Ľṹ,�ж���ʲô��������,��������Ӧ�ķ����� 
	int methodID = judgeLRMethod();
	if(methodID==1){
		//����LR0�ķ�����
		produceLR0Table();
	}
	else if(methodID==2){
		//����SLR1�ķ�����
		produceSLR1Table(); 
	}
	else{
		//���ɷ���LR1����������DFA
		produceDFA(2);
		if(!assessLR1_or_LALR1()){
			printf("LALR1�ķ��������޷��������������޹��ķ���\n");
			produceDFA(1);
			if(!assessLR1_or_LALR1()){
				printf("LR1�ķ��������޷��������������޹��ķ���\n");
				return 0;
			}
			else{
				produceLR1_or_LALR1Table();
			}
		}
		else{
			produceLR1_or_LALR1Table(); 
		}
	}
	
	
	//��������� 
	printParseTable(fout);


	//�ر��ļ�
	fin.close();
	fout.close();
	
	return 0;
}
