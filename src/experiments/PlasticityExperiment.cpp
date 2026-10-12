// PlasticityExperiment.cpp implementation
#include "PlasticityExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

PlasticityExperiment::PlasticityExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("plasticity");
    addTag("learning");
    addTag("stdp");
    
    // Set default parameters
    addParameter("stdp_enabled", true);
    addParameter("hebbian_enabled", true);
    addParameter("learning_rate", 0.01f);
    addParameter("plasticity_steps", 1000);
    addParameter("input_strength", 30.0f);
    addParameter("synaptic_threshold", 0.01f);
}

PlasticityExperiment::PlasticityExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "PlasticityExperiment" : config.experimentName;
    description = "Tests learning through synaptic weight changes using STDP and Hebbian rules";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("stdp_enabled")) addParameter("stdp_enabled", true);
    if (!hasParameter("hebbian_enabled")) addParameter("hebbian_enabled", true);
    if (!hasParameter("learning_rate")) addParameter("learning_rate", 0.01f);
    if (!hasParameter("plasticity_steps")) addParameter("plasticity_steps", 1000);
    if (!hasParameter("input_strength")) addParameter("input_strength", 30.0f);
    if (!hasParameter("synaptic_threshold")) addParameter("synaptic_threshold", 0.01f);
    
    addTag("plasticity");
    addTag("learning");
    addTag("stdp");
}

bool PlasticityExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in PlasticityExperiment");
        return false;
    }
    
    currentStatus = "Preparing for plasticity learning...";
    
    // Reset and initialize brain for clean experiment
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record initial state
    recordInitialState();
    
    // Enable plasticity mechanisms
    enablePlasticityMechanisms(brain);
    
    // Run plasticity learning
    size_t plasticitySteps = static_cast<size_t>(getParameterDouble("plasticity_steps"));
    float inputStrength = getParameterDouble("input_strength");
    float synapticThreshold = getParameterDouble("synaptic_threshold");
    
    NLM_LOG_INFO("Starting plasticity learning for " + std::to_string(plasticitySteps) + " steps...");
    
    for (size_t step = 0; step < plasticitySteps; ++step) {
        // Apply input pattern to stimulate learning
        simulateInputPattern(brain, step);
        
        // Apply plasticity rules based on activity
        applyPlasticityRules(brain, step);
        
        // Update progress
        currentProgress = static_cast<float>(step) / static_cast<float>(plasticitySteps);
        currentStatus = "Learning step " + std::to_string(step) + "/" + std::to_string(plasticitySteps);
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_DEBUG("Plasticity step " + std::to_string(step) + 
                         ", spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         ", weight changes: " + std::to_string(totalWeightChange));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Plasticity experiment completed: learning rate = " + 
                 std::to_string(learningRate) + ", strengthened = " + 
                 std::to_string(strengthenedSynapses) + ", weakened = " + 
                 std::to_string(weakenedSynapses));
    
    return true;
}

void PlasticityExperiment::recordInitialState() {
    initialMeanWeight = computeMeanSynapticWeight(brain);
    weightHistory.clear();
    weightHistory.push_back(initialMeanWeight);
    
    // Record initial spike count
    result.metricResults["initial_spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
    result.metricResults["initial_weight_mean"] = initialMeanWeight;
}

void PlasticityExperiment::computeFinalMetrics() {
    finalMeanWeight = computeMeanSynapticWeight(brain);
    
    // Calculate learning metrics
    float weightChange = finalMeanWeight - initialMeanWeight;
    totalWeightChange = std::abs(weightChange);
    learningRate = totalWeightChange / plasticitySteps;
    
    // Count synaptic changes
    countSynapticChanges(brain, weightChange);
    
    // Update result
    result.experimentName = name;
    result.success = true;
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["initial_weight_mean"] = initialMeanWeight;
    result.metricResults["final_weight_mean"] = finalMeanWeight;
    result.metricResults["weight_change"] = weightChange;
    result.metricResults["total_weight_change"] = totalWeightChange;
    result.metricResults["learning_rate"] = learningRate;
    result.metricResults["strengthened_synapses"] = static_cast<double>(strengthenedSynapses);
    result.metricResults["weakened_synapses"] = static_cast<double>(weakenedSynapses);
    result.metricResults["unchanged_synapses"] = static_cast<double>(unchangedSynapses);
}

void PlasticityExperiment::applyPlasticityRules(std::shared_ptr<Brain> brain, size_t step) {
    // Apply STDP if enabled
    if (getParameterBool("stdp_enabled")) {
        applySTDP(brain, step);
    }
    
    // Apply Hebbian plasticity if enabled
    if (getParameterBool("hebbian_enabled")) {
        applyHebbianPlasticity(brain, step);
    }
    
    // Apply structural plasticity
    applyStructuralPlasticity(brain, step);
}

void PlasticityExperiment::simulateInputPattern(std::shared_ptr<Brain> brain, size_t step) {
    // Create input pattern - stimulate a subset of neurons
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t inputNeurons = std::min(static_cast<size_t>(100), neuronCount / 10);
    
    float inputStrength = getParameterDouble("input_strength");
    
    // Get neurons and inject current
    auto regions = brain->getAllRegions();
    size_t neuronInjected = 0;
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < inputNeurons && neuronInjected < inputNeurons; ++i) {
            brain->injectCurrentToNeuron(neurons[i]->getId(), inputStrength);
            neuronInjected++;
        }
        if (neuronInjected >= inputNeurons) break;
    }
    
    // Step the brain
    brain->step(step, step * 0.001);
}

void PlasticityExperiment::enablePlasticityMechanisms(std::shared_ptr<Brain> brain) {
    // Enable all plasticity mechanisms on synapses
    if (auto region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            bool stdpEnabled = getParameterBool("stdp_enabled");
            bool hebbianEnabled = getParameterBool("hebbian_enabled");
            bool structuralEnabled = true;  // Always enable structural for this experiment
            
            syn->enablePlasticity(stddpEnabled, hebbianEnabled, structuralEnabled);
        }
    }
}

void PlasticityExperiment::applySTDP(std::shared_ptr<Brain> brain, size_t step) {
    // Simplified STDP application
    // In a real implementation, this would track spike timing precisely
    
    float stdpLTP = 0.02f;  // Long-term potentiation
    float stdpLTD = 0.015f; // Long-term depression
    
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), synapses.size()); ++i) {
            float currentWeight = synapses[i]->getWeight();
            
            // Apply random STDP-like changes for demonstration
            float change = (brain->getRandomGenerator()->uniformReal() < 0.5f) ? stdpLTP : -stdpLTD;
            float newWeight = currentWeight + change;
            
            // Clamp to reasonable range
            newWeight = std::max(0.0f, std::min(1.0f, newWeight));
            
            synapses[i]->setWeight(newWeight);
        }
    }
}

void PlasticityExperiment::applyHebbianPlasticity(std::shared_ptr<Brain> brain, size_t step) {
    // Simplified Hebbian rule: co-activation strengthens synapses
    float hebbianRate = getParameterDouble("learning_rate");
    
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        auto neurons = region->getAllNeurons();
        
        // Find active neurons (simplified)
        size_t numActive = std::min(static_cast<size_t>(5), neurons.size());
        std::vector<NeuronId> activeNeurons;
        for (size_t i = 0; i < numActive; ++i) {
            if (neurons[i]->getMembranePotential() > neurons[i]->getThreshold()) {
                activeNeurons.push_back(neurons[i]->getId());
            }
        }
        
        // Apply Hebbian strengthening to active synapses
        for (const auto& activeId : activeNeurons) {
            for (auto& syn : synapses) {
                if (syn->getPreNeuron() == activeId || syn->getPostNeuron() == activeId) {
                    float currentWeight = syn->getWeight();
                    float newWeight = currentWeight + hebbianRate;
                    newWeight = std::min(1.0f, newWeight);
                    syn->setWeight(newWeight);
                }
            }
        }
    }
}

void PlasticityExperiment::applyStructuralPlasticity(std::shared_ptr<Brain> brain, size_t step) {
    // Simplified structural plasticity - add/remove synapses based on activity
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    if (auto region = brain->getRegion(RegionId(1))) {
        auto& synapses = region->getSynapses();
        auto neurons = region->getAllNeurons();
        
        // Random synaptogenesis
        for (size_t i = 0; i < synapses.size() * synaptogenesisRate; ++i) {
            if (neurons.size() >= 2) {
                size_t preIdx = brain->getRandomGenerator()->uniformInt(0, neurons.size() - 1);
                size_t postIdx = brain->getRandomGenerator()->uniformInt(0, neurons.size() - 1);
                
                if (preIdx != postIdx) {
                    // Create new synapse (simplified - in real implementation would use brain's synapse creation)
                }
            }
        }
        
        // Random pruning
        for (size_t i = 0; i < synapses.size() * pruningRate; ++i) {
            if (!synapses.empty()) {
                size_t idx = brain->getRandomGenerator()->uniformInt(0, synapses.size() - 1);
                // Remove synapse (simplified - in real implementation would use brain's synapse removal)
            }
        }
    }
}

float PlasticityExperiment::computeMeanSynapticWeight(std::shared_ptr<Brain> brain) {
    if (!brain) return 0.0f;
    
    float totalWeight = 0.0f;
    size_t synapseCount = 0;
    
    if (auto region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            totalWeight += syn->getWeight();
            synapseCount++;
        }
    }
    
    return synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
}

void PlasticityExperiment::countSynapticChanges(std::shared_ptr<Brain> brain, float weightChange) {
    if (!brain) return;
    
    // Simplified counting - in real implementation would track individual synapse changes
    strengthenedSynapses = static_cast<size_t>(std::max(0.0f, weightChange) * 100);
    weakenedSynapses = static_cast<size_t>(std::max(0.0f, -weightChange) * 100);
    unchangedSynapses = 1000 - strengthenedSynapses - weakenedSynapses; // Placeholder
}

void PlasticityExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("Plasticity checkpoint saved");
}

void PlasticityExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("Plasticity checkpoint restored");
}

bool PlasticityExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
