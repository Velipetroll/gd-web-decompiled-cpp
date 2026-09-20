class BootScene extends Phaser["Scene"] {
  constructor() {
    super({
      key: "BootScene",
    });
  }
  ["preload"]() {
    !(function (value72) {
      if (value72["renderer"]["type"] === Phaser["WEBGL"]) {
        let value73 = value72["renderer"]["gl"];
        ((blendAdd = value72["renderer"]["addBlendMode"](
          [value73["SRC_ALPHA"], value73["ONE"]],
          value73["FUNC_ADD"],
        )),
          (blendNormal = value72["renderer"]["addBlendMode"](
            [value73["DST_COLOR"], value73["ONE_MINUS_SRC_ALPHA"]],
            value73["FUNC_ADD"],
          )));
      }
    })(this["game"]);
    let width = this["cameras"]["main"]["width"],
      height = this["cameras"]["main"]["height"],
      value71 = 0.6 * width,
      rectangle = this["add"]
        ["rectangle"](width / 2, height / 2, value71, 8, 65280)
        ["setOrigin"](0.5, 0.5);
    ((rectangle["scaleX"] = 0),
      this["load"]["on"]("progress", (progress) => {
        rectangle["scaleX"] = progress;
      }),
      this["load"]["on"]("loaderror", (loaderror) => {}),
      this["load"]["atlas"](
        "GJ_WebSheet",
        "assets/GJ_WebSheet.png",
        "assets/GJ_WebSheet.json",
      ),
      this["load"]["image"]("bigFont", "assets/bigFont.png"),
      this["load"]["text"]("bigFontFnt", "assets/bigFont.fnt"),
      this["load"]["image"]("goldFont", "assets/goldFont.png"),
      this["load"]["text"]("goldFontFnt", "assets/goldFont.fnt"),
      this["load"]["image"]("game_bg_01", "assets/game_bg_01_001.png"),
      this["load"]["image"]("sliderBar", "assets/sliderBar.png"),
      this["load"]["image"]("square04_001", "assets/square04_001.png"),
      this["load"]["image"]("GJ_square02", "assets/GJ_square02.png"),
      this["load"]["text"]("level_1", "assets/1.txt"),
      this["load"]["audio"]("stereo_madness", "assets/StereoMadness.mp3"),
      this["load"]["audio"]("explode_11", "assets/explode_11.ogg"),
      this["load"]["audio"]("endStart_02", "assets/endStart_02.ogg"),
      this["load"]["audio"]("playSound_01", "assets/playSound_01.ogg"),
      this["load"]["audio"]("quitSound_01", "assets/quitSound_01.ogg"),
      this["load"]["audio"]("highscoreGet02", "assets/highscoreGet02.ogg"));
  }
  ["create"]() {
    this["cache"]["text"]["get"]("level_1");
    const text = this["cache"]["text"]["get"]("bigFontFnt");
    text && defineFontFromFnt(this, "bigFont", text);
    const text2 = this["cache"]["text"]["get"]("goldFontFnt");
    (text2 && defineFontFromFnt(this, "goldFont", text2),
      this["scene"]["start"]("GameScene"));
  }
}
