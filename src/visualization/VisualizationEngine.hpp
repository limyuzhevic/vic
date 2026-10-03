#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../core/Types/Types.hpp"

namespace nlm {

class VisualizationBackend;
class VisualizationActor;
class VisualizationLight;
class VisualizationCamera;

// Visualization engine interface
class VisualizationEngine {
public:
    virtual ~VisualizationEngine() = default;
    
    // Initialize the engine with a backend
    virtual bool initialize(std::unique_ptr<VisualizationBackend> backend) = 0;
    
    // Shutdown the engine
    virtual void shutdown() = 0;
    
    // Update engine state
    virtual void update(double deltaTime) = 0;
    
    // Render the current frame
    virtual void render() = 0;
    
    // Get the backend
    virtual VisualizationBackend* getBackend() const = 0;
    
    // Set the backend
    virtual void setBackend(std::unique_ptr<VisualizationBackend> backend) = 0;
    
    // Create a visualization actor
    virtual VisualizationActor* createActor(const std::string& type) = 0;
    
    // Remove a visualization actor
    virtual void removeActor(VisualizationActor* actor) = 0;
    
    // Get all actors
    virtual const std::vector<VisualizationActor*>& getActors() const = 0;
    
    // Create a visualization light
    virtual VisualizationLight* createLight() = 0;
    
    // Remove a visualization light
    virtual void removeLight(VisualizationLight* light) = 0;
    
    // Get all lights
    virtual const std::vector<VisualizationLight*>& getLights() const = 0;
    
    // Create a visualization camera
    virtual VisualizationCamera* createCamera() = 0;
    
    // Remove a visualization camera
    virtual void removeCamera(VisualizationCamera* camera) = 0;
    
    // Get all cameras
    virtual const std::vector<VisualizationCamera*>& getCameras() const = 0;
    
    // Enable/disable debug rendering
    virtual void enableDebugRendering(bool enabled) = 0;
    
    // Get debug rendering status
    virtual bool isDebugRenderingEnabled() const = 0;
    
    // Enable/disable wireframe mode
    virtual void enableWireframeMode(bool enabled) = 0;
    
    // Get wireframe mode status
    virtual bool isWireframeModeEnabled() const = 0;
    
    // Set clear color
    virtual void setClearColor(float r, float g, float b, float a = 1.0f) = 0;
    
    // Get clear color
    virtual std::array<float, 4> getClearColor() const = 0;
    
    // Enable/disable vertical sync
    virtual void enableVerticalSync(bool enabled) = 0;
    
    // Get vertical sync status
    virtual bool isVerticalSyncEnabled() const = 0;
    
    // Set target frame rate
    virtual void setTargetFrameRate(double fps) = 0;
    
    // Get target frame rate
    virtual double getTargetFrameRate() const = 0;
    
    // Capture a screenshot
    virtual bool captureScreenshot(const std::string& filepath) = 0;
    
    // Start recording
    virtual void startRecording(const std::string& filepath) = 0;
    
    // Stop recording
    virtual void stopRecording() = 0;
    
    // Check if recording
    virtual bool isRecording() const = 0;
};

} // namespace nlm