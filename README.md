# GraphGrepSX
>GraphGrepSX is a querying system for databases of graphs. It is based on its predecessor GraphGrep.

<hr />

### Description
GraphGrepSX is a querying system for databases of graphs. It is based on its predecessor GraphGrep. Please see related papers for complete algorithmic details. The system implements efficient graph searching algorithms together with advanced filtering techniques that allow an initial approximate search. It allows users to select candidate subgraphs rather than entire graphs.
The method searches for subgraph isomorphism (monomorphism) between a query graph (pattern) and graphs inside a database of graphs (targets). <br>
The GraphGrepSX project was developed under GNU/Linux and provides:

* a Console User Interface for end users
- an extensible C++ Framework for developers to include GraphGrepSX techniques in their projects
Details about compilance and usage of the Console User Interface are showed in the following sections.
Framework API and development issues are illustrated in the section For Developers.
GraphGrepSX allows to work with node/edge labeled graphs, all other types of graphs (hypergraphs and multigraphs) are not allowed. It can be modified to reach induced subgraph isomorphism and graph isomorphism.

The main steps of the system are:
* **Database's Index Construction**:
Extract features from database graphs and build offline a database index to be used for the filtering process.
A feature is a labeled path presents in a graph. During this phase are extracted all paths of maximal length equal to lp. Moreover, for each extracted keeps track of the number of occurrences wich is present into every graph of the database. The set of the labeled path and the relative occurrences lists made the database index. GraphGrepSX stores the features within a Suffix tree building the branches of the structure according to the labels of the visited paths. Each node of the tree represent a path visited during the depth-first visit fo the features extraction process
* **Query's Index Construction**:
Extract featues from query graph and build a query index.
Unlike the preprocessing phase, are considered only the maximal paths visited during the depth-visit. A path is considered maximal if its length is equal to lp or the path has length less lp but cannot be extended, namely the depth-visit can not continue. Only the occurrences of the maximal paths are stored into the marked nodes of the index.
* **Filtering**:
Compare database index with query index. Search query features in the database index to approximately filter graphs that do not contain the query and generate a candidate list.
A matching between the Suffix trees of the global index and the query index is made to select the candidates. For each marked node of the query tree that represent a labeled path the approach exclude from the candidates set the graphs that are not in the occurrences list of the global index, namely the graph do not contains a path, or the occurrence of the graph is less than the occurrence of the query.
* **Matching**:
Search for exact query occurrences inside the candidate graphs list and output found matches using the backtracking algorithms of the VF-Library framework that provides an exhaustive subgraph matching algorithm.

<hr />

### Build
```
cd GraphGrepSX_v3.3
make clean 
make -B
```
Once you had unpacked sources file, enter in the GraphGrepSX_v3.3 folder and type make -B. The process will compile the VF-Library and the Console User Interface sources. At the end, make sure that the executable ggsx has been created.

<hr />

### Usage
Usage

To view complete usage helps run ggsx without parameters.

#### Database Index Construction
```
./ggsx -b db_file [OPTIONS]
```
Build the index of the database stored in the file db_file and saves it in a file called db_file.index.ggsx .
If you use the database for the querying step, make sure that the index db_file.index.ggsx is ever in the same folder of the database db_file .
You can use the index for multiple query instances, becasue you do not need to rebuild the index until the database had no changes.

| Attribute | Description |
|-----------------------|-------------|
|**--Verbose** |        print human readable details.| 
|**--full-verbose**| print extra details. |
|**--strict** | print only strict details in csv format, using tab character for separation.|
|**--lp LP**| set max DFS depth to LP.|

Default options are:
```
--lp 4 --verbose
```
#### Querying
```
./ggsx -f db_file query_file [OPTIONS]
```
The command searches for subgraph isomorphims between the pattern graph (contained in query_file) and the graphs inside the database db_file. If the query_file contains more than one graph, it considers only the first and ignores all others.

Before launching the query, make sure you have built the database index with the command ggsx -b and that it has produced the db_file.index.ggsx file.

If you are using the option --lp, make sure that the database index was produced with an LP greater than or equal to the LP used for the query.

