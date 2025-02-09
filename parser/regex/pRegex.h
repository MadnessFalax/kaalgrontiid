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
	using EndNode = nspRegexAST::pEndNode;
	using QualNode = nspRegexAST::pQualifierNode;
	using QuantNode = nspRegexAST::pQuantifierNode;
	using StartNode = nspRegexAST::pStartNode;
	
	class pRegex {


		String* _pattern = nullptr;
		Automaton* _nfa = nullptr;
		Visitor* _visitor = nullptr;

	public:
		pRegex() : _pattern(new String()), _visitor(new Visitor()) {};
		pRegex(String& pattern) : _pattern(new String(pattern)), _visitor(new Visitor()) {};
		pRegex(const char* pattern) : _pattern(new String(pattern)) {};

		~pRegex() {
			delete _pattern;
			_pattern = nullptr;
			delete _nfa;
			_nfa = nullptr;
		}
		
		bool compile(const char* pattern) {
			return true;
		}

		bool compile() {
			auto an = AltNode();
			an.accept<Visitor>(_visitor);
			return true;
		}

		bool match(String& input) {
			return false;
		}

		bool match(String* input) {
			return false;
		}



	};



}