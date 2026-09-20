function drawExpandingRing(
  helperFn21,
  helperFn22,
  helperFn23,
  helperFn24,
  helperFn25,
  helperFn26,
  value71 = !1,
  value72 = !1,
  value73 = 16777215,
) {
  const graphics = helperFn21["add"]
      ["graphics"]()
      ["setScrollFactor"](0)
      ["setDepth"](55)
      ["setBlendMode"](blendAdd),
    options10 = {
      r: helperFn24,
      t: 0,
    };
  helperFn21["tweens"]["add"]({
    targets: options10,
    r: helperFn25,
    t: 1,
    duration: helperFn26,
    ease: value71 && !value72 ? "Quad.Out" : "Linear",
    onUpdate: () => {
      const value74 = options10["t"],
        value75 = value72
          ? value74 < 0.5
            ? 2 * value74
            : 2 * (1 - value74)
          : 1 - value74;
      (graphics["clear"](),
        value71
          ? (graphics["fillStyle"](value73, Math["max"](0, value75)),
            graphics["fillCircle"](helperFn22, helperFn23, options10["r"]))
          : (graphics["lineStyle"](4, value73, Math["max"](0, value75)),
            graphics["strokeCircle"](helperFn22, helperFn23, options10["r"])));
    },
    onComplete: () => graphics["destroy"](),
  });
}
function spawnFinishParticles(
  helperFn21,
  value71 = 16777215,
  value72 = 16777215,
) {
  const num32 = 200,
    value73 = num32 + (screenWidth - 400) * Math["random"](),
    value74 = num32 + 240 * Math["random"]();
  (drawExpandingRing(
    helperFn21,
    value73,
    value74,
    40,
    140 + 60 * Math["random"](),
    500,
    !0,
    !0,
    value72,
  ),
    helperFn21["add"]
      ["particles"](value73, value74, "GJ_WebSheet", {
        frame: "square.png",
        speed: {
          min: 520,
          max: 920,
        },
        angle: {
          min: 0,
          max: 360,
        },
        scale: {
          start: 0.4,
          end: 0.13,
        },
        alpha: {
          start: 1,
          end: 0,
        },
        lifespan: {
          min: 0,
          max: 500,
        },
        stopAfter: 25,
        blendMode: blendAdd,
        tint: value71,
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
      ["setDepth"](57));
}
