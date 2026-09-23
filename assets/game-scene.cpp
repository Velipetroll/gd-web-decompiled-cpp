#include "game-scene.h"
#include "boot-scene.h"
#include "font-helpers.h"
#include "win-effects.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

static constexpr bool DEBUG_SPAWN_AT_END = false;

static void openURL(const std::string& url) {
    SDL_OpenURL(url.c_str());
}

static void toggleFullscreen(SDL_Window* window) {
    if (!window) return;
    Uint32 flags = SDL_GetWindowFlags(window);
    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

static std::string formatPlayTime(float seconds) {
    int totalSec = (int)std::floor(seconds);
    int hours = totalSec / 3600;
    int mins = (totalSec % 3600) / 60;
    int secs = totalSec % 60;
    char buf[32];
    if (hours > 0) {
        std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, mins, secs);
    } else {
        std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", mins, secs);
    }
    return std::string(buf);
}

static float easeElasticOut(float t) {
    if (t <= 0.0f) return 0.01f;
    if (t >= 1.0f) return 1.0f;
    float p = 0.6f;
    float val = std::pow(2.0f, -10.0f * t) * std::sin((t - p / 4.0f) * (2.0f * 3.14159265f) / p) + 1.0f;
    return val;
}

static float easeBounceOut(float t) {
    if (t < (1.0f / 2.75f)) {
        return 7.5625f * t * t;
    } else if (t < (2.0f / 2.75f)) {
        float p = t - (1.5f / 2.75f);
        return 7.5625f * p * p + 0.75f;
    } else if (t < (2.5f / 2.75f)) {
        float p = t - (2.25f / 2.75f);
        return 7.5625f * p * p + 0.9375f;
    } else {
        float p = t - (2.625f / 2.75f);
        return 7.5625f * p * p + 0.984375f;
    }
}

GameScene::GameScene()
: _cameraX(-groundYOffset),
_cameraY(0.0f),
_prevCameraX(-groundYOffset),
_menuCameraX(-groundYOffset),
_playerWorldX(0.0f),
_slideGroundX(0.0f),
_bgScrollX(0.0f)
{
}

GameScene::~GameScene() {}

void GameScene::init() {
    _level = std::make_unique<LevelRenderer>();
    _player = std::make_unique<Player>(_state, *_level);

    if (BootScene::textCache.find("level_1") != BootScene::textCache.end()) {
        _level->loadLevel(BootScene::textCache["level_1"]);
    }

    _resetGameplayState();
    _slideGroundX = _cameraX;

    _sfxVolume = 1.0f;
    _audio.setSfxVolume(_sfxVolume);

    _menuActive = true;
    _paused = false;
    _showEndLayerUI = false;

    _player->setCubeVisible(false);
    _player->setShipVisible(false);

    _btnAnims[BTN_MENU_PLAY].init(1.0f);
    _btnAnims[BTN_MENU_FS].init(0.64f);
    _btnAnims[BTN_MENU_INFO].init(0.64f);
    _btnAnims[BTN_MENU_STEAM].init(1.0f / 1.5f);
    _btnAnims[BTN_MENU_GOOGLE].init(1.0f / 1.5f);
    _btnAnims[BTN_MENU_APPLE].init(1.0f / 1.5f);

    _btnAnims[BTN_PAUSE_FS].init(0.64f);
    _btnAnims[BTN_PAUSE_REPLAY].init(1.0f);
    _btnAnims[BTN_PAUSE_PLAY].init(1.0f);
    _btnAnims[BTN_PAUSE_MENU].init(1.0f);

    _btnAnims[BTN_END_REPLAY].init(1.0f);
    _btnAnims[BTN_END_MENU].init(1.0f);
    _btnAnims[BTN_END_APPLE].init(1.0f / 1.5f);
    _btnAnims[BTN_END_GOOGLE].init(1.0f / 1.5f);
    _btnAnims[BTN_END_STEAM].init(1.0f / 1.5f);

    _btnAnims[BTN_INFO_CLOSE].init(0.80f);
    _btnAnims[BTN_INFO_YT].init(0.50f);
}

void GameScene::_resetGameplayState() {
    _cameraX = -groundYOffset;
    _cameraY = 0.0f;
    _prevCameraX = _cameraX;
    _playerWorldX = 0.0f;
    _deltaBuffer = 0.0f;
    _deathTimer = 0.0f;
    _deathSoundPlayed = false;
    _newBestShown = false;
    _hadNewBest = false;
    _levelWon = false;
    _showEndLayerUI = false;
    _draggingMusicSlider = false;
    _draggingSfxSlider = false;

    _endCameraOverride = false;
    _endCamTweenActive = false;
    _endCamTweenTime = 0.0f;

    _endSequencePhase = 0;
    _endSequenceTimer = 0.0f;
    _shakeTimer = 0.0f;
    _shakeIntensity = 0.0f;
    _flashAlpha = 0.0f;
    _lightRays.clear();

    _completeBannerVisible = false;
    _completeBannerTimer = 0.0f;
    _completeBannerScale = 0.01f;

    _starAwardStarted = false;
    _starAwardSoundPlayed = false;
    _starAwardTimer = 0.0f;
    _starAwardScale = 3.0f;
    _starAwardAlpha = 0.0f;

    _isMenuAnimatingOut = false;
    _menuAnimTimer = 0.0f;
    _menuPlayTimer = 0.0f;
    _menuGlitterTimer = 0.0f;
    _firstPlay = true;

    _newBestActive = false;
    _newBestTimer = 0.0f;
    _newBestScale = 0.01f;

    _endLayerHiding = false;
    _endLayerHideTimer = 0.0f;
    _endLayerHideCallback = nullptr;
}

void GameScene::_showNewBest() {
    _newBestActive = true;
    _newBestTimer = 0.0f;
    _newBestScale = 0.01f;
}

void GameScene::_hideEndLayer(std::function<void()> onComplete) {
    _endLayerHiding = true;
    _endLayerHideTimer = 0.0f;
    _endLayerHideCallback = onComplete;
}

void GameScene::startGame() {
    if (!_menuActive) return;
    _menuActive = false;

    if (DEBUG_SPAWN_AT_END) {
        _slideIn = false;
        float endX = (_level->endXPos > 0.0f) ? _level->endXPos : 6000.0f;
        _playerWorldX = endX - 1800.0f;
        _cameraX = _playerWorldX - groundYOffset;
        _prevCameraX = _cameraX;
        _slideGroundX = _cameraX;

        _state.y = 200.0f;
        _state.onGround = false;

        _endCameraOverride = false;
        _endCamTweenActive = false;
        _endCamTweenTime = 0.0f;

        _player->reset();
        _player->enterShipMode();
        _audio.startMusic();
        _firstPlay = false;
        return;
    }

    _slideIn = true;
    _isMenuAnimatingOut = true;
    _menuAnimTimer = 0.0f;

    _cameraX = -groundYOffset;
    _cameraY = 0.0f;
    _prevCameraX = _cameraX;

    _playerWorldX = -groundYOffset - 180.0f;

    _state.y = 30.0f;
    _state.onGround = true;

    _player->setCubeVisible(true);
    _player->reset();
}

void GameScene::restartLevel() {
    _attempts++;
    _resetGameplayState();
    _state.reset();
    _player->reset();
    _level->resetObjects();
    _level->resetGroundState();
    _level->resetColorTriggers();
    _level->resetEnterEffectTriggers();
    _colorManager.reset();

    if (DEBUG_SPAWN_AT_END) {
        float endX = (_level->endXPos > 0.0f) ? _level->endXPos : 6000.0f;
        _playerWorldX = endX - 1800.0f;
        _cameraX = _playerWorldX - groundYOffset;
        _prevCameraX = _cameraX;
        _state.y = 200.0f;
        _state.onGround = false;

        _endCameraOverride = false;
        _endCamTweenActive = false;
        _endCamTweenTime = 0.0f;

        _player->enterShipMode();
    }

    _audio.reset();
    _audio.startMusic();
    _paused = false;
}

void GameScene::pushButton() {
    if (_menuActive) {
        _audio.playEffect("playSound_01");
        startGame();
        return;
    }
    if (_slideIn || _state.isDead || _levelWon) return;

    _state.upKeyDown = true;
    _state.upKeyPressed = true;
    if (!_state.isFlying && _state.canJump) {
        _player->updateJump(0.0f);
        _totalJumps++;
    }
}

void GameScene::releaseButton() {
    _state.upKeyDown = false;
    _state.upKeyPressed = false;
}

void GameScene::pauseGame() {
    if (_paused || _menuActive || _slideIn || _state.isDead || _levelWon) return;
    _paused = true;
    _audio.pauseMusic();
}

void GameScene::resumeGame() {
    if (!_paused) return;
    _paused = false;
    _draggingMusicSlider = false;
    _draggingSfxSlider = false;
    _audio.resumeMusic();
}

void GameScene::handleEvent(const SDL_Event& event, int windowW, int windowH, SDL_Window* window) {
    if (window) {
        Uint32 flags = SDL_GetWindowFlags(window);
        _isFullscreen = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
    }

    if (_fadeState != 0) return;

    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP) {
            pushButton();
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            if (_showInfoPopup) {
                _showInfoPopup = false;
            } else if (_paused && !_showEndLayerUI) {
                resumeGame();
            } else if (!_showEndLayerUI) {
                pauseGame();
            }
        }
    } else if (event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP) {
            releaseButton();
        }
    }

    else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONUP) {
        float targetAspect = (float)screenWidth / (float)screenHeight;
        float windowAspect = (float)windowW / (float)windowH;
        int vpX = 0, vpY = 0, vpW = windowW, vpH = windowH;

        if (windowAspect > targetAspect) {
            vpW = (int)(windowH * targetAspect);
            vpX = (windowW - vpW) / 2;
        } else {
            vpH = (int)(windowW / targetAspect);
            vpY = (windowH - vpH) / 2;
        }

        int mouseX = (event.type == SDL_MOUSEMOTION) ? event.motion.x : event.button.x;
        int mouseY = (event.type == SDL_MOUSEMOTION) ? event.motion.y : event.button.y;

        float virtX = (float)(mouseX - vpX) * ((float)screenWidth / (float)vpW);
        float virtY = (float)(mouseY - vpY) * ((float)screenHeight / (float)vpH);
        float midX = screenWidth * 0.5f;

        auto checkButtonHit = [&](float vx, float vy) -> ButtonId {
            auto getScaleFactor = [&](ButtonId id, float baseScale) -> float {
                return (baseScale > 0.0f) ? (_btnAnims[id].scale / baseScale) : 1.0f;
            };

            if (_showInfoPopup) {
                float closeFactor = getScaleFactor(BTN_INFO_CLOSE, 0.80f);
                float closeR = 40.0f * closeFactor;
                float dClose = (vx - (midX - 220.0f)) * (vx - (midX - 220.0f)) + (vy - 172.0f) * (vy - 172.0f);
                if (dClose <= closeR * closeR) return BTN_INFO_CLOSE;

                float textW = 145.0f;
                const BitmapFont* gf = getFont("goldFont");
                if (gf) {
                    float tw = 0.0f;
                    for (char ch : std::string("by ForeverBound")) {
                        auto it = gf->chars.find((unsigned char)ch);
                        if (it != gf->chars.end()) tw += it->second.xAdvance * 0.6f;
                    }
                    if (tw > 0.0f) textW = tw;
                }
                float textRight = (midX - 20.0f) + (textW * 0.5f);
                float ytX = textRight + 20.0f + 40.0f;
                float ytFactor = getScaleFactor(BTN_INFO_YT, 0.50f);
                if (std::abs(vx - ytX) <= 35.0f * ytFactor && std::abs(vy - 368.0f) <= 25.0f * ytFactor) return BTN_INFO_YT;

                return BTN_COUNT;
            }

            if (_menuActive) {
                float fsFactor = getScaleFactor(BTN_MENU_FS, 0.64f);
                if (std::abs(vx - 33.0f) <= 32.0f * fsFactor && std::abs(vy - 33.0f) <= 32.0f * fsFactor) return BTN_MENU_FS;

                float infoFactor = getScaleFactor(BTN_MENU_INFO, 0.64f);
                if (std::abs(vx - (screenWidth - 33.0f)) <= 32.0f * infoFactor && std::abs(vy - 33.0f) <= 32.0f * infoFactor) return BTN_MENU_INFO;

                float sScale = 1.0f / 1.5f;
                float steamFactor = getScaleFactor(BTN_MENU_STEAM, sScale);
                if (std::abs(vx - (screenWidth - 130.0f)) <= 75.0f * steamFactor && std::abs(vy - 555.0f) <= 28.0f * steamFactor) return BTN_MENU_STEAM;

                float googleFactor = getScaleFactor(BTN_MENU_GOOGLE, sScale);
                if (std::abs(vx - (screenWidth - 340.0f)) <= 75.0f * googleFactor && std::abs(vy - 555.0f) <= 28.0f * googleFactor) return BTN_MENU_GOOGLE;

                float appleFactor = getScaleFactor(BTN_MENU_APPLE, sScale);
                if (std::abs(vx - (screenWidth - 550.0f)) <= 75.0f * appleFactor && std::abs(vy - 555.0f) <= 28.0f * appleFactor) return BTN_MENU_APPLE;

                float playFactor = getScaleFactor(BTN_MENU_PLAY, 1.0f);
                float dx = vx - midX;
                float dy = vy - _menuPlayBtnY;
                float playR = 70.0f * playFactor;
                if (dx * dx + dy * dy <= playR * playR) return BTN_MENU_PLAY;

                return BTN_COUNT;
            }

            if (_paused && !_showEndLayerUI) {
                float fsFactor = getScaleFactor(BTN_PAUSE_FS, 0.64f);
                if (std::abs(vx - 60.0f) <= 45.0f * fsFactor && std::abs(vy - 60.0f) <= 45.0f * fsFactor) return BTN_PAUSE_FS;

                struct PauseBtnDef { const char* name; ButtonId id; };
                PauseBtnDef pBtns[3] = {
                    {"GJ_replayBtn_001.png", BTN_PAUSE_REPLAY},
                    {"GJ_playBtn2_001.png",  BTN_PAUSE_PLAY},
                    {"GJ_menuBtn_001.png",   BTN_PAUSE_MENU}
                };

                float pW[3];
                float totalW = 0.0f;
                for (int i = 0; i < 3; ++i) {
                    const AtlasFrame* af = findAtlasFrame(pBtns[i].name);
                    pW[i] = (af && af->w > 0.0f) ? af->w : 85.0f;
                    totalW += pW[i];
                }
                totalW += 40.0f * 2.0f;

                float pStartX = midX - totalW * 0.5f;
                for (int i = 0; i < 3; ++i) {
                    float bx = pStartX + pW[i] * 0.5f;
                    float btnFactor = getScaleFactor(pBtns[i].id, 1.0f);
                    float halfHitW = (pW[i] * 0.5f + 10.0f) * btnFactor;
                    float halfHitH = (pW[i] * 0.5f + 10.0f) * btnFactor;
                    if (std::abs(vx - bx) <= halfHitW && std::abs(vy - 330.0f) <= halfHitH) {
                        return pBtns[i].id;
                    }
                    pStartX += pW[i] + 40.0f;
                }

                return BTN_COUNT;
            }

            if (_showEndLayerUI && !_endLayerHiding) {
                float replayFactor = getScaleFactor(BTN_END_REPLAY, 1.0f);
                float dReplay = (vx - (midX - 200.0f)) * (vx - (midX - 200.0f)) + (vy - 555.0f) * (vy - 555.0f);
                float replayR = 50.0f * replayFactor;
                if (dReplay <= replayR * replayR) return BTN_END_REPLAY;

                float menuFactor = getScaleFactor(BTN_END_MENU, 1.0f);
                float dMenu = (vx - (midX + 200.0f)) * (vx - (midX + 200.0f)) + (vy - 555.0f) * (vy - 555.0f);
                float menuR = 50.0f * menuFactor;
                if (dMenu <= menuR * menuR) return BTN_END_MENU;

                float sScale = 1.0f / 1.5f;
                float appleFactor = getScaleFactor(BTN_END_APPLE, sScale);
                if (std::abs(vx - (midX - 225.0f)) <= 75.0f * appleFactor && std::abs(vy - 437.5f) <= 28.0f * appleFactor) return BTN_END_APPLE;

                float googleFactor = getScaleFactor(BTN_END_GOOGLE, sScale);
                if (std::abs(vx - midX) <= 75.0f * googleFactor && std::abs(vy - 437.5f) <= 28.0f * googleFactor) return BTN_END_GOOGLE;

                float steamFactor = getScaleFactor(BTN_END_STEAM, sScale);
                if (std::abs(vx - (midX + 225.0f)) <= 75.0f * steamFactor && std::abs(vy - 437.5f) <= 28.0f * steamFactor) return BTN_END_STEAM;

                return BTN_COUNT;
            }

            return BTN_COUNT;
        };

        auto getBaseScale = [](ButtonId id) -> float {
            switch (id) {
                case BTN_MENU_FS:
                case BTN_MENU_INFO:
                case BTN_PAUSE_FS:
                    return 0.64f;
                case BTN_MENU_STEAM:
                case BTN_MENU_GOOGLE:
                case BTN_MENU_APPLE:
                case BTN_END_APPLE:
                case BTN_END_GOOGLE:
                case BTN_END_STEAM:
                    return 1.0f / 1.5f;
                case BTN_INFO_CLOSE:
                    return 0.80f;
                case BTN_INFO_YT:
                    return 0.50f;
                default:
                    return 1.0f;
            }
        };

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (_paused && !_showEndLayerUI) {
                float trackWidth = 288.4f;
                float musicStartX = (midX - 200.0f) - 144.2f;
                if (virtX >= musicStartX - 20 && virtX <= musicStartX + trackWidth + 20 && virtY >= 470 && virtY <= 530) {
                    _draggingMusicSlider = true;
                    float val = std::clamp((virtX - musicStartX) / trackWidth, 0.0f, 1.0f);
                    _audio.setUserMusicVolume(val);
                    return;
                }
                float sfxStartX = (midX + 200.0f) - 144.2f;
                if (virtX >= sfxStartX - 20 && virtX <= sfxStartX + trackWidth + 20 && virtY >= 470 && virtY <= 530) {
                    _draggingSfxSlider = true;
                    float val = std::clamp((virtX - sfxStartX) / trackWidth, 0.0f, 1.0f);
                    _sfxVolume = val;
                    _audio.setSfxVolume(_sfxVolume);
                    return;
                }
            }

            ButtonId hit = checkButtonHit(virtX, virtY);
            if (hit != BTN_COUNT) {
                _heldBtn = hit;
                _isButtonPressed = true;
                _btnAnims[hit].press(getBaseScale(hit));
                return;
            }

            if (_showInfoPopup) {
                if (virtX < midX - 240.0f || virtX > midX + 240.0f || virtY < 152.0f || virtY > 488.0f) {
                    _showInfoPopup = false;
                }
                return;
            }

            if (!_menuActive && !_paused && !_showEndLayerUI) {
                if (virtX >= screenWidth - 75.0f && virtY <= 75.0f) {
                    pauseGame();
                } else {
                    pushButton();
                }
            }
        }

        else if (event.type == SDL_MOUSEMOTION) {
            if (_paused && !_showEndLayerUI) {
                float trackWidth = 288.4f;
                if (_draggingMusicSlider) {
                    float startX = (midX - 200.0f) - 144.2f;
                    float val = std::clamp((virtX - startX) / trackWidth, 0.0f, 1.0f);
                    _audio.setUserMusicVolume(val);
                    return;
                } else if (_draggingSfxSlider) {
                    float startX = (midX + 200.0f) - 144.2f;
                    float val = std::clamp((virtX - startX) / trackWidth, 0.0f, 1.0f);
                    _sfxVolume = val;
                    _audio.setSfxVolume(_sfxVolume);
                    return;
                }
            }

            if (_heldBtn != BTN_COUNT) {
                ButtonId cur = checkButtonHit(virtX, virtY);
                if (cur != _heldBtn && _isButtonPressed) {
                    _isButtonPressed = false;
                    _btnAnims[_heldBtn].deselect();
                } else if (cur == _heldBtn && !_isButtonPressed) {
                    _isButtonPressed = true;
                    _btnAnims[_heldBtn].press(getBaseScale(_heldBtn));
                }
            }
        }

        else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            _draggingMusicSlider = false;
            _draggingSfxSlider = false;

            if (_heldBtn != BTN_COUNT) {
                ButtonId releaseHit = checkButtonHit(virtX, virtY);
                ButtonId active = _heldBtn;
                bool wasPressed = _isButtonPressed;

                _heldBtn = BTN_COUNT;
                _isButtonPressed = false;
                _btnAnims[active].release();

                if (wasPressed && releaseHit == active) {
                    switch (active) {
                        case BTN_INFO_CLOSE:
                            _showInfoPopup = false;
                            break;
                        case BTN_INFO_YT:
                            openURL("https://www.youtube.com/watch?v=JhKyKEDxo8Q");
                            break;
                        case BTN_MENU_FS:
                        case BTN_PAUSE_FS:
                            toggleFullscreen(window);
                            _isFullscreen = !_isFullscreen;
                            break;
                        case BTN_MENU_INFO:
                            _showInfoPopup = true;
                            break;
                        case BTN_MENU_STEAM:
                        case BTN_END_STEAM:
                            openURL("https://store.steampowered.com/app/322170/Geometry_Dash");
                            break;
                        case BTN_MENU_GOOGLE:
                        case BTN_END_GOOGLE:
                            openURL("https://play.google.com/store/apps/details?id=com.robtopx.geometryjump&hl=en");
                            break;
                        case BTN_MENU_APPLE:
                        case BTN_END_APPLE:
                            openURL("https://apps.apple.com/us/app/geometry-dash/id625334537");
                            break;
                        case BTN_MENU_PLAY:
                            _audio.playEffect("playSound_01");
                            startGame();
                            break;
                        case BTN_PAUSE_PLAY:
                            resumeGame();
                            break;
                        case BTN_PAUSE_REPLAY:
                            resumeGame();
                            restartLevel();
                            break;
                        case BTN_PAUSE_MENU:
                            _audio.playEffect("quitSound_01");
                            _audio.stopMusic();
                            _fadeState = 1;
                            _fadeTimer = 0.0f;
                            break;
                        case BTN_END_REPLAY:
                            _hideEndLayer([this]() {
                                restartLevel();
                            });
                            break;
                        case BTN_END_MENU:
                            _audio.playEffect("quitSound_01");
                            _audio.stopMusic();
                            _fadeState = 1;
                            _fadeTimer = 0.0f;
                            break;
                        default:
                            break;
                    }
                }
            }

            releaseButton();
        }
    }
}

float GameScene::_quantizeDelta(float dt) {
    float dtSec = dt + _deltaBuffer;
    int steps = (int)std::round(dtSec / fixedTimeStep);
    steps = std::clamp(steps, 0, 60);
    float actualDt = steps * fixedTimeStep;
    _deltaBuffer = dtSec - actualDt;
    return 60.0f * actualDt;
}

void GameScene::_updateBackground(float dt) {
    (void)dt;
}

void GameScene::_updateCameraY(float dt) {
    float targetY = _cameraY;
    if (_level->hasCeiling()) {
        targetY = _level->flyCameraTarget;
    } else {
        float pY = _state.y;
        float centerOffset = _cameraY - unusedConst180 + 320.0f;
        if (pY > centerOffset + 140.0f) {
            targetY = pY - 320.0f - 140.0f + unusedConst180;
        } else if (pY < centerOffset - 80.0f) {
            targetY = pY - 320.0f + 80.0f + unusedConst180;
        }
    }
    if (targetY < 0.0f) targetY = 0.0f;
    if (dt > 0.0f) {
        _cameraY += (targetY - _cameraY) / (10.0f / dt);
        if (_cameraY < 0.0f) _cameraY = 0.0f;
    }
}

void GameScene::_startCompleteLightRays() {
    _lightRays.clear();

    const int rayCount = 8;
    float baseAngle = -135.0f;
    float stepAngle = 90.0f / (float)rayCount;
    float targetLen = std::round(std::sqrt((float)(screenWidth * screenWidth) + 102400.0f)) + 65.0f;

    std::vector<float> angles(rayCount);
    for (int i = 0; i < rayCount; ++i) {
        angles[i] = baseAngle + i * stepAngle;
    }
    for (int i = rayCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        std::swap(angles[i], angles[j]);
    }

    for (int i = 0; i < rayCount; ++i) {
        float rnd1 = (rand() % 1000) / 500.0f - 1.0f;
        float rnd2 = (rand() % 1000) / 500.0f - 1.0f;
        float rnd3 = (rand() % 1000) / 500.0f - 1.0f;
        float rnd4 = (rand() % 1000) / 500.0f - 1.0f;

        float delay = (i * 0.195f + 0.04f + 0.04f * rnd1);
        if (delay < 0.0f) delay = 0.0f;

        float targetW = 60.0f + 40.0f * rnd2;
        float duration = 0.18f + 0.04f * rnd3;
        float maxAlpha = std::clamp((155.0f / 255.0f) + (100.0f / 255.0f) * rnd4, 0.0f, 1.0f);
        float angle = angles[i] + stepAngle * ((rand() % 1000) / 1000.0f) + 180.0f;

        CompleteLightRay ray;
        ray.angleDeg = angle;
        ray.targetW = targetW;
        ray.targetH = targetLen;
        ray.currentW = 2.0f;
        ray.currentH = 1.0f;
        ray.maxAlpha = maxAlpha;
        ray.currentAlpha = 0.0f;
        ray.delay = delay;
        ray.duration = duration;
        ray.elapsed = 0.0f;
        ray.fadeDelay = delay + duration + 0.40f;
        ray.fadeDuration = 0.4f + 0.1f * rnd1;
        ray.fadeElapsed = 0.0f;
        ray.started = false;
        ray.fading = false;
        ray.done = false;

        _lightRays.push_back(ray);
    }
}

void GameScene::_updateCompleteLightRays(float dt) {
    for (auto& ray : _lightRays) {
        if (ray.done) continue;

        if (!ray.started) {
            ray.delay -= dt;
            if (ray.delay <= 0.0f) {
                ray.started = true;
            }
        } else if (!ray.fading) {
            ray.elapsed += dt;
            float t = std::min(ray.elapsed / ray.duration, 1.0f);
            float ease = 1.0f - (1.0f - t) * (1.0f - t);

            ray.currentH = 1.0f + (ray.targetH - 1.0f) * ease;
            ray.currentW = 2.0f + (ray.targetW - 2.0f) * ease;
            ray.currentAlpha = ray.maxAlpha;

            if (ray.elapsed >= ray.duration + 0.35f) {
                ray.fading = true;
            }
        } else {
            ray.fadeElapsed += dt;
            float t = std::min(ray.fadeElapsed / ray.fadeDuration, 1.0f);
            ray.currentAlpha = ray.maxAlpha * (1.0f - t);
            if (t >= 1.0f) {
                ray.done = true;
            }
        }
    }
}

