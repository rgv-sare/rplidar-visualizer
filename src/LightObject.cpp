#include <LightObject.hpp>

using namespace em;

LightObject::LightObject(const std::string& name)
    : SceneObject(LIGHT, name)
    , color(1.0f, 1.0f, 1.0f)
    , intensity(1.0f)
{
}

LightObject::~LightObject()
{
}

void LightObject::update(float dt)
{
}

void LightObject::draw(Shader& shader)
{
}

void LightObject::setColor(const glm::vec3& color)
{
    this->color = color;
}

glm::vec3 LightObject::getColor() const
{
    return color;
}

void LightObject::setIntensity(float intensity)
{
    this->intensity = intensity;
}

float LightObject::getIntensity() const
{
    return intensity;
}

#define luaGetLightObject() \
    LightObject* lightObject; \
    luaPushValueFromKey("ptr", 1); \
    luaGetPointer(lightObject, LightObject, -1);

int LightObject::lua_this(lua_State* L)
{
    if(hasLuaInstance(L))
        return 1;

    lua_newtable(L);
    lua_pushstring(L, "ptr");
    lua_pushlightuserdata(L, this);
    lua_settable(L, -3);
    lua_pushstring(L, "transform");
    getTransform().lua_this(L);
    lua_settable(L, -3);
    lua_pushstring(L, "dynamics");
    getDynamics().lua_this(L);
    lua_settable(L, -3);

    luaL_getmetatable(L, "LightObject");
    lua_setmetatable(L, -2);

    luaRegisterInstance(L);

    return 1;
}

int LightObject::lua_openLightObjectLib(lua_State* L)
{
    static const luaL_Reg luaLightObjectMethods[] =
    {
        { "getColor", lua_getColor },
        { "getIntensity", lua_getIntensity },
        { "setColor", lua_setColor },
        { "setIntensity", lua_setIntensity },
        { NULL, NULL }
    };

    luaL_newmetatable(L, "LightObject");
    luaL_setfuncs(L, luaLightObjectMethods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_getmetatable(L, "SceneObject");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "LightObject");

    return 0;
}

int LightObject::lua_getColor(lua_State* L)
{
    luaGetLightObject();
    luaPushVec3(lightObject->color);

    return 1;
}

int LightObject::lua_getIntensity(lua_State* L)
{
    luaGetLightObject();
    lua_pushnumber(L, lightObject->intensity);

    return 1;
}

int LightObject::lua_setColor(lua_State* L)
{
    luaGetLightObject();
    luaGetVec3(lightObject->color, 2);

    return 0;
}

int LightObject::lua_setIntensity(lua_State* L)
{
    luaGetLightObject();
    lightObject->intensity = lua_tonumber(L, 2);

    return 0;
}