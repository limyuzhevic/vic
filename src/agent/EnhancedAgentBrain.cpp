#include "EnhancedAgentBrain.hpp"
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

struct EnhancedAgentBrain::Impl {
    // Core components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentConfig> config_;
    std::shared_ptr<AgentStats> stats_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Enhanced sensory and motor groups with metadata
    struct NeuronGroup {
        std::vector<Neuron*> neurons;
        NeuronType type;
        std::string name;
        float scalingFactor;
        bool enabled;
        size_t lastAccess;
        
        NeuronGroup() : type(NeuronType::Internal), name(""), scalingFactor(1.0f), enabled(true), lastAccess(0) {}
    };
    
    std::vector<NeuronGroup> motorGroups_;
    std::vector<NeuronGroup> sensoryGroups_;
    
    // Advanced neuromodulation state
    struct NeuromodulationState {
        float dopamineLevel_;
        float noveltyLevel_;
        float curiosityLevel_;
        float predictionError_;
        float expectedReward_;
        float learningRate_;
        float explorationRate_;
        
        NeuromodulationState() : dopamineLevel_(0.0f), noveltyLevel_(0.0f), 
                                 curiosityLevel_(0.0f), predictionError_(0.0f),
                                 expectedReward_(0.0f), learningRate_(0.01f),
                                 explorationRate_(0.1f) {}
    } neuromodState_;
    
    // Development and plasticity
    double developmentalAge_;
    float plasticityModifier_;
    double developmentTimeScale_;
    
    // Configuration flags
    std::unordered_map<std::string, bool> featureFlags_;
    
    // Custom actions and goals
    std::string currentGoal_;
    std::string behaviorPolicy_;
    std::unordered_map<std::string, std::function<MotorCommand()>> customActions_;
    
    // Performance tracking
    size_t operationCounter_;
    size_t memoryAccessCounter_;
    double startTime_;
    double lastFrameTime_;
    
    // State validation
    bool initialized_;
    bool brainValid_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    Impl() : brain_(nullptr), config_(std::make_shared<AgentConfig>()), stats_(std::make_shared<AgentStats>()),
             developmentalAge_(0.0), plasticityModifier_(1.0f), developmentTimeScale_(1.0),
             operationCounter_(0), memoryAccessCounter_(0), startTime_(0.0), lastFrameTime_(0.0),
             initialized_(false), brainValid_(false) {
        stats_->startTime_ = startTime_;
        featureFlags_ = {
            {"reward_modulation", true},
            {"structural_plasticity", true},
            {"development", true},
            {"curiosity", true},
            {"novelty_detection", true},
            {"prediction", true}
        };
    }
    
    ~Impl() {
        clearNeuronGroups();
    }
    
    void clearNeuronGroups() {
        motorGroups_.clear();
        sensoryGroups_.clear();
        previousVision_.clear();
    }
    
    bool validateBrain() const {
        return brain_ && brain_->getRegionCount() > 0 && brain_->getTotalNeuronCount() > 0;
    }
    
    void updateStats(double timestep) {
        stats_->lastUpdateTime_ = startTime_ + stats_->totalSimulationTime_;
        stats_->totalSimulationTime_ += timestep;
        
        // Update moving averages
        const float alpha = 0.1f; // Smoothing factor
        stats_->averageCuriosity_ = alpha * neuromodState_.curiosityLevel_ + 
                                   (1.0f - alpha) * stats_->averageCuriosity_;
        stats_->averageNovelty_ = alpha * neuromodState_.noveltyLevel_ + 
                                 (1.0f - alpha) * stats_->averageNovelty_;
        stats_->averageDopamine_ = alpha * neuromodState_.dopamineLevel_ + 
                                  (1.0f - alpha) * stats_->averageDopamine_;
        
        stats_->neuralOperations_++;
    }
    
    void categorizeNeurons() {
        motorGroups_.clear();
        sensoryGroups_.clear();
        
        if (!brain_ || !validateBrain()) return;
        
        // Initialize groups based on config
        const auto& cfg = *config_;
        
        // Motor groups
        motorGroups_.resize(cfg.motorGroups_);
        for (size_t i = 0; i < cfg.motorGroups_; ++i) {
            motorGroups_[i].name = getMotorGroupName(i);
            motorGroups_[i].type = NeuronType::Motor;
            motorGroups_[i].scalingFactor = 1.0f;
            motorGroups_[i].enabled = true;
        }
        
        // Sensory groups
        sensoryGroups_.resize(4); // Vision, Touch, Internal, Proprioception
        sensoryGroups_[0] = { {}, NeuronType::Sensory, "Vision", 1.0f, true };
        sensoryGroups_[1] = { {}, NeuronType::Sensory, "Touch", 1.0f, true };
        sensoryGroups_[2] = { {}, NeuronType::Sensory, "Internal", 1.0f, true };
        sensoryGroups_[3] = { {}, NeuronType::Sensory, "Proprioception", 1.0f, true };
        
        // Populate from brain regions
        populateNeuronGroupsFromBrain();
    }
    
    void populateNeuronGroupsFromBrain() {
        if (!brain_) return;
        
        clearNeuronGroups();
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                for (Neuron* n : pop->getNeurons()) {
                    if (type == NeuronType::Motor) {
                        addToMotorGroup(n);
                    } else if (type == NeuronType::Sensory) {
                        addToSensoryGroup(n);
                    }
                }
            }
        }
    }
    
    void addToMotorGroup(Neuron* neuron) {
        if (!neuron) return;
        
        // Find appropriate motor group
        for (auto& group : motorGroups_) {
            if (!group.enabled || group.neurons.empty()) {
                group.neurons.push_back(neuron);
                return;
            }
        }
        
        // Add to a new group if limit reached
        if (motorGroups_.size() < config_->maxNeuronGroups_) {
            motorGroups_.emplace_back();
            motorGroups_.back().neurons.push_back(neuron);
            motorGroups_.back().type = NeuronType::Motor;
            motorGroups_.back().name = "Motor_" + std::to_string(motorGroups_.size() - 1);
            motorGroups_.back().scalingFactor = 1.0f;
            motorGroups_.back().enabled = true;
        }
    }
    
    void addToSensoryGroup(Neuron* neuron) {
        if (!neuron) return;
        
        // Categorize based on neuron properties
        for (auto& group : sensoryGroups_) {
            if (group.neurons.empty()) {
                group.neurons.push_back(neuron);
                return;
            }
        }
        
        // Default to vision group if all others full
        if (!sensoryGroups_[0].neurons.empty()) {
            for (auto& group : sensoryGroups_) {
                if (group.neurons.size() < config_->maxNeuronGroups_ / 4) {
                    group.neurons.push_back(neuron);
                    return;
                }
            }
        }
    }
    
    std::string getMotorGroupName(size_t index) const {
        const char* names[] = {"Forward", "Backward", "Left", "Right", "Interact", "Wait"};
        if (index < sizeof(names) / sizeof(names[0])) {
            return "Motor_" + std::string(names[index]);
        }
        return "Motor_" + std::to_string(index);
    }
    
    void processSensoryInputAdvanced(const SensoryPercept& percept) {
        if (!brain_ || !validateBrain()) return;
        
        operationCounter_++;
        
        // Process each sensory group
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& internal = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        // Vision processing
        if (sensoryGroups_[0].enabled) {
            for (size_t i = 0; i < sensoryGroups_[0].neurons.size() && i < vision.size(); ++i) {
                if (sensoryGroups_[0].neurons[i]) {
                    float current = vision[i] * sensoryGroups_[0].scalingFactor;
                    sensoryGroups_[0].neurons[i]->injectCurrent(current);
                }
            }
        }
        
        // Touch processing
        if (sensoryGroups_[1].enabled) {
            for (size_t i = 0; i < sensoryGroups_[1].neurons.size() && i < touch.size(); ++i) {
                if (sensoryGroups_[1].neurons[i]) {
                    float current = touch[i] * sensoryGroups_[1].scalingFactor;
                    sensoryGroups_[1].neurons[i]->injectCurrent(current);
                }
            }
        }
        
        // Internal signals
        if (sensoryGroups_[2].enabled) {
            for (size_t i = 0; i < sensoryGroups_[2].neurons.size() && i < internal.size(); ++i) {
                if (sensoryGroups_[2].neurons[i]) {
                    float current = (internal[i] * 2.0f - 1.0f) * sensoryGroups_[2].scalingFactor;
                    sensoryGroups_[2].neurons[i]->injectCurrent(current);
                }
            }
        }
        
        // Proprioception
        if (sensoryGroups_[3].enabled) {
            for (size_t i = 0; i < sensoryGroups_[3].neurons.size() && i < proprio.size(); ++i) {
                if (sensoryGroups_[3].neurons[i]) {
                    float current = (proprio[i] * 2.0f - 1.0f) * sensoryGroups_[3].scalingFactor;
                    sensoryGroups_[3].neurons[i]->injectCurrent(current);
                }
            }
        }
        
        // Novelty detection
        computeNoveltyAdvanced(vision);
        
        // Update stats
        stats_->totalSensoryInputs_++;
    }
    
    void computeNoveltyAdvanced(const std::vector<float>& vision) {
        if (vision.empty() || !featureFlags_["novelty_detection"]) return;
        
        if (previousVision_.empty()) {
            previousVision_ = vision;
            neuromodState_.noveltyLevel_ = 0.0f;
            return;
        }
        
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        neuromodState_.noveltyLevel_ = totalDiff / std::max<size_t>(minSize, 1);
        neuromodState_.noveltyLevel_ *= config_->noveltyDecay_;
        
        // Update curiosity based on novelty and prediction error
        if (featureFlags_["curiosity"]) {
            neuromodState_.curiosityLevel_ = neuromodState_.noveltyLevel_ * 2.0f + 
                                            std::abs(neuromodState_.predictionError_) * 0.5f;
            neuromodState_.curiosityLevel_ = std::clamp(neuromState_.curiosityLevel_, 0.0f, 1.0f);
        }
        
        previousVision_ = vision;
    }
    
    MotorCommand decodeMotorCommandAdvanced() {
        if (!brain_ || !validateBrain()) return MotorCommand::Wait;
        
        operationCounter_++;
        
        // Calculate activity for each motor group
        std::vector<float> groupActivities;
        
        for (const auto& group : motorGroups_) {
            if (!group.enabled || group.neurons.empty()) {
                groupActivities.push_back(0.0f);
                continue;
            }
            
            float activity = calculateGroupActivity(group);
            groupActivities.push_back(activity);
        }
        
        // Find best action with curiosity-based exploration
        MotorCommand bestCommand = selectBestMotorCommand(groupActivities);
        
        // Apply curiosity/exploration if enabled
        if (featureFlags_["curiosity"] && neuromodState_.curiosityLevel_ > config_->curiosityThreshold_) {
            bestCommand = exploreWithCuriosity(bestCommand);
        }
        
        stats_->totalMotorOutputs_++;
        return bestCommand;
    }
    
    float calculateGroupActivity(const NeuronGroup& group) const {
        if (group.neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : group.neurons) {
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        
        return sum / group.neurons.size();
    }
    
    MotorCommand selectBestMotorCommand(const std::vector<float>& activities) const {
        if (activities.empty()) return MotorCommand::Wait;
        
        // Map activities to motor commands
        const MotorCommand commands[] = {
            MotorCommand::MoveForward, MotorCommand::MoveBackward,
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::Interact, MotorCommand::Wait
        };
        
        size_t numCommands = sizeof(commands) / sizeof(commands[0]);
        if (activities.size() < numCommands) {
            numCommands = activities.size();
        }
        
        float bestActivity = 0.0f;
        MotorCommand bestCommand = MotorCommand::Wait;
        
        for (size_t i = 0; i < numCommands; ++i) {
            if (activities[i] > bestActivity) {
                bestActivity = activities[i];
                bestCommand = commands[i];
            }
        }
        
        // Only act if there's meaningful activity
        if (bestActivity < 0.5f) {
            return MotorCommand::Wait;
        }
        
        return bestCommand;
    }
    
    MotorCommand exploreWithCuriosity(MotorCommand defaultCmd) {
        if (neuromodState_.curiosityLevel_ > config_->explorationThreshold_ && 
            brain_ && brain_->getRandomGenerator()) {
            
            float exploreChance = neuromodState_.curiosityLevel_ * 0.3f; // Up to 30% random
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            
            if (r < exploreChance) {
                // Random motor command - valid range is 0-6 (7 values)
                int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
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
        }
        
        return defaultCmd;
    }
    
    void updateDevelopmentAdvanced(double timestep) {
        if (!brain_ || !validateBrain()) return;
        
        developmentalAge_ += timestep * config_->developmentalTimeScale_;
        
        // Enhanced developmental stages with more granularity
        if (developmentalAge_ < 30.0) {  // ~0.5 minutes
            neuromodState_.learningRate_ = 0.05f;
            neuromodState_.explorationRate_ = 0.3f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < 150.0) {  // ~2.5 minutes
            neuromodState_.learningRate_ = 0.03f;
            neuromodState_.explorationRate_ = 0.2f;
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < 450.0) {  // ~7.5 minutes
            neuromodState_.learningRate_ = 0.02f;
            neuromodState_.explorationRate_ = 0.1f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {
            neuromodState_.learningRate_ = 0.01f;
            neuromodState_.explorationRate_ = 0.05f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
        
        // Update plasticity modifier based on development
        if (developmentalAge_ < 60.0) {
            plasticityModifier_ = 1.2f; // Slightly higher initial plasticity
        } else if (developmentalAge_ < 300.0) {
            plasticityModifier_ = 0.9f;
        } else if (developmentalAge_ < 900.0) {
            plasticityModifier_ = 0.6f;
        } else {
            plasticityModifier_ = 0.3f; // More stable adult brain
        }
        
        // Apply neuromodulation to brain systems
        if (featureFlags_["reward_modulation"]) {
            applyNeuromodulationToBrain();
        }
    }
    
    void applyNeuromodulationToBrain() {
        if (!brain_ || !validateBrain()) return;
        
        // Update STDP parameters based on neuromodulation
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            float plasticityFactor = neuromodState_.learningRate_ * plasticityModifier_;
            plasticityFactor = std::clamp(plasticityFactor, 0.01f, 0.05f);
            
            stdp->setLTPWeight(plasticityFactor * 0.8f);
            stdp->setLTDWeight(plasticityFactor * 0.85f);
        }
        
        // Apply dopamine to synapses
        if (neuromodState_.dopamineLevel_ != 0.0f) {
            for (const auto& region : brain_->getRegions()) {
                for (auto* syn : region->getSynapses()) {
                    float eligibility = syn->getEligibilityTrace();
                    if (std::abs(eligibility) > 0.001f) {
                        float delta = eligibility * neuromodState_.dopamineLevel_ * plasticityModifier_;
                        syn->addToWeight(delta);
                        syn->decayEligibilityTrace(0.1f);
                    }
                }
            }
        }
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    bool isFeatureEnabled(const std::string& feature) const {
        auto it = featureFlags_.find(feature);
        return it != featureFlags_.end() && it->second;
    }
    
    void enableFeature(const std::string& feature, bool enable) {
        featureFlags_[feature] = enable;
        
        // Log feature change
        if (enable) {
            NLM_LOG_INFO("Enabled feature: " + feature);
        } else {
            NLM_LOG_INFO("Disabled feature: " + feature);
        }
    }
    
    std::string getDebugInfo() const {
        std::stringstream ss;
        ss << "=== Enhanced AgentBrain Debug Info ===\n";
        ss << "Brain Valid: " << (brainValid_ ? "Yes" : "No") << "\n";
        ss << "Initialized: " << (initialized_ ? "Yes" : "No") << "\n";
        ss << "Developmental Age: " << developmentalAge_ << "s\n";
        ss << "Plasticity Modifier: " << plasticityModifier_ << "\n";
        ss << "Motor Groups: " << motorGroups_.size() << "\n";
        ss << "Sensory Groups: " << sensoryGroups_.size() << "\n";
        ss << "Features Enabled:\n";
        for (const auto& pair : featureFlags_) {
            ss << "  " << pair.first << ": " << (pair.second ? "Yes" : "No") << "\n";
        }
        ss << "Neuromodulation:\n";
        ss << "  Dopamine: " << neuromodState_.dopamineLevel_ << "\n";
        ss << "  Novelty: " << neuromodState_.noveltyLevel_ << "\n";
        ss << "  Curiosity: " << neuromodState_.curiosityLevel_ << "\n";
        ss << "  Prediction Error: " << neuromodState_.predictionError_ << "\n";
        ss << "Statistics:\n";
        ss << "  Total Sensory Inputs: " << stats_->totalSensoryInputs_ << "\n";
        ss << "  Total Motor Outputs: " << stats_->totalMotorOutputs_ << "\n";
        ss << "  Neural Operations: " << stats_->neuralOperations_ << "\n";
        ss << "  Uptime: " << getUptime() << "s\n";
        ss << "=========================================\n";
        
        return ss.str();
    }
    
    double getUptime() const {
        return startTime_ > 0.0 ? (stats_->lastUpdateTime_ > 0.0 ? stats_->lastUpdateTime_ : 0.0) : 0.0;
    }
    
    bool isValid() const {
        return initialized_ && brainValid_ && validateBrain();
    }
    
    void addCustomAction(const std::string& name, std::function<MotorCommand()> actionFunc) {
        customActions_[name] = actionFunc;
        NLM_LOG_INFO("Added custom action: " + name);
    }
}; 

EnhancedAgentBrain::EnhancedAgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<AgentConfig> config)
    : pImpl_(new Impl()) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    pImpl_->brain_ = brain;
    pImpl_->config_ = config;
    pImpl_->startTime_ = 0.0; // Will be set when initialized
    pImpl_->initialized_ = false;
    pImpl_->brainValid_ = false;
    
    pImpl_->brainValid_ = pImpl_->validateBrain();
    pImpl_->categorizeNeurons();
    
    NLM_LOG_INFO("EnhancedAgentBrain created with " + std::to_string(pImpl_->brain_->getTotalNeuronCount()) + " neurons");
}

EnhancedAgentBrain::~EnhancedAgentBrain() {
    delete pImpl_;
}

EnhancedAgentBrain::EnhancedAgentBrain(EnhancedAgentBrain&& other) noexcept : pImpl_(other.pImpl_) {
    other.pImpl_ = nullptr;
}

EnhancedAgentBrain& EnhancedAgentBrain::operator=(EnhancedAgentBrain&& other) noexcept {
    if (this != &other) {
        delete pImpl_;
        pImpl_ = other.pImpl_;
        other.pImpl_ = nullptr;
    }
    return *this;
}

void EnhancedAgentBrain::initialize(const SimpleWorld& world) {
    if (!pImpl_) return;
    
    pImpl_->world_ = std::make_shared<SimpleWorld>(world);
    pImpl_->previousVision_.resize(pImpl_->world_->getVisionWidth() * pImpl_->world_->getVisionHeight(), 0.0f);
    pImpl_->developmentalAge_ = 0.0;
    pImpl_->plasticityModifier_ = 1.0f;
    pImpl_->startTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    pImpl_->stats_->startTime_ = pImpl_->startTime_;
    pImpl_->initialized_ = true;
    
    NLM_LOG_INFO("EnhancedAgentBrain initialized with world dimensions: " + 
                 std::to_string(pImpl_->world_->getWidth()) + "x" + 
                 std::to_string(pImpl_->world_->getHeight()));
}

void EnhancedAgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!pImpl_ || !pImpl_->isValid()) return;
    
    pImpl_->processSensoryInputAdvanced(percept);
    pImpl_->updateStats(0.01); // Default timestep
}

