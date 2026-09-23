#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL2/SDL_opengl.h>
#include "constants.h"

struct AtlasFrame {
    std::string name;
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    float u0 = 0.0f;
    float v0 = 0.0f;
    float u1 = 0.0f;
    float v1 = 0.0f;
};

class LevelObject {
public:
    std::string type;
    float x;
    float y;
    float w;
    float h;
    bool activated;

    int id = 0;
    float rotation = 0.0f;
    bool flipX = false;
    bool flipY = false;
    float portalY = 0.0f;

    LevelObject(const std::string& type = "", float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f)
    : type(type), x(x), y(y), w(w), h(h), activated(false) {}
};

class AtlasManager {
public:
    static std::unordered_map<std::string, AtlasFrame> frames;
    static float atlasScale;
    static void loadAtlasJson(const std::string& jsonContent, int texW, int texH);
    static const AtlasFrame* findAtlasFrame(const std::string& frameName);
};

const AtlasFrame* findAtlasFrame(const std::string& frameName);

void drawAtlasFrame(const std::string& frameName, float x, float y,
                    float w = 0.0f, float h = 0.0f, float rotation = 0.0f,
                    float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                    bool flipX = false, bool flipY = false);

void drawScale9(const std::string& textureKey, float x, float y, float w, float h, float cornerSize,
                float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
