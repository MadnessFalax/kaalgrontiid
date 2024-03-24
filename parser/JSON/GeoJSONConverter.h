#pragma once

#include "JSONParser.h"
#include "GeoJSONType.h"
#include <utility>

/**
* 
*	Purpose of converter is to extract data from raw JSON and process them into GeoJSONTypes
* 
*	note: how about converting to native types during conversion?
*	recursively change coord JSONArrays to nested pArrays (tuple at leaf coords?)
*	change tree roots to smart pointers?
*/

class GeoJSONConverter
{
    JSONParser* json_parser;
    JSONType* json_root;
    bool error;
    bool recursion_error;
    bool vectorization_error;
    bool vectorized;
    GeoJSONType* geo_root;
    pArray<GJFeature*> vectorized_data;

public:
    GeoJSONConverter(pString filename) {
        error = false;
        recursion_error = false;
        vectorization_error = false;
        json_parser = new JSONParser(filename);
        json_root = nullptr;
        geo_root = nullptr;
        vectorized = false;
        vectorized_data = pArray<GJFeature*>();
    }

    ~GeoJSONConverter() {
        json_root = nullptr;
        delete json_parser;
        json_parser = nullptr;
        geo_root->dispose_gj_deep();
        delete geo_root;
        geo_root = nullptr;
    }

    void convert() {
        json_parser->read();
        json_parser->parse();
        if (json_parser->made_error()) {
            error = true;
        }
        else {
            json_root = json_parser->get_root();
            geo_root = convert_node(json_root, 'a');
        }
    }

    bool made_error() {
        return this->error || this->recursion_error || this->vectorization_error;
    }

    // obtain pointer and it's ownership, will be deprecated with next stage implementation
    // use this only for testing... prefer later stage processed data
    /*GeoJSONType* get_root() {
        GeoJSONType* tmp = this->geo_root;
        this->geo_root = nullptr;
        return tmp;
    }*/

    // convert object from tree structure to an array of leaf objects (first step preparation for SequentialArray)
    pArray<GJFeature*> vectorize() {
        if (!vectorized) {
            if (geo_root != nullptr) {
                vectorize_node(geo_root, nullptr);
                vectorized = true;
            }
        }

        pArray<GJFeature*> data_cpy = vector_deep_copy();

        return data_cpy;
    }

private:

    pArray<GJFeature*> vector_deep_copy() {
        pArray<GJFeature*> cpy = pArray<GJFeature*>();

        for (auto feat : this->vectorized_data) {
            cpy.push_back(GJFeature::copy(feat));
        }

        return cpy;
    }

    // ----- CONVERSION -----

    // process single tree node, use type's code to specify accepted types (use 'a' for all)
    GeoJSONType* convert_node(JSONType* node, char type_filter) {
        // prepare possible data holders
        JSONObject* geometry = nullptr;
        JSONObject* properties = nullptr;
        JSONArray* features = nullptr;
        JSONArray* geometries = nullptr;

        // process node JSONType
        if (node->get_code() != 'o') {
            error = true;
            return nullptr;
        }

        JSONObject* cast_node = dynamic_cast<JSONObject*>(node);
        if (cast_node == nullptr) {
            error = true;
            return nullptr;
        }

        // determine GeoJSONType
        JSONType* type = nullptr;
        JSONString* cast_type = nullptr;
        pString type_value;

        //type = cast_node->get()["type"];
        type = cast_node->get().get(pString("type"));

        cast_type = dynamic_cast<JSONString*>(type);
        if (cast_type == nullptr) {
            error = true;
            return nullptr;
        }

        type_value = cast_type->get();

        // get fields based on GeoJSONType
        if (type_value == "Feature" && (type_filter == 'f' || type_filter == 'a')) {
            geometry = process_single_field<JSONObject>(cast_node->get().get(pString("geometry")));
            properties = process_single_field<JSONObject>(cast_node->get().get(pString("properties")));
            //geometry = process_single_field<JSONObject>(cast_node->get()["geometry"]);
            //properties = process_single_field<JSONObject>(cast_node->get()["properties"]);

            GeoJSONType* tmp = convert_node(geometry, 'g');
            GeoJSONGeometry* cast_geom = dynamic_cast<GeoJSONGeometry*>(tmp);
            if (cast_geom == nullptr) {
                recursion_error = true;
            }			

            return new GJFeature(cast_geom, properties);
        }
        else if (type_value == "FeatureCollection" && (type_filter == 'c' || type_filter == 'a')) {
            features = process_single_field<JSONArray>(cast_node->get().get(pString("features")));
            //features = process_single_field<JSONArray>(cast_node->get()["features"]);

            // map onto feature objects recursively
            pArray<GJFeature*> tmp_vec = pArray<GJFeature*>();

            for (auto obj : features->get()) {
                GeoJSONType* tmp = convert_node(obj, 'f');
                GJFeature* cast_feature = dynamic_cast<GJFeature*>(tmp);
                if (cast_feature == nullptr) {
                    recursion_error = true;
                }
                else {
                    tmp_vec.push_back(cast_feature);
                }
            }

            return new GJFeatureCollection(tmp_vec);
        }
        else if (type_value == "Point" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJPoint>(cast_node);
        }
        else if (type_value == "MultiPoint" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJMultiPoint>(cast_node);
        }
        else if (type_value == "LineString" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJLineString>(cast_node);
        }
        else if (type_value == "MultiLineString" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJMultiLineString>(cast_node);
        }
        else if (type_value == "Polygon" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJPolygon>(cast_node);
        }
        else if (type_value == "MultiPolygon" && (type_filter == 'g' || type_filter == 'a')) {
            return process_basic_geometry<GJMultiPolygon>(cast_node);
        }
        else if (type_value == "GeometryCollection" && (type_filter == 'm' || type_filter == 'a')) {
            geometries = process_single_field<JSONArray>(cast_node->get().get(pString("geometries")));
            //geometries = process_single_field<JSONArray>(cast_node->get()["geometries"]);
            pArray<GeoJSONGeometry*> tmp_vec = pArray<GeoJSONGeometry*>();

            for (auto geom : geometries->get()) {
                GeoJSONType* tmp = convert_node(geom, 'g');
                GeoJSONGeometry* cast_geom = dynamic_cast<GeoJSONGeometry*>(tmp);
                if (cast_geom == nullptr) {
                    recursion_error = true;
                }
                else {
                    tmp_vec.push_back(cast_geom);
                }
            }

            return new GJGeometryCollection(tmp_vec);
        }
        
        // accessing this part means that type was incorrect format
        error = true;
        return nullptr;
    }

    template<class T>
    T* process_basic_geometry(JSONObject* cast_node){
        return new T(process_single_field<JSONArray>(cast_node->get().get(pString("coordinates"))));

        //return new T(process_single_field<JSONArray>(cast_node->get()["coordinates"]));
    }

    template<class T>
    T* process_single_field(JSONType* field) {
        T* cast_field = nullptr;

        cast_field = dynamic_cast<T*>(field);

        if (cast_field == nullptr) {
            error = true;
            return nullptr;
        }

        return cast_field;
    }

    // ----- VECTORIZATION -----

    void vectorize_node(GeoJSONType* node, JSONObject* property_vector) {
        char code = node->get_gj_code();

        if (code == 'f') {
            GJFeature* cast_node = dynamic_cast<GJFeature*>(node);
            
            if (cast_node == nullptr) {
                vectorization_error = true;
                return;
            }

            JSONObject* given_propr = cast_node->get_properties();

            vectorize_node(cast_node->get_geometry(), given_propr);
        }
        else if (code == 'c') {
            GJFeatureCollection* cast_node = dynamic_cast<GJFeatureCollection*>(node);

            if (cast_node == nullptr) {
                vectorization_error = true;
                return;
            }

            pArray<GJFeature*> feats_ptr = cast_node->get_features();

            for (auto feat : feats_ptr) {
                vectorize_node(feat, property_vector);
            }

        }
        else if (code == 'g') {
            GeoJSONGeometry* cast_node = dynamic_cast<GeoJSONGeometry*>(node);

            if (cast_node == nullptr) {
                vectorization_error = true;
                return;
            }

            JSONObject* property = property_vector;

            vectorized_data.push_back(new GJFeature(cast_node, property));
        }
        else if (code == 'm') {
            GJGeometryCollection* cast_node = dynamic_cast<GJGeometryCollection*>(node);

            if (cast_node == nullptr) {
                vectorization_error = true;
                return;
            }

            pArray<GeoJSONGeometry*> geoms_ptr = cast_node->get_geometries();

            for (auto geom : geoms_ptr) {
                vectorize_node(geom, property_vector);
            }
        }
    }
    
};
