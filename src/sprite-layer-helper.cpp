#include "sprite-layer-helper.h"
#include "boot-scene.h"
#include <cmath>

LayeredSprite createLayeredSprite(float x, float y, const std::string& frameName, int depth, bool visible) {
    LayeredSprite ls;
    const AtlasFrame* frame = findAtlasFrame(frameName);
    if (!frame) {
        ls.visible = false;
        return ls;
    }

    ls.frameName = frameName;
    ls.x = x;
    ls.y = y;
    ls.depth = depth;
    ls.visible = visible;

    if (BootScene::textCache.find("GJ_WebSheetJson") != BootScene::textCache.end()) {
        const std::string& json = BootScene::textCache["GJ_WebSheetJson"];
        size_t fnPos = json.find("\"" + frameName + "\"");
        if (fnPos != std::string::npos) {
            size_t nextPos = json.find(".png\"", fnPos + frameName.size() + 2);
            size_t chunkLen = (nextPos != std::string::npos) ? (nextPos - fnPos) : 1000;
            std::string chunk = json.substr(fnPos, chunkLen);

            auto getChunkVal = [&](const std::string& key, size_t start) -> float {
                size_t k = chunk.find("\"" + key + "\"", start);
                if (k == std::string::npos) return 0.0f;
                size_t colon = chunk.find(":", k);
                if (colon == std::string::npos) return 0.0f;
                try { return std::stof(chunk.substr(colon + 1)); } catch (...) { return 0.0f; }
            };

            size_t gjPos = chunk.find("\"gjSpriteOffset\"");
            if (gjPos != std::string::npos) {
                ls.offsetX = getChunkVal("x", gjPos);
                ls.offsetY = -getChunkVal("y", gjPos);
            } else {
                size_t sssPos = chunk.find("\"spriteSourceSize\"");
                size_t ssPos  = chunk.find("\"sourceSize\"");

                if (sssPos != std::string::npos && ssPos != std::string::npos) {
                    float offX = getChunkVal("x", sssPos);
                    float offY = getChunkVal("y", sssPos);
                    float srcW = getChunkVal("w", ssPos);
                    float srcH = getChunkVal("h", ssPos);

                    if (srcW > 0.0f && srcH > 0.0f) {
                        ls.offsetX = (offX + frame->w * 0.5f) - (srcW * 0.5f);
                        ls.offsetY = (offY + frame->h * 0.5f) - (srcH * 0.5f);
                    }
                }
            }
        }
    }

    if (AtlasManager::atlasScale > 0.0f) {
        ls.offsetX /= AtlasManager::atlasScale;
        ls.offsetY /= AtlasManager::atlasScale;
    }

    return ls;
}

void LayeredSprite::render(float extraOffsetX, float extraOffsetY) {
    if (!visible || frameName.empty()) return;

    applyBlendMode(blend);
    const AtlasFrame* af = findAtlasFrame(frameName);
    float drawW = (af ? af->w : 0.0f) * scaleX;
    float drawH = (af ? af->h : 0.0f) * scaleY;

    float rad = rotation * 0.0174532925f;
    float cosR = std::cos(rad);
    float sinR = std::sin(rad);
    float rx = (offsetX * cosR - offsetY * sinR) * scaleX;
    float ry = (offsetX * sinR + offsetY * cosR) * scaleY;

    drawAtlasFrame(frameName, x + extraOffsetX + rx, y + extraOffsetY + ry, drawW, drawH, rotation, r, g, b, a);
    applyBlendMode(BLEND_NORMAL);
}
