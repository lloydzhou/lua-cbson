#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <lua.h>
#include <lauxlib.h>
/*#include "libbson/src/bson/bson.h"*/
#include <bson.h>

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "0.1devel"
#endif


/* ===== DECODING ===== */

static bool cbson_visit_document (const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data);
static bool cbson_visit_array (const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data);

static bool cbson_visit_utf8 (const bson_iter_t *iter, const char *key, size_t v_utf8_len, const char *v_utf8, void *data){
    return false;
}

static const bson_visitor_t cbson_visitors = {
       NULL, /* visit_before */
       NULL, /* visit_after */
       NULL, /* visit_corrupt */
       NULL, /* visit_double */
       cbson_visit_utf8,
       cbson_visit_document,
       cbson_visit_array,
       NULL, /* visit_binary */
       NULL, /* visit_undefined */
       NULL, /* visit_oid */
       NULL, /* visit_bool */
       NULL, /* visit_date_time */
       NULL, /* visit_null */
       NULL, /* visit_regex */
       NULL, /* visit_dbpointer */
       NULL, /* visit_code */
       NULL, /* visit_symbol */
       NULL, /* visit_codewscope */
       NULL, /* visit_int32 */
       NULL, /* visit_timestamp */
       NULL, /* visit_int64 */
       NULL, /* visit_maxkey */
       NULL, /* visit_minkey */

};

static bool cbson_visit_document (const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data){
    if (bson_iter_init(iter, v_document)){
        bson_iter_visit_all(iter, &cbson_visitors, data);
    }
    return false;
}

static bool cbson_visit_array (const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data){
    if (bson_iter_init(iter, v_array)){
        bson_iter_visit_all(iter, &cbson_visitors, data);
    }
    return false;
}


static int bson_decode(lua_State *l)
{
    bson_t *bson;
    bson_iter_t iter;
    luaL_argcheck(l, lua_gettop(l) == 1, 1, "expected 1 argument");
    bson = lua_topointer(l, 1);
    lua_pushlstring(l, "hello", 5);
    
    if (bson_iter_init(&iter, bson)){
        /*bson_iter_visit_all(&iter, &cbson_visitors, l);*/
    }

    return 1;
}

/* ===== INITIALISATION ===== */

#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
/* Compatibility for Lua 5.1.
 *
 * luaL_setfuncs() is used to create a module table where the functions have
 * json_config_t as their first upvalue. Code borrowed from Lua 5.2 source. */
static void luaL_setfuncs (lua_State *l, const luaL_Reg *reg, int nup)
{
    int i;

    luaL_checkstack(l, nup, "too many upvalues");
    for (; reg->name != NULL; reg++) {  /* fill the table with given functions */
        for (i = 0; i < nup; i++)  /* copy upvalues to the top */
            lua_pushvalue(l, -nup);
        lua_pushcclosure(l, reg->func, nup);  /* closure with those upvalues */
        lua_setfield(l, -(nup + 2), reg->name);
    }
    lua_pop(l, nup);  /* remove upvalues */
}
#endif

int luaopen_cbson(lua_State *l)
{
    luaL_Reg reg[] = {
        { "decode", bson_decode },
        { NULL, NULL }
    };

    /* cbson module table */
    lua_newtable(l);

    /* Register functions with config data as upvalue */
    luaL_setfuncs(l, reg, 0);
    /*lua_register(l, "decode", bson_decode);*/

    /* Set module name / version fields */
    lua_pushliteral(l, CBSON_MODNAME);
    lua_setfield(l, -2, "_NAME");
    lua_pushliteral(l, CBSON_VERSION);
    lua_setfield(l, -2, "_VERSION");


#ifdef ENABLE_CBSON_GLOBAL
    /* Register a global "cbson" table. * /
    lua_pushvalue(l, -1);
    lua_setglobal(l, CBSON_MODNAME);*/
#endif

    /* Return cbson table */
    return 1;
}

/* vi:ai et sw=4 ts=4:
 */
