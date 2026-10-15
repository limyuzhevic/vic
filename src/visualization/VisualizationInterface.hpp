#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

// Visualization data structures for real-time brain monitoring
struct NeuralActivityData {
    std::vector<float> membranePotentials;
    std::vector<bool> isFiring;
    std::vector<uint64_t> neuronIds;
    std::vector<uint64_t> regionIds;
    std::vector<std::string> neuronTypes;
    double timestamp;
    
    NeuralActivityData()
        : membranePotentials(), isFiring(), neuronIds(), regionIds()
        , neuronTypes(), timestamp(0.0) {}
};

struct SynapseData {
    uint64_t sourceNeuronId;
    uint64_t targetNeuronId;
    float weight;
    bool isActive;
    double lastUpdateTime;
    
    SynapseData()
        : sourceNeuronId(0), targetNeuronId(0), weight(0.0f)
        , isActive(false), lastUpdateTime(0.0) {}
};

struct SpikeData {
    uint64_t neuronId;
    double timestamp;
    uint64_t step;
    uint64_t regionId;
    std::string neuronType;
    
    SpikeData()
        : neuronId(0), timestamp(0.0), step(0), regionId(0) {}
};

// Visualization interface for real-time brain activity monitoring
// WebSocket based for real-time updates
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization server
    bool initialize(int port = 8080);
    
    // Update visualization data
    void update();
    
    // Start visualization server
    void start();
    
    // Stop visualization server
    void stop();
    
    // Check if visualization is running
    bool isRunning() const;
    
    // Set update rate for real-time data
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Add brain for visualization
    void addBrain(const std::string& brainId, const class Brain& brain);
    
    // Remove brain from visualization
    void removeBrain(const std::string& brainId);
    
    // Get latest neural activity data
    NeuralActivityData getNeuralActivity(const std::string& brainId) const;
    
    // Get latest synapse data
    std::vector<SynapseData> getSynapseData(const std::string& brainId) const;
    
    // Get spike data
    std::vector<SpikeData> getSpikeData(const std::string& brainId, double timeWindowSeconds = 10.0) const;
    
    // Get brain statistics
    std::unordered_map<std::string, std::string> getBrainStats(const std::string& brainId) const;
    
    // Send data to all connected clients
    void broadcastData();
    
    // Handle WebSocket connection
    void handleConnection(int clientId);
    
    // Handle WebSocket message
    void handleMessage(int clientId, const std::string& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
