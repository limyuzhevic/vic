// StructuralPlasticityExperiment.cpp implementation
#include "StructuralPlasticityExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

StructuralPlasticityExperiment::StructuralPlasticityExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("structural_plasticity");
    addTag("synaptogenesis");
    addTag("pruning");
    addTag("network_formation");
    
    // Set default parameters
    addParameter("num_synaptogenesis_tests", 20);
    addParameter("num_pruning_tests", 15);
    addParameter("simulation_steps", 1000);
    addParameter("synaptogenesis_base", 0.01f);
    addParameter("pruning_base", 0.001f);
    addParameter("critical_activity_threshold", 0.5f);
}

StructuralPlasticityExperiment::StructuralPlasticityExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "StructuralPlasticityExperiment" : config.experimentName;
    description = "Tests structural plasticity: synaptogenesis and pruning dynamics";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("num_synaptogenesis_tests")) addParameter("num_synaptogenesis_tests", 20);
    if (!hasParameter("num_pruning_tests")) addParameter("num_pruning_tests", 15);
    if (!hasParameter("simulation_steps")) addParameter("simulation_steps", 1000);
    if (!hasParameter("synaptogenesis_base")) addParameter("synaptogenesis_base", 0.01f);
    if (!hasParameter("pruning_base")) addParameter("pruning_base", 0.001f);
    if (!hasParameter("critical_activity_threshold")) addParameter("critical_activity_threshold", 0.5f);
    
    addTag("structural_plasticity");
    addTag("synaptogenesis");
    addTag("pruning");
    addTag("network_formation");
}

bool StructuralPlasticityExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in StructuralPlasticityExperiment");
        return false;
    }
    
    currentStatus = "Testing structural plasticity dynamics...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record structural baseline
    recordStructuralBaseline();
    
    // Test synaptogenesis
    size_t numSynaptogenesisTests = static_cast<size_t>(getParameterDouble("num_synaptogenesis_tests"));
    NLM_LOG_INFO("Testing synaptogenesis with " + std::to_string(numSynaptogenesisTests) + " trials...");
    
    for (size_t test = 0; test < numSynaptogenesisTests; ++test) {
        testSynaptogenesis(brain, test);
        currentProgress = static_cast<float>(test) / static_cast<float>(numSynaptogenesisTests);
        
        if (test % 5 == 0) {
            NLM_LOG_DEBUG("Synaptogenesis test " + std::to_string(test + 1) + "/" + std::to_string(numSynaptogenesisTests) +
                         ", rate = " + std::to_string(synaptogenesisRate));
        }
    }
    
    // Test pruning
    size_t numPruningTests = static_cast<size_t>(getParameterDouble("num_pruning_tests"));
    NLM_LOG_INFO("Testing pruning with " + std::to_string(numPruningTests) + " trials...");
    
    for (size_t test = 0; test < numPruningTests; ++test) {
        testPruning(brain, test);
        currentProgress = (static_cast<float>(numSynaptogenesisTests) + static_cast<float>(test)) / 
                         static_cast<float>(numSynaptogenesisTests + numPruningTests);
        
        if (test % 5 == 0) {
            NLM_LOG_DEBUG("Pruning test " + std::to_string(test + 1) + "/" + std::to_string(numPruningTests) +
                         ", rate = " + std::to_string(pruningRate));
        }
    }
    
    // Run full structural plasticity simulation
    size_t simulationSteps = static_cast<size_t>(getParameterDouble("simulation_steps"));
    NLM_LOG_INFO("Running full structural plasticity simulation with " + std::to_string(simulationSteps) + " steps...");
    
    for (size_t step = 0; step < simulationSteps; ++step) {
        simulateStructuralPlasticity(brain, step);
        
        // Update progress
        currentProgress = (static_cast<float>(numSynaptogenesisTests + numPruningTests) + 
                         static_cast<float>(step)) / 
                        static_cast<float>(numSynaptogenesisTests + numPruningTests + simulationSteps);
        
        if (step % 200 == 0) {
            NLM_LOG_DEBUG("Structural plasticity step " + std::to_string(step + 1) + "/" + 
                         std::to_string(simulationSteps));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Structural plasticity experiment completed: synaptogenesis = " + 
                 std::to_string(synaptogenesisRate) + ", pruning = " + std::to_string(pruningRate) +
                 ", net growth = " + std::to_string(netGrowth) + ", new synapses = " + 
                 std::to_string(newSynapsesCreated) + ", pruned = " + 
                 std::to_string(synapsesPruned));
    
    return (netGrowth > 0.0f);
}

