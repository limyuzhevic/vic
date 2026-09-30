// Analysis.cpp - Implementation of advanced analysis and debugging tools

#include "Analysis.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cmath>

namespace nlm {

// BrainAnalyzer class implementation
BrainAnalyzer::BrainAnalyzer(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain)) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    activityHistory_.clear();
}

AnalysisResults BrainAnalyzer::analyzeCurrentState() const {
    AnalysisResults results;
    
    // Network properties
    results.totalNeurons = brain_->getTotalNeuronCount();
    results.totalSynapses = brain_->getTotalSynapseCount();
    results.activeNeurons = brain_->getActiveNeuronCount();
    results.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
    
    // Activity statistics
    results.totalSpikes = brain_->getTotalSpikeCount();
    results.averageFiringRate = brain_->getAverageFiringRate();
    results.maxConcurrentSpikes = brain_->getFiringNeuronCount();
    
    // Network topology
    results.regionCount = brain_->getRegionCount();
    results.populationCount = 0;
    for (const auto& region : brain_->getRegions()) {
        results.populationCount += region->getPopulationCount();
    }
    
    // Memory system statistics
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        results.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory) {
        results.episodicMemoryEpisodes = episodicMemory->getEpisodeCount();
    }
    
    // Neuromodulation levels
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        results.dopamineLevel = dopamine->getLevel();
    }
    
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        results.curiosityLevel = curiosity->getLevel();
    }
    
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        results.noveltyLevel = novelty->getLevel();
    }
    
    // Development stage
    results.developmentalStage = static_cast<int>(brain_->getDevelopmentalStage());
    
    results.timestamp = std::chrono::system_clock::now();
    
    return results;
}

std::vector<BrainAnalyzer::NeuronStats> BrainAnalyzer::analyzeNeuronPopulation() const {
    std::vector<NeuronStats> neuronStats;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                NeuronStats stats;
                stats.id = neuron->getId();
                stats.type = neuron->getType();
                const auto& state = neuron->getState();
                stats.membranePotential = state.membranePotential;
                stats.threshold = state.threshold;
                stats.firingRate = state.firingRate;
                stats.spikeCount = std::count_if(
                    stats.id, stats.id + 1,
                    [this](NeuronId id) { return hasSpikeRecord(brain_.get(), id); }
                );
                stats.adaptationVariable = state.adaptationVariable;
                stats.lastSpikeTime = state.lastSpikeTime;
                stats.distanceFromResting = std::abs(
                    state.membranePotential - state.restingPotential
                );
                neuronStats.push_back(stats);
            }
        }
    }
    
    return neuronStats;
}

std::vector<BrainAnalyzer::SynapseStats> BrainAnalyzer::analyzeSynapsePopulation() const {
    std::vector<SynapseStats> synapseStats;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            SynapseStats stats;
            stats.id = synapse->getId();
            stats.source = synapse->getSourceNeuron();
            stats.target = synapse->getDestinationNeuron();
            stats.weight = synapse->getWeight();
            stats.efficacy = synapse->getEfficacy();
            stats.eligibilityTrace = synapse->getEligibilityTrace();
            stats.preSpikeCount = synapse->getPreSpikeHistory().size();
            stats.postSpikeCount = synapse->getPostSpikeHistory().size();
            stats.isPlastic = synapse->getPlasticityFlags().stdp || synapse->getPlasticityFlags().hebbian;
            
            // Get short-term plasticity state if possible
            // This would require accessing internal state, so we'll estimate
            stats.shortTermDepression = 1.0f - (stats.weight > 0 ? 0.1f : 0.05f);
            stats.shortTermFacilitation = (stats.weight > 0 ? 0.05f : 0.01f);
            
            synapseStats.push_back(stats);
        }
    }
    
    return synapseStats;
}

std::vector<BrainAnalyzer::RegionStats> BrainAnalyzer::analyzeRegions() const {
    std::vector<RegionStats> regionStats;
    
    for (const auto& region : brain_->getRegions()) {
        RegionStats stats;
        stats.id = region->getId();
        stats.name = region->getName();
        stats.neuronCount = region->getTotalNeuronCount();
        stats.synapseCount = region->getSynapseCount();
        stats.averageWeight = region->getAverageSynapticWeight();
        stats.averageFiringRate = region->getAverageFiringRate();
        
        // Calculate excitation/inhibition ratio for this region
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        for (const auto* synapse : region->getSynapses()) {
            float weight = synapse->getWeight();
            if (weight > 0) totalExcitatory += weight;
            else totalInhibitory += std::abs(weight);
        }
        
        if (totalInhibitory > 0.0f) {
            stats.excitationInhibitionRatio = totalExcitatory / totalInhibitory;
        } else {
            stats.excitationInhibitionRatio = totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
        }
        
        regionStats.push_back(stats);
    }
    
    return regionStats;
}

BrainAnalyzer::ConnectionStats BrainAnalyzer::analyzeConnectivity() const {
    ConnectionStats stats;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            stats.totalConnections++;
            
            if (synapse->isExcitatory()) {
                stats.forwardConnections++;
            } else if (synapse->isInhibitory()) {
                stats.backwardConnections++;
            }
        }
    }
    
    // Calculate average connection strength
    if (stats.totalConnections > 0) {
        float totalStrength = 0.0f;
        for (const auto& region : brain_->getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                totalStrength += std::abs(synapse->getWeight());
            }
        }
        stats.averageConnectionStrength = totalStrength / stats.totalConnections;
    }
    
    return stats;
}

void BrainAnalyzer::updateActivityHistory() const {
    ActivityPattern pattern;
    pattern.timestamp = brain_->getCurrentTime();
    pattern.spikeCount = brain_->getTotalSpikeCount();
    pattern.activeNeuronCount = brain_->getActiveNeuronCount();
    pattern.averageFiringRate = brain_->getAverageFiringRate();
    
    // Calculate variance
    if (pattern.spikeCount > 0) {
        pattern.variance = static_cast<float>(pattern.spikeCount) * 0.1f; // Simplified variance
    } else {
        pattern.variance = 0.0f;
    }
    
    activityHistory_.push_back(pattern);
    
    // Keep history limited
    if (activityHistory_.size() > 1000) {
        activityHistory_.erase(activityHistory_.begin());
    }
}

float BrainAnalyzer::computeFiringRate(size_t spikes, size_t neurons, double time) const {
    if (time <= 0.0 || neurons == 0) return 0.0f;
    return static_cast<float>(spikes) / (neurons * time);
}

void BrainAnalyzer::computeNetworkStatistics() const {
    // Placeholder for network statistics computation
    // This would compute various network metrics
}

bool BrainAnalyzer::hasSpikeRecord(const Brain* brain, NeuronId id) const {
    // Placeholder implementation
    return false;
}

// High-level analysis functions
namespace analysis {
    AnalysisResults quickAnalysis(const Brain& brain) {
        AnalysisResults results;
        
        // Simple analysis
        results.totalNeurons = brain.getTotalNeuronCount();
        results.totalSpikes = brain.getTotalSpikeCount();
        results.averageFiringRate = brain.getAverageFiringRate();
        results.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
        
        return results;
    }
    
    std::string generateReport(const Brain& brain) {
        std::stringstream ss;
        ss << "NLM Analysis Report" << std::endl;
        ss << "===================" << std::endl;
        ss << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
        ss << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        ss << "Excitatory/Inhibitory Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        
        return ss.str();
    }
    
    // Additional implementation would go here...
}

} // namespace nlm
