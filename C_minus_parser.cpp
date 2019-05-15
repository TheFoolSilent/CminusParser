// C-语法法分析器 
// 词法分析器 + LL1语法分析器 
// Author: jiangsw
// 词法分析：350+
// 语法分析：650+ 

#include"Tree.h"
#include<cstring>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<map>
#include<set>
#include<cstring>
# define nonterminal_num 45
# define terminal_num 31
using namespace std;

int FOLLOW_NUM_CYCLE = 15;  // follow递归最大重复循环次数 

// 产生式结构 
struct Generative{
	string left;
	vector<string> right;
};

// 符号表结构 
struct Symbol{
	string type;
	string val;
};


// 词法分析和语法分析器类 
class MonoDevelop{
	public:
		// 保留字表 
		static char* reserve_table[6];
		
		// 操作与边界符号表 
		static char* ope_deli_table[21];
		
		// 标识符表 
		vector<Symbol> symbol_table;
		
		vector<string> waiting_input;  // 非输入终结符化,等待分析的字符串  
		// 预处理后的代码表
		vector<char> code_table;
		
		int code_len;
		
		// 终结符表
		static char* terminal_table[terminal_num];
		
		// 非终结符表 
		static char* nonterminal_table[nonterminal_num];
		
		Generative generates[200];  // 产生式表 
		
		set<string> first[nonterminal_num];  // 串首终结符集 
		
		set<string> follow[nonterminal_num];  // 非终结符的后继符号集 
		
		int predict_table[100][100];  // 预测分析表 
		
		vector<string> analysis_stack;  // 分析栈
		
		Tree* syntax_tree;  // 语法分析树 
		vector<Node*> node_stack;
		
		int generation_len;  // 产生式表的长度 
		
		int stat_cycle[100][100];  // 记录Follow循环次数,如果大于最大次数就终止 
		
		// 对Follow循环次数表与预测分析表预处理 
		MonoDevelop(){
			for(int i=0;i<100;i++)
				for(int j=0;j<100;j++){
					stat_cycle[i][j]=0;
					predict_table[i][j]=-1;
				}

		}
		/*语法部分*/ 
		
		// 读入产生式文件 
		void initGenerative();
		
		// 判断是否为非终结符 
		bool isNonterminal(string s);
		
		// 获取非终结符在表中的位置 
		int getNonterminalIndex(string s);
		
		// 获取终结符在表中的位置 
		int getTerminalIndex(string s);
		
		// 构造fellow表
		void constructFollow(string target);
		
		// 构造First表 
		void constructFirst(string target);
		
		// 构造预测分析表 
		void constructPredict();
		
		// 输出预测分析表 
		void printPredict();

		// 输出First表 
		void printFirst();
		
		// 输出Follow表 
		void printFollow();
		
		// 使用预测分析栈分析
		void syntaxAnalyzer();
		
		/* TODO (#1#): 错误输出 */
		void error(); 
		
		// 语法分析主处理函数 
		void syntaxProgress();
		
		/*词法部分*/ 
		
		// 查找保留字 
		// success: Tag 
		// fail: -1
		int searchReserve(char* str);
		
		// 判断是否为数字 
		bool isNum(char str);
		
		// 判断是否为字符 
		bool isLetter(char str);
		
		// 预处理，去除除空格的空白符
		void preSolve(char* str, int len);
		
		// 词法扫描程序
		void lexScanner(int& id, int& pos, char* token, int& count);
		
		// 输出预处理后的Token流 
		void printTokens(); 
		
