#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class SpikeView {
public:
    SpikeView();
    virtual ~SpikeView();

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

    // Set raster plot type
    void setRasterPlotType(const std::string& type);
    const std::string& getRasterPlotType() const;

    // Enable/disable spike time lines
    void enableSpikeTimeLines(bool enabled);
    bool areSpikeTimeLinesEnabled() const;

    // Enable/disable spike markers
    void enableSpikeMarkers(bool enabled);
    bool areSpikeMarkersEnabled() const;

    // Set marker size
    void setMarkerSize(float size);
    float getMarkerSize() const;

    // Set marker color
    void setMarkerColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getMarkerColor() const;

    // Set line width
    void setLineWidth(float width);
    float getLineWidth() const;

    // Enable/disable clustering
    void enableClustering(bool enabled);
    bool isClusteringEnabled() const;

    // Set cluster count
    void setClusterCount(int count);
    int getClusterCount() const;

    // Set firing rate threshold
    void setFiringRateThreshold(float threshold);
    float getFiringRateThreshold() const;

    // Enable/disable burst detection
    void enableBurstDetection(bool enabled);
    bool isBurstDetectionEnabled() const;

    // Set burst sensitivity
    void setBurstSensitivity(float sensitivity);
    float getBurstSensitivity() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Enable/disable auto-scale
    void enableAutoScale(bool enabled);
    bool isAutoScaleEnabled() const;

    // Enable/disable spike interpolation
    void enableSpikeInterpolation(bool enabled);
    bool isSpikeInterpolationEnabled() const;

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