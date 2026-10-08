#pragma once

#include "VisualizationInterface.hpp"
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <unordered_map>
#include <glm/glm.hpp>
#include <thread>
#include <atomic>

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

class ActivityVisualizer : public VisualizationInterface {
public:
    ActivityVisualizer();
    ~ActivityVisualizer() override;
    
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    bool supports3D() const override;
    std::string getName() const override;
    std::string getVersion() const override;
    void onResize(int width, int height) override;
    void setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    void setComponentEnabled(const std::string& component, bool enabled) override;
    bool isComponentEnabled(const std::string& component) const override;
    void addEventCallback(const std::string& eventType, std::function<void(const std::string&)> callback) override;
    bool exportState(const std::string& filename, const std::string& format) override;
    bool importState(const std::string& filename, const std::string& format) override;
    
    // Activity visualization specific methods
    void visualizeNeuralActivity(const Brain& brain, double elapsedTime);
    void visualizeSpikePatterns(const std::vector<SpikeEvent>& spikes, const std::vector<const Neuron*>& neurons);
    void visualizeFiringRateMaps(const std::vector<const Neuron*>& neurons, const glm::vec2& gridSize, int gridResolution);
    void visualizeCurrentActivity(const std::vector<const Neuron*>& neurons, const glm::vec3& position, float scale);
    void visualizeNetworkDynamics(const Brain& brain, double timeWindow);
    
    // Real-time visualization modes
    enum class ActivityMode {
        TIME_DOMAIN,
        FREQUENCY_DOMAIN,
        SPATIAL_DISTRIBUTION,
        TEMPORAL_SPIKES,
        CONNECTIVITY_MATRIX,
        PHASE_SPACE
    };
    
    void setActivityMode(ActivityMode mode);
    void toggleSpikeVisualization(bool show);
    void setSpikeSize(float size);
    void setFiringRateThreshold(float threshold);
    void setActivityColorScheme(const std::string& scheme);
    
    // Recording and playback
    void startRecording(const std::string& filename);
    void stopRecording();
    bool isRecording() const;
    void setPlaybackSpeed(float speed);
    float getPlaybackSpeed() const;
    
    // Time window for visualization
    void setTimeWindow(double seconds);
    double getTimeWindow() const;
    
    // Synchronization with brain simulation
    void setSimulationTime(double time);
    double getSimulationTime() const;
    
    // Event-based visualization
    void visualizeSpikeEvent(const SpikeEvent& spike, const Neuron& neuron);
    void visualizePlasticityEvent(const Neuron& pre, const Neuron& post, float weightChange);
    void visualizeSynapticActivity(const Synapse& synapse, float conductance);
    
    // Statistical visualization
    void visualizeActivityStatistics(const Brain& brain);
    void visualizeConnectivityMatrix(const Brain& brain, const glm::vec3& position, float size);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm