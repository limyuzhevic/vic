#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string outputFile;
    bool useTextOutput;
    bool saveToFile;
    time_t lastUpdateTime;
    int frameCount;
    
    Impl() : active(false), updateRate(30.0), useTextOutput(true), saveToFile(false), 
             lastUpdateTime(0), frameCount(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    pImpl->active = true;
    pImpl->lastUpdateTime = time(nullptr);
    pImpl->frameCount = 0;
    
    NLM_LOG_INFO("Initializing visualization interface...");
    
    // Set up text output if enabled
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("Text visualization enabled");
        if (!pImpl->outputFile.empty()) {
            NLM_LOG_INFO("Visualization output will be saved to: " + pImpl->outputFile);
        }
    }
    
    return true;
}

void VisualizationInterface::update() {
    pImpl->frameCount++;
    
    // Only update if enough time has passed
    time_t currentTime = time(nullptr);
    if (currentTime - pImpl->lastUpdateTime < static_cast<time_t>(1.0 / pImpl->updateRate)) {
        return;
    }
    
    pImpl->lastUpdateTime = currentTime;
    
    // Update visualization data
    updateVisualizationData();
}

void VisualizationInterface::render() {
    if (!pImpl->active) {
        return;
    }
    
    // Render based on the output mode
    if (pImpl->useTextOutput) {
        renderTextOutput();
    } else {
        // Future: OpenGL/3D rendering
        render3DOutput();
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("Visualization interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    if (hz > 0.0) {
        pImpl->updateRate = hz;
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setOutputMode(bool textOutput, const std::string& outputFile) {
    pImpl->useTextOutput = textOutput;
    pImpl->outputFile = outputFile;
    
    if (!outputFile.empty()) {
        pImpl->saveToFile = true;
        NLM_LOG_INFO("Visualization output will be saved to: " + outputFile);
    }
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Create network visualization
    std::string networkViz = createNetworkVisualization(brain);
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("[NETWORK] " + networkViz);
        if (pImpl->saveToFile) {
            saveToFile(networkViz, "network.txt");
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Create activity visualization
    std::string activityViz = createActivityVisualization(brain);
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("[ACTIVITY] " + activityViz);
        if (pImpl->saveToFile) {
            saveToFile(activityViz, "activity.txt");
        }
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Create spike raster visualization
    std::string spikeViz = createSpikeVisualization(spikes);
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("[SPIKES] " + spikeViz);
        if (pImpl->saveToFile) {
            saveToFile(spikeViz, "spikes.txt");
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Create weight matrix visualization
    std::string weightsViz = createWeightVisualization(brain);
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("[WEIGHTS] " + weightsViz);
        if (pImpl->saveToFile) {
            saveToFile(weightsViz, "weights.txt");
        }
    }
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    // Create development visualization
    std::string devViz = createDevelopmentVisualization(brain);
    
    if (pImpl->useTextOutput) {
        NLM_LOG_INFO("[DEVELOPMENT] " + devViz);
        if (pImpl->saveToFile) {
            saveToFile(devViz, "development.txt");
        }
    }
}

void VisualizationInterface::updateVisualizationData() {
    // Update any internal visualization data
    // This could include updating neural activity patterns,
    // weight changes, or other dynamic data
}

void VisualizationInterface::renderTextOutput() {
    // Render visualization data to console
    // This includes network structure, activity patterns, etc.
}

void VisualizationInterface::render3DOutput() {
    // Future: 3D OpenGL rendering
    // This would provide more sophisticated visualization capabilities
}

void VisualizationInterface::saveToFile(const std::string& content, const std::string& filename) {
    try {
        std::ofstream file(pImpl->outputFile + "/" + filename, std::ios::app);
        if (file.is_open()) {
            file << "[" << time(nullptr) << "] " << content << std::endl;
            file.close();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save visualization data: " + std::string(e.what()));
    }
}

std::string VisualizationInterface::createNetworkVisualization(const Brain& brain) {
    std::ostringstream oss;
    oss << "Network Structure:" << std::endl;
    oss << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
    oss << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
    oss << "  Regions: " << brain.getRegionCount() << std::endl;
    oss << "  Firing Rate: " << brain.getAverageFiringRate() * 100 << "%" << std::endl;
    
    // Add regional information
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto region = brain.getRegion(brain.getRegionIds()[i]);
        if (region) {
            oss << "  Region " << i+1 << ": " << region->getName() 
                << " (" << region->getAllNeurons().size() << " neurons)" << std::endl;
        }
    }
    
    return oss.str();
}

std::string VisualizationInterface::createActivityVisualization(const Brain& brain) {
    std::ostringstream oss;
    oss << "Neural Activity:" << std::endl;
    oss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    oss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    oss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    oss << "  Average Firing Rate: " << brain.getAverageFiringRate() * 100 << "%" << std::endl;
    
    // Add excitation/inhibition ratio
    float eiRatio = brain.getExcitationInhibitionRatio();
    oss << "  Excitation/Inhibition Ratio: " << eiRatio << ":1" << std::endl;
    
    // Add memory system status
    auto workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        oss << "  Working Memory Active Traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        oss << "  Episodic Memory Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    return oss.str();
}

std::string VisualizationInterface::createSpikeVisualization(const std::vector<SpikeEvent>& spikes) {
    std::ostringstream oss;
    oss << "Spike Activity:" << std::endl;
    oss << "  Total Spikes: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        // Count spikes by neuron
        std::map<NeuronId, int> spikeCounts;
        for (const auto& spike : spikes) {
            spikeCounts[spike.neuronId]++;
        }
        
        oss << "  Most Active Neurons:" << std::endl;
        int count = 0;
        for (const auto& pair : spikeCounts) {
            if (count >= 5) break; // Show top 5 neurons
            oss << "    Neuron " << pair.first.index() << ": " << pair.second << " spikes" << std::endl;
            count++;
        }
    }
    
    return oss.str();
}

std::string VisualizationInterface::createWeightVisualization(const Brain& brain) {
    std::ostringstream oss;
    oss << "Synaptic Weights:" << std::endl;
    oss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Calculate weight statistics
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::min();
    float totalWeight = 0.0f;
    int weightCount = 0;
    
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        auto region = brain.getRegion(brain.getRegionIds()[i]);
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                minWeight = std::min(minWeight, weight);
                maxWeight = std::max(maxWeight, weight);
                totalWeight += weight;
                weightCount++;
            }
        }
    }
    
    if (weightCount > 0) {
        oss << "  Weight Statistics:" << std::endl;
        oss << "    Min: " << minWeight << std::endl;
        oss << "    Max: " << maxWeight << std::endl;
        oss << "    Mean: " << totalWeight / weightCount << std::endl;
        oss << "    Std Dev: N/A (requires calculation)" << std::endl;
    }
    
    return oss.str();
}

std::string VisualizationInterface::createDevelopmentVisualization(const Brain& brain) {
    std::ostringstream oss;
    oss << "Development Status:" << std::endl;
    oss << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    oss << "  Synapse Count: " << brain.getTotalSynapseCount() << std::endl;
    oss << "  Neuron Count: " << brain.getTotalNeuronCount() << std::endl;
    
    // Add neuromodulation levels
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        oss << "  Dopamine Level: " << dopamine->getLevel() << std::endl;
    }
    
    auto curiosity = brain.getCuriosity();
    if (curiosity) {
        oss << "  Curiosity Level: " << curiosity->getLevel() << std::endl;
    }
    
    auto novelty = brain.getNovelty();
    if (novelty) {
        oss << "  Novelty Level: " << novelty->getLevel() << std::endl;
    }
    
    return oss.str();
}

} // namespace nlm