The tools also allows users to run multiple queries specified inside the same file. In this way, it searches for subgraph isomorphism between each pattern graph and the database.
```
./ggsx -f db_file --multi queries_file [OPTIONS]
```
Or multiple queries contained in the same folder.
```
./ggsx -f db_file --dir queries_folder [OPTIONS]
```
| Options | Description |
|-----------------------|-------------|
|**--verbose** |        print human readable details.| 
|**--full-verbose**| print extra details. |
|**--strict** | print only strict details in csv format, using tab character for separation.|
|**--lp LP**| set max DFS depth to LP.|
|**--all-matches**| search for all matches.|
|**--one-match**| stop at first match found for each graph of the database.|
|**--screen-match-output**| print found matches details on screen|
|**--file-match-output output_file**| print found matches details on file.|

Default options are:
```
--verbose --lp 4 --all-matches --file-match-output matches
```
#### About timers
If you want test the tool performaces please take into account these considerations about printed timers both in verbose and strict mode.

For each given graph, GraphGrepSX reads it from input file, extracts features and inserts them into the index. In the case of a database of graphs, the reading phase can be very expensive and can significantly affect the build time, for this reason the tool reports two different timers. The first one, Build Time, is referred to the total time cost for read the graphs and build the index. Instead, the second one, Pure Build Time, refers only the time to extract features and build the index without considering the readings.

For the same reason, two different timers are showed for the matching phase. Infact, GraphGrepSX does not store any informations about graphs' structures inside the index file. So, when an exaustive matching, between the query and a database graph, it needs to read graph structure from the original database file and convert it to be used in the VF-Library. Then, the Matching Time refers the total cost for reading and matching, instead Pure Matching Time refers just the cost for the structures matching by the VF-Library. 
Moreover, when the exaustive algorithm finds a match, it calls the procedure that prints the match on screen or on file. So, if you are using the options --scree-match-output or --file-match-output, Pure Matching Time includes time to print matches . If you are interested in only backtracking algorithm's opreations you need to use the command option --no-match-output so that a blank prodecure will be called for prints without affect significantly the Pure Matching Time.

An important feature of the method is the compactness of the index. Infact, GraphGrepSX outperforms other method for index construction time and index loading time. The loading time is referred to the time that the tool needs to read the index structure from file. This step is executed just one time both for single or multiple query running and this can affects the total times of your tests. For this reason may be that you need to discern between index loading time and pure querying time ( filtering time + matching time).

### Input Format
Graphs are stored in text files containing one or more items.
The current input format allows the description of undirect graphs with labels on nodes.
>#[graph_name] <br>
[number of nodes] <br>
[label_of_first_node] <br>
[label_of_second_node] <br>
... <br>
[number of edges] <br>
[node id] [node id] <br>
[node id] [node id] <br>
... <br>

GraphGrepSX assigns ids to nodes following the order in wich they are written in the input file, starting from 0.
[graph_name] and labels can not contain blank characters (spaces).
Labels are case sensitive.

Graphs IDs are assigned following the order in wich they are written into the input file.

An example of input file is the following:

>#my_graph <br>
4 <br>
A <br>
B <br>
C <br>
Br <br>
5 <br>
0 1 <br>
2 1 <br>
2 3 <br>
0 3 <br>
0 2 <br>

Since GraphGrepSX does not allow multigraphs, it ignores all duplicated edges without reporting any error.
An example of database file can be found here together a file contains just one query.


### Output Format
#### Matches
Current implementation of GraphGrepSX outputs finds matches in the following format:
```
[Query ID]:[DB Graph ID]:{([query node id], [target node id]), ...}
```
In the case of multiple queries running, --multi or --dir, the query ids assignment follows the order in wich thery are executed.

An example of output, given a query having three nodes, is:
```
0:0:{(0,2),(1,3),(2,10)}
0:10:{(0,20),(1,19),(2,10)}
```
The example shows that two matches were found between the query and the graphs of the database.
In the first row, the query node with ID 0 is matched with the node with ID 2 of the database graph number 0, an so on.

<hr />

### CSV Fields
Using the --strict mode, the tool prints running details in CSV format separating fields by a tab character but it does not print any information about fileds columns names. This section show informations about the exact sequence and the semantics of these columns.
```
./ggsx -b db_file [OPTIONS]
```

| Attribute | Description|
|----------|------------|
|DB File |input database file.|
|Init Time | data structures initialization time.|
|Build Time | time to build database's index, includes time to load graphs from file.|
|Pure Build Time | pure time to build database's index, without time to load graphs from file db_file.|
|Save Time | time to save index on file db_file.index.ggsx.|
|Total time | total process time.|

```
./ggsx -f db_file query_file [OPTIONS]
```
| Attribute | Description|
|----------|------------|
|DB File | input database file.|
|Query File | input query file.|
|DB Load Time | time to load database index from file.|
|Query Build Time | time to build query index.|
|Filtering Time | filtering time.|
|#Candidates | number of candidate database graphs.|
|Matching time | time to match query with candidate graphs, includes time to load graphs structures from file.|
|Pure Matching time | pure time to match query with candidate graphs, does not include time to load graphs structures from file.|
|#Matches | number of query occurrences found in the database.|
|Total Time | total process time.|


```
./ggsx -f db_file --multi queries_file [OPTIONS]
```
| Attribute | Description|
|----------|------------|
|DB File | input database file.|
|Query File | input queries file.|
|Query ID | input query ID. Starting from 0 and following the order in wich the queries are writtern into the file.|
|DB Load Time | time to load database index from file.|
|Query Build Time | time to build query index.|
|Filtering Time | filtering time.|
|#Candidates | number of candidate database graphs.|
|Matching time | time to match query with candidate graphs.|
|Pure Matching time | pure time to match query with candidate graphs.|
|#Matches | number of query occurrences found in the database.|
|Total Time | total process time just for current query, does not include time to load database index.|
||Total process time for all queries is not reported. You can calculate it adding total times of each single query plus thie time to load the database.|

```
./ggsx -f db_file --dir query_file [OPTIONS]
```
| Attribute | Description|
|----------|------------|
|DB File | input database file.|
|Queries Folder | input queries folder.|
|Query File Name | file name of the input query.|
|Query ID | input query ID. Starting from 0 and following the order in wich queries are executed.|
|DB Load Time | time to load database index from file.|
|Query Build Time | time to build query index.|
|Filtering Time | filtering time.|
|#Candidates | number of candidate database graphs.|
|Matching time | time to match query with candidate graphs.|
|Pure Matching time | pure time to match query with candidate graphs.|
|#Matches | number of query occurrences found in the database.|
|Total Time | total process time just for current query, does not include time to load database index.|
||Total process time for all queries is not reported. You can calculate it adding total times of each single query plus thie time to load the database.|

<hr />

### License

