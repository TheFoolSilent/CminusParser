// LL1�﷨������ 

#include<iostream>
#include<cstring>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<map>
#include<vector>
#include<set>
#include<typeinfo>
using namespace std;

struct Generative{
	string left;
	vector<string> right;
};

class MonoDevelop{
	public:
		// �����ֱ� 
//		static char* reserve_table[6];
		
		// ������߽���ű� 
//		static char* ope_deli_table[21];
		
		// ��ʶ���� 
		// char* iden_table[1000];
		
		// �ս����
		static char* terminal_table[28];
		
		// ���ս���� 
		static char* nonterminal_table[29];
		
		Generative generates[200];  // ����ʽ�� 
		
		set<string> first[29];  // �����ս���� 
		
		set<string> follow[29];  // ���ս���ĺ�̷��ż� 
		
		int predict_table[100][100];  // Ԥ������� 
		
		vector<string> analysis_stack;  // ����ջ
		vector<string> waiting_input;  // �ȴ��������ַ��� 
		
		int generation_len;  // ����ʽ��ĳ��� 
		
		// �������ʽ�ļ� 
		void initGenerative(); 
		
		// Ԥ�����Ĵ����
//		char* code_table;
		
//		int code_len;
		
		// ���ұ����� 
		// success: Tag 
		// fail: -1
//		int searchReserve(char* str);
		
		// �ж��Ƿ�Ϊ���ս�� 
		bool isNonterminal(string s);
		
		// ��ȡ���ս���ڱ��е�λ�� 
		int getNonterminalIndex(string s);
		
		// ��ȡ�ս���ڱ��е�λ�� 
		int getNonterminalIndex(string s);
		
		// �ʷ�ɨ�����
//		void Scanner(int& id, int& pos, char* token, int& count);
		
		//  ���������
		void Progress(const string path);
		
		// ����fellow��
		void constructFollow(string target);
		
		// ����First�� 
		void constructFirst(string target);
		
		// ����Ԥ������� 
		void constructPredict();
		
		// ʹ��Ԥ�����ջ����
		int predictAnalyzer(string str);
		
		/* TODO (#1#): ������� */
		void error(); 
		
	private:
		bool cmp(char* a, char* b){
			int alen = strlen(a);
			// cout<<"a: "<<a<<"  b: "<<b<<endl;
			// cout<<"alen: "<<alen<<endl;
			if(alen!=strlen(b))return false;
			for(int i=0;i<alen;i++){
				if(a[i]!=b[i]){
					return false;
				}
			}
			
			return true;
		}
};

char* MonoDevelop::terminal_table[28]={
			"if","else","return",
			"int","void","while",
			"+","-","*","/",
			">",">=","<","<=",
			"==","!=","=",";",
			",","(",")","{",
			"}","/*","*/","[",
			"]" ,"empty"
	};
		
char* MonoDevelop::nonterminal_table[29]={
			"program",
			"addop",
			"args",
			"arg_list",
			"additive_expression",
			"call",
			"compound_stmt",
			"declaration_list",
			"declaration",
			"expression",
			"expression_stmt",
			"factor",
			"fun_declaration",
			"type_specifier",
			"params",
			"param",
			"param_list",
			"local_declarations",
			"statement_list",
			"statement",
			"selection_stmt",
			"iteration_stmt",
			"return_stmt",
			"simple_expression",
			"relop",
			"term",
			"mulop",
			"var_declaration",
			"var"
	};
	

void MonoDevelop::initGenerative(){
	ifstream fin("principles.txt", ios::in);
	int cnt=0;
	char* pch;
	const char * split = " "; // �ո��Ƿָ���
	string line;
	if(!fin.is_open()){
		cout<<"file fails to open"<<endl;
		exit(0);
	}else{
		while(getline(fin, line, '\n')){
			char str[1280];
    		strcpy(str, line.c_str()); // stringתchar[] 
			pch = strtok(str, split);  
			string sc(pch);
			generates[cnt].left = sc;
//			cout<<generates[cnt].left<<"  "<<cnt<<endl;
			pch = strtok(NULL, split); // ->
			pch = strtok(NULL, split); // start right
			while(pch != NULL) {
//				cout<<"pch: "<<pch<<endl;
				if(*pch == '|'){
//					cout<<"ok"<<endl;
					cnt++;
					generates[cnt].left = generates[cnt-1].left;
					
				}else{
					
					generates[cnt].right.push_back(pch);
//					cout<<generates[cnt].right[i]<<endl;
				}
		    	pch = strtok(NULL, split);  // ע��������NULL
		    }
//		    system("pause");
		    cnt++;
		}
		fin.close();
	}
	generation_len = cnt;
//	cout<<"--------------------------"<<endl;
//	for(int i=0;i<cnt;i++){
//		cout<<"left:  "<<generates[i].left<<endl;//<<"+"<<generates[i].right.at(0)<<endl;
//		for(int j=0;j<generates[i].right.size();j++)
//			cout<<generates[i].right[j];
//		cout<<endl;
//	}
//	cout<<<<endl;
}

