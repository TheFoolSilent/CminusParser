#ifndef __TREE_H__
#define __TREE_H__

#include<iostream>
#include<vector>
using namespace std;
struct Node{
	string val;
	vector<Node*> child;
};

class Tree{
	public:
		Node* root;
		Tree(string val);
		void insert(Node* , Node*);
		void printTree(int, Node*);
};

Tree::Tree(string val){
	root = new Node();
	root->val = val;
}

void Tree::insert(Node* chi, Node* parent){
	parent->child.push_back(chi);
}

void Tree::printTree(int step, Node* now_point){
//	cout<<"Node: "<<now_point<<endl;
	
	vector<Node*>::iterator it;
	for(it=now_point->child.begin();it!=now_point->child.end();it++){ 
		for(int k=1;k<=step;k++){
			cout<<"  ";
		}
		cout<<(*it)->val<<endl;
		printTree(step+1, (*it));
	}
}

#endif
