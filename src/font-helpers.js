function defineFontFromFnt(scene, fontKey, fntText) {
  const value71 = scene["textures"]["get"](fontKey),
    value72 = value71["source"][0],
    width = value72["width"],
    height = value72["height"],
    options10 = {
      font: fontKey,
      size: 0,
      lineHeight: 0,
      chars: {},
    },
    table4 = [];
  for (const item of fntText["split"]("\n")) {
    const value73 = item["trim"]()["split"](/\s+/);
    if (!value73["length"]) continue;
    const value74 = value73[0],
      options11 = {};
    for (let i = 1; i < value73["length"]; i++) {
      const value75 = value73[i]["indexOf"]("=");
      value75 >= 0 &&
        (options11[value73[i]["slice"](0, value75)] = value73[i]["slice"](
          value75 + 1,
        )["replace"](/^"|"$/g, ""));
    }
    if ("info" === value74) options10["size"] = parseInt(options11["size"], 10);
    else if ("common" === value74)
      options10["lineHeight"] = parseInt(options11["lineHeight"], 10);
    else if ("char" === value74) {
      const parseInt2 = parseInt(options11["id"], 10),
        parseInt3 = parseInt(options11["x"], 10),
        parseInt4 = parseInt(options11["y"], 10),
        parseInt5 = parseInt(options11["width"], 10),
        parseInt6 = parseInt(options11["height"], 10),
        value75 = parseInt3 / width,
        value76 = parseInt4 / height,
        value77 = (parseInt3 + parseInt5) / width,
        value78 = (parseInt4 + parseInt6) / height;
      if (
        ((options10["chars"][parseInt2] = {
          x: parseInt3,
          y: parseInt4,
          width: parseInt5,
          height: parseInt6,
          centerX: Math["floor"](parseInt5 / 2),
          centerY: Math["floor"](parseInt6 / 2),
          xOffset: parseInt(options11["xoffset"], 10),
          yOffset: parseInt(options11["yoffset"], 10),
          xAdvance: parseInt(options11["xadvance"], 10),
          data: {},
          kerning: {},
          u0: value75,
          v0: value76,
          u1: value77,
          v1: value78,
        }),
        0 !== parseInt5 && 0 !== parseInt6)
      ) {
        const value79 = String["fromCharCode"](parseInt2),
          value80 = value71["add"](
            value79,
            0,
            parseInt3,
            parseInt4,
            parseInt5,
            parseInt6,
          );
        value80 &&
          value80["setUVs"](
            parseInt5,
            parseInt6,
            value75,
            value76,
            value77,
            value78,
          );
      }
    } else
      "kerning" === value74 &&
        table4["push"]({
          first: parseInt(options11["first"], 10),
          second: parseInt(options11["second"], 10),
          amount: parseInt(options11["amount"], 10),
        });
  }
  for (const item of table4)
    options10["chars"][item["second"]] &&
      (options10["chars"][item["second"]]["kerning"][item["first"]] =
        item["amount"]);
  scene["cache"]["bitmapFont"]["add"](fontKey, {
    data: options10,
    texture: fontKey,
    frame: null,
  });
}