bool isNonterminal(string s){
	for(int i=0;i<strlen(nonterminal_table);i++){
		string sc(nonterminal_table[i]);
		if(sc == s){
			return true;
		}
	}
	return false;
}

int getTerminalIndex(string str){
	for(int i=0;i<strlen(terminal_table);i++){
		if(terminal_table[i] == str){
			return i;
		}
	}
	return -1;
}

int getNonterminalIndex(string str){
	for(int i=0;i<strlen(nonterminal_table);i++){
		string sc(nonterminal_table[i]);
		if(sc == str){
			return i;
		}
	}
	return -1;
}


void MonoDevelop::constructFirst(string target){
	int target_pos = getNonterminalIndex(target); // ��ȡ���ս��λ���ڱ���λ�� 
	for(int i=0;i<generation_len;i++){
		if(generates[i].left == target){
			if(!isNonterminal(generates[i].right[0])){ // ������ս����ֱ�Ӽ��뼯�� 
				first[target_pos].insert(generates[i].right[0]);
			}else{ 
				// ����Ƿ��ս������ X->Y1Y2Y3...YN����Ҫ����Y1��First����
				// �����Yi��First������empty,��Ҫ����Yi+1��First����
				// ������еķ��ս��Yi��First���϶���empty�����empty 
				int has_empty;
				for(int j=0;j<generates[i].right.size();j++){
					char r = generates[i].right[j];
					has_empty = 0;
					if(!isNonterminal(r)){ // ������ս������ֹͣ���� 
						first[target_pos].insert(r);
						break; 
					}else{ // ���ս����DFS�������First���� 
						constructFirst(r);
						
						int r_pos = getNonterminalIndex(r);
						set<string>::iterator it;
						for(it = first[r_pos].begin(); it != first[r_pos].end(); it++){
							if(*it == "empty"){
								have_empty = 1;
							}else{
								first[target_pos].insert(*it);
							}
						}
						
						if(!has_empty){ // û��empty��target��first�����Ѿ��ó����ɽ������� 
							break; 
						}
						
					}
					
				}
				
				if(has_empty){
					first[target_pos].insert("empty");
				}
			}
		}
	}
}


void MonoDevelop::constructFollow(string target){
	int target_pos = getNonterminalIndex(target); // ��ȡ���ս��λ���ڱ���λ�� 
	for(int i=0;i<generation_len;i++){
		int tar_in_gene_pos = -1;
		len = generates[i].right.size();
		for(int j=0;j<len;j++){
			if(target == generates[i].right[j]){
				tar_in_gene_pos = j;
				break;
			}
		}
		
		if(tar_in_gene_pos == -1)continue;
		
		if(tar_in_gene_pos < len - 1){
			// ���ڲ���ʽ����A->aBc����First(c)�г���emptyԪ�ض�����Follow(B)��
			string tar_next = generates[i].right[tar_in_gene_pos+1];
			if(!isNonterminal(tar_next)){
				follow[target_pos].insert(tar_next);
			}else{
				int tar_next_pos = getNomternimalIndex(tar_next);
				int have_empty = 0;
				set<string>::iterator it;
				for(it = first[tar_next_pos].begin(); it != first[tar_next_pos].end(); it++){
					if(*it == "empty"){
						have_empty = 1;
					}else{
						first[target_pos].insert(*it);
					}
				}
				
				// �������empty���� A!=B ��Follow(A)����Follow(B)�� 
				if(have_empty && generates[i].left != target){
					
					constructFollow(generates[i].left);  // ���ȱ�֤���A��Follow���� 
					int left_pos = getNonterminalIndex(generates[i].left);
					set<string>::iterator it;
					for(it = follow[left_pos].begin(); it != follow[left_pos].end(); it++){
							follow[target_pos].insert(*it);
					}
				}
				
			}
			
		}else if(tar_in_gene_pos == len - 1 && generates[i].left != target){
			// ���ڲ���ʽ����A->aB����A !=B ��Follow(A)������Ԫ�ض�����Follow(B)��
			
			constructFollow(generates[i].left);  // ���ȱ�֤���A��Follow���� 
			int left_pos = getNonterminalIndex(generates[i].left);
			set<string>::iterator it;
			for(it = follow[left_pos].begin(); it != follow[left_pos].end(); it++){
					follow[target_pos].insert(*it);
			}
		}
		
	}
}


