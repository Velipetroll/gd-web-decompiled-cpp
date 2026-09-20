class ColorManager {
  constructor() {
    this["reset"]();
  }
  ["reset"]() {
    ((this["_colors"] = {
      [num30]: {
        r: 0,
        g: 102,
        b: 255,
      },
      [num31]: {
        r: 0,
        g: 68,
        b: 170,
      },
    }),
      (this["_actions"] = {}));
  }
  ["triggerColor"](value71, other, extra) {
    let options10 = {
      ...this["getColor"](value71),
    };
    ((this["_actions"][value71] = new TweenValue(options10, other, extra)),
      extra <= 0 &&
        (this["_colors"][value71] = {
          ...other,
        }));
  }
  ["step"](value71) {
    for (let item in this["_actions"]) {
      let item2 = this["_actions"][item];
      (item2["step"](value71),
        (this["_colors"][item] = {
          ...item2["current"],
        }),
        item2["done"] && delete this["_actions"][item]);
    }
  }
  ["getColor"](value71) {
    return (
      this["_colors"][value71] || {
        r: 255,
        g: 255,
        b: 255,
      }
    );
  }
  ["getHex"](value71) {
    let value72 = this["getColor"](value71);
    return (value72["r"] << 16) | (value72["g"] << 8) | value72["b"];
  }
}
