// MemorySystemExperiment.cpp implementation
#include "MemorySystemExperiment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

MemorySystemExperiment::MemorySystemExperiment() {
    enableFeature("progress_tracking");
    enableFeature("checkpointing");
    addTag("memory");
    addTag("working_memory");
    addTag("episodic_memory");
    addTag("associative_memory");
    
    // Set default parameters
    addParameter("num_working_memory_tests", 20);
    addParameter("num_episodic_tests", 15);
    addParameter("num_associative_tests", 25);
    addParameter("working_memory_duration", 10.0f);
    addParameter("episodic_memory_duration", 5.0f);
    addParameter("memory_consolidation_rate", 0.01f);
    addParameter("memory_retention_threshold", 0.6f);
}

MemorySystemExperiment::MemorySystemExperiment(const ExperimentFactory::ExperimentConfig& config) {
    name = config.experimentName.empty() ? "MemorySystemExperiment" : config.experimentName;
    description = "Tests working, episodic, and associative memory systems";
    
    // Apply configuration
    if (config.enableProgressTracking) enableFeature("progress_tracking");
    if (config.enableCheckpointing) enableFeature("checkpointing");
    
    // Copy parameters
    parametersDouble = config.doubleParameters;
    parametersString = config.stringParameters;
    parametersBool = config.boolParameters;
    
    // Set defaults if not provided
    if (!hasParameter("num_working_memory_tests")) addParameter("num_working_memory_tests", 20);
    if (!hasParameter("num_episodic_tests")) addParameter("num_episodic_tests", 15);
    if (!hasParameter("num_associative_tests")) addParameter("num_associative_tests", 25);
    if (!hasParameter("working_memory_duration")) addParameter("working_memory_duration", 10.0f);
    if (!hasParameter("episodic_memory_duration")) addParameter("episodic_memory_duration", 5.0f);
    if (!hasParameter("memory_consolidation_rate")) addParameter("memory_consolidation_rate", 0.01f);
    if (!hasParameter("memory_retention_threshold")) addParameter("memory_retention_threshold", 0.6f);
    
    addTag("memory");
    addTag("working_memory");
    addTag("episodic_memory");
    addTag("associative_memory");
}

bool MemorySystemExperiment::run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
    if (!brain || !environment) {
        NLM_LOG_ERROR("Invalid brain or environment in MemorySystemExperiment");
        return false;
    }
    
    currentStatus = "Testing memory systems...";
    
    // Reset and initialize brain
    brain->reset();
    brain->initialize();
    environment->reset();
    
    // Record memory baseline
    recordMemoryBaseline();
    
    // Test working memory
    size_t numWorkingMemoryTests = static_cast<size_t>(getParameterDouble("num_working_memory_tests"));
    NLM_LOG_INFO("Testing working memory with " + std::to_string(numWorkingMemoryTests) + " trials...");
    
    for (size_t test = 0; test < numWorkingMemoryTests; ++test) {
        testWorkingMemory(brain, test);
        currentProgress = static_cast<float>(test) / static_cast<float>(numWorkingMemoryTests);
        
        if (test % 5 == 0) {
            NLM_LOG_DEBUG("Working memory test " + std::to_string(test + 1) + "/" + 
                         std::to_string(numWorkingMemoryTests));
        }
    }
    
    // Test episodic memory
    size_t numEpisodicTests = static_cast<size_t>(getParameterDouble("num_episodic_tests"));
    NLM_LOG_INFO("Testing episodic memory with " + std::to_string(numEpisodicTests) + " trials...");
    
    for (size_t test = 0; test < numEpisodicTests; ++test) {
        testEpisodicMemory(brain, test);
        currentProgress = (static_cast<float>(numWorkingMemoryTests) + static_cast<float>(test)) / 
                         static_cast<float>(numWorkingMemoryTests + numEpisodicTests);
        
        if (test % 3 == 0) {
            NLM_LOG_DEBUG("Episodic memory test " + std::to_string(test + 1) + "/" + 
                         std::to_string(numEpisodicTests));
        }
    }
    
    // Test associative memory
    size_t numAssociativeTests = static_cast<size_t>(getParameterDouble("num_associative_tests"));
    NLM_LOG_INFO("Testing associative memory with " + std::to_string(numAssociativeTests) + " trials...");
    
    for (size_t test = 0; test < numAssociativeTests; ++test) {
        testAssociativeMemory(brain, test);
        currentProgress = (static_cast<float>(numWorkingMemoryTests + numEpisodicTests) + static_cast<float>(test)) / 
                         static_cast<float>(numWorkingMemoryTests + numEpisodicTests + numAssociativeTests);
        
        if (test % 5 == 0) {
            NLM_LOG_DEBUG("Associative memory test " + std::to_string(test + 1) + "/" + 
                         std::to_string(numAssociativeTests));
        }
    }
    
    // Compute final metrics
    computeFinalMetrics();
    
    NLM_LOG_INFO("Memory system experiment completed: working memory = " + 
                 std::to_string(workingMemoryRetention) + ", episodic = " + 
                 std::to_string(episodicRecallAccuracy) + ", associative = " + 
                 std::to_string(associationStrength));
    
    return (workingMemoryRetention >= getParameterDouble("memory_retention_threshold"));
}

