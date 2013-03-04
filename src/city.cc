/*
 * GeoIP C library binding for nodejs
 *
 * Licensed under the GNU LGPL 2.1 license
 */

#include "city.h"

using namespace v8;
using namespace geoip;

void init(Handle<Object> target) {
    HandleScope scope;
    City::Init(target);
}

NODE_MODULE(geoip, init);

void City::Init(Handle<Object> target) {
    HandleScope scope;
    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    constructor_template = Persistent<FunctionTemplate>::New(t);
    constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template->SetClassName(String::NewSymbol("geoip"));
    NODE_SET_PROTOTYPE_METHOD(constructor_template, "lookupSync", lookupSync);
    target->Set(String::NewSymbol("City"), constructor_template->GetFunction());
}

Handle<Value> City::New(const Arguments& args) {
    HandleScope scope;
    City *city = new City();
    city->Wrap(args.This());
    String::Utf8Value file_str(args[0]->ToString());
    city->db = GeoIP_open(*file_str, GEOIP_MEMORY_CACHE);
    if (!city->db) {
        return ThrowException(String::New("Error: Cannot open database"));
    }
    int db_edition = GeoIP_database_edition(city->db);
    if (db_edition != GEOIP_CITY_EDITION_REV0 && db_edition != GEOIP_CITY_EDITION_REV1) {
        return ThrowException(String::New("Error: Not valid city database"));
    }
    return scope.Close(args.This());
}

Handle<Value> City::lookupSync(const Arguments &args) {
    HandleScope scope;
    City *city = ObjectWrap::Unwrap<City>(args.This());
    Local<Object> data = Object::New();
    String::Utf8Value host_str(args[0]->ToString());
    uint32_t ipnum = _GeoIP_lookupaddress(*host_str);
    if (!ipnum) {
        return scope.Close(Null());
    }
    GeoIPRecord *record = GeoIP_record_by_ipnum(city->db, ipnum);
    if (!record) {
        return scope.Close(Null());
    }
    if (record->country_code) {
        data->Set(String::NewSymbol("country_code"), String::New(record->country_code));
    }
    if (record->country_code3) {
        data->Set(String::NewSymbol("country_code3"), String::New(record->country_code3));
    }
    if (record->country_name) {
        data->Set(String::NewSymbol("country_name"), String::New(record->country_name));
    }
    if (record->region) {
        data->Set(String::NewSymbol("region"), String::New(record->region));
    }
    if (record->city) {
        char *city_str = _GeoIP_iso_8859_1__utf8(record->city);
        if (city_str) {
            data->Set(String::NewSymbol("city"), String::New(city_str));
            free(city_str);
        }
    }
    if (record->postal_code) {
        data->Set(String::NewSymbol("postal_code"), String::New(record->postal_code));
    }
    if (record->latitude >= -90 && record->latitude <= 90) {
        data->Set(String::NewSymbol("latitude"), Number::New(record->latitude));
    }
    if (record->longitude >= -180 && record->longitude <= 180) {
        data->Set(String::NewSymbol("longitude"), Number::New(record->longitude));
    }
    if (record->metro_code > 0) {
        data->Set(String::NewSymbol("metro_code"), Number::New(record->metro_code));
    }
    if (record->dma_code > 0) {
        data->Set(String::NewSymbol("dma_code"), Number::New(record->dma_code));
    }
    if (record->area_code > 0) {
        data->Set(String::NewSymbol("area_code"), Number::New(record->area_code));
    }
    if (record->continent_code > 0) {
        data->Set(String::NewSymbol("continent_code"), String::New(record->continent_code));
    }
    if (record->country_code != NULL && record->region != NULL) {
        const char *time_zone = GeoIP_time_zone_by_country_and_region(record->country_code, record->region);
        if (time_zone != NULL) {
            data->Set(String::NewSymbol("time_zone"), String::New(time_zone));
        }
    }
    GeoIPRecord_delete(record);
    return scope.Close(data);
}
