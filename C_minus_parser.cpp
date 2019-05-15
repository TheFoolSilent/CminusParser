// C-�﷨�������� 
// �ʷ������� + LL1�﷨������ 
// Author: jiangsw
// �ʷ�������350+
// �﷨������650+ 

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

int FOLLOW_NUM_CYCLE = 15;  // follow�ݹ�����ظ�ѭ������ 

// ����ʽ�ṹ 
struct Generative{
	string left;
	vector<string> right;
};

// ���ű�ṹ 
struct Symbol{
	string type;
	string val;
};


// �ʷ��������﷨�������� 
class MonoDevelop{
	public:
		// �����ֱ� 
		static char* reserve_table[6];
		
		// ������߽���ű� 
		static char* ope_deli_table[21];
		
		// ��ʶ���� 
		vector<Symbol> symbol_table;
		
		vector<string> waiting_input;  // �������ս����,�ȴ��������ַ���  
		// Ԥ�����Ĵ����
		vector<char> code_table;
		
		int code_len;
		
		// �ս����
		static char* terminal_table[terminal_num];
		
		// ���ս���� 
		static char* nonterminal_table[nonterminal_num];
		
		Generative generates[200];  // ����ʽ�� 
		
		set<string> first[nonterminal_num];  // �����ս���� 
		
		set<string> follow[nonterminal_num];  // ���ս���ĺ�̷��ż� 
		
		int predict_table[100][100];  // Ԥ������� 
		
		vector<string> analysis_stack;  // ����ջ
		
		Tree* syntax_tree;  // �﷨������ 
		vector<Node*> node_stack;
		
		int generation_len;  // ����ʽ��ĳ��� 
		
		int stat_cycle[100][100];  // ��¼Followѭ������,�����������������ֹ 
		
		// ��Followѭ����������Ԥ�������Ԥ���� 
		MonoDevelop(){
			for(int i=0;i<100;i++)
				for(int j=0;j<100;j++){
					stat_cycle[i][j]=0;
					predict_table[i][j]=-1;
				}

		}
		/*�﷨����*/ 
		
		// �������ʽ�ļ� 
		void initGenerative();
		
		// �ж��Ƿ�Ϊ���ս�� 
		bool isNonterminal(string s);
		
		// ��ȡ���ս���ڱ��е�λ�� 
		int getNonterminalIndex(string s);
		
		// ��ȡ�ս���ڱ��е�λ�� 
		int getTerminalIndex(string s);
		
		// ����fellow��
		void constructFollow(string target);
		
		// ����First�� 
		void constructFirst(string target);
		
		// ����Ԥ������� 
		void constructPredict();
		
		// ���Ԥ������� 
		void printPredict();

		// ���First�� 
		void printFirst();
		
		// ���Follow�� 
		void printFollow();
		
		// ʹ��Ԥ�����ջ����
		void syntaxAnalyzer();
		
		/* TODO (#1#): ������� */
		void error(); 
		
		// �﷨������������ 
		void syntaxProgress();
		
		/*�ʷ�����*/ 
		
		// ���ұ����� 
		// success: Tag 
		// fail: -1
		int searchReserve(char* str);
		
		// �ж��Ƿ�Ϊ���� 
		bool isNum(char str);
		
		// �ж��Ƿ�Ϊ�ַ� 
		bool isLetter(char str);
		
		// Ԥ����ȥ�����ո�Ŀհ׷�
		void preSolve(char* str, int len);
		
		// �ʷ�ɨ�����
		void lexScanner(int& id, int& pos, char* token, int& count);
		
		// ���Ԥ������Token�� 
		void printTokens(); 
		
		//  �ʷ������������� 
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

/*�ʷ�����*/ 
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
	// ȥ��ע���뻻�� 
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
	code_table.push_back('@');  // ��������� 
	
}


