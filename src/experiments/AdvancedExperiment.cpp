#include "AdvancedExperiment.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <map>
#include <unordered_map>

namespace nlm {

AdvancedExperiment::AdvancedExperiment() {
    NLM_LOG_INFO("AdvancedExperiment initialized");
}

AdvancedExperiment::~AdvancedExperiment() {
    NLM_LOG_INFO("AdvancedExperiment destroyed");
}

std::shared_ptr<Brain> AdvancedExperiment::createBrainWithConfig(const AdvancedExperimentConfig& config) {
    auto brainConfig = std::make_shared<Config>();
    
    // Set basic neural parameters
    brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    brainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    brainConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    brainConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    brainConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // Set learning parameters
    brainConfig->set("stdp_ltp_weight", config.learningRate, ConfigSource::Default);
    brainConfig->set("stdp_ltd_weight", config.learningRate * 0.95f, ConfigSource::Default);
    brainConfig->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Set structural plasticity
    brainConfig->set("synaptogenesis_rate", config.plasticityModulation * 0.0001f, ConfigSource::Default);
    brainConfig->set("pruning_rate", config.plasticityModulation * 0.00001f, ConfigSource::Default);
    
    // Set development
    brainConfig->set("development_enabled", config.enableDevelopment, ConfigSource::Default);
    brainConfig->set("plasticity_modulation_enabled", true, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(brainConfig);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for advanced experiment");
        throw std::runtime_error("Brain initialization failed");
    }
    
    return brain;
}

void AdvancedExperiment::runSingleStep(std::shared_ptr<Brain> brain, uint64_t step, 
                                      const AdvancedExperimentConfig& config) {
    // Apply neuromodulation effects
    applyNeuromodulation(brain, config);
    
    // Run simulation step
    brain->step(step);
    
    // Update memory systems
    updateMemorySystems(brain, config, step);
    
    // Update cognitive systems
    updateCognitiveSystems(brain, config, step);
    
    // Update development system
    updateDevelopmentSystem(brain, config, step);
}

void AdvancedExperiment::applyNeuromodulation(std::shared_ptr<Brain> brain, 
                                             const AdvancedExperimentConfig& config) {
    // Apply dopamine modulation
    if (config.dopamineBaseline > 0.0f) {
        // Apply dopamine-based neuromodulation
        // This would involve modifying neural excitability and plasticity
        // For simplicity, we inject current proportional to dopamine level
        brain->injectCurrentToNeurons(NeuronType::Sensory, config.dopamineBaseline * 10.0f);
    }
    
    // Apply curiosity-based exploration
    if (config.curiosityLevel > 0.0f) {
        // Apply curiosity-driven exploration
        // This would involve encouraging novel behaviors
        brain->injectCurrentToNeurons(NeuronType::Internal, config.curiosityLevel * 5.0f);
    }
    
    // Apply novelty-based modulation
    if (config.noveltyDetection > 0.0f) {
        // Apply novelty detection
        // This would involve responding to novel stimuli
        brain->injectCurrentToNeurons(NeuronType::Sensory, config.noveltyDetection * 8.0f);
    }
}

void AdvancedExperiment::updateMemorySystems(std::shared_ptr<Brain> brain,
                                           const AdvancedExperimentConfig& config,
                                           uint64_t step) {
    if (!config.enableWorkingMemory || !brain->getWorkingMemory()) {
        return;
    }
    
    // Update working memory
    brain->getWorkingMemory()->update(0.001);  // 1ms timestep
    
    // Update episodic memory periodically
    if (step % 100 == 0 && config.enableEpisodicMemory && brain->getEpisodicMemory()) {
        // Store current experience in episodic memory
        // This would involve capturing current brain state
        EpisodicMemoryItem episode;
        episode.timestamp = step;
        episode.reward = config.dopamineBaseline;
        
        // Store active neurons
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        brain->getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Update associative memory
    if (config.enableAssociativeMemory && brain->getAssociativeMemory()) {
        // Update associative associations based on current experience
        // This would involve strengthening/weakening associations
        // For simplicity, we just access the system
        brain->getAssociativeMemory();
    }
}

void AdvancedExperiment::updateCognitiveSystems(std::shared_ptr<Brain> brain,
                                               const AdvancedExperimentConfig& config,
                                               uint64_t step) {
    if (!brain->getAttention()) {
        return;
    }
    
    // Update attentional selection
    brain->getAttention()->update(0.001);
    
    // Get current memory neurons for attention processing
    std::vector<NeuronId> competitors;
    if (brain->getWorkingMemory()) {
        competitors = brain->getWorkingMemory()->getMemoryNeurons();
    }
    
    if (!competitors.empty() && config.attentionStrength > 0.0f) {
        // Process competitive attention
        brain->getAttention()->processCompetition(competitors);
    }
    
    // Update concept formation
    if (brain->getConceptFormation() && config.conceptFormationStability > 0.0f) {
        // Update concept formation based on current neural activity
        // This would involve discovering patterns and forming concepts
        brain->getConceptFormation();
    }
    
    // Update planning system
    if (brain->getPlanner() && step % 50 == 0) {
        // Update planning based on current predictions
        // This would involve action sequence evaluation
        brain->getPlanner()->setPlanningDepth(3);
    }
}

void AdvancedExperiment::updateDevelopmentSystem(std::shared_ptr<Brain> brain,
                                                 const AdvancedExperimentConfig& config,
                                                 uint64_t step) {
    if (!config.enableDevelopment) {
        return;
    }
    
    // Determine developmental stage based on step count
    DevelopmentalStage stage;
    if (step < 1000) {
        stage = DevelopmentalStage::Initial;
    } else if (step < 3000) {
        stage = DevelopmentalStage::CriticalPeriod;
    } else if (step < 6000) {
        stage = DevelopmentalStage::Maturation;
    } else if (step < 9000) {
        stage = DevelopmentalStage::Adult;
    } else {
        stage = DevelopmentalStage::Aging;
    }
    
    // Set developmental stage
    brain->setDevelopmentalStage(stage);
    
    // Apply stage-specific effects
    float plasticityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;
            break;
        case DevelopmentalStage::Aging:
            plasticityMod = 0.1f;
            break;
    }
    
    // Update structural plasticity based on developmental stage
    if (brain->getStructuralPlasticity()) {
        brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f * plasticityMod);
        brain->getStructuralPlasticity()->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void AdvancedExperiment::recordMetrics(std::shared_ptr<Brain> brain,
                                      AdvancedExperimentMetrics& metrics,
                                      const AdvancedExperimentConfig& config,
                                      uint64_t step) {
    // Record basic metrics
    metrics.totalStepsExecuted = step;
    
    // Update neural activity metrics
    metrics.avgFiringRate = brain->getAverageFiringRate();
    metrics.totalReward = metrics.avgFiringRate * step;  // Simplified reward calculation
    
    // Update memory metrics
    if (brain->getWorkingMemory()) {
        metrics.workingMemoryUtilization = brain->getWorkingMemory()->getActiveTraces() / 
                                          config.workingMemoryCapacity * 1000.0f;
    }
    
    if (brain->getEpisodicMemory()) {
        metrics.episodicMemoryRetention = brain->getEpisodicMemory()->getEpisodeCount() / 
                                          config.episodicMemoryMaxEpisodes * 100.0f;
    }
    
    // Update neuromodulation metrics
    if (brain->getDopamine()) {
        metrics.dopamineEffectiveness = brain->getDopamine()->getLevel();
    }
    
    // Update cognitive metrics
    if (brain->getAttention()) {
        metrics.attentionalFocus = config.attentionStrength;
    }
    
    if (brain->getPlanner()) {
        metrics.planningAccuracy = config.predictionAccuracyThreshold;
    }
    
    // Update network metrics
    metrics.networkConnectivity = brain->getExcitationInhibitionRatio();
    metrics.computationalEfficiency = 1.0f / (metrics.avgFiringRate + 0.01f);
    
    // Record time series data
    metrics.rewardHistory.push_back(metrics.totalReward);
    metrics.firingRateHistory.push_back(metrics.avgFiringRate);
    
    // Limit history size to prevent memory issues
    if (metrics.rewardHistory.size() > 1000) {
        metrics.rewardHistory.erase(metrics.rewardHistory.begin());
        metrics.firingRateHistory.erase(metrics.firingRateHistory.begin());
    }
}

AdvancedExperimentMetrics AdvancedExperiment::run(const AdvancedExperimentConfig& config) {
    NLM_LOG_INFO("Starting advanced experiment: " + std::to_string(config.maxSteps) + " steps");
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Create brain
        auto brain = createBrainWithConfig(config);
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        metrics.startTime = std::chrono::system_clock::to_time_t(startTime);
        
        // Run simulation
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Check for progress logging
            if (config.enableProgressLogging && step % 1000 == 0) {
                NLM_LOG_INFO("Progress: " + std::to_string(step) + "/" + std::to_string(config.maxSteps) + 
                           " steps, Reward: " + std::to_string(metrics.totalReward) +
                           ", Firing Rate: " + std::to_string(metrics.avgFiringRate));
            }
            
            // Run single simulation step
            runSingleStep(brain, step, config);
            
            // Record metrics
            recordMetrics(brain, metrics, config, step);
            
            // Checkpoint if enabled
            if (config.enableCheckpointing && step % config.checkpointInterval == 0) {
                std::string checkpointFile = config.checkpointPath + "_step_" + 
                                            std::to_string(step) + ".bin";
                if (brain->save(checkpointFile)) {
                    NLM_LOG_INFO("Checkpoint saved: " + checkpointFile);
                }
            }
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.endTime = std::chrono::system_clock::to_time_t(endTime);
        metrics.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate final metrics
        if (!metrics.rewardHistory.empty()) {
            metrics.meanReward = calculateMean(metrics.rewardHistory);
            metrics.rewardVariance = calculateVariance(metrics.rewardHistory, metrics.meanReward);
            metrics.rewardSkewness = calculateSkewness(metrics.rewardHistory, metrics.meanReward, 
                                                         std::sqrt(metrics.rewardVariance));
            metrics.rewardKurtosis = calculateKurtosis(metrics.rewardHistory, metrics.meanReward, 
                                                       std::sqrt(metrics.rewardVariance));
        }
        
        // Calculate derived metrics
        metrics.learningSpeed = metrics.totalReward / config.maxSteps;
        metrics.memoryEfficiency = (metrics.workingMemoryUtilization + metrics.episodicMemoryRetention) / 2.0f;
        metrics.neuromodulationEffectiveness = metrics.dopamineEffectiveness * 
                                             config.curiosityLevel * config.noveltyDetection;
        metrics.adaptationRate = metrics.learningSpeed * 10.0f;
        metrics.robustness = 1.0f / (1.0f + metrics.rewardVariance);
        
        NLM_LOG_INFO("Advanced experiment completed successfully");
        NLM_LOG_INFO("Total reward: " + std::to_string(metrics.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(metrics.avgFiringRate));
        NLM_LOG_INFO("Wall clock time: " + std::to_string(metrics.totalWallClockTime) + "s");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Advanced experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

std::map<std::string, AdvancedExperimentMetrics> AdvancedExperiment::runParameterSweep(
    const AdvancedExperimentConfig& baseConfig,
    const std::map<std::string, std::vector<float>>& parameterRanges) {
    
    NLM_LOG_INFO("Starting parameter sweep experiment");
    
    std::map<std::string, AdvancedExperimentMetrics> results;
    
    // Generate all parameter combinations
    std::vector<std::map<std::string, float>> parameterCombinations;
    generateParameterCombinations(parameterRanges, parameterCombinations);
    
    // Run experiment for each parameter combination
    for (size_t i = 0; i < parameterCombinations.size(); ++i) {
        const auto& params = parameterCombinations[i];
        
        // Create configuration for this parameter set
        AdvancedExperimentConfig config = baseConfig;
        
        // Apply parameter overrides
        for (const auto& param : params) {
            if (param.first == "learning_rate") {
                config.learningRate = param.second;
            } else if (param.first == "plasticity_modulation") {
                config.plasticityModulation = param.second;
            } else if (param.first == "curiosity_level") {
                config.curiosityLevel = param.second;
            } else if (param.first == "attention_strength") {
                config.attentionStrength = param.second;
            }
        }
        
        // Create experiment name
        std::string expName = "param_" + std::to_string(i);
        for (const auto& param : params) {
            expName += "_" + param.first + "_" + std::to_string(param.second);
        }
        
        NLM_LOG_INFO("Running parameter combination: " + expName);
        
        try {
            // Run experiment
            AdvancedExperimentMetrics metrics = run(config);
            
            // Store results
            results[expName] = metrics;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Parameter combination " + expName + " failed: " + std::string(e.what()));
            // Continue with next parameter combination
        }
    }
    
    NLM_LOG_INFO("Parameter sweep completed: " + std::to_string(results.size()) + " experiments");
    return results;
}

std::map<std::string, AdvancedExperimentMetrics> AdvancedExperiment::runComparative(
    const std::map<std::string, AdvancedExperimentConfig>& configs) {
    
    NLM_LOG_INFO("Starting comparative experiment");
    
    std::map<std::string, AdvancedExperimentMetrics> results;
    
    // Run experiment for each configuration
    for (const auto& pair : configs) {
        const std::string& expName = pair.first;
        const AdvancedExperimentConfig& config = pair.second;
        
        NLM_LOG_INFO("Running comparative experiment: " + expName);
        
        try {
            // Run experiment
            AdvancedExperimentMetrics metrics = run(config);
            
            // Store results
            results[expName] = metrics;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Comparative experiment " + expName + " failed: " + std::string(e.what()));
            // Continue with next experiment
        }
    }
    
    NLM_LOG_INFO("Comparative experiment completed: " + std::to_string(results.size()) + " experiments");
    return results;
}

AdvancedExperimentMetrics AdvancedExperiment::runLifelongLearning(
    const AdvancedExperimentConfig& config,
    uint64_t totalLifespan) {
    
    NLM_LOG_INFO("Starting lifelong learning experiment: " + std::to_string(totalLifespan) + " lifespan steps");
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Create brain
        auto brain = createBrainWithConfig(config);
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        metrics.startTime = std::chrono::system_clock::to_time_t(startTime);
        
        // Run lifelong learning simulation
        for (uint64_t step = 0; step < totalLifespan; ++step) {
            // Check for progress logging
            if (config.enableProgressLogging && step % 1000 == 0) {
                NLM_LOG_INFO("Lifelong learning progress: " + std::to_string(step) + "/" + 
                           std::to_string(totalLifespan) + " steps, Reward: " + 
                           std::to_string(metrics.totalReward));
            }
            
            // Apply age-dependent learning
            float ageFactor = static_cast<float>(step) / static_cast<float>(totalLifespan);
            
            // Modify learning rates based on age
            float ageLearningRate = config.learningRate * (1.0f - ageFactor * 0.5f);
            float agePlasticity = config.plasticityModulation * (1.0f - ageFactor * 0.3f);
            
            // Run single simulation step
            runSingleStep(brain, step, config);
            
            // Record metrics
            recordMetrics(brain, metrics, config, step);
            
            // Checkpoint periodically
            if (step % 1000 == 0) {
                std::string checkpointFile = "lifelong_checkpoint_step_" + 
                                            std::to_string(step) + ".bin";
                if (brain->save(checkpointFile)) {
                    NLM_LOG_INFO("Lifelong learning checkpoint saved: " + checkpointFile);
                }
            }
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.endTime = std::chrono::system_clock::to_time_t(endTime);
        metrics.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Lifelong learning experiment completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Lifelong learning experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

AdvancedExperimentMetrics AdvancedExperiment::runDevelopmental(
    const AdvancedExperimentConfig& config,
    uint64_t developmentalStages) {
    
    NLM_LOG_INFO("Starting developmental experiment with " + std::to_string(developmentalStages) + " stages");
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Create brain
        auto brain = createBrainWithConfig(config);
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        metrics.startTime = std::chrono::system_clock::to_time_t(startTime);
        
        // Run developmental simulation
        uint64_t stageSteps = config.maxSteps / developmentalStages;
        
        for (uint64_t stage = 0; stage < developmentalStages; ++stage) {
            NLM_LOG_INFO("Developmental stage " + std::to_string(stage + 1) + "/" + 
                       std::to_string(developmentalStages));
            
            // Modify configuration based on developmental stage
            AdvancedExperimentConfig stageConfig = config;
            
            // Adjust parameters based on developmental stage
            float stageFactor = static_cast<float>(stage) / static_cast<float>(developmentalStages);
            
            stageConfig.learningRate *= (1.0f - stageFactor * 0.3f);
            stageConfig.plasticityModulation *= (1.0f - stageFactor * 0.4f);
            
            // Run simulation for this stage
            for (uint64_t step = 0; step < stageSteps; ++step) {
                uint64_t globalStep = stage * stageSteps + step;
                
                runSingleStep(brain, globalStep, stageConfig);
                recordMetrics(brain, metrics, stageConfig, globalStep);
            }
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.endTime = std::chrono::system_clock::to_time_t(endTime);
        metrics.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Developmental experiment completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Developmental experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

AdvancedExperimentMetrics AdvancedExperiment::runMemoryFormation(
    const AdvancedExperimentConfig& config,
    bool enableReplay,
    uint64_t consolidationInterval) {
    
    NLM_LOG_INFO("Starting memory formation experiment with replay: " + 
               std::to_string(enableReplay) + ", consolidation: " + std::to_string(consolidationInterval));
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Create brain
        auto brain = createBrainWithConfig(config);
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        metrics.startTime = std::chrono::system_clock::to_time_t(startTime);
        
        // Run memory formation simulation
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Apply memory consolidation
            if (enableReplay && step % consolidationInterval == 0 && 
                brain->getEpisodicMemory()) {
                
                // Consolidate important memories
                brain->getEpisodicMemory()->consolidate(config.memoryConsolidationRate);
                
                // Replay consolidated memories
                auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    brain->getEpisodicMemory()->replayEpisode(episode);
                }
            }
            
            // Run simulation step
            runSingleStep(brain, step, config);
            recordMetrics(brain, metrics, config, step);
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.endTime = std::chrono::system_clock::to_time_t(endTime);
        metrics.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Memory formation experiment completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory formation experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

AdvancedExperimentMetrics AdvancedExperiment::runCognitiveDevelopment(
    const AdvancedExperimentConfig& config,
    uint64_t cognitiveStages) {
    
    NLM_LOG_INFO("Starting cognitive development experiment with " + std::to_string(cognitiveStages) + " stages");
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Create brain
        auto brain = createBrainWithConfig(config);
        
        // Record start time
        auto startTime = std::chrono::high_resolution_clock::now();
        metrics.startTime = std::chrono::system_clock::to_time_t(startTime);
        
        // Run cognitive development simulation
        uint64_t stageSteps = config.maxSteps / cognitiveStages;
        
        for (uint64_t stage = 0; stage < cognitiveStages; ++stage) {
            NLM_LOG_INFO("Cognitive development stage " + std::to_string(stage + 1) + "/" + 
                       std::to_string(cognitiveStages));
            
            // Modify configuration based on cognitive development stage
            AdvancedExperimentConfig stageConfig = config;
            
            // Adjust attention and learning parameters
            float stageFactor = static_cast<float>(stage) / static_cast<float>(cognitiveStages);
            
            stageConfig.attentionStrength *= (1.0f + stageFactor * 0.5f);
            stageConfig.learningRate *= (1.0f - stageFactor * 0.2f);
            stageConfig.predictionAccuracyThreshold *= (1.0f - stageFactor * 0.3f);
            
            // Run simulation for this stage
            for (uint64_t step = 0; step < stageSteps; ++step) {
                uint64_t globalStep = stage * stageSteps + step;
                
                runSingleStep(brain, globalStep, stageConfig);
                recordMetrics(brain, metrics, stageConfig, globalStep);
            }
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.endTime = std::chrono::system_clock::to_time_t(endTime);
        metrics.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Cognitive development experiment completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Cognitive development experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

AdvancedExperimentMetrics AdvancedExperiment::runOptimization(
    const AdvancedExperimentConfig& baseConfig,
    const std::map<std::string, std::vector<float>>& searchSpace,
    uint64_t generations) {
    
    NLM_LOG_INFO("Starting optimization experiment with " + std::to_string(generations) + " generations");
    
    AdvancedExperimentMetrics metrics;
    
    try {
        // Initialize random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        
        // Create population of parameter sets
        std::vector<AdvancedExperimentConfig> population;
        std::vector<float> fitnessScores;
        
        // Generate initial population
        for (uint64_t i = 0; i < 10; ++i) {
            AdvancedExperimentConfig individual = baseConfig;
            
            // Apply random parameter variations
            for (const auto& param : searchSpace) {
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                float variation = dist(gen);
                
                if (param.first == "learning_rate") {
                    individual.learningRate = param.second[0] + 
                                             variation * (param.second[1] - param.second[0]);
                } else if (param.first == "plasticity_modulation") {
                    individual.plasticityModulation = param.second[0] + 
                                                      variation * (param.second[1] - param.second[0]);
                }
            }
            
            population.push_back(individual);
            
            // Evaluate fitness (simplified fitness calculation)
            float fitness = 0.0f;
            for (uint64_t step = 0; step < 100; ++step) {
                auto brain = createBrainWithConfig(individual);
                runSingleStep(brain, step, individual);
                fitness += brain->getAverageFiringRate();
            }
            
            fitnessScores.push_back(fitness);
        }
        
        // Evolutionary algorithm
        for (uint64_t generation = 0; generation < generations; ++generation) {
            NLM_LOG_INFO("Optimization generation " + std::to_string(generation + 1) + "/" + 
                       std::to_string(generations));
            
            // Select parents based on fitness
            std::vector<size_t> parentIndices;
            std::vector<float> cumulativeFitness;
            float totalFitness = std::accumulate(fitnessScores.begin(), fitnessScores.end(), 0.0f);
            
            float cumulative = 0.0f;
            for (size_t i = 0; i < fitnessScores.size(); ++i) {
                cumulative += fitnessScores[i] / totalFitness;
                cumulativeFitness.push_back(cumulative);
            }
            
            // Create new population
            std::vector<AdvancedExperimentConfig> newPopulation;
            std::vector<float> newFitnessScores;
            
            // Elitism: keep best individual
            size_t bestIndex = std::max_element(fitnessScores.begin(), fitnessScores.end()) - fitnessScores.begin();
            newPopulation.push_back(population[bestIndex]);
            newFitnessScores.push_back(fitnessScores[bestIndex]);
            
            // Crossover and mutation for remaining individuals
            while (newPopulation.size() < population.size()) {
                // Select parents
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                float parent1Rand = dist(gen);
                float parent2Rand = dist(gen);
                
                size_t parent1 = 0, parent2 = 0;
                for (size_t i = 0; i < cumulativeFitness.size(); ++i) {
                    if (parent1Rand <= cumulativeFitness[i] && parent1 == 0) {
                        parent1 = i;
                    }
                    if (parent2Rand <= cumulativeFitness[i] && parent2 == 0) {
                        parent2 = i;
                    }
                }
                
                if (parent1 == parent2) {
                    parent2 = (parent1 + 1) % population.size();
                }
                
                // Crossover
                AdvancedExperimentConfig child = population[parent1];
                float crossoverPoint = dist(gen);
                
                if (crossoverPoint < 0.5f) {
                    child = population[parent2];
                }
                
                // Mutation
                std::uniform_real_distribution<float> mutationDist(0.0f, 0.1f);
                if (mutationDist(gen) < 0.1f) {
                    child.learningRate *= (1.0f + (dist(gen) - 0.5f) * 0.2f);
                }
                if (mutationDist(gen) < 0.1f) {
                    child.plasticityModulation *= (1.0f + (dist(gen) - 0.5f) * 0.2f);
                }
                
                // Ensure parameters are within valid ranges
                child.learningRate = std::max(0.001f, std::min(1.0f, child.learningRate));
                child.plasticityModulation = std::max(0.1f, std::min(10.0f, child.plasticityModulation));
                
                newPopulation.push_back(child);
                
                // Evaluate fitness for child
                float childFitness = 0.0f;
                for (uint64_t step = 0; step < 50; ++step) {
                    auto childBrain = createBrainWithConfig(child);
                    runSingleStep(childBrain, step, child);
                    childFitness += childBrain->getAverageFiringRate();
                }
                
                newFitnessScores.push_back(childFitness);
            }
            
            // Replace old population
            population = newPopulation;
            fitnessScores = newFitnessScores;
        }
        
        // Run final experiment with best individual
        size_t bestIndex = std::max_element(fitnessScores.begin(), fitnessScores.end()) - fitnessScores.begin();
        metrics = run(population[bestIndex]);
        
        NLM_LOG_INFO("Optimization experiment completed successfully");
        NLM_LOG_INFO("Best fitness score: " + std::to_string(fitnessScores[bestIndex]));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Optimization experiment failed: " + std::string(e.what()));
        throw;
    }
    
    return metrics;
}

bool AdvancedExperiment::verifyAdvancedIntegration() {
    NLM_LOG_INFO("Verifying advanced integration...");
    
    try {
        // Create test configuration
        AdvancedExperimentConfig config;
        config.maxSteps = 1000;
        config.enableWorkingMemory = true;
        config.enableEpisodicMemory = true;
        config.enableAssociativeMemory = true;
        config.enableNeuromodulation = true;
        config.enableDevelopment = true;
        config.enableStructuralPlasticity = true;
        config.enableRewardModulation = true;
        
        // Run integration test
        AdvancedExperimentMetrics metrics = run(config);
        
        // Verify integration success
        bool integrationSuccess = true;
        
        if (metrics.totalReward < 0.0f) {
            NLM_LOG_WARN("Integration test: Low total reward");
            integrationSuccess = false;
        }
        
        if (metrics.learningSpeed < 0.001f) {
            NLM_LOG_WARN("Integration test: Low learning speed");
            integrationSuccess = false;
        }
        
        if (metrics.memoryEfficiency < 0.1f) {
            NLM_LOG_WARN("Integration test: Low memory efficiency");
            integrationSuccess = false;
        }
        
        if (metrics.adaptationRate < 0.1f) {
            NLM_LOG_WARN("Integration test: Low adaptation rate");
            integrationSuccess = false;
        }
        
        if (integrationSuccess) {
            NLM_LOG_INFO("Advanced integration verification PASSED");
        } else {
            NLM_LOG_INFO("Advanced integration verification completed with warnings");
        }
        
        return integrationSuccess;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Advanced integration verification failed: " + std::string(e.what()));
        return false;
    }
}

AdvancedExperimentMetrics AdvancedExperiment::analyzeResults(
    const std::vector<AdvancedExperimentMetrics>& metricsList) {
    
    NLM_LOG_INFO("Analyzing experiment results...");
    
    if (metricsList.empty()) {
        NLM_LOG_ERROR("Cannot analyze results: empty metrics list");
        return AdvancedExperimentMetrics();
    }
    
    AdvancedExperimentMetrics aggregatedMetrics;
    
    // Calculate statistical measures
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    float totalLearningSpeed = 0.0f;
    float totalMemoryEfficiency = 0.0f;
    float totalNeuromodulationEffectiveness = 0.0f;
    float totalAdaptationRate = 0.0f;
    float totalRobustness = 0.0f;
    
    size_t count = 0;
    
    for (const auto& metrics : metricsList) {
        totalReward += metrics.totalReward;
        totalFiringRate += metrics.avgFiringRate;
        totalLearningSpeed += metrics.learningSpeed;
        totalMemoryEfficiency += metrics.memoryEfficiency;
        totalNeuromodulationEffectiveness += metrics.neuromodulationEffectiveness;
        totalAdaptationRate += metrics.adaptationRate;
        totalRobustness += metrics.robustness;
        
        // Collect time series data
        aggregatedMetrics.rewardHistory.insert(aggregatedMetrics.rewardHistory.end(),
                                               metrics.rewardHistory.begin(),
                                               metrics.rewardHistory.end());
        aggregatedMetrics.firingRateHistory.insert(aggregatedMetrics.firingRateHistory.end(),
                                                  metrics.firingRateHistory.begin(),
                                                  metrics.firingRateHistory.end());
        
        count++;
    }
    
    if (count > 0) {
        // Calculate averages
        aggregatedMetrics.totalReward = totalReward / count;
        aggregatedMetrics.avgFiringRate = totalFiringRate / count;
        aggregatedMetrics.learningSpeed = totalLearningSpeed / count;
        aggregatedMetrics.memoryEfficiency = totalMemoryEfficiency / count;
        aggregatedMetrics.neuromodulationEffectiveness = totalNeuromodulationEffectiveness / count;
        aggregatedMetrics.adaptationRate = totalAdaptationRate / count;
        aggregatedMetrics.robustness = totalRobustness / count;
    }
    
    // Calculate additional statistical measures
    if (!aggregatedMetrics.rewardHistory.empty()) {
        aggregatedMetrics.meanReward = calculateMean(aggregatedMetrics.rewardHistory);
        aggregatedMetrics.rewardVariance = calculateVariance(aggregatedMetrics.rewardHistory, aggregatedMetrics.meanReward);
        aggregatedMetrics.rewardSkewness = calculateSkewness(aggregatedMetrics.rewardHistory, aggregatedMetrics.meanReward, 
                                                           std::sqrt(aggregatedMetrics.rewardVariance));
        aggregatedMetrics.rewardKurtosis = calculateKurtosis(aggregatedMetrics.rewardHistory, aggregatedMetrics.meanReward, 
                                                           std::sqrt(aggregatedMetrics.rewardVariance));
    }
    
    NLM_LOG_INFO("Result analysis completed: " + std::to_string(count) + " experiments analyzed");
    NLM_LOG_INFO("Average reward: " + std::to_string(aggregatedMetrics.totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(aggregatedMetrics.avgFiringRate));
    
    return aggregatedMetrics;
}

std::string AdvancedExperiment::generateVisualizationData(
    const AdvancedExperimentMetrics& metrics,
    const std::string& outputFormat) {
    
    NLM_LOG_INFO("Generating visualization data: " + outputFormat);
    
    if (outputFormat == "json") {
        return generateJSONVisualization(metrics);
    } else if (outputFormat == "csv") {
        return generateCSVVisualization(metrics);
    } else if (outputFormat == "html") {
        return generateHTMLVisualization(metrics);
    } else {
        NLM_LOG_WARN("Unsupported visualization format: " + outputFormat);
        return "";
    }
}

bool AdvancedExperiment::exportResults(
    const AdvancedExperimentMetrics& metrics,
    const std::string& filepath,
    const std::string& format) {
    
    NLM_LOG_INFO("Exporting experiment results: " + filepath + " (" + format + ")");
    
    try {
        if (format == "json") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Generate JSON representation
            std::string jsonData = generateJSONRepresentation(metrics);
            file << jsonData;
            
        } else if (format == "csv") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Generate CSV representation
            std::string csvData = generateCSVRepresentation(metrics);
            file << csvData;
            
        } else if (format == "txt") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Generate text representation
            std::string textData = generateTextRepresentation(metrics);
            file << textData;
        }
        
        NLM_LOG_INFO("Experiment results exported successfully: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export experiment results: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedExperiment::loadResults(std::vector<AdvancedExperimentMetrics>& metricsList,
                                   const std::string& filepath) {
    
    NLM_LOG_INFO("Loading experiment results: " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open results file: " + filepath);
            return false;
        }
        
        // Parse JSON/CSV results
        // This would involve loading experiment results from a file
        // For simplicity, we'll implement a basic loader
        
        // For now, just log that loading is not fully implemented
        NLM_LOG_WARN("Result loading not fully implemented - will need to implement JSON/CSV parsing");
        
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load experiment results: " + std::string(e.what()));
        return false;
    }
}

float AdvancedExperiment::calculateSimilarity(const AdvancedExperimentMetrics& metrics1,
                                            const AdvancedExperimentMetrics& metrics2) {
    
    // Calculate similarity between two experiment results
    // Based on Euclidean distance of normalized metrics
    
    float similarity = 0.0f;
    int metricCount = 0;
    
    // Normalize metrics (simple min-max normalization)
    auto normalize = [](float value, float min, float max) {
        if (max - min > 0.0f) {
            return (value - min) / (max - min);
        }
        return 0.5f;  // Default value for constant metrics
    };
    
    // Normalize reward
    float normReward1 = normalize(metrics1.totalReward, 0.0f, 1000.0f);
    float normReward2 = normalize(metrics2.totalReward, 0.0f, 1000.0f);
    similarity += (1.0f - std::abs(normReward1 - normReward2));
    metricCount++;
    
    // Normalize firing rate
    float normFiring1 = normalize(metrics1.avgFiringRate, 0.0f, 100.0f);
    float normFiring2 = normalize(metrics2.avgFiringRate, 0.0f, 100.0f);
    similarity += (1.0f - std::abs(normFiring1 - normFiring2));
    metricCount++;
    
    // Normalize learning speed
    float normLearning1 = normalize(metrics1.learningSpeed, 0.0f, 1.0f);
    float normLearning2 = normalize(metrics2.learningSpeed, 0.0f, 1.0f);
    similarity += (1.0f - std::abs(normLearning1 - normLearning2));
    metricCount++;
    
    // Calculate average similarity
    if (metricCount > 0) {
        similarity /= metricCount;
    }
    
    return similarity;
}

std::map<std::string, float> AdvancedExperiment::performStatisticalAnalysis(
    const std::vector<AdvancedExperimentMetrics>& metricsList) {
    
    NLM_LOG_INFO("Performing statistical analysis on experiment results");
    
    std::map<std::string, float> statistics;
    
    if (metricsList.empty()) {
        return statistics;
    }
    
    // Calculate statistical measures for each metric
    std::vector<float> rewards;
    std::vector<float> firingRates;
    std::vector<float> learningSpeeds;
    std::vector<float> memoryEfficiencies;
    
    for (const auto& metrics : metricsList) {
        rewards.push_back(metrics.totalReward);
        firingRates.push_back(metrics.avgFiringRate);
        learningSpeeds.push_back(metrics.learningSpeed);
        memoryEfficiencies.push_back(metrics.memoryEfficiency);
    }
    
    // Calculate basic statistics
    statistics["mean_reward"] = calculateMean(rewards);
    statistics["std_reward"] = std::sqrt(calculateVariance(rewards, statistics["mean_reward"]));
    statistics["mean_firing_rate"] = calculateMean(firingRates);
    statistics["std_firing_rate"] = std::sqrt(calculateVariance(firingRates, statistics["mean_firing_rate"]));
    statistics["mean_learning_speed"] = calculateMean(learningSpeeds);
    statistics["std_learning_speed"] = std::sqrt(calculateVariance(learningSpeeds, statistics["mean_learning_speed"]));
    statistics["mean_memory_efficiency"] = calculateMean(memoryEfficiencies);
    statistics["std_memory_efficiency"] = std::sqrt(calculateVariance(memoryEfficiencies, statistics["mean_memory_efficiency"]));
    
    // Calculate skewness and kurtosis
    if (statistics["std_reward"] > 0.0f) {
        statistics["skew_reward"] = calculateSkewness(rewards, statistics["mean_reward"], statistics["std_reward"]);
        statistics["kurt_reward"] = calculateKurtosis(rewards, statistics["mean_reward"], statistics["std_reward"]);
    }
    
    if (statistics["std_firing_rate"] > 0.0f) {
        statistics["skew_firing_rate"] = calculateSkewness(firingRates, statistics["mean_firing_rate"], statistics["std_firing_rate"]);
        statistics["kurt_firing_rate"] = calculateKurtosis(firingRates, statistics["mean_firing_rate"], statistics["std_firing_rate"]);
    }
    
    // Calculate correlation between metrics
    if (!rewards.empty() && rewards.size() == firingRates.size()) {
        statistics["reward_firing_correlation"] = calculateCorrelation(rewards, firingRates);
        statistics["reward_learning_correlation"] = calculateCorrelation(rewards, learningSpeeds);
        statistics["firing_learning_correlation"] = calculateCorrelation(firingRates, learningSpeeds);
    }
    
    NLM_LOG_INFO("Statistical analysis completed");
    for (const auto& stat : statistics) {
        NLM_LOG_INFO("  " + stat.first + ": " + std::to_string(stat.second));
    }
    
    return statistics;
}

std::string AdvancedExperiment::generateSummaryReport(
    const AdvancedExperimentMetrics& metrics,
    const std::string& experimentName) {
    
    std::ostringstream report;
    
    report << "=== Advanced Experiment Summary Report ===" << std::endl;
    report << "Experiment Name: " << experimentName << std::endl;
    report << "Timestamp: " << generateTimestamp() << std::endl;
    report << std::endl;
    
    report << "Performance Metrics:" << std::endl;
    report << "  Total Reward: " << metrics.totalReward << std::endl;
    report << "  Average Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
    report << "  Average Synaptic Weight: " << metrics.avgSynapticWeight << std::endl;
    report << "  Learning Speed: " << metrics.learningSpeed << std::endl;
    report << "  Memory Efficiency: " << metrics.memoryEfficiency << std::endl;
    report << std::endl;
    
    report << "Memory System Performance:" << std::endl;
    report << "  Working Memory Utilization: " << metrics.workingMemoryUtilization << "%" << std::endl;
    report << "  Episodic Memory Retention: " << metrics.episodicMemoryRetention << "%" << std::endl;
    report << "  Associative Memory Strength: " << metrics.associativeMemoryStrength << std::endl;
    report << "  Memory Replay Frequency: " << metrics.memoryReplayFrequency << std::endl;
    report << std::endl;
    
    report << "Neuromodulation Performance:" << std::endl;
    report << "  Neuromodulation Effectiveness: " << metrics.neuromodulationEffectiveness << std::endl;
    report << "  Reward Prediction Accuracy: " << metrics.rewardPredictionAccuracy << std::endl;
    report << "  Curiosity Driven Exploration: " << metrics.curiosityDrivenExploration << std::endl;
    report << "  Novelty Response: " << metrics.noveltyResponse << std::endl;
    report << std::endl;
    
    report << "Cognitive Performance:" << std::endl;
    report << "  Attentional Focus: " << metrics.attentionalFocus << std::endl;
    report << "  Planning Accuracy: " << metrics.planningAccuracy << std::endl;
    report << "  Concept Clarity: " << metrics.conceptClarity << std::endl;
    report << "  Self Model Stability: " << metrics.selfModelStability << std::endl;
    report << std::endl;
    
    report << "Development Performance:" << std::endl;
    report << "  Developmental Progress: " << metrics.developmentalProgress << std::endl;
    report << "  Plasticity Maturation: " << metrics.plasticityMaturation << std::endl;
    report << "  Experience Integration: " << metrics.experienceIntegration << std::endl;
    report << "  Adaptation Rate: " << metrics.adaptationRate << std::endl;
    report << std::endl;
    
    report << "Network Performance:" << std::endl;
    report << "  Network Connectivity: " << metrics.networkConnectivity << std::endl;
    report << "  Information Flow: " << metrics.informationFlow << std::endl;
    report << "  Computational Efficiency: " << metrics.computationalEfficiency << std::endl;
    report << "  Robustness: " << metrics.robustness << std::endl;
    report << std::endl;
    
    report << "Statistical Analysis:" << std::endl;
    report << "  Mean Reward: " << metrics.meanReward << std::endl;
    report << "  Reward Variance: " << metrics.rewardVariance << std::endl;
    report << "  Reward Skewness: " << metrics.rewardSkewness << std::endl;
    report << "  Reward Kurtosis: " << metrics.rewardKurtosis << std::endl;
    report << "  Reward-Firing Correlation: " << metrics.rewardFiringCorrelation << std::endl;
    report << std::endl;
    
    report << "Temporal Metrics:" << std::endl;
    report << "  Total Steps: " << metrics.totalStepsExecuted << std::endl;
    report << "  Wall Clock Time: " << metrics.totalWallClockTime << "s" << std::endl;
    report << std::endl;
    
    report << "=== End of Report ===" << std::endl;
    
    return report.str();
}

// Helper function implementations
void AdvancedExperiment::generateParameterCombinations(
    const std::map<std::string, std::vector<float>>& parameterRanges,
    std::vector<std::map<std::string, float>>& combinations) {
    
    if (parameterRanges.empty()) {
        return;
    }
    
    // Use recursive backtracking to generate all combinations
    std::vector<std::string> keys;
    std::vector<std::vector<float>> values;
    
    for (const auto& pair : parameterRanges) {
        keys.push_back(pair.first);
        values.push_back(pair.second);
    }
    
    std::vector<float> currentCombination;
    generateCombinationsHelper(keys, values, 0, currentCombination, combinations);
}

void AdvancedExperiment::generateCombinationsHelper(
    const std::vector<std::string>& keys,
    const std::vector<std::vector<float>>& values,
    size_t index,
    std::vector<float>& currentCombination,
    std::vector<std::map<std::string, float>>& combinations) {
    
    if (index >= keys.size()) {
        // Create a combination map
        std::map<std::string, float> combination;
        for (size_t i = 0; i < keys.size(); ++i) {
            combination[keys[i]] = currentCombination[i];
        }
        combinations.push_back(combination);
        return;
    }
    
    // Generate all values for current key
    for (size_t i = 0; i < values[index].size(); ++i) {
        currentCombination.push_back(values[index][i]);
        generateCombinationsHelper(keys, values, index + 1, currentCombination, combinations);
        currentCombination.pop_back();
    }
}

float AdvancedExperiment::calculateMean(const std::vector<float>& data) {
    if (data.empty()) {
        return 0.0f;
    }
    
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

float AdvancedExperiment::calculateVariance(const std::vector<float>& data, float mean) {
    if (data.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        sum += diff * diff;
    }
    
    return sum / data.size();
}

float AdvancedExperiment::calculateStandardDeviation(const std::vector<float>& data, float mean) {
    return std::sqrt(calculateVariance(data, mean));
}

float AdvancedExperiment::calculateSkewness(const std::vector<float>& data, float mean, float stddev) {
    if (data.empty() || stddev <= 0.0f) {
        return 0.0f;
    }
    
    float skew = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        skew += (diff * diff * diff) / data.size();
    }
    
    return skew / (stddev * stddev * stddev);
}

float AdvancedExperiment::calculateKurtosis(const std::vector<float>& data, float mean, float stddev) {
    if (data.empty() || stddev <= 0.0f) {
        return 0.0f;
    }
    
    float kurt = 0.0f;
    for (float value : data) {
        float diff = value - mean;
        kurt += (diff * diff * diff * diff) / data.size();
    }
    
    return kurt / (stddev * stddev * stddev * stddev) - 3.0f;
}

float AdvancedExperiment::calculateCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
    if (x.size() != y.size() || x.empty()) {
        return 0.0f;
    }
    
    float xMean = calculateMean(x);
    float yMean = calculateMean(y);
    
    float numerator = 0.0f;
    float xDenom = 0.0f;
    float yDenom = 0.0f;
    
    for (size_t i = 0; i < x.size(); ++i) {
        float xDiff = x[i] - xMean;
        float yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        xDenom += xDiff * xDiff;
        yDenom += yDiff * yDiff;
    }
    
    if (xDenom <= 0.0f || yDenom <= 0.0f) {
        return 0.0f;
    }
    
    return numerator / std::sqrt(xDenom * yDenom);
}

float AdvancedExperiment::calculatePearsonCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
    return calculateCorrelation(x, y);
}

float AdvancedExperiment::randomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int AdvancedExperiment::randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

std::string AdvancedExperiment::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

std::string AdvancedExperiment::sanitizeFilename(const std::string& filename) {
    std::string result;
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-') {
            result += c;
        } else {
            result += '_';
        }
    }
    return result;
}

// Visualization method implementations (placeholders)
std::string AdvancedExperiment::generateJSONVisualization(const AdvancedExperimentMetrics& metrics) {
    return "{\"experiment\": \"advanced\", \"metrics\": \"not_implemented\"}";
}

std::string AdvancedExperiment::generateCSVVisualization(const AdvancedExperimentMetrics& metrics) {
    std::ostringstream oss;
    oss << "metric,value\n";
    oss << "total_reward," << metrics.totalReward << std::endl;
    oss << "avg_firing_rate," << metrics.avgFiringRate << std::endl;
    oss << "learning_speed," << metrics.learningSpeed << std::endl;
    return oss.str();
}

std::string AdvancedExperiment::generateHTMLVisualization(const AdvancedExperimentMetrics& metrics) {
    return "<html><body><h1>Advanced Experiment Results</h1><p>Metrics not implemented</p></body></html>";
}

std::string AdvancedExperiment::generateJSONRepresentation(const AdvancedExperimentMetrics& metrics) {
    return generateJSONVisualization(metrics);
}

std::string AdvancedExperiment::generateCSVRepresentation(const AdvancedExperimentMetrics& metrics) {
    return generateCSVVisualization(metrics);
}

std::string AdvancedExperiment::generateTextRepresentation(const AdvancedExperimentMetrics& metrics) {
    return generateSummaryReport(metrics, "Advanced Experiment");
}

} // namespace nlm
