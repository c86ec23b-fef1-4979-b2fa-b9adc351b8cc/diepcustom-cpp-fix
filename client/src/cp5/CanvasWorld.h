#pragma once

#include <memory>
#include <vector>

#include "WorldBackend.h"
#include "cp5/Context.h"

// The world on Canvas2D, drawn into the target under its current transform. Composites go
// through screen-sized scratch canvases under the camera transform.
class CanvasWorld : public WorldBackend {
public:
    void begin(cp5::Context& target, double cameraX, double cameraY, double zoom);
    void beginComposite(double alpha, double x, double y, double w, double h) override;
    void endComposite() override;
    void shape(int sides, uint32_t physicsFlags, uint32_t styleFlags, double size, double width,
               double borderWidth, double x, double y, double angle, uint32_t fill, uint32_t stroke,
               double alpha) override;
    void solidBackground(uint32_t rgb) override;
    void patternBackground(cp5::Context& tile, bool changed, double scale, double originX, double originY) override;
    void screenRect(double x, double y, double w, double h, uint32_t rgb, double alpha) override;
    void image(cp5::Context& canvas, bool changed, const double corners[8], double alpha) override;

private:
    struct Composite {
        cp5::Context* canvas;
        double alpha, x, y, w, h;
    };

    cp5::Context* base = nullptr;
    double cameraX = 0, cameraY = 0, zoom = 1;
    std::vector<std::unique_ptr<cp5::Context>> scratchPool;
    std::vector<Composite> composites;

    cp5::Context& target();
};
