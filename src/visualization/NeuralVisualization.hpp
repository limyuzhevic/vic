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

namespace nlm {

// VisualizationInterface: Abstract base class for neural visualization
// Provides common functionality for real-time brain state monitoring

class VisualizationInterface {
public:
    VisualizationInterface() = default;
    virtual ~VisualizationInterface() = default;

    // Start the visualization system
    virtual void start() = 0;

    // Stop the visualization system
    virtual void stop() = 0;

    // Update visualization with current brain state
    virtual void update(const Brain* brain, SimulationStep step, Timestamp time) = 0;

    // Set visualization mode
    virtual void setMode(const std::string& mode) = 0;

    // Add visualization callback
    virtual void addCallback(std::function<void(const Brain* brain)> callback) = 0;

    // Set update interval (in milliseconds)
    virtual void setUpdateInterval(int interval) = 0;

    // Save visualization to file
    virtual bool saveToFile(const std::string& filepath) = 0;

    // Export brain state for external analysis
    virtual bool exportBrainState(const std::string& filepath) = 0;

    // Get current visualization status
    virtual bool isRunning() const = 0;
    virtual std::string getCurrentMode() const = 0;
    virtual size_t getFrameCount() const = 0;
    virtual float getUpdateRate() const = 0;

protected:
    mutable std::mutex callbackMutex_;
    std::vector<std::function<void(const Brain* brain)>> callbacks_;
    std::atomic<bool> running_ = false;
    std::atomic<size_t> frameCount_ = 0;
};

// NeuralVisualization: Real-time neural network visualization
// Provides interactive 3D/2D views of brain structure and activity
//
// Key features:
// - Real-time neural activity display
// - Configurable visualization modes (3D, 2D, projection, slice)
// - Neuron population coloring based on activity
// - Spike event visualization
// - Synapses visualization with dynamic weights
// - Recording and replay capabilities
// - Export capabilities for external analysis

class NeuralVisualization : public VisualizationInterface {
public:
    NeuralVisualization();
    ~NeuralVisualization();

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

    // Control camera/view
    void setCameraAngle(float yaw, float pitch, float distance);
    void setViewMode(const std::string& mode); // "orthographic", "perspective", "top-down"

    // Color schemes
    void setColorScheme(const std::string& scheme); // "activity", "region", "type", "gradient"

    // Recording
    void startRecording(const std::string& filename);
    void stopRecording();
    bool isRecording() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::thread updateThread_;
    mutable std::mutex updateMutex_;
    std::chrono::steady_clock::time_point lastUpdate_;
    std::atomic<float> updateRate_ = 0.0f;
    bool recording_ = false;
    std::string recordingFilename_;
    std::ofstream recordingFile_;

    void updateThreadFunc();
    void processFrame(const Brain* brain, SimulationStep step, Timestamp time);
    void drawNeurons(const Brain* brain);
    void drawSynapses(const Brain* brain);
    void drawConnections(const Brain* brain);
    void drawSpikes(const Brain* brain, Timestamp time);
};

} // namespace nlm