/*
 * GGSXVFLib.h
 *
 *  Wrappers for VFLib
 */

#ifndef GGSXVFLIB_H_
#define GGSXVFLIB_H_

#include <map>

#include <argraph.h>
#include <argedit.h>
#include <vf2_mono_state.h>
#include <vf2_sub_state.h>
#include <match.h>

#include "MstlGraph.h"

namespace GGSXVFLib{

using namespace GGSXLib;

/*
 * VFLib graph class.
 */
class MstlARGraph : public ARGraph<mstl_node_label_t,void>{
public:
	MstlARGraph(ARGEdit* ed) : ARGraph<mstl_node_label_t,void>(ed){};
};

/*
 * Convert GGSX Graph Type into VFLib graph type.
 */
MstlARGraph* convertGraf(MstlGraph& g){
	ARGEdit ed;
	for(mstl_nodes_list_t::iterator IT=g.nodes.begin(); IT!=g.nodes.end(); IT++){
			ed.InsertNode(&(IT->second.label));
	}

	for(mstl_nodes_list_t::iterator IT=g.nodes.begin();
			IT!=g.nodes.end(); IT++){
		for(mstl_node_neighbors_t::iterator ITT=IT->second.out_neighbors.begin();
				ITT!=IT->second.out_neighbors.end(); ITT++){
			ed.InsertEdge(IT->first,*ITT,NULL);
		}
	}
	MstlARGraph* vg=new MstlARGraph(&ed);
	return vg;
};

/*
 * Label comparator.
 */
class VIntegerComparator: public AttrComparator{
public:
	virtual bool compatible(void* pa, void* pb){
		return (*((mstl_node_label_t*)pa))==(*((mstl_node_label_t*)pb));
	};
};

/*
 * Match. Monomorphism.
 * g1 must be the query.
 * g2 must be the reference.
 */
void matchvf_monostate(	MstlARGraph* g1,
							MstlARGraph* g2,
						match_visitor visitor,
						void* data){

	VF2MonoState s0(g1,g2);
	//VF2SubState s0(g1,g2);
	match(&s0, visitor, data);
};

}


#endif /* GGSXVFLIB_H_ */
