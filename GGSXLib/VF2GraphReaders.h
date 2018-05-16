/*
 * VF2GraphReaders.h
 *
 *  Created on: Feb 12, 2013
 *      Author: vbonnici
 */

#ifndef VF2GRAPHREADERS_H_
#define VF2GRAPHREADERS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "MstlGraph.h"
#include "LabelMap.h"
#include "GGSXVFLib.h"

namespace GGSXLib{

/*
 * Abstract class
 */
class VF2GraphReader{
public:
	LabelMap& _labelMap;
	VF2GraphReader(LabelMap& labelMap) : _labelMap(labelMap){}

	/*
	 * Read next graph.
	 */
	virtual GGSXVFLib::MstlARGraph* readGraph() = 0;
	/*
	 * Stop stream pointer before graph.
	 * Just go forward needed.
	 */
	virtual bool gotoGraph(mstl_graph_id_t id) =0;
};


/*
 * Default format implementation.
 * Undirect graphs.
 */
class VF2GraphReader_gff : public VF2GraphReader{
public:
	enum State {
		AttendGraph,
		LoadNumNodes,
		LoadNodes,
		LoadNumEdges,
		LoadEdges,
		End
	};
	enum EState{
		FirstEdge,
		SecondEdge
	};


	std::ifstream& in;
	mstl_graph_id_t c_id;

	VF2GraphReader_gff(LabelMap& labelMap, std::ifstream& _in)
			: VF2GraphReader(labelMap), in(_in){
		c_id = 0;
	}

	virtual bool gotoGraph(mstl_graph_id_t id){
//		std::cout<<"go("<<id<<"):"<<c_id<<"\n";

		if(!in.is_open() || in.eof() || in.bad())
			return false;

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		int itemcount=0, i=0;
		int firste, seconde;

		while(c_id<id && !in.eof() && state!=End){
			in>>s;

//			std::cout<<"go: "<<s<<"\n";

			if(s=="")
				return false;

			if(s[0]=='#'){
			}
			else{
				return false;
			}


			in>>s;
			if(s=="")
				return false;
			itemcount=atoi(s.c_str());
			if(itemcount==0)
				return false;

			for(i=0; i<itemcount; i++){
				in>>s;
				if(s=="")
					return false;
			}

			in>>s;
			if(s=="")
				return false;
			itemcount=atoi(s.c_str());
			for(i=0; i<itemcount; i++){
				in>>s;
				if(s=="")
					return false;
				in>>s;
				if(s=="")
					return false;
			}

			c_id++;
		}
		if(c_id==id && !in.eof())
			return true;
		return false;
	}



	virtual GGSXVFLib::MstlARGraph* readGraph(){
		if(!in.is_open() || in.eof() || in.bad())
			return NULL;

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		int itemcount=0, i=0;
		int firste, seconde;

		ARGEdit ed;

		while(!in.eof() && state!=End){
			in>>s;

//			std::cout<<"read: "<<s<<"\n";

			if(s=="")
				return NULL;

			if(s[0]=='#'){
//				g->name = s;
			}
			else{
				return NULL;
			}


			in>>s;
			if(s=="")
				return NULL;
			itemcount=atoi(s.c_str());
			if(itemcount==0)
				return NULL;

			for(i=0; i<itemcount; i++){
				in>>s;
				if(s=="")
					return NULL;
//				g->insertNode(_labelMap.getLabel(s));
//				mstl_node_label_t* l = new mstl_node_label_t;
//				*l = _labelMap.getLabel(s);

				//ed.InsertNode( new mstl_node_label_t(_labelMap.getLabel(s)) );
				ed.InsertNode( (_labelMap.getLabelP(s)) );
			}

			in>>s;
			if(s=="")
				return NULL;
			itemcount=atoi(s.c_str());
			for(i=0; i<itemcount; i++){
				in>>s;
				if(s=="")
					return NULL;
				firste=atoi(s.c_str());


				in>>s;
				if(s=="")
					return NULL;
				seconde=atoi(s.c_str());

//				g->insertEdge(firste, seconde);
//				g->insertEdge(seconde, firste);
				ed.InsertEdge(firste, seconde, NULL);
				ed.InsertEdge(seconde, firste, NULL);
			}
			state=End;
		}


		c_id++;
		return new GGSXVFLib::MstlARGraph(&ed);;
	}
};

}


#endif /* VF2GRAPHREADERS_H_ */
