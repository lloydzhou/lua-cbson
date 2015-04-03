--local ffi = require "ffi"
--local libbson = ffi.load("libbson-1.0.so")
local mongo = require "mongo"
print (os.getenv("LD_LIBRARY_PATH"))
bson = require("cbson")
json = require("cjson")
--print(json.encode(bson))
print(bson._NAME)
print(bson._VERSION)
local b = mongo.bson('{"hello": "world"}')
print (b.to_json())

print(bson.decode(mongo.bson('{"arr": {"bbb": "aaa", "ccc": 1, "ddd": 2.1, "undef": {"$aaa": "$sdf"}}}')))
print(mongo.bson('{"arr": [{"bbb": 1}, {"aaa": 2.1}]}'))
print(bson.decode(mongo.bson('{"arr": ["bbb", "aaa", 1, 2,1, null, true, {"$oid": "54a3ef5369401b30e2cd0b98"}, {"$date": 1339582446756}]}')))
print(bson.decode(mongo.bson('{"hello": "world"}')))

--print(json.encode(bson.decode(mongo.bson())))
