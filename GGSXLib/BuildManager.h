/*
 * BuildManager.h
 *
 * Index building: LabelMap + SuffixTree
 */

#include "OCPTree.h"
#include "GraphReaders.h"
#include "MstlGraphVisit.h"
#include "timer.h"

#ifndef BUILDMANAGER_H_
#define BUILDMANAGER_H_


namespace GGSXLib{

class BuildManager{
public:
	OCPTree& 			_tree;
	GraphReader& 		_greader;
	MstlGraphVisitor& 	_gvisitor;
	size_t 				_lp;
	mstl_graph_id_t 	_c_graph_id;
	bool				_full_verbose;
	double 				_pure_build_time;

	BuildManager(OCPTree& tree, GraphReader& greader, MstlGraphVisitor& gvisitor, size_t lp, bool full_verbose)
			: _tree(tree), _greader(greader), _gvisitor(gvisitor), _lp(lp), _full_verbose(full_verbose){
		_c_graph_id = 0;
		_gvisitor.plistener->index_tree = &_tree;
		_pure_build_time = 0;
	}

	void run(){
		MstlGraph* g = new MstlGraph(_c_graph_id);
		bool readed = false;
		TIMEHANDLE tstart;

		do{
			if(_full_verbose) std::cout<<"Trying to read graph "<<_c_graph_id<<"...";

			readed = _greader.readGraph(g);

			if(readed){
				if(_full_verbose) std::cout<<" done: "<<g->name<<"\n";

				_gvisitor.plistener->graph_id = _c_graph_id;

				tstart = start_time();
				for(mstl_nodes_list_t::iterator n_IT = g->nodes.begin(); n_IT!= g->nodes.end(); n_IT++){
					_gvisitor.run(*g, n_IT->second, _lp);
				}
				_pure_build_time += end_time(tstart);

				_c_graph_id++;
				g = new MstlGraph(_c_graph_id);
			}
			else{
				if(_full_verbose) std::cout<<" failed! ...may be the last?\n";
			}
		}while(readed);

//		while(_greader.readGraph(g)){
//			_gvisitor.plistener->graph_id = _c_graph_id;
//
//			for(mstl_nodes_list_t::iterator n_IT = g->nodes.begin(); n_IT!= g->nodes.end(); n_IT++){
//				_gvisitor.run(*g, n_IT->second, _lp);
//			}
//
//			_c_graph_id++;
//			g = new MstlGraph(_c_graph_id);
//		}
	}

	bool run_single(MstlGraph* g){
		if(_full_verbose) std::cout<<"Trying to read graph "<<_c_graph_id<<"...";

		if(_greader.readGraph(g)){
			if(_full_verbose) std::cout<<" done: "<<g->name<<"\n";

			_gvisitor.plistener->graph_id = _c_graph_id;

			TIMEHANDLE tstart = start_time();
			for(mstl_nodes_list_t::iterator n_IT = g->nodes.begin(); n_IT!= g->nodes.end(); n_IT++){
				_gvisitor.run(*g, n_IT->second, _lp);
			}
			_pure_build_time += end_time(tstart);

			_c_graph_id++;
			return true;
		}
		else{
			if(_full_verbose) std::cout<<" failed! ...may be the last?\n";
		}

		return false;
	}

};

}


#endif /* BUILDMANAGER_H_ */
