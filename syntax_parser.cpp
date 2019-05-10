// LL1语法分析器 

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
		// 保留字表 
//		static char* reserve_table[6];
		
		// 操作与边界符号表 
//		static char* ope_deli_table[21];
		
		// 标识符表 
		// char* iden_table[1000];
		
		// 终结符表
		static char* terminal_table[28];
		
		// 非终结符表 
		static char* nonterminal_table[29];
		
		Generative generates[200];  // 产生式表 
		
		set<string> first[29];  // 串首终结符集 
		
		set<string> follow[29];  // 非终结符的后继符号集 
		
		int predict_table[100][100];  // 预测分析表 
		
		vector<string> analysis_stack;  // 分析栈
		vector<string> waiting_input;  // 等待分析的字符串 
		
		int generation_len;  // 产生式表的长度 
		
		// 读入产生式文件 
		void initGenerative(); 
		
		// 预处理后的代码表
//		char* code_table;
		
//		int code_len;
		
		// 查找保留字 
		// success: Tag 
		// fail: -1
//		int searchReserve(char* str);
		
		// 判断是否为非终结符 
		bool isNonterminal(string s);
		
		// 获取非终结符在表中的位置 
		int getNonterminalIndex(string s);
		
		// 获取终结符在表中的位置 
		int getNonterminalIndex(string s);
		
		// 词法扫描程序
//		void Scanner(int& id, int& pos, char* token, int& count);
		
		//  主处理程序
		void Progress(const string path);
		
		// 构造fellow表
		void constructFollow(string target);
		
		// 构造First表 
		void constructFirst(string target);
		
		// 构造预测分析表 
		void constructPredict();
		
		// 使用预测分析栈分析
		int predictAnalyzer(string str);
		
		/* TODO (#1#): 错误输出 */
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
	const char * split = " "; // 空格是分隔符
	string line;
	if(!fin.is_open()){
		cout<<"file fails to open"<<endl;
		exit(0);
	}else{
		while(getline(fin, line, '\n')){
			char str[1280];
    		strcpy(str, line.c_str()); // string转char[] 
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
		    	pch = strtok(NULL, split);  // 注意这里是NULL
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
	int target_pos = getNonterminalIndex(target); // 获取非终结符位置在表中位置 
	for(int i=0;i<generation_len;i++){
		if(generates[i].left == target){
			if(!isNonterminal(generates[i].right[0])){ // 如果是终结符，直接加入集合 
				first[target_pos].insert(generates[i].right[0]);
			}else{ 
				// 如果是非终结符，则 X->Y1Y2Y3...YN，需要加入Y1的First集合
				// 且如果Yi的First集合有empty,则还要加入Yi+1的First集合
				// 如果所有的非终结符Yi的First集合都有empty则加入empty 
				int has_empty;
				for(int j=0;j<generates[i].right.size();j++){
					char r = generates[i].right[j];
					has_empty = 0;
					if(!isNonterminal(r)){ // 如果是终结符，则停止迭代 
						first[target_pos].insert(r);
						break; 
					}else{ // 是终结符则DFS求出它的First集合 
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
						
						if(!has_empty){ // 没有empty则target的first集合已经得出，可结束查找 
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
	int target_pos = getNonterminalIndex(target); // 获取非终结符位置在表中位置 
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
			// 所在产生式形如A->aBc，则First(c)中除了empty元素都加入Follow(B)中
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
				
				// 如果存在empty，且 A!=B 则将Follow(A)加入Follow(B)中 
				if(have_empty && generates[i].left != target){
					
					constructFollow(generates[i].left);  // 首先保证算出A的Follow集合 
					int left_pos = getNonterminalIndex(generates[i].left);
					set<string>::iterator it;
					for(it = follow[left_pos].begin(); it != follow[left_pos].end(); it++){
							follow[target_pos].insert(*it);
					}
				}
				
			}
			
		}else if(tar_in_gene_pos == len - 1 && generates[i].left != target){
			// 所在产生式形如A->aB，且A !=B 则Follow(A)中所有元素都加入Follow(B)中
			
			constructFollow(generates[i].left);  // 首先保证算出A的Follow集合 
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
			if(!isNonterminal(r)){	// 如果为终结符，直接放入，循环结束 
				if(r == "empty"){
					empty_num++;
				}else{
					predict_table[left_pos][r_pos] = i;
				}
				break;
			}else{
				// 如果为非终结符，则将其First表中非空元素加入Select集合构造分析表 
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
		
		// 如果产生式A->a可以推导出空，则将Follow(A)加入select集合构造预测分析表 
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
	
	waiting_input.push_back("@");  // 待分析串插入结束符号 
	
	analysis_stack.push_back("@");  // 分析栈插入结束符号 
	analysis_stack.push_back("program");  // 分析栈插入开始符号
	
	while(analysis_stack.size() > 0){  // 栈非空 
		string input_string = waiting_input.front();  // 当前待分析串指向的第一个词法 
		string stack_top = analysis_stack.back();
		
		if(stack_top == input_string){
			analysis_stack.pop_back();  // 栈顶出栈 
			waiting_input.erase(waiting_input.begin());  // 删除当前指向的终结符 	
		
		}else if(!isNonterminal(stack_top)){  // 如果栈顶为终结符，则报错 
			error();
			return;
		
		}else if(predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)] != -1){  // 预测分析表中有对应项 
			int tag = predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)];
			
			/*输出推导的产生式*/
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

/* TODO (#1#): 构建主程序 */

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
