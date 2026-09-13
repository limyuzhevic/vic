#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @class VisualizationRegistry
 * @brief Plugin-style visualizer management system
 * 
 * This class provides a centralized registry for all visualization components.
 * It supports registration, lookup, and lifecycle management of visualizers.
 * The registry is a singleton pattern to ensure a single point of access
 * to all visualization components.
 */
class VisualizationRegistry {
public:
    static VisualizationRegistry& getInstance() {
        static VisualizationRegistry instance;
        return instance;
    }
    
    VisualizationRegistry(const VisualizationRegistry&) = delete;
    VisualizationRegistry& operator=(const VisualizationRegistry&) = delete;
    
    // Visualizer registration
    bool registerVisualizer(std::unique_ptr<VisualizationBase> visualizer);
    bool unregisterVisualizer(const std::string& name);
    
    // Visualizer lookup
    VisualizationBase* getVisualizer(const std::string& name);
    VisualizationBase* getVisualizer(size_t index);
    
    // Visualizer management
    std::vector<std::string> getVisualizerNames() const;
    size_t getVisualizerCount() const;
    
    // Batch operations
    void registerVisualizers(const std::vector<std::unique_ptr<VisualizationBase>>& visualizers);
    void unregisterAll();
    
    // Configuration management
    bool setGlobalConfiguration(const VisualizationConfig& config);
    const VisualizationConfig& getGlobalConfiguration() const { return globalConfig_; }
    
    // Data streaming
    void setDataStream(VisualizationDataStream* stream);
    
    // Performance monitoring
    double getSystemFPS() const { return systemFPS_; }
    double getAverageFrameTime() const { return averageFrameTime_; }
    
    // Health check
    bool isHealthy() const;
    std::string getHealthReport() const;
    
    // Initialization and cleanup
    bool initializeAll();
    void updateAll();
    void renderAll();
    void closeAll();
    
    // Group operations
    void activateVisualizers(const std::vector<std::string>& names);
    void deactivateVisualizers(const std::vector<std::string>& names);
    std::vector<std::string> getActiveVisualizerNames() const;
    
    // Statistics and profiling
    struct VisualizerStats {
        std::string name;
        uint32_t updateCount;
        double totalUpdateTime;
        double averageUpdateTime;
        uint32_t renderCount;
        double totalRenderTime;
        double averageRenderTime;
        bool isActive;
    };
    
    std::vector<VisualizerStats> getAllStats() const;
    
private:
    VisualizationRegistry() = default;
    ~VisualizationRegistry() = default;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    VisualizationConfig globalConfig_;
    VisualizationDataStream* dataStream_ = nullptr;
    double systemFPS_ = 0.0;
    double averageFrameTime_ = 0.0;
    uint64_t totalFrames_ = 0;
};

} // namespace nlm
