#include "DebugInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

DebugInterface::DebugInterface() : brain_(nullptr), verbose_(false) {
    NLM_LOG_INFO("DebugInterface created");
}

DebugInterface::~DebugInterface() = default;

void DebugInterface::setBrain(Brain* brain) {
    brain_ = brain;
    if (brain_) {
        NLM_LOG_INFO("DebugInterface connected to brain");
    }
}

void DebugInterface::setVerbose(bool verbose) {
    verbose_ = verbose;
    if (verbose) {
        NLM_LOG_INFO("DebugInterface set to verbose mode");
    }
}

std::string DebugInterface::getSystemStatus() const {
    if (!brain_) {
        return "No brain connected";
    }
    
    std::ostringstream status;
    status << "=== NLM Brain System Status ===\n";
    
    // Basic brain statistics
    status << "Neurons: " << brain_->getTotalNeuronCount() << "\n";
    status << "Synapses: " << brain_->getTotalSynapseCount() << "\n";
    status << "Regions: " << brain_->getRegionCount() << "\n";
    status << "Total spikes: " << brain_->getTotalSpikeCount() << "\n";
    status << "Firing neurons (this step): " << brain_->getFiringNeuronCount() << "\n";
    status << "Average firing rate: " << std::fixed << std::setprecision(2) << brain_->getAverageFiringRate() << " Hz\n";
    status << "E/I ratio: " << std::fixed << std::setprecision(3) << brain_->getExcitationInhibitionRatio() << "\n";
    
    // Memory systems status
    status << "\n=== Memory Systems ===\n";
    if (brain_->getWorkingMemory()) {
        status << "Working Memory: " << brain_->getWorkingMemory()->getActiveTraces() << " active traces\n";
    }
    if (brain_->getEpisodicMemory()) {
        status << "Episodic Memory: " << brain_->getEpisodicMemory()->getEpisodeCount() << " episodes\n";
    }
    if (brain_->getAssociativeMemory()) {
        status << "Associative Memory: " << brain_->getAssociativeMemory()->getAssociationCount() << " associations\n";
    }
    
    // Neuromodulation status
    status << "\n=== Neuromodulation ===\n";
    if (brain_->getDopamine()) {
        status << "Dopamine: " << std::fixed << std::setprecision(3) << brain_->getDopamine()->getLevel() << "\n";
    }
    if (brain_->getCuriosity()) {
        status << "Curiosity: " << std::fixed << std::setprecision(3) << brain_->getCuriosity()->getLevel() << "\n";
    }
    if (brain_->getNovelty()) {
        status << "Novelty: " << std::fixed << std::setprecision(3) << brain_->getNovelty()->getLevel() << "\n";
    }
    
    // Development status
    status << "\n=== Development ===\n";
    DevelopmentalStage stage = brain_->getDevelopmentalStage();
    status << "Developmental stage: " << static_cast<int>(stage) << "\n";
    
    // Region details
    status << "\n=== Region Details ===\n";
    for (const auto& regionId : brain_->getRegionIds()) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            status << "Region " << regionId.index() << " (" << region->getName() << "):\n";
            status << "  Neurons: " << region->getTotalNeuronCount() << "\n";
            status << "  Synapses: " << region->getSynapseCount() << "\n";
            status << "  Active neurons: " << region->getActiveNeuronCount() << "\n";
            status << "  Average synaptic weight: " << std::fixed << std::setprecision(4) << region->getAverageSynapticWeight() << "\n";
        }
    }
    
    // Plasticity rules status
    status << "\n=== Plasticity Rules ===\n";
    if (brain_->getSTDP()) {
        status << "STDP: enabled (LTP weight: " << std::fixed << std::setprecision(4) << brain_->getSTDP()->getLTPWeight() << ")\n";
    }
    if (brain_->getHebbian()) {
        status << "Hebbian: enabled (learning rate: " << std::fixed << std::setprecision(4) << brain_->getHebbian()->getLearningRate() << ")\n";
    }
    if (brain_->getStructuralPlasticity()) {
        status << "Structural Plasticity: enabled\n";
    }
    
    return status.str();
}

std::string DebugInterface::getConnectivityReport() const {
    if (!brain_) {
        return "No brain connected";
    }
    
    std::ostringstream report;
    report << "=== Neural Connectivity Report ===\n";
    
    size_t totalConnections = 0;
    size_t strongConnections = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            totalConnections++;
            if (std::abs(syn->getWeight()) > 0.5f) {
                strongConnections++;
            }
        }
    }
    
    report << "Total connections: " << totalConnections << "\n";
    report << "Strong connections (>0.5): " << strongConnections << " (" 
           << (totalConnections > 0 ? (strongConnections * 100 / totalConnections) : 0) << "%\n";
    
    // Find most connected neurons
    std::unordered_map<NeuronId, size_t> neuronConnectionCount;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            neuronConnectionCount[syn->getDestinationNeuron()]++;
            neuronConnectionCount[syn->getSourceNeuron()]++;
        }
    }
    
    report << "\nMost connected neurons:\n";
    size_t count = 0;
    for (const auto& pair : neuronConnectionCount) {
        if (count >= 10) break;
        report << "  Neuron " << pair.first.index() << ": " << pair.second << " connections\n";
        count++;
    }
    
    return report.str();
}

