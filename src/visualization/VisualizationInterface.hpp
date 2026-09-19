#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

// VisualizationInterface: Interface for all visualization systems
// Provides visualization of neural activity, agent behavior, and world state
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize visualization system
    virtual bool initialize() = 0;
    
    // Update visualization with current state
    virtual void update(float timestep) = 0;
    
    // Render current scene
    virtual void render() = 0;
    
    // Start recording visualization
    virtual void startRecording(const std::string& filepath) = 0;
    
    // Stop recording visualization
    virtual void stopRecording() = 0;
    
    // Toggle pause state
    virtual void togglePause() = 0;
    
    // Check if visualization is paused
    virtual bool isPaused() const = 0;
    
    // Get visualization FPS
    virtual float getFPS() const = 0;
    
    // Enable/disable specific visualization components
    virtual void enableVision(bool enable) = 0;
    virtual void enableWorld(bool enable) = 0;
    virtual void enableBrain(bool enable) = 0;
    virtual void enableAgent(bool enable) = 0;
    
    // Check if visualization components are enabled
    virtual bool isVisionEnabled() const = 0;
    virtual bool isWorldEnabled() const = 0;
    virtual bool isBrainEnabled() const = 0;
    virtual bool isAgentEnabled() const = 0;
};

} // namespace nlm