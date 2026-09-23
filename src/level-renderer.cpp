#include "level-renderer.h"
#include "boot-scene.h"
#include <algorithm>
#include <iostream>
#include <cmath>

// (Reemplaza únicamente la función getPortalOffset al inicio de level-renderer.cpp)
static void getPortalOffset(const std::string& frameName, float& outDx, float& outDy) {
    outDx = 0.0f;
    outDy = 0.0f;

    if (BootScene::textCache.find("GJ_WebSheetJson") != BootScene::textCache.end()) {
        const std::string& json = BootScene::textCache["GJ_WebSheetJson"];
        size_t fnPos = json.find("\"" + frameName + "\"");
        if (fnPos == std::string::npos && frameName.size() > 4) {
            fnPos = json.find("\"" + frameName.substr(0, frameName.size() - 4) + "\"");
        }
        if (fnPos != std::string::npos) {
            size_t nextPos = json.find(".png\"", fnPos + frameName.size() + 2);
            size_t chunkLen = (nextPos != std::string::npos) ? (nextPos - fnPos) : 1000;
            std::string chunk = json.substr(fnPos, chunkLen);

            auto getChunkVal = [&](const std::string& key, size_t start = 0) -> float {
                size_t k = chunk.find("\"" + key + "\"", start);
                if (k == std::string::npos) return 0.0f;
                size_t colon = chunk.find(":", k);
                if (colon == std::string::npos) return 0.0f;
                try { return std::stof(chunk.substr(colon + 1)); } catch (...) { return 0.0f; }
            };

            size_t gjPos = chunk.find("\"gjSpriteOffset\"");
            if (gjPos != std::string::npos) {
                outDx = getChunkVal("x", gjPos);
                outDy = -getChunkVal("y", gjPos);
            } else {
                size_t sssPos = chunk.find("\"spriteSourceSize\"");
                size_t ssPos  = chunk.find("\"sourceSize\"");
                size_t fPos   = chunk.find("\"frame\"");

                if (sssPos != std::string::npos && ssPos != std::string::npos && fPos != std::string::npos) {
                    float sssX   = getChunkVal("x", sssPos);
                    float sssY   = getChunkVal("y", sssPos);
                    float srcW   = getChunkVal("w", ssPos);
                    float srcH   = getChunkVal("h", ssPos);
                    float frameW = getChunkVal("w", fPos);
                    float frameH = getChunkVal("h", fPos);

                    if (srcW > 0.0f && srcH > 0.0f) {
                        outDx = (sssX + frameW * 0.5f) - (srcW * 0.5f);
                        outDy = (sssY + frameH * 0.5f) - (srcH * 0.5f);
                    }
                }
            }
        }
    }

    if (AtlasManager::atlasScale > 0.0f) {
        outDx /= AtlasManager::atlasScale;
        outDy /= AtlasManager::atlasScale;
    }

    if (outDx == 0.0f && outDy == 0.0f) {
        if (frameName.find("_front_") != std::string::npos) {
            outDx = 14.0f;
        } else if (frameName.find("_back_") != std::string::npos) {
            outDx = -14.0f;
        }
    }
}

static void applyTransformOffset(float& dx, float& dy, float scale, bool flipX, bool flipY, float rotDeg) {
    dx *= scale;
    dy *= scale;
    if (flipX) dx = -dx;
    if (flipY) dy = -dy;
    if (rotDeg != 0.0f) {
        float rad = rotDeg * 0.0174532925f;
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);
        float rx = dx * cosR - dy * sinR;
        float ry = dx * sinR + dy * cosR;
        dx = rx;
        dy = ry;
    }
}

LevelRenderer::LevelRenderer() {
    _buildGround();
}

LevelRenderer::~LevelRenderer() {}

void LevelRenderer::_buildGround() {
    const AtlasFrame* frame = findAtlasFrame("groundSquare_01_001.png");
    _tileW = frame ? frame->w : 1012.0f;

    int tileCount = (int)std::ceil((float)screenWidth / _tileW) + 2;
    float startX = -groundYOffset;

    _groundWorldX.clear();
    for (int i = 0; i < tileCount; ++i) {
        _groundWorldX.push_back(startX + i * _tileW);
    }
    _maxGroundWorldX = startX + (tileCount - 1) * _tileW;
}

void LevelRenderer::resetGroundState() {
    _flyGroundActive = false;
    _groundTargetValue = 0.0f;
    _groundAnimating = false;
    flyCameraTarget = -1.0f;
    _flyFloorY = 0.0f;
    _flyCeilingY = 0.0f;
    _groundStartScreenY = flipY(0.0f);
    _ceilingStartScreenY = 0.0f;
    _endPortalGameY = 240.0f;
    _vortexParticles.clear();
}

void LevelRenderer::setGroundColor(float r, float g, float b) {
    _groundR = r;
    _groundG = g;
    _groundB = b;
}

