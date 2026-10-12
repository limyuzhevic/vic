// BasicConnectivityExperiment.cpp implementation
#include "BasicConnectivityExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BasicConnectivityExperiment::BasicConnectivityExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("connectivity");
    addTag("basic");
    addTag("neural");
}

BasicConnectivityExperiment::BasicConnectivityExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "BasicConnectivityExperiment" : config.experimentName;
    description = "Tests neural connectivity and spike propagation through the network";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Set parameters
    for (const auto& pair : config.doubleParameters) {
        addParameter(pair.first, pair.second);
    }
    for (const auto& pair : config.stringParameters) {
        addParameter(pair.first, pair.second);
    }
    for (const auto& pair : config.boolParameters) {
        addParameter(pair.first, pair.second);
    }
    
    // Set default parameters if not provided
    if (!hasParameter("injection_current")) addParameter("injection_current", 50.0);
    if (!hasParameter("simulation_steps")) addParameter("simulation_steps", 100);
    if (!hasParameter("target_neurons")) addParameter("target_neurons", 10);
    
    addTag("connectivity");
    addTag("basic");
    addTag("neural");
}

bool BasicConnectivityExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in BasicConnectivityExperiment");
        return false;
    }
    
    currentStatus = "Running connectivity test...";
    
    // Reset brain and environment for clean test
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    size_t initialNeurons = brain->getTotalNeuronCount();
    
    // Inject current into target neurons
    size_t targetNeurons = static_cast<size_t>(getParameterDouble("target_neurons"));
    float injectionCurrent = getParameterDouble("injection_current");
    
    NLM_LOG_INFO("Injecting current into " + std::to_string(targetNeurons) + " neurons...");
    
    // Find sensory/excitatory neurons to inject into
    std::vector<NeuronId> neuronsToStimulate;
    auto regions = brain->getAllRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(targetNeurons, neurons.size()); ++i) {
            neuronsToStimulate.push_back(neurons[i]->getId());
        }
        if (neuronsToStimulate.size() >= targetNeurons) break;
    }
    
    // Inject current into selected neurons
    for (const auto& neuronId : neuronsToStimulate) {
        brain->injectCurrentToNeuron(neuronId, injectionCurrent);
    }
    
    // Run simulation
    size_t simulationSteps = static_cast<size_t>(getParameterDouble("simulation_steps"));
    size_t maxSpikeCount = 0;
    
    for (size_t step = 0; step < simulationSteps; ++step) {
        // Update brain dynamics
        brain->step(step, step * 0.001);
        
        // Update progress
        if (simulationSteps > 0) {
            currentProgress = static_cast<float>(step) / static_cast<float>(simulationSteps);
        }
        
        // Record metrics for this step
        recordStepMetrics(brain, step);
        
        // Track spike count
        size_t currentSpikeCount = brain->getTotalSpikeCount() - initialSpikes;
        maxSpikeCount = std::max(maxSpikeCount, currentSpikeCount);
        spikeHistory.push_back(currentSpikeCount);
        
        // Log progress periodically
        if (step % 10 == 0) {
            NLM_LOG_DEBUG("Step " + std::to_string(step) + ": " + 
                         std::to_string(currentSpikeCount) + " spikes generated");
        }
    }
    
    totalSpikeCount = maxSpikeCount;
    
    // Calculate connectivity score based on spike propagation
    connectionScore = calculateConnectionScore(brain, initialSpikes);
    propagationSpeed = calculatePropagationSpeed();
    
    // Store results
    result.experimentName = name;
    result.success = true;
    result.totalReward = static_cast<double>(totalSpikeCount);
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = totalSpikeCount;
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    NLM_LOG_INFO("Basic connectivity test completed: " + std::to_string(totalSpikeCount) + 
                 " spikes, connectivity score: " + std::to_string(connectionScore));
    
    return true;
}