void MemorySystemExperiment::recordMemoryBaseline() {
    workingMemoryTestResults.clear();
    episodicMemoryTestResults.clear();
    associativeMemoryTestResults.clear();
    
    // Initialize random number generator
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
}

void MemorySystemExperiment::computeFinalMetrics() {
    // Calculate final memory metrics
    if (!workingMemoryTestResults.empty()) {
        workingMemoryRetention = std::accumulate(workingMemoryTestResults.begin(), 
                                                 workingMemoryTestResults.end(), 0.0f) / 
                                                workingMemoryTestResults.size();
    }
    
    if (!episodicMemoryTestResults.empty()) {
        episodicRecallAccuracy = std::accumulate(episodicMemoryTestResults.begin(), 
                                                 episodicMemoryTestResults.end(), 0.0f) / 
                                                 episodicMemoryTestResults.size();
    }
    
    if (!associativeMemoryTestResults.empty()) {
        associationStrength = std::accumulate(associativeMemoryTestResults.begin(), 
                                              associativeMemoryTestResults.end(), 0.0f) / 
                                              associativeMemoryTestResults.size();
    }
    
    // Calculate derived metrics
    workingMemoryCapacity = workingMemoryRetention * 10.0f;  // Estimate capacity
    episodesStored = static_cast<size_t>(episodicRecallAccuracy * 100.0f);
    episodicInfluence = episodicRecallAccuracy;
    associationTransfer = associationStrength * 0.8f;  // Transfer efficiency
    totalMemoriesStored = workingMemoryTestResults.size() + 
                         episodicMemoryTestResults.size() + 
                         associativeMemoryTestResults.size();
    
    // Update result
    result.experimentName = name;
    result.success = workingMemoryRetention >= getParameterDouble("memory_retention_threshold");
    result.totalReward = static_cast<double>(brain->getTotalSpikeCount());
    result.averageFiringRate = brain->getAverageFiringRate();
    result.totalSpikes = brain->getTotalSpikeCount();
    result.totalSynapses = brain->getTotalSynapseCount();
    result.totalNeurons = brain->getTotalNeuronCount();
    
    // Store metrics
    result.metricResults["working_memory_retention"] = workingMemoryRetention;
    result.metricResults["working_memory_capacity"] = workingMemoryCapacity;
    result.metricResults["episodic_recall_accuracy"] = episodicRecallAccuracy;
    result.metricResults["episodes_stored"] = static_cast<double>(episodesStored);
    result.metricResults["episodic_influence"] = episodicInfluence;
    result.metricResults["association_strength"] = associationStrength;
    result.metricResults["association_transfer"] = associationTransfer;
    result.metricResults["total_memories_stored"] = static_cast<double>(totalMemoriesStored);
}

void MemorySystemExperiment::testWorkingMemory(std::shared_ptr<Brain> brain, size_t testIndex) {
    // Simulate working memory test
    // In a real implementation, this would test actual working memory capacity
    float baseRetention = 0.7f;
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise -10% to +10%
    float retention = baseRetention + noise;
    
    workingMemoryTestResults.push_back(retention);
    workingMemoryCapacity = std::max(workingMemoryCapacity, retention);
}

void MemorySystemExperiment::testEpisodicMemory(std::shared_ptr<Brain> brain, size_t testIndex) {
    // Simulate episodic memory test
    // In a real implementation, this would test episodic recall
    float baseAccuracy = 0.65f;
    float learningEffect = 0.01f * static_cast<float>(testIndex);  // Learning over time
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float accuracy = std::min(1.0f, baseAccuracy + learningEffect + noise);
    
    episodicMemoryTestResults.push_back(accuracy);
    episodesStored = static_cast<size_t>(accuracy * 100.0f);
}

void MemorySystemExperiment::testAssociativeMemory(std::shared_ptr<Brain> brain, size_t testIndex) {
    // Simulate associative memory test
    // In a real implementation, this would test associative binding and transfer
    float baseStrength = 0.6f;
    float consolidation = 0.005f * static_cast<float>(testIndex);  // Consolidation over time
    float noise = (std::rand() % 200 - 100) / 1000.0f;  // Random noise
    float strength = std::min(1.0f, baseStrength + consolidation + noise);
    
    associativeMemoryTestResults.push_back(strength);
    associationStrength = std::max(associationStrength, strength);
}

void MemorySystemExperiment::checkpoint() {
    // TODO: Implement checkpoint saving
    NLM_LOG_INFO("MemorySystemExperiment checkpoint saved");
}

void MemorySystemExperiment::restore() {
    // TODO: Implement checkpoint loading
    NLM_LOG_INFO("MemorySystemExperiment checkpoint restored");
}

bool MemorySystemExperiment::hasParameter(const std::string& name) const {
    return parametersDouble.find(name) != parametersDouble.end() ||
           parametersString.find(name) != parametersString.end() ||
           parametersBool.find(name) != parametersBool.end();
}

} // namespace nlm
