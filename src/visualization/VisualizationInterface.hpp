#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Disable copying
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
    
    // Initialization and control
    bool initialize();
    void update();
    void render();
    void close();
    
    // Status
    bool isActive() const;
    
    // Configuration
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    void setSaveToFile(bool save);
    bool getSaveToFile() const;
    void setOutputDirectory(const std::string& directory);
    std::string getOutputDirectory() const;
    
    // Visualization methods
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    void visualizeDevelopment(const Brain& brain);
    void saveAll(const Brain& brain, const std::vector<SpikeEvent>& spikes);
    
    // Static helper methods
    static std::string getTimestamp();
    static double getMinSpikeTime(const std::vector<SpikeEvent>& spikes);
    static double getMaxSpikeTime(const std::vector<SpikeEvent>& spikes);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
