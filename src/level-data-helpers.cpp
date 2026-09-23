#include "level-data-helpers.h"
#include "boot-scene.h"
#include <iostream>
#include <algorithm>
#include <sstream>

std::unordered_map<std::string, AtlasFrame> AtlasManager::frames;
float AtlasManager::atlasScale = 1.0f;

void AtlasManager::loadAtlasJson(const std::string& jsonContent, int texW, int texH) {
    if (jsonContent.empty() || texW <= 0 || texH <= 0) return;

    frames.clear();

    // 1. Extraer el tamaño base de la textura definido en el JSON (en tu archivo: "size": { "w": 1046, "h": 1046 })
    float jsonW = 0.0f;
    float jsonH = 0.0f;

    size_t sizePos = jsonContent.find("\"size\"");
    if (sizePos != std::string::npos) {
        auto getDim = [&](const std::string& key) -> float {
            size_t kPos = jsonContent.find("\"" + key + "\"", sizePos);
            if (kPos == std::string::npos || kPos - sizePos > 200) return 0.0f;
            size_t colon = jsonContent.find(":", kPos);
            if (colon == std::string::npos) return 0.0f;
            try { return std::stof(jsonContent.substr(colon + 1)); } catch (...) { return 0.0f; }
        };
        jsonW = getDim("w");
        jsonH = getDim("h");
    }

    // Si por alguna razón no viniera "size", usamos 1046.0f por defecto
    if (jsonW <= 0.0f) jsonW = 1046.0f;
    if (jsonH <= 0.0f) jsonH = 1046.0f;

    // Escala lógica para mantener los tamaños en pantalla constantes
    atlasScale = (jsonW > 1046.0f) ? (jsonW / 1046.0f) : 1.0f;
    if (atlasScale <= 0.0f) atlasScale = 1.0f;

    // 2. Parsear todos los frames normalizando respecto a jsonW / jsonH
    size_t pos = 0;
    while ((pos = jsonContent.find("\"frame\"", pos)) != std::string::npos) {
        size_t nameEnd = jsonContent.rfind(".png\"", pos);
        if (nameEnd == std::string::npos) {
            pos += 7;
            continue;
        }
        size_t nameStart = jsonContent.rfind("\"", nameEnd - 1);
        if (nameStart == std::string::npos) {
            pos += 7;
            continue;
        }

        std::string frameName = jsonContent.substr(nameStart + 1, (nameEnd + 4) - (nameStart + 1));

        auto getVal = [&](const std::string& key) -> float {
            size_t kPos = jsonContent.find("\"" + key + "\"", pos);
            if (kPos == std::string::npos || kPos - pos > 300) return 0.0f;
            size_t colon = jsonContent.find(":", kPos);
            if (colon == std::string::npos) return 0.0f;
            try { return std::stof(jsonContent.substr(colon + 1)); } catch (...) { return 0.0f; }
        };

        float fx = getVal("x");
        float fy = getVal("y");
        float fw = getVal("w");
        float fh = getVal("h");

        if (fw > 0.0f && fh > 0.0f) {
            AtlasFrame af;
            af.name = frameName;
            af.x = fx / atlasScale;
            af.y = fy / atlasScale;
            af.w = fw / atlasScale;
            af.h = fh / atlasScale;

            // NORMALIZACIÓN CORRECTA: Dividir entre el tamaño del canvas del JSON (1046)
            // Esto hace que en cualquier resolución de imagen (2048, 4096, etc.) caiga al pixel exacto
            af.u0 = fx / jsonW;
            af.v0 = fy / jsonH;
            af.u1 = (fx + fw) / jsonW;
            af.v1 = (fy + fh) / jsonH;

            frames[frameName] = af;
            if (frameName.size() > 4 && frameName.substr(frameName.size() - 4) == ".png") {
                frames[frameName.substr(0, frameName.size() - 4)] = af;
            }
        }

        pos += 7;
    }

    std::cout << "[level-data-helpers] Atlas cargado: " << frames.size() << " sprites." << std::endl;
    std::cout << "  -> Textura fisica (PNG): " << texW << "x" << texH << std::endl;
    std::cout << "  -> Canvas del JSON: " << (int)jsonW << "x" << (int)jsonH << std::endl;
    std::cout << "  -> Factor de aumento detectado: " << (float)texW / jsonW << "x" << std::endl;
}

const AtlasFrame* AtlasManager::findAtlasFrame(const std::string& frameName) {
    auto it = frames.find(frameName);
    if (it != frames.end()) return &it->second;

    if (frameName.size() > 4 && frameName.substr(frameName.size() - 4) == ".png") {
        auto it2 = frames.find(frameName.substr(0, frameName.size() - 4));
        if (it2 != frames.end()) return &it2->second;
    } else {
        auto it3 = frames.find(frameName + ".png");
        if (it3 != frames.end()) return &it3->second;
    }
    return nullptr;
}