void LevelRenderer::setFlyMode(bool active, float playerY) {
    if (active) {
        float floorY = std::floor((playerY - 300.0f) / (float)baseUnit) * baseUnit;
        if (floorY < 0.0f) floorY = 0.0f;
        _flyFloorY = floorY;
        _flyCeilingY = floorY + physicsConst600;
        _flyGroundActive = true;

        float targetCam = (_flyFloorY + 300.0f) - 320.0f + unusedConst180;
        flyCameraTarget = (targetCam < 0.0f) ? 0.0f : targetCam;

        _groundStartScreenY = flipY(0.0f) + _lastCameraY;
        _ceilingStartScreenY = 0.0f;

        _groundAnimFrom = _groundTargetValue;
        _groundAnimTo = 1.0f;
        _groundAnimTime = 0.0f;
        _groundAnimDuration = 0.5f;
        _groundAnimating = true;
    } else {
        flyCameraTarget = -1.0f;
        _groundAnimFrom = _groundTargetValue;
        _groundAnimTo = 0.0f;
        _groundAnimTime = 0.0f;
        _groundAnimDuration = 0.5f;
        _groundAnimating = true;
    }
}

void LevelRenderer::updateEndPortalY(float cameraY, bool isFlying) {
    float targetY = 140.0f + cameraY;
    _endPortalGameY = isFlying ? targetY : std::max(240.0f, targetY);
}

void LevelRenderer::stepGroundAnimation(float dt) {
    if (!_groundAnimating) return;
    _groundAnimTime += dt;
    float t = (_groundAnimDuration > 0.0f) ? std::min(_groundAnimTime / _groundAnimDuration, 1.0f) : 1.0f;
    _groundTargetValue = _groundAnimFrom + (_groundAnimTo - _groundAnimFrom) * t;

    if (t >= 1.0f) {
        _groundAnimating = false;
        _groundTargetValue = _groundAnimTo;
        if (_groundAnimTo == 0.0f) {
            _flyGroundActive = false;
        }
    }
}

float LevelRenderer::getFloorY() const {
    return _flyGroundActive ? _flyFloorY : 0.0f;
}

float LevelRenderer::getCeilingY() const {
    return _flyGroundActive ? _flyCeilingY : 100000.0f;
}

void LevelRenderer::_updateEndPortalVortex(float dt) {
    if (endXPos <= 0.0f) return;

    _vortexTimer += dt;
    while (_vortexTimer >= 0.015f) {
        _vortexTimer -= 0.015f;
        if (_vortexParticles.size() >= 100) break;

        float angle = ((85.0f + 190.0f * ((rand() % 1000) / 1000.0f)) * 3.14159265f) / 180.0f;
        float dist = 320.0f + 80.0f * (((rand() % 1000) / 500.0f) - 1.0f);

        PortalVortexParticle vp;
        vp.x = (endXPos - 30.0f) + std::cos(angle) * dist;
        vp.y = flipY(_endPortalGameY) + std::sin(angle) * dist;

        float toPortalX = (endXPos - 30.0f) - vp.x;
        float toPortalY = flipY(_endPortalGameY) - vp.y;
        float d = std::sqrt(toPortalX * toPortalX + toPortalY * toPortalY);
        if (d < 1.0f) d = 1.0f;

        vp.maxLife = (200.0f + (rand() % 801)) / 1000.0f;
        vp.life = 0.0f;
        float speed = (d - 20.0f) / vp.maxLife;
        vp.vx = (toPortalX / d) * speed;
        vp.vy = (toPortalY / d) * speed;
        vp.scale = 0.75f;

        _vortexParticles.push_back(vp);
    }

    for (auto& vp : _vortexParticles) {
        vp.life += dt;
        vp.x += vp.vx * dt;
        vp.y += vp.vy * dt;
    }
    _vortexParticles.erase(
        std::remove_if(_vortexParticles.begin(), _vortexParticles.end(), [](const PortalVortexParticle& vp) {
            return vp.life >= vp.maxLife;
        }),
        _vortexParticles.end()
    );
}

void LevelRenderer::updateGroundTiles(float cameraX, float cameraY) {
    _lastCameraY = cameraY;

    for (size_t i = 0; i < _groundWorldX.size(); ++i) {
        if (_groundWorldX[i] + _tileW <= cameraX) {
            _groundWorldX[i] = _maxGroundWorldX + _tileW;
            _maxGroundWorldX = _groundWorldX[i];
        }
    }

    _updateEndPortalVortex(1.0f / 60.0f);
}

void LevelRenderer::_addToSection(const VisualSprite& sprite) {
    size_t sec = (size_t)std::max(0, (int)std::floor(sprite.worldX / 400.0f));
    if (sec >= _sections.size()) _sections.resize(sec + 1);
    _sections[sec].push_back(sprite);
}

void LevelRenderer::_addCollisionToSection(size_t objIndex, float worldX) {
    size_t sec = (size_t)std::max(0, (int)std::floor(worldX / 400.0f));
    if (sec >= _collisionSections.size()) _collisionSections.resize(sec + 1);
    _collisionSections[sec].push_back(objIndex);
}

std::string LevelRenderer::_getGlowFrameName(const std::string& frame) {
    size_t idx = frame.rfind("_001.png");
    if (idx != std::string::npos) {
        std::string res = frame;
        res.replace(idx, 8, "_glow_001.png");
        return res;
    }
    return "";
}

void LevelRenderer::_addGlowSprite(float x, float y, const std::string& frame, const LevelObjectRaw& raw, float worldX) {
    std::string glowName = _getGlowFrameName(frame);
    if (glowName.empty() || !findAtlasFrame(glowName)) return;

    float gdx = 0.0f, gdy = 0.0f;
    getPortalOffset(glowName, gdx, gdy);
    applyTransformOffset(gdx, gdy, raw.scale, raw.flipX, raw.flipY, raw.rot);

    const AtlasFrame* af = findAtlasFrame(glowName);
    VisualSprite s;
    s.frame = glowName;
    s.x = x + gdx;
    s.y = y + gdy;
    s.w = af ? af->w : 60.0f;
    s.h = af ? af->h : 60.0f;
    s.baseX = s.x;
    s.baseY = s.y;
    s.worldX = worldX + gdx;
    s.flipX = raw.flipX;
    s.flipY = raw.flipY;
    s.rotation = raw.rot;
    s.scaleX = raw.scale;
    s.scaleY = raw.scale;
    s.blend = BLEND_ADD;
    s.layer = 0;
    s.r = 1.0f;
    s.g = 1.0f;
    s.b = 1.0f;
    s.a = 0.75f;

    _addToSection(s);
}

void LevelRenderer::loadLevel(const std::string& levelStr) {
    ParsedLevel parsed = PakoCompression::helperFn17(levelStr);
    _spawnLevelObjects(parsed.objects);
}

void LevelRenderer::_spawnLevelObjects(const std::vector<LevelObjectRaw>& rawObjects) {
    objects.clear();
    objects.reserve(rawObjects.size() + 100);

    _sections.clear();
    _collisionSections.clear();
    _colorTriggers.clear();
    _enterEffectTriggers.clear();

    float lastX = 0.0f;

    for (const auto& item : rawObjects) {
        const ObjectDefinition* def = PakoCompression::helperFn18(item.id);
        if (!def) continue;

        float objX = 2.0f * item.x;
        float objY = 2.0f * item.y;
        if (objX > lastX) lastX = objX;

        if (def->type == "trigger") {
            if (item.id == 29 || item.id == 30) {
                ColorTrigger ct;
                ct.x = objX;
                ct.index = (item.id == 29) ? 1000 : 1001;
                auto itR = item.rawMap.find(7); ct.r = (itR != item.rawMap.end()) ? std::stof(itR->second) / 255.0f : 1.0f;
                auto itG = item.rawMap.find(8); ct.g = (itG != item.rawMap.end()) ? std::stof(itG->second) / 255.0f : 1.0f;
                auto itB = item.rawMap.find(9); ct.b = (itB != item.rawMap.end()) ? std::stof(itB->second) / 255.0f : 1.0f;
                auto itD = item.rawMap.find(10); ct.duration = (itD != item.rawMap.end()) ? std::stof(itD->second) : 0.0f;
                auto itTG = item.rawMap.find(14); ct.tintGround = (itTG != item.rawMap.end()) && (itTG->second == "1");
                _colorTriggers.push_back(ct);
            }
            if (def->enterEffect >= 0) {
                _enterEffectTriggers.push_back({objX, def->enterEffect});
            }
            continue;
        }

        std::string frameName = def->frame;
        if (!def->randomFrames.empty()) {
            frameName = def->randomFrames[rand() % def->randomFrames.size()];
        }

        if (!frameName.empty()) {
            float drawX = objX;
            float drawY = flipY(objY);

            bool isPortalFront = (def->type == "portal" || def->type == "speed") && frameName.find("_front_") != std::string::npos;
            if (isPortalFront) {
                std::string backFrame = frameName;
                size_t pPos = backFrame.find("_front_");
                if (pPos != std::string::npos) backFrame.replace(pPos, 7, "_back_");

                float bdx = 0.0f, bdy = 0.0f;
                getPortalOffset(backFrame, bdx, bdy);
                applyTransformOffset(bdx, bdy, item.scale, item.flipX, item.flipY, item.rot);

                const AtlasFrame* afBack = findAtlasFrame(backFrame);
                VisualSprite backSprite;
                backSprite.frame = backFrame;
                backSprite.x = drawX + bdx;
                backSprite.y = drawY + bdy;
                backSprite.w = (afBack ? afBack->w : 60.0f) * item.scale;
                backSprite.h = (afBack ? afBack->h : 180.0f) * item.scale;
                backSprite.baseX = backSprite.x;
                backSprite.baseY = backSprite.y;
                backSprite.worldX = objX + bdx;
                backSprite.flipX = item.flipX;
                backSprite.flipY = item.flipY;
                backSprite.rotation = item.rot;
                backSprite.scaleX = item.scale;
                backSprite.scaleY = item.scale;
                backSprite.layer = 0;
                _addToSection(backSprite);
            }

            if (def->glow) {
                _addGlowSprite(drawX, drawY, frameName, item, objX);
            }

            float fdx = 0.0f, fdy = 0.0f;
            getPortalOffset(frameName, fdx, fdy);
            applyTransformOffset(fdx, fdy, item.scale, item.flipX, item.flipY, item.rot);

            const AtlasFrame* afMain = findAtlasFrame(frameName);
            VisualSprite mainSprite;
            mainSprite.frame = frameName;
            mainSprite.x = drawX + fdx;
            mainSprite.y = drawY + fdy;
            mainSprite.w = (afMain ? afMain->w : (def->gridW > 0 ? def->gridW * baseUnit : 60.0f)) * item.scale;
            mainSprite.h = (afMain ? afMain->h : (def->gridH > 0 ? def->gridH * baseUnit : 60.0f)) * item.scale;
            mainSprite.baseX = mainSprite.x;
            mainSprite.baseY = mainSprite.y;
            mainSprite.worldX = objX + fdx;
            mainSprite.flipX = item.flipX;
            mainSprite.flipY = item.flipY;
            mainSprite.rotation = item.rot;
            mainSprite.scaleX = item.scale;
            mainSprite.scaleY = item.scale;
            mainSprite.blend = (def->blend == "additive") ? BLEND_ADD : BLEND_NORMAL;

            if (isPortalFront) {
                mainSprite.layer = 2;
            } else if (def->blend == "additive" || def->z < 0) {
                mainSprite.layer = 0;
            } else {
                mainSprite.layer = 1;
            }

            if (def->black) { mainSprite.r = mainSprite.g = mainSprite.b = 0.0f; }
            else if (def->tint > 0) {
                mainSprite.r = ((def->tint >> 16) & 0xFF) / 255.0f;
                mainSprite.g = ((def->tint >> 8)  & 0xFF) / 255.0f;
                mainSprite.b =  (def->tint        & 0xFF) / 255.0f;
            }

            _addToSection(mainSprite);

            if (def->type == "solid" || def->type == "hazard") {
                std::string overlayFrame = frameName;
                size_t oPos = overlayFrame.rfind("_001.png");
                if (oPos != std::string::npos) {
                    overlayFrame.replace(oPos, 8, "_2_001.png");
                    const AtlasFrame* afOver = findAtlasFrame(overlayFrame);
                    if (afOver) {
                        float odx = 0.0f, ody = 0.0f;
                        getPortalOffset(overlayFrame, odx, ody);
                        applyTransformOffset(odx, ody, item.scale, item.flipX, item.flipY, item.rot);

                        VisualSprite overSprite;
                        overSprite.frame = overlayFrame;
                        overSprite.x = drawX + odx;
                        overSprite.y = drawY + ody;
                        overSprite.w = afOver->w * item.scale;
                        overSprite.h = afOver->h * item.scale;
                        overSprite.baseX = overSprite.x;
                        overSprite.baseY = overSprite.y;
                        overSprite.worldX = objX + odx;
                        overSprite.flipX = item.flipX;
                        overSprite.flipY = item.flipY;
                        overSprite.rotation = item.rot;
                        overSprite.scaleX = item.scale;
                        overSprite.scaleY = item.scale;
                        overSprite.blend = BLEND_NORMAL;
                        overSprite.layer = 1;
                        overSprite.r = 1.0f;
                        overSprite.g = 1.0f;
                        overSprite.b = 1.0f;
                        overSprite.a = 1.0f;

                        _addToSection(overSprite);
                    }
                }
            }

            for (const auto& ch : def->children) {
                float cdx = 0.0f, cdy = 0.0f;
                getPortalOffset(ch.frame, cdx, cdy);

                float offX = ch.localDx;
                float offY = ch.localDy;

                if (item.flipX) offX = -offX;
                if (item.flipY) offY = -offY;

                if (item.rot != 0.0f) {
                    float rad = item.rot * 3.14159265f / 180.0f;
                    float rx = offX * std::cos(rad) - offY * std::sin(rad);
                    float ry = offX * std::sin(rad) + offY * std::cos(rad);
                    offX = rx;
                    offY = ry;
                }

                applyTransformOffset(cdx, cdy, item.scale, item.flipX, item.flipY, item.rot);

                const AtlasFrame* afChild = findAtlasFrame(ch.frame);
                VisualSprite childSprite;
                childSprite.frame = ch.frame;
                childSprite.x = drawX + offX + cdx;
                childSprite.y = drawY + offY + cdy;
                childSprite.w = (afChild ? afChild->w : 60.0f) * item.scale;
                childSprite.h = (afChild ? afChild->h : 60.0f) * item.scale;
                childSprite.baseX = childSprite.x;
                childSprite.baseY = childSprite.y;
                childSprite.worldX = objX + offX + cdx;
                childSprite.flipX = item.flipX;
                childSprite.flipY = item.flipY;
                childSprite.rotation = item.rot;
                childSprite.scaleX = item.scale;
                childSprite.scaleY = item.scale;
                childSprite.blend = (ch.blend == "additive") ? BLEND_ADD : BLEND_NORMAL;
                childSprite.layer = (ch.z < 0) ? 0 : 1;
                childSprite.audioScale = ch.audioScale;

                if (ch.tint == 0) {
                    childSprite.r = 0.0f;
                    childSprite.g = 0.0f;
                    childSprite.b = 0.0f;
                } else {
                    childSprite.r = ((ch.tint >> 16) & 0xFF) / 255.0f;
                    childSprite.g = ((ch.tint >> 8)  & 0xFF) / 255.0f;
                    childSprite.b =  (ch.tint        & 0xFF) / 255.0f;
                }

                _addToSection(childSprite);
            }
        }

        if (def->type == "solid" && def->gridW > 0 && def->gridH > 0) {
            float w = def->gridW * baseUnit;
            float h = def->gridH * baseUnit;
            objects.emplace_back("solid", objX, objY, w, h);
            _addCollisionToSection(objects.size() - 1, objX);
        }
        else if (def->type == "hazard") {
            float w = (def->spriteW > 0) ? def->spriteW * def->hitboxScaleX * 2.0f : 12.0f * def->gridW;
            float h = (def->spriteH > 0) ? def->spriteH * def->hitboxScaleY * 2.0f : 24.0f * def->gridH;
            if (w > 0 && h > 0) {
                objects.emplace_back("hazard", objX, objY, w, h);
                _addCollisionToSection(objects.size() - 1, objX);
            }
        }
        else if (def->type == "portal") {
            std::string pType = (def->sub == "fly") ? portalFly : portalCube;
            objects.emplace_back(pType, objX, objY, 90.0f, def->gridH * baseUnit);
            objects.back().portalY = objY;
            _addCollisionToSection(objects.size() - 1, objX);
        }
    }

    std::sort(_colorTriggers.begin(), _colorTriggers.end(), [](const ColorTrigger& a, const ColorTrigger& b) { return a.x < b.x; });
    std::sort(_enterEffectTriggers.begin(), _enterEffectTriggers.end(), [](const EnterEffectTrigger& a, const EnterEffectTrigger& b) { return a.x < b.x; });

    endXPos = std::max((float)screenWidth + 1200.0f, lastX + 680.0f);
}

const std::vector<LevelObject*>& LevelRenderer::getNearbySectionObjects(float cameraX) {
    _nearbyBuffer.clear();
    int sec = std::max(0, (int)std::floor(cameraX / 400.0f));
    int minSec = std::max(0, sec - 1);
    int maxSec = std::min((int)_collisionSections.size() - 1, sec + 2);

    for (int i = minSec; i <= maxSec; ++i) {
        for (size_t idx : _collisionSections[i]) {
            if (idx < objects.size()) {
                _nearbyBuffer.push_back(&objects[idx]);
            }
        }
    }
    return _nearbyBuffer;
}

std::vector<ColorTrigger> LevelRenderer::checkColorTriggers(float cameraX) {
    std::vector<ColorTrigger> active;
    while (_colorTriggerIdx < _colorTriggers.size() && _colorTriggers[_colorTriggerIdx].x <= cameraX) {
        active.push_back(_colorTriggers[_colorTriggerIdx]);
        _colorTriggerIdx++;
    }
    return active;
}

void LevelRenderer::resetColorTriggers() { _colorTriggerIdx = 0; }

void LevelRenderer::checkEnterEffectTriggers(float cameraX) {
    while (_enterEffectTriggerIdx < _enterEffectTriggers.size() && _enterEffectTriggers[_enterEffectTriggerIdx].x <= cameraX) {
        _activeEnterEffect = _enterEffectTriggers[_enterEffectTriggerIdx].effect;
        _activeExitEffect  = _enterEffectTriggers[_enterEffectTriggerIdx].effect;
        _enterEffectTriggerIdx++;
    }
}

void LevelRenderer::resetEnterEffectTriggers() {
    _enterEffectTriggerIdx = 0;
    _activeEnterEffect = 0;
    _activeExitEffect = 0;

    for (auto& sec : _sections) {
        for (auto& s : sec) {
            s.eeActive = false;
            s.x = s.baseX;
            s.y = s.baseY;
            s.a = 1.0f;
            if (!s.audioScale) {
                s.scaleX = 1.0f;
                s.scaleY = 1.0f;
            }
        }
    }
}

void LevelRenderer::updateVisibility(float cameraX) {}

void LevelRenderer::applyEnterEffects(float cameraX) {
    const float sectionW = 400.0f;
    const float margin   = 140.0f;
    const float maxDist  = 200.0f;

    float camLeft   = cameraX;
    float camRight  = cameraX + (float)screenWidth;
    float camCenter = cameraX + (float)screenWidth * 0.5f;

    int minSec = std::max(0, (int)std::floor((camLeft - margin) / sectionW));
    int maxSec = std::min((int)_sections.size() - 1, (int)std::floor((camRight + margin) / sectionW));

    for (int i = minSec; i <= maxSec; ++i) {
        float secLeft = i * sectionW;
        bool isFullyInside = (secLeft >= camLeft + margin && secLeft + sectionW <= camRight - margin);

        for (auto& s : _sections[i]) {
            if (isFullyInside) {
                if (s.eeActive) {
                    s.eeActive = false;
                    s.x = s.baseX;
                    s.y = s.baseY;
                    s.a = 1.0f;
                    if (!s.audioScale) {
                        s.scaleX = 1.0f;
                        s.scaleY = 1.0f;
                    }
                }
                continue;
            }

            float wX = s.worldX;
            bool isRightSide = (wX > camCenter);

            float factor = isRightSide
            ? std::clamp((camRight - wX) / margin, 0.0f, 1.0f)
            : std::clamp((wX - camLeft) / margin, 0.0f, 1.0f);

            if (factor >= 1.0f) {
                if (s.eeActive) {
                    s.eeActive = false;
                    s.x = s.baseX;
                    s.y = s.baseY;
                    s.a = 1.0f;
                    if (!s.audioScale) {
                        s.scaleX = 1.0f;
                        s.scaleY = 1.0f;
                    }
                }
                continue;
            }

            s.eeActive = true;
            int effect = isRightSide ? _activeEnterEffect : _activeExitEffect;
            float invFactor = 1.0f - factor;

            float targetX = s.baseX;
            float targetY = s.baseY;
            float targetAlpha = factor;
            float targetScale = 1.0f;

            switch (effect) {
                case 0: // Fade
                    break;
                case 1: // Slide up
                    targetY = s.baseY + maxDist * invFactor;
                    break;
                case 2: // Slide down
                    targetY = s.baseY - maxDist * invFactor;
                    break;
                case 3: // Slide from left
                    targetX = s.baseX - maxDist * invFactor;
                    break;
                case 4: // Slide from right
                    targetX = s.baseX + maxDist * invFactor;
                    break;
                case 5: // Scale down
                    if (!s.audioScale) targetScale = factor;
                    break;
                case 6: // Zoom
                    if (!s.audioScale) targetScale = 1.0f + 0.75f * invFactor;
                    break;
                default:
                    break;
            }

            s.x = targetX;
            s.y = targetY;
            s.a = targetAlpha;
            if (!s.audioScale) {
                s.scaleX = targetScale;
                s.scaleY = targetScale;
            }
        }
    }
}

void LevelRenderer::updateAudioScale(float scale) {
    _currentAudioScale = scale;
}

void LevelRenderer::resetVisibility() { _visMinSec = _visMaxSec = -1; }

void LevelRenderer::resetObjects() {
    for (auto& obj : objects) obj.activated = false;
}

void LevelRenderer::renderLayer0(float cameraX, float cameraY) {
    glPushMatrix();
    glTranslatef(std::round(-cameraX), std::round(cameraY), 0.0f);

    int startSec = std::max(0, (int)std::floor((cameraX - 200.0f) / 400.0f));
    int endSec = std::min((int)_sections.size() - 1, (int)std::floor((cameraX + screenWidth + 200.0f) / 400.0f));

    for (int i = startSec; i <= endSec; ++i) {
        for (const auto& s : _sections[i]) {
            if (!s.visible || s.layer != 0) continue;
            applyBlendMode(s.blend);

            float dw = s.w * s.scaleX;
            float dh = s.h * s.scaleY;

            if (s.audioScale) {
                dw *= _currentAudioScale;
                dh *= _currentAudioScale;
            }

            drawAtlasFrame(s.frame, std::round(s.x), std::round(s.y), dw, dh, s.rotation, s.r, s.g, s.b, s.a, s.flipX, s.flipY);
        }
    }

    if (endXPos > 0.0f) {
        float pY = flipY(_endPortalGameY);
        applyBlendMode(BLEND_ADD);

        for (const auto& vp : _vortexParticles) {
            float pt = vp.life / vp.maxLife;
            float scale = vp.scale * (1.0f - pt * 0.8f);
            float alpha = 1.0f - pt;
            float size = 20.0f * scale;
            drawAtlasFrame("square.png", vp.x, vp.y, size, size, 0.0f, 0.0f, 1.0f, 0.0f, alpha);
        }

        const AtlasFrame* gradAf = findAtlasFrame("gradientBar.png");
        float gw = gradAf ? gradAf->w : 64.0f;
        float pX = (endXPos - 30.0f) - (gw * 0.5f);
        drawAtlasFrame("gradientBar.png", std::round(pX), std::round(pY), gw, 960.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);

        applyBlendMode(BLEND_NORMAL);
    }

    glPopMatrix();
}

void LevelRenderer::renderLayer1(float cameraX, float cameraY) {
    glPushMatrix();
    glTranslatef(-cameraX, cameraY, 0.0f);

    int startSec = std::max(0, (int)std::floor((cameraX - 200.0f) / 400.0f));
    int endSec = std::min((int)_sections.size() - 1, (int)std::floor((cameraX + screenWidth + 200.0f) / 400.0f));

    for (int i = startSec; i <= endSec; ++i) {
        for (const auto& s : _sections[i]) {
            if (!s.visible || s.layer != 1) continue;
            applyBlendMode(s.blend);

            float dw = s.w * s.scaleX;
            float dh = s.h * s.scaleY;

            if (s.audioScale) {
                dw *= _currentAudioScale;
                dh *= _currentAudioScale;
            }

            drawAtlasFrame(s.frame, s.x, s.y, dw, dh, s.rotation, s.r, s.g, s.b, s.a, s.flipX, s.flipY);
        }
    }

    if (endXPos > 0.0f) {
        float pY = flipY(_endPortalGameY);
        float blockW = (float)baseUnit;
        float blockH = (float)baseUnit;
        const int cols = 8;

        applyBlendMode(BLEND_NORMAL);

        const AtlasFrame* frontSqAf = findAtlasFrame("square_02_001.png");
        if (!frontSqAf) frontSqAf = findAtlasFrame("square_01_001.png");
        std::string frontFrame = frontSqAf ? frontSqAf->name : "square_02_001.png";

        const AtlasFrame* fillSqAf = findAtlasFrame("square_05_001.png");
        if (!fillSqAf) fillSqAf = findAtlasFrame("square_02_001.png");
        std::string fillFrame = fillSqAf ? fillSqAf->name : frontFrame;
        float fillRot = (fillFrame.find("square_02") != std::string::npos) ? -90.0f : 0.0f;

        for (int col = 0; col < cols; ++col) {
            float bx = endXPos + col * blockW;
            bool isFront = (col == 0);

            for (int i = 0; i < 16; ++i) {
                float by = pY + (i - 8) * blockH;

                if (isFront) {
                    drawAtlasFrame(frontFrame, bx, by, blockW, blockH, -90.0f, 1.0f, 1.0f, 1.0f, 1.0f);
                } else {
                    drawAtlasFrame(fillFrame, bx, by, blockW, blockH, fillRot, 1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }
    }

    glPopMatrix();
    applyBlendMode(BLEND_NORMAL);
}

struct InLevelPortalParticle {
    float rx = 0.0f, ry = 0.0f;
    float life = 0.0f, maxLife = 0.5f;
};

struct InLevelPortalEmitter {
    float x = 0.0f, y = 0.0f;
    unsigned int color = 16711935;
    float timer = 0.0f;
    std::vector<InLevelPortalParticle> particles;
};

static std::vector<InLevelPortalEmitter> _inLevelPortalEmitters;
static bool _inLevelPortalsIndexed = false;

void LevelRenderer::renderLayer2(float cameraX, float cameraY) {
    glPushMatrix();
    glTranslatef(-cameraX, cameraY, 0.0f);

    int startSec = std::max(0, (int)std::floor((cameraX - 200.0f) / 400.0f));
    int endSec = std::min((int)_sections.size() - 1, (int)std::floor((cameraX + screenWidth + 200.0f) / 400.0f));

    for (int i = startSec; i <= endSec; ++i) {
        for (const auto& s : _sections[i]) {
            if (!s.visible || s.layer != 2) continue;
            applyBlendMode(s.blend);

            float dw = s.w;
            float dh = s.h;
            if (s.audioScale) {
                float pulse = std::clamp(_currentAudioScale, 0.0f, 1.0f);
                float curScale = 0.20f + pulse * 1.0f;
                dw = s.w * curScale;
                dh = s.h * curScale;
            }

            drawAtlasFrame(s.frame, s.x, s.y, dw, dh, s.rotation, s.r, s.g, s.b, s.a, s.flipX, s.flipY);
        }
    }

    if (!_inLevelPortalsIndexed) {
        _inLevelPortalEmitters.clear();
        for (const auto& obj : objects) {
            if (obj.type == portalFly || obj.type == portalCube) {
                InLevelPortalEmitter pe;
                pe.x = obj.x - 10.0f;
                pe.y = flipY(obj.y);
                pe.color = (obj.type == portalFly) ? 16711935 : 5111552;
                pe.timer = 0.0f;
                _inLevelPortalEmitters.push_back(pe);
            }
        }
        _inLevelPortalsIndexed = true;
    }

    applyBlendMode(BLEND_ADD);
    for (auto& pe : _inLevelPortalEmitters) {
        if (pe.x >= cameraX - 100.0f && pe.x <= cameraX + (float)screenWidth + 100.0f) {
            pe.timer += (1.0f / 60.0f);
            while (pe.timer >= 0.02f) {
                pe.timer -= 0.02f;
                if (pe.particles.size() < 30) {
                    InLevelPortalParticle pp;
                    float angle = ((85.0f + 190.0f * ((rand() % 1000) / 1000.0f)) * 3.14159265f) / 180.0f;
                    float dist = 40.0f + 80.0f * ((rand() % 1000) / 1000.0f);
                    pp.rx = std::cos(angle) * dist;
                    pp.ry = std::sin(angle) * dist;
                    pp.maxLife = (200.0f + (rand() % 801)) / 1000.0f;
                    pp.life = 0.0f;
                    pe.particles.push_back(pp);
                }
            }

            float r = ((pe.color >> 16) & 0xFF) / 255.0f;
            float g = ((pe.color >> 8)  & 0xFF) / 255.0f;
            float b =  (pe.color        & 0xFF) / 255.0f;

            for (auto& pp : pe.particles) {
                pp.life += (1.0f / 60.0f);
                float pt = pp.life / pp.maxLife;
                float curDist = 1.0f - pt;
                float px = pe.x + pp.rx * curDist;
                float py = pe.y + pp.ry * curDist;
                float sc = 0.75f + (0.125f - 0.75f) * pt;
                float alpha = 0.5f * (1.0f - pt);

                drawAtlasFrame("square.png", px, py, 20.0f * sc, 20.0f * sc, 0.0f, r, g, b, alpha);
            }

            pe.particles.erase(
                std::remove_if(pe.particles.begin(), pe.particles.end(), [](const InLevelPortalParticle& pp) {
                    return pp.life >= pp.maxLife;
                }),
                pe.particles.end()
            );
        }
    }
    applyBlendMode(BLEND_NORMAL);

    glPopMatrix();
}

void LevelRenderer::renderGround(float cameraX, float cameraY) {
    _lastCameraY = cameraY;

    float groundSurfaceY;
    float ceilingSurfaceY;

    if (_flyGroundActive && _groundTargetValue > 0.001f) {
        float num32 = 620.0f;
        float num33 = 20.0f;

        groundSurfaceY = _groundStartScreenY + (num32 - _groundStartScreenY) * _groundTargetValue;
        ceilingSurfaceY = _ceilingStartScreenY + (num33 - _ceilingStartScreenY) * _groundTargetValue;

        float maxGroundY = flipY(0.0f) + cameraY;
        if (groundSurfaceY > maxGroundY) {
            groundSurfaceY = maxGroundY;
        }
    } else {
        groundSurfaceY = flipY(0.0f) + cameraY;
        ceilingSurfaceY = 0.0f;
    }

    int startTile = (int)std::floor((cameraX - 200.0f) / _tileW);
    int endTile   = (int)std::ceil((cameraX + screenWidth + 200.0f) / _tileW);

    applyBlendMode(BLEND_NORMAL);

    for (int t = startTile; t <= endTile; ++t) {
        float worldX = t * _tileW;
        float screenX = worldX - cameraX;
        drawAtlasFrame("groundSquare_01_001.png", screenX + _tileW * 0.5f, groundSurfaceY + 90.0f, _tileW, 180.0f, 0.0f, _groundR, _groundG, _groundB, 1.0f);
    }

    applyBlendMode(BLEND_ADD);
    drawAtlasFrame("floorLine_01_001.png", screenWidth * 0.5f, groundSurfaceY + 1.5f, (float)screenWidth - 168.0f, 3.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.8f);
    applyBlendMode(BLEND_NORMAL);

    drawAtlasFrame("groundSquareShadow_001.png", 42.0f, groundSurfaceY + 90.0f, 84.0f, 180.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, false, false);
    drawAtlasFrame("groundSquareShadow_001.png", screenWidth - 42.0f, groundSurfaceY + 90.0f, 84.0f, 180.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, true, false);

    if (_flyGroundActive && _groundTargetValue > 0.001f) {
        for (int t = startTile; t <= endTile; ++t) {
            float worldX = t * _tileW;
            float screenX = worldX - cameraX;
            drawAtlasFrame("groundSquare_01_001.png", screenX + _tileW * 0.5f, ceilingSurfaceY - 90.0f, _tileW, 180.0f, 0.0f, _groundR, _groundG, _groundB, 1.0f, false, true);
        }

        applyBlendMode(BLEND_ADD);
        drawAtlasFrame("floorLine_01_001.png", screenWidth * 0.5f, ceilingSurfaceY - 1.5f, (float)screenWidth - 168.0f, 3.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.8f, false, true);
        applyBlendMode(BLEND_NORMAL);

        drawAtlasFrame("groundSquareShadow_001.png", 42.0f, ceilingSurfaceY - 90.0f, 84.0f, 180.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, false, true);
        drawAtlasFrame("groundSquareShadow_001.png", screenWidth - 42.0f, ceilingSurfaceY - 90.0f, 84.0f, 180.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, true, true);
    }
}
