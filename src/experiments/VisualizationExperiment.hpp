// Visualization test mode
// Visual analysis and presentation of NLM state

#pragma once

#include "experiments/Experiment.hpp"
#include "core/PerformanceProfiler.hpp"
#include "core/OutputFormatter.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Visualization result
struct VisualizationResult {
    std::string visualizationType; // "network", "spike_raster", "weight_matrix", "trajectory"
    std::string outputFile;
    std::string format; // "png", "svg", "pdf", "ascii"
    double fileSize; // in bytes
    std::chrono::system_clock::time_point generationTime;
    bool interactive; // interactive visualization
    bool animation; // animated visualization
    std::vector<std::string> dataPoints; // data points for visualization
    std::string metadata; // additional metadata
    
    VisualizationResult() : fileSize(0), interactive(false), animation(false) {}
};

// Visualization experiment
class VisualizationExperiment : public Experiment {
public:
    VisualizationExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~VisualizationExperiment() = default;
    
    // Experiment interface
    bool setup() override;
    bool run() override;
    bool cleanup() override;
    
    // Visualization-specific methods
    bool generateNetworkVisualization(const std::string& outputFile = "network.png");
    bool generateSpikeRasterVisualization(const std::string& outputFile = "spike_raster.png");
    bool generateWeightMatrixVisualization(const std::string& outputFile = "weight_matrix.png");
    bool generateTrajectoryVisualization(const std::string& outputFile = "trajectory.png");
    
    // Interactive visualization
    bool generateInteractiveDashboard(const std::string& outputFile = "dashboard.html");
    bool generateAnimation(const std::string& outputFile = "animation.gif", 
                          std::chrono::milliseconds frameInterval = std::chrono::milliseconds(100));
    
    // Results management
    std::vector<VisualizationResult> getResults() const { return results_; }
    VisualizationResult getLatestResult() const { return results_.empty() ? VisualizationResult() : results_.back(); }
    
    // Output generation
    std::string generateReport() const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
protected:
    std::vector<VisualizationResult> results_;
    std::shared_ptr<PerformanceProfiler> profiler_;
    VisualizationConfig visualizationConfig_;
};

} // namespace nlm