const AtlasFrame* findAtlasFrame(const std::string& frameName) {
    return AtlasManager::findAtlasFrame(frameName);
}

void drawAtlasFrame(const std::string& frameName, float x, float y,
                    float w, float h, float rotation,
                    float r, float g, float b, float a,
                    bool flipX, bool flipY)
{
    const AtlasFrame* frame = findAtlasFrame(frameName);
    GLuint texID = 0;
    float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
    float drawW = w;
    float drawH = h;

    if (frame) {
        if (BootScene::textures.find("GJ_WebSheet") == BootScene::textures.end()) return;
        texID = BootScene::textures["GJ_WebSheet"].id;
        u0 = frame->u0;
        v0 = frame->v0;
        u1 = frame->u1;
        v1 = frame->v1;
        if (drawW == 0.0f) drawW = frame->w;
        if (drawH == 0.0f) drawH = frame->h;
    } else {
        std::string key = frameName;
        auto it = BootScene::textures.find(key);
        if (it == BootScene::textures.end() && key.size() > 4 && key.substr(key.size() - 4) == ".png") {
            key = key.substr(0, key.size() - 4);
            it = BootScene::textures.find(key);
        }
        if (it == BootScene::textures.end()) {
            key = frameName + ".png";
            it = BootScene::textures.find(key);
        }

        if (it != BootScene::textures.end()) {
            texID = it->second.id;
            u0 = 0.0f; v0 = 0.0f; u1 = 1.0f; v1 = 1.0f;
            if (drawW == 0.0f) drawW = (float)it->second.width;
            if (drawH == 0.0f) drawH = (float)it->second.height;
        } else {
            return;
        }
    }

    if (texID == 0 || drawW <= 0.0f || drawH <= 0.0f) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glColor4f(r, g, b, a);

    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (rotation != 0.0f) {
        glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    }

    float halfW = drawW * 0.5f;
    float halfH = drawH * 0.5f;

    if (flipX) std::swap(u0, u1);
    if (flipY) std::swap(v0, v1);

    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex2f(-halfW, -halfH);
    glTexCoord2f(u1, v0); glVertex2f( halfW, -halfH);
    glTexCoord2f(u1, v1); glVertex2f( halfW,  halfH);
    glTexCoord2f(u0, v1); glVertex2f(-halfW,  halfH);
    glEnd();

    glPopMatrix();
}

void drawScale9(const std::string& textureKey, float x, float y, float w, float h, float cornerSize,
                float r, float g, float b, float a)
{
    std::string key = textureKey;
    auto it = BootScene::textures.find(key);
    if (it == BootScene::textures.end() && key.size() > 4 && key.substr(key.size() - 4) == ".png") {
        key = key.substr(0, key.size() - 4);
        it = BootScene::textures.find(key);
    }
    if (it == BootScene::textures.end()) {
        key = textureKey + ".png";
        it = BootScene::textures.find(key);
    }
    if (it == BootScene::textures.end()) return;

    GLuint texID = it->second.id;
    float texW = (float)it->second.width;
    float texH = (float)it->second.height;
    if (texW <= 0.0f || texH <= 0.0f || w <= 0.0f || h <= 0.0f) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glColor4f(r, g, b, a);

    glPushMatrix();
    glTranslatef(x - w * 0.5f, y - h * 0.5f, 0.0f);

    float cW = std::min(cornerSize, w * 0.5f);
    float cH = std::min(cornerSize, h * 0.5f);

    float xCoords[4] = {0.0f, cW, w - cW, w};
    float yCoords[4] = {0.0f, cH, h - cH, h};

    const float baseTexDim = 80.0f;
    float uCorner = std::min(0.45f, (cornerSize / baseTexDim) * (80.0f / texW));
    float vCorner = std::min(0.45f, (cornerSize / baseTexDim) * (80.0f / texH));

    float uCoords[4] = {0.0f, uCorner, 1.0f - uCorner, 1.0f};
    float vCoords[4] = {0.0f, vCorner, 1.0f - vCorner, 1.0f};

    glBegin(GL_QUADS);
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            glTexCoord2f(uCoords[col],     vCoords[row]);     glVertex2f(xCoords[col],     yCoords[row]);
            glTexCoord2f(uCoords[col + 1], vCoords[row]);     glVertex2f(xCoords[col + 1], yCoords[row]);
            glTexCoord2f(uCoords[col + 1], vCoords[row + 1]); glVertex2f(xCoords[col + 1], yCoords[row + 1]);
            glTexCoord2f(uCoords[col],     vCoords[row + 1]); glVertex2f(xCoords[col],     yCoords[row + 1]);
        }
    }
    glEnd();

    glPopMatrix();
}