MotorCommand EnhancedAgentBrain::decodeMotorCommand() {
    if (!pImpl_ || !pImpl_->isValid()) return MotorCommand::Wait;
    
    return pImpl_->decodeMotorCommandAdvanced();
}

void EnhancedAgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!pImpl_ || !pImpl_->isValid()) return;
    
    if (pImpl_->isFeatureEnabled("reward_modulation")) {
        pImpl_->neuromodState_.predictionError_ = reward - predictedReward;
        pImpl_->neuromodState_.expectedReward_ = 0.95f * pImpl_->neuromodState_.expectedReward_ + 0.05f * reward;
        pImpl_->neuromodState_.dopamineLevel_ = pImpl_->neuromodState_.predictionError_;
        pImpl_->neuromodState_.dopamineLevel_ = std::clamp(pImpl_->neuromodState_.dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to synapses
        for (const auto& region : pImpl_->brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                if (std::abs(eligibility) > 0.001f) {
                    float delta = eligibility * pImpl_->neuromodState_.dopamineLevel_ * pImpl_->plasticityModifier_;
                    syn->addToWeight(delta);
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
    }
}

void EnhancedAgentBrain::updateDevelopment(double timestep) {
    if (!pImpl_ || !pImpl_->isValid()) return;
    
    pImpl_->updateDevelopmentAdvanced(timestep);
}

size_t EnhancedAgentBrain::getSensoryInputSize() const {
    if (!pImpl_) return 0;
    
    size_t size = 0;
    const auto& cfg = *pImpl_->config_;
    size += cfg.visionWidth_ * cfg.visionHeight_;
    size += cfg.touchSensors_;
    size += cfg.internalSensors_;
    size += cfg.proprioceptionSensors_;
    
    return size;
}

size_t EnhancedAgentBrain::getMotorOutputSize() const {
    if (!pImpl_) return 0;
    return pImpl_->motorGroups_.size();
}

DevelopmentalStage EnhancedAgentBrain::getDevelopmentalStage() const {
    if (!pImpl_) return DevelopmentalStage::Initial;
    return pImpl_->getDevelopmentalStage();
}

void EnhancedAgentBrain::reset() {
    if (!pImpl_) return;
    
    pImpl_->neuromodState_ = NeuromodulationState();
    pImpl_->developmentalAge_ = 0.0;
    pImpl_->plasticityModifier_ = 1.0f;
    pImpl_->previousVision_.clear();
    pImpl_->customActions_.clear();
    pImpl_->startTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Reset stats
    pImpl_->stats_->totalSensoryInputs_ = 0;
    pImpl_->stats_->totalMotorOutputs_ = 0;
    pImpl_->stats_->neuralOperations_ = 0;
    pImpl_->stats_->memoryAllocations_ = 0;
    
    NLM_LOG_INFO("EnhancedAgentBrain reset complete");
}

void EnhancedAgentBrain::enableFeature(const std::string& feature, bool enable) {
    if (!pImpl_) return;
    pImpl_->enableFeature(feature, enable);
}

bool EnhancedAgentBrain::isFeatureEnabled(const std::string& feature) const {
    if (!pImpl_) return false;
    return pImpl_->isFeatureEnabled(feature);
}

std::string EnhancedAgentBrain::getDebugInfo() const {
    if (!pImpl_) return "EnhancedAgentBrain not initialized";
    return pImpl_->getDebugInfo();
}

double EnhancedAgentBrain::getUptime() const {
    if (!pImpl_) return 0.0;
    return pImpl_->getUptime();
}

bool EnhancedAgentBrain::isValid() const {
    return pImpl_ && pImpl_->isValid();
}

void EnhancedAgentBrain::addCustomAction(const std::string& name, std::function<MotorCommand()> actionFunc) {
    if (!pImpl_) return;
    pImpl_->addCustomAction(name, actionFunc);
}

} // namespace nlm