/*
 * GGSXIndex.h
 */

#ifndef GGSXINDEX_H_
#define GGSXINDEX_H_

#include <iostream>

#include "OCPTree.h"
#include "LabelMap.h"

namespace GGSXLib{

class GGSXIndex{
public:
	LabelMap& _labelMap;
	OCPTree& _ocpTree;

	GGSXIndex(LabelMap& labelMap, OCPTree& ocpTree)
		: _labelMap(labelMap), _ocpTree(ocpTree){
	}

	friend std::ostream& operator<<(std::ostream& os, GGSXIndex& i){
		os<<i._labelMap;
		os<<i._ocpTree;
		return os;
	}
	friend std::istream& operator>>(std::istream& is, GGSXIndex& i){
		is>>i._labelMap;
		is>>i._ocpTree;
		return is;
	}
};

}

#endif /* GGSXINDEX_H_ */
