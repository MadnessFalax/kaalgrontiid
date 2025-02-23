//#include "parser/JSON/GeoJSONConverter.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "container/pArray.h"
#include "container/pString.h"
#include "utils/is_pointer.h"
#include "container/pMap.h"
#include "container/pPair.h"
#include "abstract/pHashable.h"
#include "parser/regex/pMatch.h"
#include "parser/regex/NFA/pState.h"
#include "parser/regex/pRegex.h"
#include "parser/regex/NFA/pAutomaton.h"
#include "parser/regex/AST/pRegexVisitor.h"
#include "parser/buffer/pFileHandler.h"
#include "utils/primitive_match.h"
#include "parser/regex/parser/pRegexLexer.h"
#include "parser/regex/parser/pRegexParser.h"

template <class T>
using Array = nspArray::pArray<T>;
using String = nspString::pString;
template <class T, class U, class V = unsigned short>
using Map = nspMap::pMap<T, U, V>;

using State = nspNFA::pState;
using Regex = nspRegex::pRegex;

using FileHandler = nspFile::pFileHandler;
using Pattern = nspPattern::pPattern;

using RegexLexer = nspRegexParser::pRegexLexer;
using RegexParser = nspRegexParser::pRegexParser;

// used for out of scope stack disposal check so that _CrtDumpMemoryLeaks doesnt show false positive leaks on stack allocated memory
static void helper() {
	//auto tmp = Regex(R"(^-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?$)").compile();			// json number

	//auto p = Pattern(String(" "));

	//auto* filename = "C:\\Users\\Petr\\Downloads\\src\\test\\kaalgrontiid\\test\\utf8_test.json";

	//auto fh = FileHandler(filename);
	//String s = fh.get_string(1);

	//printf("%s\n", p.match(s) ? "True" : "False");

	auto pattern = String(R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)");
	//auto pattern = R"(a+@\d+\.[a-z]{234,0})";

	auto parser = RegexParser(pattern);
	auto* root = parser.parse();
	root->print();
	delete root;

	/*
	const char* pattern = R"(a+@\d+\.[a-z]{2,})";
	auto* tmp_arr = new Array<unsigned char>();
	tmp_arr->push_back('a');
	auto* a = new nspRegexAST::pQualifierNode(tmp_arr);

	auto* a_star = new nspRegexAST::pQuantifierNode(0, 0, a);

	tmp_arr = new Array<unsigned char>();
	tmp_arr->push_back('@');
	auto* at = new nspRegexAST::pQualifierNode(tmp_arr);

	tmp_arr = new Array<unsigned char>();
	for (unsigned char i = '0'; i <= '9'; i++) {
		tmp_arr->push_back(i);
	}
	auto* digit = new nspRegexAST::pQualifierNode(tmp_arr);

	auto* digit_plus = new nspRegexAST::pQuantifierNode(1, 0, digit);

	tmp_arr = new Array<unsigned char>();
	tmp_arr->push_back('.');
	auto* dot1 = new nspRegexAST::pQualifierNode(tmp_arr);

	tmp_arr = new Array<unsigned char>();
	for (unsigned char i = 'a'; i <= 'z'; i++) {
		tmp_arr->push_back(i);
	}
	auto* range = new nspRegexAST::pQualifierNode(tmp_arr);

	auto* range_quant = new nspRegexAST::pQuantifierNode(2, 0, range);

	auto* conc_arr = new Array<nspRegexAST::pRegexNode*>();
	conc_arr->push_back(a_star);
	conc_arr->push_back(at);
	conc_arr->push_back(digit_plus);
	conc_arr->push_back(dot1);
	conc_arr->push_back(range_quant);
	auto* conc = new nspRegexAST::pConcatNode(conc_arr);

	auto* visitor = new nspRegexAST::pRegexVisitor(conc);

	Regex* re = new Regex(visitor);
	re->compile();

	String str = String("@1.czz");

	bool result = re->match(str);

	delete re;
	re = nullptr;*/

	return;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	
	helper();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();
	return 0;
}