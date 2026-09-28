#include "visualization/VisualizationManager.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

// NeuralDebugger implementation
void NeuralDebugger::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    history_.clear();
    stepsSinceLastSave_ = 0;
    
    NLM_LOG_INFO("NeuralDebugger initialized");
}

void NeuralDebugger::step(double timestep, size_t step) {
    stepsSinceLastSave_++;
    
    // Collect snapshot
    BrainSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.timestep = timestep;
    snapshot.step = step;
    snapshot.totalSpikeCount = brain_->getTotalSpikeCount();
    snapshot.activeNeuronCount = brain_->getActiveNeuronCount();
    snapshot.averageFiringRate = brain_->getAverageFiringRate();
    
    // Collect neuron data
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronActivations;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                neuronMembranePotentials.push_back(neuron->getMembranePotential());
                neuronActivations.push_back(std::abs(neuron->getMembranePotential() - neuron->getRestingPotential()) / 20.0f);
            }
        }
    }
    
    snapshot.neuronMembranePotentials = neuronMembranePotentials;
    snapshot.neuronActivations = neuronActivations;
    
    // Collect synaptic weights
    std::vector<float> synapticWeights;
    for (const auto& region : brain_->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            synapticWeights.push_back(syn->getWeight());
        }
    }
    snapshot.synapticWeights = synapticWeights;
    
    // Collect neuromodulator levels
    std::map<std::string, float> neuromodulatorLevels;
    if (auto* dopamine = brain_->getDopamine()) {
        neuromodulatorLevels["dopamine"] = dopamine->getLevel();
    }
    if (auto* curiosity = brain_->getCuriosity()) {
        neuromodulatorLevels["curiosity"] = curiosity->getLevel();
    }
    if (auto* novelty = brain_->getNovelty()) {
        neuromodulatorLevels["novelty"] = novelty->getLevel();
    }
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        neuromodulatorLevels["prediction_error"] = predictionError->getError();
    }
    
    snapshot.neuromodulatorLevels = neuromodulatorLevels;
    snapshot.developmentalStage = std::to_string(static_cast<int>(brain_->getDevelopmentalStage()));
    
    currentSnapshot_ = snapshot;
    history_.push_back(snapshot);
    
    // Auto-save if interval reached
    if (autoSave_ && stepsSinceLastSave_ >= saveInterval_) {
        saveState();
        stepsSinceLastSave_ = 0;
    }
}

void NeuralDebugger::printNeuronInfo(size_t neuronId) const {
    std::cout << "=== Neuron Information ===" << std::endl;
    std::cout << "Neuron ID: " << neuronId << std::endl;
    
    // Find neuron
    const Neuron* neuron = nullptr;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* n : pop->getNeurons()) {
                if (n->getId().index() == neuronId) {
                    neuron = n;
                    break;
                }
            }
            if (neuron) break;
        }
        if (neuron) break;
    }
    
    if (!neuron) {
        std::cout << "Neuron not found" << std::endl;
        return;
    }
    
    const auto& state = neuron->getState();
    std::cout << "Type: " << static_cast<int>(neuron->getType()) << std::endl;
    std::cout << "Membrane Potential: " << state.membranePotential << " mV" << std::endl;
    std::cout << "Resting Potential: " << state.restingPotential << " mV" << std::endl;
    std::cout << "Threshold: " << state.threshold << " mV" << std::endl;
    std::cout << "Reset Potential: " << state.resetPotential << " mV" << std::endl;
    std::cout << "Leak Conductance: " << state.leakConductance << " nS" << std::endl;
    std::cout << "Firing State: " << static_cast<int>(state.firingState) << std::endl;
    std::cout << "Refractory Period: " << state.refractoryPeriod << " ms" << std::endl;
    std::cout << "Refractory Remaining: " << state.refractoryRemaining << " ms" << std::endl;
    std::cout << "Last Spike Time: " << state.lastSpikeTime << " ms" << std::endl;
    std::cout << "Total Current: " << neuron->getTotalCurrent() << " pA" << std::endl;
    std::cout << "Is Firing: " << neuron->isFiring() << std::endl;
    std::cout << "Is Refractory: " << neuron->isRefractory() << std::endl;
}

