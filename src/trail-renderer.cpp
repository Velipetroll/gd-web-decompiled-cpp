#include "trail-renderer.h"

TrailRenderer::TrailRenderer(float lifetime, float minSeg, float stroke,
                             float maxSeg, unsigned int color, float opacity)
: _color(color),
_opacity(opacity),
_fadeDelta(lifetime > 0.0f ? 1.0f / lifetime : 1.0f),
_minSegSq(minSeg * minSeg),
_maxSeg(maxSeg),
_maxPoints(5 * (int)std::floor(60.0f * lifetime + 2.0f)),
_stroke(stroke),
_posInit(false),
_active(false)
{
}

TrailRenderer::~TrailRenderer() {}

void TrailRenderer::setPosition(float x, float y) {
    _posX = x;
    _posY = y;
    _posInit = true;
}

void TrailRenderer::start() {
    _active = true;
}

void TrailRenderer::stop() {
    _active = false;
}

void TrailRenderer::reset() {
    _pts.clear();
    _posInit = false;
}

void TrailRenderer::update(float dt) {
    if (!_posInit) {
        _pts.clear();
        return;
    }

    float delta = dt * _fadeDelta;
    size_t validCount = 0;

    for (size_t i = 0; i < _pts.size(); ++i) {
        _pts[i].state -= delta;
        if (_pts[i].state > 0.0f) {
            if (validCount != i) {
                _pts[validCount] = _pts[i];
            }
            validCount++;
        }
    }
    _pts.resize(validCount);

    if (_active && (int)_pts.size() < _maxPoints) {
        size_t len = _pts.size();
        bool shouldAdd = true;

        if (len > 0) {
            const auto& last = _pts[len - 1];
            float dx = _posX - last.x;
            float dy = _posY - last.y;
            float distSq = dx * dx + dy * dy;

            if (_maxSeg > 0.0f && std::sqrt(distSq) > _maxSeg) {
                _pts.clear();
            } else if (distSq < _minSegSq) {
                shouldAdd = false;
            } else if (len > 1) {
                const auto& secondLast = _pts[len - 2];
                float dx2 = _posX - secondLast.x;
                float dy2 = _posY - secondLast.y;
                if (dx2 * dx2 + dy2 * dy2 < 2.0f * _minSegSq) {
                    shouldAdd = false;
                }
            }
        }

        if (shouldAdd) {
            _pts.push_back({_posX, _posY, 1.0f});
        }
    }
}

void TrailRenderer::render(float cameraX, float cameraY) {
    if (_pts.size() < 2) return;

    float r = ((_color >> 16) & 0xFF) / 255.0f;
    float g = ((_color >> 8)  & 0xFF) / 255.0f;
    float b =  (_color        & 0xFF) / 255.0f;

    glPushMatrix();
    glTranslatef(-cameraX, cameraY, 0.0f);

    glDisable(GL_TEXTURE_2D);
    applyBlendMode(BLEND_ADD);
    glLineWidth(_stroke);

    glBegin(GL_LINES);
    for (size_t i = 0; i + 1 < _pts.size(); ++i) {
        const auto& p1 = _pts[i];
        const auto& p2 = _pts[i + 1];
        float alpha = 0.5f * (p1.state + p2.state) * _opacity;

        glColor4f(r, g, b, alpha);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
    }
    glEnd();

    glLineWidth(1.0f);
    applyBlendMode(BLEND_NORMAL);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}
