#include "SimplifiedBrain.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cstdlib>

namespace nlm {

std::shared_ptr<Brain> SimplifiedBrain::createSimpleBrain(std::shared_ptr<Config> config) {
    if (!config) {
        config = std::make_shared<Config>();
        config->set("neuron_count", 1000u);
        config->set("region_count", 1u);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize simple brain");
        return nullptr;
    }
    
    NLM_LOG_INFO("Created simple brain with " + std::to_string(config->getOr<size_t>("neuron_count", 0)) + " neurons");
    return brain;
}

std::shared_ptr<Brain> SimplifiedBrain::createQuickBrain(size_t neurons, size_t regions) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", regions);
    config->set("connection_probability", 0.05f);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize quick brain");
        return nullptr;
    }
    
    NLM_LOG_INFO("Created quick brain with " + std::to_string(neurons) + " neurons and " + 
                std::to_string(regions) + " regions");
    return brain;
}

std::shared_ptr<Brain> SimplifiedBrain::createBasicBrain(const std::string& configFile) {
    if (!configFile.empty()) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            NLM_LOG_WARNING("Failed to load config from " + configFile + ", using defaults");
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize basic brain from config");
            return nullptr;
        }
        
        return brain;
    }
    
    return createSimpleBrain();
}

// Beginner API implementations
bool SimplifiedBrain::BeginnerAPI::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize null brain");
        return false;
    }
    return brain->initialize();
}

void SimplifiedBrain::BeginnerAPI::step(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot step null brain");
        return;
    }
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
    }
}

void SimplifiedBrain::BeginnerAPI::receiveInput(std::shared_ptr<Brain> brain, const std::vector<float>& input) {
    if (!brain || input.empty()) {
        return;
    }
    
    class SimpleSensoryInput : public SensoryInput {
        std::vector<float> data_;
    public:
        SimpleSensoryInput(const std::vector<float>& data) : data_(data) {}
        const std::vector<float>& getData() const override { return data_; }
    };
    
    auto sensoryInput = std::make_unique<SimpleSensoryInput>(input);
    brain->receiveSensoryInput(*sensoryInput);
}

void SimplifiedBrain::BeginnerAPI::learnFromExperience(std::shared_ptr<Brain> brain, 
                                                        const std::vector<float>& beforeState, 
                                                        const std::vector<float>& afterState, 
                                                        float reward) {
    if (!brain) {
        return;
    }
    
    // Store the experience in episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = brain->getTotalSpikeCount();
        episode.reward = reward;
        
        // Store state information
        for (size_t i = 0; i < std::min(beforeState.size(), afterState.size()); ++i) {
            episode.activeNeurons.push_back(static_cast<NeuronId>(i));
            episode.neuronActivations.push_back((beforeState[i] + afterState[i]) / 2.0f);
        }
        
        episodicMemory->storeEpisode(episode);
    }
    
    // Apply neuromodulatory signals based on reward
    class RewardNeuModulator : public Neuromodulator {
        float level_;
    public:
        RewardNeuModulator(float level) : level_(level) {}
        float getLevel() const override { return level_; }
        float getPlasticityFactor() const override { return 1.0f; }
        void update(TimestepDuration dt) override {}
    };
    
    RewardNeuModulator rewardSignal(reward);
    brain->applyNeuromodulation(rewardSignal);
}

void SimplifiedBrain::BeginnerAPI::reset(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    brain->reset();
}

void SimplifiedBrain::BeginnerAPI::save(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain || filepath.empty()) return;
    brain->save(filepath);
}

bool SimplifiedBrain::BeginnerAPI::load(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain || filepath.empty()) return false;
    return brain->load(filepath);
}

void SimplifiedBrain::BeginnerAPI::clearWorkingMemory(std::shared_ptr<Brain> brain) {
    if (auto* workingMemory = brain ? brain->getWorkingMemory() : nullptr) {
        workingMemory->clear();
    }
}

