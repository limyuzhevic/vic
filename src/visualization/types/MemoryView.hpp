#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class MemoryView {
public:
    MemoryView();
    virtual ~MemoryView();

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

    // Set memory type
    void setMemoryType(const std::string& type);
    const std::string& getMemoryType() const;

    // Enable/disable heatmap
    void enableHeatmap(bool enabled);
    bool isHeatmapEnabled() const;

    // Enable/disable trace visualization
    void enableTraceVisualization(bool enabled);
    bool isTraceVisualizationEnabled() const;

    // Set trace opacity
    void setTraceOpacity(float opacity);
    float getTraceOpacity() const;

    // Enable/disable correlation matrix
    void enableCorrelationMatrix(bool enabled);
    bool isCorrelationMatrixEnabled() const;

    // Set correlation threshold
    void setCorrelationThreshold(float threshold);
    float getCorrelationThreshold() const;

    // Enable/disable pattern detection
    void enablePatternDetection(bool enabled);
    bool isPatternDetectionEnabled() const;

    // Set pattern sensitivity
    void setPatternSensitivity(float sensitivity);
    float getPatternSensitivity() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Enable/disable memory replay
    void enableMemoryReplay(bool enabled);
    bool isMemoryReplayEnabled() const;

    // Set replay speed
    void setReplaySpeed(float speed);
    float getReplaySpeed() const;

    // Enable/disable memory reconstruction
    void enableMemoryReconstruction(bool enabled);
    bool isMemoryReconstructionEnabled() const;

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