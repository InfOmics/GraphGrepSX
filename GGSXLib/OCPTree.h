/*
 * OCPTree.h
 *
 *  Created on: Nov 3, 2010
 */

#ifndef OCPTREE_H_
#define OCPTREE_H_

#include <iostream>
#include <fstream>
#include <map>
#include <queue>

#include "MstlGraph.h"
#include "OCPTreeNode.h"
#include "OCPTreeListeners.h"

namespace GGSXLib{

/*
 * Used for trees' match process during filtering step
 */
class OCPQueueNode{
public:
	OCPTreeNode* f_node; //first, used for query node
	OCPTreeNode* s_node; //second, used for reference node
	OCPQueueNode* next;
	OCPQueueNode(){};
	OCPQueueNode(OCPTreeNode* _f_node, OCPTreeNode* _s_node){
		this->f_node=_f_node;
		this->s_node=_s_node;
		this->next=NULL;
	};
};

/*
 * Index tree implemented by simple lists
 */
class OCPTree{
public:
	OCPTreeNode* root; //fake root

	OCPTree(){
		this->root=new OCPTreeNode(-1, NULL);
	}
	~OCPTree(){
		delete root;
	};

	/*
	 * Load from file operations
	 *
	 * TODO remove recursive way and substitute it by iterative way.
	 */

	static void load(std::istream& is, OCPTreeNode& n){
		is>>n;
		for(int i=0;i<n.child_count;i++){
			OCPTreeNode* c= new OCPTreeNode(-1,NULL);
			n.add_child_on_tail(c);
			load(is, *c);
		}
	}
	static void load(std::istream& is, OCPTree& t){
		t.root = new OCPTreeNode(-1,NULL);
		load(is, *(t.root));
	}

	friend std::istream& operator>>(std::istream& is, OCPTree& t){
			OCPTree::load(is, t);
			return is;
		}

	/*
	 * Save to file operations
	 *
	 * TODO remove recursive way and substitute it by iterative way.
	 */

	static void save(std::ostream& os, OCPTreeNode& n){
		os<<n;
		OCPTreeNode* c = n.first_child;
		while(c!=NULL){
			save(os,*c);
			c=c->next;
		}
	}
	static void save(std::ostream& os, OCPTree& t){
		if(t.root!=NULL){
			save(os, *(t.root));
		}
	}


	friend std::ostream& operator<<(std::ostream& os, OCPTree& t){
		OCPTree::save(os, t);
		return os;
	}

	/*
	 * Trees' match.
	 *
	 * This must be the query's index.
	 * g must be the database index.
	 *
	 * Implements filtering rule by OCPTreeMatchingListener
	 */
	void match(OCPTree &g, OCPTreeMatchingListener& mlistener){
		//se due nodi stanno alla stessa posizione delle due code
		//vuol dire che sono mecciati

		OCPTreeNode* nt; //current query node
		OCPTreeNode* ng; //current database node
		OCPTreeNode* ct; //child of current query node
		OCPTreeNode* cg; //child of current database node

		OCPQueueNode* current_qn;
		OCPQueueNode* tail_qn;

		OCPQueueNode* delete_queue_node;

		current_qn=new OCPQueueNode(this->root, g.root);
		tail_qn=current_qn;

		bool goon=true;
		while(current_qn!=NULL && goon){
			nt=current_qn->f_node;
			ng=current_qn->s_node;

			if(nt->is_special){
				goon = mlistener.matched_nodes(*nt,*ng);
			}
			ct=nt->first_child;
			cg=ng->first_child;
			while(ct!=NULL && goon){
				while(cg!=NULL && cg->label<ct->label)
					cg=cg->next;
				if(cg==NULL || cg->label!=ct->label){
					goon = mlistener.unmatched_node(*ct);
				}
				else{
					tail_qn->next=new OCPQueueNode(ct, cg);
					tail_qn=tail_qn->next;
				}
				ct=ct->next;
			}
			delete_queue_node = current_qn;
			current_qn=current_qn->next;
			delete delete_queue_node;
		}
	}

	/*
	 * Print tree in xml format, for debuging
	 */

	void print_xml(std::ostream& s){
		OCPTreeNode* n = this->root;
		print_xml(s,n,0);
	}
	void print_xml(std::ostream& s,OCPTreeNode* n, int level){
		if(n!=NULL){
			for(int i=0;i<level;i++)s<<"\t";
			s<<"<node label=\""<<n->label<<"\" child_count=\""<<n->child_count<<"\" isspecial=\""<<n->is_special<<"\">\n";

			for(int i=0;i<level;i++)s<<"\t";
			s<<"<OCPTNGraphsInfos>\n";
			for(OCPTNGraphsInfos::iterator s_IT = n->gsinfos.begin(); s_IT!=n->gsinfos.end(); s_IT++){
				for(int i=0;i<level+1;i++)s<<"\t";
				s<<"<element id=\""<<(s_IT->first)<<"\" path_occurences=\""<<s_IT->second<<"\" />\n";
			}
			for(int i=0;i<level;i++)s<<"\t";
			s<<"</OCPTNGraphsInfos>\n";

			OCPTreeNode* c = n->first_child;
			while(c!=NULL){
				print_xml(s,c,level+1);
				c = c->next;
			}

			for(int i=0;i<level;i++)s<<"\t";
			s<<"</node>\n";
		}
	}
};

}

#endif /* OCPTREE_H_ */
