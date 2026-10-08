#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <functional>

namespace nlm {

class VisualizationInterface;
class Brain;
class MemorySystem;
class PerformanceMetrics;

class VisualizerManager {
public:
    VisualizerManager();
    ~VisualizerManager();
    
    // Add a new visualizer to the manager
    bool addVisualizer(std::unique_ptr<VisualizationInterface> visualizer);
    
    // Remove a visualizer by name
    bool removeVisualizer(const std::string& name);
    
    // Get a visualizer by name
    VisualizationInterface* getVisualizer(const std::string& name);
    const VisualizationInterface* getVisualizer(const std::string& name) const;
    
    // Get all active visualizers
    std::vector<VisualizationInterface*> getActiveVisualizers();
    std::vector<const VisualizationInterface*> getActiveVisualizers() const;
    
    // Initialize all visualizers
    bool initializeAll();
    
    // Update all visualizers
    void updateAll(double elapsedTime);
    
    // Render all visualizers
    void renderAll();
    
    // Close all visualizers
    void closeAll();
    
    // Check if any visualizer is active
    bool isAnyActive() const;
    
    // Set global update rate
    void setGlobalUpdateRate(double hz);
    double getGlobalUpdateRate() const;
    
    // Set global visualization parameters
    void setGlobalParameter(const std::string& name, const std::string& value);
    std::string getGlobalParameter(const std::string& name) const;
    
    // Manage visualization groups
    void createGroup(const std::string& groupName);
    void addToGroup(const std::string& groupName, const std::string& visualizerName);
    void removeFromGroup(const std::string& groupName, const std::string& visualizerName);
    void removeGroup(const std::string& groupName);
    bool hasGroup(const std::string& groupName) const;
    
    // Control group visualization
    void showGroup(const std::string& groupName);
    void hideGroup(const std::string& groupName);
    void toggleGroup(const std::string& groupName);
    bool isGroupVisible(const std::string& groupName) const;
    
    // Synchronization with brain simulation
    void updateWithBrain(const Brain& brain, double elapsedTime);
    void updateWithMemory(const MemorySystem& memorySystem, double elapsedTime);
    void updateWithPerformance(const PerformanceMetrics& metrics, double elapsedTime);
    
    // Configuration management
    void loadConfiguration(const std::string& filename);
    void saveConfiguration(const std::string& filename) const;
    void resetConfiguration();
    
    // Visualization data management
    void setSharedData(const std::string& key, const std::string& data);
    std::string getSharedData(const std::string& key) const;
    void clearSharedData();
    
    // Event system
    void addEventListener(const std::string& eventType, std::function<void(const std::string&)> callback);
    void removeEventListener(const std::string& eventType, std::function<void(const std::string&)> callback);
    void emitEvent(const std::string& eventType, const std::string& data = "");
    
    // Performance optimization
    void enableAdaptiveQuality(bool enable);
    void setQualityLevel(int level);
    void optimizeForPerformance();
    void optimizeForQuality();
    
    // Resource management
    size_t getTotalMemoryUsage() const;
    size_t getGraphicsMemoryUsage() const;
    void cleanupUnusedResources();
    
    // Status and diagnostics
    std::string getStatusReport() const;
    bool isHealthy() const;
    std::vector<std::string> getErrorMessages() const;
    void clearErrorMessages();
    
    // Version and information
    std::string getManagerVersion() const;
    std::string getBuildInfo() const;
    
    // Callbacks for visualization events
    std::function<void(VisualizationInterface*)> onVisualizerAdded;
    std::function<void(VisualizationInterface*)> onVisualizerRemoved;
    std::function<void(VisualizationInterface*)> onVisualizerActivated;
    std::function<void(VisualizationInterface*)> onVisualizerDeactivated;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm