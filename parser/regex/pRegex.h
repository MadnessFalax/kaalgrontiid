#pragma once
#include "../../container/pString.h"
#include "NFA/pAutomaton.h"
#include "AST/pRegexVisitor.h"
#include "parser/pRegexParser.h"

namespace nspRegex {
	using String = nspString::pString;
	using Automaton = nspNFA::pAutomaton;
	using Visitor = nspRegexAST::pRegexVisitor;
	using AltNode = nspRegexAST::pAlternationNode;
	using ConcNode = nspRegexAST::pConcatNode;
	using QualNode = nspRegexAST::pQualifierNode;
	using QuantNode = nspRegexAST::pQuantifierNode;
	using Parser = nspRegexParser::pRegexParser;
	
	class pRegex {
		String* _pattern = nullptr;
		Parser* _parser = nullptr;
		Automaton* _nfa = nullptr;
		Visitor* _visitor = nullptr;

	public:
		pRegex() = delete;
		//doesn't take ownership of pattern
		pRegex(String& pattern) : _pattern(new String(pattern)) {};
		//doesn't take ownership of pattern
		pRegex(const char* pattern) : _pattern(new String(pattern)) {};

		~pRegex() {
			delete _parser;
			_parser = nullptr;
			delete _pattern;
			_pattern = nullptr;
			delete _nfa;
			_nfa = nullptr;
			delete _visitor;
			_visitor = nullptr;
		}

		bool compile() {
			if (_pattern) {
				_parser = new Parser(*_pattern);
				auto* tmp_node = _parser->parse();
				delete _parser;
				_parser = nullptr;
				_visitor = new Visitor(tmp_node);
				tmp_node = nullptr;
				_visitor->resolve_tree();
				_nfa = _visitor->create_NFA();
				delete _visitor;
				_visitor = nullptr;
				return true;
			}
			return false;
		}

		bool match(const char* input) {
			if (_nfa) {
				return _nfa->match(input);
			}
			else {
				throw pNFAUndefinedException();
			}
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