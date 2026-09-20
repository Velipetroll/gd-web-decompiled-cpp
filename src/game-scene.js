class GameScene extends Phaser["Scene"] {
  constructor() {
    super({
      key: "GameScene",
    });
  }
  ["create"]() {
    ((this["_bgSpeedX"] = 0.1),
      (this["_bgSpeedY"] = 0.1),
      (this["_menuCameraX"] = -groundYOffset),
      (this["_prevCameraX"] = -groundYOffset),
      (this["_bg"] = this["add"]
        ["tileSprite"](0, 0, screenWidth, screenHeight, "game_bg_01")
        ["setOrigin"](0, 0)
        ["setScrollFactor"](0)
        ["setDepth"](-10)));
    const height = this["textures"]["get"]("game_bg_01")["source"][0]["height"];
    ((this["_bgInitY"] = height - screenHeight - unusedConst180),
      (this["_cameraX"] = -groundYOffset),
      (this["_cameraY"] = 0),
      (this["_cameraXRef"] = {
        get value() {
          return this["_v"];
        },
        _v: -groundYOffset,
      }),
      (this["_state"] = new PlayerPhysicsState()),
      (this["_level"] = new LevelRenderer(this, this["_cameraXRef"])),
      (this["_player"] = new Player(this, this["_state"], this["_level"])),
      (this["_colorManager"] = new ColorManager()),
      (this["_audio"] = new AudioManager(this)));
    let text = this["cache"]["text"]["get"]("level_1");
    (text && this["_level"]["loadLevel"](text),
      this["_level"]["createEndPortal"](this),
      (this["_glitterCenterX"] = 0),
      (this["_glitterCenterY"] = yFlipBase),
      (this["_glitterEmitter"] = this["add"]["particles"](0, 0, "GJ_WebSheet", {
        frame: "square.png",
        speed: 0,
        scale: {
          start: 0.375,
          end: 0,
        },
        alpha: {
          start: 1,
          end: 0,
        },
        lifespan: {
          min: 200,
          max: 1800,
        },
        frequency: 60,
        blendMode: blendAdd,
        tint: colorGreenTint,
        emitting: !1,
        emitCallback: (point) => {
          ((point["x"] =
            this["_glitterCenterX"] +
            (2 * Math["random"]() - 1) * (screenWidth / 1.8)),
            (point["y"] =
              this["_glitterCenterY"] + 320 * (2 * Math["random"]() - 1)));
        },
      })),
      this["_level"]["additiveContainer"]["add"](this["_glitterEmitter"]),
      this["_bg"]["setTint"](this["_colorManager"]["getHex"](num30)),
      this["_level"]["setGroundColor"](this["_colorManager"]["getHex"](num31)),
      this["_level"]["additiveContainer"]["setVisible"](!1),
      this["_level"]["container"]["setVisible"](!1),
      this["_level"]["topContainer"]["setVisible"](!1),
      (this["_attempts"] = 1),
      (this["_bestPercent"] = 0),
      (this["_lastPercent"] = 0),
      (this["_endPortalGameY"] = 240),
      this["_resetGameplayState"](),
      (this["_totalJumps"] = 0),
      (this["_playTime"] = 0),
      (this["_menuActive"] = !0),
      (this["_slideIn"] = !1),
      (this["_slideGroundX"] = null),
      (this["_firstPlay"] = !0),
      this["_player"]["setCubeVisible"](!1),
      this["_player"]["setShipVisible"](!1),
      (this["_logo"] = this["add"]
        ["image"](0, 100, "GJ_WebSheet", "GJ_logo_001.png")
        ["setScrollFactor"](0)
        ["setDepth"](30)),
      (this["_robLogo"] = this["add"]
        ["image"](160, 555, "GJ_WebSheet", "RobTopLogoBig_001.png")
        ["setScrollFactor"](0)
        ["setDepth"](30)
        ["setScale"](0.9)),
      (this["_copyrightText"] = this["add"]
        ["text"](0, 625, "© 2026 RobTop Games · geometrydash.com", {
          fontSize: "14px",
          color: "#ffffff",
          fontFamily: "Arial",
        })
        ["setOrigin"](1, 1)
        ["setScrollFactor"](0)
        ["setDepth"](30)
        ["setAlpha"](0.3)),
      (this["_tryMeImg"] = this["add"]
        ["image"](0, 182.5, "GJ_WebSheet", "tryMe_001.png")
        ["setScrollFactor"](0)
        ["setDepth"](30)),
      (this["_downloadBtns"] = []));
    const table4 = [
      {
        key: "downloadSteam_001",
        url: "https://store.steampowered.com/app/322170/Geometry_Dash",
      },
      {
        key: "downloadGoogle_001",
        url: "https://play.google.com/store/apps/details?id=com.robtopx.geometryjump&hl=en",
      },
      {
        key: "downloadApple_001",
        url: "https://apps.apple.com/us/app/geometry-dash/id625334537",
      },
    ];
    for (let i = 0; i < table4["length"]; i++) {
      const i2 = table4[i],
        value71 = 1 / 1.5,
        value72 = this["add"]
          ["image"](0, 0, "GJ_WebSheet", i2["key"] + ".png")
          ["setScrollFactor"](0)
          ["setDepth"](30)
          ["setScale"](value71)
          ["setInteractive"]();
      (this["_makeBouncyButton"](
        value72,
        value71,
        () => window["open"](i2["url"], "_blank"),
        () => this["_menuActive"],
      ),
        this["_downloadBtns"]["push"](value72));
    }
    const isFullscreen = this["scale"]["isFullscreen"];
    ((this["_menuFsBtn"] = this["add"]
      ["image"](
        33,
        33,
        "GJ_WebSheet",
        isFullscreen
          ? "toggleFullscreenOff_001.png"
          : "toggleFullscreenOn_001.png",
      )
      ["setScrollFactor"](0)
      ["setDepth"](30)
      ["setScale"](0.64)
      ["setAlpha"](0.8)
      ["setTint"](
        Phaser["Display"]["Color"]["GetColor"](0, Math["round"](102), 255),
      )
      ["setInteractive"]()),
      this["_expandHitArea"](this["_menuFsBtn"], 1.5),
      this["_makeBouncyButton"](
        this["_menuFsBtn"],
        0.64,
        () => {
          const value71 = !this["scale"]["isFullscreen"];
          (this["_menuFsBtn"]["setTexture"](
            "GJ_WebSheet",
            value71
              ? "toggleFullscreenOff_001.png"
              : "toggleFullscreenOn_001.png",
          ),
            this["_expandHitArea"](this["_menuFsBtn"], 1.5),
            this["_toggleFullscreen"]());
        },
        () => this["_menuActive"],
      ),
      (this["_menuInfoBtn"] = this["add"]
        ["image"](
          screenWidth - 30 - 3,
          33,
          "GJ_WebSheet",
          "GJ_infoIcon_001.png",
        )
        ["setScrollFactor"](0)
        ["setDepth"](30)
        ["setScale"](0.64)
        ["setAlpha"](0.8)
        ["setTint"](
          Phaser["Display"]["Color"]["GetColor"](0, Math["round"](102), 255),
        )
        ["setInteractive"]()),
      this["_expandHitArea"](this["_menuInfoBtn"], 1.5),
      this["_makeBouncyButton"](
        this["_menuInfoBtn"],
        0.64,
        () => {
          this["_buildInfoPopup"]();
        },
        () => this["_menuActive"] && !this["_infoPopup"],
      ),
      (this["_menuGlitter"] = this["add"]
        ["particles"](0, 0, "GJ_WebSheet", {
          frame: "square.png",
          speed: 0,
          scale: {
            start: 0.5,
            end: 0,
          },
          alpha: {
            start: 0.6,
            end: 0.2,
          },
          lifespan: {
            min: 1e3,
            max: 2e3,
          },
          frequency: 35,
          blendMode: blendAdd,
          tint: 20670,
          x: {
            min: -130,
            max: 130,
          },
          y: {
            min: -100,
            max: 100,
          },
        })
        ["setScrollFactor"](0)
        ["setDepth"](29)),
      (this["_playBtn"] = this["add"]
        ["image"](0, 0, "GJ_WebSheet", "GJ_playBtn_001.png")
        ["setScrollFactor"](0)
        ["setDepth"](30)
        ["setInteractive"]()),
      (this["_playBtnPressed"] = !1),
      this["_makeBouncyButton"](
        this["_playBtn"],
        1,
        () => {
          (this["_audio"]["playEffect"]("playSound_01", {
            volume: 1,
          }),
            this["_startGame"]());
        },
        () => this["_menuActive"] && !this["_playBtnPressed"],
      ),
      this["_positionMenuItems"](),
      (this["_spaceWasDown"] = !1),
      (this["_spaceKey"] = this["input"]["keyboard"]["addKey"](
        Phaser["Input"]["Keyboard"]["KeyCodes"]["SPACE"],
      )),
      (this["_upKey"] = this["input"]["keyboard"]["addKey"](
        Phaser["Input"]["Keyboard"]["KeyCodes"]["UP"],
      )),
      (this["_pauseBtn"] = this["add"]
        ["image"](
          screenWidth - 30,
          30,
          "GJ_WebSheet",
          "GJ_pauseBtn_clean_001.png",
        )
        ["setScrollFactor"](0)
        ["setDepth"](30)
        ["setAlpha"](75 / 255)
        ["setVisible"](!1)),
      this["_pauseBtn"]["setInteractive"](),
      this["_expandHitArea"](this["_pauseBtn"], 2),
      this["_pauseBtn"]["on"]("pointerdown", () => this["_pauseGame"]()),
      (this["_escKey"] = this["input"]["keyboard"]["addKey"](
        Phaser["Input"]["Keyboard"]["KeyCodes"]["ESC"],
      )),
      this["_escKey"]["on"]("down", () => {
        this["_paused"]
          ? this["_resumeGame"]()
          : this["_menuActive"] ||
            this["_slideIn"] ||
            this["_state"]["isDead"] ||
            this["_levelWon"] ||
            this["_pauseGame"]();
      }),
      (this["_paused"] = !1),
      (this["_pauseContainer"] = null),
      (this["_sfxVolume"] = this["game"]["registry"]["get"]("userSfxVol") ?? 1),
      this["input"]["on"]("pointerdown", () => {
        this["_menuActive"] || this["_paused"] || this["_pushButton"]();
      }),
      this["input"]["on"]("pointerup", () => {
        this["_menuActive"] || this["_paused"] || this["_releaseButton"]();
      }),
      window["addEventListener"]("pointerup", () => this["_releaseButton"]()),
      window["addEventListener"]("touchend", () => this["_releaseButton"]()),
      this["scale"]["on"]("enterfullscreen", () =>
        this["_onFullscreenChange"](!0),
      ),
      this["scale"]["on"]("leavefullscreen", () =>
        this["_onFullscreenChange"](!1),
      ),
      this["_buildHUD"](),
      document["addEventListener"]("visibilitychange", () => {
        document["hidden"]
          ? this["_audio"]["pauseMusic"]()
          : this["_menuActive"] ||
            this["_paused"] ||
            this["_state"]["isDead"] ||
            this["_levelWon"] ||
            this["_audio"]["resumeMusic"]();
      }),
      window["addEventListener"]("orientationchange", () => {
        this["time"]["delayedCall"](100, () => this["scale"]["refresh"]());
      }),
      window["addEventListener"]("resize", () => {
        this["scale"]["refresh"]();
      }),
      this["game"]["registry"]["get"]("fadeInFromBlack") &&
        (this["game"]["registry"]["remove"]("fadeInFromBlack"),
        this["cameras"]["main"]["fadeIn"](400, 0, 0, 0)));
  }
  ["_buildHUD"]() {
    ((this["_attemptsLabel"] = this["add"]
      ["bitmapText"](0, 0, "bigFont", "Attempt 1", 65)
      ["setOrigin"](0.5, 0.5)
      ["setVisible"](!1)),
      this["_level"]["topContainer"]["add"](this["_attemptsLabel"]),
      this["_positionAttemptsLabel"](),
      (this["_fpsText"] = this["add"]
        ["text"](screenWidth - 20, 10, "", {
          fontSize: "28px",
          fill: "#ff0000",
          fontFamily: "Arial",
        })
        ["setOrigin"](1, 0)
        ["setScrollFactor"](0)
        ["setDepth"](999)
        ["setVisible"](!1)),
      (this["_fpsAccum"] = 0),
      (this["_fpsFrames"] = 0),
      this["input"]["keyboard"]
        ["addKey"](Phaser["Input"]["Keyboard"]["KeyCodes"]["H"])
        ["on"]("down", () => {
          this["_fpsText"]["setVisible"](!this["_fpsText"]["visible"]);
        }));
  }
  ["toggleGlitter"](value71) {
    value71
      ? this["_glitterEmitter"]["start"]()
      : this["_glitterEmitter"]["stop"]();
  }
  ["_setParticleTimeScale"](value71) {
    const callback51 = (value72) => {
      (value72 &&
        "ParticleEmitter" === value72["type"] &&
        (value72["timeScale"] = value71),
        value72 && value72["list"] && value72["list"]["forEach"](callback51));
    };
    (callback51(this["_level"]["container"]),
      callback51(this["_level"]["topContainer"]),
      this["_glitterEmitter"] &&
        (this["_glitterEmitter"]["timeScale"] = value71));
  }
  ["_pauseGame"]() {
    this["_paused"] ||
      this["_menuActive"] ||
      this["_slideIn"] ||
      this["_state"]["isDead"] ||
      this["_levelWon"] ||
      ((this["_paused"] = !0),
      this["_pauseBtn"]["setVisible"](!1),
      this["_audio"]["pauseMusic"](),
      this["_setParticleTimeScale"](0),
      this["_buildPauseOverlay"]());
  }
  ["_resumeGame"]() {
    this["_paused"] &&
      (this["_setParticleTimeScale"](1),
      (this["_paused"] = !1),
      this["_pauseBtn"]["setVisible"](!0)["setAlpha"](75 / 255),
      this["_audio"]["resumeMusic"](),
      this["_pauseContainer"] &&
        (this["_pauseContainer"]["destroy"](),
        (this["_pauseContainer"] = null)));
  }
  ["_buildPauseOverlay"]() {
    const value71 = screenWidth / 2,
      num32 = 320,
      value72 = screenWidth - 40;
    this["_pauseContainer"] = this["add"]
      ["container"](0, 0)
      ["setScrollFactor"](0)
      ["setDepth"](100);
    const rectangle = this["add"]["rectangle"](
      value71,
      num32,
      screenWidth,
      screenHeight,
      0,
      75 / 255,
    );
    (rectangle["setInteractive"](), this["_pauseContainer"]["add"](rectangle));
    const value73 =
        0.325 * this["textures"]["get"]("square04_001")["source"][0]["width"],
      value74 = this["_drawScale9"](
        value71,
        num32,
        value72,
        600,
        "square04_001",
        value73,
        0,
        150 / 255,
      );
    this["_pauseContainer"]["add"](value74);
    const isFullscreen = this["scale"]["isFullscreen"],
      image = this["add"]
        ["image"](
          value71 - value72 / 2 + 40,
          60,
          "GJ_WebSheet",
          isFullscreen
            ? "toggleFullscreenOff_001.png"
            : "toggleFullscreenOn_001.png",
        )
        ["setScale"](0.64)
        ["setInteractive"]();
    (this["_expandHitArea"](image, 2.5),
      this["_pauseContainer"]["add"](image),
      this["_makeBouncyButton"](image, 0.64, () => {
        const value84 = !this["scale"]["isFullscreen"];
        (image["setTexture"](
          "GJ_WebSheet",
          value84
            ? "toggleFullscreenOff_001.png"
            : "toggleFullscreenOn_001.png",
        ),
          this["_expandHitArea"](image, 2.5),
          this["_toggleFullscreen"]());
      }),
      this["_pauseContainer"]["add"](
        this["add"]
          ["bitmapText"](value71, 65, "bigFont", "Stereo Madness", 40)
          ["setOrigin"](0.5, 0.5),
      ));
    const num33 = 170,
      value75 = this["_bestPercent"] || 0,
      image2 = this["add"]
        ["image"](value71, num33, "GJ_WebSheet", "GJ_progressBar_001.png")
        ["setTint"](0)
        ["setAlpha"](125 / 255);
    this["_pauseContainer"]["add"](image2);
    const value76 = this["textures"]["getFrame"](
        "GJ_WebSheet",
        "GJ_progressBar_001.png",
      ),
      value77 = value76 ? value76["width"] : 680,
      value78 = value76 ? value76["height"] : 40,
      value79 = Math["max"](1, Math["floor"](value77 * (value75 / 100))),
      gameObject = this["add"]
        ["image"](0, 0, "GJ_WebSheet", "GJ_progressBar_001.png")
        ["setTint"](65280)
        ["setScale"](0.992, 0.86)
        ["setOrigin"](0, 0.5)
        ["setCrop"](0, 0, value79, value78);
    (gameObject["setPosition"](value71 - (0.992 * value77) / 2, num33),
      this["_pauseContainer"]["add"](gameObject),
      this["_pauseContainer"]["add"](
        this["add"]
          ["bitmapText"](value71, num33, "bigFont", value75 + "%", 30)
          ["setOrigin"](0.5, 0.5)
          ["setScale"](0.7),
      ),
      this["_pauseContainer"]["add"](
        this["add"]
          ["bitmapText"](value71, 130, "bigFont", "Normal Mode", 30)
          ["setOrigin"](0.5, 0.5)
          ["setScale"](0.78),
      ));
    const table4 = [
        {
          frame: "GJ_replayBtn_001.png",
          action: () => {
            (this["_resumeGame"](), this["_restartLevel"]());
          },
        },
        {
          frame: "GJ_playBtn2_001.png",
          action: () => this["_resumeGame"](),
        },
        {
          frame: "GJ_menuBtn_001.png",
          action: () => {
            (this["_audio"]["playEffect"]("quitSound_01"),
              this["_audio"]["stopMusic"](),
              this["_resumeGame"](),
              this["scene"]["restart"]());
          },
        },
      ],
      value80 = table4["map"]((item) => {
        const value84 = this["textures"]["getFrame"](
          "GJ_WebSheet",
          item["frame"],
        );
        return value84 ? value84["width"] : 246;
      });
    let value81 =
      value71 -
      (value80["reduce"]((accumulator, item) => accumulator + item, 0) +
        40 * (table4["length"] - 1)) /
        2;
    for (let i = 0; i < table4["length"]; i++) {
      const i2 = table4[i],
        i3 = value80[i],
        image3 = this["add"]
          ["image"](value81 + i3 / 2, 330, "GJ_WebSheet", i2["frame"])
          ["setInteractive"]();
      (this["_pauseContainer"]["add"](image3),
        this["_makeBouncyButton"](image3, 1, i2["action"]),
        (value81 += i3 + 40));
    }
    const num34 = 500,
      num35 = 0.7,
      value82 = this["textures"]["getFrame"]("GJ_WebSheet", "slidergroove.png"),
      value83 = value82 ? value82["width"] : 420,
      callback51 = (value84, other, extra, extra2) => {
        this["_pauseContainer"]["add"](
          this["add"]
            ["image"](value84 - 180 - 5, num34, "GJ_WebSheet", other)
            ["setScale"](1.2),
        );
        const value85 = (value83 - 8) * num35,
          value86 = value84 - (value83 * num35) / 2 + 2.8,
          value87 = extra * value85,
          tileSprite = this["add"]
            ["tileSprite"](
              value86,
              num34,
              value87 > 0 ? value87 : 1,
              11.2,
              "sliderBar",
            )
            ["setOrigin"](0, 0.5)
            ["setVisible"](value87 > 0);
        this["_pauseContainer"]["add"](tileSprite);
        const image3 = this["add"]
          ["image"](value84, num34, "GJ_WebSheet", "slidergroove.png")
          ["setScale"](num35);
        this["_pauseContainer"]["add"](image3);
        const value88 = value86 + extra * value85,
          image4 = this["add"]
            ["image"](value88, num34, "GJ_WebSheet", "sliderthumb.png")
            ["setScale"](num35)
            ["setInteractive"]({
              draggable: !0,
              useHandCursor: !0,
            });
        (this["_pauseContainer"]["add"](image4),
          image4["on"]("pointerdown", () =>
            image4["setTexture"]("GJ_WebSheet", "sliderthumbsel.png"),
          ),
          image4["on"]("pointerup", () =>
            image4["setTexture"]("GJ_WebSheet", "sliderthumb.png"),
          ),
          image4["on"]("pointerout", () =>
            image4["setTexture"]("GJ_WebSheet", "sliderthumb.png"),
          ),
          image4["on"]("drag", (drag, other2) => {
            image4["x"] = Math["max"](
              value86,
              Math["min"](value86 + value85, other2),
            );
            const value89 = (image4["x"] - value86) / value85,
              value90 = value89 < 0.03 ? 0 : value89;
            ((tileSprite["width"] = Math["max"](1, value90 * value85)),
              tileSprite["setVisible"](value90 > 0),
              extra2(value90));
          }));
      };
    (callback51(
      value71 - 200,
      "gj_songIcon_001.png",
      this["_audio"]["getUserMusicVolume"](),
      (value84) => this["_audio"]["setUserMusicVolume"](value84),
    ),
      callback51(
        value71 + 200,
        "GJ_sfxIcon_001.png",
        this["_sfxVolume"],
        (value84) => {
          ((this["_sfxVolume"] = value84),
            this["game"]["registry"]["set"]("userSfxVol", value84));
        },
      ));
  }
  ["_buildInfoPopup"]() {
    if (this["_infoPopup"]) return;
    const value71 = screenWidth / 2,
      num32 = 320,
      num33 = 336;
    this["_infoPopup"] = this["add"]
      ["container"](0, 0)
      ["setScrollFactor"](0)
      ["setDepth"](200);
    const rectangle = this["add"]["rectangle"](
      value71,
      num32,
      screenWidth,
      screenHeight,
      0,
      100 / 255,
    );
    (rectangle["setInteractive"](), this["_infoPopup"]["add"](rectangle));
    const value72 =
        0.325 * this["textures"]["get"]("GJ_square02")["source"][0]["width"],
      value73 = this["_drawScale9"](
        value71,
        num32,
        480,
        num33,
        "GJ_square02",
        value72,
        16777215,
        1,
      );
    this["_infoPopup"]["add"](value73);
    const image = this["add"]
      ["image"](value71 - 240 + 20, 172, "GJ_WebSheet", "GJ_closeBtn_001.png")
      ["setScale"](0.8)
      ["setInteractive"]();
    (this["_infoPopup"]["add"](image),
      this["_expandHitArea"](image, 2),
      this["_makeBouncyButton"](image, 0.8, () => this["_closeInfoPopup"]()));
    let num34 = 206;
    const bitmapText = this["add"]
      ["bitmapText"](value71, num34, "bigFont", "Credits", 40)
      ["setOrigin"](0.5, 0.5);
    (this["_infoPopup"]["add"](bitmapText), (num34 += 70));
    const bitmapText2 = this["add"]
      ["bitmapText"](value71, num34, "goldFont", "Made by RobTop Games", 40)
      ["setOrigin"](0.5, 0.5)
      ["setScale"](0.6);
    (this["_infoPopup"]["add"](bitmapText2), (num34 += 60));
    const bitmapText3 = this["add"]
      ["bitmapText"](value71, num34, "goldFont", "Song: Stereo Madness", 40)
      ["setOrigin"](0.5, 0.5)
      ["setScale"](0.6);
    (this["_infoPopup"]["add"](bitmapText3), (num34 += 30));
    const bitmapText4 = this["add"]
      ["bitmapText"](value71 - 20, num34, "goldFont", "by ForeverBound", 40)
      ["setOrigin"](0.5, 0.5)
      ["setScale"](0.6);
    this["_infoPopup"]["add"](bitmapText4);
    const value74 = value71 - 10 + (0.6 * bitmapText4["width"]) / 2,
      image2 = this["add"]
        ["image"](
          value74 + 20 + 50 - 10,
          num34 + 2,
          "GJ_WebSheet",
          "gj_ytIcon_001.png",
        )
        ["setScale"](0.5)
        ["setInteractive"]();
    (this["_infoPopup"]["add"](image2),
      this["_expandHitArea"](image2, 2),
      this["_makeBouncyButton"](image2, 0.5, () => {
        window["open"]("https://www.youtube.com/watch?v=JhKyKEDxo8Q", "_blank");
      }));
    const text = this["add"]
      ["text"](value71, 446, "© 2026 RobTop Games. All rights reserved.", {
        fontSize: "12px",
        color: "#000000",
        fontFamily: "Arial",
      })
      ["setOrigin"](0.5, 0.5)
      ["setAlpha"](0.7)
      ["setResolution"](2);
    this["_infoPopup"]["add"](text);
    const text2 = this["add"]
      ["text"](
        value71,
        463,
        "Unauthorized copying, distribution, or hosting of this demo is prohibited.",
        {
          fontSize: "12px",
          color: "#000000",
          fontFamily: "Arial",
        },
      )
      ["setOrigin"](0.5, 0.5)
      ["setAlpha"](0.7)
      ["setResolution"](2);
    this["_infoPopup"]["add"](text2);
  }
  ["_closeInfoPopup"]() {
    this["_infoPopup"] &&
      (this["_infoPopup"]["destroy"](), (this["_infoPopup"] = null));
  }
  ["_expandHitArea"](value71, other) {
    const width = value71["width"],
      height = value71["height"],
      value72 = (width * (other - 1)) / 2,
      value73 = (height * (other - 1)) / 2;
    value71["input"]["hitArea"]["setTo"](
      -value72,
      -value73,
      width + 2 * value72,
      height + 2 * value73,
    );
  }
  ["_makeBouncyButton"](gameObject, other, extra, extra2) {
    const value71 = 1.26 * other;
    return (
      gameObject["on"]("pointerdown", () => {
        (extra2 && !extra2()) ||
          ((gameObject["_pressed"] = !0),
          this["tweens"]["killTweensOf"](gameObject, "scale"),
          this["tweens"]["add"]({
            targets: gameObject,
            scale: value71,
            duration: 300,
            ease: "Bounce.Out",
          }));
      }),
      gameObject["on"]("pointerout", () => {
        gameObject["_pressed"] &&
          ((gameObject["_pressed"] = !1),
          this["tweens"]["killTweensOf"](gameObject, "scale"),
          this["tweens"]["add"]({
            targets: gameObject,
            scale: other,
            duration: 400,
            ease: "Bounce.Out",
          }));
      }),
      gameObject["on"]("pointerup", () => {
        gameObject["_pressed"] &&
          ((gameObject["_pressed"] = !1),
          this["tweens"]["killTweensOf"](gameObject, "scale"),
          gameObject["setScale"](other),
          extra());
      }),
      gameObject
    );
  }
  ["_toggleFullscreen"]() {
    if (this["scale"]["isFullscreen"]) this["scale"]["stopFullscreen"]();
    else {
      this["scale"]["startFullscreen"]();
      try {
        screen["orientation"]["lock"]("landscape")["catch"](() => {});
      } catch (value71) {}
    }
  }
  ["_drawScale9"](
    value71,
    other,
    extra,
    extra2,
    extra3,
    extra4,
    extra5,
    extra6,
  ) {
    const container = this["add"]["container"](value71, other),
      value72 = this["textures"]["get"](extra3),
      value73 = value72["source"][0],
      width = value73["width"],
      height = value73["height"],
      value74 = extra - 2 * extra4,
      value75 = extra2 - 2 * extra4,
      table4 = [
        {
          sx: 0,
          sy: 0,
          sw: extra4,
          sh: extra4,
          dx: -extra / 2,
          dy: -extra2 / 2,
          dw: extra4,
          dh: extra4,
        },
        {
          sx: extra4,
          sy: 0,
          sw: width - 2 * extra4,
          sh: extra4,
          dx: -extra / 2 + extra4,
          dy: -extra2 / 2,
          dw: value74,
          dh: extra4,
        },
        {
          sx: width - extra4,
          sy: 0,
          sw: extra4,
          sh: extra4,
          dx: extra / 2 - extra4,
          dy: -extra2 / 2,
          dw: extra4,
          dh: extra4,
        },
        {
          sx: 0,
          sy: extra4,
          sw: extra4,
          sh: height - 2 * extra4,
          dx: -extra / 2,
          dy: -extra2 / 2 + extra4,
          dw: extra4,
          dh: value75,
        },
        {
          sx: extra4,
          sy: extra4,
          sw: width - 2 * extra4,
          sh: height - 2 * extra4,
          dx: -extra / 2 + extra4,
          dy: -extra2 / 2 + extra4,
          dw: value74,
          dh: value75,
        },
        {
          sx: width - extra4,
          sy: extra4,
          sw: extra4,
          sh: height - 2 * extra4,
          dx: extra / 2 - extra4,
          dy: -extra2 / 2 + extra4,
          dw: extra4,
          dh: value75,
        },
        {
          sx: 0,
          sy: height - extra4,
          sw: extra4,
          sh: extra4,
          dx: -extra / 2,
          dy: extra2 / 2 - extra4,
          dw: extra4,
          dh: extra4,
        },
        {
          sx: extra4,
          sy: height - extra4,
          sw: width - 2 * extra4,
          sh: extra4,
          dx: -extra / 2 + extra4,
          dy: extra2 / 2 - extra4,
          dw: value74,
          dh: extra4,
        },
        {
          sx: width - extra4,
          sy: height - extra4,
          sw: extra4,
          sh: extra4,
          dx: extra / 2 - extra4,
          dy: extra2 / 2 - extra4,
          dw: extra4,
          dh: extra4,
        },
      ];
    for (let i = 0; i < table4["length"]; i++) {
      const i2 = table4[i],
        value76 = "_s9_" + i;
      value72["has"](value76) ||
        value72["add"](value76, 0, i2["sx"], i2["sy"], i2["sw"], i2["sh"]);
      const image = this["add"]
        ["image"](i2["dx"], i2["dy"], extra3, value76)
        ["setOrigin"](0, 0)
        ["setDisplaySize"](i2["dw"], i2["dh"]);
      (void 0 !== extra5 && image["setTint"](extra5),
        void 0 !== extra6 && image["setAlpha"](extra6),
        container["add"](image));
    }
    return container;
  }
  ["_startGame"]() {
    if (!this["_menuActive"]) return;
    if (
      ((this["_menuActive"] = !1),
      (this["_slideIn"] = !0),
      this["_menuGlitter"] &&
        (this["_menuGlitter"]["destroy"](), (this["_menuGlitter"] = null)),
      this["_playBtn"] &&
        (this["tweens"]["killTweensOf"](this["_playBtn"]),
        this["tweens"]["add"]({
          targets: this["_playBtn"],
          scale: 0.01,
          duration: 200,
          ease: "Quad.In",
          onComplete: () => {
            (this["_playBtn"]["destroy"](), (this["_playBtn"] = null));
          },
        })),
      this["_robLogo"] &&
        this["tweens"]["add"]({
          targets: this["_robLogo"],
          y: screenHeight + this["_robLogo"]["height"],
          duration: 300,
          ease: "Quad.In",
          onComplete: () => {
            (this["_robLogo"]["destroy"](), (this["_robLogo"] = null));
          },
        }),
      this["_copyrightText"] &&
        this["tweens"]["add"]({
          targets: this["_copyrightText"],
          y: 680,
          duration: 300,
          ease: "Quad.In",
          onComplete: () => {
            (this["_copyrightText"]["destroy"](),
              (this["_copyrightText"] = null));
          },
        }),
      this["_menuFsBtn"] &&
        this["tweens"]["add"]({
          targets: this["_menuFsBtn"],
          y: -this["_menuFsBtn"]["height"],
          duration: 300,
          ease: "Quad.In",
          onComplete: () => {
            (this["_menuFsBtn"]["destroy"](), (this["_menuFsBtn"] = null));
          },
        }),
      this["_menuInfoBtn"] &&
        this["tweens"]["add"]({
          targets: this["_menuInfoBtn"],
          y: -this["_menuInfoBtn"]["height"],
          duration: 300,
          ease: "Quad.In",
          onComplete: () => {
            (this["_menuInfoBtn"]["destroy"](), (this["_menuInfoBtn"] = null));
          },
        }),
      this["_closeInfoPopup"](),
      this["_tryMeImg"] &&
        this["tweens"]["add"]({
          targets: this["_tryMeImg"],
          y: -this["_tryMeImg"]["height"],
          duration: 300,
          ease: "Quad.In",
          onComplete: () => {
            (this["_tryMeImg"]["destroy"](), (this["_tryMeImg"] = null));
          },
        }),
      this["_downloadBtns"])
    ) {
      for (const item of this["_downloadBtns"])
        (this["tweens"]["killTweensOf"](item),
          this["tweens"]["add"]({
            targets: item,
            y: screenHeight + item["height"],
            duration: 300,
            ease: "Quad.In",
            onComplete: () => item["destroy"](),
          }));
      this["_downloadBtns"] = null;
    }
    (this["_logo"] &&
      this["tweens"]["add"]({
        targets: this["_logo"],
        y: -this["_logo"]["height"],
        duration: 300,
        ease: "Quad.In",
        onComplete: () => {
          (this["_logo"]["destroy"](), (this["_logo"] = null));
        },
      }),
      (this["_cameraX"] = -groundYOffset),
      (this["_cameraY"] = 0),
      (this["_cameraXRef"]["_v"] = this["_cameraX"]),
      (this["_prevCameraX"] = this["_cameraX"]));
    const value71 = this["_cameraX"] - (this["_menuCameraX"] || 0);
    (this["_level"]["shiftGroundTiles"](value71),
      (this["_playerWorldX"] = this["_cameraX"]),
      (this["_state"]["y"] = 30),
      (this["_state"]["onGround"] = !0),
      this["_level"]["additiveContainer"]["setVisible"](!0),
      this["_level"]["container"]["setVisible"](!0),
      this["_level"]["topContainer"]["setVisible"](!0),
      this["_player"]["setCubeVisible"](!0),
      this["_player"]["reset"](),
      this["_attemptsLabel"]["setVisible"](this["_attempts"] > 1),
      this["_positionAttemptsLabel"]());
  }
  ["_pushButton"]() {
    if (this["_menuActive"])
      return (
        this["_audio"]["playEffect"]("playSound_01", {
          volume: 1,
        }),
        void this["_startGame"]()
      );
    this["_slideIn"] ||
      this["_state"]["isDead"] ||
      ((this["_state"]["upKeyDown"] = !0),
      (this["_state"]["upKeyPressed"] = !0),
      !this["_state"]["isFlying"] &&
        this["_state"]["canJump"] &&
        (this["_player"]["updateJump"](0), this["_totalJumps"]++));
  }
  ["_releaseButton"]() {
    ((this["_state"]["upKeyDown"] = !1), (this["_state"]["upKeyPressed"] = !1));
  }
  ["_positionMenuItems"]() {
    const value71 = screenWidth / 2;
    if (
      (this["_logo"] && (this["_logo"]["x"] = value71),
      this["_menuInfoBtn"] &&
        (this["_menuInfoBtn"]["x"] = screenWidth - 30 - 3),
      this["_copyrightText"] &&
        (this["_copyrightText"]["x"] = screenWidth - 20),
      this["_tryMeImg"] && (this["_tryMeImg"]["x"] = value71 + 175),
      this["_menuGlitter"] &&
        ((this["_menuGlitter"]["x"] = value71),
        (this["_menuGlitter"]["y"] = 320)),
      this["_playBtn"] &&
        ((this["_playBtn"]["x"] = value71),
        this["tweens"]["killTweensOf"](this["_playBtn"], "y"),
        (this["_playBtn"]["y"] = 320),
        this["tweens"]["add"]({
          targets: this["_playBtn"],
          y: 324,
          duration: 750,
          ease: "Quad.InOut",
          yoyo: !0,
          repeat: -1,
        })),
      this["_downloadBtns"])
    ) {
      const value72 = screenWidth - 130,
        num32 = 555,
        num33 = 210;
      for (let i = 0; i < this["_downloadBtns"]["length"]; i++)
        this["_downloadBtns"][i]["setPosition"](value72 - i * num33, num32);
    }
  }
  ["_positionAttemptsLabel"]() {
    let value71 = this["_cameraX"] + screenWidth / 2;
    (this["_attempts"] > 1 && (value71 += 100),
      this["_attemptsLabel"]["setPosition"](value71, 150));
  }
  ["_resetGameplayState"]() {
    ((this["_cameraX"] = -groundYOffset),
      (this["_cameraY"] = 0),
      (this["_cameraXRef"]["_v"] = -groundYOffset),
      (this["_prevCameraX"] = -groundYOffset),
      (this["_playerWorldX"] = 0),
      (this["_deltaBuffer"] = 0),
      (this["_deathTimer"] = 0),
      (this["_deathSoundPlayed"] = !1),
      (this["_newBestShown"] = !1),
      (this["_hadNewBest"] = !1),
      (this["_levelWon"] = !1),
      (this["_endCameraOverride"] = !1),
      (this["_endCamTween"] = null),
      (this["_spaceWasDown"] = !1));
  }
  ["_restartLevel"]() {
    this["_attempts"]++;
    const cameraX = this["_cameraX"];
    (this["_resetGameplayState"](),
      this["_state"]["reset"](),
      this["_player"]["reset"](),
      this["_glitterEmitter"]["stop"](),
      this["_level"]["resetObjects"](),
      this["_level"]["shiftGroundTiles"](this["_cameraX"] - cameraX),
      this["_level"]["resetGroundState"](),
      this["_level"]["resetColorTriggers"](),
      this["_level"]["resetEnterEffectTriggers"](),
      this["_level"]["resetVisibility"](),
      this["_colorManager"]["reset"](),
      this["_audio"]["reset"](),
      this["_audio"]["startMusic"](),
      (this["_paused"] = !1),
      this["_pauseContainer"] &&
        (this["_pauseContainer"]["destroy"](),
        (this["_pauseContainer"] = null)),
      this["_pauseBtn"]["setVisible"](!0)["setAlpha"](75 / 255),
      this["_attemptsLabel"]["setText"]("Attempt " + this["_attempts"]),
      this["_attemptsLabel"]["setVisible"](!0),
      this["_positionAttemptsLabel"]());
  }
  ["_onFullscreenChange"](value71) {
    (value71 || setScreenWidth(1138),
      this["time"]["delayedCall"](200, () => this["_applyScreenResize"]()));
  }
  ["_applyScreenResize"]() {
    if (this["scale"]["isFullscreen"]) {
      const value71 = window["innerWidth"] / window["innerHeight"];
      setScreenWidth(Math["round"](screenHeight * value71));
    }
    if (
      (this["scale"]["setGameSize"](screenWidth, screenHeight),
      this["scale"]["refresh"](),
      this["_bg"]["setSize"](screenWidth, screenHeight),
      (this["_pauseBtn"]["x"] = screenWidth - 30),
      this["_menuActive"] && this["_positionMenuItems"](),
      this["_paused"] &&
        this["_pauseContainer"] &&
        (this["_pauseContainer"]["destroy"](),
        (this["_pauseContainer"] = null),
        this["_buildPauseOverlay"]()),
      this["_level"]["resizeScreen"](),
      !this["_menuActive"])
    ) {
      const cameraX = this["_cameraX"];
      ((this["_cameraX"] = this["_playerWorldX"] - groundYOffset),
        (this["_cameraXRef"]["_v"] = this["_cameraX"]),
        (this["_level"]["additiveContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["additiveContainer"]["y"] = this["_cameraY"]),
        (this["_level"]["container"]["x"] = -this["_cameraX"]),
        (this["_level"]["container"]["y"] = this["_cameraY"]),
        (this["_level"]["topContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["topContainer"]["y"] = this["_cameraY"]),
        this["_level"]["shiftGroundTiles"](this["_cameraX"] - cameraX),
        this["_level"]["updateGroundTiles"](this["_cameraY"]),
        this["_level"]["updateVisibility"](this["_cameraX"]),
        this["_level"]["applyEnterEffects"](this["_cameraX"]));
      const value71 = this["_playerWorldX"] - this["_cameraX"];
      this["_player"]["syncSprites"](
        this["_cameraX"],
        this["_cameraY"],
        0,
        value71,
      );
    }
  }
  ["_updateBackground"]() {
    ((this["_bg"]["tilePositionX"] +=
      (this["_cameraX"] - this["_prevCameraX"]) * this["_bgSpeedX"]),
      (this["_prevCameraX"] = this["_cameraX"]),
      (this["_bg"]["tilePositionY"] =
        this["_bgInitY"] - this["_cameraY"] * this["_bgSpeedY"]));
  }
  ["_updateCameraY"](value71) {
    let cameraY = this["_cameraY"],
      value72 = cameraY;
    if (null !== this["_level"]["flyCameraTarget"])
      value72 = this["_level"]["flyCameraTarget"];
    else {
      let value73 = this["_state"]["y"],
        num32 = 140,
        num33 = 80,
        value74 = cameraY - unusedConst180 + 320;
      value73 > value74 + num32
        ? (value72 = value73 - 320 - num32 + unusedConst180)
        : value73 < value74 - num33 &&
          (value72 = value73 - 320 + num33 + unusedConst180);
    }
    (value72 < 0 && (value72 = 0), 0 !== value71) &&
      ((cameraY += (value72 - cameraY) / (10 / value71)),
      cameraY < 0 && (cameraY = 0),
      (this["_cameraY"] = cameraY));
  }
  ["_quantizeDelta"](value71) {
    let value72 = value71 / 1e3 + this["_deltaBuffer"],
      value73 = Math["round"](value72 / fixedTimeStep);
    (value73 < 0 && (value73 = 0), value73 > 60 && (value73 = 60));
    let value74 = value73 * fixedTimeStep;
    return ((this["_deltaBuffer"] = value72 - value74), 60 * value74);
  }
  ["update"](value71, other) {
    if (
      ((this["_fpsAccum"] += other),
      this["_fpsFrames"]++,
      this["_fpsAccum"] >= 250 &&
        (this["_fpsText"]["setText"](
          Math["round"]((1e3 * this["_fpsFrames"]) / this["_fpsAccum"]),
        ),
        (this["_fpsAccum"] = 0),
        (this["_fpsFrames"] = 0)),
      this["_paused"])
    )
      return void (this["_deltaBuffer"] = 0);
    if (this["_menuActive"]) {
      if (
        (this["_spaceKey"]["isDown"] || this["_upKey"]["isDown"]) &&
        !this["_spaceWasDown"]
      )
        return (
          (this["_spaceWasDown"] = !0),
          this["_audio"]["playEffect"]("playSound_01", {
            volume: 1,
          }),
          void this["_startGame"]()
        );
      this["_spaceWasDown"] =
        this["_spaceKey"]["isDown"] || this["_upKey"]["isDown"];
      const value79 = Math["min"]((other / 1e3) * 60, 2),
        num32 = 0.25;
      this["_menuCameraX"] =
        (this["_menuCameraX"] || 0) +
        value79 * gravityConst * physicsConst09 * num32;
      const cameraX = this["_cameraX"];
      return (
        (this["_cameraX"] = this["_menuCameraX"]),
        this["_updateBackground"](),
        (this["_cameraX"] = cameraX),
        (this["_prevCameraX"] = this["_menuCameraX"]),
        (this["_cameraXRef"]["_v"] = this["_menuCameraX"]),
        this["_level"]["stepGroundAnimation"](other / 1e3),
        void this["_level"]["updateGroundTiles"](this["_cameraY"])
      );
    }
    if (this["_slideIn"]) {
      const value79 = this["_quantizeDelta"](other);
      this["_playerWorldX"] += value79 * gravityConst * physicsConst09;
      const num32 = 0.25;
      ((this["_slideGroundX"] =
        (this["_slideGroundX"] || this["_cameraX"]) +
        value79 * gravityConst * physicsConst09 * num32),
        (this["_cameraXRef"]["_v"] = this["_slideGroundX"]));
      const value80 = this["_playerWorldX"] - this["_cameraX"];
      if (
        (this["_player"]["updateGroundRotation"](value79 * physicsConst09),
        this["_player"]["syncSprites"](
          this["_cameraX"],
          this["_cameraY"],
          other / 1e3,
          value80,
        ),
        (this["_level"]["additiveContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["additiveContainer"]["y"] = this["_cameraY"]),
        (this["_level"]["container"]["x"] = -this["_cameraX"]),
        (this["_level"]["container"]["y"] = this["_cameraY"]),
        (this["_level"]["topContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["topContainer"]["y"] = this["_cameraY"]),
        this["_level"]["updateVisibility"](this["_cameraX"]),
        this["_updateBackground"](),
        this["_level"]["stepGroundAnimation"](other / 1e3),
        this["_level"]["updateGroundTiles"](this["_cameraY"]),
        this["_playerWorldX"] >= 0)
      ) {
        ((this["_slideIn"] = !1),
          (this["_deltaBuffer"] = 0),
          (this["_playerWorldX"] = 0),
          (this["_cameraX"] = this["_playerWorldX"] - groundYOffset),
          (this["_cameraXRef"]["_v"] = this["_cameraX"]));
        const value81 = this["_cameraX"] - this["_slideGroundX"];
        (this["_level"]["shiftGroundTiles"](value81),
          this["_firstPlay"] &&
            ((this["_firstPlay"] = !1), this["_audio"]["startMusic"]()),
          this["_pauseBtn"]["setVisible"](!0)["setAlpha"](0),
          this["tweens"]["add"]({
            targets: this["_pauseBtn"],
            alpha: 75 / 255,
            duration: 500,
          }));
      }
      return;
    }
    let value72 = this["_spaceKey"]["isDown"] || this["_upKey"]["isDown"];
    if (
      (value72 && !this["_spaceWasDown"]
        ? this["_pushButton"]()
        : !value72 && this["_spaceWasDown"] && this["_releaseButton"](),
      (this["_spaceWasDown"] = value72),
      !this["input"]["activePointer"]["isDown"] ||
        this["_state"]["upKeyDown"] ||
        this["_state"]["isDead"] ||
        (this["_state"]["upKeyDown"] = !0),
      this["_level"]["updateEndPortalY"](
        this["_cameraY"],
        this["_state"]["isFlying"],
      ),
      !this["_levelWon"] &&
        !this["_state"]["isDead"] &&
        this["_level"]["endXPos"] > 0)
    ) {
      const num32 = 600;
      this["_playerWorldX"] >= this["_level"]["endXPos"] - num32 &&
        ((this["_levelWon"] = !0),
        (this["_endPortalGameY"] = this["_level"]["_endPortalGameY"] || 240),
        this["_triggerEndPortal"]());
    }
    if (this["_levelWon"]) {
      if (((this["_deltaBuffer"] = 0), this["_endCamTween"])) {
        const endCamTween = this["_endCamTween"];
        ((this["_cameraX"] =
          endCamTween["fromX"] +
          (endCamTween["toX"] - endCamTween["fromX"]) * endCamTween["p"]),
          (this["_cameraY"] =
            endCamTween["fromY"] +
            (endCamTween["toY"] - endCamTween["fromY"]) * endCamTween["p"]));
      }
      return (
        (this["_cameraXRef"]["_v"] = this["_cameraX"]),
        (this["_level"]["additiveContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["additiveContainer"]["y"] = this["_cameraY"]),
        (this["_level"]["container"]["x"] = -this["_cameraX"]),
        (this["_level"]["container"]["y"] = this["_cameraY"]),
        (this["_level"]["topContainer"]["x"] = -this["_cameraX"]),
        (this["_level"]["topContainer"]["y"] = this["_cameraY"]),
        this["_updateBackground"](),
        this["_level"]["stepGroundAnimation"](other / 1e3),
        void this["_level"]["updateGroundTiles"](this["_cameraY"])
      );
    }
    if (this["_state"]["isDead"]) {
      if (
        (this["_deathSoundPlayed"] ||
          (this["_audio"]["stopMusic"](),
          this["_audio"]["playEffect"]("explode_11", {
            volume: 0.65,
          }),
          (this["_deathSoundPlayed"] = !0)),
        !this["_newBestShown"])
      ) {
        this["_newBestShown"] = !0;
        let value80 = this["_level"]["endXPos"] || 6e3,
          playerWorldX2 = this["_playerWorldX"];
        ((this["_lastPercent"] = Math["min"](
          99,
          Math["max"](0, Math["floor"]((playerWorldX2 / value80) * 100)),
        )),
          this["_lastPercent"] > this["_bestPercent"] &&
            ((this["_bestPercent"] = this["_lastPercent"]),
            (this["_hadNewBest"] = !0),
            this["_showNewBest"]()));
      }
      (this["_player"]["updateExplosionPieces"](other),
        (this["_deathTimer"] += other));
      let value79 = this["_hadNewBest"] ? 1400 : 1e3;
      return void (this["_deathTimer"] > value79 && this["_restartLevel"]());
    }
    ((this["_playTime"] += other / 1e3),
      this["_audio"]["update"](other / 1e3),
      this["_level"]["updateAudioScale"](this["_audio"]["getMeteringValue"]()));
    let value73 = this["_quantizeDelta"](other),
      value74 = value73 > 0 ? Math["max"](1, Math["round"](4 * value73)) : 0;
    value74 > 60 && (value74 = 60);
    let value75 = value74 > 0 ? value73 / value74 : 0,
      value76 = value75 * physicsConst09;
    const value77 = this["_state"]["y"];
    for (let i = 0; i < value74; i++)
      ((this["_state"]["lastY"] = this["_state"]["y"]),
        this["_player"]["updateJump"](value76),
        (this["_state"]["y"] += this["_state"]["yVelocity"] * value76),
        this["_player"]["checkCollisions"](
          this["_playerWorldX"] - groundYOffset,
        ),
        (this["_playerWorldX"] += value75 * gravityConst * physicsConst09),
        this["_state"]["isFlying"] ||
          (this["_state"]["onGround"]
            ? this["_player"]["updateGroundRotation"](value76)
            : this["_player"]["rotateActionActive"] &&
              this["_player"]["updateRotateAction"](fixedTimeStep)));
    if (((this["_state"]["lastY"] = value77), !this["_endCameraOverride"])) {
      const value79 = this["_playerWorldX"] - groundYOffset;
      if (this["_level"]["endXPos"] > 0) {
        const value80 = this["_level"]["endXPos"] - screenWidth;
        if (value79 >= value80 - 200) {
          ((this["_endCameraOverride"] = !0), (this["_cameraX"] = value79));
          const value81 = -140 + (this["_level"]["_endPortalGameY"] || 240),
            num32 = 1.8,
            callback51 = (value82) =>
              value82 < 0.5
                ? Math["pow"](2 * value82, num32) / 2
                : 1 - Math["pow"](2 * (1 - value82), num32) / 2;
          ((this["_endCamTween"] = {
            p: 0,
            fromX: this["_cameraX"],
            toX: value80,
            fromY: this["_cameraY"],
            toY: value81,
          }),
            this["tweens"]["add"]({
              targets: this["_endCamTween"],
              p: 1,
              duration: 1200,
              ease: callback51,
            }));
        } else this["_cameraX"] = value79;
      } else this["_cameraX"] = value79;
    }
    if (this["_endCameraOverride"] && this["_endCamTween"]) {
      const endCamTween = this["_endCamTween"];
      ((this["_cameraX"] =
        endCamTween["fromX"] +
        (endCamTween["toX"] - endCamTween["fromX"]) * endCamTween["p"]),
        (this["_cameraY"] =
          endCamTween["fromY"] +
          (endCamTween["toY"] - endCamTween["fromY"]) * endCamTween["p"]));
    }
    ((this["_cameraXRef"]["_v"] = this["_cameraX"]),
      this["_endCameraOverride"] || this["_updateCameraY"](value73),
      (this["_level"]["additiveContainer"]["x"] = -this["_cameraX"]),
      (this["_level"]["additiveContainer"]["y"] = this["_cameraY"]),
      (this["_level"]["container"]["x"] = -this["_cameraX"]),
      (this["_level"]["container"]["y"] = this["_cameraY"]),
      (this["_level"]["topContainer"]["x"] = -this["_cameraX"]),
      (this["_level"]["topContainer"]["y"] = this["_cameraY"]));
    let playerWorldX = this["_playerWorldX"];
    for (let item of this["_level"]["checkColorTriggers"](playerWorldX))
      (this["_colorManager"]["triggerColor"](
        item["index"],
        item["color"],
        item["duration"],
      ),
        item["tintGround"] &&
          this["_colorManager"]["triggerColor"](
            num31,
            item["color"],
            item["duration"],
          ));
    (this["_colorManager"]["step"](other / 1e3),
      this["_bg"]["setTint"](this["_colorManager"]["getHex"](num30)),
      this["_level"]["setGroundColor"](this["_colorManager"]["getHex"](num31)),
      this["_level"]["updateVisibility"](this["_cameraX"]),
      this["_level"]["checkEnterEffectTriggers"](playerWorldX),
      this["_level"]["applyEnterEffects"](this["_cameraX"]),
      (this["_glitterCenterX"] = this["_cameraX"] + screenWidth / 2),
      (this["_glitterCenterY"] = yFlipBase - this["_cameraY"]),
      this["_updateBackground"](),
      this["_level"]["stepGroundAnimation"](other / 1e3),
      this["_level"]["updateGroundTiles"](this["_cameraY"]),
      this["_state"]["isFlying"] &&
        this["_player"]["updateShipRotation"](value73));
    const value78 = this["_playerWorldX"] - this["_cameraX"];
    this["_player"]["syncSprites"](
      this["_cameraX"],
      this["_cameraY"],
      other / 1e3,
      value78,
    );
  }
  ["_showNewBest"]() {
    let value71 = screenWidth / 2,
      image = this["add"]
        ["image"](0, 0, "GJ_WebSheet", "GJ_newBest_001.png")
        ["setOrigin"](0.5, 1),
      bitmapText = this["add"]
        ["bitmapText"](0, 2, "bigFont", this["_lastPercent"] + "%", 65)
        ["setOrigin"](0.5, 0)
        ["setScale"](1.1),
      container = this["add"]
        ["container"](value71, 300, [image, bitmapText])
        ["setScrollFactor"](0)
        ["setDepth"](60)
        ["setScale"](0.01);
    this["tweens"]["add"]({
      targets: container,
      scale: 1,
      duration: 400,
      ease: "Elastic.Out",
      easeParams: [1, 0.6],
      onComplete: () => {
        this["tweens"]["add"]({
          targets: container,
          scale: 0.01,
          duration: 200,
          delay: 700,
          ease: "Quad.In",
          onComplete: () => {
            (container["setVisible"](!1), container["destroy"]());
          },
        });
      },
    });
  }
  ["_triggerEndPortal"]() {
    this["_player"]["playEndAnimation"](
      this["_level"]["endXPos"],
      () => this["_levelComplete"](),
      this["_endPortalGameY"],
    );
  }
  ["_levelComplete"]() {
    const value71 = this["_level"]["endXPos"] - this["_cameraX"],
      value72 = flipY(this["_endPortalGameY"]) + this["_cameraY"];
    for (let i = 0; i < 5; i++)
      this["time"]["delayedCall"](50 * i, () =>
        drawExpandingRing(
          this,
          value71,
          value72,
          10,
          screenWidth,
          500,
          !1,
          !0,
          colorGreenTint,
        ),
      );
    (drawExpandingRing(
      this,
      value71,
      value72,
      10,
      1e3,
      500,
      !0,
      !1,
      colorGreenTint,
    ),
      this["_showCompleteEffect"]());
  }
  ["_showCompleteEffect"]() {
    (this["_audio"]["fadeOutMusic"](1500),
      this["sound"]["play"]("endStart_02", {
        volume: 0.8,
      }),
      !(function (value71, other, extra, extra2) {
        const num32 = 2,
          num33 = 8,
          value72 = 1 * num32,
          value73 = 30 * num32,
          value74 = 20 * num32,
          value75 =
            Math["round"](Math["sqrt"](screenWidth ** 2 + 102400)) +
            32.5 * num32,
          num34 = 180,
          num35 = 40,
          num36 = 195,
          num37 = 40,
          num38 = 40,
          value76 = 155 / 255,
          value77 = 100 / 255,
          num39 = 400,
          value78 = -135,
          value79 = 90 / num33,
          value80 = Array["from"](
            {
              length: num33,
            },
            (value81, other2) => value78 + other2 * value79,
          );
        for (let i = value80["length"] - 1; i > 0; i--) {
          const value81 = Math["floor"](Math["random"]() * (i + 1));
          [value80[i], value80[value81]] = [value80[value81], value80[i]];
        }
        let num40 = 0;
        const table4 = [];
        for (let i = 0; i < num33; i++) {
          const value81 =
              i * num36 + num37 + num38 * (2 * Math["random"]() - 1),
            value82 = value73 + value74 * (2 * Math["random"]() - 1),
            value83 = num34 + num35 * (2 * Math["random"]() - 1),
            value84 = Math["min"](
              1,
              Math["max"](0, value76 + value77 * (2 * Math["random"]() - 1)),
            ),
            value85 = value80[i] + value79 * Math["random"]() + 180,
            gameObject = value71["add"]
              ["graphics"]()
              ["setScrollFactor"](0)
              ["setDepth"](-1)
              ["setBlendMode"](blendAdd)
              ["setPosition"](other, extra)
              ["setAngle"](value85)
              ["setAlpha"](value84)
              ["setVisible"](!1),
            options10 = {
              h: 1,
              w: value72,
            };
          (value71["time"]["delayedCall"](Math["max"](0, value81), () => {
            (gameObject["setVisible"](!0),
              value71["tweens"]["add"]({
                targets: options10,
                h: value75,
                w: value82,
                duration: value83,
                ease: "Quad.Out",
                onUpdate: () => {
                  const value86 = value72 + (options10["w"] - value72) / 4;
                  (gameObject["clear"](),
                    gameObject["fillStyle"](extra2, 1),
                    gameObject["beginPath"](),
                    gameObject["moveTo"](-value86 / 2, 0),
                    gameObject["lineTo"](value86 / 2, 0),
                    gameObject["lineTo"](options10["w"] / 2, options10["h"]),
                    gameObject["lineTo"](-options10["w"] / 2, options10["h"]),
                    gameObject["closePath"](),
                    gameObject["fillPath"]());
                },
              }));
          }),
            value81 > num40 && (num40 = value81),
            table4["push"](gameObject));
        }
        value71["time"]["delayedCall"](num40 + num39, () => {
          for (const item of table4) {
            const value81 = 200 * Math["random"](),
              value82 = 400 + 100 * (2 * Math["random"]() - 1);
            value71["tweens"]["add"]({
              targets: item,
              alpha: 0,
              delay: value81,
              duration: value82,
              onComplete: () => item["destroy"](),
            });
          }
        });
      })(
        this,
        this["_level"]["endXPos"] - this["_cameraX"] + 60,
        flipY(this["_endPortalGameY"]) + this["_cameraY"],
        colorGreenTint,
      ),
      this["cameras"]["main"]["shake"](1950, 0.004),
      this["time"]["delayedCall"](1950, () => this["_showCompleteText"]()));
  }
  ["_showCompleteText"]() {
    const value71 = screenWidth / 2,
      image = this["add"]
        ["image"](value71, 250, "GJ_WebSheet", "GJ_levelComplete_001.png")
        ["setScrollFactor"](0)
        ["setDepth"](60)
        ["setScale"](0.01);
    this["tweens"]["add"]({
      targets: image,
      scale: 1.1,
      duration: 660,
      ease: "Elastic.Out",
      easeParams: [1, 0.6],
      onComplete: () => {
        this["tweens"]["add"]({
          targets: image,
          scale: 0.01,
          duration: 220,
          delay: 880,
          ease: "Quad.In",
          onComplete: () => {
            (image["setVisible"](!1), image["destroy"]());
          },
        });
      },
    });
    const table4 = [colorGreenTint, 16777215];
    for (let i = 0; i < 2; i++)
      this["add"]
        ["particles"](value71, 250, "GJ_WebSheet", {
          frame: "square.png",
          speed: {
            min: 300,
            max: 700,
          },
          angle: {
            min: 0,
            max: 360,
          },
          scale: {
            start: 0.4,
            end: 0.13,
          },
          lifespan: {
            min: 0,
            max: 1e3,
          },
          quantity: 50,
          stopAfter: 200,
          blendMode: blendAdd,
          tint: table4[i],
          x: {
            min: -800,
            max: 800,
          },
          y: {
            min: -80,
            max: 80,
          },
        })
        ["setScrollFactor"](0)
        ["setDepth"](59);
    const value72 = this["_level"]["endXPos"] - this["_cameraX"],
      value73 = flipY(this["_endPortalGameY"]) + this["_cameraY"];
    (drawExpandingRing(
      this,
      value72,
      value73,
      10,
      screenWidth,
      800,
      !0,
      !1,
      colorGreenTint,
    ),
      drawExpandingRing(
        this,
        value71,
        250,
        10,
        1e3,
        800,
        !0,
        !1,
        colorGreenTint,
      ));
    for (let i = 0; i < 5; i++)
      this["time"]["delayedCall"](50 * i, () =>
        drawExpandingRing(
          this,
          value72,
          value73,
          10,
          screenWidth,
          500,
          !1,
          !0,
          colorGreenTint,
        ),
      );
    for (let i = 0; i < 10; i++) {
      const value74 = 150 * i + (160 * Math["random"]() - 80);
      this["time"]["delayedCall"](Math["max"](0, value74), () =>
        spawnFinishParticles(this, colorGreenTint, colorCyanTint),
      );
    }
    this["time"]["delayedCall"](1500, () => this["_showEndLayer"]());
  }
  ["_showEndLayer"]() {
    this["_pauseBtn"] &&
      this["tweens"]["add"]({
        targets: this["_pauseBtn"],
        alpha: 0,
        duration: 300,
      });
    const value71 = screenWidth / 2,
      num32 = 320;
    ((this["_endLayerOverlay"] = this["add"]
      ["rectangle"](value71, num32, screenWidth, screenHeight, 0, 0)
      ["setScrollFactor"](0)
      ["setDepth"](200)
      ["setInteractive"]()),
      (this["_endLayerInternal"] = this["add"]
        ["container"](0, -640)
        ["setScrollFactor"](0)
        ["setDepth"](201)),
      this["tweens"]["add"]({
        targets: this["_endLayerOverlay"],
        alpha: 100 / 255,
        duration: 1e3,
      }));
    const options10 = {
      p: 0,
    };
    this["tweens"]["add"]({
      targets: options10,
      p: 1,
      duration: 1e3,
      ease: "Bounce.Out",
      onUpdate: () => {
        this["_endLayerInternal"]["y"] = 650 * options10["p"] - 640;
      },
      onComplete: () => this["_playStarAward"](),
    });
    const num33 = 712,
      num34 = 460,
      value72 = (screenWidth - num33) / 2;
    this["_endLayerInternal"]["add"](
      this["add"]["rectangle"](value72 + 356, 310, num33, num34, 0, 180 / 255),
    );
    const value73 = this["textures"]["getFrame"](
        "GJ_WebSheet",
        "GJ_table_side_001.png",
      ),
      value74 = value73 ? num34 / value73["height"] : 1;
    (this["_endLayerInternal"]["add"](
      this["add"]
        ["image"](value72 - 40, 80, "GJ_WebSheet", "GJ_table_side_001.png")
        ["setOrigin"](0, 0)
        ["setScale"](1, value74),
    ),
      this["_endLayerInternal"]["add"](
        this["add"]
          ["image"](
            value72 + num33 + 40,
            80,
            "GJ_WebSheet",
            "GJ_table_side_001.png",
          )
          ["setOrigin"](1, 0)
          ["setFlipX"](!0)
          ["setScale"](1, value74),
      ));
    const image = this["add"]["image"](
      value72 + 356,
      70,
      "GJ_WebSheet",
      "GJ_table_top_001.png",
    );
    (this["_endLayerInternal"]["add"](image),
      this["_endLayerInternal"]["add"](
        this["add"]["image"](
          value72 + 356,
          560,
          "GJ_WebSheet",
          "GJ_table_bottom_001.png",
        ),
      ));
    const value75 = image["y"] - 65;
    (this["_endLayerInternal"]["add"](
      this["add"]
        ["image"](value71 - 312, value75, "GJ_WebSheet", "chain_01_001.png")
        ["setOrigin"](0.5, 1),
    ),
      this["_endLayerInternal"]["add"](
        this["add"]
          ["image"](value71 + 312, value75, "GJ_WebSheet", "chain_01_001.png")
          ["setOrigin"](0.5, 1),
      ),
      this["_endLayerInternal"]["add"](
        this["add"]
          ["image"](value71, 170, "GJ_WebSheet", "GJ_levelComplete_001.png")
          ["setScale"](0.8),
      ));
    const num35 = 0.8;
    let num36 = 250;
    const bitmapText = this["add"]
      ["bitmapText"](
        value71,
        num36,
        "goldFont",
        "Attempts: " + this["_attempts"],
        40,
      )
      ["setOrigin"](0.5, 0.5)
      ["setScale"](num35);
    (this["_endLayerInternal"]["add"](bitmapText),
      (num36 += 48),
      this["_endLayerInternal"]["add"](
        this["add"]
          ["bitmapText"](
            value71,
            num36,
            "goldFont",
            "Jumps: " + this["_totalJumps"],
            40,
          )
          ["setOrigin"](0.5, 0.5)
          ["setScale"](num35),
      ),
      (num36 += 48));
    const value76 = Math["floor"](this["_playTime"]),
      value77 = Math["floor"](value76 / 3600),
      value78 = Math["floor"]((value76 % 3600) / 60),
      value79 = value76 % 60;
    let value80;
    value80 =
      value77 > 0
        ? String(value77)["padStart"](2, "0") +
          ":" +
          String(value78)["padStart"](2, "0") +
          ":" +
          String(value79)["padStart"](2, "0")
        : String(value78)["padStart"](2, "0") +
          ":" +
          String(value79)["padStart"](2, "0");
    const value81 = num36;
    this["_endLayerInternal"]["add"](
      this["add"]
        ["bitmapText"](value71, num36, "goldFont", "Time: " + value80, 40)
        ["setOrigin"](0.5, 0.5)
        ["setScale"](num35),
    );
    const table4 = [
        "Awesome!",
        "Good\nJob!",
        "Well\nDone!",
        "Impressive!",
        "Amazing!",
        "Incredible!",
        "Skillful!",
        "Brilliant!",
        "Not\nbad!",
        "Warp\nSpeed!",
        "Challenge\nBreaker!",
        "Reflex\nMaster!",
        "I am\nspeechless...",
        "You are...\nThe One!",
        "How is this\npossible!?",
        "You beat\nme...",
      ],
      value82 = table4[Math["floor"](Math["random"]() * table4["length"])],
      num37 = 225;
    (this["_endLayerInternal"]["add"](
      this["add"]
        ["bitmapText"](value71 + num37, value81, "bigFont", value82, 40)
        ["setOrigin"](0.5, 0.5)
        ["setScale"](0.8)
        ["setCenterAlign"](),
    ),
      this["_endLayerInternal"]["add"](
        this["add"]
          ["image"](value71 - num37, 352.5, "GJ_WebSheet", "getIt_001.png")
          ["setScale"](1 / 1.5),
      ));
    const table5 = [
      {
        key: "downloadApple_001",
        url: "https://apps.apple.com/us/app/geometry-dash/id625334537",
      },
      {
        key: "downloadGoogle_001",
        url: "https://play.google.com/store/apps/details?id=com.robtopx.geometryjump&hl=en",
      },
      {
        key: "downloadSteam_001",
        url: "https://store.steampowered.com/app/322170/Geometry_Dash",
      },
    ];
    for (let i = 0; i < table5["length"]; i++) {
      const i2 = table5[i],
        value83 = (i - 1) * num37,
        value84 = 1 / 1.5,
        image2 = this["add"]
          ["image"](value71 + value83, 437.5, "GJ_WebSheet", i2["key"] + ".png")
          ["setScale"](value84)
          ["setInteractive"]();
      (this["_endLayerInternal"]["add"](image2),
        this["_makeBouncyButton"](image2, value84, () =>
          window["open"](i2["url"], "_blank"),
        ));
    }
    (bitmapText["width"],
      (this["_endStarX"] = value71 + num37),
      (this["_endStarY"] = value81 - 77.5));
    const table6 = [
      {
        frame: "GJ_replayBtn_001.png",
        dx: -200,
        action: () => this["_hideEndLayer"](() => this["_restartLevel"]()),
      },
      {
        frame: "GJ_menuBtn_001.png",
        dx: 200,
        action: () => {
          (this["_audio"]["playEffect"]("quitSound_01"),
            this["_audio"]["stopMusic"](),
            this["game"]["registry"]["set"]("fadeInFromBlack", !0),
            this["cameras"]["main"]["fadeOut"](
              400,
              0,
              0,
              0,
              (value83, other) => {
                other >= 1 && this["scene"]["restart"]();
              },
            ));
        },
      },
    ];
    for (const item of table6) {
      const image2 = this["add"]
        ["image"](value71 + item["dx"], 555, "GJ_WebSheet", item["frame"])
        ["setInteractive"]();
      (this["_endLayerInternal"]["add"](image2),
        this["_makeBouncyButton"](image2, 1, item["action"]));
    }
  }
  ["_playStarAward"]() {
    if (!this["_endLayerInternal"]) return;
    const endStarX = this["_endStarX"],
      endStarY = this["_endStarY"],
      image = this["add"]
        ["image"](endStarX, endStarY, "GJ_WebSheet", "GJ_bigStar_001.png")
        ["setScale"](3)
        ["setAlpha"](0);
    (this["_endLayerInternal"]["add"](image),
      this["tweens"]["add"]({
        targets: image,
        scale: 0.8,
        alpha: 1,
        duration: 300,
        delay: 0,
        ease: "Bounce.Out",
      }),
      this["time"]["delayedCall"](100, () => {
        this["_audio"]["playEffect"]("highscoreGet02");
        const value71 = endStarX,
          value72 = endStarY + this["_endLayerInternal"]["y"];
        this["add"]
          ["particles"](value71, value72, "GJ_WebSheet", {
            frame: "square.png",
            speed: {
              min: 200,
              max: 600,
            },
            angle: {
              min: 0,
              max: 360,
            },
            scale: {
              start: 0.5,
              end: 0,
            },
            alpha: {
              start: 1,
              end: 0,
            },
            lifespan: {
              min: 200,
              max: 600,
            },
            quantity: 30,
            stopAfter: 30,
            blendMode: blendAdd,
            tint: 16776960,
          })
          ["setScrollFactor"](0)
          ["setDepth"](202);
        const graphics = this["add"]
            ["graphics"]()
            ["setScrollFactor"](0)
            ["setDepth"](202)
            ["setBlendMode"](blendAdd),
          options10 = {
            t: 0,
          };
        this["tweens"]["add"]({
          targets: options10,
          t: 1,
          duration: 400,
          ease: "Quad.Out",
          onUpdate: () => {
            (graphics["clear"](),
              graphics["fillStyle"](16776960, 1 - options10["t"]),
              graphics["fillCircle"](
                value71,
                value72,
                20 + 200 * options10["t"],
              ));
          },
          onComplete: () => graphics["destroy"](),
        });
      }));
  }
  ["_hideEndLayer"](value71) {
    if (!this["_endLayerInternal"]) return void (value71 && value71());
    const options10 = {
      p: 0,
    };
    (this["tweens"]["add"]({
      targets: options10,
      p: 1,
      duration: 500,
      ease: (value72) =>
        value72 < 0.5
          ? Math["pow"](2 * value72, 2) / 2
          : 1 - Math["pow"](2 * (1 - value72), 2) / 2,
      onUpdate: () => {
        this["_endLayerInternal"]["y"] = -640 * options10["p"];
      },
      onComplete: () => {
        (this["_endLayerInternal"]["destroy"](),
          (this["_endLayerInternal"] = null),
          this["_endLayerOverlay"] &&
            (this["_endLayerOverlay"]["destroy"](),
            (this["_endLayerOverlay"] = null)),
          value71 && value71());
      },
    }),
      this["tweens"]["add"]({
        targets: this["_endLayerOverlay"],
        alpha: 0,
        duration: 500,
      }));
  }
}