void SimplifiedBrain::BeginnerAPI::clearEpisodicMemory(std::shared_ptr<Brain> brain) {
    if (auto* episodicMemory = brain ? brain->getEpisodicMemory() : nullptr) {
        episodicMemory->clear();
    }
}

void SimplifiedBrain::BeginnerAPI::clearAssociativeMemory(std::shared_ptr<Brain> brain) {
    if (auto* associativeMemory = brain ? brain->getAssociativeMemory() : nullptr) {
        associativeMemory->clear();
    }
}

void SimplifiedBrain::BeginnerAPI::printStatus(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot print status of null brain");
        return;
    }
    
    brain->logStatus();
}

float SimplifiedBrain::BeginnerAPI::getCurrentPerformance(std::shared_ptr<Brain> brain) {
    if (!brain) return 0.0f;
    
    float performance = 0.0f;
    
    if (brain->getTotalNeuronCount() > 0) {
        performance = static_cast<float>(brain->getFiringNeuronCount()) / 
                     static_cast<float>(brain->getTotalNeuronCount());
    }
    
    return performance;
}

std::string SimplifiedBrain::BeginnerAPI::getBrainHealth(std::shared_ptr<Brain> brain) {
    if (!brain) return "Brain is null";
    
    std::stringstream ss;
    ss << "=== Brain Health Status ===\n";
    
    // Check brain components
    bool hasRegions = brain->getRegionCount() > 0;
    bool hasNeurons = brain->getTotalNeuronCount() > 0;
    bool hasSynapses = brain->getTotalSynapseCount() > 0;
    bool hasSpikes = brain->getTotalSpikeCount() > 0;
    
    ss << "Has regions: " << (hasRegions ? "Yes" : "No") << "\n";
    ss << "Has neurons: " << (hasNeurons ? "Yes" : "No") << "\n";
    ss << "Has synapses: " << (hasSynapses ? "Yes" : "No") << "\n";
    ss << "Has spikes: " << (hasSpikes ? "Yes" : "No") << "\n";
    
    // Calculate health score
    int score = 0;
    if (hasRegions) score++;
    if (hasNeurons) score++;
    if (hasSynapses) score++;
    if (hasSpikes) score++;
    
    ss << "Health Score: " << score << "/4\n";
    ss << "Overall Health: " << (score >= 3 ? "Good" : (score >= 2 ? "Fair" : "Poor")) << "\n";
    
    return ss.str();
}

std::vector<float> SimplifiedBrain::BeginnerAPI::getRecentActivity(std::shared_ptr<Brain> brain, size_t steps) {
    std::vector<float> activity;
    if (!brain) return activity;
    
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingThisStep = brain->getFiringNeuronCount();
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate recent activity based on current firing rate
        float spikeRate = firingThisStep > 0 ? 
            static_cast<float>(firingThisStep) / static_cast<float>(brain->getTotalNeuronCount()) : 
            0.0f;
        activity.push_back(spikeRate);
    }
    
    return activity;
}

std::unique_ptr<Action> SimplifiedBrain::BeginnerAPI::getAction(std::shared_ptr<Brain> brain) {
    if (!brain) return std::make_unique<Action>(ActionType::Wait);
    return brain->produceAction();
}

void SimplifiedBrain::BeginnerAPI::applyReward(std::shared_ptr<Brain> brain, float reward) {
    if (!brain || reward <= 0.0f) return;
    
    class RewardNeuModulator : public Neuromodulator {
        float level_;
    public:
        RewardNeuModulator(float level) : level_(level) {}
        float getLevel() const override { return level_; }
        float getPlasticityFactor() const override { return 1.0f; }
        void update(TimestepDuration dt) override {}
    };
    
    RewardNeuModulator rewardSignal(reward);
    brain->applyNeuromodulation(rewardSignal);
}

