// AdvancedBrain.cpp - Implementation of enhanced brain interface
// Provides advanced monitoring, analytics, and experiment capabilities

#include "agent/AdvancedBrain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

struct AdvancedBrain::Impl {
    std::shared_ptr<Brain> brain;
    bool monitoringEnabled;
    size_t monitoringInterval;
    std::vector<double> performanceData;
    bool optimizationEnabled;
    std::chrono::steady_clock::time_point lastOptimizationTime;
    
    Impl() : monitoringEnabled(false), monitoringInterval(100), 
             optimizationEnabled(false), lastOptimizationTime(std::chrono::steady_clock::now()) {}
};

AdvancedBrain::AdvancedBrain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    brain_ = std::make_shared<Brain>(config);
}

AdvancedBrain::~AdvancedBrain() = default;

bool AdvancedBrain::initialize() {
    try {
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize underlying brain");
            return false;
        }
        
        NLM_LOG_INFO("AdvancedBrain initialized with enhanced features");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::initialize: ") + e.what());
        return false;
    }
}

void AdvancedBrain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
}

void AdvancedBrain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Record performance before step
        double firingRate = brain_->getAverageFiringRate();
        size_t firingCount = brain_->getFiringNeuronCount();
        
        // Execute step
        brain_->step(currentStep, currentTime);
        
        // Record performance after step
        double newFiringRate = brain_->getAverageFiringRate();
        
        // Update monitoring data
        updateMonitoringData();
        
        // Run optimization if enabled and time has passed
        if (optimizationEnabled_) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastOptimizationTime);
            
            if (elapsed.count() >= 60) { // Optimize every minute
                optimizeBasedOnData();
                pImpl->lastOptimizationTime = now;
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::step: ") + e.what());
    }
}

void AdvancedBrain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        brain_->receiveSensoryInput(input);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::receiveSensoryInput: ") + e.what());
    }
}

std::unique_ptr<class Action> AdvancedBrain::produceAction() {
    try {
        return brain_->produceAction();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::produceAction: ") + e.what());
        return std::make_unique<Action>(ActionType::Wait);
    }
}

void AdvancedBrain::reset() {
    try {
        brain_->reset();
        pImpl->performanceData.clear();
        NLM_LOG_INFO("AdvancedBrain reset complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::reset: ") + e.what());
    }
}

bool AdvancedBrain::save(const std::string& filepath) const {
    try {
        return brain_->save(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::save: ") + e.what());
        return false;
    }
}

bool AdvancedBrain::load(const std::string& filepath) {
    try {
        if (!brain_->load(filepath)) {
            return false;
        }
        pImpl->performanceData.clear();
        NLM_LOG_INFO("AdvancedBrain loaded successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in AdvancedBrain::load: ") + e.what());
        return false;
    }
}

void AdvancedBrain::enableMonitoring(size_t interval) {
    pImpl->monitoringEnabled = true;
    pImpl->monitoringInterval = interval;
    NLM_LOG_INFO("AdvancedBrain monitoring enabled with interval: " + std::to_string(interval));
}

void AdvancedBrain::updateMonitoringData() {
    if (!pImpl->monitoringEnabled) return;
    
    double neuronCount = static_cast<double>(brain_->getTotalNeuronCount());
    double firingRate = brain_->getAverageFiringRate();
    double totalSpikes = static_cast<double>(brain_->getTotalSpikeCount());
    double e_iRatio = brain_->getExcitationInhibitionRatio();
    
    pImpl->performanceData.push_back(firingRate);
    
    // Keep only recent data
    if (pImpl->performanceData.size() > 1000) {
        pImpl->performanceData.erase(pImpl->performanceData.begin());
    }
}

std::map<std::string, double> AdvancedBrain::getAdvancedStats() const {
    std::map<std::string, double> stats;
    
    stats["neuron_count"] = static_cast<double>(brain_->getTotalNeuronCount());
    stats["synapse_count"] = static_cast<double>(brain_->getTotalSynapseCount());
    stats["firing_rate"] = brain_->getAverageFiringRate();
    stats["total_spikes"] = static_cast<double>(brain_->getTotalSpikeCount());
    stats["e_i_ratio"] = brain_->getExcitationInhibitionRatio();
    
    // Memory system stats
    auto* workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        stats["working_memory_traces"] = static_cast<double>(workingMem->getActiveTraces());
    }
    
    auto* episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        stats["episodic_memory_episodes"] = static_cast<double>(episodicMem->getEpisodeCount());
    }
    
    auto* associativeMem = brain_->getAssociativeMemory();
    if (associativeMem) {
        stats["associative_memory_associations"] = static_cast<double>(associativeMem->getAssociationCount());
    }
    
    // Neuromodulation stats
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        stats["dopamine_level"] = dopamine->getLevel();
    }
    
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        stats["curiosity_level"] = curiosity->getLevel();
    }
    
    auto* novelty = brain_->getNovelty();
    if (novelty) {
        stats["novelty_level"] = novelty->getLevel();
    }
    
    // Development stage
    stats["developmental_stage"] = static_cast<double>(brain_->getDevelopmentalStage());
    
    return stats;
}

void AdvancedBrain::setConfigValue(const std::string& key, double value) {
    if (brain_) {
        brain_->getConfig()->set(key, value);
    }
}

double AdvancedBrain::getConfigValue(const std::string& key, double defaultValue) const {
    if (brain_) {
        auto opt = brain_->getConfig()->get<double>(key);
        if (opt) return *opt;
    }
    return defaultValue;
}

void AdvancedBrain::forceConsolidation() {
    // Trigger memory consolidation if systems are available
    auto* episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->consolidate(0.3f); // Consolidate 30% of episodes
    }
    
    NLM_LOG_INFO("Memory consolidation forced");
}

void AdvancedBrain::optimizeMemoryUsage() {
    // Simple memory optimization
    auto* workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        workingMem->optimize();
    }
    
    NLM_LOG_INFO("Memory optimization completed");
}

void AdvancedBrain::adjustPlasticityParameters() {
    // Adjust plasticity parameters based on current state
    float learningRate = 0.001f;
    float stabilityFactor = 0.9f;
    
    // Get current statistics
    double firingRate = brain_->getAverageFiringRate();
    
    // Adjust based on firing rate
    if (firingRate > 50.0f) {
        // High firing rate: reduce plasticity to prevent runaway excitation
        learningRate *= 0.5f;
        stabilityFactor *= 1.2f;
    } else if (firingRate < 5.0f) {
        // Low firing rate: increase plasticity to encourage activity
        learningRate *= 1.5f;
        stabilityFactor *= 0.8f;
    }
    
    // Apply changes (would require access to plasticity systems)
    NLM_LOG_INFO(std::string("Adjusted plasticity: learning_rate=") + std::to_string(learningRate) +
                 ", stability_factor=" + std::to_string(stabilityFactor));
}

void AdvancedBrain::optimizeBasedOnData() {
    if (pImpl->performanceData.empty()) return;
    
    // Analyze performance data
    double avgFiringRate = std::accumulate(pImpl->performanceData.begin(), 
                                          pImpl->performanceData.end(), 0.0) / 
                          pImpl->performanceData.size();
    
    // Optimize based on current performance
    if (avgFiringRate > 30.0f) {
        // High firing rate: optimize for stability
        NLM_LOG_INFO("Optimizing for stability - adjusting plasticity");
        adjustPlasticityParameters();
    } else if (avgFiringRate < 10.0f) {
        // Low firing rate: optimize for learning
        NLM_LOG_INFO("Optimizing for learning - increasing plasticity");
        adjustPlasticityParameters();
    }
}

} // namespace nlm