/*
 * MSTLGraph.h
 *
 *  Minimal STL Graph Structures
 */

#ifndef MSTLGRAPH_H_
#define MSTLGRAPH_H_

#include <set>
#include <map>
#include <iostream>
#include <fstream>

namespace GGSXLib{

typedef int 						mstl_node_id_t;
typedef int 						mstl_node_label_t;
typedef std::set<mstl_node_id_t> 	mstl_node_id_set_t;
typedef mstl_node_id_set_t 			mstl_node_neighbors_t;

class MstlGNode {
public:
	mstl_node_id_t 			id;
	mstl_node_label_t 		label;
	mstl_node_neighbors_t 	out_neighbors;

	MstlGNode(){
		this->id = -1;
		this->label = -1;
	}
	MstlGNode(mstl_node_id_t id, mstl_node_label_t label){
		this->id = id;
		this->label = label;
	}
};


typedef int													mstl_graph_id_t;
typedef std::set<mstl_graph_id_t> 							mstl_graph_id_set_t;
typedef std::map<mstl_node_id_t, MstlGNode> 				mstl_nodes_list_t;
typedef std::map<mstl_node_label_t, mstl_node_id_set_t> 	mstl_label_nodes_map_t;

class MstlGraph {
public:
	mstl_graph_id_t id;
	mstl_nodes_list_t nodes;
	std::string name;


	MstlGraph(mstl_graph_id_t id){
		this->id  = id;
		this->name = "";
	}

	size_t size(){
		return nodes.size();
	}

	MstlGNode* insertNode(mstl_node_id_t id, mstl_node_label_t label){
		this->nodes[id] = MstlGNode(id, label);
		return &(this->nodes[id]);
	}

	MstlGNode* insertNode(mstl_node_label_t label){
		mstl_node_id_t nid = (mstl_node_id_t)this->nodes.size();
		return insertNode(nid, label);
	}

	void insertEdge(mstl_node_id_t from, mstl_node_id_t to){
		this->nodes[from].out_neighbors.insert(to);
	}

	void print(std::ostream &os){
		os<<"Graph: "<<this->id<<" :: "<<this->name<<"\n";
		for(mstl_nodes_list_t::iterator IT=this->nodes.begin(); IT!=this->nodes.end(); IT++){
			os<<IT->first<<": "<<IT->second.label<<": \n";
			os<<"out neighbors: ";
			for(mstl_node_neighbors_t::iterator ITT=IT->second.out_neighbors.begin(); ITT!=IT->second.out_neighbors.end();ITT++)
				os<<*ITT<<",";
			os<<"\n";
		}
	}
};

}

#endif /* MSTLGRAPH_H_ */