void NeuralDebugger::printSynapseInfo(size_t synapseId) const {
    std::cout << "=== Synapse Information ===" << std::endl;
    std::cout << "Synapse ID: " << synapseId << std::endl;
    
    // Find synapse
    const Synapse* synapse = nullptr;
    for (const auto& region : brain_->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            if (syn->getId().index() == synapseId) {
                synapse = syn;
                break;
            }
        }
        if (synapse) break;
    }
    
    if (!synapse) {
        std::cout << "Synapse not found" << std::endl;
        return;
    }
    
    std::cout << "Source Neuron: " << synapse->getSourceNeuron().index() << std::endl;
    std::cout << "Target Neuron: " << synapse->getDestinationNeuron().index() << std::endl;
    std::cout << "Weight: " << synapse->getWeight() << std::endl;
    std::cout << "Delay: " << synapse->getDelay() << " ms" << std::endl;
    std::cout << "Type: " << static_cast<int>(synapse->getType()) << std::endl;
    std::cout << "Eligibility Trace: " << synapse->getEligibilityTrace() << std::endl;
    std::cout << "Plasticity Flags: STDP=" << synapse->getPlasticityFlags().stdp 
              << " Hebbian=" << synapse->getPlasticityFlags().hebbian 
              << " Structural=" << synapse->getPlasticityFlags().structuralPlasticity << std::endl;
    
    // Get spike history
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    std::cout << "Pre-synaptic Spike Count: " << preSpikes.size() << std::endl;
    std::cout << "Post-synaptic Spike Count: " << postSpikes.size() << std::endl;
}

void NeuralDebugger::printRegionInfo(size_t regionId) const {
    std::cout << "=== Region Information ===" << std::endl;
    std::cout << "Region ID: " << regionId << std::endl;
    
    auto* region = brain_->getRegion(RegionId(regionId));
    if (!region) {
        std::cout << "Region not found" << std::endl;
        return;
    }
    
    std::cout << "Name: " << region->getName() << std::endl;
    std::cout << "Total Neurons: " << region->getTotalNeuronCount() << std::endl;
    std::cout << "Population Count: " << region->getPopulationCount() << std::endl;
    std::cout << "Total Synapses: " << region->getSynapseCount() << std::endl;
    std::cout << "Average Synaptic Weight: " << region->getAverageSynapticWeight() << std::endl;
    std::cout << "Synaptic Density: " << region->getSynapticDensity() << std::endl;
    std::cout << "Active Neurons: " << region->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << region->getFiringNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << region->getAverageFiringRate() << std::endl;
}

void NeuralDebugger::printNetworkStats() const {
    std::cout << "=== Network Statistics ===" << std::endl;
    std::cout << "Total Regions: " << brain_->getRegionCount() << std::endl;
    std::cout << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
    std::cout << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
    std::cout << "Excitatory/Inhibitory Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Pending Spike Events: " << brain_->getPendingSpikeEventCount() << std::endl;
}

void NeuralDebugger::printPlasticityStats() const {
    std::cout << "=== Plasticity Statistics ===" << std::endl;
    
    size_t stdpCount = 0;
    size_t hebbianCount = 0;
    size_t structuralCount = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) stdpCount++;
            if (syn->getPlasticityFlags().hebbian) hebbianCount++;
            if (syn->getPlasticityFlags().structuralPlasticity) structuralCount++;
        }
    }
    
    std::cout << "STDP Synapses: " << stdpCount << std::endl;
    std::cout << "Hebbian Synapses: " << hebbianCount << std::endl;
    std::cout << "Structural Plasticity Synapses: " << structuralCount << std::endl;
    std::cout << "Total Plastic Synapses: " << (stdpCount + hebbianCount + structuralCount) << std::endl;
}

void NeuralDebugger::printNeuromodulationStats() const {
    std::cout << "=== Neuromodulation Statistics ===" << std::endl;
    
    if (auto* dopamine = brain_->getDopamine()) {
        std::cout << "Dopamine Level: " << dopamine->getLevel() << std::endl;
        std::cout << "Dopamine Plasticity Factor: " << dopamine->getPlasticityFactor() << std::endl;
    }
    
    if (auto* curiosity = brain_->getCuriosity()) {
        std::cout << "Curiosity Level: " << curiosity->getLevel() << std::endl;
        std::cout << "Exploration Drive: " << curiosity->getExplorationDrive() << std::endl;
    }
    
    if (auto* novelty = brain_->getNovelty()) {
        std::cout << "Novelty Level: " << novelty->getLevel() << std::endl;
        std::cout << "Novelty History Size: " << novelty->getHistory().size() << std::endl;
    }
    
    if (auto* predictionError = brain_->getPredictionErrorSignal()) {
        std::cout << "Prediction Error: " << predictionError->getError() << std::endl;
        std::cout << "Prediction Error Magnitude: " << predictionError->getMagnitude() << std::endl;
        std::cout << "Prediction Error History Size: " << predictionError->getHistory().size() << std::endl;
    }
}