std::string DebugInterface::getActivityReport() const {
    if (!brain_) {
        return "No brain connected";
    }
    
    std::ostringstream report;
    report << "=== Neural Activity Report ===\n";
    
    // Current step activity
    report << "Activity this step:\n";
    report << "  Total spikes: " << brain_->getTotalSpikeCount() << "\n";
    report << "  Firing neurons: " << brain_->getFiringNeuronCount() << "\n";
    report << "  Pending spike events: " << brain_->getPendingSpikeEventCount() << "\n";
    
    // Regional activity
    report << "\nRegional activity:\n";
    for (const auto& regionId : brain_->getRegionIds()) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            report << "  Region " << regionId.index() << ": " << region->getActiveNeuronCount() 
                   << " active neurons (" << region->getTotalNeuronCount() << " total)\n";
        }
    }
    
    // Neuron type distribution
    std::unordered_map<NeuronType, size_t> typeCount;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            typeCount[pop->getNeuronType()] += pop->getNeuronCount();
        }
    }
    
    report << "\nNeuron type distribution:\n";
    for (const auto& pair : typeCount) {
        std::string typeStr;
        switch (pair.first) {
            case NeuronType::Sensory: typeStr = "Sensory"; break;
            case NeuronType::Internal: typeStr = "Internal"; break;
            case NeuronType::Motor: typeStr = "Motor"; break;
            default: typeStr = "Unknown"; break;
        }
        report << "  " << typeStr << ": " << pair.second << "\n";
    }
    
    return report.str();
}

std::string DebugInterface::getPlasticityReport() const {
    if (!brain_) {
        return "No brain connected";
    }
    
    std::ostringstream report;
    report << "=== Plasticity Report ===\n";
    
    // STDP parameters
    if (brain_->getSTDP()) {
        report << "STDP Parameters:\n";
        report << "  LTP weight: " << std::fixed << std::setprecision(4) << brain_->getSTDP()->getLTPWeight() << "\n";
        report << "  LTD weight: " << std::fixed << std::setprecision(4) << brain_->getSTDP()->getLTDWeight() << "\n";
        report << "  Time constant: " << brain_->getSTDP()->getTimeConstant() << " ms\n";
    }
    
    // Hebbian parameters
    if (brain_->getHebbian()) {
        report << "Hebbian Parameters:\n";
        report << "  Learning rate: " << std::fixed << std::setprecision(4) << brain_->getHebbian()->getLearningRate() << "\n";
        report << "  Max weight: " << brain_->getHebbian()->getMaxWeight() << "\n";
    }
    
    // Structural plasticity
    if (brain_->getStructuralPlasticity()) {
        report << "Structural Plasticity: enabled\n";
        // Note: StructuralPlasticity doesn't have getters for rate parameters
    }
    
    return report.str();
}

std::string DebugInterface::getDebugInfo() const {
    if (!brain_) {
        return "No brain connected";
    }
    
    std::ostringstream info;
    info << "=== Debug Information ===\n";
    info << "Brain connected at: " << (verbose_ ? "YES" : "NO - enable verbose for more details") << "\n";
    
    if (verbose_) {
        info << "\nDetailed Debug Info:\n";
        info << "Step: " << brain_->getTotalSpikeCount() << " total spikes\n";
        info << "Time: " << brain_->getConfig()->getOr<double>("simulation_timestep", 0.001) << "s per step\n";
        
        // Memory system details
        if (brain_->getWorkingMemory()) {
            info << "Working Memory capacity: " << brain_->getWorkingMemory()->getCapacity() << "\n";
        }
        if (brain_->getEpisodicMemory()) {
            info << "Episodic Memory max episodes: " << brain_->getEpisodicMemory()->getMaxEpisodes() << "\n";
        }
        
        // Neuromodulator details
        if (brain_->getDopamine()) {
            info << "Dopamine plasticity factor: " << brain_->getDopamine()->getPlasticityFactor() << "\n";
        }
        
        // Region connectivity
        info << "Region connectivity:\n";
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                info << "  Region " << regionId.index() << ": " << region->getSynapseCount() << " synapses\n";
            }
        }
    }
    
    return info.str();
}

void DebugInterface::saveDebugReport(const std::string& filename) const {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot save debug report: no brain connected");
        return;
    }
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open debug report file: " + filename);
            return;
        }
        
        file << "NLM Debug Report" << std::endl;
        file << "Generated at: " << getCurrentTimestamp() << std::endl;
        file << "=" << std::string(50, '=') << std::endl;
        
        file << getSystemStatus() << std::endl;
        file << getConnectivityReport() << std::endl;
        file << getActivityReport() << std::endl;
        file << getPlasticityReport() << std::endl;
        
        file.close();
        NLM_LOG_INFO("Debug report saved to: " + filename);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving debug report: " + std::string(e.what()));
    }
}

std::string DebugInterface::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(&time, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm