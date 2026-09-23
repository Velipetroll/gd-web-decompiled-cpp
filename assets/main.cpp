#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

#include "stb_image.h"
#include "constants.h"
#include "boot-scene.h"
#include "level-data-helpers.h"
#include "game-scene.h"
#include "win-effects.h"

static void setWindowIcon(SDL_Window* window) {
    stbi_set_flip_vertically_on_load(false);
    int w = 0, h = 0, channels = 0;
    unsigned char* pixels = nullptr;

    // 1. Try loading assets/icon.png if available
    pixels = stbi_load("assets/icon.png", &w, &h, &channels, 4);

    // 2. If no icon.png, parse assets/icon.ico container
    if (!pixels) {
        std::ifstream file("assets/icon.ico", std::ios::binary);
        if (file) {
            std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            if (data.size() >= 22) {
                uint16_t type  = data[2] | (data[3] << 8);
                uint16_t count = data[4] | (data[5] << 8);

                if (type == 1 && count > 0) {
                    uint32_t bestOffset = 0, bestSize = 0;
                    int maxDim = -1;

                    for (uint16_t i = 0; i < count; ++i) {
                        size_t entry = 6 + i * 16;
                        if (entry + 16 > data.size()) break;

                        int ew = data[entry] == 0 ? 256 : data[entry];
                        uint32_t sz  = data[entry+8]  | (data[entry+9]<<8)  | (data[entry+10]<<16) | (data[entry+11]<<24);
                        uint32_t off = data[entry+12] | (data[entry+13]<<8) | (data[entry+14]<<16) | (data[entry+15]<<24);

                        if (ew > maxDim && off + sz <= data.size()) {
                            maxDim = ew;
                            bestOffset = off;
                            bestSize = sz;
                        }
                    }

                    if (bestSize > 0) {
                        const uint8_t* sub = data.data() + bestOffset;
                        // Embedded PNG in modern .ico
                        if (bestSize >= 8 && sub[0] == 0x89 && sub[1] == 'P' && sub[2] == 'N' && sub[3] == 'G') {
                            pixels = stbi_load_from_memory(sub, bestSize, &w, &h, &channels, 4);
                        } else if (bestSize >= 40) {
                            // Embedded DIB / BMP
                            int32_t dibH = (int32_t)(sub[8] | (sub[9]<<8) | (sub[10]<<16) | (sub[11]<<24)) / 2;
                            uint16_t bpp = sub[14] | (sub[15]<<8);

                            std::vector<uint8_t> bmp(14 + bestSize);
                            bmp[0] = 'B'; bmp[1] = 'M';
                            uint32_t fs = 14 + bestSize;
                            bmp[2] = fs & 0xFF; bmp[3] = (fs>>8) & 0xFF; bmp[4] = (fs>>16) & 0xFF; bmp[5] = (fs>>24) & 0xFF;

                            uint32_t pOff = 54;
                            if (bpp <= 8) {
                                uint32_t cols = (sub[32] | (sub[33]<<8)) ? (sub[32] | (sub[33]<<8)) : (1 << bpp);
                                pOff += cols * 4;
                            }
                            bmp[10] = pOff & 0xFF; bmp[11] = (pOff>>8) & 0xFF; bmp[12] = (pOff>>16) & 0xFF; bmp[13] = (pOff>>24) & 0xFF;

                            std::memcpy(bmp.data() + 14, sub, bestSize);
                            bmp[14+8]  = dibH & 0xFF;
                            bmp[14+9]  = (dibH>>8) & 0xFF;
                            bmp[14+10] = (dibH>>16) & 0xFF;
                            bmp[14+11] = (dibH>>24) & 0xFF;

                            pixels = stbi_load_from_memory(bmp.data(), bmp.size(), &w, &h, &channels, 4);
                        }
                    }
                }
            }
        }
    }

    if (pixels) {
        SDL_Surface* iconSurface = SDL_CreateRGBSurfaceWithFormatFrom(
            pixels, w, h, 32, w * 4, SDL_PIXELFORMAT_RGBA32
        );
        if (iconSurface) {
            SDL_SetWindowIcon(window, iconSurface);
            SDL_FreeSurface(iconSurface);
        }
        stbi_image_free(pixels);
    }
}

void updateViewport(int windowWidth, int windowHeight) {
    float targetAspect = (float)screenWidth / (float)screenHeight;
    float windowAspect = (float)windowWidth / (float)windowHeight;

    int vpX = 0, vpY = 0;
    int vpW = windowWidth;
    int vpH = windowHeight;

    if (windowAspect > targetAspect) {
        vpW = (int)(windowHeight * targetAspect);
        vpX = (windowWidth - vpW) / 2;
    } else {
        vpH = (int)(windowWidth / targetAspect);
        vpY = (windowHeight - vpH) / 2;
    }

    glViewport(vpX, vpY, vpW, vpH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Top-left origin: Y=0 at top, Y=screenHeight at bottom
    glOrtho(0.0, screenWidth, screenHeight, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "[Main] Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(
        "Geometry Dash - Play Level 1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "[Main] Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    setWindowIcon(window);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    updateViewport(winW, winH);

    BootScene bootScene;
    bootScene.preload(window);
    bootScene.create();

    if (BootScene::textCache.find("GJ_WebSheetJson") != BootScene::textCache.end()) {
        Texture tex = BootScene::textures["GJ_WebSheet"];
        AtlasManager::loadAtlasJson(BootScene::textCache["GJ_WebSheetJson"], tex.width, tex.height);
    }

    std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
    gameScene->init();

    bool running = true;
    SDL_Event event;

    Uint64 lastTime = SDL_GetPerformanceCounter();
    Uint64 timerFreq = SDL_GetPerformanceFrequency();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                updateViewport(event.window.data1, event.window.data2);
            } else {
                SDL_GetWindowSize(window, &winW, &winH);
                gameScene->handleEvent(event, winW, winH, window);
            }
        }

        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = (float)(currentTime - lastTime) / (float)timerFreq;
        lastTime = currentTime;

        if (dt > 0.1f) dt = 0.1f;

        gameScene->update(dt);
        WinEffects::update(dt);

        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        gameScene->render();

        SDL_GL_SwapWindow(window);
    }

    gameScene.reset();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
