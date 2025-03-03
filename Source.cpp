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
	auto pattern = String(R"(\-?(0|[1-9]\d*)(\.\d+)?([eE][\+\-]?\d+)?)");

	auto* regex = new Regex(pattern);
	if (regex->compile()) {
		printf("result: %s\n", regex->match(String(".5")) ? "True" : "False");   
		printf("result: %s\n", regex->match(String("5.")) ? "True" : "False");   
		printf("result: %s\n", regex->match(String("-.5")) ? "True" : "False");  
		printf("result: %s\n", regex->match(String("+42")) ? "True" : "False");  
		printf("result: %s\n", regex->match(String("00")) ? "True" : "False");   
		printf("result: %s\n", regex->match(String("01")) ? "True" : "False");   
		printf("result: %s\n", regex->match(String("1e")) ? "True" : "False");   
		printf("result: %s\n", regex->match(String("1e+")) ? "True" : "False");  
		printf("result: %s\n", regex->match(String("1.0e")) ? "True" : "False"); 
		printf("result: %s\n", regex->match(String("e10")) ? "True" : "False");  
		printf("result: %s\n", regex->match(String("--42")) ? "True" : "False"); 
		printf("result: %s\n", regex->match(String("3.14.15")) ? "True" : "False");
		printf("result: %s\n", regex->match(String("1e2.5")) ? "True" : "False"); 
		printf("result: %s\n", regex->match(String(" ")) ? "True" : "False");    
		printf("result: %s\n", regex->match(String("")) ? "True" : "False");     

	}

	delete regex;
	regex = nullptr;

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