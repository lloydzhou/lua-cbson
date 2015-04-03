# lua-cbson
lua cbson library based on [libbson](https://github.com/mongodb/libbson.git).
Can using in ngx_lua_module.
Suggest to using this library with [luajit-mongo](https://github.com/lloydzhou/luajit-mongo.git)
For now just implementation the "decode" method, can decode the bson_t struct into lua table.

# install
~~~bash
git clone https://github.com/lloydzhou/lua-cbson.git
cd lua-cbson
git submodule init
git submodule update

cd libbson
./autogen.sh
make && sudo make install

cd ..
make clean && make LUA_INCLUDE_DIR=/usr/local/openresty/luajit/include/luajit-2.1 LUA_CMODULE_DIR=/usr/local/openresty/lualib LUA_MODULE_DIR=/usr/local/openresty/lualib CBSON_CFLAGS="-g -fpic -I/usr/local/include/libbson-1.0/ " CC=cc

~~~