		//  词法分析主处理函数 
		void lexProgress(const string path);
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

/*词法部分*/ 
char* MonoDevelop::reserve_table[6]={
			"if","else","return",
			"int","void","while"
};
		
char* MonoDevelop::ope_deli_table[21]={
			"+","-","*","/",
			">",">=","<","<=",
			"==","!=","=",";",
			",","(",")","{",
			"}","/*","*/","[",
			"]"
};

int MonoDevelop::searchReserve(char* str){
	for(int i=0;i<6;i++){
		if(cmp(reserve_table[i], str)){
			return i;
		}
	}
	return -1;
}


bool MonoDevelop::isLetter(char str){
	if(str>='a'&&str<='z' || str>='A'&&str<='Z'){
		return true;
	}
	return false;
} 


bool MonoDevelop::isNum(char str){
	if(str>='0' && str<='9')return true;
	return false;
}


void MonoDevelop::preSolve(char* str, int len){
	// 去除注释与换行 
	for(int i=0;i<len;i++){
		if(str[i] == '/' && str[i+1] == '/' && i<len){
			while(i!='\n'){
				i++;
			}
		}else if(str[i] == '/' && str[i+1] == '*'){
			while(i<len){
				if(str[i] == '*' && str[i+1] == '/'){
					i++;
					break;
				}
				i++;
			}
		}else if(str[i]!='\n'&&str[i]!='\t'){
			code_table.push_back(str[i]);
			code_len++;
		}
	}
	
	code_table.pop_back();
	code_table.push_back('@');  // 插入结束符 
	
}


void MonoDevelop::lexScanner(int& id, int& pos, char* token, int& count){

	// char token[20];
	// count=0;
	for(int i=0;i<20;i++){
		token[i] = '\0';
	}
	
	// 去除空格和制表符 
	while(code_table[pos]==' '){
		pos++;
	}
	// cout<<code_table[pos]<<endl;
	if(isLetter(code_table[pos])){
		token[count++]=code_table[pos];
		pos++;
		while(isLetter(code_table[pos]) || isNum(code_table[pos])){
			token[count++]=code_table[pos];
			pos++;
		}
		token[count]='\0';
		id = searchReserve(token);
		if(id == -1){
			id = 100; // 标识符码 
		}
		return; 
	}else if(isNum(code_table[pos])){
		token[count++]=code_table[pos];
		pos++;
		while(isNum(code_table[pos])){
			token[count++]=code_table[pos];
			pos++;
		}
		token[count]='\0';
		id = 101; // 常数码 
	}else if(code_table[pos]=='+'||code_table[pos]=='-'||code_table[pos]=='*'||code_table[pos]=='/'
			||code_table[pos]=='['||code_table[pos]==']'||code_table[pos]=='{'||code_table[pos]=='}'
			||code_table[pos]=='('||code_table[pos]==')'||code_table[pos]==';'||code_table[pos]==','){
		token[0]=code_table[pos];
		token[1]='\0';
		for(int i=0;i<21;i++){
			if(cmp(ope_deli_table[i], token)){
				id = 6 +i;
				// cout<<"ope:"<<ope_deli_table[i]<<endl;
				// cout<<"***:"<<id<<endl;
				break;
			}
		}
		pos++;
		return ;
	}else if(code_table[pos]=='>'){
		pos++;
		token[0]='>';
		if(code_table[pos]=='='){
			id = 12;
			token[0]='=';
		}else{
			id = 11;
			pos--;
		}
		pos++;
		return;
	}else if(code_table[pos]=='<'){
		pos++;
		token[0]='<';
		if(code_table[pos]=='='){
			id = 14;
			token[1]='=';
		}else{
			id = 13;
			pos--;
		}
		pos++;
		return;
		
	}else if(code_table[pos]=='='){
		pos++;
		token[0]='=';
		if(code_table[pos]=='='){
			id = 15;
			token[1]='=';
		}else{
			id = 17;
			pos--;
		}
		pos++;
		return;
		
	}else if(code_table[pos]=='!'){
		pos++;
		token[0]='!';
		if(code_table[pos]=='='){
			id = 16;
			token[1]='=';
		}else{
			id = 30;
			pos--;
		}
		pos++;
		return;
		
	}else if(code_table[pos]=='@'){
		id = 200; // 表示程序结束 
		return;
	}
		
}

void MonoDevelop::lexProgress(const string path){
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
		preSolve(ch, cnt);
		// cout<<"Prepared:\n"<<code_table<<endl;	
	}
	
//	fstream fout("lexunit.txt", ios::out);
//	if(!fout.is_open()){
//		cout<<"词法单元输出文件打开出错"<<endl;
//		exit(0);
//	}
	
	int id=-1;
	int pos=0;
	
	// cout<<"Length: "<<code_len<<endl;
	while(id!=200){
		char str_word[20];
		int word_len=0;
		lexScanner(id,pos,str_word,word_len);
		if(id == 100){ // 标识符 
//			fout<<"ID, name= "<<str_word<<endl;
			Symbol bian;
			bian.type = "ID";
			bian.val = str_word;
			symbol_table.push_back(bian);
			waiting_input.push_back("ID");
		}else if(id >= 0 && id <= 5){ // 保留字 
//			fout<<"Reserve Word: "<<str_word<<endl;
			waiting_input.push_back(str_word);
		}else if(id == 101){ // 常数 
//			fout<<"NUM, val= "<<str_word<<endl;
			waiting_input.push_back("NUM");
			Symbol bian;
			bian.type = "NUM";
			bian.val = str_word;
			symbol_table.push_back(bian);
		}else if(id>5 && id<27){ // 运算与边界符 
//			fout<<str_word<<endl;
			waiting_input.push_back(str_word);
		}
	
	}
	
	
}


