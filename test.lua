--local ffi = require "ffi"
--local libbson = ffi.load("libbson-1.0.so")
print (os.getenv("LD_LIBRARY_PATH"))
bson = require("cbson")
print(bson)
print(bson._NAME)
print(bson._VERSION)
print(bson and bson.decode and bson.decode("sdfsd"))
