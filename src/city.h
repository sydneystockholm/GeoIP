 /*
 * GeoIP C library binding for nodejs
 *
 * Licensed under the GNU LGPL 2.1 license
 */

#ifndef NODE_GEOIP_CITY_H
#define NODE_GEOIP_CITY_H

#include <v8.h>
#include <node.h>
#include <GeoIPCity.h>

extern "C" GEOIP_API unsigned long _GeoIP_lookupaddress (const char *host);

namespace geoip {
  class City: public node::ObjectWrap {
    protected:
      GeoIP *db;
      static v8::Persistent<v8::FunctionTemplate> constructor_template;
      static v8::Handle<v8::Value> New(const v8::Arguments& args);
    public:
      static void Init(v8::Handle<v8::Object> target);
      static v8::Handle<v8::Value> lookupSync(const v8::Arguments &args);
      static void close(const v8::Arguments &args);
      City() : db(NULL) {}
      ~City() {
        if (db) GeoIP_delete(db);
      }
  };

  v8::Persistent<v8::FunctionTemplate> City::constructor_template;
}

#endif
