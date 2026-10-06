#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize with configuration
    bool initialize(std::shared_ptr<Config> config);
    
    // Rendering methods
    void renderNeuralNetwork(const Brain& brain, const std::string& outputFile = "neural_network.html");
    void renderActivityMap(const Brain& brain, const std::string& outputFile = "activity_map.html");
    void renderMemoryTrajectories(const Brain& brain, const std::string& outputFile = "memory_trajectories.html");
    void renderTrainingProgress(const std::string& outputFile = "training_progress.html");
    
    // Real-time visualization
    void startRealTimeVisualization(const Brain& brain, int updateIntervalMs = 100);
    void stopRealTimeVisualization();
    bool isRealTimeVisualizationRunning() const;
    
    // Export formats
    void exportToJSON(const Brain& brain, const std::string& filepath);
    void exportToCSV(const Brain& brain, const std::string& filepath);
    void exportToNeuroML(const Brain& brain, const std::string& filepath);
    
    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setUpdateInterval(int intervalMs);
    void enableAutoSave(bool enable, int saveIntervalSteps = 1000);
    void setQualityLevel(QualityLevel level);
    
    // Metrics and statistics
    std::string getPerformanceMetrics() const;
    std::string getVisualizationStats() const;
    
    // Interactive features
    void enableInteractivity(bool enable);
    void setInteractiveCallback(std::function<void(const std::string& event)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

enum class QualityLevel {
    Low,      // Fast rendering, basic visualization
    Medium,   // Balanced performance and quality
    High,     // High quality, slower rendering
    Ultra    // Highest quality, slowest rendering
};

} // namespace nlm
