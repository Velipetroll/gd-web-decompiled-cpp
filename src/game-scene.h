#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "constants.h"
#include "player-physics-state.h"
#include "level-renderer.h"
#include "player.h"
#include "color-manager.h"
#include "audio-manager.h"

struct MenuGlitter {
    float x, y, life, maxLife, scale;
};

struct CompleteLightRay {
    float angleDeg = 0.0f;
    float targetW = 60.0f;
    float targetH = 1250.0f;
    float currentW = 2.0f;
    float currentH = 1.0f;
    float maxAlpha = 0.8f;
    float currentAlpha = 0.0f;
    float delay = 0.0f;
    float duration = 0.18f;
    float elapsed = 0.0f;
    float fadeDelay = 0.0f;
    float fadeDuration = 0.4f;
    float fadeElapsed = 0.0f;
    bool started = false;
    bool fading = false;
    bool done = false;
};

class GameScene {
public:
    GameScene();
    ~GameScene();

    void init();
    void handleEvent(const SDL_Event& event, int windowW, int windowH, SDL_Window* window = nullptr);
    void update(float dt);
    void render();

    void startGame();
    void restartLevel();
    void pauseGame();
    void resumeGame();

    void pushButton();
    void releaseButton();

    bool isMenuActive() const { return _menuActive; }
    bool isPaused() const { return _paused; }

private:
    void _updateBackground(float dt);
    void _updateCameraY(float dt);
    float _quantizeDelta(float dt);
    void _resetGameplayState();
    void _triggerEndPortal();
    void _levelComplete();
    void _showNewBest();

    void _startCompleteLightRays();
    void _updateCompleteLightRays(float dt);
    void _renderCompleteLightRays();

    void _renderHUD();
    void _renderMenu();
    void _renderPauseOverlay();
    void _renderEndLayer();
    void _renderInfoPopup();

    PlayerPhysicsState _state;
    std::unique_ptr<LevelRenderer> _level;
    std::unique_ptr<Player> _player;
    ColorManager _colorManager;
    AudioManager _audio;

    float _cameraX;
    float _cameraY;
    float _prevCameraX;
    float _menuCameraX;
    float _playerWorldX;
    float _slideGroundX;
    float _bgScrollX;

    int _fadeState = 0;
    float _fadeTimer = 0.0f;
    float _blackFadeAlpha = 0.0f;

    bool _menuActive = true;
    bool _slideIn = false;
    bool _paused = false;
    bool _levelWon = false;
    bool _firstPlay = true;
    bool _isFullscreen = false;

    bool _endCameraOverride = false;
    bool _endCamTweenActive = false;
    float _endCamTweenTime = 0.0f;
    float _endCamFromX = 0.0f, _endCamToX = 0.0f;
    float _endCamFromY = 0.0f, _endCamToY = 0.0f;

    int _endSequencePhase = 0;
    float _endSequenceTimer = 0.0f;
    float _shakeTimer = 0.0f;
    float _shakeIntensity = 0.0f;
    float _flashAlpha = 0.0f;
    std::vector<CompleteLightRay> _lightRays;
    float _lightRaysOriginX = 0.0f;
    float _lightRaysOriginY = 0.0f;

    bool _completeBannerVisible = false;
    float _completeBannerTimer = 0.0f;
    float _completeBannerScale = 0.01f;

    bool _starAwardStarted = false;
    bool _starAwardSoundPlayed = false;
    float _starAwardTimer = 0.0f;
    float _starAwardScale = 3.0f;
    float _starAwardAlpha = 0.0f;

    bool _showInfoPopup = false;

    bool _draggingMusicSlider = false;
    bool _draggingSfxSlider = false;
    float _sfxVolume = 1.0f;

    bool _isMenuAnimatingOut = false;
    float _menuAnimTimer = 0.0f;

    float _deltaBuffer = 0.0f;

    int _attempts = 1;
    int _bestPercent = 0;
    int _lastPercent = 0;
    int _totalJumps = 0;
    float _playTime = 0.0f;
    float _deathTimer = 0.0f;
    bool _deathSoundPlayed = false;
    bool _newBestShown = false;
    bool _hadNewBest = false;

    float _menuPlayBtnY = 320.0f;
    float _menuPlayTimer = 0.0f;
    std::vector<MenuGlitter> _menuParticles;

    float _endPortalGameY = 240.0f;
    bool _showEndLayerUI = false;
    std::string _completeMessage = "Awesome!";
};