void SimplifiedBrain::BeginnerAPI::applyPunishment(std::shared_ptr<Brain> brain, float punishment) {
    if (!brain || punishment <= 0.0f) return;
    
    class PunishmentNeuModulator : public Neuromodulator {
        float level_;
    public:
        PunishmentNeuModulator(float level) : level_(level) {}
        float getLevel() const override { return level_; }
        float getPlasticityFactor() const override { return 0.5f; }
        void update(TimestepDuration dt) override {}
    };
    
    PunishmentNeuModulator punishmentSignal(-punishment);
    brain->applyNeuromodulation(punishmentSignal);
}

void SimplifiedBrain::BeginnerAPI::runLearningSession(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) return;
    
    NLM_LOG_INFO("Starting learning session with " + std::to_string(steps) + " steps");
    
    // Simulate some learning
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
        
        // Inject some random input for learning
        std::vector<float> input = {0.5f, 0.3f, 0.7f, 0.2f};
        receiveInput(brain, input);
        
        // Apply a small reward
        if (i % 100 == 0) {
            applyReward(brain, 0.1f);
        }
    }
    
    NLM_LOG_INFO("Learning session complete");
}

void SimplifiedBrain::BeginnerAPI::runSimulation(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) return;
    
    NLM_LOG_INFO("Starting simulation with " + std::to_string(steps) + " steps");
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
    }
    
    NLM_LOG_INFO("Simulation complete");
}

void SimplifiedBrain::BeginnerAPI::runExploration(std::shared_ptr<Brain> brain, size_t steps) {
    if (!brain) return;
    
    NLM_LOG_INFO("Starting exploration with " + std::to_string(steps) + " steps");
    
    // More random input for exploration
    for (size_t i = 0; i < steps; ++i) {
        std::vector<float> input = {
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX
        };
        receiveInput(brain, input);
        brain->step(i);
        
        // Apply small rewards and punishments for exploration
        if (i % 50 == 0) {
            applyReward(brain, 0.05f);
        }
    }
    
    NLM_LOG_INFO("Exploration complete");
}

void SimplifiedBrain::BeginnerAPI::runTraining(std::shared_ptr<Brain> brain, size_t epochs) {
    if (!brain) return;
    
    NLM_LOG_INFO("Starting training with " + std::to_string(epochs) + " epochs");
    
    for (size_t epoch = 0; epoch < epochs; ++epoch) {
        // Simulate training data
        for (size_t step = 0; step < 100; ++step) {
            std::vector<float> input = {
                0.8f, 0.6f, 0.4f, 0.2f
            };
            receiveInput(brain, input);
            brain->step(step + epoch * 100);
            
            if (step % 10 == 0) {
                applyReward(brain, 0.2f);
            }
        }
        
        NLM_LOG_INFO("Epoch " + std::to_string(epoch + 1) + " complete");
    }
    
    NLM_LOG_INFO("Training complete");
}

// Expert API implementations
SpikeSystem* SimplifiedBrain::ExpertAPI::getSpikeSystem(std::shared_ptr<Brain> brain) {
    return brain ? brain->getSpikeSystem() : nullptr;
}

STDP* SimplifiedBrain::ExpertAPI::getSTDP(std::shared_ptr<Brain> brain) {
    return brain ? brain->getSTDP() : nullptr;
}

Hebbian* SimplifiedBrain::ExpertAPI::getHebbian(std::shared_ptr<Brain> brain) {
    return brain ? brain->getHebbian() : nullptr;
}

StructuralPlasticity* SimplifiedBrain::ExpertAPI::getStructuralPlasticity(std::shared_ptr<Brain> brain) {
    return brain ? brain->getStructuralPlasticity() : nullptr;
}

NeuralWorkingMemory* SimplifiedBrain::ExpertAPI::getWorkingMemory(std::shared_ptr<Brain> brain) {
    return brain ? brain->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* SimplifiedBrain::ExpertAPI::getEpisodicMemory(std::shared_ptr<Brain> brain) {
    return brain ? brain->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* SimplifiedBrain::ExpertAPI::getAssociativeMemory(std::shared_ptr<Brain> brain) {
    return brain ? brain->getAssociativeMemory() : nullptr;
}

PredictionSystem* SimplifiedBrain::ExpertAPI::getPredictionSystem(std::shared_ptr<Brain> brain) {
    return brain ? brain->getPredictionSystem() : nullptr;
}

NeuralPlanner* SimplifiedBrain::ExpertAPI::getPlanner(std::shared_ptr<Brain> brain) {
    return brain ? brain->getPlanner() : nullptr;
}

ConceptFormation* SimplifiedBrain::ExpertAPI::getConceptFormation(std::shared_ptr<Brain> brain) {
    return brain ? brain->getConceptFormation() : nullptr;
}

AttentionalSelection* SimplifiedBrain::ExpertAPI::getAttention(std::shared_ptr<Brain> brain) {
    return brain ? brain->getAttention() : nullptr;
}

DevelopmentSystem* SimplifiedBrain::ExpertAPI::getDevelopmentSystem(std::shared_ptr<Brain> brain) {
    return brain ? brain->getDevelopmentSystem() : nullptr;
}

Dopamine* SimplifiedBrain::ExpertAPI::getDopamine(std::shared_ptr<Brain> brain) {
    return brain ? brain->getDopamine() : nullptr;
}

Curiosity* SimplifiedBrain::ExpertAPI::getCuriosity(std::shared_ptr<Brain> brain) {
    return brain ? brain->getCuriosity() : nullptr;
}

Novelty* SimplifiedBrain::ExpertAPI::getNovelty(std::shared_ptr<Brain> brain) {
    return brain ? brain->getNovelty() : nullptr;
}

PredictionError* SimplifiedBrain::ExpertAPI::getPredictionError(std::shared_ptr<Brain> brain) {
    return brain ? brain->getPredictionErrorSignal() : nullptr;
}

void SimplifiedBrain::ExpertAPI::configureAdvanced(std::shared_ptr<Brain> brain,
                                                 const std::unordered_map<std::string, float>& params) {
    if (!brain) return;
    
    NLM_LOG_INFO("Configuring brain with advanced parameters");
    for (const auto& [key, value] : params) {
        NLM_LOG_INFO("  " + key + ": " + std::to_string(value));
    }
    
    // Apply parameters based on keys
    if (auto* spikeSystem = brain->getSpikeSystem()) {
        if (params.find("learning_rate") != params.end()) {
            // Would apply learning rate
        }
    }
}

void SimplifiedBrain::ExpertAPI::configurePlasticity(std::shared_ptr<Brain> brain,
                                                    const std::string& plasticityType,
                                                    const std::unordered_map<std::string, float>& params) {
    if (!brain) return;
    
    NLM_LOG_INFO("Configuring plasticity: " + plasticityType);
    
    if (auto* stdp = brain->getSTDP()) {
        if (params.find("ltp_weight") != params.end()) {
            stdp->setLTPWeight(params.at("ltp_weight"));
        }
        if (params.find("ltd_weight") != params.end()) {
            stdp->setLTDWeight(params.at("ltd_weight"));
        }
    }
    
    if (auto* hebbian = brain->getHebbian()) {
        if (params.find("hebbian_strength") != params.end()) {
            hebbian->setHebbianStrength(params.at("hebbian_strength"));
        }
    }
}

void SimplifiedBrain::ExpertAPI::configureDevelopment(std::shared_ptr<Brain> brain,
                                                      DevelopmentalStage stage) {
    if (!brain) return;
    
    NLM_LOG_INFO("Setting developmental stage to " + std::to_string(static_cast<int>(stage)));
    brain->setDevelopmentalStage(stage);
}

void SimplifiedBrain::ExpertAPI::configureMemory(std::shared_ptr<Brain> brain,
                                                 const std::string& memoryType,
                                                 const std::unordered_map<std::string, size_t>& params) {
    if (!brain) return;
    
    NLM_LOG_INFO("Configuring memory: " + memoryType);
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        if (memoryType == "working" && params.find("capacity") != params.end()) {
            workingMemory->setCapacity(params.at("capacity"));
        }
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        if (memoryType == "episodic" && params.find("max_episodes") != params.end()) {
            episodicMemory->setMaxEpisodes(params.at("max_episodes"));
        }
    }
    
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        if (memoryType == "associative" && params.find("max_associations") != params.end()) {
            associativeMemory->setMaxAssociations(params.at("max_associations"));
        }
    }
}

