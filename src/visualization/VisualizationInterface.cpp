#include "VisualizationInterface.hpp"
#include <algorithm>
#include <map>
#include <sstream>
#include <iomanip>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    double lastUpdateTime;
    
    // Enhanced monitoring data
    std::vector<double> activityHistory;
    std::vector<double> firingRateHistory;
    std::vector<double> weightHistory;
    
    // Real-time metrics
    double currentActivityLevel;
    double currentFiringRate;
    double currentAverageWeight;
    
    // Update intervals
    size_t historySize;
    size_t updateInterval;
    
    // Visualization state
    bool showNetworkTopology;
    bool showActivityPattern;
    bool showSpikePatterns;
    bool showWeightDynamics;
    bool showMemorySystems;
    bool showNeuromodulation;
    bool showDevelopment;
    
    // Console formatting
    int consoleWidth;
    bool useColorOutput;
    
    Impl() 
        : active(false)
        , updateRate(30.0)
        , lastUpdateTime(0.0)
        , currentActivityLevel(0.0)
        , currentFiringRate(0.0)
        , currentAverageWeight(0.0)
        , historySize(100)
        , updateInterval(5)
        , showNetworkTopology(true)
        , showActivityPattern(true)
        , showSpikePatterns(false)
        , showWeightDynamics(true)
        , showMemorySystems(true)
        , showNeuromodulation(true)
        , showDevelopment(true)
        , consoleWidth(80)
        , useColorOutput(true)
    {
        activityHistory.reserve(historySize);
        firingRateHistory.reserve(historySize);
        weightHistory.reserve(historySize);
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization with enhanced capabilities
    pImpl->active = true;
    
    NLM_LOG_INFO("VisualizationInterface: Enhanced initialization complete");
    NLM_LOG_INFO("  Features enabled:");
    NLM_LOG_INFO("    - Real-time activity monitoring");
    NLM_LOG_INFO("    - Network topology visualization");
    NLM_LOG_INFO("    - Weight dynamics tracking");
    NLM_LOG_INFO("    - Memory system monitoring");
    NLM_LOG_INFO("    - Neuromodulation visualization");
    NLM_LOG_INFO("    - Development stage monitoring");
    
    return true;
}

void VisualizationInterface::update(const Brain& brain) {
    if (!pImpl->active) return;
    
    // Update real-time metrics
    pImpl->currentActivityLevel = brain.getActiveNeuronCount() / 
                                  std::max<float>(brain.getTotalNeuronCount(), 1);
    pImpl->currentFiringRate = brain.getAverageFiringRate();
    
    // Calculate average weight
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
            synapseCount++;
        }
    }
    pImpl->currentAverageWeight = synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
    
    // Update history
    pImpl->activityHistory.push_back(pImpl->currentActivityLevel);
    pImpl->firingRateHistory.push_back(pImpl->currentFiringRate);
    pImpl->weightHistory.push_back(pImpl->currentAverageWeight);
    
    // Maintain history size
    if (pImpl->activityHistory.size() > pImpl->historySize) {
        pImpl->activityHistory.erase(pImpl->activityHistory.begin());
    }
    if (pImpl->firingRateHistory.size() > pImpl->historySize) {
        pImpl->firingRateHistory.erase(pImpl->firingRateHistory.begin());
    }
    if (pImpl->weightHistory.size() > pImpl->historySize) {
        pImpl->weightHistory.erase(pImpl->weightHistory.begin());
    }
    
    // Log updates at specified intervals
    static size_t updateCount = 0;
    if (++updateCount % pImpl->updateInterval == 0) {
        logRealTimeStatus(brain);
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // For console-based visualization, we'll print comprehensive status
    printConsoleVisualization();
}