void MonoDevelop::printTokens(){
	fstream fout("precode.txt", ios::out);
	if(!fout.is_open()){
		cout<<"预处理后代码输出文件打开出错"<<endl;
		exit(0);
	}
	vector<string>::iterator it;
	vector<Symbol>::iterator sym;
	sym = symbol_table.begin();
	for(it = waiting_input.begin();it != waiting_input.end(); it++){
		if(*it == "NUM" || *it == "ID"){
			fout<<(*sym).val<<" ";
			sym++;
		}else fout<<*it<<" ";
	}
	fout.close();
}


/*语法部分*/ 

char* MonoDevelop::terminal_table[]={
			"if","else","return",
			"int","void","while",
			"+","-","*","/",
			">",">=","<","<=",
			"==","!=","=",";",
			",","(",")","{",
			"}","/*","*/","[",
			"]" ,"empty","ID","NUM",
			"@"
	};
 
char* MonoDevelop::nonterminal_table[]={
	"program",
	"declaration-list",
	"additive-expression",
	"additive-expression-1",
	"addop",
	"arg-list",
	"arg-list-1",
	"args",
	"compound-stmt",
	"declaration",
	"declaration-1",
	"declaration-list-1",
	"expression",
	"expression-1",
	"expression-2",
	"expression-stmt",
	"factor",
	"factor-1",
	"fun-declaration",
	"iteration-stmt",
	"local-declarations",
	"local-declarations-1",
	"mulop",
	"param",
	"param-1",
	"param-list",
	"param-list-1",
	"params",
	"params-1",
	"relop",
	"return-stmt",
	"return-stmt-1",
	"selection-stmt",
	"selection-stmt-1",
	"simple-expression",
	"simple-expression-1",
	"statement",
	"statement-list",
	"statement-list-1",
	"term",
	"term-1",
	"type-specifier",
	"var-1",
	"var-declaration",
	"var-declaration-1"
};


void MonoDevelop::initGenerative(){
	ifstream fin("grammar.txt", ios::in);
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
			pch = strtok(NULL, split); // ->
			pch = strtok(NULL, split); // start right
			while(pch != NULL) {
//				cout<<"pch: "<<pch<<endl;
				if(*pch == '|'){

					cnt++;
					generates[cnt].left = generates[cnt-1].left;
					
				}else{
					generates[cnt].right.push_back(pch);
				}
		    	pch = strtok(NULL, split);  // 注意这里是NULL
		    }

		    cnt++;
		}
		fin.close(); 
	}
	generation_len = cnt;

}

bool MonoDevelop::isNonterminal(string s){
	for(int i=0;i<nonterminal_num;i++){
		string sc(nonterminal_table[i]);
		if(sc == s){
			return true;
		}
	}
	return false;
}

int MonoDevelop::getTerminalIndex(string str){
	for(int i=0;i<terminal_num;i++){
		string sc(terminal_table[i]);
		if(sc == str){
			return i;
		}
	}
	return -1;
}

int MonoDevelop::getNonterminalIndex(string str){
	for(int i=0;i<nonterminal_num;i++){
		string sc(nonterminal_table[i]);
		if(sc == str){
			return i;
		}
	}
	return -1;
}


