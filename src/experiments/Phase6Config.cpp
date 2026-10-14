#include "Phase6Config.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

void Phase6Config::loadFromString(const std::string& configString) {
    std::istringstream stream(configString);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos || end == std::string::npos) {
            continue; // empty line
        }
        
        line = line.substr(start, end - start + 1);
        
        // Skip comments
        if (line.substr(0, 2) == "//") {
            continue;
        }
        
        // Parse key=value
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue; // malformed line
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Remove whitespace around key and value
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        
        configValues[key] = value;
    }
}

bool Phase6Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filename);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    loadFromString(buffer.str());
    
    // Apply loaded values to config
    for (const auto& pair : configValues) {
        setValue(pair.first, pair.second);
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filename);
    return true;
}

bool Phase6Config::validate() const {
    bool valid = true;
    
    // Validate neuron count
    if (neuronCount == 0) {
        NLM_LOG_WARNING("Neuron count should be positive");
        valid = false;
    }
    
    // Validate region count
    if (regionCount == 0) {
        NLM_LOG_WARNING("Region count should be positive");
        valid = false;
    }
    
    // Validate connection probability
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_WARNING("Connection probability should be between 0 and 1");
        valid = false;
    }
    
    // Validate timestep
    if (simulationTimestep <= 0.0) {
        NLM_LOG_WARNING("Simulation timestep should be positive");
        valid = false;
    }
    
    // Validate memory capacities
    if (workingMemoryCapacity == 0) {
        NLM_LOG_WARNING("Working memory capacity should be positive");
        valid = false;
    }
    
    if (maxEpisodicEpisodes == 0) {
        NLM_LOG_WARNING("Max episodic episodes should be positive");
        valid = false;
    }
    
    // Validate plasticity parameters
    if (hebbianLearningRate < 0.0f) {
        NLM_LOG_WARNING("Hebbian learning rate should be non-negative");
        valid = false;
    }
    
    if (stdpLTPWeight <= 0.0f) {
        NLM_LOG_WARNING("STDP LTP weight should be positive");
        valid = false;
    }
    
    if (stdpLTDWeight <= 0.0f) {
        NLM_LOG_WARNING("STDP LTD weight should be positive");
        valid = false;
    }
    
    if (stdpTimeConstant <= 0.0f) {
        NLM_LOG_WARNING("STDP time constant should be positive");
        valid = false;
    }
    
    // Validate plasticity rates
    if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
        NLM_LOG_WARNING("Synaptogenesis rate should be between 0 and 1");
        valid = false;
    }
    
    if (pruningRate < 0.0f || pruningRate > 1.0f) {
        NLM_LOG_WARNING("Pruning rate should be between 0 and 1");
        valid = false;
    }
    
    // Validate development intervals
    if (replayInterval == 0) {
        NLM_LOG_WARNING("Replay interval should be positive");
        valid = false;
    }
    
    if (consolidationInterval == 0) {
        NLM_LOG_WARNING("Consolidation interval should be positive");
        valid = false;
    }
    
    // Validate cognitive parameters
    if (planningDepth <= 0) {
        NLM_LOG_WARNING("Planning depth should be positive");
        valid = false;
    }
    
    if (attentionInhibitionStrength < 0.0f) {
        NLM_LOG_WARNING("Attention inhibition strength should be non-negative");
        valid = false;
    }
    
    // Validate checkpoint parameters
    if (checkpointFrequency <= 0) {
        NLM_LOG_WARNING("Checkpoint frequency should be positive");
        valid = false;
    }
    
    if (maxCheckpoints <= 0) {
        NLM_LOG_WARNING("Max checkpoints should be positive");
        valid = false;
    }
    
    // Validate log level
    if (logLevel < 0 || logLevel > 3) {
        NLM_LOG_WARNING("Log level should be between 0 and 3");
        valid = false;
    }
    
    if (!valid) {
        NLM_LOG_WARNING("Configuration validation failed");
    }
    
    return valid;
}

std::string Phase6Config::getValue(const std::string& key) const {
    auto it = configValues.find(key);
    if (it != configValues.end()) {
        return it->second;
    }
    return "";
}

void Phase6Config::setValue(const std::string& key, const std::string& value) {
    configValues[key] = value;
}

std::vector<std::string> Phase6Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configValues.size());
    for (const auto& pair : configValues) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Phase6Config::resetToDefaults() {
    // Reset to default values
    maxSteps = 10000;
    neuronCount = 1000;
    regionCount = 1;
    connectionProbability = 0.1f;
    simulationTimestep = 0.001;
    enableWorkingMemory = true;
    enableEpisodicMemory = true;
    enableSemanticMemory = true;
    enableProceduralMemory = true;
    enableAssociativeMemory = true;
    workingMemoryCapacity = 100;
    maxEpisodicEpisodes = 1000;
    enableDopamine = true;
    enableCuriosity = true;
    enablePredictionError = true;
    enableNovelty = true;
    baselineDopamineLevel = 0.5f;
    curiosityThreshold = 0.1f;
    enableHebbianPlasticity = true;
    enableSTDP = true;
    enableStructuralPlasticity = true;
    hebbianLearningRate = 0.01f;
    stdpLTPWeight = 0.01f;
    stdpLTDWeight = 0.012f;
    stdpTimeConstant = 20.0f;
    synaptogenesisRate = 0.0001f;
    pruningRate = 0.00001f;
    enableDevelopment = true;
    enableSynaptogenesis = true;
    enablePruning = true;
    enableMaturation = true;
    replayInterval = 100;
    consolidationInterval = 1000;
    developmentUpdateInterval = 1000;
    enablePredictionSystem = true;
    enableNeuralPlanner = true;
    enableConceptFormation = true;
    enableAttentionSystem = true;
    planningDepth = 5;
    attentionInhibitionStrength = 0.5f;
    enableWorldInteraction = true;
    enableSensoryProcessing = true;
    enableMotorOutput = true;
    worldType = "SimpleWorld";
    sensoryMode = "Vision";
    motorMode = "ActionSelection";
    enableCheckpointing = true;
    enableReplay = true;
    checkpointPath = "./checkpoint_test.bin";
    checkpointFrequency = 1000;
    maxCheckpoints = 10;
    enableAgentBrain = true;
    enableSocialLearning = false;
    enableContinualLearning = true;
    enableLogging = true;
    enableProfiling = false;
    enableValidation = false;
    logLevel = 2;
    experimentSeed = 42;
    testEdgeCases = false;
    stressTest = false;
    performanceBenchmark = false;
    concurrencyLevel = 1;
    
    // Clear internal values
    configValues.clear();
}

void Phase6Config::printSummary() const {
    NLM_LOG_INFO("=== Phase 6 Configuration Summary ===");
    NLM_LOG_INFO("Basic parameters:");
    NLM_LOG_INFO("  Max steps: " + std::to_string(maxSteps));
    NLM_LOG_INFO("  Neuron count: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  Region count: " + std::to_string(regionCount));
    NLM_LOG_INFO("  Connection probability: " + std::to_string(connectionProbability));
    NLM_LOG_INFO("  Simulation timestep: " + std::to_string(simulationTimestep));
    
    NLM_LOG_INFO("Memory systems:");
    NLM_LOG_INFO("  Working memory: " + std::string(enableWorkingMemory ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Episodic memory: " + std::string(enableEpisodicMemory ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Semantic memory: " + std::string(enableSemanticMemory ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Procedural memory: " + std::string(enableProceduralMemory ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Associative memory: " + std::string(enableAssociativeMemory ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Working memory capacity: " + std::to_string(workingMemoryCapacity));
    NLM_LOG_INFO("  Max episodic episodes: " + std::to_string(maxEpisodicEpisodes));
    
    NLM_LOG_INFO("Neuromodulation:");
    NLM_LOG_INFO("  Dopamine: " + std::string(enableDopamine ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Curiosity: " + std::string(enableCuriosity ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Prediction error: " + std::string(enablePredictionError ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Novelty: " + std::string(enableNovelty ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Baseline dopamine level: " + std::to_string(baselineDopamineLevel));
    
    NLM_LOG_INFO("Plasticity:");
    NLM_LOG_INFO("  Hebbian plasticity: " + std::string(enableHebbianPlasticity ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  STDP: " + std::string(enableSTDP ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Structural plasticity: " + std::string(enableStructuralPlasticity ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Hebbian learning rate: " + std::to_string(hebbianLearningRate));
    NLM_LOG_INFO("  STDP LTP weight: " + std::to_string(stdpLTPWeight));
    NLM_LOG_INFO("  STDP LTD weight: " + std::to_string(stdpLTDWeight));
    NLM_LOG_INFO("  STDP time constant: " + std::to_string(stdpTimeConstant));
    
    NLM_LOG_INFO("Development:");
    NLM_LOG_INFO("  Development: " + std::string(enableDevelopment ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Synaptogenesis: " + std::string(enableSynaptogenesis ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Pruning: " + std::string(enablePruning ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Maturation: " + std::string(enableMaturation ? "ENABLED" : "DISABLED"));
    
    NLM_LOG_INFO("Prediction and cognition:");
    NLM_LOG_INFO("  Prediction system: " + std::string(enablePredictionSystem ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Neural planner: " + std::string(enableNeuralPlanner ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Concept formation: " + std::string(enableConceptFormation ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Attention system: " + std::string(enableAttentionSystem ? "ENABLED" : "DISABLED"));
    
    NLM_LOG_INFO("Environment and interaction:");
    NLM_LOG_INFO("  World interaction: " + std::string(enableWorldInteraction ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Sensory processing: " + std::string(enableSensoryProcessing ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Motor output: " + std::string(enableMotorOutput ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  World type: " + worldType);
    NLM_LOG_INFO("  Sensory mode: " + sensoryMode);
    NLM_LOG_INFO("  Motor mode: " + motorMode);
    
    NLM_LOG_INFO("Checkpoint and persistence:");
    NLM_LOG_INFO("  Checkpointing: " + std::string(enableCheckpointing ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Replay: " + std::string(enableReplay ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Checkpoint path: " + checkpointPath);
    NLM_LOG_INFO("  Checkpoint frequency: " + std::to_string(checkpointFrequency));
    
    NLM_LOG_INFO("Experimental parameters:");
    NLM_LOG_INFO("  Agent brain: " + std::string(enableAgentBrain ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Social learning: " + std::string(enableSocialLearning ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Continual learning: " + std::string(enableContinualLearning ? "ENABLED" : "DISABLED"));
    
    NLM_LOG_INFO("Performance and debugging:");
    NLM_LOG_INFO("  Logging: " + std::string(enableLogging ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Profiling: " + std::string(enableProfiling ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Validation: " + std::string(enableValidation ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Log level: " + std::to_string(logLevel));
    NLM_LOG_INFO("  Experiment seed: " + std::to_string(experimentSeed));
}

Phase6Config Phase6Config::createScenario(const std::string& scenario) {
    Phase6Config config;
    
    if (scenario == "simple") {
        // Simple testing scenario
        config.maxSteps = 1000;
        config.neuronCount = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.05f;
        config.enableDevelopment = false;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableProfiling = true;
    }
    else if (scenario == "complex") {
        // Complex development scenario
        config.maxSteps = 100000;
        config.neuronCount = 10000;
        config.regionCount = 5;
        config.connectionProbability = 0.2f;
        config.enableDevelopment = true;
        config.enableSocialLearning = true;
        config.enableContinualLearning = true;
        config.checkpointFrequency = 5000;
        config.maxCheckpoints = 20;
        config.testEdgeCases = true;
    }
    else if (scenario == "stress") {
        // Stress testing scenario
        config.maxSteps = 50000;
        config.neuronCount = 50000;
        config.regionCount = 10;
        config.connectionProbability = 0.3f;
        config.enableDevelopment = true;
        config.enableSocialLearning = true;
        config.enableContinualLearning = true;
        config.enableProfiling = true;
        config.stressTest = true;
        config.concurrencyLevel = 4;
    }
    else if (scenario == "memory_focused") {
        // Memory-focused scenario
        config.maxSteps = 20000;
        config.neuronCount = 2000;
        config.regionCount = 2;
        config.connectionProbability = 0.1f;
        config.enableWorkingMemory = true;
        config.enableEpisodicMemory = true;
        config.enableSemanticMemory = true;
        config.workingMemoryCapacity = 500;
        config.maxEpisodicEpisodes = 5000;
        config.enableDevelopment = true;
        config.replayInterval = 50;
        config.consolidationInterval = 500;
        config.enableProfiling = true;
    }
    else if (scenario == "plasticity_focused") {
        // Plasticity-focused scenario
        config.maxSteps = 50000;
        config.neuronCount = 5000;
        config.regionCount = 3;
        config.connectionProbability = 0.15f;
        config.enableHebbianPlasticity = true;
        config.enableSTDP = true;
        config.enableStructuralPlasticity = true;
        config.hebbianLearningRate = 0.05f;
        config.stdpLTPWeight = 0.05f;
        config.stdpLTDWeight = 0.045f;
        config.stdpTimeConstant = 30.0f;
        config.enableDevelopment = true;
        config.enableProfiling = true;
    }
    else {
        // Default scenario
        NLM_LOG_WARNING("Unknown scenario: " + scenario + ". Using default configuration.");
    }
    
    return config;
}

} // namespace nlm