void MonoDevelop::constructPredict(){
	for(int i=0;i<generation_len;i++){
		int left_pos = getNonterminalIndex(generates[i].left);
		int right_len = generates[i].right.size();
		int empty_num=0;
		for(int j=0;j<right_len;j++){
			string r = generates[i].right[j];
			int r_pos = getIndex(r);
			if(!isNonterminal(r)){	// ���Ϊ�ս����ֱ�ӷ��룬ѭ������ 
				if(r == "empty"){
					empty_num++;
				}else{
					predict_table[left_pos][r_pos] = i;
				}
				break;
			}else{
				// ���Ϊ���ս��������First���зǿ�Ԫ�ؼ���Select���Ϲ�������� 
				set<string>::iterator it;
				for(it = firt[r_pos].begin(); it != first[r_pos].end(); it++){
					if(*it == "empty"){
						empty_num++;
					}else{
						int it_pos = getIndex(*it);
						predict_table[left_pos][it_pos] = i;	
					}
				}
				
				if(first[r_pos].count("empty") != 0){
					break;
				}
			}
			
		}
		
		// �������ʽA->a�����Ƶ����գ���Follow(A)����select���Ϲ���Ԥ������� 
		if(empty_num == right_len){
			set<string>::iterator it;
			for(it = follow[left_pos].begin(); it != follow[left_pos].end(); it++){
					int it_pos = getIndex(*it); 
					predict_table[left_pos][it_pos] = i;
			}
		}
		
	} 
	
}


int MonoDevelop::predictAnalyzer(){
	
	waiting_input.push_back("@");  // ������������������� 
	
	analysis_stack.push_back("@");  // ����ջ����������� 
	analysis_stack.push_back("program");  // ����ջ���뿪ʼ����
	
	while(analysis_stack.size() > 0){  // ջ�ǿ� 
		string input_string = waiting_input.front();  // ��ǰ��������ָ��ĵ�һ���ʷ� 
		string stack_top = analysis_stack.back();
		
		if(stack_top == input_string){
			analysis_stack.pop_back();  // ջ����ջ 
			waiting_input.erase(waiting_input.begin());  // ɾ����ǰָ����ս�� 	
		
		}else if(!isNonterminal(stack_top)){  // ���ջ��Ϊ�ս�����򱨴� 
			error();
			return;
		
		}else if(predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)] != -1){  // Ԥ����������ж�Ӧ�� 
			int tag = predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)];
			
			/*����Ƶ��Ĳ���ʽ*/
			cout<<generates[tag].left<<"->";
			for(int i=0;i<generates[tag].right.size();i++){
				cout<<generates[tag].right[i]<<" ";
			}
			cout<<endl;
			
			analysis_stack.pop_back();
			
			if(generates[tag].right[0] != "empty"){
				for(int i=generates[tag].right.size()-1;i>=0;i--){
					analysis_stack.push_back(generates[tag].right[i]);
				}
			}
			
		}else{
			error();
			return; 
		} 
	} 
	
	 
}

/* TODO (#1#): ���������� */

void MonoDevelop::Progress(const string path){
	// char buffer[256];
	fstream file;
	char* ch = new char[1000];
	int cnt=0;
	
	file.open(path); 
	if(!file.is_open()){
		cout<<"file fails to open"<<endl;
		exit(0);
	}else{
		while(!file.eof()){
			file.get(ch[cnt++]);
		}
		file.close();
//		preSolve(ch, cnt);
		// cout<<"Prepared:\n"<<code_table<<endl;	
	}
	
	
	
}

int main(){
	MonoDevelop developer = MonoDevelop();
	developer.initGenerative();
//	developer.Progress("example.c-");
}
