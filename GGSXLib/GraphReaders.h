/*
 * GraphReaders.h
 *
 * Graph Readers abstract class and default implementaion.
 */

#ifndef GRAPHREADERS_H_
#define GRAPHREADERS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <queue>

#include "MstlGraph.h"
#include "LabelMap.h"

namespace GGSXLib{

/*
 * Abstract class
 */
class GraphReader{
public:
	LabelMap& _labelMap;
	GraphReader(LabelMap& labelMap) : _labelMap(labelMap){}

	/*
	 * Read next graph.
	 */
	virtual bool readGraph(MstlGraph* g) = 0;
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
class GraphReader_gff : public GraphReader{
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

	GraphReader_gff(LabelMap& labelMap, std::ifstream& _in)
			: GraphReader(labelMap), in(_in){
		c_id = 0;
	}

	virtual bool gotoGraph(mstl_graph_id_t id){
		if(!in.is_open() || in.eof() || in.bad())
			return false;

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		int itemcount=0, i=0;
		int firste, seconde;

		while(c_id<id && !in.eof() && state!=End){
			in>>s;
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


//			switch (state){
//				case AttendGraph:{
//					if(s[0]=='#'){
//						state=LoadNumNodes;
//					}
//					else{
//						return false;
//					}
//					break;
//				}
//				case LoadNumNodes:{
//					itemcount=atoi(s.c_str());
//					if(itemcount==0)
//						return false;
//					i=0;
//					state=LoadNodes;
//					break;
//				}
//				case LoadNodes:{
//					i++;
//					if(i>=itemcount)
//						state=LoadNumEdges;
//
//					break;
//				}
//				case LoadNumEdges:{
//					//itemcount=stringto<int>(s);
//					itemcount=atoi(s.c_str()) *2;
//					i=0;
//					state=LoadEdges;
//					break;
//				}
//				case LoadEdges:{
//					i++;
//					if(i>=itemcount){
//						c_id++;
//						if(c_id!=id)
//							state=AttendGraph;
//						else
//							state=End;
//					}
//					break;
//				}
//			}
		}
		if(c_id==id && !in.eof())
			return true;
		return false;
	}



	virtual bool readGraph(MstlGraph* g){
		if(!in.is_open() || in.eof() || in.bad())
			return false;

		State state = AttendGraph;
		EState estate=FirstEdge;

		std::string s;
		int itemcount=0, i=0;
		int firste, seconde;

		while(!in.eof() && state!=End){
			in>>s;
			if(s=="")
				return false;

			if(s[0]=='#'){
				g->name = s;
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
				g->insertNode(_labelMap.getLabel(s));
			}

			in>>s;
			if(s=="")
				return false;
			itemcount=atoi(s.c_str());
			for(i=0; i<itemcount; i++){
				in>>s;
				if(s=="")
					return false;
				firste=atoi(s.c_str());


				in>>s;
				if(s=="")
					return false;
				seconde=atoi(s.c_str());

				g->insertEdge(firste, seconde);
				g->insertEdge(seconde, firste);
			}
			state=End;

//			switch (state){
//				case AttendGraph:{
//					if(s[0]=='#'){
//						g->name = s;
//						state=LoadNumNodes;
//					}
//					else{
//						return false;
//					}
//					break;
//				}
//				case LoadNumNodes:{
//					itemcount=atoi(s.c_str());
//					if(itemcount==0)
//						return false;
//					i=0;
//					state=LoadNodes;
//					break;
//				}
//				case LoadNodes:{
//					g->insertNode(_labelMap.getLabel(s));
//					i++;
//					if(i>=itemcount)
//						state=LoadNumEdges;
//
//					break;
//				}
//				case LoadNumEdges:{
//					//itemcount=stringto<int>(s);
//					itemcount=atoi(s.c_str());
//					i=0;
//					state=LoadEdges;
//					break;
//				}
//				case LoadEdges:{
//					if(i>=itemcount){
//						state=End;
//					}
//					else{
//						if(estate==FirstEdge){
//							firste=atoi(s.c_str());
//							estate=SecondEdge;
//						}
//						else{
//							seconde=atoi(s.c_str());
//							g->insertEdge(firste, seconde);
//							g->insertEdge(seconde, firste);
//							estate=FirstEdge;
//							i++;
//						}
//						if(i>=itemcount){
//							state=End;
//						}
//					}
//					break;
//				}
//			}
		}
		c_id++;
		return true;
	}
};

}

#endif /* GRAPHREADERS_H_ */