void NeuralDebugger::exportNeuronData(size_t neuronId, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open neuron data file: " + filename);
        return;
    }

    file << "neuron_id,time,membrane_potential,activation,firing_state" << std::endl;
    
    // This would need actual neuron data collection over time
    // For now, just write current state
    file << neuronId << "," << currentSnapshot_.step << "," 
         << currentSnapshot_.neuronMembranePotentials[std::min(neuronId, currentSnapshot_.neuronMembranePotentials.size()-1)] << "," 
         << currentSnapshot_.neuronActivations[std::min(neuronId, currentSnapshot_.neuronActivations.size()-1)] << ","
         << (currentSnapshot_.neuronActivations[std::min(neuronId, currentSnapshot_.neuronActivations.size()-1)] > 0.5 ? "firing" : "resting") << std::endl;
    
    file.close();
    NLM_LOG_INFO("Neuron data exported to: " + filename);
}

void NeuralDebugger::exportSynapseData(size_t synapseId, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open synapse data file: " + filename);
        return;
    }

    file << "synapse_id,time,weight,eligibility_trace,pre_spike_count,post_spike_count" << std::endl;
    
    // This would need actual synapse data collection over time
    file << synapseId << "," << currentSnapshot_.step << "," 
         << currentSnapshot_.synapticWeights[std::min(synapseId, currentSnapshot_.synapticWeights.size()-1)] << "," 
         << 0.0 << "," << 0 << "," << 0 << std::endl;
    
    file.close();
    NLM_LOG_INFO("Synapse data exported to: " + filename);
}

void NeuralDebugger::exportRegionData(size_t regionId, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open region data file: " + filename);
        return;
    }

    file << "region_id,time,neuron_count,synapse_count,average_weight" << std::endl;
    file << regionId << "," << currentSnapshot_.step << "," 
         << currentSnapshot_.neuronMembranePotentials.size() / brain_->getRegionCount() << "," 
         << currentSnapshot_.synapticWeights.size() / brain_->getRegionCount() << "," 
         << (currentSnapshot_.synapticWeights.empty() ? 0.0 : std::accumulate(currentSnapshot_.synapticWeights.begin(), currentSnapshot_.synapticWeights.end(), 0.0f) / currentSnapshot_.synapticWeights.size()) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Region data exported to: " + filename);
}

void NeuralDebugger::exportNetworkData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open network data file: " + filename);
        return;
    }

    file << "step,time,total_neurons,total_synapses,total_spikes,active_neurons,average_firing_rate,e_i_ratio" << std::endl;
    
    for (size_t i = 0; i < history_.size(); ++i) {
        const auto& snapshot = history_[i];
        file << snapshot.step << "," << snapshot.timestep * snapshot.step << "," 
             << snapshot.neuronMembranePotentials.size() << "," 
             << snapshot.synapticWeights.size() << "," 
             << snapshot.totalSpikeCount << "," 
             << snapshot.activeNeuronCount << "," 
             << snapshot.averageFiringRate << "," 
             << (snapshot.synapticWeights.size() > 0 ? std::accumulate(snapshot.synapticWeights.begin(), snapshot.synapticWeights.end(), 0.0f) / snapshot.synapticWeights.size() : 0.0) << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Network data exported to: " + filename);
}

void NeuralDebugger::saveState() {
    std::string filename = "debug_neural_" + std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())) + ".json";
    exportNetworkData(filename);
    NLM_LOG_INFO("Neural debug state saved to: " + filename);
}

// PerformanceProfiler implementation
PerformanceMetrics PerformanceProfiler::currentMetrics_;
std::vector<PerformanceProfiler::PerformanceMetrics> PerformanceProfiler::history_;
std::chrono::high_resolution_clock::time_point PerformanceProfiler::stepStartTime_;
std::shared_ptr<Brain> PerformanceProfiler::brain_;

void PerformanceProfiler::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    history_.clear();
    
    NLM_LOG_INFO("PerformanceProfiler initialized");
}

void PerformanceProfiler::startStep(size_t step) {
    stepStartTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::endStep() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - stepStartTime_);
    
    PerformanceMetrics metrics;
    metrics.cpuTime = duration.count() / 1000.0;  // Convert to seconds
    metrics.memoryUsage = 0.0;  // This would require actual memory monitoring
    metrics.neuronCount = brain_->getTotalNeuronCount();
    metrics.synapseCount = brain_->getTotalSynapseCount();
    metrics.spikeCount = brain_->getTotalSpikeCount();
    metrics.averageFiringRate = brain_->getAverageFiringRate();
    metrics.plasticityRate = 0.0;  // This would require actual plasticity monitoring
    metrics.learningRate = 0.01;   // Default learning rate
    metrics.totalSteps = metrics.spikeCount;  // Approximate
    metrics.simulationTime = metrics.cpuTime;
    
    currentMetrics_ = metrics;
    history_.push_back(metrics);
}

void PerformanceProfiler::exportMetrics(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metrics file: " + filename);
        return;
    }

    file << "step,cpu_time,memory_usage,neuron_count,synapse_count,spike_count,firing_rate,plasticity_rate,learning_rate" << std::endl;
    
    for (size_t i = 0; i < history_.size(); ++i) {
        const auto& metrics = history_[i];
        file << i << "," << metrics.cpuTime << "," << metrics.memoryUsage << "," 
             << metrics.neuronCount << "," << metrics.synapseCount << "," << metrics.spikeCount << "," 
             << metrics.averageFiringRate << "," << metrics.plasticityRate << "," << metrics.learningRate << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Performance metrics exported to: " + filename);
}

void PerformanceProfiler::reset() {
    history_.clear();
    currentMetrics_ = PerformanceMetrics();
    NLM_LOG_INFO("Performance profiler reset");
}

// VisualizationManager implementation
void VisualizationManager::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
    
    // Create visualization components
    visualizer_ = std::make_unique<TextVisualizer>();
    debugger_ = std::make_unique<NeuralDebugger>();
    profiler_ = std::make_unique<PerformanceProfiler>();
    
    visualizer_->initialize(brain);
    debugger_->initialize(brain);
    profiler_->initialize(brain);
    
    NLM_LOG_INFO("VisualizationManager initialized");
}

void VisualizationManager::shutdown() {
    if (visualizer_) {
        visualizer_.reset();
    }
    if (debugger_) {
        debugger_.reset();
    }
    if (profiler_) {
        profiler_.reset();
    }
    
    brain_.reset();
    NLM_LOG_INFO("VisualizationManager shutdown");
}

void VisualizationManager::update(double timestep, size_t step) {
    if (visualizer_) {
        visualizer_->update(timestep, step);
    }
    if (debugger_) {
        debugger_->step(timestep, step);
    }
    if (profiler_) {
        profiler_->startStep(step);
        profiler_->endStep();
    }
}

void VisualizationManager::render() {
    if (visualizer_) {
        visualizer_->render();
    }
}

void VisualizationManager::saveState(const std::string& filename) {
    if (debugger_) {
        debugger_->saveState();
    }
    
    if (visualizer_) {
        visualizer_->saveToFile(filename, format_);
    }
}

void VisualizationManager::loadState(const std::string& filename) {
    // This would need implementation for loading saved state
    NLM_LOG_WARNING("Load state not yet implemented");
}

void VisualizationManager::setDebugLevel(DebugLevel level) {
    debugLevel_ = level;
    if (visualizer_) {
        visualizer_->setDebugLevel(level);
    }
}

DebugLevel VisualizationManager::getDebugLevel() const {
    return debugLevel_;
}

void VisualizationManager::printStats() const {
    if (debugger_) {
        debugger_->printNetworkStats();
    }
    if (profiler_) {
        const auto& metrics = profiler_->getCurrentMetrics();
        std::cout << "=== Performance Stats ===" << std::endl;
        std::cout << "CPU Time: " << metrics.cpuTime << " s" << std::endl;
        std::cout << "Neuron Count: " << metrics.neuronCount << std::endl;
        std::cout << "Synapse Count: " << metrics.synapseCount << std::endl;
        std::cout << "Spike Count: " << metrics.spikeCount << std::endl;
        std::cout << "Average Firing Rate: " << metrics.averageFiringRate << std::endl;
    }
}

void VisualizationManager::printNetworkTopology() const {
    std::cout << "=== Network Topology ===" << std::endl;
    std::cout << "Regions: " << brain_->getRegionCount() << std::endl;
    for (size_t i = 0; i < brain_->getRegionCount(); ++i) {
        auto* region = brain_->getRegion(RegionId(i + 1));
        if (region) {
            std::cout << "  Region " << i + 1 << ": " 
                      << region->getTotalNeuronCount() << " neurons, " 
                      << region->getSynapseCount() << " synapses" << std::endl;
        }
    }
}

void VisualizationManager::printNeuronInfo(size_t neuronId) const {
    if (debugger_) {
        debugger_->printNeuronInfo(neuronId);
    }
}

void VisualizationManager::printSynapseInfo(size_t synapseId) const {
    if (debugger_) {
        debugger_->printSynapseInfo(synapseId);
    }
}

} // namespace nlm

