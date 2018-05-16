#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#include "ArgsParser.h"
#include "GGSXLib.h"

#include "timer.h"

using namespace GGSXLib;

void usage(char* cmdname);

void build(const TypedOptions& topts);
void find(const TypedOptions& topts);
void find_multi(const TypedOptions& topts);
void find_dir(const TypedOptions& topts);

int main(int argc, char* argv[]){

	if(argc < 3){
		usage(argv[0]);
		exit(1);
	}

	std::string cmd = argv[1];

	TypedOptions topts;
	std::map<std::string, std::string> opts;
	std::string opt;

	opts_add(opts, "db_file", argv[2]);

	if(cmd == "-b"){
		if(! parse_build_argv(argc, argv, 3, opts)){
			usage(argv[0]);
			exit(1);
		}
		topts.parse(opts);
		if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT)
			print_opts(opts);

		build(topts);
	}
	else if(cmd == "-f"){
		if(! parse_find_argv(argc, argv, 3, opts)){
			usage(argv[0]);
			exit(1);
		}
		topts.parse(opts);
		if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT)
			print_opts(opts);

		switch(topts.query_type){
			case TypedOptions::QUERY_TYPE_SINGLE:
				find(topts);
			break;
			case TypedOptions::QUERY_TYPE_MULTI:
				find_multi(topts);
			break;
			case TypedOptions::QUERY_TYPE_DIR:
				find_dir(topts);
			break;
		}
	}
	else{
		usage(argv[0]);
		exit(1);
	}

	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT)
		std::cout<<std::endl;

	exit(0);
};

void usage(char* cmdname){
	std::cout<<"\n\t=========================\n";
	std::cout<<"\t GraphGrepSX version 3.3\n";
	std::cout<<"\t=========================\n\n";

	std::cout<<"Usage:"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"BUILD"<<std::endl;
	std::cout<<cmdname<<" -b db_file [OPTIONS]\t\t build database index"<<std::endl;
	std::cout<<"\tOptions:"<<std::endl;
	std::cout<<"\t\t --verbose : print human readable details"<<std::endl;
	std::cout<<"\t\t --strict : print csv readable details"<<std::endl;
	std::cout<<"\t\t --full-verbose : print human readable extra details"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\t\t --lp LP : set max DFS depth to LP"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\tDefault options: --verbose --lp 4"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\tOutput format for strict build mode (separated by tab character) : \n";
	std::cout<<"\t\t #\n";
	std::cout<<"\t\t DB File : input database file\n";
	std::cout<<"\t\t Init Time : data structures initialization time\n";
	std::cout<<"\t\t Build Time : time to build database's index, includes time to load graphs from file\n";
	std::cout<<"\t\t Pure Build Time : pure time to build database's index, without time to load graphs\n";
	std::cout<<"\t\t Save Time : time to save index on file\n";
	std::cout<<"\t\t Total time : total process time\n";
	std::cout<<std::endl;
	std::cout<<"FIND"<<std::endl;
	std::cout<<cmdname<<" -f db_file  query_file [OPTIONS]\t\t\t\t single query"<<std::endl;
	std::cout<<cmdname<<" -f db_file  --multi  queries_file [OPTIONS]\t\t multiple queries within the same file"<<std::endl;
	std::cout<<cmdname<<" -f db_file  --dir  queries_directory [OPTIONS]\t\t multiple queries within the same directory"<<std::endl;
	std::cout<<"\tOptions:"<<std::endl;
	std::cout<<"\t\t --verbose : print human readable details"<<std::endl;
	std::cout<<"\t\t --strict : print csv readable details"<<std::endl;
	std::cout<<"\t\t --full-verbose : print human readable extra details"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\t\t --lp LP : set max DFS depth to LP"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\t\t --all-matches : search for all matches"<<std::endl;
	std::cout<<"\t\t --one-match : stop at first match found for each graph of the database"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\t\t --no-match-output : don't print found matches details"<<std::endl;
	std::cout<<"\t\t --screen-match-output : print found matches details on screen"<<std::endl;
	std::cout<<"\t\t --file-match-output output_file: print found matches details on file"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\tDefault options: --verbose --lp 4 --all-matches --file-match-output matches"<<std::endl;
	std::cout<<std::endl;
	std::cout<<"\tOutput format for strict mode on single query (separated by tab character) :\n";
	std::cout<<"\t\t #\n";
	std::cout<<"\t\t DB File : input database file\n";
	std::cout<<"\t\t Query File : input query file\n";
	std::cout<<"\t\t DB Load Time : time to load database index from file\n";
	std::cout<<"\t\t DB Structs Load Time : time to load database graphs structs\n";
	std::cout<<"\t\t Query Build Time : time to build query index\n";
	std::cout<<"\t\t Filtering Time : filtering time\n";
	std::cout<<"\t\t #Candidates : number of candidate graphs\n";
	std::cout<<"\t\t Matching Time : time to match query with candidate graphs,\n";
	std::cout<<"\t\t Pure Matching Time : pure time to match query with candidate graphs\n";
	std::cout<<"\t\t #Matches : number of query occurrences found in the database\n";
	std::cout<<"\t\t Total Time : total process time\n";
	std::cout<<"\tOutput format for --multi query mode (separated by tab character) :\n";
	std::cout<<"\t\t #\n";
	std::cout<<"\t\t DB File : input database file\n";
	std::cout<<"\t\t Query File : input queries file\n";
	std::cout<<"\t\t Query ID : input query ID\n";
	std::cout<<"\t\t DB Load Time : time to load database index from file\n";
	std::cout<<"\t\t DB Structs Load Time : time to load database graphs structs\n";
	std::cout<<"\t\t Query Build Time : time to build query index\n";
	std::cout<<"\t\t Filtering Time : filtering time\n";
	std::cout<<"\t\t #Candidates : number of candidate graphs\n";
	std::cout<<"\t\t Matching Time : time to match query with candidate graphs\n";
	std::cout<<"\t\t Pure Matching Time : pure time to match query with candidate graphs\n";
	std::cout<<"\t\t #Matches : number of query occurrences found in the database\n";
	std::cout<<"\t\t Total Time : total process time for current query, does not include time to load database index\n";
//	std::cout<<"\t\t Total Time + DB Load Time: total process time for current query, includes time to load database index\n";
	std::cout<<std::endl;
	std::cout<<"\tOutput format for --dir query mode (separated by tab character) :\n";
	std::cout<<"\t\t #\n";
	std::cout<<"\t\t DB File : input database file\n";
	std::cout<<"\t\t Queries Folder : input queries folder\n";
	std::cout<<"\t\t Query File Name : input query file name\n";
	std::cout<<"\t\t Query ID : input query ID\n";
	std::cout<<"\t\t DB Load Time : time to load database index from file\n";
	std::cout<<"\t\t DB Structs Load Time : time to load database graphs structs\n";
	std::cout<<"\t\t Query Build Time : time to build query index\n";
	std::cout<<"\t\t Filtering Time : filtering time\n";
	std::cout<<"\t\t #Candidates : number of candidate graphs\n";
	std::cout<<"\t\t Matching Time : time to match query with candidate graphs\n";
	std::cout<<"\t\t Pure Matching Time : pure time to match query with candidate graphs\n";
	std::cout<<"\t\t #Matches : number of query occurrences found in the database\n";
	std::cout<<"\t\t Total Time : total process time for current query, does not include time to load database index\n";
//	std::cout<<"\t\t Total Time + DB Load Time: total process time for current query, includes time to load database index\n";
	std::cout<<std::endl;
	std::cout<<"\tOutput format of found matches : \n";
	std::cout<<"\t\t QueryID : Graph ID : { (query node id, graph node id), ... }\n";
	std::cout<<std::endl;
};

/* ============================================================
 * Build database index and save it on file.
 * ============================================================
 */
void build(const TypedOptions& topts){

	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Building database index...\n";
		std::cout<<"DB file : "<<topts.db_file<<"\n";
	}

double init_time, build_time, pure_build_time, save_time, total_time; //timers
TIMEHANDLE start=start_time();

	/*
	 * Creating data structures
	 */