void VisualizationInterface::close() {
    pImpl->active = false;
    NLM_LOG_INFO("VisualizationInterface: Enhanced shutdown complete");
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

void VisualizationInterface::setHistorySize(size_t size) {
    pImpl->historySize = size;
}

void VisualizationInterface::setUpdateInterval(size_t interval) {
    pImpl->updateInterval = interval;
}

void VisualizationInterface::setVisualizationOption(const std::string& option, bool enabled) {
    if (option == "network") pImpl->showNetworkTopology = enabled;
    else if (option == "activity") pImpl->showActivityPattern = enabled;
    else if (option == "spikes") pImpl->showSpikePatterns = enabled;
    else if (option == "weights") pImpl->showWeightDynamics = enabled;
    else if (option == "memory") pImpl->showMemorySystems = enabled;
    else if (option == "neuromodulation") pImpl->showNeuromodulation = enabled;
    else if (option == "development") pImpl->showDevelopment = enabled;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->showNetworkTopology) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                    NETWORK TOPOLOGY VISUALIZATION                  ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    ss << "║ Regions: " << std::setw(3) << brain.getRegionCount() << " | Neurons: " << std::setw(5) << brain.getTotalNeuronCount() 
       << " | Synapses: " << std::setw(6) << brain.getTotalSynapseCount() << "║" << std::endl;
    
    // Color-coded region display
    for (const auto& region : brain.getRegions()) {
        ss << "║   Region " << std::setw(2) << region->getId().index() << ": " << std::left << std::setw(25) 
           << region->getName() << " │ Neurons: " << std::setw(3) << region->getTotalNeuronCount() 
           << " │ Synapses: " << std::setw(4) << region->getSynapseCount() << " ║" << std::endl;
    }
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->showActivityPattern) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                  ACTIVITY PATTERN MONITORING                    ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    double activityLevel = pImpl->currentActivityLevel;
    double firingRate = pImpl->currentFiringRate;
    
    ss << "║ Activity Level:  " << std::setw(6) << std::fixed << std::setprecision(3) << (activityLevel * 100) << "%  │ "
       << std::setw(6) << brain.getActiveNeuronCount() << " neurons ║" << std::endl;
    ss << "║ Firing Rate:     " << std::setw(6) << std::fixed << std::setprecision(3) << firingRate << " Hz  │ "
       << std::setw(6) << brain.getFiringNeuronCount() << " firing ║" << std::endl;
    ss << "║ E/I Ratio:       " << std::setw(6) << std::fixed << std::setprecision(3) << brain.getExcitationInhibitionRatio() << "     │ "
       << std::setw(6) << brain.getPendingSpikeEventCount() << " pending ║" << std::endl;
    
    // Visual activity bar
    ss << "║ ["; 
    size_t barWidth = 30;
    size_t filledBars = static_cast<size_t>(activityLevel * barWidth);
    for (size_t i = 0; i < barWidth; ++i) {
        if (i < filledBars) {
            ss << "█";
        } else {
            ss << "░";
        }
    }
    ss << "] " << std::setw(3) << static_cast<int>(activityLevel * 100) << "%║" << std::endl;
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->showSpikePatterns) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                     SPIKE PATTERN ANALYSIS                     ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (spikes.empty()) {
        ss << "║ No spikes recorded                                         ║" << std::endl;
    } else {
        // Analyze spike patterns
        std::map<NeuronId, int> spikeCounts;
        std::vector<double> spikeTimes;
        
        for (const auto& spike : spikes) {
            spikeCounts[spike.neuronId]++;
            spikeTimes.push_back(spike.time);
        }
        
        ss << "║ Total spikes: " << std::setw(5) << spikes.size() << "  │ "
           << std::setw(5) << spikeCounts.size() << " neurons affected ║" << std::endl;
        
        if (!spikeTimes.empty()) {
            double minTime = *std::min_element(spikeTimes.begin(), spikeTimes.end());
            double maxTime = *std::max_element(spikeTimes.begin(), spikeTimes.end());
            ss << "║ Time span:    " << std::setw(5) << std::fixed << std::setprecision(3) << (maxTime - minTime) 
               << " s  │ Rate: " << std::setw(8) << std::fixed << std::setprecision(3) << (spikes.size() / (maxTime - minTime + 0.001)) 
               << " Hz ║" << std::endl;
        }
        
        // Find most active neuron
        if (!spikeCounts.empty()) {
            auto mostActive = std::max_element(spikeCounts.begin(), spikeCounts.end(),
                                             [](const auto& a, const auto& b) { return a.second < b.second; });
            ss << "║ Most active:  Neuron " << std::setw(5) << mostActive->first 
               << " (" << std::setw(5) << mostActive->second << " spikes)  ║" << std::endl;
        }
    }
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->showWeightDynamics) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                  WEIGHT DYNAMICS MONITORING                    ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    double avgWeight = pImpl->currentAverageWeight;
    
    ss << "║ Average weight: " << std::setw(8) << std::fixed << std::setprecision(4) << avgWeight << "     │ "
       << std::setw(6) << brain.getTotalSynapseCount() << " synapses ║" << std::endl;
    
    // Calculate weight statistics
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float weight = syn->getWeight();
            minWeight = std::min(minWeight, weight);
            maxWeight = std::max(maxWeight, weight);
            totalWeight += weight;
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        double stdDev = 0.0;
        if (pImpl->weightHistory.size() > 1) {
            double mean = pImpl->currentAverageWeight;
            double sumSq = 0.0;
            for (double w : pImpl->weightHistory) {
                sumSq += (w - mean) * (w - mean);
            }
            stdDev = std::sqrt(sumSq / pImpl->weightHistory.size());
        }
        
        ss << "║ Min weight:    " << std::setw(8) << std::fixed << std::setprecision(4) << minWeight << "  │ "
           << std::setw(8) << std::fixed << std::setprecision(4) << maxWeight << "  Max ║" << std::endl;
        ss << "║ Std Dev:       " << std::setw(8) << std::fixed << std::setprecision(4) << stdDev << "  │ "
           << std::setw(8) << std::fixed << std::setprecision(4) << totalWeight << "  Total ║" << std::endl;
    }
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeMemorySystems(const Brain& brain) {
    if (!pImpl->showMemorySystems) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                 MEMORY SYSTEMS MONITORING                      ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Working memory
    if (auto* wm = brain.getWorkingMemory()) {
        size_t activeTraces = wm->getActiveTraces();
        ss << "║ Working Memory:  " << std::setw(6) << activeTraces << " active traces      │ "
           << std::setw(6) << wm->getCapacity() << " capacity ║" << std::endl;
    }
    
    // Episodic memory
    if (auto* em = brain.getEpisodicMemory()) {
        size_t episodes = em->getEpisodeCount();
        ss << "║ Episodic Memory: " << std::setw(6) << episodes << " episodes stored     │ "
           << std::setw(6) << em->getMaxEpisodes() << " max ║" << std::endl;
    }
    
    // Associative memory
    if (auto* am = brain.getAssociativeMemory()) {
        size_t associations = am->getAssociationCount();
        ss << "║ Associative Mem:" << std::setw(6) << associations << " associations      ║" << std::endl;
    }
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    if (!pImpl->showNeuromodulation) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║               NEUROMODULATION SYSTEMS MONITORING                ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Dopamine
    if (auto* dopamine = brain.getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        ss << "║ Dopamine:       " << std::setw(8) << std::fixed << std::setprecision(3) << dopamineLevel 
           << "  │ Plasticity: " << std::setw(6) << dopamine->getPlasticityFactor() << " ║" << std::endl;
    }
    
    // Curiosity
    if (auto* curiosity = brain.getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        ss << "║ Curiosity:      " << std::setw(8) << std::fixed << std::setprecision(3) << curiosityLevel 
           << "  │ Enabled:   " << std::setw(6) << (curiosity->isEnabled() ? "Yes" : "No") << "  ║" << std::endl;
    }
    
    // Novelty
    if (auto* novelty = brain.getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        ss << "║ Novelty:        " << std::setw(8) << std::fixed << std::setprecision(3) << noveltyLevel << "                ║" << std::endl;
    }
    
    // Prediction Error
    if (auto* predError = brain.getPredictionErrorSignal()) {
        float predErrorLevel = predError->getLevel();
        ss << "║ Prediction Err: " << std::setw(8) << std::fixed << std::setprecision(3) << predErrorLevel << "                ║" << std::endl;
    }
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    if (!pImpl->showDevelopment) return;
    
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                 DEVELOPMENT STAGE MONITORING                    ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    auto stage = brain.getDevelopmentalStage();
    std::string stageStr;
    double plasticityMod = 1.0;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            stageStr = "Initial (High Plasticity)";
            plasticityMod = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            stageStr = "Critical Period (Medium Plasticity)";
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            stageStr = "Maturation (Low Plasticity)";
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            stageStr = "Adult (Low Plasticity)";
            plasticityMod = 0.2f;
            break;
        case DevelopmentalStage::Aging:
            stageStr = "Aging (Very Low Plasticity)";
            plasticityMod = 0.1f;
            break;
    }
    
    ss << "║ Stage:          " << std::left << std::setw(30) << stageStr << "  ║" << std::endl;
    ss << "║ Plasticity Mod: " << std::setw(8) << std::fixed << std::setprecision(3) << plasticityMod << "               ║" << std::endl;
    ss << "║ Age:            N/A (placeholder)                               ║" << std::endl;
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::printConsoleVisualization(const Brain& brain) {
    std::stringstream ss;
    ss << std::endl;
    ss << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
    ss << "║                 NLM REAL-TIME VISUALIZATION                     ║" << std::endl;
    ss << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Basic brain status
    ss << "║ Neurons:   " << std::setw(6) << brain.getTotalNeuronCount() 
       << "  │ Active: " << std::setw(6) << brain.getActiveNeuronCount() << "  "
       << std::setw(3) << std::fixed << std::setprecision(1) << (brain.getTotalNeuronCount() ? 
            100.0 * brain.getActiveNeuronCount() / brain.getTotalNeuronCount() : 0.0) << "% ║" << std::endl;
    
    ss << "║ Synapses:   " << std::setw(6) << brain.getTotalSynapseCount() 
       << "  │ Firing: " << std::setw(6) << brain.getFiringNeuronCount() 
       << "  │ E/I:    " << std::setw(6) << std::fixed << std::setprecision(2) << brain.getExcitationInhibitionRatio() << " ║" << std::endl;
    
    ss << "║ Spikes:     " << std::setw(6) << brain.getTotalSpikeCount() 
       << "  │ Pending:" << std::setw(6) << brain.getPendingSpikeEventCount() 
       << "  │ Rate:   " << std::setw(6) << std::fixed << std::setprecision(3) << brain.getAverageFiringRate() << " Hz ║" << std::endl;
    
    // Visual activity meter
    ss << "║ Activity:  ["; 
    size_t activityBar = static_cast<size_t>(pImpl->currentActivityLevel * 40);
    for (size_t i = 0; i < 40; ++i) {
        if (i < activityBar) {
            ss << "▓";
        } else {
            ss << "░";
        }
    }
    ss << "] " << std::setw(3) << static_cast<int>(pImpl->currentActivityLevel * 100) << "% ║" << std::endl;
    
    ss << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

void VisualizationInterface::logRealTimeStatus(const Brain& brain) {
    std::stringstream ss;
    ss << "[Real-Time] Step " << brain.getTotalSpikeCount() << ": "
       << std::fixed << std::setprecision(1) << (pImpl->currentActivityLevel * 100) << "% active, "
       << brain.getFiringNeuronCount() << " firing, "
       << std::fixed << std::setprecision(3) << pImpl->currentFiringRate << " Hz avg rate";
    
    NLM_LOG_INFO(ss.str());
}

} // namespace nlm
