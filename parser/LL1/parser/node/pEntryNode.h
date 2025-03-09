#pragma once
#include "pParserNode.h"

namespace nspParser {
	template<typename enum_t, typename enum_r>
	class pEntryNode : public pParserNode<enum_t, enum_r> {
		enum_r _entry_rule = enum_r{};
		enum_r _this_node_rule = enum_r{};	

	public:
		/*
		* entry_rule is the rule that is supposed to be entered first after init
		* this_node_rule is the rule that this node is a part of - it is used for error reporting
		*/
		pEntryNode(enum_r entry_rule, enum_r this_node_rule) : _entry_rule(entry_rule), _this_node_rule(this_node_rule) {};

		~pEntryNode() {
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>* visitor) {
			visitor->visit(*this);
		}

		template<class visitor_t>
		void accept(pParserNode<enum_t, enum_r>::Visitor<visitor_t>& visitor) {
			visitor.visit(*this);
		}

		char get_type() override {
			return 'n';
		}

		enum_r get_entry_rule_id() const {
			return _entry_rule;
		}	

		enum_r get_this_node_rule_id() const {
			return _this_node_rule;
		}
	};
}