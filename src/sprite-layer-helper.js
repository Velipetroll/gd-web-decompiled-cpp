function createLayeredSprite(scene, x, y, frameName, depth, visible) {
  let helperFn42 = findAtlasFrame(scene, frameName);
  if (!helperFn42) return null;
  let image = scene["add"]["image"](
    x,
    y,
    helperFn42["atlas"],
    helperFn42["frame"],
  );
  return (
    image["setDepth"](depth),
    image["setVisible"](visible),
    {
      sprite: image,
    }
  );
}
