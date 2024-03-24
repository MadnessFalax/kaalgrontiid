#pragma once

/**
* 
*	- bbox and foreign members are so far not implemented
* 
*/


class GeoJSONType
{
protected:
    char gj_code;

public:

    virtual ~GeoJSONType() {}

    virtual char get_gj_code() {
        return this->gj_code;
    }

    // delete this nodes pointed objects (shallow disposal)
    virtual void dispose() {}

    // deep disposal of GeoJSONType objects while leaving JSONType properties untouched (as they probably were deleted elsewhere)
    virtual void dispose_gj_deep() {}

    // delete this nodes pointed objects recursively
    virtual void dispose_deep() {}
};

// ----------------------------------------

class GeoJSONGeometry : public GeoJSONType {

protected:
    char gtype;
    bool multi;

public:

    GeoJSONGeometry() {

    }

    char get_gtype() {
        return this->gtype;
    }

    bool is_multi() {
        return this->multi;
    }

    virtual ~GeoJSONGeometry() {}
};

// ----------------------------------------

class GJFeature : public GeoJSONType {
    GeoJSONGeometry* geometry;
    JSONObject* properties;

public:
    GJFeature(GeoJSONGeometry* geom, JSONObject* props) {
        geometry = geom;
        properties = props;
        gj_code = 'f';
    }

    ~GJFeature() {
        geometry = nullptr;
        properties = nullptr;
    }

    GeoJSONGeometry* get_geometry() {
        return geometry;
    }

    void set_geometry(GeoJSONGeometry* new_geom) {
        geometry = new_geom;
    }

    JSONObject* get_properties() {
        return properties;
    }

    void set_properties(JSONObject* new_props) {
        properties = new_props;
    }

    void dispose() override {
        delete geometry;
        geometry = nullptr;
        delete properties;
        properties = nullptr;
    }

    void dispose_gj_deep() override {
        geometry->dispose_gj_deep();
        delete geometry;
        geometry = nullptr;
        properties = nullptr;
    }

    void dispose_deep() override {
        geometry->dispose_deep();
        delete geometry;
        geometry = nullptr;
        properties->dispose_deep();
        delete properties;
        properties = nullptr;
    }

    static GJFeature* copy(GJFeature* x);
};

// ----------------------------------------

class GJFeatureCollection : public GeoJSONType {
    pArray<GJFeature*> features;

public:
    GJFeatureCollection(pArray<GJFeature*> feats) {
        features = feats;
        gj_code = 'c';
    }

    ~GJFeatureCollection() {
        for (auto feat : features) {
            feat = nullptr;
        }
        features.clear();
    }

    pArray<GJFeature*> get_features() {
        return features;
    }

    void set_features(pArray<GJFeature*> new_feats) {
        features = new_feats;
    }

    void dispose() override {
        for (auto x : features) {
            delete x;
            x = nullptr;
        }
        features.clear();
    }

    void dispose_gj_deep() override {
        for (auto x : features) {
            x->dispose_gj_deep();
            delete x;
            x = nullptr;
        }
        features.clear();
    }

    void dispose_deep() override {
        for (auto x : features) {
            x->dispose_deep();
            delete x;
            x = nullptr;
        }
        features.clear();
    }

    static GJFeatureCollection* copy(GJFeatureCollection* x);
};

// ----------------------------------------

class GJPoint : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJPoint(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = false;
        gtype = 'p';
    }

    ~GJPoint() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJPoint* copy(GJPoint* x);
};

// ----------------------------------------

class GJMultiPoint : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJMultiPoint(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = true;
        gtype = 'p';
    }

    ~GJMultiPoint() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJMultiPoint* copy(GJMultiPoint* x);
};

// ----------------------------------------

class GJLineString : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJLineString(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = false;
        gtype = 's';
    }

    ~GJLineString() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJLineString* copy(GJLineString* x);
};

// ----------------------------------------

class GJPolygon : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJPolygon(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = false;
        gtype = 'y';
    }

    ~GJPolygon() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJPolygon* copy(GJPolygon* x);
};

// ----------------------------------------

class GJMultiLineString : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJMultiLineString(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = true;
        gtype = 's';
    }

    ~GJMultiLineString() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJMultiLineString* copy(GJMultiLineString* x);
};

// ----------------------------------------

class GJMultiPolygon : public GeoJSONGeometry {
    JSONArray* coordinates;

public:
    GJMultiPolygon(JSONArray* coord) {
        coordinates = coord;
        gj_code = 'g';
        multi = true;
        gtype = 'y';
    }

    ~GJMultiPolygon() {
        coordinates = nullptr;
    }

    JSONArray* get_coordinates() {
        return coordinates;
    }

    void set_coordinates(JSONArray* new_coords) {
        coordinates = new_coords;
    }

    void dispose() override {
        delete coordinates;
        coordinates = nullptr;
    }

    void dispose_gj_deep() override {
        coordinates = nullptr;
    }

    void dispose_deep() override {
        coordinates->dispose_deep();
        delete coordinates;
        coordinates = nullptr;
    }

    static GJMultiPolygon* copy(GJMultiPolygon* x);
};

// ----------------------------------------

class GJGeometryCollection : public GeoJSONGeometry {
    pArray<GeoJSONGeometry*> geometries;

public:
    GJGeometryCollection(pArray<GeoJSONGeometry*> geoms) {
        geometries = geoms;
        gj_code = 'm';

        // this is redundant but kept in for safety since there is inherited accessor
        gtype = 'm';
        multi = false;
    }

    ~GJGeometryCollection() {
        for (auto geom : geometries) {
            geom = nullptr;
        }
        geometries.clear();
    }

    pArray<GeoJSONGeometry*> get_geometries() {
        return geometries;
    }

    void set_geometries(pArray<GeoJSONGeometry*> new_geoms) {
        geometries = new_geoms;
    }

    void dispose() override {
        for (auto x : geometries) {
            delete x;
            x = nullptr;
        }
        geometries.clear();
    }

    void dispose_gj_deep() override {
        for (auto x : geometries) {
            x->dispose_gj_deep();
            delete x;
            x = nullptr;
        }
        geometries.clear();
    }

    void dispose_deep() override {
        for (auto x : geometries) {
            x->dispose_deep();
            delete x;
            x = nullptr;
        }
        geometries.clear();
    }

    static GJGeometryCollection* copy(GJGeometryCollection* x);
};

// ----------------------------------------

// return pointer to newly heap constructed deep copy object
GJFeature* GJFeature::copy(GJFeature* x) {
    GeoJSONGeometry* geom = x->geometry;


    switch (geom->get_gtype()) {
        case 'p': {
            if (geom->is_multi()) {
                GJMultiPoint* cast_geom = dynamic_cast<GJMultiPoint*>(geom);
                return new GJFeature(GJMultiPoint::copy(cast_geom), JSONObject::copy(x->properties));
            }
            else {
                GJPoint* cast_geom = dynamic_cast<GJPoint*>(geom);
                return new GJFeature(GJPoint::copy(cast_geom), JSONObject::copy(x->properties));
            }
            break;
        }
        case 's': {
            if (geom->is_multi()) {
                GJMultiLineString* cast_geom = dynamic_cast<GJMultiLineString*>(geom);
                return new GJFeature(GJMultiLineString::copy(cast_geom), JSONObject::copy(x->properties));
            }
            else {
                GJLineString* cast_geom = dynamic_cast<GJLineString*>(geom);
                return new GJFeature(GJLineString::copy(cast_geom), JSONObject::copy(x->properties));
            }
            break;
        }
        case 'y': {
            if (geom->is_multi()) {
                GJMultiPolygon* cast_geom = dynamic_cast<GJMultiPolygon*>(geom);
                return new GJFeature(GJMultiPolygon::copy(cast_geom), JSONObject::copy(x->properties));
            }
            else {
                GJPolygon* cast_geom = dynamic_cast<GJPolygon*>(geom);
                return new GJFeature(GJPolygon::copy(cast_geom), JSONObject::copy(x->properties));
            }
            break;
        }
        case 'm': {
            GJGeometryCollection* cast_geom = dynamic_cast<GJGeometryCollection*>(geom);
            return new GJFeature(GJGeometryCollection::copy(cast_geom), JSONObject::copy(x->properties));
        }
        default: {
            break;
        }
    }

    return nullptr;
}

// return pointer to newly heap constructed deep copy object
GJFeatureCollection* GJFeatureCollection::copy(GJFeatureCollection* x) {
    pArray<GJFeature*> tmp_feats = pArray<GJFeature*>();

    for (auto feat : x->features) {
        tmp_feats.push_back(GJFeature::copy(feat));
    }

    return new GJFeatureCollection(tmp_feats);
}

// return pointer to newly heap constructed deep copy object
GJPoint* GJPoint::copy(GJPoint* x) {
    return new GJPoint(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJMultiPoint* GJMultiPoint::copy(GJMultiPoint* x) {
    return new GJMultiPoint(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJLineString* GJLineString::copy(GJLineString* x) {
    return new GJLineString(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJPolygon* GJPolygon::copy(GJPolygon* x) {
    return new GJPolygon(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJMultiLineString* GJMultiLineString::copy(GJMultiLineString* x) {
    return new GJMultiLineString(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJMultiPolygon* GJMultiPolygon::copy(GJMultiPolygon* x) {
    return new GJMultiPolygon(JSONArray::copy(x->coordinates));
}

// return pointer to newly heap constructed deep copy object
GJGeometryCollection* GJGeometryCollection::copy(GJGeometryCollection* x) {
    pArray<GeoJSONGeometry*> tmp_vec = pArray<GeoJSONGeometry*>();
    
    for (auto geom : x->geometries) {
        char code = geom->get_gtype();
        switch (code) {
            case 'p': {
                if (geom->is_multi()) {
                    GJMultiPoint* cast_geom = dynamic_cast<GJMultiPoint*>(geom);
                    tmp_vec.push_back(GJMultiPoint::copy(cast_geom));
                }
                else {
                    GJPoint* cast_geom = dynamic_cast<GJPoint*>(geom);
                    tmp_vec.push_back(GJPoint::copy(cast_geom));
                }
                break;
            }
            case 's': {
                if (geom->is_multi()) {
                    GJMultiLineString* cast_geom = dynamic_cast<GJMultiLineString*>(geom);
                    tmp_vec.push_back(GJMultiLineString::copy(cast_geom));
                }
                else {
                    GJLineString* cast_geom = dynamic_cast<GJLineString*>(geom);
                    tmp_vec.push_back(GJLineString::copy(cast_geom));
                }
                break;
            }
            case 'y': {
                if (geom->is_multi()) {
                    GJMultiPolygon* cast_geom = dynamic_cast<GJMultiPolygon*>(geom);
                    tmp_vec.push_back(GJMultiPolygon::copy(cast_geom));
                }
                else {
                    GJPolygon* cast_geom = dynamic_cast<GJPolygon*>(geom);
                    tmp_vec.push_back(GJPolygon::copy(cast_geom));
                }
                break;
            }
            // geometry collection nesting is forbiden by standard
            default: {
                break;
            }
        }
    }

    return new GJGeometryCollection(tmp_vec);
}
