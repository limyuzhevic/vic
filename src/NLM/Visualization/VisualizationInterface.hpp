#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>
#include <functional>

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;
class MemorySystem;
class PerformanceMetrics;

// Abstract base class for all visualization components
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize visualization system
    virtual bool initialize() = 0;
    
    // Update visualization data
    virtual void update() = 0;
    
    // Render current visualization state
    virtual void render() = 0;
    
    // Close visualization and cleanup resources
    virtual void close() = 0;
    
    // Check if visualization is currently active
    virtual bool isActive() const = 0;
    
    // Set visualization update rate (Hz)
    virtual void setUpdateRate(double hz) = 0;
    
    // Get current update rate
    virtual double getUpdateRate() const = 0;
    
    // Check if visualization supports 3D rendering
    virtual bool supports3D() const = 0;
    
    // Get visualization name/identifier
    virtual std::string getName() const = 0;
    
    // Get visualization version
    virtual std::string getVersion() const = 0;
    
    // Handle window resize events
    virtual void onResize(int width, int height) = 0;
    
    // Set visualization parameters
    virtual void setParameter(const std::string& name, const std::string& value) = 0;
    
    // Get visualization parameters
    virtual std::string getParameter(const std::string& name) const = 0;
    
    // Enable or disable visualization components
    virtual void setComponentEnabled(const std::string& component, bool enabled) = 0;
    
    // Check if visualization component is enabled
    virtual bool isComponentEnabled(const std::string& component) const = 0;
    
    // Add event callback for visualization interactions
    virtual void addEventCallback(const std::string& eventType, std::function<void(const std::string&)> callback) = 0;
    
    // Export current visualization state
    virtual bool exportState(const std::string& filename, const std::string& format) = 0;
    
    // Import visualization state
    virtual bool importState(const std::string& filename, const std::string& format) = 0;
};

} // namespace nlm