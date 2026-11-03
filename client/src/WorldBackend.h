#pragma once

#include <cstdint>

namespace cp5 { class Context; }

// Where the world draws: WorldRenderer on WebGL2, CanvasWorld on Canvas2D.
class WorldBackend {
public:
    virtual ~WorldBackend() = default;

    // What follows until endComposite fades as one at alpha; x/y/w/h bound it in device pixels.
    virtual void beginComposite(double alpha, double x, double y, double w, double h) = 0;
    virtual void endComposite() = 0;
    virtual void shape(int sides, uint32_t physicsFlags, uint32_t styleFlags, double size, double width,
                       double borderWidth, double x, double y, double angle, uint32_t fill, uint32_t stroke,
                       double alpha) = 0;
    // The background, drawn before anything else in the frame.
    virtual void solidBackground(uint32_t rgb) = 0;
    // The tile's canvas repeated like a Canvas2D pattern fill under setTransform(scale, 0, 0, scale, 0, 0)
    // and translate(originX, originY). changed: the tile was redrawn since the last frame.
    virtual void patternBackground(cp5::Context& tile, bool changed, double scale, double originX, double originY) = 0;
    // A Canvas2D fillRect in device pixels.
    virtual void screenRect(double x, double y, double w, double h, uint32_t rgb, double alpha) = 0;
    // A cp5 canvas over the world-space rectangle with corners top-left, top-right, bottom-left,
    // bottom-right. changed: the canvas was redrawn since it was last drawn here.
    virtual void image(cp5::Context& canvas, bool changed, const double corners[8], double alpha) = 0;
};
