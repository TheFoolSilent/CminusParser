#include "Tree.h"
using namespace std;

//void init(string val, Tree* test_tree){
//	Node* new_point = new Node();
//	new_point->val = val;
//	test_tree->insert(new_point, test_tree->root);
//}

int main(){
	Tree* test_tree = new Tree("CHN");
	Node* new_point0 = new Node();
	new_point0->val = "ALIBABA";
	test_tree->insert(new_point0, test_tree->root);
	
	Node* new_point1 = new Node();
	new_point1->val = "TENCENT";
	test_tree->insert(new_point1, test_tree->root);
	
	
	Node* new_point2 = new Node();
	new_point2->val = "BAIDU";
	test_tree->insert(new_point2, new_point0);
	
	Node* new_point3 = new Node();
	new_point3->val = "BYTEDANCE";
	test_tree->insert(new_point3, new_point1);
	
	cout<<"CNH"<<endl;
	test_tree->printTree(1,test_tree->root);
}
