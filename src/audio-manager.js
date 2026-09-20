class AudioManager {
  constructor(value71) {
    ((this["_scene"] = value71),
      (this["_music"] = null),
      (this["_userMusicVol"] =
        value71["game"]["registry"]["get"]("userMusicVol") ?? 1),
      (this["_meteringEnabled"] = !1),
      (this["_analyser"] = null),
      (this["_meterBuffer"] = null),
      (this["_meterValue"] = 0.1),
      (this["_lastAudio"] = 0.1),
      (this["_lastPeak"] = 0),
      (this["_silenceCounter"] = 0));
  }
  ["_effectiveVolume"]() {
    return 0.8 * this["_userMusicVol"];
  }
  ["startMusic"]() {
    (this["_music"] && (this["_music"]["stop"](), this["_music"]["destroy"]()),
      (this["_music"] = this["_scene"]["sound"]["add"]("stereo_madness", {
        loop: !0,
        volume: this["_effectiveVolume"](),
      })),
      this["_music"]["play"](),
      this["_setupAnalyser"]());
  }
  ["stopMusic"]() {
    this["_music"] && this["_music"]["stop"]();
  }
  ["pauseMusic"]() {
    this["_music"] && this["_music"]["isPlaying"] && this["_music"]["pause"]();
  }
  ["resumeMusic"]() {
    this["_music"] && this["_music"]["isPaused"] && this["_music"]["resume"]();
  }
  ["getUserMusicVolume"]() {
    return this["_userMusicVol"];
  }
  ["setUserMusicVolume"](value71) {
    ((this["_userMusicVol"] = value71),
      this["_scene"]["game"]["registry"]["set"]("userMusicVol", value71),
      this["_music"] &&
        (this["_music"]["volume"] = this["_effectiveVolume"]()));
  }
  ["getMusicVolume"]() {
    return this["_effectiveVolume"]();
  }
  ["setMusicVolume"](value71) {
    this["setUserMusicVolume"](value71 / 0.8);
  }
  ["fadeInMusic"](value71 = 1e3) {
    (this["_music"] && (this["_music"]["stop"](), this["_music"]["destroy"]()),
      (this["_music"] = this["_scene"]["sound"]["add"]("stereo_madness", {
        loop: !0,
        volume: 0,
      })),
      this["_music"]["play"](),
      this["_setupAnalyser"](),
      this["_scene"]["tweens"]["add"]({
        targets: this["_music"],
        volume: this["_effectiveVolume"](),
        duration: value71,
      }));
  }
  ["fadeOutMusic"](value71 = 1500) {
    this["_music"] &&
      this["_music"]["isPlaying"] &&
      (this["_music"]["setLoop"](!1),
      this["_scene"]["tweens"]["add"]({
        targets: this["_music"],
        volume: 0,
        duration: value71,
        onComplete: () => {
          this["_music"] && this["_music"]["stop"]();
        },
      }));
  }
  ["playEffect"](value71, value72 = {}) {
    const value73 =
      void 0 !== this["_scene"]["_sfxVolume"]
        ? this["_scene"]["_sfxVolume"]
        : 1;
    ((value72["volume"] = (value72["volume"] || 1) * value73),
      this["_scene"]["sound"]["play"](value71, value72));
  }
  ["_setupAnalyser"]() {
    const context = this["_scene"]["sound"]["context"];
    context &&
      ((this["_analyser"] = context["createAnalyser"]()),
      (this["_analyser"]["fftSize"] = 2048),
      (this["_meterBuffer"] = new Float32Array(this["_analyser"]["fftSize"])),
      this["_scene"]["sound"]["masterVolumeNode"]["connect"](this["_analyser"]),
      (this["_meteringEnabled"] = !0));
  }
  ["update"](value71) {
    if (!this["_meteringEnabled"] || !this["_analyser"]) return;
    this["_analyser"]["getFloatTimeDomainData"](this["_meterBuffer"]);
    let num32 = 0;
    for (let i = 0; i < this["_meterBuffer"]["length"]; i++) {
      let value74 = Math["abs"](this["_meterBuffer"][i]);
      value74 > num32 && (num32 = value74);
    }
    const value72 = this["_effectiveVolume"]();
    (value72 > 0 && (num32 /= value72), (this["_meterValue"] = 0.1 + num32));
    const value73 = 60 * value71;
    (this["_silenceCounter"] < 3 ||
    this["_meterValue"] < 1.1 * this["_lastAudio"] ||
    (this["_meterValue"] < 0.95 * this["_lastPeak"] &&
      this["_lastAudio"] > 0.2 * this["_lastPeak"])
      ? (this["_meterValue"] = this["_lastAudio"] * Math["pow"](0.92, value73))
      : ((this["_silenceCounter"] = 0),
        (this["_lastPeak"] = this["_meterValue"]),
        (this["_meterValue"] *= Math["pow"](1.46, value73))),
      this["_meterValue"] <= 0.1 && (this["_lastPeak"] = 0),
      (this["_lastAudio"] = this["_meterValue"]),
      this["_silenceCounter"]++);
  }
  ["getMeteringValue"]() {
    return this["_meterValue"];
  }
  ["reset"]() {
    ((this["_meterValue"] = 0.1),
      (this["_lastAudio"] = 0.1),
      (this["_lastPeak"] = 0),
      (this["_silenceCounter"] = 0),
      this["stopMusic"]());
  }
}
