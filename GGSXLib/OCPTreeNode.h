/*
 * OCPTreeNode.h
 *
 */

#ifndef OCPTREENODE_H_
#define OCPTREENODE_H_

#include <map>
#include <set>
#include <iostream>
#include <fstream>

#include "MstlGraph.h"

namespace GGSXLib{

/*
 * Class OCPTNGraphsInfos  used for occurrences list.
 *
 */
class OCPTNGraphsInfos : public std::map<mstl_graph_id_t, size_t>{
public:
	/*
	 * Save to file
	 */
	friend std::istream& operator>>(std::istream& is, OCPTNGraphsInfos& gi){
		size_t size = 0;
		is.read((char*)&size, sizeof(size_t));
		for(int i=0;i<size;i++){
			mstl_graph_id_t graph_id=0;
			size_t occ;
			is.read((char*)&graph_id, sizeof(mstl_graph_id_t));
			is.read((char*)&occ, sizeof(size_t));
			gi.insert(std::pair<mstl_graph_id_t, size_t>(graph_id, occ));
		}
		return is;
	}

	/*
	 * Load from file
	 */
	friend std::ostream& operator<<(std::ostream& os, OCPTNGraphsInfos& gi){
		size_t size = gi.size();
		os.write((char*)&size, sizeof(size_t));
		for(OCPTNGraphsInfos::iterator IT=gi.begin(); IT!=gi.end();IT++){
			os.write((char*)&(IT->first), sizeof(mstl_graph_id_t));
			os.write((char*)&(IT->second), sizeof(size_t));
		}
		return os;
	}

	void print(std::ostream& os){
		for(OCPTNGraphsInfos::iterator IT=this->begin(); IT!=this->end(); IT++){
			os<<IT->first<<":: occ= "<<IT->second<<"\n";
		}
	}
};


/*
 * Class OCPTreeNode
 *
 */
class OCPTreeNode{
public:
	mstl_node_label_t 	label;
	OCPTreeNode* 		parent;
	OCPTreeNode* 		first_child;
	OCPTreeNode* 		last_child;
	OCPTreeNode* 		next;
	size_t 				child_count;
	bool 				is_special;		//used only for query index
	OCPTNGraphsInfos 	gsinfos;

	OCPTreeNode(mstl_node_label_t _label, OCPTreeNode* _parent){
		this->label=_label;
		this->parent=_parent;
		this->next=NULL;
		this->first_child=NULL;
		this->last_child=NULL;
		this->child_count=0;
		this->is_special=false;
	}

	~OCPTreeNode(){
		OCPTreeNode* n = first_child;
		OCPTreeNode* nn;
		while(n != NULL){
			nn = n->next;
			delete n;
			n = nn;
		}
	}

	/*
	 * Child insertion ordered by label
	 */
	OCPTreeNode* add_child(mstl_node_label_t _label){
		if(this->first_child){
			if((this->first_child)->label > _label){
				OCPTreeNode* n=this->first_child;
				this->first_child=new OCPTreeNode(_label, this);
				(this->first_child)->next=n;
				(this->child_count)++;
				return this->first_child;
			}
			else if((this->first_child)->label == _label){
				return this->first_child;
			}

			else if((this->last_child)->label < _label){
				(this->last_child)->next=new OCPTreeNode(_label, this);
				this->last_child=(this->last_child)->next;
				(this->child_count)++;
				return this->last_child;
			}
			else if((this->last_child)->label == _label){
				return this->last_child;
			}

			else{
				OCPTreeNode* c=this->first_child;
				while(c->next!=NULL && (c->next)->label<=_label)
					c=c->next;

				if(c->label == _label){
					return c;
				}
				else{
					OCPTreeNode* n=c->next;
					c->next=new OCPTreeNode(_label, this);
					(c->next)->next=n;
					if(c->next->next==NULL)
						this->last_child=c->next;
					(this->child_count)++;
					return c->next;
				}
			}
		}
		else{
			this->first_child=new OCPTreeNode(_label, this);;
			this->last_child=this->first_child;
			(this->child_count)++;
			return this->first_child;
		}
	};

	OCPTreeNode* add_child_on_tail(mstl_node_label_t _label){
		if(this->first_child){
			if((this->last_child)->label > _label){
				return add_child(_label);
			}
			else if((this->last_child)->label < _label){
				(this->last_child)->next=new OCPTreeNode(_label, this);
				this->last_child=(this->last_child)->next;
				(this->child_count)++;
				return this->last_child;
			}
			else if((this->last_child)->label == _label){
				return this->last_child;
			}
		}
		else{
			this->first_child=new OCPTreeNode(_label, this);
			this->last_child=this->first_child;
			(this->child_count)++;
			return this->first_child;
		}
	}

	void add_child_on_tail(OCPTreeNode* c){
		c->parent = this;
		if(this->last_child){
			this->last_child->next = c;
			this->last_child = this->last_child->next;
		}
		else{
			this->first_child = c;
			this->last_child = c;
		}
	}


	OCPTreeNode* get_child_by_label(mstl_node_label_t _label){
		OCPTreeNode* n=NULL;
		if(this->first_child!=NULL){
			if(_label==this->first_child->label)
				n=this->first_child;
			else if(_label==this->last_child->label)
				n=this->last_child;
			else if((_label>this->first_child->label)&&(_label<this->last_child->label)){
				OCPTreeNode* c=this->first_child->next;
				while(c!=NULL){
					if(_label==c->label){
						n=c;
						c=NULL;
					}
					else c=c->next;
				}
			}
		}
		return n;
	};

	void print_path(){
		OCPTreeNode* n = this;
		while(n!=NULL){
			std::cout<<n->label<<".";
			n=n->parent;
		}
	}
	void print_path(std::ostream& os){
		OCPTreeNode* n = this;
		while(n!=NULL){
			os<<n->label;
			n=n->parent;
		}
	}

	/*
	 * Save to file
	 */
	friend std::istream& operator>>(std::istream& is, OCPTreeNode& n){
		is.read((char*)&(n.label), sizeof(mstl_node_label_t));
		is.read((char*)&(n.child_count), sizeof(size_t));
		is>>n.gsinfos;
		return is;
	}
	/*
	 * Load from file
	 */
	friend std::ostream& operator<<(std::ostream& os, OCPTreeNode& n){
		os.write((char*)&(n.label), sizeof(mstl_node_label_t));
		os.write((char*)&(n.child_count), sizeof(size_t));
		os<<n.gsinfos;
		return os;
	}
};

}

#endif /* OCPTREENODE_H_ */
