#pragma once

#include "VisualizationBackend.hpp"
#include <string>

namespace nlm {

class OpenGLBackend : public VisualizationBackend {
public:
    OpenGLBackend();
    ~OpenGLBackend() override;
    
    bool initialize(int width, int height) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    bool captureFrame(const std::string& filepath) override;
    void setRenderQuality(float quality) override;
    void enableAntiAliasing(bool enabled) override;
    void setAntiAliasingQuality(int quality) override;
    void setFrameRate(double fps) override;
    void enableVerticalSync(bool enabled) override;
    void clearBuffers(bool depth, bool stencil) override;
    void enableScissorTest(bool enabled) override;
    void setScissor(int x, int y, int width, int height) override;
    void setViewport(int x, int y, int width, int height) override;
    void setClearColor(float r, float g, float b, float a) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm