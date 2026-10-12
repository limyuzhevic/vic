// NeuromodulationExperiment.cpp implementation
#include "NeuromodulationExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuromodulationExperiment::NeuromodulationExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("neuromodulation");
    addTag("dopamine");
    addTag("curiosity");
    addTag("novelty");
    addTag("motivation");
    
    // Set default parameters
    addParameter("num_curiosity_trials", 30);
    addParameter("num_novelty_trials", 25);
    addParameter("simulation_steps", 500);
    addParameter("dopamine_base", 0.5f);
    addParameter("curiosity_base", 0.3f);
    addParameter("novelty_sensitivity", 0.8f);
    addParameter("reward_prediction_error", 0.1f);
}

NeuromodulationExperiment::NeuromodulationExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "NeuromodulationExperiment" : config.experimentName;
    description = "Tests neuromodulation systems: dopamine, curiosity, novelty signals";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("num_curiosity_trials")) addParameter("num_curiosity_trials", 30);
    if (!hasParameter("num_novelty_trials")) addParameter("num_novelty_trials", 25);
    if (!hasParameter("simulation_steps")) addParameter("simulation_steps", 500);
    if (!hasParameter("dopamine_base")) addParameter("dopamine_base", 0.5f);
    if (!hasParameter("curiosity_base")) addParameter("curiosity_base", 0.3f);
    if (!hasParameter("novelty_sensitivity")) addParameter("novelty_sensitivity", 0.8f);
    if (!hasParameter("reward_prediction_error")) addParameter("reward_prediction_error", 0.1f);
    
    addTag("neuromodulation");
    addTag("dopamine");
    addTag("curiosity");
    addTag("novelty");
    addTag("motivation");
}

bool NeuromodulationExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in NeuromodulationExperiment");
        return false;
    }
    
    currentStatus = "Testing neuromodulation systems...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record neuromodulation baseline
    recordNeuromodulationBaseline();
    
    // Test novelty detection
    size_t numNoveltyTrials = static_cast<size_t>(getParameterDouble("num_novelty_trials"));
    NLM_LOG_INFO("Testing novelty detection with " + std::to_string(numNoveltyTrials) + " trials...");
    
    for (size_t trial = 0; trial < numNoveltyTrials; ++trial) {
        testNoveltyDetection(brain, trial);
        currentProgress = static_cast<float>(trial) / static_cast<float>(numNoveltyTrials);
        
        if (trial % 5 == 0) {
            NLM_LOG_DEBUG("Novelty test " + std::to_string(trial + 1) + "/" + std::to_string(numNoveltyTrials) +
                         ", novelty = " + std::to_string(noveltyLevel));
        }
    }
    
    // Test curiosity-driven learning
    size_t numCuriosityTrials = static_cast<size_t>(getParameterDouble("num_curiosity_trials"));
    NLM_LOG_INFO("Testing curiosity-driven learning with " + std::to_string(numCuriosityTrials) + " trials...");
    
    for (size_t trial = 0; trial < numCuriosityTrials; ++trial) {
        testCuriosityDrivenLearning(brain, trial);
        currentProgress = (static_cast<float>(numNoveltyTrials) + static_cast<float>(trial)) / 
                         static_cast<float>(numNoveltyTrials + numCuriosityTrials);
        
        if (trial % 5 == 0) {
            NLM_LOG_DEBUG("Curiosity test " + std::to_string(trial + 1) + "/" + std::to_string(numCuriosityTrials) +
                         ", curiosity = " + std::to_string(curiosityLevel));
        }
    }
    
    // Run full neuromodulation simulation
    size_t simulationSteps = static_cast<size_t>(getParameterDouble("simulation_steps"));
    NLM_LOG_INFO("Running full neuromodulation simulation with " + std::to_string(simulationSteps) + " steps...");
    
    for (size_t step = 0; step < simulationSteps; ++step) {
        simulateNeuromodulationActivity(brain, step);
        
        // Update progress
        currentProgress = (static_cast<float>(numNoveltyTrials + numCuriosityTrials) + 
                         static_cast<float>(step)) / 
                        static_cast<float>(numNoveltyTrials + numCuriosityTrials + simulationSteps);
        
        if (step % 100 == 0) {
            NLM_LOG_DEBUG("Neuromodulation step " + std::to_string(step + 1) + "/" + 
                         std::to_string(simulationSteps));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Neuromodulation experiment completed: dopamine = " + std::to_string(dopamineLevel) +
                 ", curiosity = " + std::to_string(curiosityLevel) + ", novelty = " + 
                 std::to_string(noveltyLevel) + ", motivation = " + std::to_string(motivationalDrive));
    
    return (curiosityLevel > 0.0f && noveltyLevel > 0.0f);
}

