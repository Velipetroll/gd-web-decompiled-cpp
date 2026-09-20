const table = ["GJ_WebSheet"];
function findAtlasFrame(scene, frameName) {
  for (let item of table)
    if (scene["textures"]["exists"](item))
      if (scene["textures"]["get"](item)["has"](frameName))
        return {
          atlas: item,
          frame: frameName,
        };
  return null;
}
function addImageFromAtlas(scene, x, y, frameName) {
  let helperFn42 = findAtlasFrame(scene, frameName);
  return helperFn42
    ? scene["add"]["image"](x, y, helperFn42["atlas"], helperFn42["frame"])
    : scene["textures"]["exists"](frameName)
      ? scene["add"]["image"](x, y, frameName)
      : null;
}
class LevelObject {
  constructor(value71, other, extra, extra2, extra3) {
    ((this["type"] = value71),
      (this["x"] = other),
      (this["y"] = extra),
      (this["w"] = extra2),
      (this["h"] = extra3),
      (this["activated"] = !1));
  }
}
function zeroArray(array) {
  let length = array["length"];
  for (; --length >= 0;) array[length] = 0;
}
const num11 = 256,
  k = 286,
  num12 = 30,
  num13 = 15,
  Uint8Array2 = new Uint8Array([
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
    5, 5, 5, 0,
  ]),
  Uint8Array3 = new Uint8Array([
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
    11, 11, 12, 12, 13, 13,
  ]),
  Uint8Array4 = new Uint8Array([
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7,
  ]),
  Uint8Array5 = new Uint8Array([
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15,
  ]),
  Array2 = new Array(576);
zeroArray(Array2);
const Array3 = new Array(60);
zeroArray(Array3);
const Array4 = new Array(512);
zeroArray(Array4);
const Array5 = new Array(256);
zeroArray(Array5);
const Array6 = new Array(29);
zeroArray(Array6);
const j = new Array(num12);