TIMEHANDLE start_p=start_time();
	std::ifstream is;
	is.open(topts.db_file.c_str(), std::ios::in);
	if(!is.is_open() || is.bad()){
		std::cout<<"Error on opening input file : "<<topts.db_file<<"\n";
		exit(1);
	}
	LabelMap labelMap;
	GraphReader_gff greader(labelMap,is);
	OCPTree index;
init_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT)
		std::cout<<"init time : "<<init_time<<" sec.\n";

	/*
	 * Building index ( label map + suffix tree )
	 */
start_p=start_time();
	MstlGraphVisitor gvisitor(new MstlGAllPathListener());
	BuildManager bman(index, greader, gvisitor, topts.lp, (topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE ));
	bman.run();
	is.close();
pure_build_time = bman._pure_build_time;
build_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"build time : "<<build_time<<" sec.\n";
		std::cout<<"pure build time : "<<pure_build_time<<" sec.\n";
	}

	/*
	 * Saving index on file [db_file].index.ggsx
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Saving index on file : "<<topts.db_file<<".index.ggsx ...\n";
	}
start_p=start_time();
	GGSXIndex ggsx_index(labelMap, index);
	std::ofstream os;
	os.open((topts.db_file+".index.ggsx").c_str(), std::ios::out);
	if(!os.is_open() || os.bad()){
		std::cout<<"Error on opening output file : "<<topts.db_file<<".index.ggxs \n";
		exit(1);
	}
	os<<ggsx_index;
	os.flush();
	os.close();
save_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT)
		std::cout<<"save time : "<<save_time<<" sec.\n";

total_time = end_time(start);

	/*
	 * Final prints
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"All Done!\n";
		std::cout<<"total time : "<<total_time<<" sec.\n";
	}

//	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
//		std::cout<<std::endl;
//		std::cout << "#\tDB_File\tInit_Time\tBuild_Time\tPure_Build_Time\tSave_Time\tTotal_Time\n";
//	}
	if(topts.verbose == TypedOptions::VERBOSE_TYPE_STRICT)
		std::cout<<"#\t"<<topts.db_file<<"\t"<<init_time<<"\t"<<build_time<<"\t"<<pure_build_time<<"\t"<<save_time<<"\t"<<total_time<<"\n";
};

/* ============================================================
 * Find one query.
 * ============================================================
 */
void find(const TypedOptions& topts){
double load_db_time, load_dbfile_time, query_build_time, filtering_time, match_time, total_time;
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Running query...\n";
		std::cout<<"DB file : "<<topts.db_file<<"\n";
		std::cout<<"Query file : "<<topts.query_file<<"\n";
	}

