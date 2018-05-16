/*
 * MSTLGraphVisit.h
 *
 *  Minimal STL Graph Path Listeners
 *
 *  Used during graphs visit.
 */

#ifndef MSTLGPATHLISTENER_H_
#define MSTLGPATHLISTENER_H_

#include <set>

#include "MstlGraph.h"
#include "OCPTree.h"

namespace GGSXLib{

/*
 * Abstract class
 *
 * Example of stack calls for visit path A->B->C->D :
 * 		start_vertex(A)
 * 		discover_vertex(B)
 * 		discover_vertex(C)
 * 		discover_vertex(D)
 * 		finish_Vertex(D)
 * 		finish_Vertex(C)
 * 		finish_Vertex(B)
 * 		finish_Vertex(A)
 */
class MstlGPathListener {
public:
	OCPTree* 			index_tree;
	mstl_node_id_t 		start_vertex_id;
	OCPTreeNode* 		current_node;
	mstl_graph_id_t 	graph_id;

public:
	MstlGPathListener(){
		index_tree=NULL;
		start_vertex_id=-1;
		current_node=NULL;
		graph_id=-1;
	}
	MstlGPathListener(
				OCPTree* index_tree,
				mstl_graph_id_t graph_id){
		this->index_tree = index_tree;
		this->graph_id = graph_id;
		this->start_vertex_id = -1;
		this->current_node = NULL;
	}

	virtual void start_vertex(MstlGNode &n)=0;
	virtual void discover_vertex(MstlGNode &n)=0;
	virtual void finish_vertex(MstlGNode &n)=0;
};


/*
 * Concrete implementations
 */

/*
 * Used for database index.
 * Add informations (occurrences list) to each node of the tree
 */
class MstlGAllPathListener: public MstlGPathListener{
public:
	MstlGAllPathListener()
			: MstlGPathListener(){
	}
	MstlGAllPathListener(	OCPTree* index_tree,
							mstl_graph_id_t graph_id)
			: MstlGPathListener(index_tree, graph_id){
	}

	virtual void start_vertex(MstlGNode &n){
		mstl_node_label_t label = n.label;
		current_node=index_tree->root->add_child(label);
		start_vertex_id = n.id;
		current_node->gsinfos[graph_id]++;
	}
	virtual void discover_vertex(MstlGNode &n){
		mstl_node_label_t label = n.label;
		current_node=current_node->add_child(label);
		current_node->gsinfos[graph_id]++;
	}
	virtual void finish_vertex(MstlGNode &n){
		current_node=current_node->parent;
	}
};


/*
 * Used for query index.
 * Add informations (occurrences list) only on maximal path
 * Mark maximal path as special
 */
class MstlGOnePathListener: public MstlGPathListener{
protected:
	std::set<OCPTreeNode*> v_set;
	std::set<OCPTreeNode*>::iterator v_set_it;
public:

	virtual void start_vertex(MstlGNode &n){
		v_set.clear();
		mstl_node_label_t label = n.label;
		current_node=index_tree->root->add_child(label);
		start_vertex_id = n.id;
	}
	virtual void discover_vertex(MstlGNode &n){
		v_set.insert(current_node);
		mstl_node_label_t label = n.label;
		current_node=current_node->add_child(label);
	}
	virtual void finish_vertex(MstlGNode &n){
		v_set_it=v_set.find(current_node);
		if(v_set_it==v_set.end()){
			current_node->is_special=true;
			current_node->gsinfos[graph_id]++;
		}
		else
			v_set.erase(v_set_it);
		current_node=current_node->parent;
	}
};

}

#endif /* MSTLGPATHLISTENER_H_ */
