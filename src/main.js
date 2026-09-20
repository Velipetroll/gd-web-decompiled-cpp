const gameConfig = {
  type: Phaser["AUTO"],
  width: screenWidth,
  height: screenHeight,
  resolution: 1,
  fps: {
    smoothStep: !0,
  },
  backgroundColor: "#000000",
  parent: document["body"],
  input: {
    windowEvents: !1,
  },
  render: {
    powerPreference: "high-performance",
  },
  scale: {
    mode: Phaser["Scale"]["FIT"],
    autoCenter: Phaser["Scale"]["CENTER_BOTH"],
  },
  scene: [BootScene, GameScene],
};
new Phaser["Game"](gameConfig);
