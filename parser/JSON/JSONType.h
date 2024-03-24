#pragma once

#include <iostream>
#include "../../utils/pString.h"
#include "../../utils/pArray.h"
#include "../../utils/pMap.h"

/**
*
*	- unicode is not implemented
*
*/

class JSONType {

protected:
    char code;

public:
    JSONType() {
        code = 't';
    }

    virtual ~JSONType() {

    }

    virtual char get_code() {
        return this->code;
    }

    virtual void dispose() {

    }

    virtual void dispose_deep() {

    }

};

class JSONNumber : public JSONType {
    double value;

public:
    JSONNumber() {
        this->value = NULL;
        code = 'n';
    }

    JSONNumber(double x) {
        this->value = x;
        code = 'n';
    }

    double get() {
        return this->value;
    }

    void set(double x) {
        this->value = x;
    }

    char get_code() {
        return this->code;
    }

    static JSONNumber* copy(JSONNumber* x) {
        return new JSONNumber(x->value);
    }
};

class JSONString : public JSONType {
    pString value;

public:
    JSONString() {
        this->value = pString("");
        code = 's';
    }

    JSONString(pString& x) {
        this->value = pString(x);
        code = 's';
    }

    pString get() {
        return this->value;
    }

    void set(pString& x) {
        this->value = x;
    }

    void append(pString& x) {
        this->value += x;
    }

    void append(char x) {
        this->value += x;
    }

    char get_code() {
        return this->code;
    }

    static JSONString* copy(JSONString* x) {
        return new JSONString(x->value);
    }
};

class JSONBoolean : public JSONType {
    bool value;

public:
    JSONBoolean() {
        this->value = NULL;
        code = 'b';
    }

    JSONBoolean(bool x) {
        this->value = x;
        code = 'b';
    }

    bool get() {
        return this->value;
    }

    void set(bool x) {
        this->value = x;
    }

    char get_code() {
        return this->code;
    }

    static JSONBoolean* copy(JSONBoolean* x) {
        return new JSONBoolean(x->value);
    }
};

class JSONEmpty : public JSONType {

public:
    JSONEmpty() {
        code = 'e';

    }

    char get_code() {
        return this->code;
    }

    static JSONEmpty* copy(JSONEmpty* x) {
        return new JSONEmpty();
    }
};

class JSONArray : public JSONType {
    pArray<JSONType*> value;

public:
    JSONArray() {
        this->value = pArray<JSONType*>();
        code = 'a';
    }

    JSONArray(pArray<JSONType*> x) {
        this->value = x;
        code = 'a';
    }

    ~JSONArray() {
        for (auto x : value) {
            x = nullptr;
        }
        value.clear();
    }

    pArray<JSONType*> get() {
        return this->value;
    }

    void set(pArray<JSONType*> x) {
        this->value = x;
    }

    char get_code() {
        return this->code;
    }

    void dispose() override {
        for (auto x : value) {
            delete x;
            x = nullptr;
        }
        value.clear();
    }

    void dispose_deep() override {
        for (auto x : value) {
            x->dispose_deep();
            delete x;
            x = nullptr;
        }
    }

    static JSONArray* copy(JSONArray* x);
};


// JSONObject key are of static pString type for further processing simplification
// note: it has to be JSON String anyway, meaning that polymorphism isn't needed here
class JSONObject : public JSONType {
    pMap<JSONType*> value;

public:
    JSONObject() {
        this->value = pMap<JSONType*>();
        code = 'o';
    }

    JSONObject(pMap<JSONType*> x) {
        this->value = x;
        code = 'o';
    }

    ~JSONObject() {
        for (auto x : value) {
            x.second = nullptr;
        }
        value.clear();
    }

    pMap<JSONType*> get() {
        return this->value;
    }

    void set(pMap<JSONType*> x) {
        this->value = x;
    }

    char get_code() {
        return this->code;
    }

    void dispose() override {
        for (auto x : value) {
            delete x.second;
            x.second = nullptr;
        }
        value.clear();
    }

    void dispose_deep() override {
        for (auto x : value) {
            x.second->dispose_deep();
            delete x.second;
            x.second = nullptr;
        }
    }

    static JSONObject* copy(JSONObject* x);
    
};

JSONObject* JSONObject::copy(JSONObject* x) {
    JSONObject* tmp = new JSONObject();

    for (auto pair : x->get()) {
        JSONType* element = pair.second;
        char code = element->get_code();
        switch (code) {
            case 'n': {
                JSONNumber* cast_el = dynamic_cast<JSONNumber*>(element);
                tmp->value.set(pString(pair.first), JSONNumber::copy(cast_el));
                //tmp->value[pair.first] = (JSONNumber::copy(cast_el));
                break;
            }
            case 's': {
                JSONString* cast_el = dynamic_cast<JSONString*>(element);
                tmp->value.set(pString(pair.first), JSONString::copy(cast_el));
                //tmp->value[pair.first] = (JSONString::copy(cast_el));
                break;
            }
            case 'b': {
                JSONBoolean* cast_el = dynamic_cast<JSONBoolean*>(element);
                tmp->value.set(pString(pair.first), JSONBoolean::copy(cast_el));
                //tmp->value[pair.first] = (JSONBoolean::copy(cast_el));
                break;
            }
            case 'e': {
                JSONEmpty* cast_el = dynamic_cast<JSONEmpty*>(element);
                tmp->value.set(pString(pair.first), JSONEmpty::copy(cast_el));
                //tmp->value[pair.first] = (JSONEmpty::copy(cast_el));
                break;
            }
            case 'a': {
                JSONArray* cast_el = dynamic_cast<JSONArray*>(element);
                tmp->value.set(pString(pair.first), JSONArray::copy(cast_el));
                //tmp->value[pair.first] = (JSONArray::copy(cast_el));
                break;
            }
            case 'o': {
                JSONObject* cast_el = dynamic_cast<JSONObject*>(element);
                tmp->value.set(pString(pair.first), JSONObject::copy(cast_el));
                //tmp->value[pair.first] = (JSONObject::copy(cast_el));
                break;
            }
            default: {
                break;
            }
        }
    }

    return tmp;
}

JSONArray* JSONArray::copy(JSONArray* x) {
    JSONArray* tmp = new JSONArray();
    int count = x->get().size();

    for (int i = 0; i < count; i++) {
        JSONType* element = (x->get())[i];
        char code = element->get_code();
        switch (code) {
            case 'n': {
                JSONNumber* cast_el = dynamic_cast<JSONNumber*>(element);
                tmp->value.push_back(JSONNumber::copy(cast_el));
                break;
            }
            case 's': {
                JSONString* cast_el = dynamic_cast<JSONString*>(element);
                tmp->value.push_back(JSONString::copy(cast_el));
                break;
            }
            case 'b': {
                JSONBoolean* cast_el = dynamic_cast<JSONBoolean*>(element);
                tmp->value.push_back(JSONBoolean::copy(cast_el));
                break;
            }
            case 'e': {
                JSONEmpty* cast_el = dynamic_cast<JSONEmpty*>(element);
                tmp->value.push_back(JSONEmpty::copy(cast_el));
                break;
            }
            case 'a': {
                JSONArray* cast_el = dynamic_cast<JSONArray*>(element);
                tmp->value.push_back(JSONArray::copy(cast_el));
                break;
            }
            case 'o': {
                JSONObject* cast_el = dynamic_cast<JSONObject*>(element);
                tmp->value.push_back(JSONObject::copy(cast_el));
                break;
            }
            default: {
                break;
            }
        }
    }

    return tmp;
}