#include "Tree.h"
using namespace std;

Tree::Tree(string val){
	root->val = val;
}

void Tree::insert(Node* chi, Node* parent){
	parent->child.push_back(chi);
}

void Tree::printTree(int step, Node* now_point){
	vector<Node*>::iterator it;
	for(it=now_point->child.begin();it!=now_point->child.end();it++){ 
		for(int k=1;k<=step;k++){
			cout<<" ";
		}
		cout<<(*it)->val<<endl;
		printTree(step+1, (*it));
	}
}

