#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>

namespace nlm {

// SimpleTextVisualization: Terminal-based visualization
// Provides basic text-based visualization for headless environments
// and debugging

class SimpleTextVisualization : public VisualizationInterface {
public:
    SimpleTextVisualization();
    ~SimpleTextVisualization();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Start visualization (non-blocking)
    void start() override;

    // Stop visualization
    void stop() override;

    // Update with current brain state
    void update(const Brain* brain, SimulationStep step, Timestamp time) override;

    // Set visualization mode
    void setMode(const std::string& mode) override;

    // Add visualization callback
    void addCallback(std::function<void(const Brain* brain)> callback) override;

    // Set update interval
    void setUpdateInterval(int interval) override;

    // Save visualization to file
    bool saveToFile(const std::string& filepath) override;

    // Export brain state for external analysis
    bool exportBrainState(const std::string& filepath) override;

    // Get current status
    bool isRunning() const override;
    std::string getCurrentMode() const override;
    size_t getFrameCount() const override;
    float getUpdateRate() const override;

    // Set visualization parameters
    void setNeuronSize(float size);
    void setSynapseWidth(float width);
    void setActivityThreshold(float threshold);
    void enableSpikes(bool enable);
    void enableConnections(bool enable);

    // Control display
    void setShowNeurons(bool show);
    void setShowConnections(bool show);
    void setShowStatistics(bool show);
    void setShowMemory(bool show);

    // Export options
    void setExportFormat(const std::string& format); // "csv", "json", "txt"
    void setExportPath(const std::string& path);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::thread updateThread_;
    mutable std::mutex updateMutex_;
    std::chrono::steady_clock::time_point lastUpdate_;
    std::atomic<float> updateRate_ = 0.0f;

    void updateThreadFunc();
    void drawTextNeurons(const Brain* brain);
    void drawTextConnections(const Brain* brain);
    void drawTextStatistics(const Brain* brain);
    void drawTextMemory(const Brain* brain);
    void exportToCSV(const std::string& filepath, const Brain* brain);
    void exportToJSON(const std::string& filepath, const Brain* brain);
    void exportToText(const std::string& filepath, const Brain* brain);
};

} // namespace nlm