class Player {
  constructor(value71, other, extra) {
    ((this["_scene"] = value71),
      (this["p"] = other),
      (this["_gameLayer"] = extra),
      (this["_rotation"] = 0),
      (this["rotateActionActive"] = !1),
      (this["rotateActionTime"] = 0),
      (this["rotateActionDuration"] = 0),
      (this["rotateActionStart"] = 0),
      (this["rotateActionTotal"] = 0),
      (this["_showHitboxes"] = !1),
      (this["_lastLandObject"] = null),
      (this["_lastXOffset"] = 0),
      (this["_lastCameraX"] = 0),
      (this["_lastCameraY"] = 0),
      this["_createSprites"](),
      this["_initParticles"](value71),
      value71["events"]["on"]("shutdown", () => this["_cleanupExplosion"]()));
  }
  ["_createSprites"]() {
    const scene = this["_scene"],
      helperFn22 = flipY(this["p"]["y"]),
      value71 = groundYOffset;
    if (
      ((this["_playerGlowLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "player_01_glow_001.png",
        9,
        !1,
      )),
      (this["_playerSpriteLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "player_01_001.png",
        10,
        !0,
      )),
      (this["_playerOverlayLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "player_01_2_001.png",
        8,
        !0,
      )),
      (this["_playerExtraLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "player_01_extra_001.png",
        12,
        !0,
      )),
      this["_playerGlowLayer"] &&
        (this["_playerGlowLayer"]["sprite"]["setTint"](colorCyanTint),
        (this["_playerGlowLayer"]["sprite"]["_glowEnabled"] = !1)),
      this["_playerSpriteLayer"])
    )
      this["_playerSpriteLayer"]["sprite"]["setTint"](colorGreenTint);
    else {
      let rectangle = scene["add"]["rectangle"](
        value71,
        helperFn22,
        baseUnitAlias,
        baseUnitAlias,
        colorGreenTint,
      );
      (rectangle["setDepth"](10),
        (this["_playerSpriteLayer"] = {
          sprite: rectangle,
        }));
    }
    if (
      (this["_playerOverlayLayer"] &&
        this["_playerOverlayLayer"]["sprite"]["setTint"](colorCyanTint),
      (this["_shipGlowLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "ship_01_glow_001.png",
        9,
        !1,
      )),
      (this["_shipSpriteLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "ship_01_001.png",
        10,
        !1,
      )),
      (this["_shipOverlayLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "ship_01_2_001.png",
        8,
        !1,
      )),
      (this["_shipExtraLayer"] = createLayeredSprite(
        scene,
        value71,
        helperFn22,
        "ship_01_extra_001.png",
        12,
        !1,
      )),
      this["_shipGlowLayer"] &&
        (this["_shipGlowLayer"]["sprite"]["setTint"](colorCyanTint),
        (this["_shipGlowLayer"]["sprite"]["_glowEnabled"] = !1)),
      this["_shipSpriteLayer"])
    )
      this["_shipSpriteLayer"]["sprite"]["setTint"](colorGreenTint);
    else {
      let polygon = scene["add"]["polygon"](
        value71,
        helperFn22,
        [
          {
            x: -72,
            y: 40,
          },
          {
            x: 72,
            y: 0,
          },
          {
            x: -72,
            y: -40,
          },
          {
            x: -40,
            y: 0,
          },
        ],
        colorGreenTint,
      );
      (polygon["setDepth"](10)["setVisible"](!1),
        (this["_shipSpriteLayer"] = {
          sprite: polygon,
        }));
    }
    (this["_shipOverlayLayer"] &&
      this["_shipOverlayLayer"]["sprite"]["setTint"](colorCyanTint),
      (this["playerSprite"] = this["_playerSpriteLayer"]["sprite"]),
      (this["shipSprite"] = this["_shipSpriteLayer"]["sprite"]),
      (this["_playerLayers"] = [
        this["_playerSpriteLayer"],
        this["_playerGlowLayer"],
        this["_playerOverlayLayer"],
        this["_playerExtraLayer"],
      ]),
      (this["_shipLayers"] = [
        this["_shipSpriteLayer"],
        this["_shipGlowLayer"],
        this["_shipOverlayLayer"],
        this["_shipExtraLayer"],
      ]),
      (this["_allLayers"] = [
        ...this["_playerLayers"],
        ...this["_shipLayers"],
      ]));
  }
  ["_initParticles"](value71) {
    ((this["_particleEmitter"] = value71["add"]["particles"](
      0,
      0,
      "GJ_WebSheet",
      {
        frame: "square.png",
        speed: {
          min: 110,
          max: 190,
        },
        angle: {
          min: 225,
          max: 315,
        },
        lifespan: {
          min: 150,
          max: 450,
        },
        scale: {
          start: 0.5,
          end: 0,
        },
        gravityY: 600,
        frequency: 1e3 / 30,
        blendMode: "ADD",
        alpha: {
          start: 1,
          end: 0,
        },
        tint: colorGreenTint,
      },
    )),
      this["_particleEmitter"]["stop"](),
      this["_particleEmitter"]["setDepth"](9),
      this["_gameLayer"]["container"]["add"](this["_particleEmitter"]),
      (this["_flyParticleEmitter"] = value71["add"]["particles"](
        0,
        0,
        "GJ_WebSheet",
        {
          frame: "square.png",
          speed: {
            min: 22,
            max: 38,
          },
          angle: {
            min: 225,
            max: 315,
          },
          lifespan: {
            min: 150,
            max: 450,
          },
          scale: {
            start: 0.5,
            end: 0,
          },
          gravityY: 600,
          frequency: 1e3 / 30,
          blendMode: "ADD",
          tint: {
            start: 16737280,
            end: 16711680,
          },
          alpha: {
            start: 1,
            end: 0,
          },
        },
      )),
      this["_flyParticleEmitter"]["stop"](),
      this["_flyParticleEmitter"]["setDepth"](9),
      this["_gameLayer"]["container"]["add"](this["_flyParticleEmitter"]),
      (this["_flyParticle2Emitter"] = value71["add"]["particles"](
        0,
        0,
        "GJ_WebSheet",
        {
          frame: "square.png",
          speed: {
            min: 220,
            max: 380,
          },
          angle: {
            min: 180,
            max: 360,
          },
          lifespan: {
            min: 150,
            max: 450,
          },
          scale: {
            start: 0.75,
            end: 0,
          },
          gravityY: 600,
          frequency: 1e3 / 30,
          blendMode: "ADD",
          tint: {
            start: 16760320,
            end: 16711680,
          },
          alpha: {
            start: 1,
            end: 0,
          },
        },
      )),
      this["_flyParticle2Emitter"]["stop"](),
      this["_flyParticle2Emitter"]["setDepth"](9),
      this["_gameLayer"]["container"]["add"](this["_flyParticle2Emitter"]),
      (this["_shipDragEmitter"] = value71["add"]["particles"](
        0,
        0,
        "GJ_WebSheet",
        {
          frame: "square.png",
          x: {
            min: -18,
            max: 18,
          },
          speed: {
            min: 149.2 * 1.5,
            max: 229.2 * 1.5,
          },
          angle: {
            min: 205,
            max: 295,
          },
          lifespan: {
            min: 80,
            max: 220,
          },
          scale: {
            start: 0.375,
            end: 0,
          },
          gravityX: -700,
          gravityY: 600,
          frequency: 25,
          blendMode: "ADD",
          alpha: {
            start: 1,
            end: 0,
          },
        },
      )),
      this["_shipDragEmitter"]["stop"](),
      this["_shipDragEmitter"]["setDepth"](22),
      (this["_shipDragActive"] = !1),
      (this["_particleActive"] = !1),
      (this["_flyParticle2Active"] = !1),
      (this["_flyParticleActive"] = !1));
    const options10 = {
      frame: "square.png",
      speed: {
        min: 250,
        max: 350,
      },
      angle: {
        min: 210,
        max: 330,
      },
      lifespan: {
        min: 50,
        max: 600,
      },
      scale: {
        start: 0.625,
        end: 0,
      },
      gravityY: 1e3,
      blendMode: "ADD",
      alpha: {
        start: 1,
        end: 0,
      },
      tint: colorGreenTint,
      emitting: !1,
    };
    ((this["_landEmitter1"] = value71["add"]["particles"](0, 0, "GJ_WebSheet", {
      ...options10,
    })),
      (this["_landEmitter2"] = value71["add"]["particles"](
        0,
        0,
        "GJ_WebSheet",
        {
          ...options10,
        },
      )),
      (this["_aboveContainer"] = value71["add"]["container"](0, 0)),
      this["_aboveContainer"]["setDepth"](13),
      this["_aboveContainer"]["add"](this["_landEmitter1"]),
      this["_aboveContainer"]["add"](this["_landEmitter2"]),
      (this["_landIdx"] = !1),
      (this["_streak"] = new TrailRenderer(
        this["_scene"],
        "streak_01",
        0.231,
        10,
        8,
        100,
        colorCyanTint,
        0.7,
      )),
      this["_streak"]["addToContainer"](this["_gameLayer"]["container"], 8));
  }
  ["_updateParticles"](value71, other, extra) {
    if (this["p"]["isDead"]) return;
    const playerWorldX = this["_scene"]["_playerWorldX"],
      helperFn22 = flipY(this["p"]["y"]);
    ((this["_particleEmitter"]["particleX"] = playerWorldX - 20),
      (this["_particleEmitter"]["particleY"] = helperFn22 + 26));
    const value72 = this["p"]["onGround"] && !this["p"]["isFlying"];
    value72 && !this["_particleActive"]
      ? (this["_particleEmitter"]["start"](), (this["_particleActive"] = !0))
      : !value72 &&
        this["_particleActive"] &&
        (this["_particleEmitter"]["stop"](), (this["_particleActive"] = !1));
    {
      const value75 = Math["cos"](this["_rotation"]),
        value76 = Math["sin"](this["_rotation"]),
        value77 = -24,
        num32 = 18,
        value78 = playerWorldX + value77 * value75 - num32 * value76,
        value79 = helperFn22 + value77 * value76 + num32 * value75,
        value80 = 2 * (2 * Math["random"]() - 1) * 2;
      ((this["_flyParticleEmitter"]["particleX"] = value78),
        (this["_flyParticleEmitter"]["particleY"] = value79 + value80),
        (this["_flyParticle2Emitter"]["particleX"] = value78),
        (this["_flyParticle2Emitter"]["particleY"] = value79 + value80),
        this["_streak"]["setPosition"](value78 + 8, value79));
    }
    this["_streak"]["update"](extra);
    const isFlying = this["p"]["isFlying"];
    isFlying && !this["_flyParticleActive"]
      ? (this["_flyParticleEmitter"]["start"](),
        (this["_flyParticleActive"] = !0))
      : !isFlying &&
        this["_flyParticleActive"] &&
        (this["_flyParticleEmitter"]["stop"](),
        (this["_flyParticleActive"] = !1));
    const value73 = this["p"]["isFlying"] && this["p"]["upKeyDown"];
    (value73 && !this["_flyParticle2Active"]
      ? (this["_flyParticle2Emitter"]["start"](),
        (this["_flyParticle2Active"] = !0))
      : !value73 &&
        this["_flyParticle2Active"] &&
        (this["_flyParticle2Emitter"]["stop"](),
        (this["_flyParticle2Active"] = !1)),
      (this["_shipDragEmitter"]["x"] = groundYOffset),
      (this["_shipDragEmitter"]["particleY"] =
        flipY(this["p"]["y"]) + other + 30));
    const value74 =
      this["p"]["isFlying"] && this["p"]["onGround"] && !this["p"]["onCeiling"];
    value74 && !this["_shipDragActive"]
      ? (this["_shipDragEmitter"]["start"](), (this["_shipDragActive"] = !0))
      : !value74 &&
        this["_shipDragActive"] &&
        (this["_shipDragEmitter"]["stop"](), (this["_shipDragActive"] = !1));
  }
  ["setCubeVisible"](value71) {
    (this["_playerSpriteLayer"]["sprite"]["setVisible"](value71),
      this["_playerGlowLayer"] &&
        this["_playerGlowLayer"]["sprite"]["setVisible"](
          value71 && this["_playerGlowLayer"]["sprite"]["_glowEnabled"],
        ),
      this["_playerOverlayLayer"] &&
        this["_playerOverlayLayer"]["sprite"]["setVisible"](value71),
      this["_playerExtraLayer"] &&
        this["_playerExtraLayer"]["sprite"]["setVisible"](value71));
  }
  ["setShipVisible"](value71) {
    (this["_shipSpriteLayer"]["sprite"]["setVisible"](value71),
      this["_shipGlowLayer"] &&
        this["_shipGlowLayer"]["sprite"]["setVisible"](
          value71 && this["_shipGlowLayer"]["sprite"]["_glowEnabled"],
        ),
      this["_shipOverlayLayer"] &&
        this["_shipOverlayLayer"]["sprite"]["setVisible"](value71),
      this["_shipExtraLayer"] &&
        this["_shipExtraLayer"]["sprite"]["setVisible"](value71));
  }
  ["syncSprites"](value71, other, extra, extra2) {
    if (this["_endAnimating"]) return;
    const value72 = void 0 !== extra2 ? extra2 : groundYOffset,
      value73 = flipY(this["p"]["y"]) + other,
      rotation = this["_rotation"];
    if (
      ((this["_lastCameraX"] = value71),
      (this["_lastCameraY"] = other),
      (this["_aboveContainer"]["x"] = -value71),
      (this["_aboveContainer"]["y"] = other),
      this["p"]["isFlying"])
    ) {
      const num32 = 10,
        value74 = Math["cos"](rotation),
        value75 = Math["sin"](rotation),
        value76 = -num32 * value75,
        value77 = num32 * value74,
        value78 = num32 * value75,
        value79 = -num32 * value74;
      for (const item of this["_shipLayers"])
        item &&
          ((item["sprite"]["x"] = value72 + value76),
          (item["sprite"]["y"] = value73 + value77),
          (item["sprite"]["rotation"] = rotation));
      for (const item of this["_playerLayers"])
        item &&
          ((item["sprite"]["x"] = value72 + value78),
          (item["sprite"]["y"] = value73 + value79),
          (item["sprite"]["rotation"] = rotation));
    } else
      for (const item of this["_allLayers"])
        item &&
          ((item["sprite"]["x"] = value72),
          (item["sprite"]["y"] = value73),
          (item["sprite"]["rotation"] = rotation));
    this["_updateParticles"](value71, other, extra);
  }
  ["enterShipMode"](value71 = null) {
    if (this["p"]["isFlying"]) return;
    ((this["p"]["isFlying"] = !0),
      this["_scene"]["toggleGlitter"](!0),
      (this["p"]["yVelocity"] *= 0.5),
      (this["p"]["onGround"] = !1),
      (this["p"]["canJump"] = !1),
      (this["p"]["isJumping"] = !1),
      this["stopRotation"](),
      (this["_rotation"] = 0),
      this["_particleEmitter"]["stop"](),
      (this["_flyParticle2Active"] = !1),
      this["_streak"]["reset"](),
      this["_streak"]["start"](),
      this["setShipVisible"](!0));
    for (const item of this["_playerLayers"])
      item && item["sprite"]["setScale"](0.55);
    let value72 = this["p"]["y"];
    (value71 &&
      (value72 =
        void 0 !== value71["portalY"] ? value71["portalY"] : value71["y"]),
      this["_gameLayer"]["setFlyMode"](!0, value72));
  }
  ["exitShipMode"]() {
    if (this["p"]["isFlying"]) {
      ((this["p"]["isFlying"] = !1),
        this["_scene"]["toggleGlitter"](!1),
        (this["p"]["yVelocity"] *= 0.5),
        (this["p"]["onGround"] = !1),
        (this["p"]["canJump"] = !1),
        (this["p"]["isJumping"] = !1),
        this["stopRotation"](),
        (this["_rotation"] = 0),
        this["_flyParticleEmitter"]["stop"](),
        (this["_flyParticleActive"] = !1),
        this["_flyParticle2Emitter"]["stop"](),
        (this["_flyParticle2Active"] = !1),
        this["_shipDragEmitter"]["stop"](),
        (this["_shipDragActive"] = !1),
        (this["_particleActive"] = !1),
        this["_streak"]["stop"](),
        this["_streak"]["reset"](),
        this["setShipVisible"](!1),
        this["setCubeVisible"](!0));
      for (const item of this["_playerLayers"])
        item && item["sprite"]["setScale"](1);
      this["_gameLayer"]["setFlyMode"](!1, 0);
    }
  }
  ["hitGround"]() {
    const value71 = !this["p"]["onGround"];
    if (
      (this["p"]["isFlying"] || (this["p"]["lastGroundY"] = this["p"]["y"]),
      (this["p"]["yVelocity"] = 0),
      (this["p"]["onGround"] = !0),
      (this["p"]["canJump"] = !0),
      (this["p"]["isJumping"] = !1),
      this["stopRotation"](),
      value71 && !this["p"]["isFlying"])
    ) {
      this["_landIdx"] = !this["_landIdx"];
      const value72 = this["_landIdx"]
          ? this["_landEmitter1"]
          : this["_landEmitter2"],
        value73 = this["_lastCameraX"] + groundYOffset,
        value74 = flipY(this["p"]["y"]) + 30;
      value72["explode"](10, value73, value74);
    }
  }
  ["killPlayer"]() {
    if (this["p"]["isDead"]) return;
    ((this["p"]["isDead"] = !0),
      this["_scene"]["toggleGlitter"](!1),
      this["_particleEmitter"]["stop"](),
      (this["_particleActive"] = !1),
      this["_flyParticleEmitter"]["stop"](),
      (this["_flyParticleActive"] = !1),
      this["_flyParticle2Emitter"]["stop"](),
      (this["_flyParticle2Active"] = !1),
      this["_shipDragEmitter"]["stop"](),
      (this["_shipDragActive"] = !1),
      this["_streak"]["stop"](),
      this["_streak"]["reset"]());
    const scene = this["_scene"],
      value71 = scene["_playerWorldX"] - scene["_cameraX"],
      value72 = flipY(this["p"]["y"]) + this["_lastCameraY"],
      num32 = 0.9;
    scene["add"]
      ["particles"](value71, value72, "GJ_WebSheet", {
        frame: "square.png",
        speed: {
          min: 200,
          max: 800,
        },
        angle: {
          min: 0,
          max: 360,
        },
        scale: {
          start: 18 / 32,
          end: 0,
        },
        alpha: {
          start: 1,
          end: 0,
        },
        lifespan: {
          min: 50,
          max: 800,
        },
        quantity: 100,
        stopAfter: 100,
        blendMode: blendAdd,
        tint: colorGreenTint,
        x: {
          min: -20,
          max: 20,
        },
        y: {
          min: -20,
          max: 20,
        },
      })
      ["setScrollFactor"](0)
      ["setDepth"](15);
    const graphics = scene["add"]
        ["graphics"]()
        ["setScrollFactor"](0)
        ["setDepth"](15)
        ["setBlendMode"](blendAdd),
      options10 = {
        t: 0,
      };
    (scene["tweens"]["add"]({
      targets: options10,
      t: 1,
      duration: 500,
      ease: "Quad.Out",
      onUpdate: () => {
        const value73 = 18 + 144 * options10["t"],
          value74 = 1 - options10["t"];
        (graphics["clear"](),
          graphics["fillStyle"](colorGreenTint, value74),
          graphics["fillCircle"](value71, value72, value73));
      },
      onComplete: () => graphics["destroy"](),
    }),
      this["_createExplosionPieces"](value71, value72, num32),
      this["setCubeVisible"](!1),
      this["setShipVisible"](!1));
  }
  ["_createExplosionPieces"](value71, other, extra) {
    const scene = this["_scene"],
      value72 = 40 * extra,
      value73 = Math["round"](2 * value72),
      value74 = scene["make"]["renderTexture"]({
        x: 0,
        y: 0,
        width: value73,
        height: value73,
        add: !1,
      }),
      table4 = [
        this["_playerGlowLayer"],
        this["_playerOverlayLayer"],
        this["_shipGlowLayer"],
        this["_shipOverlayLayer"],
        this["_playerSpriteLayer"],
        this["_playerExtraLayer"],
        this["_shipSpriteLayer"],
        this["_shipExtraLayer"],
      ];
    for (const item of table4) {
      if (!item || !item["sprite"]["visible"]) continue;
      const sprite = item["sprite"];
      value74["draw"](
        sprite,
        value73 / 2 + (sprite["x"] - value71),
        value73 / 2 + (sprite["y"] - other),
      );
    }
    const value75 = "__deathRT_" + Date["now"]();
    value74["saveTexture"](value75);
    const value76 = scene["textures"]["get"](value75);
    let value77 = 2 + Math["round"](2 * Math["random"]()),
      value78 = 2 + Math["round"](2 * Math["random"]());
    const value79 = Math["random"]();
    value79 > 0.95 ? (value77 = 1) : value79 > 0.9 && (value78 = 1);
    const value80 = 9.34740324 * 0.8,
      value81 = 0.5 * value80,
      value82 = 1 * value80,
      num32 = 0.45,
      value83 = value73 / value77,
      value84 = value73 / value78,
      table5 = [],
      table6 = [],
      table7 = [0],
      table8 = [0];
    let num33 = 0,
      num34 = 0;
    for (let i = 0; i < value77 - 1; i++) {
      const value85 = Math["round"](
        value83 * (0.55 + Math["random"]() * num32 * 2),
      );
      (table5["push"](value85), (num33 += value85), table7["push"](num33));
    }
    table5["push"](value73 - num33);
    for (let i = 0; i < value78 - 1; i++) {
      const value85 = Math["round"](
        value84 * (0.55 + Math["random"]() * num32 * 2),
      );
      (table6["push"](value85), (num34 += value85), table8["push"](num34));
    }
    (table6["push"](value73 - num34),
      (this["_explosionPieces"] = []),
      (this["_explosionContainer"] = scene["add"]
        ["container"](value71, other)
        ["setDepth"](16)));
    let num35 = 0;
    for (let i = 0; i < value77; i++) {
      const i2 = table5[i],
        i3 = table7[i];
      for (let i4 = 0; i4 < value78; i4++) {
        const value85 = table6[i4],
          value86 = table8[i4];
        if (i2 <= 0 || value85 <= 0) continue;
        num35++;
        const value87 = "piece_" + i + "_" + i4;
        value76["add"](value87, 0, i3, value86, i2, value85);
        const image = scene["add"]["image"](0, 0, value75, value87);
        ((image["x"] = i3 + i2 / 2 - value73 / 2),
          (image["y"] = -(value86 + value85 / 2 - value73 / 2)),
          this["_explosionContainer"]["add"](image));
        let value88 = null;
        if (num35 % 2 == 0) {
          const value89 = 200 + 200 * Math["random"](),
            point = image;
          ((value88 = scene["add"]["particles"](0, 0, "GJ_WebSheet", {
            frame: "square.png",
            speed: 0,
            scale: {
              start: 0.5,
              end: 0,
            },
            alpha: {
              start: 1,
              end: 0,
            },
            lifespan: value89,
            frequency: 25,
            quantity: 1,
            emitting: !0,
            blendMode: blendAdd,
            tint: colorGreenTint,
            emitCallback: (point2) => {
              ((point2["x"] = point["x"] + 3 * (2 * Math["random"]() - 1) * 2),
                (point2["y"] =
                  point["y"] + 3 * (2 * Math["random"]() - 1) * 2));
            },
          })),
            this["_explosionContainer"]["addAt"](value88, 0));
        }
        const options10 = {
          spr: image,
          particle: value88,
          xVel: value81 + (2 * Math["random"]() - 1) * value82,
          yVel: -(12 + 6 * (2 * Math["random"]() - 1)),
          timer: 1.4,
          fadeTime: 0.5,
          rotDelta: (360 * (2 * Math["random"]() - 1)) / 60,
          halfSize: Math["min"](i2, value85) / 2,
        };
        this["_explosionPieces"]["push"](options10);
      }
    }
    ((this["_explosionGroundSY"] = flipY(0) + this["_lastCameraY"]),
      (this["_explosionRT"] = value74),
      (this["_explosionTexKey"] = value75));
  }
  ["updateExplosionPieces"](value71) {
    if (!this["_explosionPieces"] || 0 === this["_explosionPieces"]["length"])
      return;
    const value72 = value71 / 1e3,
      value73 = Math["min"](60 * value72 * 0.9, 2),
      value74 = 0.5 * value73 * 2,
      value75 = this["_explosionGroundSY"] - this["_explosionContainer"]["y"];
    let num32 = 0;
    for (; num32 < this["_explosionPieces"]["length"];) {
      const num322 = this["_explosionPieces"][num32];
      if (((num322["timer"] -= value72), num322["timer"] > 0)) {
        {
          ((num322["yVel"] += value74),
            (num322["xVel"] *= 0.98 + 0.02 * (1 - value73)));
          let value76 = num322["spr"]["x"] + num322["xVel"] * value73,
            value77 = num322["spr"]["y"] + num322["yVel"] * value73;
          const value78 = value75 - num322["halfSize"];
          if (
            (value77 > value78 &&
              num322["yVel"] > 0 &&
              ((value77 = value78),
              (num322["yVel"] *= -0.8),
              Math["abs"](num322["yVel"]) < 3 && (num322["yVel"] = -3)),
            (num322["spr"]["x"] = value76),
            (num322["spr"]["y"] = value77),
            (num322["spr"]["angle"] += num322["rotDelta"] * value73),
            num322["timer"] < num322["fadeTime"])
          ) {
            const value79 = num322["timer"] / num322["fadeTime"];
            (num322["spr"]["setAlpha"](value79),
              num322["particle"] && num322["particle"]["setAlpha"](value79));
          }
        }
        num32++;
      } else
        (num322["particle"] &&
          (num322["particle"]["stop"](), num322["particle"]["destroy"]()),
          num322["spr"]["destroy"](),
          this["_explosionPieces"]["splice"](num32, 1));
    }
    0 === this["_explosionPieces"]["length"] && this["_cleanupExplosion"]();
  }
  ["_cleanupExplosion"]() {
    if (this["_explosionPieces"])
      for (const item of this["_explosionPieces"])
        (item["particle"] &&
          (item["particle"]["stop"](), item["particle"]["destroy"]()),
          item["spr"] && item["spr"]["destroy"]());
    (this["_explosionContainer"] &&
      (this["_explosionContainer"]["destroy"](),
      (this["_explosionContainer"] = null)),
      this["_explosionTexKey"] &&
        (this["_scene"]["textures"]["remove"](this["_explosionTexKey"]),
        (this["_explosionTexKey"] = null)),
      this["_explosionRT"] &&
        (this["_explosionRT"]["destroy"](), (this["_explosionRT"] = null)),
      (this["_explosionPieces"] = null));
  }
  ["_playPortalShine"](point) {
    const scene = this["_scene"],
      value71 = point["x"],
      helperFn22 = flipY(point["portalY"]),
      table4 = ["portalshine_02_front_001.png", "portalshine_02_back_001.png"],
      table5 = [
        this["_gameLayer"]["topContainer"],
        this["_gameLayer"]["container"],
      ];
    for (let i = 0; i < 2; i++) {
      const helperFn42 = findAtlasFrame(scene, table4[i]);
      if (!helperFn42) continue;
      const image = scene["add"]["image"](
        value71,
        helperFn22,
        helperFn42["atlas"],
        helperFn42["frame"],
      );
      (image["setBlendMode"](blendAdd),
        image["setAlpha"](0),
        table5[i]["add"](image),
        scene["tweens"]["add"]({
          targets: image,
          alpha: {
            from: 0,
            to: 1,
          },
          duration: 50,
          onComplete: () => {
            scene["tweens"]["add"]({
              targets: image,
              alpha: 0,
              duration: 400,
              onComplete: () => image["destroy"](),
            });
          },
        }));
    }
  }
  ["_checkSnapJump"](point) {
    const table4 = [
        {
          dx: 240,
          dy: 60,
        },
        {
          dx: 300,
          dy: -60,
        },
        {
          dx: 180,
          dy: 120,
        },
      ],
      lastLandObject = this["_lastLandObject"];
    if (
      lastLandObject &&
      lastLandObject !== point &&
      lastLandObject["type"] === solid
    ) {
      const value71 = lastLandObject["x"],
        value72 = lastLandObject["y"],
        value73 = point["x"],
        value74 = point["y"],
        value75 = this["p"]["gravityFlipped"] ? -1 : 1;
      let value76 = !1;
      for (const item of table4)
        if (
          Math["abs"](value73 - (value71 + item["dx"])) <= 2 &&
          Math["abs"](value74 - (value72 + item["dy"] * value75)) <= 2
        ) {
          value76 = !0;
          break;
        }
      if (value76) {
        const value77 = point["x"] + this["_lastXOffset"],
          playerWorldX = this["_scene"]["_playerWorldX"];
        let value78;
        ((value78 =
          Math["abs"](value77 - playerWorldX) <= 2
            ? value77
            : value77 > playerWorldX
              ? playerWorldX + 2
              : playerWorldX - 2),
          (this["_scene"]["_playerWorldX"] = value78));
      }
    }
    ((this["_lastLandObject"] = point),
      (this["_lastXOffset"] = this["_scene"]["_playerWorldX"] - point["x"]));
  }
  ["_isFallingPastThreshold"]() {
    return this["p"]["gravityFlipped"]
      ? this["p"]["yVelocity"] > 0.25
      : this["p"]["yVelocity"] < -0.25;
  }
  ["flipMod"]() {
    return this["p"]["gravityFlipped"] ? -1 : 1;
  }
  ["runRotateAction"]() {
    ((this["rotateActionActive"] = !0),
      (this["rotateActionTime"] = 0),
      (this["rotateActionDuration"] = 0.39 / physicsConst09),
      (this["rotateActionStart"] = this["_rotation"]),
      (this["rotateActionTotal"] = Math["PI"] * this["flipMod"]()));
  }
  ["stopRotation"]() {
    this["rotateActionActive"] = !1;
  }
  ["updateRotateAction"](value71) {
    if (!this["rotateActionActive"]) return;
    ((this["rotateActionTime"] += value71),
      this["rotateActionTime"] >= this["rotateActionDuration"] &&
        (this["rotateActionActive"] = !1));
    let value72 = Math["min"](
      this["rotateActionTime"] / this["rotateActionDuration"],
      1,
    );
    this["_rotation"] =
      this["rotateActionStart"] + this["rotateActionTotal"] * value72;
  }
  ["convertToClosestRotation"]() {
    let value71 = Math["PI"] / 2;
    return Math["round"](this["_rotation"] / value71) * value71;
  }
  ["slerp2D"](value71, other, extra) {
    let value72 = other - value71;
    for (; value72 > Math["PI"];) value72 -= 2 * Math["PI"];
    for (; value72 < -Math["PI"];) value72 += 2 * Math["PI"];
    return value71 + value72 * extra;
  }
  ["updateGroundRotation"](value71) {
    let value72 = this["convertToClosestRotation"](),
      value73 = 0.1575 * 3,
      value74 = Math["min"](1 * value71, value73 * value71);
    this["_rotation"] = this["slerp2D"](this["_rotation"], value72, value74);
  }
  ["updateShipRotation"](value71) {
    let value72 = -(this["p"]["y"] - this["p"]["lastY"]),
      value73 = 10.3860036 * value71;
    if (value73 * value73 + value72 * value72 >= 0.6 * value71) {
      let value74 = Math["atan2"](value72, value73),
        num32 = 0.15,
        value75 = Math["min"](1 * value71, num32 * value71);
      this["_rotation"] = this["slerp2D"](this["_rotation"], value74, value75);
    }
  }
  ["playerIsFalling"]() {
    return this["p"]["gravityFlipped"]
      ? this["p"]["yVelocity"] > 3.832796
      : this["p"]["yVelocity"] < 3.832796;
  }
  ["updateJump"](value71) {
    if (this["p"]["isFlying"]) this["_updateFlyJump"](value71);
    else if (this["p"]["upKeyDown"] && this["p"]["canJump"])
      ((this["p"]["isJumping"] = !0),
        (this["p"]["onGround"] = !1),
        (this["p"]["canJump"] = !1),
        (this["p"]["upKeyPressed"] = !1),
        (this["p"]["yVelocity"] = 22.360064 * this["flipMod"]()),
        this["runRotateAction"]());
    else if (this["p"]["isJumping"])
      ((this["p"]["yVelocity"] -=
        physicsConst1916 * value71 * this["flipMod"]()),
        this["playerIsFalling"]() &&
          ((this["p"]["isJumping"] = !1), (this["p"]["onGround"] = !1)));
    else if (
      (this["playerIsFalling"]() && (this["p"]["canJump"] = !1),
      (this["p"]["yVelocity"] -=
        physicsConst1916 * value71 * this["flipMod"]()),
      this["p"]["gravityFlipped"]
        ? (this["p"]["yVelocity"] = Math["min"](this["p"]["yVelocity"], 30))
        : (this["p"]["yVelocity"] = Math["max"](this["p"]["yVelocity"], -30)),
      this["_isFallingPastThreshold"]() &&
        !this["rotateActionActive"] &&
        this["runRotateAction"](),
      this["playerIsFalling"]())
    ) {
      let value72;
      ((value72 = this["p"]["gravityFlipped"]
        ? this["p"]["yVelocity"] > 4
        : this["p"]["yVelocity"] < -4),
        value72 && (this["p"]["onGround"] = !1));
    }
  }
  ["_updateFlyJump"](value71) {
    let num32 = 0.8;
    (this["p"]["upKeyDown"] && !this["p"]["wasBoosted"] && (num32 = -1),
      this["p"]["upKeyDown"] || this["playerIsFalling"]() || (num32 = 1.2));
    let num33 = 0.4;
    (this["p"]["upKeyDown"] && this["playerIsFalling"]() && (num33 = 0.5),
      (this["p"]["yVelocity"] -=
        physicsConst1916 * value71 * this["flipMod"]() * num32 * num33),
      this["p"]["upKeyDown"] && (this["p"]["onGround"] = !1),
      this["p"]["wasBoosted"] ||
        (this["p"]["gravityFlipped"]
          ? ((this["p"]["yVelocity"] = Math["max"](
              this["p"]["yVelocity"],
              -16,
            )),
            (this["p"]["yVelocity"] = Math["min"](
              this["p"]["yVelocity"],
              12.8,
            )))
          : ((this["p"]["yVelocity"] = Math["max"](
              this["p"]["yVelocity"],
              -12.8,
            )),
            (this["p"]["yVelocity"] = Math["min"](
              this["p"]["yVelocity"],
              16,
            )))));
  }
  ["checkCollisions"](value71) {
    const num32 = 30,
      value72 = value71 + groundYOffset,
      value73 = this["p"]["y"],
      lastY = this["p"]["lastY"],
      value74 = this["p"]["isFlying"] ? 12 : 20;
    ((this["p"]["collideTop"] = 0),
      (this["p"]["collideBottom"] = 0),
      (this["p"]["onCeiling"] = !1));
    let value75 = !1;
    const value76 = this["_gameLayer"]["getNearbySectionObjects"](value72);
    for (let item of value76) {
      let value79 = item["x"] - item["w"] / 2,
        value80 = item["x"] + item["w"] / 2,
        value81 = item["y"] - item["h"] / 2,
        value82 = item["y"] + item["h"] / 2;
      if (!(
        value72 + 30 <= value79 ||
        value72 - 30 >= value80 ||
        value73 + num32 <= value81 ||
        value73 - num32 >= value82
      ))
        if (item["type"] !== portalFly) {
          if (item["type"] !== portalCube) {
            if (item["type"] === hazard) return void this["killPlayer"]();
            if (item["type"] === solid) {
              let value83 = value73 - num32 + value74,
                value84 = lastY - num32 + value74,
                value85 = value73 + num32 - value74,
                value86 = lastY + num32 - value74;
              const num33 = 9,
                value87 =
                  value72 + num33 > value79 &&
                  value72 - num33 < value80 &&
                  value73 + num33 > value81 &&
                  value73 - num33 < value82,
                value88 =
                  (this["p"]["yVelocity"] <= 0 || this["p"]["onGround"]) &&
                  (value83 >= value82 || value84 >= value82);
              if (value87 && !value88) return void this["killPlayer"]();
              if (value72 + 30 - 5 > value79 && value72 - 30 + 5 < value80) {
                if (
                  (value83 >= value82 || value84 >= value82) &&
                  (this["p"]["yVelocity"] <= 0 || this["p"]["onGround"])
                ) {
                  ((this["p"]["y"] = value82 + num32),
                    this["hitGround"](),
                    (value75 = !0),
                    (this["p"]["collideBottom"] = value82),
                    this["p"]["isFlying"] || this["_checkSnapJump"](item));
                  continue;
                }
                if (
                  (value85 <= value81 || value86 <= value81) &&
                  (this["p"]["yVelocity"] >= 0 || this["p"]["onGround"]) &&
                  this["p"]["isFlying"]
                ) {
                  ((this["p"]["y"] = value81 - num32),
                    this["hitGround"](),
                    (this["p"]["onCeiling"] = !0),
                    (this["p"]["collideTop"] = value81));
                  continue;
                }
              }
            }
          } else
            item["activated"] ||
              ((item["activated"] = !0),
              this["_playPortalShine"](item),
              this["exitShipMode"]());
        } else
          item["activated"] ||
            ((item["activated"] = !0),
            this["_playPortalShine"](item),
            this["enterShipMode"](item));
    }
    if (0 !== this["p"]["collideTop"] && 0 !== this["p"]["collideBottom"])
      if (
        Math["abs"](this["p"]["collideTop"] - this["p"]["collideBottom"]) < 48
      )
        return void this["killPlayer"]();
    let value77 = this["_gameLayer"]["getFloorY"]();
    value75 ||
      (this["p"]["y"] <= value77 + 30 &&
        ((this["p"]["y"] = value77 + 30), this["hitGround"]()));
    let value78 = this["_gameLayer"]["getCeilingY"]();
    if (
      (null !== value78 &&
        this["p"]["y"] >= value78 - 30 &&
        ((this["p"]["y"] = value78 - 30),
        this["hitGround"](),
        (this["p"]["onCeiling"] = !0)),
      this["p"]["isFlying"])
    ) {
      const value79 = this["p"]["y"] <= value77 + 30,
        value80 = null !== value78 && this["p"]["y"] >= value78 - 30;
      value75 ||
        value79 ||
        0 !== this["p"]["collideTop"] ||
        value80 ||
        (this["p"]["onGround"] = !1);
    }
  }
  ["drawHitboxes"](value71, other, extra) {
    if ((value71["clear"](), !this["_showHitboxes"])) return;
    const num32 = 30,
      num33 = 30,
      value72 = other + groundYOffset,
      value73 = this["p"]["y"],
      value74 = this["p"]["isFlying"] ? 12 : 20,
      value75 = this["_gameLayer"]["getNearbySectionObjects"](value72);
    for (let item of value75) {
      let value80 = item["x"] - other,
        value81 = flipY(item["y"]) + extra,
        num34 = 65280;
      (item["type"] === hazard
        ? (num34 = 16729156)
        : (item["type"] !== portalFly && item["type"] !== portalCube) ||
          (num34 = 4491519),
        value71["lineStyle"](2, num34, 0.7),
        value71["strokeRect"](
          value80 - item["w"] / 2,
          value81 - item["h"] / 2,
          item["w"],
          item["h"],
        ));
    }
    const value76 = groundYOffset,
      value77 = flipY(value73) + extra;
    (value71["lineStyle"](2, 65535, 0.8),
      value71["strokeRect"](
        value76 - num32,
        value77 - num33,
        baseUnitAlias,
        baseUnitAlias,
      ),
      value71["lineStyle"](2, 16776960, 0.8),
      value71["strokeRect"](
        value76 - num32 + 5,
        value77 - num33,
        50,
        baseUnitAlias,
      ),
      value71["lineStyle"](2, 16711680, 0.8),
      value71["strokeRect"](
        value76 - num32,
        value77 - num33 + 5,
        baseUnitAlias,
        50,
      ));
    let value78 = flipY(value73 - num33 + value74) + extra,
      value79 = flipY(value73 + num33 - value74) + extra;
    (value71["lineStyle"](2, 16746496, 0.9),
      value71["lineBetween"](
        value76 - num32 - 8,
        value78,
        value76 + num32 + 8,
        value78,
      ),
      value71["lineBetween"](
        value76 - num32 - 8,
        value79,
        value76 + num32 + 8,
        value79,
      ),
      value71["lineStyle"](2, 16777215, 1),
      value71["strokeRect"](value76 - 9, value77 - 9, 36, 18));
  }
  ["setShowHitboxes"](value71) {
    this["_showHitboxes"] = value71;
  }
  ["playEndAnimation"](value71, other, extra) {
    this["_endAnimating"] = !0;
    const scene = this["_scene"],
      value72 = extra || 240,
      playerWorldX = scene["_playerWorldX"],
      value73 = this["p"]["y"],
      value74 = value71 + 100,
      value75 = value72 - 40,
      value76 = playerWorldX,
      value77 = value73,
      value78 = playerWorldX + 80,
      value79 = value72 + 300,
      value80 = [
        this["_playerSpriteLayer"],
        this["_playerGlowLayer"],
        this["_playerOverlayLayer"],
        this["_playerExtraLayer"],
        this["_shipSpriteLayer"],
        this["_shipGlowLayer"],
        this["_shipOverlayLayer"],
        this["_shipExtraLayer"],
      ]
        ["filter"]((item) => item && item["sprite"]["visible"])
        ["map"]((item) => item["sprite"]);
    (this["_particleEmitter"]["stop"](),
      this["_flyParticleEmitter"]["stop"](),
      this["_flyParticle2Emitter"]["stop"](),
      this["_shipDragEmitter"]["stop"]());
    const isFlying = this["p"]["isFlying"],
      table4 = [
        this["_shipSpriteLayer"],
        this["_shipGlowLayer"],
        this["_shipOverlayLayer"],
        this["_shipExtraLayer"],
      ],
      table5 = [
        this["_playerSpriteLayer"],
        this["_playerGlowLayer"],
        this["_playerOverlayLayer"],
        this["_playerExtraLayer"],
      ],
      value81 = value80["map"]((item) => {
        let num32 = 0;
        if (isFlying) {
          const value82 = table4["some"](
              (item2) => item2 && item2["sprite"] === item,
            ),
            value83 = table5["some"](
              (item2) => item2 && item2["sprite"] === item,
            );
          value82 ? (num32 = 10) : value83 && (num32 = -10);
        }
        return {
          spr: item,
          localY: num32,
        };
      }),
      streak = this["_streak"],
      options10 = {
        val: 0,
      };
    scene["tweens"]["add"]({
      targets: options10,
      val: 1,
      duration: 1e3,
      ease: (value82) => Math["pow"](value82, 1.2),
      onUpdate: () => {
        const val = options10["val"],
          value82 =
            (1 - val) ** 3 * value76 +
            3 * (1 - val) ** 2 * val * value76 +
            3 * (1 - val) * val ** 2 * value78 +
            val ** 3 * value74,
          value83 =
            (1 - val) ** 3 * value77 +
            3 * (1 - val) ** 2 * val * value77 +
            3 * (1 - val) * val ** 2 * value79 +
            val ** 3 * value75,
          value84 = value82 - scene["_cameraX"],
          value85 = flipY(value83) + scene["_cameraY"],
          value86 = 1 - val * val,
          rotation = value81[0]["spr"]["rotation"],
          value87 = Math["cos"](rotation),
          value88 = Math["sin"](rotation);
        for (const item of value81) {
          const value89 = -item["localY"] * value88,
            value90 = item["localY"] * value87;
          (item["spr"]["setPosition"](value84 + value89, value85 + value90),
            item["spr"]["setAlpha"](value86));
        }
        (streak["setPosition"](value82, flipY(value83)),
          streak["update"](scene["game"]["loop"]["delta"] / 1e3));
      },
      onComplete: () => {
        for (const item of value81) item["spr"]["setVisible"](!1);
        (streak["stop"](), streak["reset"](), other());
      },
    });
    for (const item of value80)
      scene["tweens"]["add"]({
        targets: item,
        angle: item["angle"] + 360,
        duration: 1e3,
        ease: (value82) => Math["pow"](value82, 1.5),
      });
  }
  ["reset"]() {
    (this["_cleanupExplosion"](),
      (this["_endAnimating"] = !1),
      (this["_lastLandObject"] = null),
      (this["_lastXOffset"] = 0),
      this["stopRotation"](),
      (this["rotateActionTime"] = 0),
      (this["_rotation"] = 0),
      (this["_lastCameraX"] = 0),
      (this["_lastCameraY"] = 0),
      this["setCubeVisible"](!0),
      this["setShipVisible"](!1));
    for (const item of this["_allLayers"])
      item && item["sprite"]["setAlpha"](1);
    for (const item of this["_playerLayers"])
      item && item["sprite"]["setScale"](1);
    (this["_particleEmitter"]["stop"](),
      (this["_particleActive"] = !1),
      this["_flyParticleEmitter"]["stop"](),
      (this["_flyParticleActive"] = !1),
      this["_flyParticle2Emitter"]["stop"](),
      (this["_flyParticle2Active"] = !1),
      this["_shipDragEmitter"]["stop"](),
      (this["_shipDragActive"] = !1),
      this["_streak"]["stop"](),
      this["_streak"]["reset"]());
  }
}
const num30 = 1e3,
  num31 = 1001;
