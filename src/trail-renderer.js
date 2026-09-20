class TrailRenderer {
  constructor(
    value71,
    other,
    extra,
    extra2,
    extra3,
    extra4,
    value72 = 16777215,
    value73 = 1,
  ) {
    ((this["_color"] = value72),
      (this["_opacity"] = value73),
      (this["_fadeDelta"] = 1 / extra),
      (this["_minSegSq"] = extra2 * extra2),
      (this["_maxSeg"] = extra4),
      (this["_maxPoints"] = 5 * Math["floor"](60 * extra + 2)),
      (this["_stroke"] = extra3),
      (this["_pts"] = []),
      (this["_posR"] = {
        x: 0,
        y: 0,
      }),
      (this["_posInit"] = !1),
      (this["_active"] = !1),
      (this["_gfx"] = value71["add"]["graphics"]()),
      this["_gfx"]["setBlendMode"](Phaser["BlendModes"]["ADD"]));
  }
  ["addToContainer"](value71, other) {
    (value71["add"](this["_gfx"]), this["_gfx"]["setDepth"](other));
  }
  ["setPosition"](value71, other) {
    ((this["_posR"]["x"] = value71),
      (this["_posR"]["y"] = other),
      (this["_posInit"] = !0));
  }
  ["start"]() {
    this["_active"] = !0;
  }
  ["stop"]() {
    this["_active"] = !1;
  }
  ["reset"]() {
    ((this["_pts"] = []), (this["_posInit"] = !1), this["_gfx"]["clear"]());
  }
  ["update"](value71) {
    if (!this["_posInit"]) return void this["_gfx"]["clear"]();
    const value72 = value71 * this["_fadeDelta"];
    let num32 = 0;
    for (let i = 0; i < this["_pts"]["length"]; i++)
      ((this["_pts"][i]["state"] -= value72),
        this["_pts"][i]["state"] > 0 &&
          (num32 !== i && (this["_pts"][num32] = this["_pts"][i]), num32++));
    if (
      ((this["_pts"]["length"] = num32),
      this["_active"] && this["_pts"]["length"] < this["_maxPoints"])
    ) {
      const length2 = this["_pts"]["length"];
      let value73 = !0;
      if (length2 > 0) {
        const point = this["_pts"][length2 - 1],
          value74 = this["_posR"]["x"] - point["x"],
          value75 = this["_posR"]["y"] - point["y"],
          value76 = value74 * value74 + value75 * value75;
        if (this["_maxSeg"] > 0 && Math["sqrt"](value76) > this["_maxSeg"])
          this["_pts"]["length"] = 0;
        else if (value76 < this["_minSegSq"]) value73 = !1;
        else if (length2 > 1) {
          const point2 = this["_pts"][length2 - 2],
            value77 = this["_posR"]["x"] - point2["x"],
            value78 = this["_posR"]["y"] - point2["y"];
          value77 * value77 + value78 * value78 < 2 * this["_minSegSq"] &&
            (value73 = !1);
        }
      }
      value73 &&
        this["_pts"]["push"]({
          x: this["_posR"]["x"],
          y: this["_posR"]["y"],
          state: 1,
        });
    }
    this["_gfx"]["clear"]();
    const length = this["_pts"]["length"];
    if (!(length < 2))
      for (let i = 0; i < length - 1; i++) {
        const i2 = this["_pts"][i],
          point = this["_pts"][i + 1],
          value73 = 0.5 * (i2["state"] + point["state"]) * this["_opacity"];
        (this["_gfx"]["lineStyle"](this["_stroke"], this["_color"], value73),
          this["_gfx"]["lineBetween"](
            i2["x"],
            i2["y"],
            point["x"],
            point["y"],
          ));
      }
  }
}
