#pragma once

#include <string>
#include <vector>
#include <map>
#include "../core/Types/Types.hpp"

namespace nlm {

class TimeSeriesView {
public:
    TimeSeriesView();
    virtual ~TimeSeriesView();

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

    // Set time series type
    void setTimeSeriesType(const std::string& type);
    const std::string& getTimeSeriesType() const;

    // Enable/disable multiple traces
    void enableMultipleTraces(bool enabled);
    bool areMultipleTracesEnabled() const;

    // Set trace grouping
    void setTraceGrouping(const std::string& grouping);
    const std::string& getTraceGrouping() const;

    // Set smoothing factor
    void setSmoothingFactor(float factor);
    float getSmoothingFactor() const;

    // Enable/disable interpolation
    void enableInterpolation(bool enabled);
    bool isInterpolationEnabled() const;

    // Set time window
    void setTimeWindow(Timestamp start, Timestamp end);
    std::pair<Timestamp, Timestamp> getTimeWindow() const;

    // Enable/disable range selection
    void enableRangeSelection(bool enabled);
    bool isRangeSelectionEnabled() const;

    // Set range min/max
    void setRangeMin(float min);
    float getRangeMin() const;

    void setRangeMax(float max);
    float getRangeMax() const;

    // Enable/disable zoom
    void enableZoom(bool enabled);
    bool isZoomEnabled() const;

    // Enable/disable pan
    void enablePan(bool enabled);
    bool isPanEnabled() const;

    // Enable/disable cursor line
    void enableCursorLine(bool enabled);
    bool isCursorLineEnabled() const;

    // Set cursor line color
    void setCursorLineColor(float r, float g, float b, float a = 1.0f);
    std::array<float, 4> getCursorLineColor() const;

    // Enable/disable statistics overlay
    void enableStatisticsOverlay(bool enabled);
    bool areStatisticsOverlayEnabled() const;

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