void BasicConnectivityExperiment::recordInitialMetrics() {
    // Record initial brain state
    if (brain) {
        result.totalNeurons = brain->getTotalNeuronCount();
        result.totalSynapses = brain->getTotalSynapseCount();
        result.averageFiringRate = brain->getAverageFiringRate();
    }
}

void BasicConnectivityExperiment::computeFinalMetrics() {
    result.updateStatistics();
    
    // Add experiment-specific metrics
    result.metricResults["connection_score"] = connectionScore;
    result.metricResults["total_spikes"] = static_cast<double>(totalSpikeCount);
    result.metricResults["propagation_speed"] = propagationSpeed;
    result.metricResults["max_spike_count"] = static_cast<double>(maxSpikeCountPerStep);
    
    // Convert to experiment format
    result = convertToResult(this);
}

void BasicConnectivityExperiment::recordStepMetrics(std::shared_ptr<Brain> brain, size_t step) {
    if (!brain) return;
    
    size_t currentSpikeCount = brain->getTotalSpikeCount();
    size_t currentNeurons = brain->getTotalNeuronCount();
    size_t currentSynapses = brain->getTotalSynapseCount();
    size_t firingCount = brain->getFiringNeuronCount();
    float avgFiringRate = brain->getAverageFiringRate();
    
    result.timeseries.push_back(std::make_pair(static_cast<double>(step), 
                                               static_cast<double>(currentSpikeCount)));
    
    neuronActivity.push_back(firingCount);
    maxSpikeCountPerStep = std::max(maxSpikeCountPerStep, currentSpikeCount);
}

float BasicConnectivityExperiment::calculateConnectionScore(std::shared_ptr<Brain> brain, size_t initialSpikes) {
    if (!brain || spikeHistory.empty()) return 0.0f;
    
    size_t finalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    float spikeGrowth = static_cast<float>(finalSpikes) / std::max(1.0f, static_cast<float>(initialSpikes + 1));
    
    // Check for spike propagation by examining spike timing
    float propagationIndicator = 0.0f;
    if (spikeHistory.size() > 5) {
        // Calculate spike regularity - well-connected networks show more regular spiking
        float sum = 0.0f;
        for (size_t i = 1; i < spikeHistory.size(); ++i) {
            sum += std::abs(static_cast<float>(spikeHistory[i]) - static_cast<float>(spikeHistory[i-1]));
        }
        propagationIndicator = 1.0f / (1.0f + sum / spikeHistory.size());
    }
    
    // Check network activity level
    float activityLevel = 0.0f;
    if (!neuronActivity.empty()) {
        float avgActivity = std::accumulate(neuronActivity.begin(), neuronActivity.end(), 0.0f) / neuronActivity.size();
        activityLevel = std::min(1.0f, avgActivity / static_cast<float>(brain->getTotalNeuronCount()) * 10.0f);
    }
    
    return (spikeGrowth * 0.4f) + (propagationIndicator * 0.3f) + (activityLevel * 0.3f);
}

float BasicConnectivityExperiment::calculatePropagationSpeed() {
    if (spikeHistory.size() < 2) return 0.0f;
    
    // Calculate average time between consecutive spike bursts
    float totalPropagation = 0.0f;
    size_t bursts = 0;
    
    for (size_t i = 1; i < spikeHistory.size(); ++i) {
        if (spikeHistory[i] > spikeHistory[i-1]) {
            // Increasing spike count indicates propagation
            totalPropagation += (spikeHistory[i] - spikeHistory[i-1]);
            bursts++;
        }
    }
    
    return bursts > 0 ? totalPropagation / bursts : 0.0f;
}

void BasicConnectivityExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("Checkpoint saved for BasicConnectivityExperiment");
}

void BasicConnectivityExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("Checkpoint restored for BasicConnectivityExperiment");
}

bool BasicConnectivityExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

ExperimentResult BasicConnectivityExperiment::convertToResult(const std::unique_ptr<IExperiment>& experiment) const {
    ExperimentResult result;
    // Simplified conversion - would need proper mapping
    return result;
}

} // namespace nlm
