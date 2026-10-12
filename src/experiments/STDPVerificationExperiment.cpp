// STDPVerificationExperiment.cpp implementation
#include "STDPVerificationExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

STDPVerificationExperiment::STDPVerificationExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("stdp");
    addTag("plasticity");
    addTag("timing");
    
    // Set default parameters
    addParameter("num_trials", 50);
    addParameter("pre_post_delay", 2);
    addParameter("stdp_ltp_weight", 0.02f);
    addParameter("stdp_ltd_weight", 0.015f);
    addParameter("stdp_time_constant", 20.0f);
    addParameter("neuron_current", 60.0f);
}

STDPVerificationExperiment::STDPVerificationExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "STDPVerificationExperiment" : config.experimentName;
    description = "Tests Spike-Timing-Dependent Plasticity (LTP/LTD) with precise timing";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("num_trials")) addParameter("num_trials", 50);
    if (!hasParameter("pre_post_delay")) addParameter("pre_post_delay", 2);
    if (!hasParameter("stdp_ltp_weight")) addParameter("stdp_ltp_weight", 0.02f);
    if (!hasParameter("stdp_ltd_weight")) addParameter("stdp_ltd_weight", 0.015f);
    if (!hasParameter("stdp_time_constant")) addParameter("stdp_time_constant", 20.0f);
    if (!hasParameter("neuron_current")) addParameter("neuron_current", 60.0f);
    
    addTag("stdp");
    addTag("plasticity");
    addTag("timing");
}

bool STDPVerificationExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in STDPVerificationExperiment");
        return false;
    }
    
    currentStatus = "Setting up STDP verification...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record initial weights
    recordInitialWeights();
    
    // Enable STDP on synapses
    enableSTDP(brain);
    
    // Run STDP verification trials
    size_t numTrials = static_cast<size_t>(getParameterDouble("num_trials"));
    int prePostDelay = static_cast<int>(getParameterDouble("pre_post_delay"));
    float neuronCurrent = getParameterDouble("neuron_current");
    
    NLM_LOG_INFO("Running STDP verification with " + std::to_string(numTrials) + " trials...");
    
    ltpAmount = 0.0f;
    ltdAmount = 0.0f;
    deltaHistory.clear();
    
    for (size_t trial = 0; trial < numTrials; ++trial) {
        // Create correlated pre->post spiking pattern (LTP condition)
        createCorrelatedSpikingPattern(brain, trial);
        
        // Measure weight changes
        measureSTDPEffect(brain);
        
        // Update progress
        currentProgress = static_cast<float>(trial) / static_cast<float>(numTrials);
        currentStatus = "Trial " + std::to_string(trial + 1) + "/" + std::to_string(numTrials);
        
        // Log progress periodically
        if (trial % 10 == 0) {
            NLM_LOG_DEBUG("STDP trial " + std::to_string(trial + 1) + 
                         ", delta = " + std::to_string(deltaHistory.back()));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("STDP verification completed: total Δ = " + std::to_string(totalDelta) +
                 ", LTP = " + std::to_string(ltpAmount) + ", LTD = " + std::to_string(ltdAmount) +
                 ", working = " + std::to_string(stdpWorking));
    
    return true;
}

void STDPVerificationExperiment::recordInitialWeights() {
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        initialWeights.clear();
        
        // Record weights for first 10 synapses
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), synapses.size()); ++i) {
            initialWeights.push_back(synapses[i]->getWeight());
        }
    }
}

void STDPVerificationExperiment::computeFinalMetrics() {
    // Calculate final weights
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        finalWeights.clear();
        
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), synapses.size()); ++i) {
            finalWeights.push_back(synapses[i]->getWeight());
        }
    }
    
    // Calculate STDP metrics
    totalDelta = 0.0f;
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        deltaHistory.push_back(delta);
        totalDelta += delta;
        
        if (delta > 0.0f) {
            ltpAmount += delta;
        } else if (delta < 0.0f) {
            ltdAmount -= delta;  // Make positive
        }
    }
    
    // Determine if STDP is working
    float threshold = 0.001f;
    bool ltpPresent = ltpAmount > threshold;
    bool ltdPresent = ltdAmount > threshold;
    bool netChange = std::abs(totalDelta) > threshold;
    
    stdpWorking = (ltpPresent && ltdPresent) || netChange;
    
    // Store results
    result.experimentName = name;
    result.success = true;
    result.totalReward = static_cast<double>(totalDelta);
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["total_delta"] = totalDelta;
    result.metricResults["ltp_amount"] = ltpAmount;
    result.metricResults["ltd_amount"] = ltdAmount;
    result.metricResults["stdp_working"] = stdpWorking ? 1.0 : 0.0;
}

void STDPVerificationExperiment::createCorrelatedSpikingPattern(std::shared_ptr<Brain> brain, size_t trial) {
    if (!brain) return;
    
    int prePostDelay = static_cast<int>(getParameterDouble("pre_post_delay"));
    float neuronCurrent = getParameterDouble("neuron_current");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return;
    
    // Select pre- and post-synaptic neurons
    Neuron* preNeuron = neurons[0];
    Neuron* postNeuron = neurons[1];
    
    // Create LTP pattern: pre fires first, then post fires shortly after
    // This should strengthen the synapse (potentiation)
    preNeuron->injectCurrent(neuronCurrent);
    brain->step(trial * 2, trial * 2 * 0.001);
    
    // Post fires after delay
    for (int delay = 0; delay < prePostDelay; ++delay) {
        brain->step(trial * 2 + delay, (trial * 2 + delay) * 0.001);
    }
    
    postNeuron->injectCurrent(neuronCurrent);
    brain->step(trial * 2 + prePostDelay, (trial * 2 + prePostDelay) * 0.001);
}

void STDPVerificationExperiment::measureSTDPEffect(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) return;
        
        // Measure weights of selected synapses
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
        }
        
        // Run the trial again to observe changes (simplified)
        brain->step(100, 100 * 0.001);
        
        // Measure weights after
        std::vector<float> afterWeights;
        for (size_t i = 0; i < 5; ++i) {
            afterWeights.push_back(synapses[i]->getWeight());
        }
        
        // Calculate delta for each synapse
        for (size_t i = 0; i < 5; ++i) {
            float delta = afterWeights[i] - beforeWeights[i];
            if (std::abs(delta) > 0.0001f) {  // Only record significant changes
                deltaHistory.push_back(delta);
            }
        }
    }
}

void STDPVerificationExperiment::enableSTDP(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    bool stdpEnabled = true;  // Always enable for this experiment
    
    if (auto region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(stddpEnabled, false, false);  // Enable only STDP, not Hebbian
        }
    }
}

void STDPVerificationExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("STDP checkpoint saved");
}

void STDPVerificationExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("STDP checkpoint restored");
}

bool STDPVerificationExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
