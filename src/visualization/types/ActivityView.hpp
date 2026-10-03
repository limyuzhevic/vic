#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class ActivityView {
public:
    ActivityView();
    virtual ~ActivityView();

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

    // Set heatmap type
    void setHeatmapType(const std::string& type);
    const std::string& getHeatmapType() const;

    // Enable/disable color scaling
    void enableColorScaling(bool enabled);
    bool isColorScalingEnabled() const;

    // Set color scale
    void setColorScale(const std::string& scale);
    const std::string& getColorScale() const;

    // Set colormap
    void setColormap(const std::string& colormap);
    const std::string& getColormap() const;

    // Set threshold
    void setThreshold(float threshold);
    float getThreshold() const;

    // Enable/disable normalization
    void enableNormalization(bool enabled);
    bool isNormalizationEnabled() const;

    // Enable/disable smoothing
    void enableSmoothing(bool enabled);
    bool isSmoothingEnabled() const;

    // Set smoothing factor
    void setSmoothingFactor(float factor);
    float getSmoothingFactor() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Enable/disable real-time updates
    void enableRealTimeUpdates(bool enabled);
    bool areRealTimeUpdatesEnabled() const;

    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;

    // Enable/disable spike overlay
    void enableSpikeOverlay(bool enabled);
    bool isSpikeOverlayEnabled() const;

    // Enable/disable weight overlay
    void enableWeightOverlay(bool enabled);
    bool isWeightOverlayEnabled() const;

    // Set neuron filtering
    void setNeuronFilter(const std::string& filter);
    const std::string& getNeuronFilter() const;

    // Set region filtering
    void setRegionFilter(const std::string& filter);
    const std::string& getRegionFilter() const;

    // Set activity threshold
    void setActivityThreshold(float threshold);
    float getActivityThreshold() const;

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