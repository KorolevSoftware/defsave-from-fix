#define LIB_NAME "HTML5_storage"
#define MODULE_NAME "html5_storage"
// include the Defold SDK
#include <dmsdk/sdk.h>


#ifdef __EMSCRIPTEN__  // HTML5
#include <emscripten.h>

EM_JS(char*, get_local_storage, (const char* path), {
    let load_data = null;
    if (typeof wx !== 'undefined') { // weechat
        load_data = wx.getStorageSync(UTF8ToString(path));
    } else { // simple browser
        load_data = window.localStorage.getItem(UTF8ToString(path));
    }
    return stringToNewUTF8(load_data || '{}');
});

EM_JS(void, set_local_storage, (const char* path, const char* data), {
    if (typeof wx !== 'undefined') { // weechat
        wx.setStorageSync(UTF8ToString(path), UTF8ToString(data));
    } else {
        window.localStorage.setItem(UTF8ToString(path), UTF8ToString(data));
    }
});

static int Lua_get_local_storage(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    char* path = (char*)luaL_checkstring(L, 1);
    lua_pushstring(L, get_local_storage(path));
    return 1;
}

static int Lua_set_local_storage(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);
    char* path = (char*)luaL_checkstring(L, 1);
    char* data = (char*)luaL_checkstring(L, 2);
    set_local_storage(path, data);
    return 0;
}


// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
    {"get", Lua_get_local_storage},
    {"set", Lua_set_local_storage},
    {0, 0}
};

static void LuaInit(lua_State* L) {
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

#endif

static dmExtension::Result InitializeMyExtension(dmExtension::Params* params) {
    // Init Lua
    #ifdef __EMSCRIPTEN__ 
        LuaInit(params->m_L);
        dmLogInfo("Registered %s Extension", MODULE_NAME);
    #else
        dmLogInfo("Registered %s Extension unsupported this platform", MODULE_NAME);
    #endif
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(HTML5_storage, LIB_NAME, NULL, NULL, InitializeMyExtension, NULL, NULL, NULL)