TIMEHANDLE start=start_time();

	/*
	 * Load database index
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading database index...\n";
	}
TIMEHANDLE start_p=start_time();
	LabelMap labelMap; OCPTree index_tree;
	GGSXIndex index(labelMap, index_tree);
	load_db_index(index, topts.db_file);
load_db_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db load time : "<<load_db_time<<" sec.\n";
	}

	/*
	 * Build query index, using database label map
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Building query index...\n";
	}
start_p=start_time();
	std::ifstream is;
	is.open(topts.query_file.c_str(), std::ios::in);
	if(!is.is_open() || is.bad()){
		std::cout<<"Error on opening input file : "<<topts.query_file<<"\n";
		exit(1);
	}
	OCPTree query;
	GraphReader_gff greader(index._labelMap,is);
	MstlGraph* query_graph = new MstlGraph(0);
	bool qreaded = build_query_sxtree(query, greader, topts.lp, (topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE), query_graph);
	is.close();
	if(!qreaded){
		std::cout<<"Error on read query!\n";
		exit(1);
	}


	is.open(topts.query_file.c_str(), std::ios::in);
	if(!is.is_open() || is.bad()){
		std::cout<<"Error on opening input file : "<<topts.query_file<<"\n";
		exit(1);
	}
	VF2GraphReader_gff vf2greader(index._labelMap,is);
	GGSXVFLib::MstlARGraph* vf2_query_graph = vf2greader.readGraph();
	is.close();
	if(vf2_query_graph==NULL){
		std::cout<<"Error on read query for VF2!\n";
		exit(1);
	}
	vf2_query_graph->SetNodeComparator(new GGSXVFLib::VIntegerComparator());

query_build_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"query build time : "<<query_build_time<<" sec.\n";
	}

	/*
	 * Filtering
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Filtering database graphs...\n";
	}
start_p=start_time();
	filtering_graph_set_t fgset;
	filter_db(query, index._ocpTree, fgset);
filtering_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"filtering time : "<<filtering_time<<" sec.\n";
		std::cout<<"Number of candidate graphs : "<<fgset.size()<<"\n";
		if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE){
			print_fgset(fgset);
		}
	}

	/*
	 * Loading DB structures
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading graphs databases...\n";
	}
	start_p=start_time();
	std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph*> graphsdb;
	load_graphsdb(topts.db_file, index._labelMap, fgset, graphsdb, topts);
	load_dbfile_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db file load time : "<<load_dbfile_time<<" sec.\n";
		std::cout<<"number of loaded graphs : "<<graphsdb.size()<<"\n";
	}

	/*
	 * Exact matching
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Exact matching candidate graphs...\n";
	}


start_p=start_time();
	int mcount=0;
	double pure_match_time=0;
	match_query(0, vf2_query_graph, graphsdb, fgset, topts, &mcount, &pure_match_time);
match_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"matching time : "<<match_time<<" sec.\n";
		std::cout<<"pure matching time : "<<pure_match_time<<" sec.\n";
		std::cout<<"total match count : "<<mcount<<"\n";
	}

total_time = end_time(start);

	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"All Done!\n";
		std::cout<<"total time : "<<total_time<<" sec.\n";

		std::cout<<std::endl;
//		std::cout<<"#\tDB_File\tQuery_File\tDB_Load_Time\tQuery_Build_Time\tFiltering_Time\t#Cadidates\tMatching_Time\tPure_Matching_Time\t#Matches\tTotal_Time\n";
	}

	if(topts.verbose == TypedOptions::VERBOSE_TYPE_STRICT)
		std::cout<<"#\t"<<topts.db_file<<"\t"<<topts.query_file<<"\t"<<load_db_time<<"\t"<<load_dbfile_time<<"\t"<<query_build_time<<"\t"<<filtering_time<<"\t"<<fgset.size()<<"\t"<<match_time<<"\t"<<pure_match_time<<"\t"<<mcount<<"\t"<<total_time<<"\n";
};

/* ============================================================
 * Find multiple queries within the same file
 * ============================================================
 */
void find_multi(const TypedOptions& topts){
double load_db_time, load_dbfile_time, query_build_time, filtering_time, match_time, total_time;
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Running queries...\n";
		std::cout<<"DB file : "<<topts.db_file<<"\n";
		std::cout<<"Queries file : "<<topts.query_file<<"\n";
	}

