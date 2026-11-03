#include "cp5/CanvasWorld.h"

#include <cmath>

#include "Const.h"

void CanvasWorld::begin(cp5::Context& frameTarget, double camX, double camY, double frameZoom) {
    base = &frameTarget;
    cameraX = camX;
    cameraY = camY;
    zoom = frameZoom;
    composites.clear();
}

cp5::Context& CanvasWorld::target() {
    return composites.empty() ? *base : *composites.back().canvas;
}

void CanvasWorld::beginComposite(double alpha, double x, double y, double w, double h) {
    size_t depth = composites.size();
    if (depth >= scratchPool.size()) scratchPool.push_back(std::make_unique<cp5::Context>(true));
    cp5::Context& canvas = *scratchPool[depth];
    canvas.setSize(cp5::canvasWidth(), cp5::canvasHeight());
    canvas.clearRegion(x, y, w, h);
    canvas.save();
    canvas.setTransform(1, 0, 0, 1, 0, 0);
    canvas.translate(cp5::canvasWidth() / 2.0, cp5::canvasHeight() / 2.0);
    canvas.scale(zoom, zoom);
    canvas.translate(-cameraX, -cameraY);
    composites.push_back({&canvas, alpha, x, y, w, h});
}

void CanvasWorld::endComposite() {
    if (composites.empty()) return;
    Composite done = composites.back();
    composites.pop_back();
    done.canvas->restore();
    cp5::Context& ctx = target();
    ctx.save();
    ctx.resetTransform();
    ctx.scaleAlpha(done.alpha);
    ctx.drawContext(*done.canvas, done.x, done.y, done.w, done.h, done.x, done.y, done.w, done.h);
    ctx.restore();
}

void CanvasWorld::shape(int sides, uint32_t physicsFlags, uint32_t styleFlags, double size, double width,
                        double borderWidth, double x, double y, double angle, uint32_t fill, uint32_t stroke,
                        double alpha) {
    cp5::Context& ctx = target();
    ctx.save();
    ctx.scaleAlpha(alpha);
    ctx.translate(x, y);

    if (sides == 1) {
        ctx.beginPath();
        ctx.arc(0, 0, size, 0, 2 * M_PI);
        ctx.closePath();
        ctx.setFillColor(fill);
        ctx.fill();
        if (borderWidth > 0) {
            ctx.setStrokeColor(stroke);
            ctx.setLineWidth(borderWidth);
            ctx.stroke();
        }
        ctx.restore();
        return;
    }

    ctx.rotate(angle);
    ctx.beginPath();
    if (sides == 2) {
        if (physicsFlags & PhysicsFlags::isTrapezoid) {
            ctx.moveTo(-size * 0.5, -width * 0.5);
            ctx.lineTo(size * 0.5, -width * 0.875);
            ctx.lineTo(size * 0.5, width * 0.875);
            ctx.lineTo(-size * 0.5, width * 0.5);
            ctx.closePath();
        } else {
            ctx.rect(-size * 0.5, -width * 0.5, size, width);
        }
    } else if (sides >= 3) {
        bool star = styleFlags & StyleFlags::isStar;
        int count = sides << (star ? 1 : 0);
        double radius = size * 1.414214;
        double offset = count == 4 ? M_PI / 4 : 0;
        for (int i = 0; i < count; ++i) {
            double r = (star && !(i & 1)) ? radius * 0.4 : radius;
            double a = offset + i * 2 * M_PI / count;
            if (i == 0) ctx.moveTo(std::cos(a) * r, std::sin(a) * r);
            else ctx.lineTo(std::cos(a) * r, std::sin(a) * r);
        }
        ctx.closePath();
    } else {
        ctx.restore();
        return;
    }

    ctx.setFillColor(fill);
    ctx.fill();
    if (borderWidth > 0) {
        ctx.setLineJoin(cp5::LineJoin::round);
        ctx.setStrokeColor(stroke);
        ctx.setLineWidth(borderWidth);
        ctx.stroke();
    }
    ctx.restore();
}

void CanvasWorld::solidBackground(uint32_t rgb) {
    cp5::Context& ctx = target();
    ctx.save();
    ctx.resetTransform();
    ctx.setAlpha(1);
    ctx.setFillColor(rgb);
    ctx.fillRect(0, 0, cp5::canvasWidth(), cp5::canvasHeight());
    ctx.restore();
}

void CanvasWorld::patternBackground(cp5::Context& tile, bool, double scale, double originX, double originY) {
    cp5::Context& ctx = target();
    ctx.save();
    ctx.setAlpha(1);
    ctx.setTransform(scale, 0, 0, scale, 0, 0);
    ctx.translate(originX, originY);
    ctx.fillPattern(tile, 0, 0, cp5::canvasWidth() / scale + tile.width(), cp5::canvasHeight() / scale + tile.height());
    ctx.restore();
}

void CanvasWorld::screenRect(double x, double y, double w, double h, uint32_t rgb, double alpha) {
    cp5::Context& ctx = target();
    ctx.save();
    ctx.resetTransform();
    ctx.setAlpha(alpha);
    ctx.setFillColor(rgb);
    ctx.fillRect(x, y, w, h);
    ctx.restore();
}

void CanvasWorld::image(cp5::Context& canvas, bool, const double corners[8], double alpha) {
    double ux = corners[2] - corners[0], uy = corners[3] - corners[1];
    double vx = corners[4] - corners[0], vy = corners[5] - corners[1];
    cp5::Context& ctx = target();
    ctx.save();
    ctx.scaleAlpha(alpha);
    ctx.translate(corners[0], corners[1]);
    ctx.rotate(std::atan2(uy, ux));
    ctx.scale(std::hypot(ux, uy) / canvas.width(), std::hypot(vx, vy) / canvas.height());
    ctx.drawContext(canvas, 0, 0);
    ctx.restore();
}
