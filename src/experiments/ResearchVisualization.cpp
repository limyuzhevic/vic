#include "AdvancedExperimentManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct ResearchVisualization::Impl {
    std::vector<Phase6IntegrationResult> experimentResults;
    json visualizationData;
    bool initialized;
};

ResearchVisualization::ResearchVisualization() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

ResearchVisualization::~ResearchVisualization() = default;

void ResearchVisualization::initialize(const std::vector<Phase6IntegrationResult>& results) {
    pImpl->experimentResults = results;
    pImpl->initialized = true;
    
    std::cout << "Initializing research visualization with " << results.size() << " experiments" << std::endl;
}

bool ResearchVisualization::generateComparativePlots(
    const std::string& outputPrefix,
    const std::set<std::string>& metrics) {
    
    if (!pImpl->initialized || pImpl->experimentResults.empty()) {
        std::cout << "No data available for visualization" << std::endl;
        return false;
    }
    
    std::cout << "Generating comparative plots to: " << outputPrefix << std::endl;
    
    // Generate data for different metrics
    json plotData;
    
    // Extract metric data
    std::vector<double> rewards, firingRates, times, novelties, curiosities;
    
    for (const auto& result : pImpl->experimentResults) {
        rewards.push_back(result.totalReward);
        firingRates.push_back(result.avgFiringRate);
        times.push_back(result.totalWallClockTime);
        novelties.push_back(result.noveltyLevel);
        curiosities.push_back(result.curiosityLevel);
    }
    
    // Create plot datasets
    json rewardsPlot, firingPlot, timePlot;
    
    // Experiment indices
    json experimentIndices;
    for (size_t i = 0; i < pImpl->experimentResults.size(); ++i) {
        experimentIndices.push_back(i);
    }
    
    // Rewards plot
    rewardsPlot["x"] = experimentIndices;
    rewardsPlot["y"] = rewards;
    rewardsPlot["type"] = "scatter";
    rewardsPlot["name"] = "Total Reward";
    
    // Firing rate plot
    firingPlot["x"] = experimentIndices;
    firingPlot["y"] = firingRates;
    firingPlot["type"] = "boxplot";
    firingPlot["name"] = "Firing Rate";
    
    // Time plot
    timePlot["x"] = experimentIndices;
    timePlot["y"] = times;
    timePlot["type"] = "violin";
    timePlot["name"] = "Wall Clock Time";
    
    plotData["rewards"] = rewardsPlot;
    plotData["firing"] = firingPlot;
    plotData["time"] = timePlot;
    
    // Save plot data
    std::string plotFile = outputPrefix + "_plots.json";
    std::ofstream file(plotFile);
    if (file.is_open()) {
        file << plotData.dump(2);
        file.close();
        std::cout << "  Plot data saved to: " << plotFile << std::endl;
    }
    
    return true;
}

bool ResearchVisualization::generateLearningCurves(const std::string& outputPrefix) {
    
    if (!pImpl->initialized || pImpl->experimentResults.empty()) {
        std::cout << "No data available for learning curve generation" << std::endl;
        return false;
    }
    
    std::cout << "Generating learning curves to: " << outputPrefix << std::endl;
    
    json learningData;
    
    // For demonstration, create synthetic learning curves
    // In a real implementation, this would track metrics over time within each experiment
    
    json learningCurve;
    learningCurve["x"] = std::vector<double>{0, 100, 1000, 5000, 10000}; // Step counts
    
    // Create multiple learning curves (one per experiment)
    json curves = json::array();
    
    for (size_t i = 0; i < std::min(size_t(5), pImpl->experimentResults.size()); ++i) {
        json curve;
        curve["experiment"] = i;
        curve["name"] = "Experiment " + std::to_string(i);
        
        std::vector<double> values;
        double baseReward = pImpl->experimentResults[i].totalReward;
        
        // Create a learning curve that starts low and approaches the final value
        for (double step : learningCurve["x"]) {
            double progress = step / 10000.0; // Normalize
            double value = baseReward * progress * 0.8 + baseReward * 0.2; // Start low, approach final
            values.push_back(value);
        }
        
        curve["y"] = values;
        curves.push_back(curve);
    }
    
    learningData["curves"] = curves;
    learningData["xLabel"] = "Simulation Steps";
    learningData["yLabel"] = "Total Reward";
    learningData["title"] = "Learning Curves: Experiment Comparison";
    
    // Save learning curve data
    std::string learningFile = outputPrefix + "_learning_curves.json";
    std::ofstream file(learningFile);
    if (file.is_open()) {
        file << learningData.dump(2);
        file.close();
        std::cout << "  Learning curves saved to: " << learningFile << std::endl;
    }
    
    return true;
}

bool ResearchVisualization::generateParameterHeatmap(
    const json& sensitivityData,
    const std::string& outputFile) {
    
    std::cout << "Generating parameter heatmap: " << outputFile << std::endl;
    
    // For demonstration, create a simple heatmap visualization
    // This would typically require more sophisticated plotting libraries
    
    json heatmapData;
    
    if (sensitivityData.contains("dataPoints")) {
        heatmapData["title"] = "Parameter Sensitivity Heatmap";
        heatmapData["xLabel"] = "Parameter Value";
        heatmapData["yLabel"] = "Experiment Index";
        
        // Create a simple 2D representation
        std::vector<std::vector<double>> matrix;
        size_t numExperiments = std::min(size_t(10), pImpl->experimentResults.size());
        
        for (size_t i = 0; i < numExperiments; ++i) {
            std::vector<double> row;
            for (size_t j = 0; j < numExperiments; ++j) {
                // Simple distance-based value for demonstration
                double distance = std::abs(static_cast<double>(i) - static_cast<double>(j));
                row.push_back(1.0 / (1.0 + distance));
            }
            matrix.push_back(row);
        }
        
        heatmapData["matrix"] = matrix;
        heatmapData["experiments"] = numExperiments;
    }
    
    // Save heatmap data
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << heatmapData.dump(2);
        file.close();
        std::cout << "  Heatmap saved to: " << outputFile << std::endl;
    }
    
    return true;
}

bool ResearchVisualization::createNetworkVisualization(
    const std::string& resultFile,
    const std::string& outputFile) {
    
    std::cout << "Creating network visualization: " << outputFile << std::endl;
    
    // For demonstration, create a simple network visualization
    json networkData;
    
    networkData["title"] = "Network Visualization: Best Performing Experiment";
    networkData["type"] = "circular_layout";
    
    // Create a simple node representation
    json nodes = json::array();
    size_t numNeurons = 50; // Example number
    
    for (size_t i = 0; i < numNeurons; ++i) {
        json node;
        node["id"] = i;
        node["x"] = (i % 10) * 50.0;
        node["y"] = (i / 10) * 50.0;
        node["radius"] = 5.0 + (pImpl->experimentResults.empty() ? 5.0 : pImpl->experimentResults[0].totalReward / 100.0) * 10.0;
        node["color"] = getColorForValue(pImpl->experimentResults.empty() ? 0.5 : pImpl->experimentResults[0].totalReward / 100.0);
        
        nodes.push_back(node);
    }
    
    networkData["nodes"] = nodes;
    
    // Create simple edges
    json edges = json::array();
    size_t numEdges = numNeurons * 2; // Example number
    
    for (size_t i = 0; i < numEdges; ++i) {
        json edge;
        edge["source"] = i % numNeurons;
        edge["target"] = (i + 1) % numNeurons;
        edge["weight"] = 0.5 + (std::rand() % 100) / 100.0;
        edges.push_back(edge);
    }
    
    networkData["edges"] = edges;
    
    // Save network data
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << networkData.dump(2);
        file.close();
        std::cout << "  Network visualization saved to: " << outputFile << std::endl;
    }
    
    return true;
}

bool ResearchVisualization::generateStatisticalPlots(const std::string& outputPrefix) {
    
    if (!pImpl->initialized || pImpl->experimentResults.empty()) {
        std::cout << "No data available for statistical plot generation" << std::endl;
        return false;
    }
    
    std::cout << "Generating statistical plots to: " << outputPrefix << std::endl;
    
    json statsData;
    
    // Extract all metrics
    std::vector<double> rewards, firingRates, times, novelties, curiosities, dopamines;
    
    for (const auto& result : pImpl->experimentResults) {
        rewards.push_back(result.totalReward);
        firingRates.push_back(result.avgFiringRate);
        times.push_back(result.totalWallClockTime);
        novelties.push_back(result.noveltyLevel);
        curiosities.push_back(result.curiosityLevel);
        dopamines.push_back(result.dopamineLevel);
    }
    
    // Calculate statistics
    statsData["statistics"] = {
        {"totalRewardMean", calculateMean(rewards)},
        {"totalRewardStd", calculateStdDev(rewards, calculateMean(rewards))},
        {"firingRateMean", calculateMean(firingRates)},
        {"firingRateStd", calculateStdDev(firingRates, calculateMean(firingRates))},
        {"timeMean", calculateMean(times)},
        {"timeStd", calculateStdDev(times, calculateMean(times))}
    };
    
    // Create histogram data
    statsData["histograms"] = {
        {"rewards", createHistogram(rewards, 10)},
        {"firingRates", createHistogram(firingRates, 10)},
        {"times", createHistogram(times, 10)}
    };
    
    // Save statistical data
    std::string statsFile = outputPrefix + "_statistics.json";
    std::ofstream file(statsFile);
    if (file.is_open()) {
        file << statsData.dump(2);
        file.close();
        std::cout << "  Statistical plots saved to: " << statsFile << std::endl;
    }
    
    return true;
}