TIMEHANDLE start_t=start_time();

	/*
	 * Load database index
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading database index...\n";
	}
TIMEHANDLE start_p=start_time();
	LabelMap labelMap; OCPTree index_tree;
	GGSXIndex index(labelMap, index_tree);
	load_db_index(index, topts.db_file);
load_db_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db load time : "<<load_db_time<<" sec.\n";
	}

	std::ifstream is;
	is.open(topts.query_file.c_str(), std::ios::in);
	if(!is.is_open() || is.bad()){
		std::cout<<"Error on opening input file : "<<topts.query_file<<"\n";
		exit(1);
	}

	std::ifstream vf2is;
	vf2is.open(topts.query_file.c_str(), std::ios::in);
	if(!vf2is.is_open() || vf2is.bad()){
		std::cout<<"Error on opening input file : "<<topts.query_file<<"\n";
		exit(1);
	}


	std::ofstream fos;
	if(topts.moutput == TypedOptions::MOUTPUT_TYPE_FILE){
		fos.open(topts.moutput_file.c_str(), std::ios::out);
		if(!fos.is_open() || fos.bad()){
			std::cout<<"Error on opening output file : "<< topts.moutput_file <<" \n";
			exit(1);
		}
	}


	/*
	 * Loading DB structures
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading graphs databases...\n";
	}
	start_p=start_time();
	std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph*> graphsdb;
	load_graphsdb(topts.db_file, index._labelMap, graphsdb, topts);
	load_dbfile_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db file load time : "<<load_dbfile_time<<" sec.\n";
	}

	/*
	 * Matching queries
	 */


	int query_number = 0;
	bool next = true;
	while(is.is_open() && !is.eof() && next){
		if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE)
			std::cout<<"Trying Query : "<<query_number<<" ...\n";
		TIMEHANDLE start=start_time();

		/*
		 * Build query index
		 */
		start_p=start_time();
			OCPTree query;
			GraphReader_gff greader(index._labelMap,is);
			MstlGraph* query_graph = new MstlGraph(query_number);
			next = build_query_sxtree(query, greader, topts.lp, (topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE), query_graph);

			VF2GraphReader_gff vf2greader(index._labelMap,vf2is);
			GGSXVFLib::MstlARGraph* vf2_query_graph = vf2greader.readGraph();
		query_build_time = end_time(start_p);

		if(next){
			vf2_query_graph->SetNodeComparator(new GGSXVFLib::VIntegerComparator());
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<"________________________________________________\n\n";
				std::cout<<"Query number "<<query_number<<"\n";
				std::cout<<"Name : "<<query_graph->name<<"\n";
				std::cout<<"query build time : "<<query_build_time<<" sec.\n";
			}

			/*
			 * Filtering
			 */
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<std::endl;
				std::cout<<"Filtering database graphs...\n";
			}
			start_p=start_time();
				filtering_graph_set_t fgset;
				filter_db(query, index._ocpTree, fgset);
			filtering_time = end_time(start_p);
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<"filtering time : "<<filtering_time<<" sec.\n";
				std::cout<<"Number of candidate graphs : "<<fgset.size()<<"\n";
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE){
					print_fgset(fgset);
				}
			}

			/*
			 * Exact matching
			 */
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<std::endl;
				std::cout<<"Exact matching candidate graphs...\n";
			}
//				std::ifstream dbis;
//				dbis.open(topts.db_file.c_str(), std::ios::in);
//				GraphReader_gff dbgreader(index._labelMap, dbis);

			start_p=start_time();
				int mcount=0;
				double pure_match_time=0;

			switch(topts.moutput){
				case TypedOptions::MOUTPUT_TYPE_NO:
					match_query(query_number, vf2_query_graph, graphsdb, std::cout, fgset, topts, &mcount, &pure_match_time);
					break;
				case TypedOptions::MOUTPUT_TYPE_SCREEN:
					match_query(query_number, vf2_query_graph, graphsdb, std::cout, fgset, topts, &mcount, &pure_match_time);
					break;
				case TypedOptions::MOUTPUT_TYPE_FILE:
					match_query(query_number, vf2_query_graph, graphsdb, fos, fgset, topts, &mcount, &pure_match_time);
					break;
			}

			match_time = end_time(start_p);
//				dbis.close();
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<"matching time : "<<match_time<<" sec.\n";
				std::cout<<"pure matching time : "<<pure_match_time<<" sec.\n";
				std::cout<<"total match count : "<<mcount<<"\n";
			}

			total_time = end_time(start);
			if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
				std::cout<<std::endl;
				std::cout<<"Query Done!\n";
				std::cout<<"total query time : "<<total_time<<" sec.\n";
//				std::cout<<"total query time + load DB time: "<<total_time + load_db_time<<" sec.\n";
				std::cout<<std::endl;
				//std::cout<<"#\tDB_File\tQueries_File\tQuery_ID\tDB_Load_Time\tQuery_Build_Time\tFiltering_Time\t#Cadidates\tMatching_Time\tPure_Matching_Time\t#Matches\tTotal_Time\ttotal_Time+Load_DB_Time\n";
			}
			if(topts.verbose == TypedOptions::VERBOSE_TYPE_STRICT)
				std::cout<<"#\t"<<topts.db_file<<"\t"<<topts.query_file<<"\t"<<query_number<<"\t"<<load_db_time<<"\t"<<load_dbfile_time<<"\t"<<query_build_time<<"\t"<<filtering_time<<"\t"<<fgset.size()<<"\t"<<match_time<<"\t"<<pure_match_time<<"\t"<<mcount<<"\t"<<total_time<<"\n";

			delete query_graph;
		}
		query_number++;
	}

	if(topts.moutput == TypedOptions::MOUTPUT_TYPE_FILE){
		fos.flush();
		fos.close();
	}

	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"________________________________________________\n\n";
		std::cout<<"All Queries Done!\n";
		std::cout<<"Total queries time : "<<end_time(start_t)<<" sec.\n";
	}

	is.close();
	vf2is.close();
};

