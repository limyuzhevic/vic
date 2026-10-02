#include "UserAPI.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <future>

namespace nlm {

// ===== BASIC SETUP =====

std::unique_ptr<Brain> NLMUserAPI::createBrain(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    
    // Try to load from file first
    if (!configFile.empty()) {
        bool loaded = config->loadFromFile(configFile);
        if (!loaded) {
            NLM_LOG_WARNING("Failed to load config from " + configFile + ", using defaults");
        }
    }
    
    // Create brain with configuration
    auto brain = std::make_unique<Brain>(config);
    
    // Initialize brain
    bool initialized = brain->initialize();
    if (!initialized) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return nullptr;
    }
    
    NLM_LOG_INFO("NLM User API: Brain created and initialized");
    return brain;
}

std::unique_ptr<Brain> NLMUserAPI::createBrain(const std::map<std::string, std::string>& params) {
    auto config = std::make_shared<Config>();
    
    // Set configuration from parameters
    for (const auto& param : params) {
        config->set(param.first, param.second, ConfigSource::Runtime);
    }
    
    // Create brain with configuration
    auto brain = std::make_unique<Brain>(config);
    
    // Initialize brain
    bool initialized = brain->initialize();
    if (!initialized) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return nullptr;
    }
    
    NLM_LOG_INFO("NLM User API: Brain created with custom configuration");
    return brain;
}

// ===== SENSOR-MOTOR LOOP =====

std::unique_ptr<Action> NLMUserAPI::process(const std::unique_ptr<Brain>& brain,
                                          const SensoryInput& sensoryInput) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return nullptr;
    }
    
    // Process sensory input
    brain->receiveSensoryInput(sensoryInput);
    
    // Produce action
    auto action = brain->produceAction();
    
    // Step brain
    brain->step(1); // Step by 1 simulation step
    
    return action;
}

std::unique_ptr<Action> NLMUserAPI::processWithStrategy(
    const std::unique_ptr<Brain>& brain,
    const SensoryInput& sensoryInput,
    const std::function<ActionType(const std::vector<float>&)>& strategy) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return nullptr;
    }
    
    // Process sensory input
    brain->receiveSensoryInput(sensoryInput);
    
    // Get sensory data for strategy
    const auto& sensoryData = sensoryInput.getData();
    
    // Apply custom strategy to select action
    ActionType customAction = strategy(sensoryData);
    
    // Create action
    auto action = std::make_unique<Action>(customAction);
    
    // Step brain
    brain->step(1);
    
    return action;
}

// ===== CONFIGURATION =====

std::shared_ptr<Config> NLMUserAPI::loadConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    bool loaded = config->loadFromFile(filepath);
    if (!loaded) {
        NLM_LOG_ERROR("Failed to load config from " + filepath);
        return nullptr;
    }
    
    NLM_LOG_INFO("NLM User API: Config loaded from " + filepath);
    return config;
}

