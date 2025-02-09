#pragma once


namespace nspAST {

	/*
	* derived - what kind of visitor is being used
	* node_type - what node are we visiting
	*/
	template <typename derived> 
	class pASTVisitor {
	public:
		template<typename node_type>
		void visit(node_type& node) {
			static_cast<derived*>(this)->resolve_visit(node);
		}
	};
}