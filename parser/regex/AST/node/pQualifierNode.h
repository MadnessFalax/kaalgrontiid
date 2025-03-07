#pragma once
#include "../pRegexNode.h"
#include "../../../../container/pArray.h"
#include "../../../../container/pString.h"

#ifdef _DEBUG
#define __PROFILE_QL printf("QualifierNode.\n");
#endif

#ifndef _DEBUG
#define __PROFILE_QL
#endif

namespace nspRegexAST {
	template <class value_type>
	using Array = nspArray::pArray<value_type>;
	using String = nspString::pString;

	class pQualifierNode : public pRegexNode {
	public:
		Array<unsigned char>* _qualified_characters = nullptr;

		pQualifierNode() = delete;

		pQualifierNode(const unsigned char lower_bound, const unsigned char upper_bound) {
			_qualified_characters = new Array<unsigned char>();
			for (unsigned char i = lower_bound; i >= lower_bound && i <= upper_bound; i++) {
				_qualified_characters->push_back(i);
			}
		}

		pQualifierNode(const char c) {
			_qualified_characters = new Array<unsigned char>();
			_qualified_characters->push_back(c);
		}

		// takes ownership of qualified_characters array 
		pQualifierNode(Array<unsigned char>* qualified_characters) {
			if (qualified_characters == nullptr) {
				_qualified_characters = new Array<unsigned char>();
			}
			else {
				_qualified_characters = qualified_characters;
			}
		};

		// takes ownership of qualifier_characters_ptr pointed object, and sets ptr to nullptr
		pQualifierNode(Array<unsigned char>** qualifier_characters_ptr) {
			if (qualifier_characters_ptr == nullptr) {
				_qualified_characters = new Array<unsigned char>();
			}
			else {
				if (*qualifier_characters_ptr == nullptr) {
					_qualified_characters = new Array<unsigned char>();
				}
				else {
					_qualified_characters = *qualifier_characters_ptr;
					*qualifier_characters_ptr = nullptr;
				}
			}
		}

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

		void print(unsigned int indent = 0) override {
			auto prefix = String("");
			for (unsigned int i = 0; i < indent; i++) {
				prefix += "  ";
			}
			printf("%sQualifierNode\n", prefix.c_str());
			size_t size = _qualified_characters->size();
			for (size_t i = 0; i < size; i++) {
				printf("%s  %c\n", prefix.c_str(), (*_qualified_characters)[i]);
			}
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>& visitor) {
			visitor.visit(*this);
		}

		template <class visitor_type>
		void accept(Visitor<visitor_type>* visitor) {
			visitor->visit(*this);
		}

		char get_type() override {
			return 'l';
		}
	};
}