void MonoDevelop::constructFirst(string target){
	int target_pos = getNonterminalIndex(target); // 获取非终结符位置在表中位置 
//	cout<<"target: "<<target<<"  "<<"pos: "<<target_pos<<endl;
	for(int i=0;i<generation_len;i++){
		if(generates[i].left == target){
			if(!isNonterminal(generates[i].right[0])){ // 如果是终结符，直接加入集合 
				first[target_pos].insert(generates[i].right[0]);
			}else{ 
				// 如果是非终结符，则 X->Y1Y2Y3...YN，需要加入Y1的First集合
				// 且如果Yi的First集合有empty,则还要加入Yi+1的First集合
				// 如果所有的非终结符Yi的First集合都有empty则加入empty 
				int have_empty;
				for(int j=0;j<generates[i].right.size();j++){
					string r = generates[i].right[j];
					have_empty = 0;
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
						
						if(!have_empty){ // 没有empty则target的first集合已经得出，可结束查找 
							break; 
						}
						
					}
					
				}
				
				if(have_empty){
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
		int len = generates[i].right.size();
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
				
				int tar_next_pos = getNonterminalIndex(tar_next);
				int have_empty = 0;
				
				set<string>::iterator it;
				for(it = first[tar_next_pos].begin(); it != first[tar_next_pos].end(); it++){
					if(*it == "empty"){
						have_empty = 1;
					}else{
						follow[target_pos].insert(*it);
					}
				}
				
				// 如果存在empty，且 A!=B
				if(have_empty && generates[i].left != target){
					
					constructFollow(tar_next);  // 首先保证算出tar_next的Follow集合 
					
					set<string>::iterator it;
					for(it = follow[tar_next_pos].begin(); it != follow[tar_next_pos].end(); it++){
							follow[target_pos].insert(*it);
					}
				}
				
			}
			
		}else if(tar_in_gene_pos == len - 1 && generates[i].left != target){
			// 所在产生式形如A->aB，且A !=B 则Follow(A)中所有元素都加入Follow(B)中
			
			int left_pos = getNonterminalIndex(generates[i].left);
			stat_cycle[target_pos][left_pos]++;
			if(stat_cycle[target_pos][left_pos] >= FOLLOW_NUM_CYCLE)return;
			
			constructFollow(generates[i].left);  // 首先保证算出A的Follow集合 

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
			if(!isNonterminal(r)){	// 如果为终结符，直接放入，循环结束 

				if(r == "empty"){
					empty_num++;
				}else{
					int r_pos = getTerminalIndex(r);
					predict_table[left_pos][r_pos] = i;
				}
				break;
			}else{
//				cout<<"YES_2"<<endl;
				// 如果为非终结符，则将其First表中非空元素加入Select集合构造分析表 
				int r_pos = getNonterminalIndex(r);
				set<string>::iterator it;
				for(it = first[r_pos].begin(); it != first[r_pos].end(); it++){
					string sc(*it);
					if(sc == "empty"){
						empty_num++;
					}else{

						int it_pos = getTerminalIndex(*it);
						predict_table[left_pos][it_pos] = i;
					}
				}
				
				if(first[r_pos].count("empty") == 0){
					break;
				}
			}
			
		}
		
		// 如果产生式A->a可以推导出空，则将Follow(A)加入select集合构造预测分析表 
		if(empty_num == right_len){
			set<string>::iterator it;
			for(it = follow[left_pos].begin(); it != follow[left_pos].end(); it++){
					int it_pos = getTerminalIndex(*it);
					if(predict_table[left_pos][it_pos]!=-1)continue;
					predict_table[left_pos][it_pos] = i;
			}
		}
		
	} 
	
}

// 输出FIrst集合 
void MonoDevelop::printFirst(){
	cout<<endl<<"FIRST TABLE:"<<endl;
	set<string>::iterator it;
	for(int i=0;i<nonterminal_num;i++){
		cout<<nonterminal_table[i]<<" ------> "<<endl;
		cout<<"{ ";
		for(it=first[i].begin(); it != first[i].end(); it++){
			cout<<*it<<"  ";
		}
		cout<<"  }"<<endl;
		cout<<"=================================="<<endl;
	}
}

// 输出Follow集合 
void MonoDevelop::printFollow(){
	cout<<"FOLLOW TABLE:"<<endl;
	set<string>::iterator it;
	for(int i=0;i<nonterminal_num;i++){
		cout<<nonterminal_table[i]<<" ------> "<<endl;
		cout<<"{ ";
		for(it=follow[i].begin(); it != follow[i].end(); it++){
			cout<<*it<<"  ";
		}
		cout<<"  }"<<endl;
		cout<<"=================================="<<endl;
	}
}

// 输出预测分析表 
void MonoDevelop::printPredict(){
	cout<<"PREDICT TABLE:"<<endl;
	
	for(int i=0;i<nonterminal_num;i++){
		cout<<nonterminal_table[i]<<" ------> "<<endl;
		for(int j=0;j<terminal_num;j++){
			int pro_pos = predict_table[i][j];
			if(pro_pos == -1)continue;
			cout<<terminal_table[j]<<"  :  ";
			cout<<generates[pro_pos].left<<"  ->  ";
			for(int p=0;p<generates[pro_pos].right.size();p++)
				cout<<generates[pro_pos].right[p]<<" ";
			cout<<endl;
		}
		cout<<"=================================="<<endl;
	}
}

