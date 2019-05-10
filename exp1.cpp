// C-词法分析器 

#include<iostream>
#include<cstring>
#include<fstream>
#include<cstdlib>
#include<vector>
using namespace std;

class MonoDevelop{
	public:
		// 保留字表 
		static char* reserve_table[6];
		
		// 操作与边界符号表 
		static char* ope_deli_table[21];
		
		// 标识符表 
		// char* iden_table[1000];
		
		vector<string> waiting_input;  // 非终结符化 
		// 预处理后的代码表
		char* code_table;
		
		int code_len;
		
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
		void Scanner(int& id, int& pos, char* token, int& count);
		
		//  主处理程序
		void Progress(const string path);
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
	code_table = new char[1010];
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
			code_table[code_len++]=str[i];
		}
	}
	// cout<<"before: "<<code_table[code_len-1]<<endl;
	
	code_table[code_len-1]='@';
	
	// cout<<"after: "<<code_table[code_len-1]<<endl;
	code_table[code_len]='\0';
	
//	for(int i=0;i<code_len;i++){
//		cout<<(int)code_table[i]<<" ";
//	}
}


void MonoDevelop::Scanner(int& id, int& pos, char* token, int& count){

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
		preSolve(ch, cnt);
		// cout<<"Prepared:\n"<<code_table<<endl;	
	}
	
	int id=-1;
	int pos=0;
	
	// cout<<"Length: "<<code_len<<endl;
	while(id!=200){
		char str_word[20];
		int word_len=0;
		Scanner(id,pos,str_word,word_len);
		if(id == 100){ // 标识符 
//			cout<<"ID, name= "<<str_word<<endl;
			waiting_input.push_back("ID");
		}else if(id >= 0 && id <= 5){ // 保留字 
//			cout<<"Reserve Word: "<<str_word<<endl;
			waiting_input.push_back(str_word);
		}else if(id == 101){ // 常数 
//			cout<<"NUM, val= "<<str_word<<endl;
			waiting_input.push_back("NUM");
		}else if(id>5 && id<27){ // 运算与边界符 
//			cout<<str_word<<endl;
			waiting_input.push_back(str_word);
		}
		// cout<<"id: "<<id<<endl;
		// system("pause");
	}
	
	fstream outfile("nonterminal.txt");
	
	if(!outfile.is_open()){
		cout<<"outfile fails to open"<<endl;
		exit(0);
	}else{
		vector<string>::iterator it;
		for(it = waiting_input.begin(); it != waiting_input.end(); it++){
			outfile<<*it<<" ";
		}
		
		outfile.close();
	}
	
}

int main(){
	MonoDevelop developer = MonoDevelop();
	developer.Progress("example.c-");
}