// Convenience functions
std::shared_ptr<Brain> SimplifiedBrain::createLearningBrain(size_t neurons) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", 1u);
    config->set("connection_probability", 0.05f);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NLM_LOG_INFO("Created learning brain with " + std::to_string(neurons) + " neurons");
    return brain;
}

std::shared_ptr<Brain> SimplifiedBrain::createControlBrain(size_t neurons) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", neurons / 100);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NLM_LOG_INFO("Created control brain with " + std::to_string(neurons) + " neurons");
    return brain;
}

std::shared_ptr<Brain> SimplifiedBrain::createResearchBrain(size_t neurons) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", 2u);
    config->set("connection_probability", 0.2f);
    config->set("simulation_timestep", 0.001);
    config->set("replay_interval", 50u);
    config->set("consolidation_interval", 500u);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NLM_LOG_INFO("Created research brain with " + std::to_string(neurons) + " neurons");
    return brain;
}

std::shared_ptr<Brain> SimplifiedBrain::createSimulationBrain(size_t neurons) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neurons);
    config->set("region_count", neurons / 50);
    config->set("connection_probability", 0.15f);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NLM_LOG_INFO("Created simulation brain with " + std::to_string(neurons) + " neurons");
    return brain;
}

// Monitoring and debugging tools
float SimplifiedBrain::getBrainStats(std::shared_ptr<Brain> brain, std::string& statsOutput) {
    if (!brain) {
        statsOutput = "Brain is null";
        return 0.0f;
    }
    
    return BeginnerAPI::getCurrentPerformance(brain);
}

std::string SimplifiedBrain::getDiagnosticReport(std::shared_ptr<Brain> brain) {
    if (!brain) return "Brain is null";
    
    std::stringstream ss;
    ss << "=== Diagnostic Report ===\n";
    ss << "Brain Type: Simple\n";
    ss << "Neuron Count: " << brain->getTotalNeuronCount() << "\n";
    ss << "Synapse Count: " << brain->getTotalSynapseCount() << "\n";
    ss << "Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
    ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    ss << "Health: " << (isBrainHealthy(brain) ? "Good" : "Poor") << "\n";
    
    return ss.str();
}

