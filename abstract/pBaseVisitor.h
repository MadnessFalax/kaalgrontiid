#pragma once


/*
* derived - what kind of visitor is being used
* node_type - what node are we visiting
*/
template <typename derived> 
class pBaseVisitor {
public:
	template<typename node_type>
	void visit(node_type& node) {
		static_cast<derived*>(this)->resolve_visit(static_cast<node_type&>(node));
	}

	virtual ~pBaseVisitor() = default;
};