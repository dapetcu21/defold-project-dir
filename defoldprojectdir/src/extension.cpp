// myextension.cpp
// Extension lib defines
#define LIB_NAME "defoldprojectdir"
#define MODULE_NAME "defoldprojectdir"

// include the Defold SDK
#include <dmsdk/sdk.h>

#ifndef _WIN32
#include <libgen.h>
#else
#include <Shlwapi.h>
#endif

#include <string.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <linux/limits.h>
#endif

#if defined(_WIN32)
    #define SEP "\\"
    #define SEPCH '\\'
#else
    #define SEP "/"
    #define SEPCH '/'
#endif

static char * getExecutablePath() {
    char *exePath = NULL;

    #ifdef __APPLE__
    uint32_t bufsize = 0;
    _NSGetExecutablePath(NULL, &bufsize);
    exePath = (char*)malloc(bufsize);
    _NSGetExecutablePath(exePath, &bufsize);

    #elif defined(__linux__)
    exePath = (char*)malloc(PATH_MAX + 2);
    ssize_t ret = readlink("/proc/self/exe", exePath, PATH_MAX + 2);
    if (ret >= 0 && ret <= PATH_MAX + 1) {
      exePath[ret] = 0;
    } else {
      exePath[0] = 0;
    }

    #elif defined(_WIN32)
    exePath = (char*)malloc(MAX_PATH);
    size_t ret = GetModuleFileNameA(GetModuleHandle(NULL), exePath, MAX_PATH);
    if (ret <= 0 || ret >= MAX_PATH) {
        exePath[0] = 0;
    }
    #endif

    return exePath;
}

static int GetExecutablePath(lua_State* L) {
    char *exePath = getExecutablePath();
    if (exePath && exePath[0]) {
        lua_pushstring(L, exePath);
    } else {
        lua_pushnil(L);
    }
    free(exePath);
    return 1;
}

#if defined(DM_PLATFORM_OSX) || defined(DM_PLATFORM_WINDOWS) || defined(DM_PLATFORM_LINUX)

#ifdef _WIN32
static char * dirname(char * path) {
    size_t i = strlen(path);
    do {
        i -= 1;
        if (path[i] == SEPCH) {
            path[i] = 0;
            break;
        }
    } while (i);
    return path;
}
#endif

static bool endsIn(const char * haystack, const char * needle) {
    size_t needleLen = strlen(needle);
    size_t haystackLen = strlen(haystack);
    return (haystackLen >= needleLen && 0 == strcmp(needle, haystack + haystackLen - needleLen));
}

static char * getProjectDir() {
    char *exePath = getExecutablePath();

    // Detect if the game is running in the editor

    #if defined(__APPLE__)
    #define FMB_PLATFORM "osx"
    #define FMB_PLATFORM_ALT "darwin"
    #define FMB_EXT ""
    #elif defined(__linux__)
    #define FMB_PLATFORM "linux"
    #define FMB_EXT ""
    #elif defined(_WIN32)
    #define FMB_PLATFORM "win32"
    #define FMB_EXT ".exe"
    #endif

    #if defined(__x86_64__) || defined(_M_X64)
    #define FMB_ARCH "x86_64"
    #elif defined(__i386) || defined(_M_IX86)
    #define FMB_ARCH "x86"
    #endif

    #if defined(FMB_PLATFORM) && defined(FMB_ARCH)
    #define FMB_EDITOR_SUFFIX SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM SEP "dmengine" FMB_EXT

    #ifdef FMB_PLATFORM_ALT
    #define FMB_EDITOR_SUFFIX_ALT SEP "build" SEP FMB_ARCH "-" FMB_PLATFORM_ALT SEP "dmengine" FMB_EXT
    if (endsIn(exePath, FMB_EDITOR_SUFFIX) || endsIn(exePath, FMB_EDITOR_SUFFIX_ALT)) {
    #else
    if (endsIn(exePath, FMB_EDITOR_SUFFIX)) {
    #endif
        char* projPath = (char*)dirname(dirname(dirname(exePath)));
        return projPath;
    }
    #endif

    free(exePath);
    return NULL;
}

#else

static char * getProjectDir() {
  return NULL;
}

#endif

static int GetProjectDir(lua_State* L) {
    char *projPath = getProjectDir();
    if (projPath && projPath[0]) {
        lua_pushstring(L, projPath);
    } else {
        lua_pushnil(L);
    }
    free(projPath);
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"get_project_directory", GetProjectDir},
    {"get_executable_path", GetExecutablePath},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pushstring(L, SEP);
    lua_setfield(L, -2, "PATH_SEP");

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeMyExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeMyExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result OnUpdateMyExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

void OnEventMyExtension(dmExtension::Params* params, const dmExtension::Event* event)
{
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// MyExtension is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(defoldprojectdir, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, OnUpdateMyExtension, OnEventMyExtension, FinalizeMyExtension)