bool ResearchVisualization::generateExperimentTimeline(
    const std::vector<Phase6IntegrationResult>& results,
    const std::string& outputFile) {
    
    std::cout << "Generating experiment timeline: " << outputFile << std::endl;
    
    json timelineData;
    
    timelineData["title"] = "Experiment Timeline";
    timelineData["experiments"] = results.size();
    
    // Create timeline events
    json events = json::array();
    
    double startTime = getCurrentTimestampDouble();
    
    for (size_t i = 0; i < results.size(); ++i) {
        json event;
        event["id"] = i;
        event["name"] = "Experiment " + std::to_string(i);
        event["start"] = startTime + i * 1000; // 1000 units apart
        event["end"] = startTime + i * 1000 + results[i].totalWallClockTime * 100;
        event["duration"] = results[i].totalWallClockTime * 100;
        event["result"] = results[i].totalReward;
        event["success"] = results[i].totalReward > 0;
        
        events.push_back(event);
    }
    
    timelineData["events"] = events;
    
    // Save timeline data
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << timelineData.dump(2);
        file.close();
        std::cout << "  Timeline saved to: " << outputFile << std::endl;
    }
    
    return true;
}
json ResearchVisualization::exportVisualizationData() const {
    json data;
    
    if (!pImpl->initialized) {
        return data;
    }
    
    data["experimentsCount"] = pImpl->experimentResults.size();
    data["initialized"] = pImpl->initialized;
    
    // Export key metrics
    std::vector<double> rewards, firingRates, times;
    std::vector<double> novelties, curiosities, dopamines;
    
    for (const auto& result : pImpl->experimentResults) {
        rewards.push_back(result.totalReward);
        firingRates.push_back(result.avgFiringRate);
        times.push_back(result.totalWallClockTime);
        novelties.push_back(result.noveltyLevel);
        curiosities.push_back(result.curiosityLevel);
        dopamines.push_back(result.dopamineLevel);
    }
    
    data["metrics"] = {
        {"totalReward", rewards},
        {"firingRate", firingRates},
        {"wallClockTime", times},
        {"noveltyLevel", novelties},
        {"curiosityLevel", curiosities},
        {"dopamineLevel", dopamines}
    };
    
    return data;
}
void ResearchVisualization::setVisualizationOptions(const json& options) {
    pImpl->visualizationData = options;
}
json ResearchVisualization::getCurrentData() const {
    return pImpl->visualizationData;
}

// Helper functions for ResearchVisualization

std::string ResearchVisualization::getColorForValue(double value) {
    if (value < 0.33) return "#FF0000";  // Red
    if (value < 0.67) return "#FFFF00";  // Yellow
    return "#00FF00";  // Green
}

json ResearchVisualization::createHistogram(const std::vector<double>& values, size_t bins) {
    json histogram;
    
    if (values.empty()) {
        return histogram;
    }
    
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    double range = max - min;
    
    if (range < 1e-10) {
        histogram["values"] = {min};
        return histogram;
    }
    
    double binWidth = range / bins;
    std::vector<size_t> counts(bins, 0);
    
    for (double value : values) {
        size_t bin = static_cast<size_t>((value - min) / binWidth);
        if (bin >= bins) bin = bins - 1;
        counts[bin]++;
    }
    
    histogram["bins"] = bins;
    histogram["min"] = min;
    histogram["max"] = max;
    histogram["binWidth"] = binWidth;
    histogram["counts"] = counts;
    
    return histogram;
}
double ResearchVisualization::getCurrentTimestampDouble() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return static_cast<double>(millis.count());
}

} // namespace nlm
