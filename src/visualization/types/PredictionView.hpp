#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class PredictionView {
public:
    PredictionView();
    virtual ~PredictionView();

    // Initialize the view
    virtual bool initialize() = 0;

    // Update the view with new data
    virtual void update(const VisualizationData& data) = 0;

    // Render the view
    virtual void render() = 0;

    // Get view name
    const std::string& getName() const;

    // Set view name
    void setName(const std::string& name);

    // Enable/disable view
    void enable(bool enabled);
    bool isEnabled() const;

    // Set prediction type
    void setPredictionType(const std::string& type);
    const std::string& getPredictionType() const;

    // Enable/disable confidence visualization
    void enableConfidenceVisualization(bool enabled);
    bool isConfidenceVisualizationEnabled() const;

    // Enable/disable error visualization
    void enableErrorVisualization(bool enabled);
    bool isErrorVisualizationEnabled() const;

    // Set confidence threshold
    void setConfidenceThreshold(float threshold);
    float getConfidenceThreshold() const;

    // Set error threshold
    void setErrorThreshold(float threshold);
    float getErrorThreshold() const;

    // Enable/disable probability heatmap
    void enableProbabilityHeatmap(bool enabled);
    bool isProbabilityHeatmapEnabled() const;

    // Enable/disable prediction trajectory
    void enablePredictionTrajectory(bool enabled);
    bool isPredictionTrajectoryEnabled() const;

    // Set trajectory smoothness
    void setTrajectorySmoothness(float smoothness);
    float getTrajectorySmoothness() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Enable/disable uncertainty visualization
    void enableUncertaintyVisualization(bool enabled);
    bool isUncertaintyVisualizationEnabled() const;

    // Set uncertainty threshold
    void setUncertaintyThreshold(float threshold);
    float getUncertaintyThreshold() const;

    // Reset view to default
    virtual void reset();

    // Export view data
    virtual bool exportView(const std::string& filepath) = 0;

    // Get view statistics
    virtual std::map<std::string, double> getStatistics() const = 0;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm