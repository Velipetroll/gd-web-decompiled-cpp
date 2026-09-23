#include "font-helpers.h"
#include "boot-scene.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

static std::unordered_map<std::string, BitmapFont> loadedFonts;

static std::vector<std::string> splitWhitespace(const std::string& str) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

static std::string getParamValue(const std::string& param) {
    size_t eqPos = param.find('=');
    if (eqPos == std::string::npos) return "";
    std::string val = param.substr(eqPos + 1);
    if (!val.empty() && val.front() == '"') val.erase(0, 1);
    if (!val.empty() && val.back() == '"') val.pop_back();
    return val;
}

static std::string getParamKey(const std::string& param) {
    size_t eqPos = param.find('=');
    if (eqPos == std::string::npos) return "";
    return param.substr(0, eqPos);
}

void defineFontFromFnt(const std::string& fontKey, const std::string& fntText) {
    if (BootScene::textures.find(fontKey) == BootScene::textures.end()) {
        std::cerr << "[font-helpers] Error: Texture not found for: " << fontKey << std::endl;
        return;
    }

    Texture tex = BootScene::textures[fontKey];
    float width = (float)tex.width;
    float height = (float)tex.height;

    BitmapFont font;
    font.fontKey = fontKey;
    font.textureID = tex.id;
    font.texWidth = tex.width;
    font.texHeight = tex.height;

    struct KerningEntry {
        int first, second, amount;
    };
    std::vector<KerningEntry> kerningList;

    std::stringstream stream(fntText);
    std::string line;

    float scaleW = width;
    float scaleH = height;

    while (std::getline(stream, line)) {
        std::vector<std::string> tokens = splitWhitespace(line);
        if (tokens.empty()) continue;

        std::string tag = tokens[0];
        std::unordered_map<std::string, std::string> params;
        for (size_t i = 1; i < tokens.size(); ++i) {
            params[getParamKey(tokens[i])] = getParamValue(tokens[i]);
        }

        if (tag == "info") {
            if (params.find("size") != params.end()) font.size = std::stoi(params["size"]);
        }
        else if (tag == "common") {
            if (params.find("lineHeight") != params.end()) font.lineHeight = std::stoi(params["lineHeight"]);
            if (params.find("scaleW") != params.end()) scaleW = std::stof(params["scaleW"]);
            if (params.find("scaleH") != params.end()) scaleH = std::stof(params["scaleH"]);
        }
        else if (tag == "char") {
            int id = std::stoi(params["id"]);
            int x = std::stoi(params["x"]);
            int y = std::stoi(params["y"]);
            int w = std::stoi(params["width"]);
            int h = std::stoi(params["height"]);

            BitmapChar c;
            c.id = id;
            c.x = x;
            c.y = y;
            c.width = w;
            c.height = h;
            c.xOffset = std::stoi(params["xoffset"]);
            c.yOffset = std::stoi(params["yoffset"]);
            c.xAdvance = std::stoi(params["xadvance"]);

            c.u0 = (float)x / scaleW;
            c.v0 = (float)y / scaleH;
            c.u1 = (float)(x + w) / scaleW;
            c.v1 = (float)(y + h) / scaleH;

            font.chars[id] = c;
        }
        else if (tag == "kerning") {
            kerningList.push_back({
                std::stoi(params["first"]),
                                  std::stoi(params["second"]),
                                  std::stoi(params["amount"])
            });
        }
    }

    for (const auto& k : kerningList) {
        if (font.chars.find(k.second) != font.chars.end()) {
            font.chars[k.second].kerning[k.first] = k.amount;
        }
    }

    loadedFonts[fontKey] = font;
    std::cout << "[font-helpers] Font loaded: " << fontKey
    << " (" << font.chars.size() << " glyphs)" << std::endl;
}

const BitmapFont* getFont(const std::string& fontKey) {
    auto it = loadedFonts.find(fontKey);
    return (it != loadedFonts.end()) ? &it->second : nullptr;
}

void drawBitmapText(const std::string& fontKey, const std::string& text,
                    float x, float y, float scale,
                    float r, float g, float b, float a,
                    bool centerAlign)
{
    const BitmapFont* font = getFont(fontKey);
    if (!font) return;

    if (centerAlign) {
        float totalWidth = 0.0f;
        for (char ch : text) {
            auto it = font->chars.find((unsigned char)ch);
            if (it != font->chars.end()) totalWidth += it->second.xAdvance * scale;
        }
        x -= totalWidth * 0.5f;
        y -= (float)font->lineHeight * scale * 0.5f;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->textureID);
    glColor4f(r, g, b, a);

    float curX = x;
    int prevChar = -1;

    glBegin(GL_QUADS);
    for (char ch : text) {
        auto it = font->chars.find((unsigned char)ch);
        if (it == font->chars.end()) continue;

        const BitmapChar& c = it->second;
        if (prevChar != -1) {
            auto kIt = c.kerning.find(prevChar);
            if (kIt != c.kerning.end()) curX += kIt->second * scale;
        }

        if (c.width > 0 && c.height > 0) {
            float gx = curX + c.xOffset * scale;
            float gy = y + c.yOffset * scale;
            float gw = c.width * scale;
            float gh = c.height * scale;

            glTexCoord2f(c.u0, c.v0); glVertex2f(gx, gy);
            glTexCoord2f(c.u1, c.v0); glVertex2f(gx + gw, gy);
            glTexCoord2f(c.u1, c.v1); glVertex2f(gx + gw, gy + gh);
            glTexCoord2f(c.u0, c.v1); glVertex2f(gx, gy + gh);
        }

        curX += c.xAdvance * scale;
        prevChar = (unsigned char)ch;
    }
    glEnd();
}

static GLuint ttfFontTexID = 0;
static stbtt_packedchar ttfAsciiChars[95];
static stbtt_packedchar ttfCopyrightChar[1];
static stbtt_packedchar ttfMiddleDotChar[1];
static bool ttfFontInitialized = false;

static void initSystemTrueTypeFont() {
    if (ttfFontInitialized) return;

    // Platform-specific system font fallback search paths
    #ifdef _WIN32
    const char* fontPaths[] = {
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\tahoma.ttf",
        "assets/arial.ttf"
    };
    #else
    const char* fontPaths[] = {
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/gnu-free/FreeSans.ttf"
    };
    #endif

    std::vector<unsigned char> ttfBuffer;
    for (const char* path : fontPaths) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            ttfBuffer.resize(size);
            if (file.read((char*)ttfBuffer.data(), size)) {
                std::cout << "[font-helpers] Loaded system font: " << path << std::endl;
                break;
            }
        }
    }

    if (ttfBuffer.empty()) {
        std::cerr << "[font-helpers] Could not find system TTF font." << std::endl;
        return;
    }

    unsigned char alphaBitmap[512 * 512];
    std::memset(alphaBitmap, 0, sizeof(alphaBitmap));

    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, alphaBitmap, 512, 512, 0, 1, NULL);
    stbtt_PackFontRange(&pc, ttfBuffer.data(), 0, 26.0f, 32, 95, ttfAsciiChars);
    stbtt_PackFontRange(&pc, ttfBuffer.data(), 0, 26.0f, 0x00A9, 1, ttfCopyrightChar);
    stbtt_PackFontRange(&pc, ttfBuffer.data(), 0, 26.0f, 0x00B7, 1, ttfMiddleDotChar);
    stbtt_PackEnd(&pc);

    // Convert alpha bitmap to RGBA for OpenGL 1.1 compatibility
    unsigned char rgbaBitmap[512 * 512 * 4];
    for (int i = 0; i < 512 * 512; ++i) {
        unsigned char a = alphaBitmap[i];
        rgbaBitmap[i * 4 + 0] = 255;
        rgbaBitmap[i * 4 + 1] = 255;
        rgbaBitmap[i * 4 + 2] = 255;
        rgbaBitmap[i * 4 + 3] = a;
    }

    glGenTextures(1, &ttfFontTexID);
    glBindTexture(GL_TEXTURE_2D, ttfFontTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaBitmap);

    ttfFontInitialized = true;
}

void drawGenericText(const std::string& text, float x, float y, float size,
                     float r, float g, float b, float a,
                     int align)
{
    initSystemTrueTypeFont();
    if (!ttfFontInitialized || ttfFontTexID == 0) return;

    float fontScale = size / 26.0f;

    float totalW = 0.0f;
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c1 = (unsigned char)text[i];
        if (c1 == 0xC2 && i + 1 < text.size()) {
            unsigned char c2 = (unsigned char)text[i + 1];
            if (c2 == 0xA9) { totalW += ttfCopyrightChar[0].xadvance * fontScale; i++; continue; }
            if (c2 == 0xB7) { totalW += ttfMiddleDotChar[0].xadvance * fontScale; i++; continue; }
        }
        if (c1 >= 32 && c1 <= 126) {
            totalW += ttfAsciiChars[c1 - 32].xadvance * fontScale;
        }
    }

    if (align == 1) {
        x -= totalW * 0.5f;
    } else if (align == 2) {
        x -= totalW;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ttfFontTexID);
    glColor4f(r, g, b, a);

    float curX = x;
    float curY = y + size * 0.35f;

    glBegin(GL_QUADS);
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c1 = (unsigned char)text[i];
        stbtt_aligned_quad q;

        if (c1 == 0xC2 && i + 1 < text.size()) {
            unsigned char c2 = (unsigned char)text[i + 1];
            if (c2 == 0xA9) {
                stbtt_GetPackedQuad(ttfCopyrightChar, 512, 512, 0, &curX, &curY, &q, 0);
                i++;
            } else if (c2 == 0xB7) {
                stbtt_GetPackedQuad(ttfMiddleDotChar, 512, 512, 0, &curX, &curY, &q, 0);
                i++;
            } else {
                continue;
            }
        } else if (c1 >= 32 && c1 <= 126) {
            stbtt_GetPackedQuad(ttfAsciiChars, 512, 512, c1 - 32, &curX, &curY, &q, 0);
        } else {
            continue;
        }

        float x0 = x + (q.x0 - x) * fontScale;
        float x1 = x + (q.x1 - x) * fontScale;
        float y0 = y + (q.y0 - y) * fontScale;
        float y1 = y + (q.y1 - y) * fontScale;

        glTexCoord2f(q.s0, q.t0); glVertex2f(x0, y0);
        glTexCoord2f(q.s1, q.t0); glVertex2f(x1, y0);
        glTexCoord2f(q.s1, q.t1); glVertex2f(x1, y1);
        glTexCoord2f(q.s0, q.t1); glVertex2f(x0, y1);
    }
    glEnd();
}