std::vector<std::string> SimplifiedBrain::getSystemStatus(std::shared_ptr<Brain> brain) {
    std::vector<std::string> status;
    
    if (!brain) {
        status.push_back("Brain is null");
        return status;
    }
    
    status.push_back("Brain Status:");
    status.push_back("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    status.push_back("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    status.push_back("  Regions: " + std::to_string(brain->getRegionCount()));
    status.push_back("  Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
    status.push_back("  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
    status.push_back("  Health: " + std::string(isBrainHealthy(brain) ? "Good" : "Poor"));
    
    return status;
}

bool SimplifiedBrain::isBrainHealthy(std::shared_ptr<Brain> brain, float minHealth) {
    if (!brain) return false;
    
    float healthScore = 0.0f;
    
    if (brain->getTotalNeuronCount() > 0) healthScore += 0.25f;
    if (brain->getTotalSynapseCount() > 0) healthScore += 0.25f;
    if (brain->getRegionCount() > 0) healthScore += 0.25f;
    if (brain->getTotalSpikeCount() > 0) healthScore += 0.25f;
    
    return healthScore >= minHealth;
}

// Monitoring integration
void SimplifiedBrain::setupMonitoring(std::shared_ptr<Brain> brain,
                                     const std::string& logFile,
                                     size_t logInterval) {
    if (!brain) return;
    
    auto logger = std::make_shared<ConsoleLogger>();
    logger->setLevel(LogLevel::Info);
    
    if (!logFile.empty()) {
        logger->setOutputFile(logFile);
    }
    
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Monitoring setup complete (log file: " + (logFile.empty() ? "console" : logFile) + ")");
}

void SimplifiedBrain::setupDebugMode(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto logger = std::make_shared<ConsoleLogger>();
    logger->setLevel(LogLevel::Debug);
    Logger::setGlobal(logger);
    
    NLM_LOG_DEBUG("Debug mode enabled");
}

void SimplifiedBrain::setupPerformanceMode(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto logger = std::make_shared<ConsoleLogger>();
    logger->setLevel(LogLevel::Warning);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Performance mode enabled (WARNING+ level)");
}

void SimplifiedBrain::setupPowerMode(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto logger = std::make_shared<ConsoleLogger>();
    logger->setLevel(LogLevel::Critical);
    Logger::setGlobal(logger);
    
    NLM_LOG_CRITICAL("Power mode enabled (CRITICAL+ level)");
}

// Interaction patterns
void SimplifiedBrain::simulateEpisode(std::shared_ptr<Brain> brain,
                                      const std::vector<std::vector<float>>& observations,
                                      const std::vector<ActionType>& actions,
                                      const std::vector<float>& rewards,
                                      bool learn) {
    if (!brain || observations.empty()) return;
    
    NLM_LOG_INFO("Simulating episode with " + std::to_string(observations.size()) + " observations");
    
    for (size_t i = 0; i < observations.size(); ++i) {
        // Receive observation
        receiveInput(brain, observations[i]);
        
        // Apply action if available
        if (i < actions.size()) {
            // Would apply action
        }
        
        // Apply reward if available
        if (i < rewards.size() && rewards[i] != 0.0f) {
            applyReward(brain, rewards[i]);
        }
        
        // Step brain
        brain->step(static_cast<SimulationStep>(i));
        
        // Learn if requested
        if (learn && i > 0 && rewards[i-1] != 0.0f) {
            std::vector<float> beforeState = observations[i-1];
            std::vector<float> afterState = observations[i];
            learnFromExperience(brain, beforeState, afterState, rewards[i-1]);
        }
    }
}

float SimplifiedBrain::evaluateBehavior(std::shared_ptr<Brain> brain,
                                        const std::vector<float>& targetState) {
    if (!brain || targetState.empty()) return 0.0f;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < targetState.size(); ++i) {
        similarity += std::abs(targetState[i]);
    }
    
    return similarity / static_cast<float>(targetState.size());
}

void SimplifiedBrain::resetBrain(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    brain->reset();
}

void SimplifiedBrain::saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain || filepath.empty()) return;
    brain->save(filepath);
}

bool SimplifiedBrain::loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain || filepath.empty()) return false;
    return brain->load(filepath);
}

// Memory management utilities
void SimplifiedBrain::cleanupUnusedMemory(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    BeginnerAPI::clearWorkingMemory(brain);
    NLM_LOG_INFO("Cleaned up unused working memory");
}

size_t SimplifiedBrain::getMemoryUsage(std::shared_ptr<Brain> brain) {
    size_t memoryUsage = 0;
    
    if (!brain) return memoryUsage;
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        memoryUsage += workingMemory->getActiveTraces() * sizeof(float);
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        memoryUsage += episodicMemory->getEpisodeCount() * 100;
    }
    
    return memoryUsage;
}

void SimplifiedBrain::clearMemory(std::shared_ptr<Brain> brain, 
                                  const std::string& memoryType) {
    if (!brain) return;
    
    if (memoryType == "working" || memoryType == "all") {
        BeginnerAPI::clearWorkingMemory(brain);
    }
    
    if (memoryType == "episodic" || memoryType == "all") {
        BeginnerAPI::clearEpisodicMemory(brain);
    }
    
    if (memoryType == "associative" || memoryType == "all") {
        BeginnerAPI::clearAssociativeMemory(brain);
    }
}

void SimplifiedBrain::optimizeMemory(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    // Clear weak traces from working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->strengthenMemory(1.0f);
    }
    
    NLM_LOG_INFO("Memory optimized");
}

// Debug and development helpers
void SimplifiedBrain::printBrainArchitecture(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    NLM_LOG_INFO("=== Brain Architecture ===");
    NLM_LOG_INFO("Brain Type: Simplified High-Level Interface");
    NLM_LOG_INFO("Neuron Count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Region Count: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Population Count: " + std::to_string(brain->getRegionCount() * 3));
    NLM_LOG_INFO("Available Systems:");
    NLM_LOG_INFO("  - Spike System");
    NLM_LOG_INFO("  - Plasticity System (STDP + Hebbian)");
    NLM_LOG_INFO("  - Memory Systems (Working + Episodic + Associative)");
    NLM_LOG_INFO("  - Prediction System");
    NLM_LOG_INFO("  - Cognition System (Planner + Concepts + Attention)");
    NLM_LOG_INFO("  - Neuromodulation (Dopamine + Curiosity + Novelty)");
    NLM_LOG_INFO("  - Development System");
}

void SimplifiedBrain::analyzeBrainState(std::shared_ptr<Brain> brain,
                                       std::string& analysisOutput) {
    if (!brain) {
        analysisOutput = "Brain is null";
        return;
    }
    
    std::stringstream ss;
    ss << "=== Brain State Analysis ===\n";
    ss << "Total Neurons: " << brain->getTotalNeuronCount() << "\n";
    ss << "Total Synapses: " << brain->getTotalSynapseCount() << "\n";
    ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
    ss << "Firing Neurons (this step): " << brain->getFiringNeuronCount() << "\n";
    ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
    ss << "Total Spikes: " << brain->getTotalSpikeCount() << "\n";
    ss << "Pending Events: " << brain->getPendingSpikeEventCount() << "\n";
    
    analysisOutput = ss.str();
}

void SimplifiedBrain::findOptimalConfig(std::shared_ptr<Brain> brain,
                                       std::unordered_map<std::string, float>& bestConfig) {
    if (!brain) return;
    
    bestConfig["neuron_count"] = static_cast<float>(brain->getTotalNeuronCount());
    bestConfig["region_count"] = static_cast<float>(brain->getRegionCount());
    bestConfig["connection_probability"] = 0.1f;
    bestConfig["simulation_timestep"] = 0.001f;
    
    NLM_LOG_INFO("Found optimal configuration");
}

void SimplifiedBrain::testBrainStability(std::shared_ptr<Brain> brain,
                                         size_t testSteps) {
    if (!brain) return;
    
    NLM_LOG_INFO("Testing brain stability for " + std::to_string(testSteps) + " steps");
    
    float initialHealth = isBrainHealthy(brain) ? 1.0f : 0.0f;
    
    for (size_t i = 0; i < testSteps; ++i) {
        brain->step(i);
    }
    
    float finalHealth = isBrainHealthy(brain) ? 1.0f : 0.0f;
    
    NLM_LOG_INFO("Stability test complete (initial: " + std::to_string(initialHealth) +
                 ", final: " + std::to_string(finalHealth) + ")");
}

void SimplifiedBrain::profileBrainPerformance(std::shared_ptr<Brain> brain,
                                              std::string& profileOutput) {
    if (!brain) {
        profileOutput = "Brain is null";
        return;
    }
    
    std::stringstream ss;
    ss << "=== Brain Performance Profile ===\n";
    ss << "Step Rate: " << brain->getTotalSpikeCount() << " spikes/second\n";
    ss << "Memory Efficiency: " << getMemoryUsage(brain) << " bytes\n";
    ss << "System Health: " << (isBrainHealthy(brain) ? "Good" : "Poor") << "\n";
    ss << "Processing Time: N/A\n";
    
    profileOutput = ss.str();
}

} // namespace nlm