void MonoDevelop::lexScanner(int& id, int& pos, char* token, int& count){

	// char token[20];
	// count=0;
	for(int i=0;i<20;i++){
		token[i] = '\0';
	}
	
	// ȥ���ո���Ʊ�� 
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
			id = 100; // ��ʶ���� 
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
		id = 101; // ������ 
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
		id = 200; // ��ʾ������� 
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
//		cout<<"�ʷ���Ԫ����ļ��򿪳���"<<endl;
//		exit(0);
//	}
	
	int id=-1;
	int pos=0;
	
	// cout<<"Length: "<<code_len<<endl;
	while(id!=200){
		char str_word[20];
		int word_len=0;
		lexScanner(id,pos,str_word,word_len);
		if(id == 100){ // ��ʶ�� 
//			fout<<"ID, name= "<<str_word<<endl;
			Symbol bian;
			bian.type = "ID";
			bian.val = str_word;
			symbol_table.push_back(bian);
			waiting_input.push_back("ID");
		}else if(id >= 0 && id <= 5){ // ������ 
//			fout<<"Reserve Word: "<<str_word<<endl;
			waiting_input.push_back(str_word);
		}else if(id == 101){ // ���� 
//			fout<<"NUM, val= "<<str_word<<endl;
			waiting_input.push_back("NUM");
			Symbol bian;
			bian.type = "NUM";
			bian.val = str_word;
			symbol_table.push_back(bian);
		}else if(id>5 && id<27){ // ������߽�� 
//			fout<<str_word<<endl;
			waiting_input.push_back(str_word);
		}
	
	}
	
	
}


void MonoDevelop::printTokens(){
	fstream fout("precode.txt", ios::out);
	if(!fout.is_open()){
		cout<<"Ԥ������������ļ��򿪳���"<<endl;
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


/*�﷨����*/ 

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
		    	pch = strtok(NULL, split);  // ע��������NULL
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
	int target_pos = getNonterminalIndex(target); // ��ȡ���ս��λ���ڱ���λ�� 
//	cout<<"target: "<<target<<"  "<<"pos: "<<target_pos<<endl;
	for(int i=0;i<generation_len;i++){
		if(generates[i].left == target){
			if(!isNonterminal(generates[i].right[0])){ // ������ս����ֱ�Ӽ��뼯�� 
				first[target_pos].insert(generates[i].right[0]);
			}else{ 
				// ����Ƿ��ս������ X->Y1Y2Y3...YN����Ҫ����Y1��First����
				// �����Yi��First������empty,��Ҫ����Yi+1��First����
				// ������еķ��ս��Yi��First���϶���empty�����empty 
				int have_empty;
				for(int j=0;j<generates[i].right.size();j++){
					string r = generates[i].right[j];
					have_empty = 0;
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
						
						if(!have_empty){ // û��empty��target��first�����Ѿ��ó����ɽ������� 
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
	int target_pos = getNonterminalIndex(target); // ��ȡ���ս��λ���ڱ���λ�� 
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
			
			// ���ڲ���ʽ����A->aBc����First(c)�г���emptyԪ�ض�����Follow(B)��
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
				
				// �������empty���� A!=B
				if(have_empty && generates[i].left != target){
					
					constructFollow(tar_next);  // ���ȱ�֤���tar_next��Follow���� 
					
					set<string>::iterator it;
					for(it = follow[tar_next_pos].begin(); it != follow[tar_next_pos].end(); it++){
							follow[target_pos].insert(*it);
					}
				}
				
			}
			
		}else if(tar_in_gene_pos == len - 1 && generates[i].left != target){
			// ���ڲ���ʽ����A->aB����A !=B ��Follow(A)������Ԫ�ض�����Follow(B)��
			
			int left_pos = getNonterminalIndex(generates[i].left);
			stat_cycle[target_pos][left_pos]++;
			if(stat_cycle[target_pos][left_pos] >= FOLLOW_NUM_CYCLE)return;
			
			constructFollow(generates[i].left);  // ���ȱ�֤���A��Follow���� 

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
			if(!isNonterminal(r)){	// ���Ϊ�ս����ֱ�ӷ��룬ѭ������ 

				if(r == "empty"){
					empty_num++;
				}else{
					int r_pos = getTerminalIndex(r);
					predict_table[left_pos][r_pos] = i;
				}
				break;
			}else{
//				cout<<"YES_2"<<endl;
				// ���Ϊ���ս��������First���зǿ�Ԫ�ؼ���Select���Ϲ�������� 
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
		
		// �������ʽA->a�����Ƶ����գ���Follow(A)����select���Ϲ���Ԥ������� 
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

// ���FIrst���� 
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

// ���Follow���� 
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

// ���Ԥ������� 
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

// �÷���ջ�ķ�������LL1�ķ� 
void MonoDevelop::syntaxAnalyzer(){
	// ��ÿһ���Ƶ����õĲ���ʽ������ļ� 
	fstream fou("producer.txt", ios::out);
	if(!fou.is_open()){
		cout<<"����ʽ����ļ��򿪳���"<<endl;
		exit(0);
	}
	
	waiting_input.push_back("@");  // ������������������� 
	
	analysis_stack.push_back("@");  // ����ջ����������� 
	
	analysis_stack.push_back("program");  // ����ջ���뿪ʼ����
	
	syntax_tree = new Tree("program");  // �﷨���������ڵ� 
	node_stack.push_back(syntax_tree->root); // ���ڵ���ջ 
	
	while(analysis_stack.size() > 0){  // ջ�ǿ� 
		string input_string = waiting_input.front();  // ��ǰ��������ָ��ĵ�һ���ʷ� 
		string stack_top = analysis_stack.back();  // ����ջ��ջ�� 
		
		if(stack_top == input_string){
			
			if(stack_top == "ID" || stack_top == "NUM"){
				//  ���Ƶ���ID��NUM�ս����������⴦�� 
				vector<Symbol>::iterator it = symbol_table.begin();
				string p = (*it).val;
				fou<<stack_top<<"->"<<p<<" "<<endl;
				symbol_table.erase(symbol_table.begin());
				
				// ������Ҷ�ڵ� 
				Node* now_root = node_stack.back();
				Node* new_point1 = new Node();
				new_point1->val = p;
				syntax_tree->insert(new_point1, now_root);
				node_stack.pop_back();
			}
			
			analysis_stack.pop_back();  // ջ����ջ 
			waiting_input.erase(waiting_input.begin());  // ɾ����ǰָ����ս�� 	
		
		}else if(!isNonterminal(stack_top)){  // ���ջ��Ϊ�ս�����򱨴� 
//			cout<<"now_stacktop:  "<<stack_top<<"   input:  "<<input_string<<endl;
			fou<<"---------------ERROR!-----------------"<<endl;
			error();
			exit(0);
		
		}else if(predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)] != -1){  // Ԥ����������ж�Ӧ�� 
			int tag = predict_table[getNonterminalIndex(stack_top)][getTerminalIndex(input_string)];
			
			Node* now_root = node_stack.back();  // ȡ��ջ��������ʽ��ߵĽ�㣩
			node_stack.pop_back(); 
			for(int i=generates[tag].right.size()-1;i>=0;i--){
				Node* new_point0 = new Node();
				new_point0->val = generates[tag].right[i];
				syntax_tree->insert(new_point0, now_root);
				if(isNonterminal(generates[tag].right[i]) || generates[tag].right[i] == "NUM" || generates[tag].right[i] == "ID"){
					node_stack.push_back(new_point0);  // �ڵ���ջ 
				}
			}
//			delete now_root;
			
			/*����Ƶ��Ĳ���ʽ*/
			fou<<generates[tag].left<<"->";
			for(int i=0;i<generates[tag].right.size();i++){
				fou<<generates[tag].right[i]<<" ";
			}
			fou<<endl;
//			cout<<"!  "<<input_string<<"  !"<<endl;
			
			analysis_stack.pop_back();  // ����ʽ�󲿳�ջ 
			
			if(generates[tag].right[0] != "empty"){  // �Ƶ�Ϊ��Ϊ��ʱ����ʽ�Ҳ���ջ 
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
	initGenerative();  // ��ʼ������ʽ 

//	cout<<"Begin constructFirst"<<endl;
	
	for(int i=0;i<nonterminal_num;i++){  // Ϊÿ�����ս������First���� 
		constructFirst(nonterminal_table[i]);
	}
	
//	printFirst();
	
//	cout<<"Begin constructFollow"<<endl;
	
	for(int i=0;i<nonterminal_num;i++){  //Ϊÿ�����ս������Follow���� 
		if(i == 0){
			follow[0].insert("@");
		}
		constructFollow(nonterminal_table[i]);
		
	}
//	printFollow();
	
//	cout<<"Begin constructPredict"<<endl;
	constructPredict();  // ����Ԥ������� 
	
//	printPredict();
	
//	cout<<"Begin syntaxAnalyzer"<<endl;
	syntaxAnalyzer();  // �����﷨ 
	
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
