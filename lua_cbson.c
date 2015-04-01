#include <assert.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <lua.h>
#include <lauxlib.h>

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "0.1devel"
#endif


/* ===== DECODING ===== */

static int bson_decode(lua_State *l)
{

    luaL_argcheck(l, lua_gettop(l) == 1, 1, "expected 1 argument");
    lua_pushlstring(l, "hello", 5);

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
