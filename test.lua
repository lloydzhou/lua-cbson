local ffi = require "ffi"
local mongo = require "mongo"
bson = require("cbson")
json = require("cjson")
print(bson._NAME)
print(bson._VERSION)
local b = mongo.bson('{"hello": "world"}')
print (b.to_json())
print (b)
print(mongo.bson('{"arr": [{"bbb": 1}, {"aaa": 2.1}]}'))

print(json.encode(bson.decode(mongo.bson('{"arr": {"bbb": "aaa", "ccc": 1, "ddd": 2.1, "undef": {"$aaa": "$sdf"}}}'))))
print(json.encode(bson.decode(mongo.bson('{"arr": ["bbb", "aaa", 1, 2,1, null, true, {"$oid": "54a3ef5369401b30e2cd0b98"}, {"$date": 1339582446756}]}'))))
print(json.encode(bson.decode(mongo.bson('{"hello": "world"}'))))

print(json.encode(bson.decode(mongo.bson())))
