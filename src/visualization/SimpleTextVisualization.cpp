// SimpleTextVisualization Implementation

#include "SimpleTextVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <thread>

namespace nlm {

struct SimpleTextVisualization::Impl {
    Brain* brain;
    std::atomic<bool> running;
    std::thread updateThread;
    std::chrono::steady_clock::time_point lastUpdate;
    float updateRate;
    
    // Visualization state
    std::string mode;
    float neuronSize;
    float synapseWidth;
    float activityThreshold;
    bool showSpikes;
    bool showConnections;
    bool showStatistics;
    bool showMemory;
    std::string exportFormat;
    std::string exportPath;
    
    Impl() : brain(nullptr), running(false), updateRate(0.0f),
             neuronSize(1.0f), synapseWidth(0.5f), activityThreshold(0.1f),
             showSpikes(true), showConnections(true), showStatistics(true),
             showMemory(false), exportFormat("csv"), exportPath(".") {}
};

SimpleTextVisualization::SimpleTextVisualization()
    : pImpl(new Impl)
{
}

SimpleTextVisualization::~SimpleTextVisualization() {
    stop();
}

void SimpleTextVisualization::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Simple text visualization initialized");
}

void SimpleTextVisualization::start() {
    if (pImpl->running) return;
    
    pImpl->running = true;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    pImpl->updateThread = std::thread(&SimpleTextVisualization::updateThreadFunc, this);
    NLM_LOG_INFO("Simple text visualization started");
}

void SimpleTextVisualization::stop() {
    if (!pImpl->running) return;
    
    pImpl->running = false;
    if (pImpl->updateThread.joinable()) {
        pImpl->updateThread.join();
    }
    NLM_LOG_INFO("Simple text visualization stopped");
}

void SimpleTextVisualization::update(const Brain* brain, SimulationStep step, Timestamp time) {
    std::lock_guard<std::mutex> lock(updateMutex_);
    updateThreadFunc();
}

void SimpleTextVisualization::setMode(const std::string& mode) {
    pImpl->mode = mode;
}

void SimpleTextVisualization::addCallback(std::function<void(const Brain* brain)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    callbacks_.push_back(callback);
}

void SimpleTextVisualization::setUpdateInterval(int interval) {
    pImpl->updateRate = 1000.0f / static_cast<float>(interval);
}

bool SimpleTextVisualization::saveToFile(const std::string& filepath) {
    if (pImpl->brain) {
        switch (pImpl->exportFormat[0]) {
            case 'c': return exportToCSV(filepath, pImpl->brain); break;
            case 'j': return exportToJSON(filepath, pImpl->brain); break;
            case 't': return exportToText(filepath, pImpl->brain); break;
            default: return exportToCSV(filepath, pImpl->brain); break;
        }
    }
    return false;
}

bool SimpleTextVisualization::exportBrainState(const std::string& filepath) {
    if (pImpl->brain) {
        return pImpl->brain->save(filepath);
    }
    return false;
}

bool SimpleTextVisualization::isRunning() const {
    return pImpl->running;
}

std::string SimpleTextVisualization::getCurrentMode() const {
    return pImpl->mode;
}

size_t SimpleTextVisualization::getFrameCount() const {
    return 0; // Simple visualization doesn't record frames
}

float SimpleTextVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

void SimpleTextVisualization::setNeuronSize(float size) {
    pImpl->neuronSize = size;
}

void SimpleTextVisualization::setSynapseWidth(float width) {
    pImpl->synapseWidth = width;
}

void SimpleTextVisualization::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = threshold;
}

void SimpleTextVisualization::enableSpikes(bool enable) {
    pImpl->showSpikes = enable;
}

void SimpleTextVisualization::enableConnections(bool enable) {
    pImpl->showConnections = enable;
}

void SimpleTextVisualization::setShowNeurons(bool show) {
    pImpl->showStatistics = show;
}

void SimpleTextVisualization::setShowConnections(bool show) {
    pImpl->showConnections = show;
}

void SimpleTextVisualization::setShowStatistics(bool show) {
    pImpl->showStatistics = show;
}

void SimpleTextVisualization::setShowMemory(bool show) {
    pImpl->showMemory = show;
}

void SimpleTextVisualization::setExportFormat(const std::string& format) {
    pImpl->exportFormat = format;
}

void SimpleTextVisualization::setExportPath(const std::string& path) {
    pImpl->exportPath = path;
}

void SimpleTextVisualization::updateThreadFunc() {
    const float targetInterval = 1000.0f / pImpl->updateRate;
    
    while (pImpl->running) {
        auto startTime = std::chrono::steady_clock::now();
        
        // Clear screen (ANSI escape codes)
        std::cout << "\033[2J\033[1;1H";  // Clear screen and move cursor to top-left
        
        // Draw visualization
        if (pImpl->brain) {
            drawTextNeurons(pImpl->brain);
            drawTextConnections(pImpl->brain);
            drawTextStatistics(pImpl->brain);
            if (pImpl->showMemory) {
                drawTextMemory(pImpl->brain);
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        if (elapsed < targetInterval) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetInterval - elapsed)));
        }
    }
}

