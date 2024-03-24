#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "JSONType.h"
#include <iostream>
#include <cstdio>
#include "../../utils/pString.h"
// #include <fstream>

class JSONParser
{

    pString filename;
    FILE* filestream;

    pString buf;
    int index;
    int buf_size;
    bool error;
    bool last;
    bool skip_whitespace;

    char cur_char;
    char prec_char;

    JSONType* root;

public:
    JSONParser(pString filename) {
        this->filename = filename;
    
        this->buf = pString();
        this->index = 0;
        this->buf_size = 0;
        this->error = false;
        this->last = false;
        this->skip_whitespace = true;

        this->cur_char = '\0';
        this->prec_char = '\0';
    }

    ~JSONParser() {
        root->dispose_deep();
        delete root;
        root = nullptr;
    }

    JSONType* get_root() {
        return this->root;
    }

    bool read() {
        char* holder = new char[1024];

        this->filestream = fopen(filename.c_str(), "r");
        
        while (!feof(filestream)) {
            fgets(holder, 1022, filestream);
            buf += holder;
        }

        fclose(filestream);
        delete[] holder;

        buf_size = (int)buf.length();

        if (buf_size <= 0) {
            return false;
        }
        else
            return true;
    }

    void parse() {
        // load first character
        next_char();

        root = parse_node();
    }


    bool made_error() {
        return this->error;
    }

private:
    JSONType* parse_node() {

        if (cur_char == '{') {
            return parse_object();
        }

        else if (cur_char == '[') {
            return parse_array();
        }

        else if (cur_char == '\"') {
            return parse_string();
        }

        else if (cur_char == 't') {
            return parse_boolean("true");
        }

        else if (cur_char == 'f') {
            return parse_boolean("false");
        }

        else if (cur_char == '-' || (cur_char >= '0' && cur_char <= '9')) {
            return parse_number();
        }

        else if (cur_char == 'n') {
            return parse_empty();
        }

        error = true;
        return new JSONEmpty();
    }

    // ----- parsing of JSONTypes -----

    JSONNumber* parse_number() {
        pString value = pString("");

        pString whole_part = pString("");
        pString fraction_part = pString("");
        pString exponent = pString("");

        bool frac = false;
        bool exp = false;

        whole_part = parse_digits(true, false, true);

        if (detect_character('.')) {
            frac = true;
            fraction_part = parse_digits(false, false, false);
        }

        if (detect_character('e')) {
            exp = true;
            exponent = parse_digits(false, true, true);
        }
        else if (detect_character('E')) {
            exp = true;
            exponent = parse_digits(false, true, true);
        }

        value += whole_part;

        if (frac) {
            value += '.';
            value += fraction_part;
        }

        if (exp) {
            value += 'e';
            value += exponent;
        }

        if (error == true)
            return nullptr;
        else
            return new JSONNumber(strtod(value.c_str(), nullptr));
    }

    JSONString* parse_string() {
        pString value = pString("");
        
        skip_whitespace = false;

        while (index < buf_size) {
            next_char();

            if (cur_char == '\"') {
                skip_whitespace = true;
                next_char();
                return new JSONString(value);
            }
            else {
                value += cur_char;
            }
        }
        skip_whitespace = true;
        error = true;
        return nullptr;
    }

    JSONBoolean* parse_boolean(pString pattern) {
        if (match(pattern)) {
            return new JSONBoolean(pattern == "true");
        }
        else {
            return nullptr;
        }
    }

    JSONEmpty* parse_empty() {
        if (match("null"))
            return new JSONEmpty();
        return nullptr;
    }

    JSONArray* parse_array() {
        pArray<JSONType*> array;

        JSONType* tmp_value = nullptr;

        next_char();

        while (!detect_character(']')) {
            tmp_value = parse_node();

            if (tmp_value == nullptr) {
                error = true;
                return new JSONArray(array);
            }

            array.push_back(tmp_value);

            if (!detect_character(',')) {
                if (!detect_character(']'))
                    error = true;
                break;
            }
        }

        return new JSONArray(array);

    }

    JSONObject* parse_object() {
        pMap<JSONType*> map = pMap<JSONType*>();

        JSONString* tmp_key = nullptr;
        JSONType* tmp_value = nullptr;

        next_char();

        while (!detect_character('}')) {
            tmp_key = parse_string();

            if (tmp_key == nullptr) {
                error = true;
                return new JSONObject(map);
            }
            
            if (!detect_character(':')) {
                error = true;
                return new JSONObject(map);
            }

            tmp_value = parse_node();

            if (tmp_value == nullptr) {
                error = true;
                return new JSONObject(map);
            }

            //map[tmp_key->get()] = tmp_value;
            map.set(pString(tmp_key->get()), tmp_value);
            tmp_key = nullptr;
            tmp_value = nullptr;

            if (!detect_character(',')) {
                if (detect_character('}'))
                    break;
                else {
                    error = true;
                    return nullptr;
                }
            }
        }

        JSONObject* return_val = new JSONObject(map);

        return return_val;
            
    }
    
    // ----- Utils -----

    // call this when you expect specific character... it either detects it a moves to next char and returns true, or stops and returns false
    bool detect_character(char c) {
        bool detected = false;
        if (cur_char == c) {
            detected = true;
            next_char();
        }

        return detected;
    }

    // return zero - return if starts with zero, allow_plus - can start with plus, allow_minus - can start with minus
    pString parse_digits(bool return_zero, bool allow_plus, bool allow_minus) {
        pString value = pString("");

        if (cur_char == '+')
        {
            if (!allow_plus) {
                error = true;
            }
            next_char();
        }

        if (cur_char == '-') {
            if (!allow_minus) {
                error = true;
            }
            value += '-';
            next_char();
        }

        if (return_zero) {
            if (cur_char == '0') {
                next_char();
                return value;
            }
        }

        if (!(cur_char >= '0' && cur_char <= '9')) {
            error = true;
        }
        while (cur_char >= '0' && cur_char <= '9') {
            value += cur_char;
            next_char();
        }
        return value;
    }

    // cycle to next character in buffer stream
    char next_char() {
        if (last)
            return cur_char;

        prec_char = cur_char;
        cur_char = buf[index];
        index++;

        if (index == buf_size)
            last = true;

        // remove whitespace
        if (skip_whitespace)
            if (cur_char == ' ' || cur_char == '\n')
                next_char();

        return cur_char;
    }

    // never should be called twice without next_char call in between
    char step_back() {
        if (prec_char == '\0') {
            error = true;
            return '\0';
        }

        cur_char = prec_char;
        prec_char = '\0';
        index--;

        return cur_char;
    }

    // match stream with pattern, cur_char should be equivalent to the first char of the pattern
    bool match(pString pattern) {
        int pattern_index = 0;
        int pattern_size = (int)pattern.length();

        while (pattern_index < pattern_size) {
            if (pattern[pattern_index] == cur_char) {
                next_char();
                pattern_index++;
            }
            else {
                error = true;
                return false;
            }
        }

        return true;
    }
};
