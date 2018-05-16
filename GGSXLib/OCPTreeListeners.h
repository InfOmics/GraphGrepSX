/*
 * OCPTreeListeners.h
 *
 *  Listeners of OCPTree operations.
 */

#ifndef OCPTREELISTENERS_H_
#define OCPTREELISTENERS_H_

#include <set>
#include <map>
#include "OCPTreeNode.h"

namespace GGSXLib{

/*
 * Abstract classes
 */

class OCPTreeMatchingListener{
public:
	/*
	 * retun true if there is yet a valid candidates set (set size > 0)
	 *       false otherwise
	 */
	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b)=0;

	/*
	 * A path of the query's tree was not fount in the global index.
	 *
	 * retun true if there is yet a valid candidates set
	 *       false otherwise
	 */
	virtual bool unmatched_node(OCPTreeNode &a) =0;
};


/*
 * Default implementation
 */

typedef std::set<mstl_graph_id_t> filtering_graph_set_t;

class DefaultOCPTMatchingListener : public OCPTreeMatchingListener{
public:
	filtering_graph_set_t& _graphs;
	bool first;

	DefaultOCPTMatchingListener(filtering_graph_set_t &graphs)
			: _graphs(graphs){
		first = true;
	}

	/*
	 * Fill candidates set
	 */
	bool prune_rule_1(	OCPTreeNode &a,
						OCPTreeNode &b){
		size_t pre_size= _graphs.size();
		size_t occ = a.gsinfos[0];
		for(OCPTNGraphsInfos::iterator IT = b.gsinfos.begin(); IT!=b.gsinfos.end(); IT++){
			if(IT->second >= occ)
				_graphs.insert(IT->first);
		}
		return pre_size!=_graphs.size();
	}

	/*
	 * Use filtering rule based on occurrences list
	 */
	bool prune_rule_1(	OCPTreeNode &a,
						OCPTreeNode &b,
						filtering_graph_set_t::iterator& fgs_IT){
		OCPTNGraphsInfos::iterator gis_IT = b.gsinfos.find(*fgs_IT);
		if(gis_IT==b.gsinfos.end()){
			return false;
		}
		else{
			if(gis_IT->second < a.gsinfos[0]){
				return false;
			}
		}
		return true;
	}

	/*
	 * At first matched tree's node it fills the candidates set by occurrences list of the current node
	 * else it applies the prune rule.
	 */
	virtual bool matched_nodes(OCPTreeNode &a, OCPTreeNode &b){
		if(first){
			prune_rule_1(a,b);
			first = false;
		}
		else{
			filtering_graph_set_t::iterator fgs_IT = _graphs.begin();
			bool erase = false;
			while(fgs_IT!=_graphs.end()){

				erase = !prune_rule_1(a,b,fgs_IT);
				if(erase){
					_graphs.erase(fgs_IT++);
				}
				else{
					fgs_IT++;
				}
			}
		}
		if(_graphs.size()==0)
			return false;
		return true;
	}

	/*
	 * Unfount path, clear candidates set
	 */
	virtual bool unmatched_node(OCPTreeNode &a){
		_graphs.clear();
		return false;
	}
};

}

#endif /* OCPTREELISTENERS_H_ */