void SimpleTextVisualization::drawTextNeurons(const Brain* brain) {
    std::cout << "=== NLM Brain Visualization ===" << std::endl;
    std::cout << "Simulation Step: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Time: " << static_cast<float>(brain->getTotalSpikeCount()) * 0.001f << "s" << std::endl;
    
    std::cout << "\n=== Regions ===" << std::endl;
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        const auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            std::cout << "Region " << i + 1 << ": " << region->getName() 
                     << " (" << region->getTotalNeuronCount() << " neurons, "
                     << region->getSynapseCount() << " synapses)" << std::endl;
        }
    }
    
    std::cout << "\n=== Neurons ===" << std::endl;
    std::cout << "Total: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Active: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    std::cout << "\n=== Synapses ===" << std::endl;
    std::cout << "Total: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    std::cout << "\n=== Memory Systems ===" << std::endl;
    auto workingMem = brain->getWorkingMemory();
    if (workingMem) {
        std::cout << "Working Memory: " << workingMem->getActiveTraces() << " traces" << std::endl;
    }
    
    auto episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        std::cout << "Episodic Memory: " << episodicMem->getEpisodeCount() << " episodes" << std::endl;
    }
}

void SimpleTextVisualization::drawTextConnections(const Brain* brain) {
    if (!pImpl->showConnections) return;
    
    std::cout << "\n=== Neural Connections ===" << std::endl;
    for (const auto& region : brain->getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            if (std::abs(synapse->getWeight()) > pImpl->activityThreshold) {
                std::cout << "  Synapse: " << synapse->getSourceNeuron().index() << " -> "
                         << synapse->getDestinationNeuron().index()
                         << " (weight: " << std::fixed << std::setprecision(3)
                         << synapse->getWeight() << ")" << std::endl;
            }
        }
    }
}

void SimpleTextVisualization::drawTextStatistics(const Brain* brain) {
    if (!pImpl->showStatistics) return;
    
    std::cout << "\n=== Statistical Summary ===" << std::endl;
    std::cout << "Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
    
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "Dopamine Level: " << dopamine->getLevel() << std::endl;
    }
    
    auto curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity Level: " << curiosity->getLevel() << std::endl;
    }
}

void SimpleTextVisualization::drawTextMemory(const Brain* brain) {
    std::cout << "\n=== Memory Status ===" << std::endl;
    
    auto workingMem = brain->getWorkingMemory();
    if (workingMem) {
        std::cout << "Working Memory Capacity: " << workingMem->getCapacity() << std::endl;
        std::cout << "Working Memory Traces: " << workingMem->getActiveTraces() << std::endl;
        std::cout << "Working Memory Similarity: " << workingMem->getSimilarity() << std::endl;
    }
    
    auto episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        std::cout << "Episodic Memory Max Episodes: " << episodicMem->getMaxEpisodes() << std::endl;
        std::cout << "Episodic Memory Current Episodes: " << episodicMem->getEpisodeCount() << std::endl;
        std::cout << "Episodic Memory Replay Interval: " << episodicMem->getReplayInterval() << std::endl;
    }
    
    auto associativeMem = brain->getAssociativeMemory();
    if (associativeMem) {
        std::cout << "Associative Memory Similarity Threshold: " << associativeMem->getSimilarityThreshold() << std::endl;
        std::cout << "Associative Memory Activation Threshold: " << associativeMem->getActivationThreshold() << std::endl;
    }
}

void SimpleTextVisualization::exportToCSV(const std::string& filepath, const Brain* brain) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for CSV export: " + filepath);
        return;
    }
    
    file << "Simulation Step,Time,Total Neurons,Total Synapses,Active Neurons,Firing Neurons,Avg Firing Rate,E/I Ratio,Development Stage" << std::endl;
    
    file << brain->getTotalSpikeCount() << ","
         << static_cast<float>(brain->getTotalSpikeCount()) * 0.001f << ","
         << brain->getTotalNeuronCount() << ","
         << brain->getTotalSynapseCount() << ","
         << brain->getActiveNeuronCount() << ","
         << brain->getFiringNeuronCount() << ","
         << brain->getAverageFiringRate() << ","
         << brain->getExcitationInhibitionRatio() << ","
         << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Exported brain state to CSV: " + filepath);
}

void SimpleTextVisualization::exportToJSON(const std::string& filepath, const Brain* brain) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON export: " + filepath);
        return;
    }
    
    file << "{" << std::endl;
    file << "  \"simulationStep\": " << brain->getTotalSpikeCount() << "," << std::endl;
    file << "  \"time\": " << static_cast<float>(brain->getTotalSpikeCount()) * 0.001f << "," << std::endl;
    file << "  \"totalNeurons\": " << brain->getTotalNeuronCount() << "," << std::endl;
    file << "  \"totalSynapses\": " << brain->getTotalSynapseCount() << "," << std::endl;
    file << "  \"activeNeurons\": " << brain->getActiveNeuronCount() << "," << std::endl;
    file << "  \"firingNeurons\": " << brain->getFiringNeuronCount() << "," << std::endl;
    file << "  \"averageFiringRate\": " << brain->getAverageFiringRate() << "," << std::endl;
    file << "  \"excitationInhibitionRatio\": " << brain->getExcitationInhibitionRatio() << "," << std::endl;
    file << "  \"developmentStage\": " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    file << "}" << std::endl;
    
    file.close();
    NLM_LOG_INFO("Exported brain state to JSON: " + filepath);
}

void SimpleTextVisualization::exportToText(const std::string& filepath, const Brain* brain) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for text export: " + filepath);
        return;
    }
    
    file << "NLM Brain State Export" << std::endl;
    file << "=======================" << std::endl << std::endl;
    
    file << "Simulation Step: " << brain->getTotalSpikeCount() << std::endl;
    file << "Time: " << static_cast<float>(brain->getTotalSpikeCount()) * 0.001f << "s" << std::endl;
    file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    file << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    file << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    file << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    file << "Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl << std::endl;
    
    file << "Regions:" << std::endl;
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        const auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            file << "  " << region->getName() << ": "
                 << region->getTotalNeuronCount() << " neurons, "
                 << region->getSynapseCount() << " synapses" << std::endl;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Exported brain state to text: " + filepath);
}

} // namespace nlm