#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string logFile;
    std::ofstream logStream;
    time_t startTime;
    
    Impl() : active(false), updateRate(30.0), logFile("nlm_visualization.log"), startTime(0) {
        std::time(&startTime);
    }
    
    ~Impl() {
        if (logStream.is_open()) {
            logStream.close();
        }
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const std::string& logFile) {
    pImpl->logFile = logFile;
    pImpl->logStream.open(logFile, std::ios::out | std::ios::trunc);
    
    if (pImpl->logStream.is_open()) {
        pImpl->logStream << "=== NLM Visualization Log ===" << std::endl;
        pImpl->logStream << "Start time: " << std::ctime(&pImpl->startTime);
        pImpl->logStream << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
        pImpl->active = true;
        return true;
    }
    return false;
}

void VisualizationInterface::update() {
    if (pImpl->active && pImpl->logStream.is_open()) {
        // Log timestamp
        time_t now = std::time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        pImpl->logStream << "[" << timeStr << "] Updating visualization..." << std::endl;
    }
}

void VisualizationInterface::render() {
    if (pImpl->active && pImpl->logStream.is_open()) {
        pImpl->logStream << "\n=== Current Brain State ===" << std::endl;
    }
}

void VisualizationInterface::close() {
    if (pImpl->logStream.is_open()) {
        pImpl->logStream << "\n=== Visualization Session Ended ===" << std::endl;
        pImpl->logStream.close();
    }
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::logStatistics(const Brain& brain, const std::string& label) {
    if (!pImpl->active || !pImpl->logStream.is_open()) return;
    
    std::stringstream ss;
    ss << "=== " << (label.empty() ? "Brain Statistics" : label) << " ===" << std::endl;
    ss << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
       << brain.getAverageFiringRate() << " Hz" << std::endl;
    ss << "Regions: " << brain.getRegionCount() << std::endl;
    ss << "Step: " << brain.getConfig()->getOr<size_t>("current_step", 0) << std::endl;
    
    pImpl->logStream << ss.str() << std::endl;
}

void VisualizationInterface::logNetwork(const Brain& brain) {
    if (!pImpl->active || !pImpl->logStream.is_open()) return;
    
    pImpl->logStream << "Network visualization:" << std::endl;
    pImpl->logStream << visualizeNetworkASCII(brain) << std::endl;
}

void VisualizationInterface::logActivity(const Brain& brain) {
    if (!pImpl->active || !pImpl->logStream.is_open()) return;
    
    pImpl->logStream << "Activity visualization:" << std::endl;
    pImpl->logStream << visualizeActivityASCII(brain) << std::endl;
}

void VisualizationInterface::logSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active || !pImpl->logStream.is_open()) return;
    
    pImpl->logStream << "Spike events (" << spikes.size() << "): " << std::endl;
    for (size_t i = 0; i < std::min(spikes.size(), size_t(10)); ++i) {
        const auto& spike = spikes[i];
        pImpl->logStream << "  Neuron " << spike.source_neuron.index() 
                        << " at time " << spike.timestamp << "s" << std::endl;
    }
    if (spikes.size() > 10) {
        pImpl->logStream << "  ... and " << (spikes.size() - 10) << " more" << std::endl;
    }
    pImpl->logStream << std::endl;
}

void VisualizationInterface::logWeights(const Brain& brain) {
    if (!pImpl->active || !pImpl->logStream.is_open()) return;
    
    pImpl->logStream << "Weight matrix summary:" << std::endl;
    // Simplified weight logging - in practice would be more detailed
    pImpl->logStream << "  Excitatory synapses: Need implementation" << std::endl;
    pImpl->logStream << "  Inhibitory synapses: Need implementation" << std::endl;
    pImpl->logStream << "  Modulatory synapses: Need implementation" << std::endl;
    pImpl->logStream << std::endl;
}

std::string VisualizationInterface::visualizeNetworkASCII(const Brain& brain) const {
    std::stringstream ss;
    
    ss << "Network Structure:" << std::endl;
    ss << "=================" << std::endl;
    ss << "Regions: " << brain.getRegionCount() << std::endl;
    ss << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Show region details
    auto regionIds = brain.getRegionIds();
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto region = brain.getRegion(regionIds[i]);
        if (region) {
            ss << "  Region " << regionIds[i].index() 
               << ": " << region->getPopulations().size() << " populations, "
               << region->getTotalNeuronCount() << " neurons" << std::endl;
        }
    }
    
    return ss.str();
}

std::string VisualizationInterface::visualizeActivityASCII(const Brain& brain) const {
    std::stringstream ss;
    
    ss << "Neural Activity Summary:" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "Firing Rate (avg): " << std::fixed << std::setprecision(2) 
       << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    // Add some firing rate distribution info
    float firingRate = brain.getAverageFiringRate();
    ss << "Activity Level: ";
    if (firingRate < 1.0f) ss << "Low";
    else if (firingRate < 10.0f) ss << "Moderate";
    else ss << "High";
    ss << std::endl;
    
    return ss.str();
}

} // namespace nlm
