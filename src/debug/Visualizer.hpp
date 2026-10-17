#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <unordered_map>
#include "../core/Logger/Logger.hpp"
#include "DebugConfig.hpp"
#include "Profiler.hpp"
#include "AlertSystem.hpp"

namespace nlm {

// Visualization data structures
struct NeuralNetworkVisualization {
    std::vector<std::vector<float>> layerActivities;
    std::vector<std::vector<float>> connectionWeights;
    std::vector<std::string> neuronTypes;
    std::vector<std::string> neuronLabels;
    std::map<std::string, std::vector<float>> timeSeriesData;
};

struct ActivityPattern {
    std::string patternId;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    std::vector<float> activityValues;
    std::vector<float> firingRates;
    float peakActivity;
    float averageActivity;
    std::string patternType;
    std::map<std::string, float> statisticalFeatures;
};

struct VisualizationResult {
    std::string resultId;
    std::chrono::steady_clock::time_point timestamp;
    std::string visualizationType;
    std::string outputFormat;
    std::string filePath;
    std::vector<unsigned char> imageData;
    std::string encodedData; // For text-based formats
    std::map<std::string, std::string> metadata;
    bool success;
    std::string errorMessage;
};

// Visualization tool interface
class IVisualizer {
public:
    virtual ~IVisualizer() = default;
    virtual void initialize(std::shared_ptr<DebugConfig> config) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual VisualizationResult visualizeNeuralNetwork(const NeuralNetworkVisualization& network) = 0;
    virtual VisualizationResult visualizeActivityPatterns(const std::vector<ActivityPattern>& patterns) = 0;
    virtual VisualizationResult visualizeResults(const std::map<std::string, double>& results) = 0;
    virtual VisualizationResult visualizeTimeSeries(const std::map<std::string, std::vector<float>>& timeSeriesData, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) = 0;
    virtual VisualizationResult visualizeStatisticalData(const std::map<std::string, std::vector<float>>& statisticalData) = 0;
    virtual bool saveVisualization(const VisualizationResult& result, const std::string& filepath) = 0;
    virtual std::vector<VisualizationResult> getRecentVisualizations(size_t count = 10) const = 0;
    virtual void clearVisualizationCache() = 0;
};

// Visualization tool implementation
class Visualizer : public IVisualizer {
public:
    Visualizer();
    ~Visualizer() override;
    
    // Disable copying, enable moving
    Visualizer(const Visualizer&) = delete;
    Visualizer& operator=(const Visualizer&) = delete;
    Visualizer(Visualizer&&) noexcept;
    Visualizer& operator=(Visualizer&&) noexcept;
    
    void initialize(std::shared_ptr<DebugConfig> config) override;
    void start() override;
    void stop() override;
    void update() override;
    VisualizationResult visualizeNeuralNetwork(const NeuralNetworkVisualization& network) override;
    VisualizationResult visualizeActivityPatterns(const std::vector<ActivityPattern>& patterns) override;
    VisualizationResult visualizeResults(const std::map<std::string, double>& results) override;
    VisualizationResult visualizeTimeSeries(const std::map<std::string, std::vector<float>>& timeSeriesData, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) override;
    VisualizationResult visualizeStatisticalData(const std::map<std::string, std::vector<float>>& statisticalData) override;
    bool saveVisualization(const VisualizationResult& result, const std::string& filepath) override;
    std::vector<VisualizationResult> getRecentVisualizations(size_t count = 10) const override;
    void clearVisualizationCache() override;
    
    // Visualization format support
    enum class VisualizationFormat {
        PNG,
        JSON,
        CSV,
        HTML,
        Text,
        Custom
    };
    
    // Visualization type
    enum class VisualizationType {
        NeuralNetwork,
        ActivityPattern,
        Results,
        TimeSeries,
        Statistical,
        Custom
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
