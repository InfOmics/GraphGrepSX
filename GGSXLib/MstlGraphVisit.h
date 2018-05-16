/*
 * MstlGraphVisit.h
 *
 *  Used for DFS visit and index construction (by listeners)
 */

#ifndef MSTLGRAPHVISIT_H_
#define MSTLGRAPHVISIT_H_

#include <stack>
#include "MstlGPathListener.h"

namespace GGSXLib{

class MstlGraphVisitor{

	class stack_item{
	public:
		MstlGNode* node;
		mstl_node_neighbors_t::iterator e_IT;
		bool out_IT;
		bool start;

		stack_item(MstlGNode* n){
			node = n;
			e_IT = n->out_neighbors.begin();
			out_IT=true;
			start = true;
		}

		bool next_neigh(std::map<mstl_node_id_t,bool> &visited){
			if(out_IT){
				if(start){
					start = false;
					while((e_IT!=node->out_neighbors.end()) && visited[*e_IT]){
						e_IT++;
					}
				}
				else{
					do{
						e_IT++;
					}while((e_IT!=node->out_neighbors.end()) && visited[*e_IT]);
				}
				if(e_IT==node->out_neighbors.end()){
					out_IT=false;
					start = true;
				}
				else{
					return true;
				}
			}
			return false;
		}
	};

public:
	MstlGPathListener* plistener;

public:
	MstlGraphVisitor(MstlGPathListener* plistener){
		this->plistener = plistener;
	}

	/*
	 * Run DFS visit for each node of the graph.
	 */
	void run(MstlGraph &g, size_t &maxdepth){
		for(mstl_nodes_list_t::iterator IT =g.nodes.begin(); IT!=g.nodes.end(); IT++)
			run(g, IT->second, maxdepth);
	}


	/*
	 * Run DFS visit starting from node n.
	 */
	void run(MstlGraph &g, MstlGNode &n, size_t &maxdepth){
		std::stack<stack_item> stack;
		std::map<mstl_node_id_t,bool> visited;

		plistener->start_vertex(n);
		visited[n.id] = true;
		stack.push(stack_item(&n));

		int depth = 1;
		stack_item* c_item;
		MstlGNode* c;
		while(!stack.empty()){
			c_item=&(stack.top());
			if(depth+1<=maxdepth && c_item->next_neigh(visited)){
				c = &(g.nodes[*(c_item->e_IT)]);
				plistener->discover_vertex(*c);
				visited[c->id] = true;
				stack.push(stack_item(c));
				depth++;
			}
			else{
				plistener->finish_vertex(*(c_item->node));
				visited[c_item->node->id] = false;
				stack.pop();
				depth--;
			}
		}
	}

};

}


#endif /* MSTLGRAPHVISIT_H_ */
