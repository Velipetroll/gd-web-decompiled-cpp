class TweenValue {
  constructor(value71, other, extra) {
    ((this["from"] = {
      ...value71,
    }),
      (this["to"] = {
        ...other,
      }),
      (this["duration"] = extra),
      (this["elapsed"] = 0),
      (this["done"] = extra <= 0),
      (this["current"] =
        extra <= 0
          ? {
              ...other,
            }
          : {
              ...value71,
            }));
  }
  ["step"](value71) {
    if (this["done"]) return;
    this["elapsed"] += value71;
    let value72 =
      this["duration"] > 0
        ? Math["min"](this["elapsed"] / this["duration"], 1)
        : 1;
    value72 >= 1
      ? ((this["current"] = {
          ...this["to"],
        }),
        (this["done"] = !0))
      : (this["current"] = {
          r: Math["round"](
            this["from"]["r"] + (this["to"]["r"] - this["from"]["r"]) * value72,
          ),
          g: Math["round"](
            this["from"]["g"] + (this["to"]["g"] - this["from"]["g"]) * value72,
          ),
          b: Math["round"](
            this["from"]["b"] + (this["to"]["b"] - this["from"]["b"]) * value72,
          ),
        });
  }
}
