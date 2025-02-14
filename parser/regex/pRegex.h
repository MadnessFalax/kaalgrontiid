#pragma once
#include "../../container/pString.h"
#include "NFA/pAutomaton.h"
#include "AST/pRegexVisitor.h"

namespace nspRegex {
	using String = nspString::pString;
	using Automaton = nspNFA::pAutomaton;
	using Visitor = nspRegexAST::pRegexVisitor;
	using AltNode = nspRegexAST::pAlternationNode;
	using ConcNode = nspRegexAST::pConcatNode;
	using QualNode = nspRegexAST::pQualifierNode;
	using QuantNode = nspRegexAST::pQuantifierNode;
	
	class pRegex {


		String* _pattern = nullptr;
		Automaton* _nfa = nullptr;
		Visitor* _visitor = nullptr;

	public:
		pRegex() : _pattern(new String()), _visitor(new Visitor()) {};
		pRegex(String& pattern) : _pattern(new String(pattern)), _visitor(new Visitor()) {};
		pRegex(Visitor* visitor) : _visitor(visitor) {};
		pRegex(const char* pattern) : _pattern(new String(pattern)) {};

		~pRegex() {
			delete _pattern;
			_pattern = nullptr;
			delete _nfa;
			_nfa = nullptr;
			delete _visitor;
			_visitor = nullptr;
		}

		bool compile() {
			if (_visitor) {
				_visitor->resolve_tree();
				_nfa = _visitor->create_NFA();
				delete _visitor;
				_visitor = nullptr;
				return true;
			}
			return false;
		}

		bool match(String& input) {
			if (_nfa) {
				return _nfa->match(input.c_str());
			}
			else {
				throw pNFAUndefinedException();
			}
		}

		bool match(String* input) {
			if (_nfa) {
				return _nfa->match(input->c_str());
			}
			else {
				throw pNFAUndefinedException();
			}
		}



	};



}