void NeuromodulationExperiment::recordNeuromodulationBaseline() {
    dopamineHistory.clear();
    curiosityHistory.clear();
    noveltyHistory.clear();
    explorationHistory.clear();
    
    dopamineLevel = getParameterDouble("dopamine_base");
    curiosityLevel = getParameterDouble("curiosity_base");
    noveltyLevel = 0.0f;
    motivationalDrive = 0.0f;
    rewardPredictionError = getParameterDouble("reward_prediction_error");
    explorationRate = 0.0f;
    learningRateModulation = 1.0f;
}

void NeuromodulationExperiment::computeFinalMetrics() {
    // Calculate final neuromodulation metrics
    if (!dopamineHistory.empty()) {
        dopamineLevel = dopamineHistory.back();
    }
    
    if (!curiosityHistory.empty()) {
        curiosityLevel = curiosityHistory.back();
    }
    
    if (!noveltyHistory.empty()) {
        noveltyLevel = noveltyHistory.back();
    }
    
    if (!explorationHistory.empty()) {
        explorationRate = std::accumulate(explorationHistory.begin(), 
                                          explorationHistory.end(), 0.0f) / 
                                          explorationHistory.size();
    }
    
    // Calculate motivational drive
    motivationalDrive = (dopamineLevel * 0.4f) + (curiosityLevel * 0.3f) + 
                       (noveltyLevel * 0.3f);
    
    // Calculate learning rate modulation
    learningRateModulation = 1.0f + (curiosityLevel * 0.5f) + (rewardPredictionError * 0.2f);
    
    // Update result
    result.experimentName = name;
    result.success = (curiosityLevel > 0.0f && noveltyLevel > 0.0f);
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["dopamine_level"] = dopamineLevel;
    result.metricResults["curiosity_level"] = curiosityLevel;
    result.metricResults["novelty_level"] = noveltyLevel;
    result.metricResults["motivational_drive"] = motivationalDrive;
    result.metricResults["reward_prediction_error"] = rewardPredictionError;
    result.metricResults["exploration_rate"] = explorationRate;
    result.metricResults["learning_rate_modulation"] = learningRateModulation;
}

void NeuromodulationExperiment::simulateNeuromodulationActivity(std::shared_ptr<Brain> brain, size_t step) {
    // Simulate neuromodulator dynamics
    float noveltyStimulus = (brain->getRandomGenerator()->uniformReal() < 0.1f) ? 
        brain->getRandomGenerator()->uniformReal(0.5f, 1.0f) : 0.0f;
    
    // Update dopamine based on prediction error
    float predictionError = brain->getAverageFiringRate() * 0.01f - rewardPredictionError;
    dopamineLevel = std::max(0.0f, dopamineLevel + predictionError * 0.05f);
    dopamineHistory.push_back(dopamineLevel);
    
    // Update curiosity based on novelty
    curiosityLevel = curiosityLevel + noveltyStimulus * 0.03f;
    curiosityLevel = std::clamp(curiosityLevel, 0.0f, 1.0f);
    curiosityHistory.push_back(curiosityLevel);
    
    // Update novelty level
    noveltyLevel = noveltyStimulus * getParameterDouble("novelty_sensitivity");
    noveltyHistory.push_back(noveltyLevel);
    
    // Update exploration rate
    explorationRate = curiosityLevel * (1.0f - noveltyLevel);
    explorationHistory.push_back(explorationRate);
    
    // Update learning rate modulation
    learningRateModulation = 1.0f + curiosityLevel * 0.5f + rewardPredictionError * 0.1f;
    
    // Update brain based on neuromodulators
    if (brain) {
        brain->step(step, step * 0.001);
    }
}

void NeuromodulationExperiment::testNoveltyDetection(std::shared_ptr<Brain> brain, size_t trial) {
    // Simulate novelty detection test
    float baseNovelty = 0.3f;
    float noveltyLearning = 0.01f * static_cast<float>(trial);
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float novelty = baseNovelty + noveltyLearning + noise;
    
    novelty = std::clamp(novelty, 0.0f, 1.0f);
    noveltyHistory.push_back(novelty);
    noveltyLevel = std::max(noveltyLevel, novelty);
}

void NeuromodulationExperiment::testCuriosityDrivenLearning(std::shared_ptr<Brain> brain, size_t trial) {
    // Simulate curiosity-driven learning test
    float baseCuriosity = 0.2f;
    float explorationDriven = 0.02f * static_cast<float>(trial);  // Increase with exploration
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float curiosity = baseCuriosity + explorationDriven + noise;
    
    curiosity = std::clamp(curiosity, 0.0f, 1.0f);
    curiosityHistory.push_back(curiosity);
    curiosityLevel = std::max(curiosityLevel, curiosity);
}

void NeuromodulationExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("NeuromodulationExperiment checkpoint saved");
}

void NeuromodulationExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("NeuromodulationExperiment checkpoint restored");
}

bool NeuromodulationExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