// 用分析栈的方法分析LL1文法 
void MonoDevelop::syntaxAnalyzer(){
	// 将每一步推导所用的产生式输出到文件 
	fstream fou("producer.txt", ios::out);
	if(!fou.is_open()){
		cout<<"产生式输出文件打开出错"<<endl;
		exit(0);
	}
	
	waiting_input.push_back("@");  // 待分析串插入结束符号 
	
	analysis_stack.push_back("@");  // 分析栈插入结束符号 
	
	analysis_stack.push_back("program");  // 分析栈插入开始符号
	
	syntax_tree = new Tree("program");  // 语法分析树根节点 
	node_stack.push_back(syntax_tree->root); // 根节点入栈 
	
	while(analysis_stack.size() > 0){  // 栈非空 
		string input_string = waiting_input.front();  // 当前待分析串指向的第一个词法 
		string stack_top = analysis_stack.back();  // 分析栈的栈顶 
		
		if(stack_top == input_string){
			
			if(stack_top == "ID" || stack_top == "NUM"){
				//  对推导出ID或NUM终结符的情况特殊处理 
				vector<Symbol>::iterator it = symbol_table.begin();
				string p = (*it).val;
				fou<<stack_top<<"->"<<p<<" "<<endl;
				symbol_table.erase(symbol_table.begin());
				
				// 构建树叶节点 
				Node* now_root = node_stack.back();
				Node* new_point1 = new Node();
				new_point1->val = p;
				syntax_tree->insert(new_point1, now_root);
				node_stack.pop_back();
			}
			
			analysis_stack.pop_back();  // 栈顶出栈 
			waiting_input.erase(waiting_input.begin());  // 删除当前指向的终结符 	
		
		}else if(!isNonterminal(stack_top)){  // 如果栈顶为终结符，则报错 
//			cout<<"now_stacktop:  "<<stack_top<<"   input:  "<<input_string<<endl;
			fou<<"---------------ERROR!-----------------"<<endl;
			error();
			exit(0);
		
		}else if(predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)] != -1){  // 预测分析表中有对应项 
			int tag = predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)];
			
			Node* now_root = node_stack.back();  // 取出栈顶（产生式左边的结点）
			node_stack.pop_back(); 
			for(int i=generates[tag].right.size()-1;i>=0;i--){
				Node* new_point0 = new Node();
				new_point0->val = generates[tag].right[i];
				syntax_tree->insert(new_point0, now_root);
				if(isNonterminal(generates[tag].right[i]) || generates[tag].right[i] == "NUM" || generates[tag].right[i] == "ID"){
					node_stack.push_back(new_point0);  // 节点入栈 
				}
			}
//			delete now_root;
			
			/*输出推导的产生式*/
			fou<<generates[tag].left<<"->";
			for(int i=0;i<generates[tag].right.size();i++){
				fou<<generates[tag].right[i]<<" ";
			}
			fou<<endl;
//			cout<<"!  "<<input_string<<"  !"<<endl;
			
			analysis_stack.pop_back();  // 产生式左部出栈 
			
			if(generates[tag].right[0] != "empty"){  // 推导为不为空时产生式右部入栈 
				for(int i=generates[tag].right.size()-1;i>=0;i--){
					analysis_stack.push_back(generates[tag].right[i]);
				}
			}
			
		}else{
			
//			cout<<"top1:  "<<stack_top<<"   input:  "<<input_string<<endl;
			fou<<"---------------ERROR!-----------------"<<endl;
			error();
			exit(0); 
		} 
	}
	
	fou<<"FINISHED: OK!"<<endl;
}

void MonoDevelop::error(){
	cout<<"---------------ERROR!-----------------"<<endl;
} 

void MonoDevelop::syntaxProgress(){
	initGenerative();  // 初始化产生式 

//	cout<<"Begin constructFirst"<<endl;
	
	for(int i=0;i<nonterminal_num;i++){  // 为每个非终结符构造First集合 
		constructFirst(nonterminal_table[i]);
	}
	
//	printFirst();
	
//	cout<<"Begin constructFollow"<<endl;
	
	for(int i=0;i<nonterminal_num;i++){  //为每个非终结符构造Follow集合 
		if(i == 0){
			follow[0].insert("@");
		}
		constructFollow(nonterminal_table[i]);
		
	}
//	printFollow();
	
//	cout<<"Begin constructPredict"<<endl;
	constructPredict();  // 构造预测分析表 
	
//	printPredict();
	
//	cout<<"Begin syntaxAnalyzer"<<endl;
	syntaxAnalyzer();  // 分析语法 
	
} 

int main(){
	MonoDevelop developer = MonoDevelop();
//	cout<<"Begin LexProgress"<<endl;
	developer.lexProgress("/testfile/example.c-");
//	developer.printTokens();
//	cout<<"Begin syntaxProgress"<<endl;
	developer.syntaxProgress();
//	cout<<"program"<<endl;
	developer.syntax_tree->printTree(1, developer.syntax_tree->root);

	return 0;
}
