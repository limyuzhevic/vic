#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include "../performance/Performance.hpp"

namespace nlm {

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize with real-time capabilities
    bool initialize(int width = 1280, int height = 720, bool headless = false);
    
    // Real-time update loop (non-blocking)
    void startRealtimeUpdate(double fps = 30.0);
    void stopRealtimeUpdate();
    
    // Core visualization methods
    void visualizeNetwork(const Brain& brain, bool animated = true);
    void visualizeActivity(const Brain& brain, double timeWindow = 1.0);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes, size_t maxSpikes = 1000);
    void visualizeWeights(const Brain& brain, const std::vector<RegionId>& regions = {});
    
    // Advanced visualization capabilities
    void visualizeMemoryTrace(const Brain& brain, size_t episodeId = -1);
    void visualizeLearningDynamics(const Brain& brain, double timeScale = 1.0);
    void visualizeNeuralTopology(const Brain& brain);
    void visualizePerformanceMetrics(const PerformanceStats& stats);
    
    // Export visualizations
    bool exportFrame(const std::string& filename);
    bool exportAnimation(const std::string& filename, double duration = 5.0);
    bool saveSnapshot(const Brain& brain, const std::string& filename);
    
    // Data inspection
    void setInspectionMode(bool enabled);
    bool isInspectionMode() const;
    
    // Configuration
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    void setColorScheme(const std::string& scheme);
    void setVisualizationOptions(const std::map<std::string, bool>& options);
    
    // Progress tracking
    double getProgress() const;
    std::string getStatus() const;
    
    // Callbacks for events
    using VisualizationCallback = std::function<void(double progress, const std::string& message)>;
    void setProgressCallback(VisualizationCallback callback);
    
    // Live data monitoring
    void enableLiveDataMonitoring(bool enable);
    void updateLiveData(const Brain& brain, double dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
