#pragma once
#include "../pRegexNode.h"
#include "../../../../container/pArray.h"


namespace nspRegexAST {
	template <class value_type>
	using Array = nspArray::pArray<value_type>;

	class pQualifierNode : public pRegexNode {
		Array<unsigned char>* _qualified_characters = nullptr;

	public:
		// takes ownership of qualified_characters array 
		pQualifierNode() : _qualified_characters(new Array<unsigned char>()) {};
		pQualifierNode(Array<unsigned char>* qualified_characters) {
			if (qualified_characters == nullptr) {
				_qualified_characters = new Array<unsigned char>();
			}
			else {
				_qualified_characters = qualified_characters;
			}
		};
		pQualifierNode(const pQualifierNode& other) : _qualified_characters(new Array<unsigned char>(*other._qualified_characters)) {}
		pQualifierNode(pQualifierNode&& other) noexcept : _qualified_characters(other._qualified_characters) {
			other._qualified_characters = nullptr;
		}
		~pQualifierNode() { 
			delete _qualified_characters; 
			_qualified_characters = nullptr; 
		}

		pQualifierNode& operator=(const pQualifierNode& other) {
			if (this != &other) {
				delete _qualified_characters;
				_qualified_characters = nullptr;
				_qualified_characters = new Array<unsigned char>(*other._qualified_characters);
			}

			return *this;
		}

		pQualifierNode& operator=(pQualifierNode&& other) noexcept {
			if (this != &other) {
				delete _qualified_characters;
				_qualified_characters = nullptr;
				_qualified_characters = other._qualified_characters;
				other._qualified_characters = nullptr;
			}

			return *this;
		}

	};
}