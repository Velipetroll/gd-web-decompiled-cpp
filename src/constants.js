let screenWidth = Math["round"](10240 / 9);
const screenHeight = 640,
  baseUnit = 60,
  unusedConst180 = 180;
let groundYOffset = screenWidth / 2 - 150;
function setScreenWidth(newWidth) {
  ((screenWidth = newWidth), (groundYOffset = newWidth / 2 - 150));
}
const fixedTimeStep = 1 / 240,
  gravityConst = 11.540004,
  physicsConst09 = 0.9,
  physicsConst1916 = 1.916398,
  physicsConst600 = 600,
  baseUnitAlias = baseUnit,
  colorGreenTint = 65280,
  colorCyanTint = 65535,
  solid = "solid",
  hazard = "hazard",
  portalFly = "portal_fly",
  portalCube = "portal_cube",
  yFlipBase = 460;
function flipY(yValue) {
  return yFlipBase - yValue;
}
let blendAdd = Phaser["BlendModes"]["ADD"],
  blendNormal = Phaser["BlendModes"]["NORMAL"];