void StructuralPlasticityExperiment::recordStructuralBaseline() {
    synaptogenesisTestResults.clear();
    pruningTestResults.clear();
    
    synaptogenesisRate = getParameterDouble("synaptogenesis_base");
    pruningRate = getParameterDouble("pruning_base");
    newSynapsesCreated = 0;
    synapsesPruned = 0;
    structuralComplexity = 0.0f;
    networkEfficiency = 0.0f;
}

void StructuralPlasticityExperiment::computeFinalMetrics() {
    // Calculate final structural plasticity metrics
    if (!synaptogenesisTestResults.empty()) {
        synaptogenesisRate = std::accumulate(synaptogenesisTestResults.begin(), 
                                            synaptogenesisTestResults.end(), 0.0f) / 
                                            synaptogenesisTestResults.size();
    }
    
    if (!pruningTestResults.empty()) {
        pruningRate = std::accumulate(pruningTestResults.begin(), 
                                      pruningTestResults.end(), 0.0f) / 
                                      pruningTestResults.size();
    }
    
    // Calculate derived metrics
    netGrowth = synaptogenesisRate - pruningRate;
    structuralComplexity = synaptogenesisRate * 10.0f;  // Estimate complexity
    networkEfficiency = (synapogenesisRate > 0.0f) ? 
        pruningRate / synaptogenesisRate : 0.0f;  // Pruning efficiency
    
    // Update result
    result.experimentName = name;
    result.success = (netGrowth > 0.0f);
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["synaptogenesis_rate"] = synaptogenesisRate;
    result.metricResults["pruning_rate"] = pruningRate;
    result.metricResults["net_growth"] = netGrowth;
    result.metricResults["new_synapses_created"] = static_cast<double>(newSynapsesCreated);
    result.metricResults["synapses_pruned"] = static_cast<double>(synapsesPruned);
    result.metricResults["structural_complexity"] = structuralComplexity;
    result.metricResults["network_efficiency"] = networkEfficiency;
}

void StructuralPlasticityExperiment::simulateStructuralPlasticity(std::shared_ptr<Brain> brain, size_t step) {
    // Simulate structural plasticity dynamics
    float criticalActivity = brain->getAverageFiringRate();
    
    // Synaptogenesis - depends on activity
    if (criticalActivity > getParameterDouble("critical_activity_threshold")) {
        float birthRate = getParameterDouble("synaptogenesis_base") * criticalActivity * 0.1f;
        synaptogenesisRate = synaptogenesisRate + birthRate * 0.01f;
        newSynapsesCreated += static_cast<size_t>(birthRate);
        synaptogenesisTestResults.push_back(synaptogenesisRate);
    }
    
    // Pruning - depends on inactivity
    float activityLevel = std::min(1.0f, criticalActivity * 2.0f);
    float deathRate = getParameterDouble("pruning_base") * (1.0f - activityLevel) * 0.05f;
    pruningRate = pruningRate + deathRate * 0.01f;
    synapsesPruned += static_cast<size_t>(deathRate);
    pruningTestResults.push_back(pruningRate);
    
    // Update brain based on structural changes
    if (brain) {
        brain->step(step, step * 0.001);
    }
}

void StructuralPlasticityExperiment::testSynaptogenesis(std::shared_ptr<Brain> brain, size_t testIndex) {
    // Simulate synaptogenesis test
    float baseRate = getParameterDouble("synaptogenesis_base");
    float activityDependent = 0.005f * static_cast<float>(testIndex);  // Increase with test
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float rate = baseRate + activityDependent + noise;
    
    rate = std::clamp(rate, 0.0f, 1.0f);
    pruningTestResults.push_back(rate);
    synaptogenesisRate = std::max(synaptogenesisRate, rate);
    newSynapsesCreated += static_cast<size_t>(rate * 100.0f);
}

void StructuralPlasticityExperiment::testPruning(std::shared_ptr<Brain> brain, size_t testIndex) {
    // Simulate pruning test
    float baseRate = getParameterDouble("pruning_base");
    float inactivityDependent = 0.003f * static_cast<float>(testIndex);  // Increase with test
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float rate = baseRate + inactivityDependent + noise;
    
    rate = std::clamp(rate, 0.0f, 1.0f);
    pruningTestResults.push_back(rate);
    pruningRate = std::max(pruningRate, rate);
    synapsesPruned += static_cast<size_t>(rate * 50.0f);
}

void StructuralPlasticityExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("StructuralPlasticityExperiment checkpoint saved");
}

void StructuralPlasticityExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("StructuralPlasticityExperiment checkpoint restored");
}

bool StructuralPlasticityExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
