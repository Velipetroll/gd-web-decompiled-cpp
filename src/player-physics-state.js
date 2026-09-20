class PlayerPhysicsState {
  constructor() {
    this["reset"]();
  }
  ["reset"]() {
    ((this["y"] = 30),
      (this["lastY"] = 30),
      (this["lastGroundPosY"] = 30),
      (this["yVelocity"] = 0),
      (this["onGround"] = !0),
      (this["canJump"] = !0),
      (this["isJumping"] = !1),
      (this["gravityFlipped"] = !1),
      (this["isFlying"] = !1),
      (this["wasBoosted"] = !1),
      (this["collideTop"] = 0),
      (this["collideBottom"] = 0),
      (this["onCeiling"] = !1),
      (this["upKeyDown"] = !1),
      (this["upKeyPressed"] = !1),
      (this["isDead"] = !1));
  }
}