GraphGrepSX is distributed under the MIT license. This means that it is free for both academic and commercial use. Note however that some third party components in GraphGrepSX require that you reference certain works in scientific publications.
You are free to link or use GraphGrepSX inside source code of your own program. If do so, please reference (cite) GraphGrepSX and this website. We appreciate bug fixes and would be happy to collaborate for improvements. 
[MIT License](https://raw.githubusercontent.com/GiugnoLab/GraphGrepSX/master/LICENSE.txt)




### For Developers
#### Library Details
GraphGrepSX was developed in C++ using the Object Oriented paradigm. All core classes are locate in the GGSXLib folder and they are included in the GGSXLib namespace. The VF-Library is compiled and used as extarnal library, linked to the project by the makefile. Interfaces to this library are included in the GGSXVFLib namespace.

Project files are listed below along with a brief description of their.

| Name | Description |
|-------------|-------------|
|*GraphGrepSXConsole.cpp* | **Console User Interface**; implements all default end user operations.|
|*GGSXLib/ArgsParser.h*|For internal use only. <br> </brUsed>Used for shell arguments parsing||
|*GGSXLib/BuildMnager.h*| GraphGrepSX index building.|
|*GGSXLib.GGSXIndex.h*| Defines the global structure of the index: Suffix Tree (and paths occurrences) plus LabelMap.|
|*GGSXLib/GGSXLib.h*| High level library procedures.|
|*GGSXLib/GGSXVFLib.h*| Interface to the VF-Library.|
|*GGSXLib/GraphReaders.h*| Abstract declaration and default implementation of graphs' readers.|
|*GGSXLib/LabelMap.h*| Maps labels into integers, for efficency.|
|*GGSXLib/MatchManager.h*| Exaustive matching between query and target graphs (database).|
|*GGSXLib/MstlGPathListener.h*| Index build modalities. <br> **MstlGAllPathListener** for all paths build (database), it retrieves all paths up to LP depth. <br>**MstlGOnePathListener** only for maximal paths build (query), it retrieves maximal paths up to LP depth.|
|*GGSXLib/MstlGraph.h*| Graphs data structures definitions.|
|*GGSXLib/MstlGraphVisit.h*| Implements DFS visit for features extraction.|
|*GGSXLib/OCPTree.h*| Suffix tree definition. The tree is implemented using linked listes to represent the childs of a tree node. It, also, defines procedure to match trees (for filtering).|
|*GGSXLib/OCPTreeListeners.h*| For internal use only. <br>It implements the pruning rules of the filtering step.|
|*GGSXLib/OCPTreeNode.h*| Suffix tree node and table of paths occurrences (OCPTNGraphsInfos) implementation.|
|*GGSXLib/timer.h*| Timers implementation. See GraphGrepSXConsole.cpp for usage examples.|
|*GGSXLib/VF2GraphReaders.h*|Abstract declaration and default implementation of graphs' readers for the VF2 library.|
|*vflib2*| VF-Library's source code as relesed by authors. Include also the library's documentation. |


##### Build Database Index

To build the database index you can use high level API provided by GGSXLib.h, as follow:
```
#include "GGSXLib.h"
using namespace GGSXLib;
...
std::ifstream is;
is.open("databse_file", std::ios::in);
LabelMap labelMap;
GraphReader_gff greader(labelMap,is);	//default format reader
OCPTree sxtree;
build_db_sxtree(sxtree, greader, 4, false);
```

Otherwise, you can use standard library API:
```
#include "GGSXLib.h"
using namespace GGSXLib;
...
std::ifstream is;
is.open("databse_file", std::ios::in);
LabelMap labelMap;
GraphReader_gff greader(labelMap,is);	//default format reader
OCPTree sxtree;
MstlGraphVisitor gvisitor(new MstlGAllPathListener());	//all paths extractor
BuildManager bman(sxtree, greader, gvisitor, 4, false);
bman.run();
```

##### Build Query Index
To build query's index, at first you need to load the LabelMap of the specific database:
```
#include "GGSXLib.h"
using namespace GGSXLib;
...
LabelMap labelMap; OCPTree db_sxtree;
GGSXIndex index(labelMap, db_sxtree);
load_db_index(index, "database_file"); //you must input text database file name, not .index.ggsx file.
```
Then you can use high level API:
```
#include "GGSXLib.h"
using namespace GGSXLib;
...
std::ifstream is;
is.open("query_file", std::ios::in); 
OCPTree query_sxtree;
GraphReader_gff greader(db_sxtree._labelMap,is);	//using database label map
MstlGraph* query_graph = new MstlGraph(0);
bool qreaded = build_query_sxtree(query_sxtree, greader, lp, full_verbose, query_graph);
```
Or classic API:
```
#include "GGSXLib.h"
using namespace GGSXLib;
...
std::ifstream is;
is.open("query_file", std::ios::in); 
OCPTree query_sxtree;
GraphReader_gff greader(db_sxtree._labelMap,is);	//using database label map
MstlGraph* query_graph = new MstlGraph(0);
MstlGraphVisitor gvisitor(new MstlGOnePathListener());	//only maximal paths extractor
BuildManager bman(query, greader, gvisitor, lp, full_verbose);
bool qreaded = bman.run_single(query_graph);
```
##### Filtering

```
Once you got both database's index and query's suffix tree:
filtering_graph_set_t fgset;	//empty candidates list
query_sxtree.match(db_sxtree, *(new DefaultOCPTMatchingListener(fgset)));
```
##### Matching

After you filled the candidates list by the filtering step, you need to
load a VF2 structure of the query
load the VF2 structures of the database graphs
and then you can start the exaustive matching step.
```
...
std::ifstream is;
is.open("query file", std::ios::in);
VF2GraphReader_gff vf2greader(index._labelMap,is);
GGSXVFLib::MstlARGraph* vf2_query_graph = vf2greader.readGraph();
vf2_query_graph->SetNodeComparator(new GGSXVFLib::VIntegerComparator());

std::map graphsdb;
load_graphsdb(topts.db_file, index._labelMap, fgset, graphsdb, topts);

int mcount=0;
double pure_match_time=0;
match_query(0, vf2_query_graph, graphsdb, fgset, topts, &mcount, &pure_match_time);
```
For a full understading of the MatchingManager object, you can see the high level implementations of the procedure match_query in GGSXLib.h.

#### Create A Graph Reader

To create your own graph format reader, you must extend the abstract class GraphReader located in GraphReaders.h. This file contains also the default implementation called GraphReader_gff.

##### Modify Output Format

If you want modify the print format of found matches, you must rewrite the following function located in MatchManager.h
```
bool my_visitor(
		int n, 	//size of following arrays, equals to the number of nodes of the query.
		node_id ni1[], //sequence of query's nodes ids
		node_id ni2[],  //sequence of target nodes ids
		/* 
		 * ni1[0] is matched with ni2[0], an so on.
		*/
		void* usr_data){
			
	my_visitor_data_t* vis = (my_visitor_data_t*)usr_data;
	vis->match_count++;	
	...	
	return vis->justFirst;
};
```

##### Choose The Matching Algorithm

GraphGrepSX searches for subgraph isomorphims but it can also used for graph isomorphism and induced subgraph isomorphism.

To search for induced subgraph isomorphism you must change the matching algorithm, in GGSXVFLib.h, in this way:
```
void matchvf_monostate(	
		MstlARGraph* g1,
		MstlARGraph* g2,
		match_visitor visitor,
		void* data){

	//VF2MonoState s0(g1,g2);	//replace it
	VF2SubState s0(g1,g2);
	match(&s0, visitor, data);
};
```
Instead, to search for graph isomorphism you still need to change the matching algorithm:
```
void matchvf_monostate(	
		MstlARGraph* g1,
		MstlARGraph* g2,
		match_visitor visitor,
		void* data){
	if(g1->NodeCount() == g2->NodeCount()){	//add it
		//VF2MonoState s0(g1,g2);	//replace it
		VF2SubState s0(g1,g2);
		match(&s0, visitor, data);
	}
};
```
But it can also be very helpful modify the pruning rules, in OCPTreeListeners.h, to improve filtering power:
```
/*
 * Fill candidates set
 */
bool prune_rule_1(	OCPTreeNode &a,
					OCPTreeNode &b){
	size_t pre_size= _graphs.size();
	size_t occ = a.gsinfos[0];
	for(OCPTNGraphsInfos::iterator IT = b.gsinfos.begin(); IT!=b.gsinfos.end(); IT++){
		if(IT->second == occ)	//change it from >= to ==
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
		if(gis_IT->second != a.gsinfos[0]){	//change it from < to !=
			return false;
		}
	}
	return true;
}
```
#### Dialing With Labels On Edges

Since version 3.3, GraphGrepSX can deals with labels on edges during the verification step excuted by the VF2 library.
To allow this type of labels you need to:

+ redefine the type MstlARGraph defined in GGSXLib/GGSXVFLib.h
+ write a VF2GraphReader for your input file format
+ define a specific AttrComparator for your labels
+ assign the comparator/s to the query graph object
+ see the VF2lib documentation for more details about how to write an attributes comparator

Note that edge labels are still not considered for the indexing and filtering steps.

#####TODO List

Hoping to improve GraphGrepSX...
+ improve APIs
Semplify and remove unhelpful parameters.
Extend to search for all types fo isomorphims without rewrite source code.
+ improve filtering power
GraphGrephSX's paper says nothing about backward edges of the DFS vist for extract features. Current implementation looks only for forward edges ignoring cycles in graphs. An enhancement of the filteirng power can be obtained implementing two different paths' occurrences lists, the first one for classic forward edges and the second one for backward edges.
+ possibility of merging different database indexes
Add graphs to database index on the fly
+ replace the VF library with an ad-hoc matching module
No data structures conversion.
+ more graph readers
Write readers for the most important graph file formats.

#### References

V. Bonnici, A. Ferro, R. Giugno, A. Pulvirenti, D. Shasha, Enhancing Graph Database Indexing By Suffix Tree Structure.
Proc. of ACM 5th IAPR International Conference on Pattern Recognition in Bioinformatic. pp. 195-203 Lecture Notes in Bioinformatics. 22-24 September 2010, Nijmegen, The Netherlands.

R. Giugno, D. Shasha, GraphGrep: A Fast and Universal Method for Querying Graphs.
Proceeding of the International Conference in Pattern recognition (ICPR), Quebec, Canada, August 2002. 

[pdf](https://cs.nyu.edu/cs/faculty/shasha/papers/graphgrep/icpr2002.pdf)

D. Shasha, J.T-L Wang, R. Giugno, Algorithmics and Applications of Tree and Graph Searching
Proceeding of the ACM Symposium on Principles of Database Systems (PODS), Madison, Wisconsin, June 2002.

[pdf](https://cs.nyu.edu/cs/faculty/shasha/papers/graphgrep/pods2002.pdf)

Cordella L, Foggia P, Sansone C, Vento M, A (Sub)Graph Isomorphism Algorithm for Matching Large Graphs. 
IEEE Transactions on Pattern Analysis and Machine Intelligence 2004, 26(10):1367-1372.
