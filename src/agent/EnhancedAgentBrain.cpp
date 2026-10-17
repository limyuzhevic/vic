#include "EnhancedAgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

namespace nlm {

struct AgentBrain::Impl {
    std::shared_ptr<Brain> brain_;
    AgentConfig config_;
    AgentStats stats_;
    SimpleWorld* world_ = nullptr;
    
    // Neuron group management
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    
    // Development state
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
    DevelopmentalStage developmentalStage_ = DevelopmentalStage::Initial;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_ = 0.99f;
    
    // Configuration flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Episode tracking
    int currentEpisode_ = 0;
    float episodeStartTime_ = 0.0f;
    float episodeTotalReward_ = 0.0f;
    
    // Performance monitoring
    double lastUpdateTime_ = 0.0;
    size_t updatesSinceLastStat_ = 0;
    
    // State management
    enum class State {
        Initializing,
        Running,
        Paused,
        Saving,
        Loading,
        ShuttingDown
    };
    
    State state_ = State::Initializing;
    
    // Feature flags
    std::unordered_map<std::string, bool> featureFlags_;
    
    // Performance optimization
    bool performanceInitialized_ = false;
    std::mutex stateMutex_;
    
    Impl() = default;
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
    
    // Initialize feature flags
    pImpl->featureFlags_["reward_modulation"] = config.enableRewardModulation;
    pImpl->featureFlags_["structural_plasticity"] = config.enableStructuralPlasticity;
    pImpl->featureFlags_["development"] = config.enableDevelopment;
    pImpl->featureFlags_["curiosity"] = config.enableCuriosityExploration;
    pImpl->featureFlags_["novelty"] = config.enableNoveltyDetection;
    pImpl->featureFlags_["prediction"] = config.enablePredictionError;
    
    // Initialize neuron groups
    initializeNeuronGroups();
    
    NLM_LOG_INFO("Enhanced AgentBrain created with standard configuration");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    if (!pImpl->brain_) return;
    
    // Clear existing groups
    pImpl->motorForward_.clear();
    pImpl->motorBackward_.clear();
    pImpl->motorTurnLeft_.clear();
    pImpl->motorTurnRight_.clear();
    pImpl->motorInteract_.clear();
    pImpl->motorWait_.clear();
    
    pImpl->sensoryVision_.clear();
    pImpl->sensoryTouch_.clear();
    pImpl->sensoryInternal_.clear();
    pImpl->sensoryProprioception_.clear();
    
    // Distribute neurons to appropriate groups
    for (const auto& region : pImpl->brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            NeuronType type = population->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : population->getNeurons()) {
                    // Distribute motor neurons
                    size_t totalMotor = pImpl->motorForward_.size() + pImpl->motorBackward_.size() +
                                      pImpl->motorTurnLeft_.size() + pImpl->motorTurnRight_.size() +
                                      pImpl->motorInteract_.size() + pImpl->motorWait_.size();
                    
                    switch (totalMotor % 6) {
                        case 0: pImpl->motorForward_.push_back(n); break;
                        case 1: pImpl->motorBackward_.push_back(n); break;
                        case 2: pImpl->motorTurnLeft_.push_back(n); break;
                        case 3: pImpl->motorTurnRight_.push_back(n); break;
                        case 4: pImpl->motorInteract_.push_back(n); break;
                        case 5: pImpl->motorWait_.push_back(n); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : population->getNeurons()) {
                    // Distribute sensory neurons
                    size_t totalSensory = pImpl->sensoryVision_.size() + pImpl->sensoryTouch_.size() +
                                        pImpl->sensoryInternal_.size() + pImpl->sensoryProprioception_.size();
                    
                    switch (totalSensory % 4) {
                        case 0: pImpl->sensoryVision_.push_back(n); break;
                        case 1: pImpl->sensoryTouch_.push_back(n); break;
                        case 2: pImpl->sensoryInternal_.push_back(n); break;
                        case 3: pImpl->sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Neuron groups initialized: " +
                 std::to_string(pImpl->motorForward_.size()) + " motor forward, " +
                 std::to_string(pImpl->sensoryVision_.size()) + " sensory vision");
}

void AgentBrain::initialize(const SimpleWorld& world) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    pImpl->world_ = const_cast<SimpleWorld*>(&world);
    pImpl->previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    pImpl->developmentalAge_ = 0.0;
    pImpl->plasticityModifier_ = 1.0f;
    pImpl->developmentalStage_ = DevelopmentalStage::Initial;
    
    // Reset configuration flags
    pImpl->rewardModulationEnabled_ = pImpl->featureFlags_["reward_modulation"];
    pImpl->structuralPlasticityEnabled_ = pImpl->featureFlags_["structural_plasticity"];
    pImpl->developmentEnabled_ = pImpl->featureFlags_["development"];
    pImpl->curiosityEnabled_ = pImpl->featureFlags_["curiosity"];
    
    pImpl->state_ = Impl::State::Running;
    
    NLM_LOG_INFO("Enhanced AgentBrain initialized successfully");
}

void AgentBrain::update(double timestep) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    if (pImpl->state_ != Impl::State::Running) {
        return;
    }
    
    pImpl->stats_.totalSteps++;
    pImpl->updatesSinceLastStat_++;
    pImpl->developmentalAge_ += timestep;
    
    pImpl->lastUpdateTime_ = timestep;
    
    // Update brain state
    if (pImpl->brain_) {
        pImpl->brain_->step(pImpl->stats_.totalSteps, pImpl->stats_.totalSteps * 0.001);
    }
    
    // Update stats periodically
    if (pImpl->updatesSinceLastStat_ >= 100) {
        updateStats();
        pImpl->updatesSinceLastStat_ = 0;
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    if (pImpl->state_ != Impl::State::Running || !pImpl->brain_) {
        return;
    }
    
    // Vision input processing
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < pImpl->sensoryVision_.size() && i < vision.size(); ++i) {
        if (pImpl->sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            pImpl->sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < pImpl->sensoryTouch_.size() && i < touch.size(); ++i) {
        if (pImpl->sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            pImpl->sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < pImpl->sensoryInternal_.size() && i < intern.size(); ++i) {
        if (pImpl->sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            pImpl->sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < pImpl->sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (pImpl->sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            pImpl->sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Novelty detection
    if (pImpl->featureFlags_["novelty"] && !vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < pImpl->previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - pImpl->previousVision_[i]);
            totalDiff += diff;
        }
        
        pImpl->noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        pImpl->noveltyLevel_ *= pImpl->sensoryNoveltyDecay_;
        
        pImpl->previousVision_ = vision;
    }
    
    // Curiosity update
    if (pImpl->curiosityEnabled_) {
        pImpl->curiosityLevel_ = pImpl->noveltyLevel_ * 2.0f + std::abs(pImpl->predictionError_) * 0.5f;
        pImpl->curiosityLevel_ = std::clamp(pImpl->curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update development
    if (pImpl->developmentEnabled_) {
        updateDevelopment(timestep);
    }
}

MotorCommand AgentBrain::getMotorCommand() {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    if (!pImpl->brain_) {
        return MotorCommand::Wait;
    }
    
    MotorCommand command = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (pImpl->curiosityEnabled_ && pImpl->curiosityLevel_ > pImpl->config_.curiosityThreshold) {
        command = selectActionWithCuriosity(command);
    }
    
    // Apply novelty-based exploration
    if (pImpl->featureFlags_["novelty"] && pImpl->noveltyLevel_ > 0.5f) {
        command = selectActionWithNovelty(command);
    }
    
    return command;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    if (!pImpl->brain_ || !pImpl->rewardModulationEnabled_) {
        return;
    }
    
    // Compute prediction error
    pImpl->predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward_ = 0.95f * pImpl->expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    pImpl->dopamineLevel_ = pImpl->predictionError_;
    pImpl->dopamineLevel_ = std::clamp(pImpl->dopamineLevel_, -1.0f, 1.0f);
    
    // Update brain statistics
    pImpl->stats_.totalReward += reward;
    
    // Apply neuromodulation to brain
    // (In a real implementation, this would be done through dedicated neuromodulation systems)
    
    // Update stats
    pImpl->stats_.dopamineLevel = pImpl->dopamineLevel_;
    pImpl->stats_.curiosityLevel = pImpl->curiosityLevel_;
    pImpl->stats_.noveltyLevel = pImpl->noveltyLevel_;
    pImpl->stats_.predictionError = pImpl->predictionError_;
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!pImpl->brain_) return;
    
    pImpl->developmentalAge_ += timestep;
    
    // Update developmental stages based on age
    if (pImpl->developmentalAge_ < 60.0) {
        pImpl->plasticityModifier_ = 1.0f;
        pImpl->developmentalStage_ = DevelopmentalStage::Initial;
        pImpl->stats_.developmentalStage = DevelopmentalStage::Initial;
    } else if (pImpl->developmentalAge_ < 300.0) {
        pImpl->plasticityModifier_ = 0.8f;
        pImpl->developmentalStage_ = DevelopmentalStage::CriticalPeriod;
        pImpl->stats_.developmentalStage = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->developmentalAge_ < 900.0) {
        pImpl->plasticityModifier_ = 0.5f;
        pImpl->developmentalStage_ = DevelopmentalStage::Maturation;
        pImpl->stats_.developmentalStage = DevelopmentalStage::Maturation;
    } else {
        pImpl->plasticityModifier_ = 0.2f;
        pImpl->developmentalStage_ = DevelopmentalStage::Adult;
        pImpl->stats_.developmentalStage = DevelopmentalStage::Adult;
    }
    
    // Structural plasticity changes
    if (pImpl->structuralPlasticityEnabled_) {
        auto* sp = pImpl->brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * pImpl->plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - pImpl->plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::configure(const AgentConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    pImpl->config_ = config;
    
    // Update feature flags
    pImpl->featureFlags_["reward_modulation"] = config.enableRewardModulation;
    pImpl->featureFlags_["structural_plasticity"] = config.enableStructuralPlasticity;
    pImpl->featureFlags_["development"] = config.enableDevelopment;
    pImpl->featureFlags_["curiosity"] = config.enableCuriosityExploration;
    pImpl->featureFlags_["novelty"] = config.enableNoveltyDetection;
    pImpl->featureFlags_["prediction"] = config.enablePredictionError;
    
    // Update configuration flags
    pImpl->rewardModulationEnabled_ = config.enableRewardModulation;
    pImpl->structuralPlasticityEnabled_ = config.enableStructuralPlasticity;
    pImpl->developmentEnabled_ = config.enableDevelopment;
    pImpl->curiosityEnabled_ = config.enableCuriosityExploration;
    
    NLM_LOG_INFO("AgentBrain configuration updated");
}

MotorCommand AgentBrain::selectActionWithCuriosity(MotorCommand defaultCmd) {
    if (!pImpl->brain_ || !pImpl->curiosityEnabled_) {
        return defaultCmd;
    }
    
    float exploreChance = pImpl->curiosityLevel_ * pImpl->config_.explorationChance;
    
    float r = pImpl->brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        int choice = pImpl->brain_->getRandomGenerator()->uniformInt(0, 7);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(pImpl->motorForward_);
    float backwardAct = calcActivity(pImpl->motorBackward_);
    float leftAct = calcActivity(pImpl->motorTurnLeft_);
    float rightAct = calcActivity(pImpl->motorTurnRight_);
    float interactAct = calcActivity(pImpl->motorInteract_);
    float waitAct = calcActivity(pImpl->motorWait_);
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

void AgentBrain::startEpisode(int episode) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    pImpl->currentEpisode_ = episode;
    pImpl->episodeStartTime_ = 0.0f;
    pImpl->episodeTotalReward_ = 0.0f;
    
    NLM_LOG_INFO("Starting episode " + std::to_string(episode));
}

void AgentBrain::endEpisode(float totalReward) {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    NLM_LOG_INFO("Ending episode " + std::to_string(pImpl->currentEpisode_) + 
                 " with total reward: " + std::to_string(totalReward));
    
    pImpl->stats_.memoryEpisodesStored++;
}

void AgentBrain::updateStats() {
    if (!pImpl->brain_) return;
    
    // Update firing rate stats
    size_t activeNeurons = pImpl->brain_->getFiringNeuronCount();
    float avgFiringRate = pImpl->brain_->getAverageFiringRate();
    
    pImpl->stats_.avgFiringRate = avgFiringRate;
    pImpl->stats_.workingMemoryTraces = pImpl->brain_->getWorkingMemory() ? 
                                        pImpl->brain_->getWorkingMemory()->getActiveTraces() : 0;
    pImpl->stats_.memoryEpisodesStored = pImpl->brain_->getEpisodicMemory() ? 
                                          pImpl->brain_->getEpisodicMemory()->getEpisodeCount() : 0;
}

void AgentBrain::enableFeature(const std::string& feature, bool enable) {
    pImpl->featureFlags_[feature] = enable;
    
    // Update corresponding configuration flags
    if (feature == "reward_modulation") {
        pImpl->rewardModulationEnabled_ = enable;
    } else if (feature == "structural_plasticity") {
        pImpl->structuralPlasticityEnabled_ = enable;
    } else if (feature == "development") {
        pImpl->developmentEnabled_ = enable;
    } else if (feature == "curiosity") {
        pImpl->curiosityEnabled_ = enable;
    }
    
    NLM_LOG_INFO("Feature '" + feature + "': " + (enable ? "enabled" : "disabled"));
}

bool AgentBrain::isFeatureEnabled(const std::string& feature) const {
    std::lock_guard<std::mutex> lock(pImpl->stateMutex_);
    
    auto it = pImpl->featureFlags_.find(feature);
    return it != pImpl->featureFlags_.end() ? it->second : false;
}

// Factory implementations
std::unique_ptr<AgentBrain> AgentBrainFactory::createStandardAgent(std::shared_ptr<Brain> brain) {
    AgentConfig config;
    enableAllFeatures(config);
    return std::make_unique<AgentBrain>(std::move(brain), config);
}

std::unique_ptr<AgentBrain> AgentBrainFactory::createMinimalAgent(std::shared_ptr<Brain> brain) {
    AgentConfig config;
    enableCoreFeatures(config);
    return std::make_unique<AgentBrain>(std::move(brain), config);
}

std::unique_ptr<AgentBrain> AgentBrainFactory::createAdvancedAgent(std::shared_ptr<Brain> brain) {
    AgentConfig config;
    enableAllFeatures(config);
    config.enableVectorization = true;
    config.enableParallelProcessing = true;
    config.maxThreads = 8;
    return std::make_unique<AgentBrain>(std::move(brain), config);
}

std::unique_ptr<AgentBrain> AgentBrainFactory::createFromConfig(std::shared_ptr<Brain> brain, const AgentConfig& config) {
    return std::make_unique<AgentBrain>(std::move(brain), config);
}

void AgentBrainFactory::enableAllFeatures(AgentConfig& config) {
    config.enableRewardModulation = true;
    config.enableStructuralPlasticity = true;
    config.enableDevelopment = true;
    config.enableCuriosityExploration = true;
    config.enableNoveltyDetection = true;
    config.enablePredictionError = true;
    config.enableWorkingMemory = true;
    config.enableEpisodicMemory = true;
    config.enableVectorization = true;
}

void AgentBrainFactory::enableCoreFeatures(AgentConfig& config) {
    config.enableRewardModulation = true;
    config.enableStructuralPlasticity = false;
    config.enableDevelopment = false;
    config.enableCuriosityExploration = false;
    config.enableNoveltyDetection = false;
    config.enablePredictionError = false;
    config.enableWorkingMemory = true;
    config.enableEpisodicMemory = false;
}

void AgentBrainFactory::enableBasicFeatures(AgentConfig& config) {
    config.enableRewardModulation = false;
    config.enableStructuralPlasticity = false;
    config.enableDevelopment = false;
    config.enableCuriosityExploration = false;
    config.enableNoveltyDetection = false;
    config.enablePredictionError = false;
    config.enableWorkingMemory = false;
    config.enableEpisodicMemory = false;
}

void AgentBrainFactory::disableAllFeatures(AgentConfig& config) {
    config.enableRewardModulation = false;
    config.enableStructuralPlasticity = false;
    config.enableDevelopment = false;
    config.enableCuriosityExploration = false;
    config.enableNoveltyDetection = false;
    config.enablePredictionError = false;
    config.enableWorkingMemory = false;
    config.enableEpisodicMemory = false;
    config.enableVectorization = false;
    config.enableParallelProcessing = false;
}

} // namespace nlm
