#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationEngine;
class VisualizationScene;

// Visualization renderer interface
class VisualizationRenderer {
public:
    virtual ~VisualizationRenderer() = default;
    
    // Initialize the renderer
    virtual bool initialize(VisualizationEngine* engine) = 0;
    
    // Shutdown the renderer
    virtual void shutdown() = 0;
    
    // Update the renderer
    virtual void update(double deltaTime) = 0;
    
    // Render the scene
    virtual void render(VisualizationScene* scene) = 0;
    
    // Get the engine
    virtual VisualizationEngine* getEngine() const = 0;
    
    // Set the engine
    virtual void setEngine(VisualizationEngine* engine) = 0;
    
    // Enable/disable rendering
    virtual void enableRendering(bool enabled) = 0;
    
    // Check if rendering is enabled
    virtual bool isRenderingEnabled() const = 0;
    
    // Set render mode
    virtual void setRenderMode(const std::string& mode) = 0;
    
    // Get render mode
    virtual std::string getRenderMode() const = 0;
    
    // Enable/disable post-processing
    virtual void enablePostProcessing(bool enabled) = 0;
    
    // Check if post-processing is enabled
    virtual bool isPostProcessingEnabled() const = 0;
    
    // Add post-processing effect
    virtual void addPostProcessingEffect(const std::string& effect) = 0;
    
    // Remove post-processing effect
    virtual void removePostProcessingEffect(const std::string& effect) = 0;
    
    // Get all post-processing effects
    virtual const std::vector<std::string>& getPostProcessingEffects() const = 0;
    
    // Set render quality
    virtual void setRenderQuality(float quality) = 0;
    
    // Get render quality
    virtual float getRenderQuality() const = 0;
    
    // Enable/disable anti-aliasing
    virtual void enableAntiAliasing(bool enabled) = 0;
    
    // Check if anti-aliasing is enabled
    virtual bool isAntiAliasingEnabled() const = 0;
    
    // Set anti-aliasing quality
    virtual void setAntiAliasingQuality(int quality) = 0;
    
    // Get anti-aliasing quality
    virtual int getAntiAliasingQuality() const = 0;
    
    // Capture a frame
    virtual bool captureFrame(const std::string& filepath) = 0;
    
    // Set frame rate
    virtual void setFrameRate(double fps) = 0;
    
    // Get frame rate
    virtual double getFrameRate() const = 0;
    
    // Enable/disable vsync
    virtual void enableVerticalSync(bool enabled) = 0;
    
    // Check if vsync is enabled
    virtual bool isVerticalSyncEnabled() const = 0;
    
    // Set viewport
    virtual void setViewport(int x, int y, int width, int height) = 0;
    
    // Get viewport
    virtual std::array<int, 4> getViewport() const = 0;
    
    // Set camera for rendering
    virtual void setCamera(class VisualizationCamera* camera) = 0;
    
    // Get camera
    virtual class VisualizationCamera* getCamera() const = 0;
    
    // Set light for rendering
    virtual void setLight(class VisualizationLight* light) = 0;
    
    // Get light
    virtual class VisualizationLight* getLight() const = 0;
    
    // Set clear color
    virtual void setClearColor(float r, float g, float b, float a = 1.0f) = 0;
    
    // Get clear color
    virtual std::array<float, 4> getClearColor() const = 0;
    
    // Enable/disable depth buffer
    virtual void enableDepthBuffer(bool enabled) = 0;
    
    // Check if depth buffer is enabled
    virtual bool isDepthBufferEnabled() const = 0;
    
    // Enable/disable stencil buffer
    virtual void enableStencilBuffer(bool enabled) = 0;
    
    // Check if stencil buffer is enabled
    virtual bool isStencilBufferEnabled() const = 0;
    
    // Enable/disable scissor test
    virtual void enableScissorTest(bool enabled) = 0;
    
    // Check if scissor test is enabled
    virtual bool isScissorTestEnabled() const = 0;
};

} // namespace nlm