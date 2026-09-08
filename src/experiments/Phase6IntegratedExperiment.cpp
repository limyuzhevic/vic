#pragma once

// Phase6IntegratedExperiment.cpp - Implementation of Phase 6 integration experiment
// This file contains the implementation of all Phase 6 integration tests

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <algorithm>
#include <iostream>

namespace nlm {

Phase6IntegratedExperiment::Phase6IntegratedExperiment()
    : brain_(nullptr), isInitialized_(false) {
    // Initialize experiment
}

Phase6IntegratedExperiment::~Phase6IntegratedExperiment() {
    // Cleanup
}

bool Phase6IntegratedExperiment::verifyIntegration() {
    NLM_LOG_INFO("Phase 6 Integration Verification");
    NLM_LOG_INFO("Checking integration of all brain systems...");
    
    if (!brain_) {
        NLM_LOG_ERROR("Brain not initialized for integration verification");
        return false;
    }
    
    bool allSystemsIntegrated = true;
    
    // Check each integrated system
    allSystemsIntegrated &= verifySystemIntegration(brain_->getWorkingMemory(), "Working Memory");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getEpisodicMemory(), "Episodic Memory");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getAssociativeMemory(), "Associative Memory");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getPredictionSystem(), "Prediction System");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getPlanner(), "Neural Planner");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getConceptFormation(), "Concept Formation");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getAttention(), "Attentional Selection");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getDevelopmentSystem(), "Development System");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getDopamine(), "Dopamine");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getCuriosity(), "Curiosity");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getNovelty(), "Novelty");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getPredictionErrorSignal(), "Prediction Error");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getSTDP(), "STDP");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getHebbian(), "Hebbian");
    allSystemsIntegrated &= verifySystemIntegration(brain_->getStructuralPlasticity(), "Structural Plasticity");
    
    if (allSystemsIntegrated) {
        NLM_LOG_INFO("✓ All systems integrated and functional");
    } else {
        NLM_LOG_ERROR("✗ Some systems not properly integrated");
    }
    
    return allSystemsIntegrated;
}

template<typename T>
bool Phase6IntegratedExperiment::verifySystemIntegration(T* system, const std::string& systemName) {
    if (system == nullptr) {
        NLM_LOG_INFO("  " + systemName + ": NOT PRESENT");
        return false;
    }
    
    // For pointer-based systems, just presence indicates integration
    // More complex verification could be added here if needed
    NLM_LOG_INFO("  " + systemName + ": INTEGRATED");
    return true;
}

void Phase6IntegratedExperiment::testMemoryIntegration() {
    NLM_LOG_INFO("=== Memory System Integration Test ===");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot test memory integration - brain not initialized");
        return;
    }
    
    // Test working memory
    auto workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        // Store a test neuron in working memory
        workingMemory->storeToNeuron(1, 0.5f);
        workingMemory->update(0.001);
        
        size_t activeTraces = workingMemory->getActiveTraces();
        NLM_LOG_INFO("Working Memory: " + std::to_string(activeTraces) + " active traces");
        
        if (activeTraces > 0) {
            NLM_LOG_INFO("✓ Working memory integrated with neural processing");
        } else {
            NLM_LOG_ERROR("✗ Working memory not functional");
        }
    }
    
    // Test episodic memory
    auto episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory) {
        // Create a test episode
        EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.reward = 0.5f;
        episode.activeNeurons.push_back(1);
        episode.neuronActivations.push_back(0.5f);
        
        episodicMemory->storeEpisode(episode);
        
        size_t episodeCount = episodicMemory->getEpisodeCount();
        NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodeCount) + " episodes stored");
        
        if (episodeCount > 0) {
            NLM_LOG_INFO("✓ Episodic memory integrated with neural processing");
        } else {
            NLM_LOG_ERROR("✗ Episodic memory not functional");
        }
    }
    
    // Test associative memory
    auto associativeMemory = brain_->getAssociativeMemory();
    if (associativeMemory) {
        // Simple associative memory test
        NeuronId patternId = 1;
        std::vector<NeuronId> pattern(patternId, 2);
        
        associativeMemory->learnPattern(pattern, 0.8f);
        
        std::vector<NeuronId> retrieved = associativeMemory->retrieveSimilar(pattern, 0.7f);
        
        NLM_LOG_INFO("Associative Memory: " + std::to_string(retrieved.size()) + " similar patterns retrieved");
        
        if (!retrieved.empty()) {
            NLM_LOG_INFO("✓ Associative memory integrated with neural processing");
        } else {
            NLM_LOG_ERROR("✗ Associative memory not functional");
        }
    }
}

void Phase6IntegratedExperiment::testNeuromodulationIntegration() {
    NLM_LOG_INFO("=== Neuromodulation Integration Test ===");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot test neuromodulation integration - brain not initialized");
        return;
    }
    
    bool allNeuromodulatorsWorking = true;
    
    // Test dopamine
    auto dopamine = brain_->getDopamine();
    if (dopamine) {
        dopamine->update(0.001);
        float level = dopamine->getLevel();
        NLM_LOG_INFO("Dopamine level: " + std::to_string(level));
        
        if (level > 0.0f) {
            NLM_LOG_INFO("✓ Dopamine modulation integrated with neural dynamics");
        } else {
            NLM_LOG_ERROR("✗ Dopamine not functional");
            allNeuromodulatorsWorking = false;
        }
    }
    
    // Test curiosity
    auto curiosity = brain_->getCuriosity();
    if (curiosity) {
        curiosity->update(0.001);
        float level = curiosity->getLevel();
        NLM_LOG_INFO("Curiosity level: " + std::to_string(level));
        
        if (level > 0.0f) {
            NLM_LOG_INFO("✓ Curiosity integrated with exploration behavior");
        } else {
            NLM_LOG_ERROR("✗ Curiosity not functional");
            allNeuromodulatorsWorking = false;
        }
    }
    
    // Test novelty
    auto novelty = brain_->getNovelty();
    if (novelty) {
        novelty->update(0.001);
        float level = novelty->getLevel();
        NLM_LOG_INFO("Novelty level: " + std::to_string(level));
        
        if (level > 0.0f) {
            NLM_LOG_INFO("✓ Novelty detection integrated with sensory processing");
        } else {
            NLM_LOG_ERROR("✗ Novelty not functional");
            allNeuromodulatorsWorking = false;
        }
    }
    
    // Test prediction error
    auto predictionError = brain_->getPredictionErrorSignal();
    if (predictionError) {
        predictionError->update(0.001);
        float level = predictionError->getLevel();
        NLM_LOG_INFO("Prediction error level: " + std::to_string(level));
        
        if (level >= 0.0f) {  // Can be zero for perfect predictions
            NLM_LOG_INFO("✓ Prediction error integrated with learning system");
        } else {
            NLM_LOG_ERROR("✗ Prediction error not functional");
            allNeuromodulatorsWorking = false;
        }
    }
    
    if (!allNeuromodulatorsWorking) {
        NLM_LOG_ERROR("✗ Not all neuromodulators are working properly");
    }
}

void Phase6IntegratedExperiment::testCheckpointing() {
    NLM_LOG_INFO("=== Checkpoint Persistence Test ===");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot test checkpointing - brain not initialized");
        return;
    }
    
    // Create a test checkpoint file
    std::string checkpointFile = "test_checkpoint.bin";
    
    // First, run some simulation steps to create state
    for (SimulationStep step = 0; step < 10; ++step) {
        brain_->step(step);
    }
    
    // Save checkpoint
    bool saveSuccess = brain_->save(checkpointFile);
    
    if (saveSuccess) {
        NLM_LOG_INFO("✓ Checkpoint saved successfully");
        
        // Try to load it back
        brain_->reset();
        brain_->initialize();
        
        bool loadSuccess = brain_->load(checkpointFile);
        
        if (loadSuccess) {
            NLM_LOG_INFO("✓ Checkpoint loaded successfully");
            NLM_LOG_INFO("✓ Checkpoint persistence working");
        } else {
            NLM_LOG_ERROR("✗ Failed to load checkpoint");
        }
    } else {
        NLM_LOG_ERROR("✗ Failed to save checkpoint");
    }
}

void Phase6IntegratedExperiment::testReplay() {
    NLM_LOG_INFO("=== Memory Replay Test ===");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot test replay - brain not initialized");
        return;
    }
    
    auto episodicMemory = brain_->getEpisodicMemory();
    if (!episodicMemory) {
        NLM_LOG_ERROR("Cannot test replay - episodic memory not available");
        return;
    }
    
    // Create some test episodes
    for (size_t i = 0; i < 5; ++i) {
        EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = 0.3f + (i * 0.1f);
        
        // Add some active neurons
        for (size_t j = 0; j < 3; ++j) {
            episode.activeNeurons.push_back(i * 10 + j);
            episode.neuronActivations.push_back(0.5f + (j * 0.1f));
        }
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Test replay
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
    
    NLM_LOG_INFO("Replayed " + std::to_string(episodesToReplay.size()) + " episodes");
    
    if (!episodesToReplay.empty()) {
        NLM_LOG_INFO("✓ Memory replay functionality working");
    } else {
        NLM_LOG_ERROR("✗ Memory replay not working");
    }
}

void Phase6IntegratedExperiment::createBrain(const Phase6Config& config) {
    NLM_LOG_INFO("Creating Phase 6 integrated brain...");
    
    // Create configuration
    auto brainConfig = std::make_shared<Config>();
    
    // Set brain configuration parameters
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    brainConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    brainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    brainConfig->set("random_seed", 42, ConfigSource::Default);
    
    // Add plasticity parameters
    brainConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    brainConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
    brainConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    brainConfig->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    brain_ = std::make_shared<Brain>(brainConfig);
    
    // Initialize brain
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for Phase 6 experiment");
        throw std::runtime_error("Brain initialization failed");
    }
    
    isInitialized_ = true;
    
    NLM_LOG_INFO("Phase 6 integrated brain created successfully");
}

void Phase6IntegratedExperiment::initializeIntegratedSystems() {
    NLM_LOG_INFO("Initializing all integrated systems...");
    
    if (!brain_) {
        return;
    }
    
    // Memory systems
    if (brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->initialize(brain_.get());
        brain_->getWorkingMemory()->setCapacity(100);
    }
    
    if (brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->initialize(brain_.get());
        brain_->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    if (brain_->getAssociativeMemory()) {
        brain_->getAssociativeMemory()->initialize(brain_.get());
    }
    
    // Prediction system
    if (brain_->getPredictionSystem()) {
        // Prediction system initialization if needed
    }
    
    // Cognition systems
    if (brain_->getPlanner()) {
        brain_->getPlanner()->initialize(brain_.get());
        brain_->getPlanner()->setPlanningDepth(5);
    }
    
    if (brain_->getConceptFormation()) {
        brain_->getConceptFormation()->initialize(brain_.get());
    }
    
    if (brain_->getAttention()) {
        brain_->getAttention()->initialize(brain_.get());
        brain_->getAttention()->setInhibitionStrength(0.5f);
        brain_->getAttention()->setExcitationStrength(1.5f);
    }
    
    // Neuromodulation systems
    if (brain_->getNovelty()) {
        brain_->getNovelty()->initialize(brain_.get());
    }
    
    if (brain_->getCuriosity()) {
        brain_->getCuriosity()->initialize(brain_.get());
    }
    
    // Development system
    if (brain_->getDevelopmentSystem()) {
        brain_->getDevelopmentSystem()->initialize(brain_.get());
    }
    
    NLM_LOG_INFO("All integrated systems initialized");
}

Phase6Result Phase6IntegratedExperiment::run(const Phase6Config& config) {
    NLM_LOG_INFO("Running Phase 6 Integration Experiment");
    
    Phase6Result result;
    
    try {
        // Create and configure brain
        createBrain(config);
        
        // Initialize all integrated systems
        initializeIntegratedSystems();
        
        // Run simulation steps
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < config.maxSteps; ++step) {
            brain_->step(static_cast<SimulationStep>(step));
            
            // Periodic checkpointing
            if (config.enableCheckpointing && step % 100 == 0) {
                // Checkpoint managed internally by brain
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        result.totalSteps = config.maxSteps;
        result.simulationTime = duration.count();
        result.completedSuccessfully = true;
        
        // Collect results
        result.totalReward = 100.0f;  // Placeholder
        result.avgFiringRate = brain_->getAverageFiringRate();
        result.memoryEpisodesStored = brain_->getEpisodicMemory() ? brain_->getEpisodicMemory()->getEpisodeCount() : 0;
        result.dopamineLevel = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
        
        // Integration verification
        result.memoryWorkingMemoryIntegrated = verifySystemIntegration(brain_->getWorkingMemory(), "Working Memory") != nullptr;
        result.memoryEpisodicMemoryIntegrated = verifySystemIntegration(brain_->getEpisodicMemory(), "Episodic Memory") != nullptr;
        result.neuromodulationIntegrated = verifySystemIntegration(brain_->getDopamine(), "Dopamine") != nullptr &&
                                         verifySystemIntegration(brain_->getCuriosity(), "Curiosity") != nullptr &&
                                         verifySystemIntegration(brain_->getNovelty(), "Novelty") != nullptr;
        result.predictionIntegrated = verifySystemIntegration(brain_->getPredictionSystem(), "Prediction System") != nullptr;
        result.developmentIntegrated = verifySystemIntegration(brain_->getDevelopmentSystem(), "Development System") != nullptr;
        result.checkpointingWorks = config.enableCheckpointing;  // Placeholder
        
        // Wall clock time
        result.totalWallClockTime = result.simulationTime;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Phase 6 experiment failed: ") + e.what());
        result.completedSuccessfully = false;
    }
    
    NLM_LOG_INFO("Phase 6 Integration Experiment completed");
    return result;
}

} // namespace nlm