/* ============================================================
 * Find multiple queries within the same folder
 * ============================================================
 */
bool is_regular_file(const char* file){
	struct stat st_buf;
	if (stat(file, &st_buf) == 0) {
		if (S_ISREG (st_buf.st_mode)) {
			return true;
		}
	}
	return false;
}
void get_dir_files(std::string dir, std::set<std::string>& files){
	DIR *dp;
	struct dirent *dirp;
	if((dp = opendir(dir.c_str())) == NULL) {
		std::cout<<"Error on opening input folder : "<<dir<<"\n";
		exit(1);
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.insert(std::string(dirp->d_name));
	}
	closedir(dp);
}

void find_dir(const TypedOptions& topts){
double load_db_time, load_dbfile_time, query_build_time, filtering_time, match_time, total_time;
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Running queries...\n";
		std::cout<<"DB file : "<<topts.db_file<<"\n";
		std::cout<<"Queries folder : "<<topts.query_file<<"\n";
	}

TIMEHANDLE start_t=start_time();

	/*
	 * Load database index
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading database index...\n";
	}
TIMEHANDLE start_p=start_time();
	LabelMap labelMap; OCPTree index_tree;
	GGSXIndex index(labelMap, index_tree);
	load_db_index(index, topts.db_file);
load_db_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db load time : "<<load_db_time<<" sec.\n";
	}

	std::string path = topts.query_file;
	if(topts.query_file[topts.query_file.length()-1]  != '/')//WHY;
		path = topts.query_file + "/";

	std::ofstream fos;
	if(topts.moutput == TypedOptions::MOUTPUT_TYPE_FILE){
		fos.open(topts.moutput_file.c_str(), std::ios::out);
		if(!fos.is_open() || fos.bad()){
			std::cout<<"Error on opening output file : "<< topts.moutput_file <<" \n";
			exit(1);
		}
	}

	/*
	 * Loading DB structures
	 */
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"Loading graphs databases...\n";
	}
	start_p=start_time();
	std::map<mstl_graph_id_t, GGSXVFLib::MstlARGraph*> graphsdb;
	load_graphsdb(topts.db_file, index._labelMap, graphsdb, topts);
	load_dbfile_time = end_time(start_p);
	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<"db file load time : "<<load_dbfile_time<<" sec.\n";
	}

	/*
	 * Matching queries
	 */

	std::set<std::string> files;
	get_dir_files(topts.query_file, files);
	int query_number = 0;
	for(std::set<std::string>::iterator f_IT = files.begin(); f_IT!=files.end(); f_IT++){
		std::string query_file = path + (*f_IT);
		if(is_regular_file(query_file.c_str())){

			if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE)
				std::cout<<"Trying Query : "<<query_number<<" : "<<query_file<<" ...\n";
			TIMEHANDLE start=start_time();

			/*
			 * Build query index
			 */
			std::ifstream is;
			is.open(query_file.c_str(), std::ios::in);
			if(!is.is_open() || is.bad()){
				std::cout<<"Error on opening input file : "<<query_file<<"\n";
				exit(1);
			}

			std::ifstream vf2is;
			vf2is.open(query_file.c_str(), std::ios::in);
			if(!vf2is.is_open() || vf2is.bad()){
				std::cout<<"Error on opening input file : "<<query_file<<"\n";
				exit(1);
			}

			start_p=start_time();
				OCPTree query;
				GraphReader_gff greader(index._labelMap,is);
				MstlGraph* query_graph = new MstlGraph(query_number);
				bool next = build_query_sxtree(query, greader, topts.lp, (topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE), query_graph);

				VF2GraphReader_gff vf2greader(index._labelMap,vf2is);
				GGSXVFLib::MstlARGraph* vf2_query_graph = vf2greader.readGraph();
//				vf2_query_graph->SetNodeComparator(new GGSXVFLib::VIntegerComparator());
			query_build_time = end_time(start_p);

			if(next){
				vf2_query_graph->SetNodeComparator(new GGSXVFLib::VIntegerComparator());
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<"________________________________________________\n\n";
					std::cout<<"Query File : "<<query_file<<"\n";
					std::cout<<"Query ID : "<<query_number<<"\n";
					std::cout<<"Name : "<<query_graph->name<<"\n";
					std::cout<<"query build time : "<<query_build_time<<" sec.\n";
				}

				/*
				 * Filtering
				 */
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<std::endl;
					std::cout<<"Filtering database graphs...\n";
				}
				start_p=start_time();
					filtering_graph_set_t fgset;
					filter_db(query, index._ocpTree, fgset);
				filtering_time = end_time(start_p);
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<"filtering time : "<<filtering_time<<" sec.\n";
					std::cout<<"Number of candidate graphs : "<<fgset.size()<<"\n";
					if(topts.verbose > TypedOptions::VERBOSE_TYPE_VERBOSE){
						print_fgset(fgset);
					}
				}

				/*
				 * Exact matching
				 */
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<std::endl;
					std::cout<<"Exact matching candidate graphs...\n";
				}