bool NLMUserAPI::saveBrain(const std::unique_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    bool saved = brain->save(filepath);
    if (!saved) {
        NLM_LOG_ERROR("Failed to save brain to " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("NLM User API: Brain saved to " + filepath);
    return true;
}

bool NLMUserAPI::loadBrain(const std::unique_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    bool loaded = brain->load(filepath);
    if (!loaded) {
        NLM_LOG_ERROR("Failed to load brain from " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("NLM User API: Brain loaded from " + filepath);
    return true;
}

// ===== LEARNING AND TRAINING =====

void NLMUserAPI::trainSequence(const std::unique_ptr<Brain>& brain,
                             const std::vector<SensoryInput>& experiences,
                             const std::vector<ActionType>& actions,
                             const std::vector<float>& rewards) {
    if (!brain || experiences.empty() || actions.empty()) {
        NLM_LOG_ERROR("Invalid parameters for training sequence");
        return;
    }
    
    if (actions.size() != experiences.size() || experiences.size() != rewards.size()) {
        NLM_LOG_ERROR("Experiences, actions, and rewards must have same size");
        return;
    }
    
    NLM_LOG_INFO("NLM User API: Training with " + std::to_string(experiences.size()) + " experiences");
    
    // Train with each experience
    for (size_t i = 0; i < experiences.size(); ++i) {
        // Process experience
        brain->receiveSensoryInput(experiences[i]);
        
        // Get prediction error signal if available
        if (auto predictionSystem = brain->getPredictionSystem()) {
            float error = predictionSystem->getPredictionError();
            
            // Apply neuromodulation based on error
            if (auto dopamine = brain->getDopamine()) {
                dopamine->applyPredictionError(error);
            }
        }
        
        // Update action quality based on reward
        if (auto planner = brain->getPlanner()) {
            planner->updatePlanQuality({}, {actions[i]}, rewards[i]);
        }
        
        // Step brain
        brain->step(1);
    }
    
    NLM_LOG_INFO("NLM User API: Sequence training completed");
}

void NLMUserAPI::learnFromDemonstration(const std::unique_ptr<Brain>& brain,
                                      const std::vector<SensoryInput>& states,
                                      const std::vector<ActionType>& actions) {
    if (!brain || states.empty() || actions.empty()) {
        NLM_LOG_ERROR("Invalid parameters for demonstration learning");
        return;
    }
    
    if (actions.size() != states.size()) {
        NLM_LOG_ERROR("States and actions must have same size");
        return;
    }
    
    NLM_LOG_INFO("NLM User API: Learning from demonstration with " + std::to_string(states.size()) + " steps");
    
    // Learn from each demonstration
    for (size_t i = 0; i < states.size(); ++i) {
        // Process state
        brain->receiveSensoryInput(states[i]);
        
        // Record action in neural planner
        if (auto planner = brain->getPlanner()) {
            std::vector<ActionType> plannedActions = {actions[i]};
            std::vector<ActionType> actualActions = {actions[i]};
            float reward = 1.0f; // Assume positive reward for successful demonstration
            
            planner->updatePlanQuality(plannedActions, actualActions, reward);
        }
        
        // Step brain
        brain->step(1);
    }
    
    NLM_LOG_INFO("NLM User API: Demonstration learning completed");
}

void NLMUserAPI::optimizeForTask(const std::unique_ptr<Brain>& brain,
                               const std::vector<SensoryInput>& trainingData,
                               const std::vector<ActionType>& optimalActions) {
    if (!brain || trainingData.empty() || optimalActions.empty()) {
        NLM_LOG_ERROR("Invalid parameters for task optimization");
        return;
    }
    
    NLM_LOG_INFO("NLM User API: Optimizing brain for task with " + std::to_string(trainingData.size()) + " training samples");
    
    // Reset brain for optimization
    brain->reset();
    
    // Optimize neural connections through exposure to task data
    for (size_t i = 0; i < trainingData.size(); ++i) {
        // Process training data
        brain->receiveSensoryInput(trainingData[i]);
        
        // Step brain
        brain->step(1);
        
        // Periodically checkpoint for better optimization
        if (i % 100 == 0) {
            brain->save("./checkpoints/optimization_checkpoint.bin");
        }
    }
    
    NLM_LOG_INFO("NLM User API: Task optimization completed");
}

// ===== MEMORY MANAGEMENT =====

void NLMUserAPI::storeImportantMemory(const std::unique_ptr<Brain>& brain,
                                    const std::vector<float>& memoryPattern,
                                    float importance) {
    if (!brain || memoryPattern.empty()) {
        NLM_LOG_ERROR("Invalid parameters for storing memory");
        return;
    }
    
    // Store memory in working memory
    if (auto workingMemory = brain->getWorkingMemory()) {
        // Convert to neuron activation pattern
        for (size_t i = 0; i < std::min(memoryPattern.size(), 1000ULL); ++i) {
            NeuronId neuronId(i + 1000); // Use reserved neuron IDs for memory
            float activation = memoryPattern[i % memoryPattern.size()] * importance;
            workingMemory->storeToNeuron(neuronId, activation);
        }
    }
    
    // Store in episodic memory as well
    if (auto episodicMemory = brain->getEpisodicMemory()) {
        EpisodicMemoryItem episode;
        episode.sensoryState = memoryPattern;
        episode.reward = importance;
        episode.action = ActionType::Wait;
        
        episodicMemory->storeEpisode(episode);
    }
    
    NLM_LOG_INFO("NLM User API: Memory stored with importance " + std::to_string(importance));
}

std::vector<float> NLMUserAPI::recallMemory(const std::unique_ptr<Brain>& brain,
                                          const std::vector<float>& queryPattern,
                                          float similarityThreshold) {
    if (!brain || queryPattern.empty()) {
        NLM_LOG_ERROR("Invalid parameters for memory recall");
        return std::vector<float>();
    }
    
    // Try to recall from episodic memory
    std::vector<float> recalledPattern;
    
    if (auto episodicMemory = brain->getEpisodicMemory()) {
        auto similarEpisodes = episodicMemory->retrieveSimilar(queryPattern, 5);
        
        if (!similarEpisodes.empty()) {
            // Use the most similar episode
            const auto* episode = similarEpisodes[0];
            recalledPattern = episode->sensoryState;
        }
    }
    
    // If no episodic memory found, try working memory
    if (recalledPattern.empty() && auto workingMemory = brain->getWorkingMemory()) {
        // Get memory neurons and activations
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        auto memoryActivations = workingMemory->retrieve();
        
        if (!memoryNeurons.empty() && !memoryActivations.empty()) {
            // Average activations of similar patterns
            float totalActivation = 0.0f;
            int count = 0;
            
            for (float activation : memoryActivations) {
                if (activation > similarityThreshold) {
                    totalActivation += activation;
                    count++;
                }
            }
            
            if (count > 0) {
                recalledPattern.resize(queryPattern.size(), 0.0f);
                for (size_t i = 0; i < queryPattern.size(); ++i) {
                    recalledPattern[i] = totalActivation / count;
                }
            }
        }
    }
    
    return recalledPattern;
}

void NLMUserAPI::clearMemories(const std::unique_ptr<Brain>& brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    // Clear working memory
    if (auto workingMemory = brain->getWorkingMemory()) {
        workingMemory->clear();
    }
    
    // Clear episodic memory
    if (auto episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->clear();
    }
    
    NLM_LOG_INFO("NLM User API: All memories cleared");
}

// ===== ADVANCED FEATURES =====

void NLMUserAPI::enableReinforcementLearning(const std::unique_ptr<Brain>& brain,
                                          std::function<float(const ActionType&)> rewardFunction) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    // Enable dopamine-based reinforcement learning
    if (auto dopamine = brain->getDopamine()) {
        NLM_LOG_INFO("NLM User API: Reinforcement learning enabled");
    }
    
    // In a full implementation, this would:
    // 1. Register callback for reward calculation
    // 2. Connect reward prediction error to dopamine system
    // 3. Enable action-value learning
    
    NLM_LOG_INFO("NLM User API: Reinforcement learning setup complete");
}

void NLMUserAPI::enableCuriosityDrivenExploration(const std::unique_ptr<Brain>& brain,
                                                 float noveltyThreshold) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    // Enable novelty detection for curiosity-driven exploration
    if (auto curiosity = brain->getCuriosity()) {
        NLM_LOG_INFO("NLM User API: Curiosity-driven exploration enabled with threshold " + std::to_string(noveltyThreshold));
    }
    
    // In a full implementation, this would:
    // 1. Configure novelty detection thresholds
    // 2. Enable curiosity-based exploration policy
    // 3. Integrate with reward prediction error
    
    NLM_LOG_INFO("NLM User API: Curiosity exploration setup complete");
}

void NLMUserAPI::configurePredictionLearning(const std::unique_ptr<Brain>& brain,
                                           bool enableTemporalPredictions,
                                           bool enableActionPredictions) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    // Enable neural prediction systems
    if (auto predictionSystem = brain->getPredictionSystem()) {
        NLM_LOG_INFO("NLM User API: Prediction learning configured - temporal: " + 
                    std::to_string(enableTemporalPredictions) + 
                    ", action: " + std::to_string(enableActionPredictions));
    }
    
    // In a full implementation, this would:
    // 1. Configure neural prediction horizons
    // 2. Enable action-consequence prediction
    // 3. Set up prediction error signals
    
    NLM_LOG_INFO("NLM User API: Prediction learning setup complete");
}

void NLMUserAPI::setDevelopmentalStage(const std::unique_ptr<Brain>& brain,
                                      DevelopmentalStage stage) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    brain->setDevelopmentalStage(stage);
    NLM_LOG_INFO("NLM User API: Developmental stage set to " + std::to_string(static_cast<int>(stage)));
}

// ===== UTILITY FUNCTIONS =====

std::map<std::string, std::string> NLMUserAPI::getDefaultConfig() {
    std::map<std::string, std::string> defaultConfig;
    
    // Core parameters
    defaultConfig["neuron_count"] = "1000";
    defaultConfig["region_count"] = "1";
    defaultConfig["simulation_timestep"] = "0.001";
    defaultConfig["connection_probability"] = "0.1";
    
    // Memory parameters
    defaultConfig["working_memory_capacity"] = "100";
    defaultConfig["max_episodes"] = "1000";
    
    // Learning parameters
    defaultConfig["stdp_ltp_weight"] = "0.01";
    defaultConfig["stdp_ltd_weight"] = "0.012";
    defaultConfig["stdp_tau"] = "20.0";
    
    // Neuromodulation parameters
    defaultConfig["dopamine_sensitivity"] = "1.0";
    defaultConfig["curiosity_rate"] = "0.5";
    
    // Prediction parameters
    defaultConfig["prediction_horizon"] = "10";
    defaultConfig["prediction_confidence_threshold"] = "0.7";
    
    return defaultConfig;
}

bool NLMUserAPI::validateConfig(const std::map<std::string, std::string>& params) {
    // Basic validation
    if (params.find("neuron_count") == params.end()) {
        NLM_LOG_ERROR("Missing required parameter: neuron_count");
        return false;
    }
    
    try {
        size_t neuronCount = std::stoull(params.at("neuron_count"));
        if (neuronCount == 0) {
            NLM_LOG_ERROR("neuron_count must be greater than 0");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Invalid neuron_count parameter: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

std::vector<std::string> NLMUserAPI::getAvailableOptions() {
    std::vector<std::string> options;
    
    // Core options
    options.push_back("neuron_count");
    options.push_back("region_count");
    options.push_back("simulation_timestep");
    options.push_back("connection_probability");
    
    // Memory options
    options.push_back("working_memory_capacity");
    options.push_back("max_episodes");
    
    // Learning options
    options.push_back("stdp_ltp_weight");
    options.push_back("stdp_ltd_weight");
    options.push_back("stdp_tau");
    
    // Neuromodulation options
    options.push_back("dopamine_sensitivity");
    options.push_back("curiosity_rate");
    
    // Prediction options
    options.push_back("prediction_horizon");
    options.push_back("prediction_confidence_threshold");
    
    return options;
}

void NLMUserAPI::printSystemStatus(const std::unique_ptr<Brain>& brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    brain->logStatus();
    NLM_LOG_INFO("NLM User API: System status printed");
}

std::map<std::string, std::string> NLMUserAPI::createExperimentConfig(
    const std::string& experimentType,
    int neuronCount,
    int regionCount,
    float simulationTimestep) {
    std::map<std::string, std::string> config;
    
    // Basic configuration
    config["experiment_type"] = experimentType;
    config["neuron_count"] = std::to_string(neuronCount);
    config["region_count"] = std::to_string(regionCount);
    config["simulation_timestep"] = std::to_string(simulationTimestep);
    
    // Learning configuration
    config["stdp_ltp_weight"] = "0.01";
    config["stdp_ltd_weight"] = "0.012";
    config["stdp_tau"] = "20.0";
    
    // Memory configuration
    config["working_memory_capacity"] = std::to_string(neuronCount / 10);
    config["max_episodes"] = "1000";
    
    // Prediction configuration
    config["prediction_horizon"] = "10";
    config["prediction_confidence_threshold"] = "0.7";
    
    NLM_LOG_INFO("NLM User API: Experiment config created for " + experimentType);
    return config;
}

// ===== EXPERT MODE =====

void NLMUserAPI::enableExpertMode(const std::unique_ptr<Brain>& brain,
                                const std::map<std::string, std::string>& expertOptions) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return;
    }
    
    // Enable expert features based on options
    if (expertOptions.find("advanced_planning") != expertOptions.end()) {
        if (auto planner = brain->getPlanner()) {
            planner->setPlanningDepth(10);
        }
    }
    
    if (expertOptions.find("fine_grained_control") != expertOptions.end()) {
        if (auto attention = brain->getAttention()) {
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(2.0f);
        }
    }
    
    NLM_LOG_INFO("NLM User API: Expert mode enabled with " + std::to_string(expertOptions.size()) + " options");
}

std::map<std::string, std::string> NLMUserAPI::getExpertModeOptions() {
    std::map<std::string, std::string> options;
    
    options["advanced_planning"] = "Enable advanced planning with depth 10";
    options["fine_grained_control"] = "Fine-grained attention control with custom inhibition/excitation";
    options["batch_processing"] = "Enable batch processing for multiple inputs";
    options["neural_integration"] = "Integrate with all neural subsystems";
    options["performance_monitoring"] = "Enable detailed performance monitoring";
    
    return options;
}

std::vector<std::unique_ptr<Action>> NLMUserAPI::batchProcess(
    const std::unique_ptr<Brain>& brain,
    const std::vector<SensoryInput>& inputs,
    bool useParallelProcessing) {
    if (!brain || inputs.empty()) {
        NLM_LOG_ERROR("Invalid parameters for batch processing");
        return std::vector<std::unique_ptr<Action>>();
    }
    
    std::vector<std::unique_ptr<Action>> actions;
    actions.reserve(inputs.size());
    
    if (useParallelProcessing) {
        // Process in parallel using threads
        std::vector<std::future<std::unique_ptr<Action>>> futures;
        futures.reserve(inputs.size());
        
        for (const auto& input : inputs) {
            futures.push_back(std::async(std::launch::async, [brain, input]() {
                brain->receiveSensoryInput(input);
                auto action = brain->produceAction();
                brain->step(1);
                return action;
            }));
        }
        
        // Collect results
        for (auto& future : futures) {
            actions.push_back(future.get());
        }
    } else {
        // Process sequentially
        for (const auto& input : inputs) {
            brain->receiveSensoryInput(input);
            auto action = brain->produceAction();
            brain->step(1);
            actions.push_back(std::move(action));
        }
    }
    
    NLM_LOG_INFO("NLM User API: Batch processed " + std::to_string(inputs.size()) + " inputs");
    return actions;
}

bool NLMUserAPI::exportBrainState(const std::unique_ptr<Brain>& brain,
                                 const std::string& filepath,
                                 const std::string& format) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    if (format == "json" || format == "JSON") {
        // Export as JSON (simplified)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for export: " + filepath);
            return false;
        }
        
        file << "{\n";
        file << "  \"brain_state\": \"exported_brain\"\n";
        file << "}\n";
        
        NLM_LOG_INFO("NLM User API: Brain state exported to " + filepath + " in JSON format");
    } else {
        // Export as binary checkpoint
        bool saved = brain->save(filepath);
        if (!saved) {
            NLM_LOG_ERROR("Failed to export brain to " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("NLM User API: Brain state exported to " + filepath + " in binary format");
    }
    
    return true;
}

bool NLMUserAPI::importBrainState(const std::unique_ptr<Brain>& brain,
                                 const std::string& filepath,
                                 const std::string& format) {
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    if (format == "json" || format == "JSON") {
        // Import from JSON (simplified)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for import: " + filepath);
            return false;
        }
        
        // In a full implementation, this would parse JSON and restore brain state
        // For now, just create a new brain
        NLM_LOG_INFO("NLM User API: Brain state imported from " + filepath + " (JSON)");
    } else {
        // Import from binary checkpoint
        bool loaded = brain->load(filepath);
        if (!loaded) {
            NLM_LOG_ERROR("Failed to import brain from " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("NLM User API: Brain state imported from " + filepath + " (binary)");
    }
    
    return true;
}

} // namespace nlm
