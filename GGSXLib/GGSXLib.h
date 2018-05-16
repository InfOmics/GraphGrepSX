/*
 * GGSXLib.h
 *
 *  Created on: Nov 1, 2011
 *      Author: bovi
 */

#ifndef GGSXLIB_H_
#define GGSXLIB_H_

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "GGSXIndex.h"
#include "BuildManager.h"
#include "MatchManager.h"

#include "VF2GraphReaders.h"

namespace GGSXLib{

void build_db_sxtree(OCPTree& index, GraphReader& greader, int lp, bool full_verbose){
	MstlGraphVisitor gvisitor(new MstlGAllPathListener());
	BuildManager bman(index, greader, gvisitor, lp, full_verbose);
	bman.run();
};

void save_db_index(GGSXIndex& index, std::string db_file){
	std::ofstream os;
	os.open((db_file+".index.ggsx").c_str(), std::ios::out);
	if(!os.is_open() || os.bad()){
		std::cout<<"Error on opening output file : "<<db_file<<".index.ggxs \n";
		exit(1);
	}
	os<<index;
	os.flush();
	os.close();
};


void load_db_index(GGSXIndex& index, std::string db_file){
	std::ifstream is;
	is.open((db_file+".index.ggsx").c_str(), std::ios::in);
	if(!is.is_open() || is.bad()){
		std::cout<<"Error on opening input file : "<<db_file<<".index.ggxs \n";
		exit(1);
	}
	is>>index;
	is.close();
};

bool build_query_sxtree(OCPTree& query, GraphReader& greader, int lp, bool full_verbose, MstlGraph* query_graph){
	MstlGraphVisitor gvisitor(new MstlGOnePathListener());
	BuildManager bman(query, greader, gvisitor, lp, full_verbose);
	return bman.run_single(query_graph);
};


void filter_db(OCPTree& query_sxtree, OCPTree& db_sxtree, filtering_graph_set_t& fgset){
	query_sxtree.match(db_sxtree, *(new DefaultOCPTMatchingListener(fgset)));
};

void print_fgset(filtering_graph_set_t& fgs){
	std::cout<<"Candidate graphs: ";
	for(filtering_graph_set_t::iterator IT = fgs.begin(); IT!=fgs.end(); IT++){
		std::cout<<(*IT)<<"; ";
	}
	std::cout<<"\n";
};


void load_graphsdb(	const std::string& db_file,
						LabelMap& labelMap,
						filtering_graph_set_t& fgset,
						std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph*>& graphsdb,
						const TypedOptions& topts){
	std::ifstream dbis;
	dbis.open(db_file.c_str(), std::ios::in);
	VF2GraphReader_gff dbgreader(labelMap, dbis);
	for(filtering_graph_set_t::iterator IT = fgset.begin(); IT!=fgset.end(); IT++){
		dbgreader.gotoGraph(*IT);
		GGSXVFLib::MstlARGraph* g;

		if((g = dbgreader.readGraph()) != NULL){
//			GGSXVFLib::MstlARGraph* vga = GGSXVFLib::convertGraf(*g);
			graphsdb.insert(std::pair<mstl_graph_id_t, GGSXVFLib::MstlARGraph*>(*IT, g));
		}
		else{
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT) std::cout<<"error on reading graph from file!\n";
		}
	}

	dbis.close();
}


void load_graphsdb(	const std::string& db_file,
						LabelMap& labelMap,
						std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph*>& graphsdb,
						const TypedOptions& topts){
	std::ifstream dbis;
	dbis.open(db_file.c_str(), std::ios::in);
	VF2GraphReader_gff dbgreader(labelMap, dbis);

	mstl_graph_id_t c_graph_id = 0;
	bool readed = false;
	TIMEHANDLE tstart;

	do{
		if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE)
			std::cout<<"Trying to read graph "<<c_graph_id<<"...";

		GGSXVFLib::MstlARGraph* g;
		readed = (g = dbgreader.readGraph()) !=NULL;

		if(readed){
			//if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE) std::cout<<" done: "<<g->name<<"\n";

			//GGSXVFLib::MstlARGraph* vga = GGSXVFLib::convertGraf(*g);
			graphsdb.insert(std::pair<mstl_graph_id_t, GGSXVFLib::MstlARGraph*>(c_graph_id, g));

			c_graph_id++;
//			g = new MstlGraph(c_graph_id);
		}
		else{
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE) std::cout<<" failed! ...may be the last?\n";
		}
	}while(readed);

	dbis.close();
}

void match_query(	int query_id,
					GGSXVFLib::MstlARGraph* query_graph,
					std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph* > dbgraphs,
					std::ostream& os,
					filtering_graph_set_t& fgset,
					const TypedOptions& topts,
					int* mcount,
					double* pure_time){

//	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT  &&  query_graph.nodes.size()==0)
//		std::cout<<"Warning: query graph has no nodes!!!\n";

	MatchManager mman(	query_id,
						query_graph,
						dbgraphs,
						os,
						(topts.moutput != TypedOptions::MOUTPUT_TYPE_NO),
						!topts.all_matches,
						( topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE )
						);
	mman.run(fgset);
	*mcount =  mman._visitor->match_count;
	*pure_time = mman._pure_match_time;
};


void match_query(	int query_id,
					GGSXVFLib::MstlARGraph* query_graph,
					std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph* > dbgraphs,
					filtering_graph_set_t& fgset,
					const TypedOptions& topts,
					int* mcount,
					double* pure_time){

	switch(topts.moutput){
		case TypedOptions::MOUTPUT_TYPE_NO:
			match_query(query_id, query_graph, dbgraphs, std::cout, fgset, topts, mcount, pure_time);
			break;
		case TypedOptions::MOUTPUT_TYPE_SCREEN:
			match_query(query_id, query_graph, dbgraphs, std::cout, fgset, topts, mcount, pure_time);
			break;
		case TypedOptions::MOUTPUT_TYPE_FILE:
			std::ofstream fos;
			fos.open(topts.moutput_file.c_str(), std::ios::out);
			if(!fos.is_open() || fos.bad()){
				std::cout<<"Error on opening output file : "<< topts.moutput_file <<" \n";
				exit(1);
			}
			match_query(query_id, query_graph, dbgraphs, fos, fgset, topts, mcount, pure_time);
			fos.flush();
			fos.close();
			break;
	}

}

}


#endif /* GGSXLIB_H_ */