//					std::ifstream dbis;
//					dbis.open(topts.db_file.c_str(), std::ios::in);
//					GraphReader_gff dbgreader(index._labelMap, dbis);

				start_p=start_time();
					int mcount=0;
					double pure_match_time=0;

					switch(topts.moutput){
						case TypedOptions::MOUTPUT_TYPE_NO:
							match_query(query_number, vf2_query_graph, graphsdb, std::cout, fgset, topts, &mcount, &pure_match_time);
							break;
						case TypedOptions::MOUTPUT_TYPE_SCREEN:
							match_query(query_number, vf2_query_graph, graphsdb, std::cout, fgset, topts, &mcount, &pure_match_time);
							break;
						case TypedOptions::MOUTPUT_TYPE_FILE:
							match_query(query_number, vf2_query_graph, graphsdb, fos, fgset, topts, &mcount, &pure_match_time);
							break;
					}

				match_time = end_time(start_p);
//					dbis.close();
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<"matching time : "<<match_time<<" sec.\n";
					std::cout<<"pure matching time : "<<pure_match_time<<" sec.\n";
					std::cout<<"total match count : "<<mcount<<"\n";
				}

				total_time = end_time(start);
				if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
					std::cout<<std::endl;
					std::cout<<"Query Done!\n";
					std::cout<<"total query time : "<<total_time<<" sec.\n";
//					std::cout<<"total query time + load DB time: "<<total_time + load_db_time<<" sec.\n";
					std::cout<<std::endl;
					//std::cout<<"#\tDB_File\tQueries_File\tQuery_ID\tDB_Load_Time\tQuery_Build_Time\tFiltering_Time\t#Cadidates\tMatching_Time\tPure_Matching_Time\t#Matches\tTotal_Time\ttotal_Time+Load_DB_Time\n";
				}
				if(topts.verbose == TypedOptions::VERBOSE_TYPE_STRICT)
					std::cout<<"#\t"<<topts.db_file<<"\t"<<topts.query_file<<"\t"<<*f_IT<<"\t"<<query_number<<"\t"<<load_db_time<<"\t"<<load_dbfile_time<<"\t"<<query_build_time<<"\t"<<filtering_time<<"\t"<<fgset.size()<<"\t"<<match_time<<"\t"<<pure_match_time<<"\t"<<mcount<<"\t"<<total_time<<"\n";

				query_number++;
			}

			is.close();
			vf2is.close();
		}
	}

	if(topts.moutput == TypedOptions::MOUTPUT_TYPE_FILE){
		fos.flush();
		fos.close();
	}

	if(topts.verbose > TypedOptions::VERBOSE_TYPE_STRICT){
		std::cout<<std::endl;
		std::cout<<"________________________________________________\n\n";
		std::cout<<"All Queries Done!\n";
		std::cout<<"Total queries time : "<<end_time(start_t)<<" sec.\n";
	}


};