void GameScene::_renderCompleteLightRays() {
    if (_lightRays.empty()) return;

    float originX = _level->endXPos - _cameraX + 60.0f;
    float originY = flipY(_endPortalGameY) + _cameraY;

    glDisable(GL_TEXTURE_2D);
    applyBlendMode(BLEND_ADD);

    for (const auto& ray : _lightRays) {
        if (!ray.started || ray.done || ray.currentAlpha <= 0.0f) continue;

        glPushMatrix();
        glTranslatef(originX, originY, 0.0f);
        glRotatef(ray.angleDeg, 0.0f, 0.0f, 1.0f);

        glColor4f(0.0f, 1.0f, 0.0f, ray.currentAlpha);

        float wBase = 2.0f + (ray.currentW - 2.0f) * 0.25f;
        float wEnd  = ray.currentW;
        float hEnd  = ray.currentH;

        glBegin(GL_QUADS);
        glVertex2f(-wBase * 0.5f, 0.0f);
        glVertex2f( wBase * 0.5f, 0.0f);
        glVertex2f( wEnd  * 0.5f, hEnd);
        glVertex2f(-wEnd  * 0.5f, hEnd);
        glEnd();

        glPopMatrix();
    }

    applyBlendMode(BLEND_NORMAL);
    glEnable(GL_TEXTURE_2D);
}

struct FlightGlitter {
    float x, y;
    float life, maxLife;
    float scale;
};
static std::vector<FlightGlitter> _flightGlitters;
static float _flightGlitterTimer = 0.0f;

void GameScene::update(float dt) {
    for (int i = 0; i < BTN_COUNT; ++i) {
        _btnAnims[i].update(dt);
    }

    if (_newBestActive) {
        _newBestTimer += dt;
        if (_newBestTimer <= 0.40f) {
            float t = _newBestTimer / 0.40f;
            _newBestScale = easeElasticOut(t);
        } else if (_newBestTimer <= 1.10f) {
            _newBestScale = 1.0f;
        } else if (_newBestTimer <= 1.30f) {
            float t = (_newBestTimer - 1.10f) / 0.20f;
            _newBestScale = std::max(0.01f, 1.0f - t * t);
        } else {
            _newBestActive = false;
            _newBestScale = 0.01f;
        }
    }

    if (_endLayerHiding) {
        _endLayerHideTimer += dt;
        float t = std::min(_endLayerHideTimer / 0.5f, 1.0f);
        if (t >= 1.0f) {
            _endLayerHiding = false;
            _showEndLayerUI = false;
            if (_endLayerHideCallback) {
                auto cb = _endLayerHideCallback;
                _endLayerHideCallback = nullptr;
                cb();
            }
        }
    }

    bool isFlightActive = _state.isFlying && !_state.isDead && !_levelWon && !_menuActive && !_paused;
    if (isFlightActive) {
        _flightGlitterTimer += dt;
        while (_flightGlitterTimer >= 0.06f) {
            _flightGlitterTimer -= 0.06f;
            if (_flightGlitters.size() < 60) {
                FlightGlitter fg;
                float centerX = _cameraX + screenWidth * 0.5f;
                float centerY = yFlipBase - _cameraY;
                fg.x = centerX + (((rand() % 1000) / 500.0f) - 1.0f) * (screenWidth / 1.8f);
                fg.y = centerY + 320.0f * (((rand() % 1000) / 500.0f) - 1.0f);
                fg.maxLife = (200.0f + (rand() % 1601)) / 1000.0f;
                fg.life = 0.0f;
                fg.scale = 0.375f;
                _flightGlitters.push_back(fg);
            }
        }
    } else {
        _flightGlitterTimer = 0.0f;
    }

    for (auto& fg : _flightGlitters) fg.life += dt;
    _flightGlitters.erase(
        std::remove_if(_flightGlitters.begin(), _flightGlitters.end(), [](const FlightGlitter& fg) {
            return fg.life >= fg.maxLife;
        }),
        _flightGlitters.end()
    );

    if (_fadeState == 1) {
        _fadeTimer += dt;
        float t = std::min(_fadeTimer / 0.4f, 1.0f);
        _blackFadeAlpha = t;

        if (t >= 1.0f) {
            _menuActive = true;
            _paused = false;
            _showEndLayerUI = false;

            _state.reset();
            _player->reset();
            _player->setCubeVisible(false);
            _player->setShipVisible(false);

            _level->resetObjects();
            _level->resetGroundState();
            _level->resetColorTriggers();
            _level->resetEnterEffectTriggers();
            _level->resetVisibility();

            _colorManager.reset();
            float bgR, bgG, bgB, gR, gG, gB;
            _colorManager.getGLColor(ColorManager::COLOR_BG, bgR, bgG, bgB);
            _colorManager.getGLColor(ColorManager::COLOR_GROUND, gR, gG, gB);
            _level->setGroundColor(gR, gG, gB);

            WinEffects::reset();
            _flightGlitters.clear();
            _resetGameplayState();

            _fadeState = 2;
            _fadeTimer = 0.0f;
        }
        return;
    } else if (_fadeState == 2) {
        _fadeTimer += dt;
        float t = std::min(_fadeTimer / 0.4f, 1.0f);
        _blackFadeAlpha = 1.0f - t;

        if (t >= 1.0f) {
            _fadeState = 0;
            _blackFadeAlpha = 0.0f;
        }
    }

    if (_shakeTimer > 0.0f) {
        _shakeTimer -= dt;
        if (_shakeTimer <= 0.0f) _shakeTimer = 0.0f;
    }

    if (_flashAlpha > 0.0f) {
        _flashAlpha = std::max(0.0f, _flashAlpha - dt * 2.5f);
    }

    if (_paused) {
        _deltaBuffer = 0.0f;
        return;
    }

    if (_isMenuAnimatingOut) {
        _menuAnimTimer += dt;
        if (_menuAnimTimer >= 0.3f) {
            _isMenuAnimatingOut = false;
        }
    }

    if (_menuActive) {
        _menuPlayTimer += dt;
        _menuPlayBtnY = 322.0f + std::sin(_menuPlayTimer * (3.14159265f / 0.75f)) * 2.0f;

        float dx = dt * 60.0f * gravityConst * physicsConst09 * 0.25f;
        _menuCameraX += dx;
        _cameraX = _menuCameraX;

        _slideGroundX += dx;
        _bgScrollX += dx * 0.1f;
        _prevCameraX = _cameraX;

        _level->stepGroundAnimation(dt);
        _level->updateGroundTiles(_slideGroundX, _cameraY, dt);

        _menuGlitterTimer += dt;
        while (_menuGlitterTimer >= 0.035f) {
            _menuGlitterTimer -= 0.035f;
            MenuGlitter mg;
            mg.x = (screenWidth * 0.5f) + ((rand() % 260) - 130);
            mg.y = 320.0f + ((rand() % 200) - 100);
            mg.life = 0.0f;
            mg.maxLife = 1.0f + (rand() % 100) / 100.0f;
            mg.scale = 0.5f;
            _menuParticles.push_back(mg);
        }
        for (auto& mp : _menuParticles) mp.life += dt;
        _menuParticles.erase(
            std::remove_if(_menuParticles.begin(), _menuParticles.end(), [](const MenuGlitter& mp){
                return mp.life >= mp.maxLife;
            }),
            _menuParticles.end()
        );

        return;
    }

    if (_slideIn) {
        float qDt = _quantizeDelta(dt);
        float playerDx = qDt * gravityConst * physicsConst09;
        _playerWorldX += playerDx;

        float groundDx = playerDx * 0.25f;
        _slideGroundX += groundDx;
        _bgScrollX += groundDx * 0.1f;

        _player->updateGroundRotation(qDt * physicsConst09);
        _player->update(dt, _playerWorldX, _cameraY, _cameraX);

        _level->stepGroundAnimation(dt);
        _level->updateGroundTiles(_slideGroundX, _cameraY, dt);
        _level->applyEnterEffects(_cameraX);

        if (_playerWorldX >= 0.0f) {
            _slideIn = false;
            _playerWorldX = 0.0f;
            _cameraX = _playerWorldX - groundYOffset;
            _prevCameraX = _cameraX;

            _audio.startMusic();
            _firstPlay = false;
        }
        return;
    }

    if (_state.isDead) {
        if (!_deathSoundPlayed) {
            _audio.stopMusic();
            _audio.playEffect("explode_11", 0.65f);
            _deathSoundPlayed = true;
        }

        if (!_newBestShown) {
            _newBestShown = true;
            float endX = _level->endXPos > 0.0f ? _level->endXPos : 6000.0f;
            _lastPercent = std::clamp((int)std::floor((_playerWorldX / endX) * 100.0f), 0, 99);
            if (_lastPercent > _bestPercent) {
                _bestPercent = _lastPercent;
                _hadNewBest = true;
                _showNewBest();
            }
        }

        _player->update(dt, _playerWorldX, _cameraY, _cameraX);
        _deathTimer += dt * 1000.0f;
        float waitLimit = _hadNewBest ? 1400.0f : 1000.0f;
        if (_deathTimer > waitLimit) {
            restartLevel();
        }
        return;
    }

    if (!_levelWon && _level->endXPos > 0.0f && _playerWorldX >= _level->endXPos - 600.0f) {
        _levelWon = true;
        _triggerEndPortal();
    }

    if (_levelWon) {
        if (_endCameraOverride && _endCamTweenActive) {
            _endCamTweenTime += dt;
            float t = std::min(_endCamTweenTime / 1.2f, 1.0f);
            float p = (t < 0.5f) ? (std::pow(2.0f * t, 1.8f) * 0.5f) : (1.0f - std::pow(2.0f * (1.0f - t), 1.8f) * 0.5f);
            _cameraX = _endCamFromX + (_endCamToX - _endCamFromX) * p;
            _cameraY = _endCamFromY + (_endCamToY - _endCamFromY) * p;
            if (t >= 1.0f) _endCamTweenActive = false;
        }

        float dx = _cameraX - _prevCameraX;
        _slideGroundX += dx;
        _bgScrollX += dx * 0.1f;
        _prevCameraX = _cameraX;

        _player->update(dt, _playerWorldX, _cameraY, _cameraX);
        _level->stepGroundAnimation(dt);
        _level->updateGroundTiles(_slideGroundX, _cameraY, dt);
        _level->applyEnterEffects(_cameraX);

        _audio.update(dt);
        _endSequenceTimer += dt;
        _updateCompleteLightRays(dt);

        if (_endSequencePhase == 1 && _endSequenceTimer >= 1.95f) {
            _endSequencePhase = 2;
            _completeBannerVisible = true;
            _completeBannerTimer = 0.0f;

            float vX = _level->endXPos - _cameraX + 60.0f;
            float vY = flipY(_endPortalGameY) + _cameraY;

            WinEffects::drawExpandingRing(vX, vY, 10.0f, (float)screenWidth, 800.0f, true, false, colorGreenTint);
            WinEffects::drawExpandingRing(screenWidth * 0.5f, 250.0f, 10.0f, 1000.0f, 800.0f, true, false, colorGreenTint);

            for (int i = 0; i < 5; ++i) {
                WinEffects::drawExpandingRing(vX, vY, 10.0f, (float)screenWidth, 500.0f, false, true, colorGreenTint, i * 50.0f);
            }

            for (int i = 0; i < 10; ++i) {
                float d = std::max(0.0f, 150.0f * i + (rand() % 160 - 80.0f));
                WinEffects::spawnFinishParticles(colorGreenTint, colorCyanTint, d);
            }
        }

        if (_completeBannerVisible) {
            _completeBannerTimer += dt;
            if (_completeBannerTimer <= 0.66f) {
                float t = _completeBannerTimer / 0.66f;
                _completeBannerScale = easeElasticOut(t);
            } else if (_completeBannerTimer <= 1.54f) {
                _completeBannerScale = 1.1f;
            } else if (_completeBannerTimer <= 1.76f) {
                float t = (_completeBannerTimer - 1.54f) / 0.22f;
                _completeBannerScale = 1.1f * (1.0f - t * t);
            } else {
                _completeBannerVisible = false;
            }
        }

        if (_endSequencePhase == 2 && _endSequenceTimer >= 3.45f) {
            _endSequencePhase = 3;
            _showEndLayerUI = true;
        }

        if (_showEndLayerUI && !_endLayerHiding) {
            float p = std::clamp((_endSequenceTimer - 3.45f) / 1.0f, 0.0f, 1.0f);
            if (p >= 1.0f && !_starAwardStarted) {
                _starAwardStarted = true;
                _starAwardTimer = 0.0f;
            }

            if (_starAwardStarted) {
                _starAwardTimer += dt;
                float sp = std::min(_starAwardTimer / 0.3f, 1.0f);
                float sBounce = easeBounceOut(sp);
                _starAwardScale = 3.0f + (0.8f - 3.0f) * sBounce;
                _starAwardAlpha = sp;

                if (_starAwardTimer >= 0.10f && !_starAwardSoundPlayed) {
                    _starAwardSoundPlayed = true;
                    _audio.playEffect("highscoreGet02");

                    float starX = (screenWidth * 0.5f) + 225.0f;
                    float starY = 268.5f;
                    WinEffects::drawExpandingRing(starX, starY, 20.0f, 220.0f, 400.0f, true, false, 16776960);
                    WinEffects::spawnStarParticles(starX, starY, 30);
                }
            }
        }

        return;
    }

    _playTime += dt;
    _audio.update(dt);
    _level->updateAudioScale(_audio.getMeteringValue());

    float qDt = _quantizeDelta(dt);
    int subSteps = qDt > 0.0f ? std::clamp((int)std::round(4.0f * qDt), 1, 60) : 0;
    float subDt = subSteps > 0 ? (qDt / subSteps) * physicsConst09 : 0.0f;
    float subDx = subSteps > 0 ? (qDt / subSteps) : 0.0f;

    float preFrameY = _state.y;

    for (int i = 0; i < subSteps; ++i) {
        _state.lastY = _state.y;
        _player->updateJump(subDt);
        _state.y += _state.yVelocity * subDt;
        _player->checkCollisions(_playerWorldX - groundYOffset, _cameraY);
        _playerWorldX += subDx * gravityConst * physicsConst09;

        if (!_state.isFlying) {
            if (_state.onGround) _player->updateGroundRotation(subDt);
            else _player->updateRotateAction(fixedTimeStep);
        }
    }

    _state.lastY = preFrameY;

    if (!_endCameraOverride) {
        float targetX = _playerWorldX - groundYOffset;
        if (_level->endXPos > 0.0f) {
            float endLockX = _level->endXPos - (float)screenWidth;
            if (targetX >= endLockX - 200.0f) {
                _endCameraOverride = true;
                _endCamTweenActive = true;
                _endCamTweenTime = 0.0f;
                _endCamFromX = _cameraX;
                _endCamToX = endLockX;
                _endCamFromY = _cameraY;
                _endCamToY = -140.0f + _endPortalGameY;
            } else {
                _cameraX = targetX;
            }
        } else {
            _cameraX = targetX;
        }
    }

    if (_endCameraOverride && _endCamTweenActive) {
        _endCamTweenTime += dt;
        float t = std::min(_endCamTweenTime / 1.2f, 1.0f);
        float p = (t < 0.5f) ? (std::pow(2.0f * t, 1.8f) * 0.5f) : (1.0f - std::pow(2.0f * (1.0f - t), 1.8f) * 0.5f);
        _cameraX = _endCamFromX + (_endCamToX - _endCamFromX) * p;
        _cameraY = _endCamFromY + (_endCamToY - _endCamFromY) * p;
        if (t >= 1.0f) _endCamTweenActive = false;
    } else if (!_endCameraOverride) {
        _updateCameraY(qDt);
    }

    float dx = _cameraX - _prevCameraX;
    _slideGroundX += dx;
    _bgScrollX += dx * 0.1f;
    _prevCameraX = _cameraX;

    if (_state.isFlying) {
        _player->updateShipRotation(qDt);
    }

    for (const auto& ct : _level->checkColorTriggers(_playerWorldX)) {
        _colorManager.triggerColor(ct.index, { (int)(ct.r * 255), (int)(ct.g * 255), (int)(ct.b * 255) }, ct.duration);
    }
    _colorManager.step(dt);

    float bgR, bgG, bgB;
    _colorManager.getGLColor(ColorManager::COLOR_BG, bgR, bgG, bgB);
    float gR, gG, gB;
    _colorManager.getGLColor(ColorManager::COLOR_GROUND, gR, gG, gB);
    _level->setGroundColor(gR, gG, gB);

    _level->checkEnterEffectTriggers(_playerWorldX);
    _level->applyEnterEffects(_cameraX);

    _level->stepGroundAnimation(dt);
    _level->updateGroundTiles(_slideGroundX, _cameraY, dt);
    _level->updatePortals(dt, _cameraX);
    _player->update(dt, _playerWorldX, _cameraY, _cameraX);

    _level->updateEndPortalY(_cameraY, _state.isFlying);
    _endPortalGameY = _level->getEndPortalGameY();
}

void GameScene::_triggerEndPortal() {
    _player->playEndAnimation(_level->endXPos, [this]() {
        _levelComplete();
    }, _endPortalGameY);
}

void GameScene::_levelComplete() {
    _audio.fadeOutMusic(1500.0f);
    _audio.playEffect("endStart_02", 0.8f);

    _endSequencePhase = 1;
    _endSequenceTimer = 0.0f;
    _shakeTimer = 1.95f;
    _shakeIntensity = 4.5f;
    _flashAlpha = 1.0f;

    _startCompleteLightRays();

    static const std::vector<std::string> quotes = {
        "Awesome!", "Good\nJob!", "Well\nDone!", "Impressive!",
        "Amazing!", "Incredible!", "Skillful!", "Brilliant!",
        "Warp\nSpeed!", "You are...\nThe One!", "Challenge\nBreaker!",
        "Reflex\nMaster!", "Not\nbad!", "How is this\npossible!?"
    };
    _completeMessage = quotes[rand() % quotes.size()];
}

void GameScene::_renderNewBest() {
    if (!_newBestActive || _newBestScale <= 0.01f) return;

    float midX = screenWidth * 0.5f;
    float centerY = 300.0f;

    const AtlasFrame* nbAf = findAtlasFrame("GJ_newBest_001.png");
    float origW = nbAf ? nbAf->w : 170.0f;
    float origH = nbAf ? nbAf->h : 40.0f;
    float nbW = origW * _newBestScale;
    float nbH = origH * _newBestScale;

    drawAtlasFrame("GJ_newBest_001.png", midX, centerY - nbH * 0.5f, nbW, nbH);

    std::string pctStr = std::to_string(_lastPercent) + "%";
    drawBitmapText("bigFont", pctStr, midX, centerY + 2.0f * _newBestScale + 25.0f * _newBestScale,
                   1.1f * _newBestScale, 1.0f, 1.0f, 1.0f, 1.0f, true);
}

void GameScene::render() {
    float bgR, bgG, bgB;
    _colorManager.getGLColor(ColorManager::COLOR_BG, bgR, bgG, bgB);
    glClearColor(bgR, bgG, bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    if (_shakeTimer > 0.0f) {
        float sx = (((rand() % 200) / 100.0f) - 1.0f) * _shakeIntensity;
        float sy = (((rand() % 200) / 100.0f) - 1.0f) * _shakeIntensity;
        glTranslatef(sx, sy, 0.0f);
    }

    GLuint bgTex = BootScene::textures["game_bg_01"].id;
    if (bgTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bgTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glColor3f(bgR, bgG, bgB);

        const float baseBgW = 1024.0f;
        const float baseBgH = 1024.0f;

        float uvW = (float)screenWidth / baseBgW;
        float uvH = (float)screenHeight / baseBgH;

        float uvOffsetX = _bgScrollX / baseBgW;

        float bgInitY = baseBgH - (float)screenHeight - 180.0f;
        float bgPosY = bgInitY - _cameraY * 0.1f;
        float uvOffsetY = bgPosY / baseBgH;

        glBegin(GL_QUADS);
        glTexCoord2f(uvOffsetX,       uvOffsetY);       glVertex2f(0.0f, 0.0f);
        glTexCoord2f(uvOffsetX + uvW, uvOffsetY);       glVertex2f(screenWidth, 0.0f);
        glTexCoord2f(uvOffsetX + uvW, uvOffsetY + uvH); glVertex2f(screenWidth, screenHeight);
        glTexCoord2f(uvOffsetX,       uvOffsetY + uvH); glVertex2f(0.0f, screenHeight);
        glEnd();
    }

    if (!_menuActive) {
        _level->renderLayer0(_cameraX, _cameraY);
        _level->renderLayer1(_cameraX, _cameraY);
    }

    if (!_flightGlitters.empty()) {
        applyBlendMode(BLEND_ADD);
        for (const auto& fg : _flightGlitters) {
            float pt = fg.life / fg.maxLife;
            float sc = fg.scale * (1.0f - pt);
            float alpha = 1.0f - pt;
            float size = 20.0f * sc;
            drawAtlasFrame("square.png", fg.x - _cameraX, fg.y + _cameraY, size, size, 0.0f, 0.0f, 1.0f, 0.0f, alpha);
        }
        applyBlendMode(BLEND_NORMAL);
    }

    if (!_menuActive) {
        _player->render(_cameraX, _cameraY);
        _level->renderLayer2(_cameraX, _cameraY);
    }

    _level->renderGround(_slideGroundX, _cameraY);

    _renderCompleteLightRays();
    WinEffects::render();

    if (_flashAlpha > 0.0f) {
        glDisable(GL_TEXTURE_2D);
        applyBlendMode(BLEND_ADD);
        glColor4f(0.3f, 1.0f, 0.5f, _flashAlpha * 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(screenWidth, 0.0f);
        glVertex2f(screenWidth, screenHeight);
        glVertex2f(0.0f, screenHeight);
        glEnd();
        applyBlendMode(BLEND_NORMAL);
        glEnable(GL_TEXTURE_2D);
    }

    glPopMatrix();

    _renderNewBest();

    if (_completeBannerVisible && _completeBannerScale > 0.02f) {
        const AtlasFrame* lcAf = findAtlasFrame("GJ_levelComplete_001.png");
        float lw = (lcAf ? lcAf->w : 400.0f) * _completeBannerScale;
        float lh = (lcAf ? lcAf->h : 80.0f) * _completeBannerScale;
        drawAtlasFrame("GJ_levelComplete_001.png", screenWidth * 0.5f, 250.0f, lw, lh);
    }

    if (_menuActive || _isMenuAnimatingOut) {
        _renderMenu();
    } else {
        _renderHUD();
    }

    if (_paused && !_showEndLayerUI) {
        _renderPauseOverlay();
    }

    if (_showEndLayerUI) {
        _renderEndLayer();
    }

    if (_showInfoPopup) {
        _renderInfoPopup();
    }

    if (_blackFadeAlpha > 0.001f) {
        glDisable(GL_TEXTURE_2D);
        applyBlendMode(BLEND_NORMAL);
        glColor4f(0.0f, 0.0f, 0.0f, _blackFadeAlpha);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(screenWidth, 0.0f);
        glVertex2f(screenWidth, screenHeight);
        glVertex2f(0.0f, screenHeight);
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
}

void GameScene::_renderHUD() {
    if (_attempts > 0 && _playerWorldX < 1200.0f) {
        float alpha = std::clamp(1.0f - (_playerWorldX / 1000.0f), 0.0f, 1.0f);
        float posX = screenWidth * 0.5f + (_attempts > 1 ? 100.0f : 0.0f);
        drawBitmapText("bigFont", "Attempt " + std::to_string(_attempts),
                       posX, 150.0f, 0.85f, 1.0f, 1.0f, 1.0f, alpha, true);
    }
    drawAtlasFrame("GJ_pauseBtn_clean_001.png", screenWidth - 30.0f, 30.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 75.0f / 255.0f);
}

void GameScene::_renderMenu() {
    float t = std::min(_menuAnimTimer / 0.3f, 1.0f);
    float tPlay = std::min(_menuAnimTimer / 0.2f, 1.0f);

    float ease = t * t;
    float playExitScale = 1.0f - (tPlay * tPlay);

    float midX = screenWidth * 0.5f;

    applyBlendMode(BLEND_ADD);
    for (const auto& mp : _menuParticles) {
        float alpha = 0.6f * (1.0f - mp.life / mp.maxLife) * (1.0f - t);
        float sc = mp.scale * (1.0f - mp.life / mp.maxLife);
        drawAtlasFrame("square.png", mp.x, mp.y, 20.0f * sc, 20.0f * sc, 0.0f, 0.0f, 0.314f, 0.745f, alpha);
    }
    applyBlendMode(BLEND_NORMAL);

    drawAtlasFrame("GJ_logo_001.png", midX, 100.0f - ease * 200.0f, 0.0f, 0.0f);
    drawAtlasFrame("tryMe_001.png", midX + 175.0f, 182.5f - ease * 250.0f, 0.0f, 0.0f);

    if (playExitScale > 0.01f) {
        const AtlasFrame* playFrame = findAtlasFrame("GJ_playBtn_001.png");
        float baseScale = _btnAnims[BTN_MENU_PLAY].scale;
        float pw = (playFrame ? playFrame->w : 126.0f) * playExitScale * baseScale;
        float ph = (playFrame ? playFrame->h : 126.0f) * playExitScale * baseScale;
        drawAtlasFrame("GJ_playBtn_001.png", midX, _menuPlayBtnY, pw, ph);
    }

    const AtlasFrame* robFrame = findAtlasFrame("RobTopLogoBig_001.png");
    float rw = (robFrame ? robFrame->w : 150.0f) * 0.9f;
    float rh = (robFrame ? robFrame->h : 50.0f) * 0.9f;
    drawAtlasFrame("RobTopLogoBig_001.png", 160.0f, 555.0f + ease * 150.0f, rw, rh);

    auto drawStoreBtn = [&](const std::string& name, ButtonId id, float px) {
        const AtlasFrame* af = findAtlasFrame(name);
        float curScale = _btnAnims[id].scale;
        float bw = (af ? af->w : 140.0f) * curScale;
        float bh = (af ? af->h : 45.0f) * curScale;
        drawAtlasFrame(name, px, 555.0f + ease * 150.0f, bw, bh);
    };

    drawStoreBtn("downloadSteam_001.png",  BTN_MENU_STEAM,  screenWidth - 130.0f);
    drawStoreBtn("downloadGoogle_001.png", BTN_MENU_GOOGLE, screenWidth - 340.0f);
    drawStoreBtn("downloadApple_001.png",  BTN_MENU_APPLE,  screenWidth - 550.0f);

    auto drawCornerIcon = [&](const std::string& name, ButtonId id, float px, float py, float r, float g, float b, float a) {
        const AtlasFrame* af = findAtlasFrame(name);
        float curScale = _btnAnims[id].scale;
        float iw = (af ? af->w : 40.0f) * curScale;
        float ih = (af ? af->h : 40.0f) * curScale;
        drawAtlasFrame(name, px, py - ease * 100.0f, iw, ih, 0.0f, r, g, b, a);
    };

    std::string fsTexture = _isFullscreen ? "toggleFullscreenOff_001.png" : "toggleFullscreenOn_001.png";
    float cornerAlpha = 0.8f * (1.0f - ease);

    drawCornerIcon(fsTexture, BTN_MENU_FS, 33.0f, 33.0f, 0.0f, 102.0f / 255.0f, 1.0f, cornerAlpha);
    drawCornerIcon("GJ_infoIcon_001.png", BTN_MENU_INFO, screenWidth - 33.0f, 33.0f, 0.0f, 102.0f / 255.0f, 1.0f, cornerAlpha);

    drawGenericText("© 2026 RobTop Games · geometrydash.com", screenWidth - 20.0f, 625.0f + ease * 150.0f, 14.0f, 1.0f, 1.0f, 1.0f, 0.30f, 2);
}

void GameScene::_renderPauseOverlay() {
    float midX = screenWidth * 0.5f;

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 75.0f / 255.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(screenWidth, 0.0f);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0.0f, screenHeight);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    drawScale9("square04_001", midX, 320.0f, screenWidth - 40.0f, 600.0f, 35.0f, 0.0f, 0.0f, 0.0f, 150.0f / 255.0f);

    std::string fsTexture = _isFullscreen ? "toggleFullscreenOff_001.png" : "toggleFullscreenOn_001.png";
    const AtlasFrame* fsFrame = findAtlasFrame(fsTexture);
    float fsScale = _btnAnims[BTN_PAUSE_FS].scale;
    float fsw = (fsFrame ? fsFrame->w : 40.0f) * fsScale;
    float fsh = (fsFrame ? fsFrame->h : 40.0f) * fsScale;
    drawAtlasFrame(fsTexture, 60.0f, 60.0f, fsw, fsh);

    drawBitmapText("bigFont", "Stereo Madness", midX, 65.0f, 0.70f, 1.0f, 1.0f, 1.0f, 1.0f, true);
    drawBitmapText("bigFont", "Normal Mode", midX, 130.0f, 0.55f, 1.0f, 1.0f, 1.0f, 1.0f, true);

    const AtlasFrame* barFrame = findAtlasFrame("GJ_progressBar_001.png");
    float origW = (barFrame && barFrame->w > 0.0f) ? barFrame->w : 680.0f;
    float origH = (barFrame && barFrame->h > 0.0f) ? barFrame->h : 40.0f;

    drawAtlasFrame("GJ_progressBar_001.png", midX, 170.0f, origW, origH, 0.0f, 0.0f, 0.0f, 0.0f, 125.0f / 255.0f);

    int percent = std::clamp(_bestPercent, 0, 100);
    if (percent > 0 && barFrame && BootScene::textures.find("GJ_WebSheet") != BootScene::textures.end()) {
        GLuint texID = BootScene::textures["GJ_WebSheet"].id;

        float scaleX = 0.992f;
        float scaleY = 0.86f;
        float totalW = origW * scaleX;
        float totalH = origH * scaleY;

        float startX = midX - (totalW * 0.5f);
        float startY = 170.0f - (totalH * 0.5f);

        float cropW = std::max(1.0f, std::floor(origW * (percent / 100.0f)));
        float drawW = cropW * scaleX;

        float u0 = barFrame->u0;
        float v0 = barFrame->v0;
        float u1 = u0 + (barFrame->u1 - u0) * (cropW / origW);
        float v1 = barFrame->v1;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        glBegin(GL_QUADS);
        glTexCoord2f(u0, v0); glVertex2f(startX,         startY);
        glTexCoord2f(u1, v0); glVertex2f(startX + drawW, startY);
        glTexCoord2f(u1, v1); glVertex2f(startX + drawW, startY + totalH);
        glTexCoord2f(u0, v1); glVertex2f(startX,         startY + totalH);
        glEnd();
    }

    drawBitmapText("bigFont", std::to_string(percent) + "%", midX, 170.0f, 0.50f, 1.0f, 1.0f, 1.0f, 1.0f, true);

    struct PauseBtnDef {
        std::string frame;
        ButtonId id;
    };
    PauseBtnDef pauseBtns[3] = {
        {"GJ_replayBtn_001.png", BTN_PAUSE_REPLAY},
        {"GJ_playBtn2_001.png",  BTN_PAUSE_PLAY},
        {"GJ_menuBtn_001.png",   BTN_PAUSE_MENU}
    };

    float btnWidths[3];
    float totalBtnW = 0.0f;
    for (int i = 0; i < 3; ++i) {
        const AtlasFrame* af = findAtlasFrame(pauseBtns[i].frame);
        btnWidths[i] = (af && af->w > 0.0f) ? af->w : 85.0f;
        totalBtnW += btnWidths[i];
    }
    totalBtnW += 40.0f * (3 - 1);

    float btnStartX = midX - totalBtnW * 0.5f;
    for (int i = 0; i < 3; ++i) {
        float posX = btnStartX + btnWidths[i] * 0.5f;
        float posY = 330.0f;
        const AtlasFrame* af = findAtlasFrame(pauseBtns[i].frame);
        float sc = _btnAnims[pauseBtns[i].id].scale;
        float bw = (af ? af->w : 85.0f) * sc;
        float bh = (af ? af->h : 85.0f) * sc;
        drawAtlasFrame(pauseBtns[i].frame, posX, posY, bw, bh);
        btnStartX += btnWidths[i] + 40.0f;
    }

    float grooveScale = 0.7f;
    const AtlasFrame* grooveAf = findAtlasFrame("slidergroove.png");
    float origGrooveW = (grooveAf && grooveAf->w > 0.0f) ? grooveAf->w : 420.0f;
    float grooveW = origGrooveW * grooveScale;
    float grooveH = (grooveAf ? grooveAf->h : 24.0f) * grooveScale;
    float trackW = (origGrooveW - 8.0f) * grooveScale;
    float barHeight = 11.2f;

    auto drawSliderTileBar = [&](float startX, float fillWidth) {
        if (fillWidth <= 0.0f) return;
        auto it = BootScene::textures.find("sliderBar");
        if (it == BootScene::textures.end() || it->second.id == 0) return;

        GLuint texID = it->second.id;
        const float baseBarTexW = 16.0f;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        float uRepeat = fillWidth / baseBarTexW;
        float yTop = 500.0f - (barHeight * 0.5f);
        float yBot = 500.0f + (barHeight * 0.5f);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,    0.0f); glVertex2f(startX,             yTop);
        glTexCoord2f(uRepeat, 0.0f); glVertex2f(startX + fillWidth, yTop);
        glTexCoord2f(uRepeat, 1.0f); glVertex2f(startX + fillWidth, yBot);
        glTexCoord2f(0.0f,    1.0f); glVertex2f(startX,             yBot);
        glEnd();
    };

    float musicCenterX = midX - 200.0f;
    float musicStartX = musicCenterX - (origGrooveW * grooveScale) * 0.5f + 2.8f;
    float mVol = std::clamp(_audio.getUserMusicVolume(), 0.0f, 1.0f);
    float mFillW = (mVol < 0.03f) ? 0.0f : (mVol * trackW);

    const AtlasFrame* mIconAf = findAtlasFrame("gj_songIcon_001.png");
    float miw = (mIconAf ? mIconAf->w : 36.0f) * 1.2f;
    float mih = (mIconAf ? mIconAf->h : 36.0f) * 1.2f;
    drawAtlasFrame("gj_songIcon_001.png", musicCenterX - 185.0f, 500.0f, miw, mih);
    drawSliderTileBar(musicStartX, mFillW);
    drawAtlasFrame("slidergroove.png", musicCenterX, 500.0f, grooveW, grooveH);

    std::string mThumbName = _draggingMusicSlider ? "sliderthumbsel.png" : "sliderthumb.png";
    const AtlasFrame* mThAf = findAtlasFrame(mThumbName);
    float mthw = (mThAf ? mThAf->w : 38.0f) * grooveScale;
    float mthh = (mThAf ? mThAf->h : 48.0f) * grooveScale;
    drawAtlasFrame(mThumbName, musicStartX + (mVol * trackW), 500.0f, mthw, mthh);

    float sfxCenterX = midX + 200.0f;
    float sfxStartX = sfxCenterX - (origGrooveW * grooveScale) * 0.5f + 2.8f;
    float sVol = std::clamp(_sfxVolume, 0.0f, 1.0f);
    float sFillW = (sVol < 0.03f) ? 0.0f : (sVol * trackW);

    const AtlasFrame* sIconAf = findAtlasFrame("GJ_sfxIcon_001.png");
    float siw = (sIconAf ? sIconAf->w : 36.0f) * 1.2f;
    float sih = (sIconAf ? sIconAf->h : 36.0f) * 1.2f;
    drawAtlasFrame("GJ_sfxIcon_001.png", sfxCenterX - 185.0f, 500.0f, siw, sih);
    drawSliderTileBar(sfxStartX, sFillW);
    drawAtlasFrame("slidergroove.png", sfxCenterX, 500.0f, grooveW, grooveH);

    std::string sThumbName = _draggingSfxSlider ? "sliderthumbsel.png" : "sliderthumb.png";
    const AtlasFrame* sThAf = findAtlasFrame(sThumbName);
    float sthw = (sThAf ? sThAf->w : 38.0f) * grooveScale;
    float sthh = (sThAf ? sThAf->h : 48.0f) * grooveScale;
    drawAtlasFrame(sThumbName, sfxStartX + (sVol * trackW), 500.0f, sthw, sthh);
}

void GameScene::_renderInfoPopup() {
    float midX = screenWidth * 0.5f;

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 100.0f / 255.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(screenWidth, 0.0f);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(0.0f, screenHeight);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    drawScale9("GJ_square02", midX, 320.0f, 480.0f, 336.0f, 35.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    const AtlasFrame* closeAf = findAtlasFrame("GJ_closeBtn_001.png");
    float closeSc = _btnAnims[BTN_INFO_CLOSE].scale;
    float cw = (closeAf ? closeAf->w : 40.0f) * closeSc;
    float ch = (closeAf ? closeAf->h : 40.0f) * closeSc;
    drawAtlasFrame("GJ_closeBtn_001.png", midX - 220.0f, 172.0f, cw, ch);

    drawBitmapText("bigFont", "Credits", midX, 206.0f, 0.75f, 1.0f, 1.0f, 1.0f, 1.0f, true);
    drawBitmapText("goldFont", "Made by RobTop Games", midX, 276.0f, 0.60f, 1.0f, 0.8f, 0.2f, 1.0f, true);
    drawBitmapText("goldFont", "Song: Stereo Madness", midX, 336.0f, 0.60f, 1.0f, 0.8f, 0.2f, 1.0f, true);
    drawBitmapText("goldFont", "by ForeverBound", midX - 20.0f, 366.0f, 0.60f, 1.0f, 0.8f, 0.2f, 1.0f, true);

    float textW = 145.0f;
    const BitmapFont* gf = getFont("goldFont");
    if (gf) {
        float tw = 0.0f;
        for (char ch : std::string("by ForeverBound")) {
            auto it = gf->chars.find((unsigned char)ch);
            if (it != gf->chars.end()) tw += it->second.xAdvance * 0.60f;
        }
        if (tw > 0.0f) textW = tw;
    }

    const AtlasFrame* ytAf = findAtlasFrame("gj_ytIcon_001.png");
    float ytSc = _btnAnims[BTN_INFO_YT].scale;
    float ytw = (ytAf ? ytAf->w : 64.0f) * ytSc;
    float yth = (ytAf ? ytAf->h : 44.0f) * ytSc;
    float textRightEdge = (midX - 20.0f) + (textW * 0.5f);
    float ytX = textRightEdge + 20.0f + 40.0f;
    drawAtlasFrame("gj_ytIcon_001.png", ytX, 368.0f, ytw, yth);

    drawGenericText("© 2026 RobTop Games. All rights reserved.", midX, 446.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.7f, 1);
    drawGenericText("Unauthorized copying, distribution, or hosting of this demo is prohibited.", midX, 463.0f, 12.0f, 0.0f, 0.0f, 0.0f, 0.7f, 1);
}

void GameScene::_renderEndLayer() {
    float midX = screenWidth * 0.5f;
    float dropOffsetY = 0.0f;
    float overlayAlpha = 0.0f;

    if (_endLayerHiding) {
        float t = std::min(_endLayerHideTimer / 0.5f, 1.0f);
        float ease = (t < 0.5f) ? (2.0f * t * t) : (1.0f - 2.0f * (1.0f - t) * (1.0f - t));
        dropOffsetY = -640.0f * ease;
        overlayAlpha = (100.0f / 255.0f) * (1.0f - t);
    } else {
        float p = 1.0f;
        if (_endSequencePhase >= 3) {
            p = std::clamp((_endSequenceTimer - 3.45f) / 1.0f, 0.0f, 1.0f);
        }
        float bp = easeBounceOut(p);
        dropOffsetY = (650.0f * bp - 640.0f);
        overlayAlpha = (100.0f / 255.0f) * p;
    }

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, overlayAlpha);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f); glVertex2f(screenWidth, 0.0f);
    glVertex2f(screenWidth, screenHeight); glVertex2f(0.0f, screenHeight);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(0.0f, dropOffsetY, 0.0f);

    const float boxW = 712.0f;
    const float boxH = 460.0f;
    const float sideOffset = (boxW * 0.5f) - 31.0f;

    const AtlasFrame* chainAf = findAtlasFrame("chain_01_001.png");
    float chw = chainAf ? chainAf->w : 22.0f;
    float chh = 90.0f;
    float chainBottomY = 5.0f;
    drawAtlasFrame("chain_01_001.png", midX - 312.0f, chainBottomY - chh * 0.5f, chw, chh);
    drawAtlasFrame("chain_01_001.png", midX + 312.0f, chainBottomY - chh * 0.5f, chw, chh);

    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 180.0f / 255.0f);
    glBegin(GL_QUADS);
    glVertex2f(midX - boxW * 0.5f, 310.0f - boxH * 0.5f);
    glVertex2f(midX + boxW * 0.5f, 310.0f - boxH * 0.5f);
    glVertex2f(midX + boxW * 0.5f, 310.0f + boxH * 0.5f);
    glVertex2f(midX - boxW * 0.5f, 310.0f + boxH * 0.5f);
    glEnd();
    glEnable(GL_TEXTURE_2D);

    const AtlasFrame* sideAf = findAtlasFrame("GJ_table_side_001.png");
    float sw = sideAf ? sideAf->w : 40.0f;
    drawAtlasFrame("GJ_table_side_001.png", midX - sideOffset, 310.0f, sw, boxH);
    drawAtlasFrame("GJ_table_side_001.png", midX + sideOffset, 310.0f, sw, boxH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, true, false);

    drawAtlasFrame("GJ_table_top_001.png", midX, 70.0f, 0.0f, 0.0f);
    drawAtlasFrame("GJ_table_bottom_001.png", midX, 560.0f, 0.0f, 0.0f);

    const AtlasFrame* titleAf = findAtlasFrame("GJ_levelComplete_001.png");
    float tlw = (titleAf ? titleAf->w : 400.0f) * 0.8f;
    float tlh = (titleAf ? titleAf->h : 80.0f) * 0.8f;
    drawAtlasFrame("GJ_levelComplete_001.png", midX, 170.0f, tlw, tlh);

    float statsScale = 0.8f;
    drawBitmapText("goldFont", "Attempts: " + std::to_string(_attempts), midX, 250.0f, statsScale, 1.0f, 0.8f, 0.2f, 1.0f, true);
    drawBitmapText("goldFont", "Jumps: " + std::to_string(_totalJumps), midX, 298.0f, statsScale, 1.0f, 0.8f, 0.2f, 1.0f, true);
    drawBitmapText("goldFont", "Time: " + formatPlayTime(_playTime), midX, 346.0f, statsScale, 1.0f, 0.8f, 0.2f, 1.0f, true);

    drawBitmapText("bigFont", _completeMessage, midX + 225.0f, 346.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, true);

    if (_starAwardStarted && _starAwardAlpha > 0.01f) {
        const AtlasFrame* starAf = findAtlasFrame("GJ_bigStar_001.png");
        float stw = (starAf ? starAf->w : 64.0f) * _starAwardScale;
        float sth = (starAf ? starAf->h : 64.0f) * _starAwardScale;
        drawAtlasFrame("GJ_bigStar_001.png", midX + 225.0f, 268.5f, stw, sth, 0.0f, 1.0f, 1.0f, 1.0f, _starAwardAlpha);
    }

    const AtlasFrame* getItAf = findAtlasFrame("getIt_001.png");
    float giw = (getItAf ? getItAf->w : 120.0f) / 1.5f;
    float gih = (getItAf ? getItAf->h : 60.0f) / 1.5f;
    drawAtlasFrame("getIt_001.png", midX - 225.0f, 352.5f, giw, gih);

    auto drawEndStoreBtn = [&](const std::string& name, ButtonId id, float px) {
        const AtlasFrame* af = findAtlasFrame(name);
        float sc = _btnAnims[id].scale;
        float bw = (af ? af->w : 140.0f) * sc;
        float bh = (af ? af->h : 45.0f) * sc;
        drawAtlasFrame(name, px, 437.5f, bw, bh);
    };

    drawEndStoreBtn("downloadApple_001.png",  BTN_END_APPLE,  midX - 225.0f);
    drawEndStoreBtn("downloadGoogle_001.png", BTN_END_GOOGLE, midX);
    drawEndStoreBtn("downloadSteam_001.png",  BTN_END_STEAM,  midX + 225.0f);

    auto drawEndNavBtn = [&](const std::string& name, ButtonId id, float px, float py) {
        const AtlasFrame* af = findAtlasFrame(name);
        float sc = _btnAnims[id].scale;
        float bw = (af ? af->w : 85.0f) * sc;
        float bh = (af ? af->h : 85.0f) * sc;
        drawAtlasFrame(name, px, py, bw, bh);
    };

    drawEndNavBtn("GJ_replayBtn_001.png", BTN_END_REPLAY, midX - 200.0f, 555.0f);
    drawEndNavBtn("GJ_menuBtn_001.png",   BTN_END_MENU,   midX + 200.0f, 555.0f);

    glPopMatrix();
}
