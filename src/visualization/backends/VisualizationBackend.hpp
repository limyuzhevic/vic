#pragma once

#include <string>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationBackend {
public:
    virtual ~VisualizationBackend() = default;
    
    virtual bool initialize(int width, int height) = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual bool captureFrame(const std::string& filepath) = 0;
    virtual void setRenderQuality(float quality) = 0;
    virtual void enableAntiAliasing(bool enabled) = 0;
    virtual void setAntiAliasingQuality(int quality) = 0;
    virtual void setFrameRate(double fps) = 0;
    virtual void enableVerticalSync(bool enabled) = 0;
    virtual void clearBuffers(bool depth, bool stencil) = 0;
    virtual void enableScissorTest(bool enabled) = 0;
    virtual void setScissor(int x, int y, int width, int height) = 0;
    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void setClearColor(float r, float g, float b, float a) = 0;
};

} // namespace nlm