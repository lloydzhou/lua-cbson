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

print(bson.decode(mongo.bson('{"arr": {"bbb": "aaa"}}')))
print(bson.decode(mongo.bson('{"hello": "world"}')))

--print(json.encode(bson.decode(mongo.bson())))
