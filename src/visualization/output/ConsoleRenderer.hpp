#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "../core/Types/Types.hpp"

namespace nlm {

class ConsoleRenderer {
public:
    ConsoleRenderer();
    ~ConsoleRenderer();

    // Initialize the renderer
    bool initialize();

    // Shutdown the renderer
    void shutdown();

    // Update the renderer
    void update(double deltaTime);

    // Render data to console
    bool render(const std::map<std::string, VisualizationData>& data);

    // Export data to console
    bool exportToConsole(const std::string& filepath, const std::map<std::string, VisualizationData>& data);

    // Set output stream
    void setOutputStream(std::ostream& stream);

    // Get output stream
    std::ostream& getOutputStream();

    // Enable/disable colors
    void enableColors(bool enabled);
    bool areColorsEnabled() const;

    // Enable/disable verbose output
    void enableVerbose(bool enabled);
    bool isVerboseEnabled() const;

    // Set output format
    void setOutputFormat(const std::string& format);
    const std::string& getOutputFormat() const;

    // Enable/disable line wrapping
    void enableLineWrapping(bool enabled);
    bool isLineWrappingEnabled() const;

    // Set line width
    void setLineWidth(int width);
    int getLineWidth() const;

    // Enable/disable progress indicators
    void enableProgressIndicators(bool enabled);
    bool areProgressIndicatorsEnabled() const;

    // Set progress indicator style
    void setProgressIndicatorStyle(const std::string& style);
    const std::string& getProgressIndicatorStyle() const;

    // Render network to console
    void renderNetwork(const VisualizationData::Network& network);

    // Render activity to console
    void renderActivity(const VisualizationData::Activity& activity);

    // Render spikes to console
    void renderSpikes(const VisualizationData::Spikes& spikes);

    // Render weights to console
    void renderWeights(const VisualizationData::Weights& weights);

    // Render time series to console
    void renderTimeSeries(const VisualizationData::TimeSeries& timeSeries);

    // Render memory to console
    void renderMemory(const VisualizationData::Memory& memory);

    // Render prediction to console
    void renderPrediction(const VisualizationData::Prediction& prediction);

    // Render simulation to console
    void renderSimulation(const VisualizationData::Simulation& simulation);

    // Render performance to console
    void renderPerformance(const VisualizationData::Performance& performance);

    // Clear console
    void clear();

    // Get console size
    std::pair<int, int> getConsoleSize() const;

    // Set console size
    void setConsoleSize(int width, int height);

    // Enable/disable auto-resize
    void enableAutoResize(bool enabled);
    bool isAutoResizeEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm