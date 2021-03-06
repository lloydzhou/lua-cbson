#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <lua.h>
#include <lauxlib.h>
#include <bson.h>

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "0.1devel"
#endif


/* ===== DECODING ===== */

typedef struct {
    uint32_t count;
    bool keys;
    uint32_t depth;
    lua_State *l;
} cbson_state;

static bool cbson_visit_document (const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data);
static bool cbson_visit_array (const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data);

static bool cbson_visit_before (const bson_iter_t *iter, const char *key, void *data){
    cbson_state *s = data;
    s->count++;
    if (s->keys)
        lua_pushstring(s->l, key);

    return false;
}

static bool cbson_visit_after (const bson_iter_t *iter, const char *key, void *data){
    cbson_state *s = data;
    if (s->keys)
        lua_rawset(s->l, -3);
    else
        lua_rawseti(s->l, -2, s->count);

    return false;
}

static bool cbson_visit_utf8 (const bson_iter_t *iter, const char *key, size_t v_utf8_len, const char *v_utf8, void *data){
    cbson_state *s = data;
    lua_pushlstring(s->l, v_utf8, v_utf8_len);
    return false;
}

static bool cbson_visit_int32 (const bson_iter_t *iter, const char *key, int32_t v_int32, void *data){
    cbson_state *s = data;
    lua_pushnumber(s->l, v_int32);
    return false;
}

static bool cbson_visit_int64 (const bson_iter_t *iter, const char *key, int64_t v_int64, void *data){
    cbson_state *s = data;
    lua_pushnumber(s->l, v_int64);
    return false;
}

static bool cbson_visit_double (const bson_iter_t *iter, const char *key, double v_d, void *data){
    cbson_state *s = data;
    lua_pushnumber(s->l, v_d);
    return false;
}

static bool cbson_visit_undefined (const bson_iter_t *iter, const char *key, void *data){
    cbson_state *s = data;
    lua_newtable(s->l);
    lua_pushlstring(s->l, "$undefined", 10);
    lua_pushboolean(s->l, true);
    lua_rawset(s->l, -3);
    return false;
}

static bool cbson_visit_null (const bson_iter_t *iter, const char *key, void *data){
    cbson_state *s = data;
    lua_pushnil(s->l);
    return false;
}

static bool cbson_visit_bool (const bson_iter_t *iter, const char *key, bool v_bool, void *data){
    cbson_state *s = data;
    lua_pushboolean(s->l, v_bool);
    return false;
}

static bool cbson_visit_oid (const bson_iter_t *iter, const char *key, const bson_oid_t *oid, void *data){
    cbson_state *s = data;
    char str[25];
    bson_return_val_if_fail (oid, false);
    bson_oid_to_string (oid, str);
    lua_newtable(s->l);
    lua_pushlstring(s->l, "$oid", 4);
    lua_pushlstring(s->l, str, 24);
    lua_rawset(s->l, -3);
    return false;
}

static bool cbson_visit_regex (const bson_iter_t *iter, const char *key, const  char *v_reg, const char *v_options, void *data){
    cbson_state *s = data;
    lua_newtable(s->l);
    lua_pushlstring(s->l, "$regex", 6);
    lua_pushstring(s->l, v_reg);
    lua_rawset(s->l, -3);
    lua_newtable(s->l);
    lua_pushlstring(s->l, "$options", 8);
    lua_pushstring(s->l, v_options);
    lua_rawset(s->l, -3);
    return false;
}

static bool cbson_visit_date_time (const bson_iter_t *iter, const char *key, int64_t msec_since_epoch, void *data){
    cbson_state *s = data;
    lua_newtable(s->l);
    lua_pushlstring(s->l, "$date", 5);
    lua_pushnumber(s->l, msec_since_epoch);
    lua_rawset(s->l, -3);
    return false;
}

static const bson_visitor_t cbson_visitors = {
       cbson_visit_before,
       cbson_visit_after,
       NULL, /* visit_corrupt */
       cbson_visit_double,
       cbson_visit_utf8,
       cbson_visit_document,
       cbson_visit_array,
       NULL, /* visit_binary */
       cbson_visit_undefined,
       cbson_visit_oid,
       cbson_visit_bool,
       cbson_visit_date_time,
       cbson_visit_null,
       cbson_visit_regex,
       NULL, /* visit_dbpointer */
       NULL, /* visit_code */
       NULL, /* visit_symbol */
       NULL, /* visit_codewscope */
       cbson_visit_int32,
       NULL, /* visit_timestamp */
       cbson_visit_int64,
       NULL, /* visit_maxkey */
       NULL, /* visit_minkey */

};

static bool cbson_visit_document (const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data){
    bson_iter_t iter_new;
    cbson_state *s = data;
    cbson_state cs = {0, true};
    if (bson_iter_init(&iter_new, v_document)){
        lua_newtable(s->l);
        cs.depth = s->depth + 1;
        cs.l = s->l;
        bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
    }
    else
        luaL_error(s->l, "error to init bson iter.");
    return false;
}

static bool cbson_visit_array (const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data){
    bson_iter_t iter_new;
    cbson_state *s = data;
    cbson_state cs = {0, false};
    if (bson_iter_init(&iter_new, v_array)){
        lua_newtable(s->l);
        cs.depth = s->depth + 1;
        cs.l = s->l;
        bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
    }
    else
        luaL_error(s->l, "error to init bson iter.");
    return false;
}


static int bson_decode(lua_State *l)
{
    bson_t *bson;
    cbson_state s = {0, true, 0};
    bson_iter_t iter;
    luaL_argcheck(l, lua_gettop(l) == 1, 1, "expected 1 argument");
    bson = *(bson_t **)lua_topointer(l, 1);

    lua_pushnil(l);
    if (bson_iter_init(&iter, bson)){
        lua_newtable(l);
        s.l = l;
        bson_iter_visit_all(&iter, &cbson_visitors, &s);
    }
    else
        luaL_error(l, "error to init bson iter.");

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
