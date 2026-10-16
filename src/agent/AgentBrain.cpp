#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, nullptr)
{
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const std::shared_ptr<Config>& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , advancedDevelopmentalStage_(AdvancedDevelopmentalStage::Embryonic)
    , stageStartTime_(0.0)
    , stageDuration_(60.0)
    , availableResources_(1.0f)
    , resourceRegenerationRate_(0.01f)
    , currentStressLevel_(0.0f)
    , stressRecoveryRate_(0.05f)
    , lastStressEventTime_(0.0)
    , consecutiveErrors_(0)
    , lastErrorTime_(0.0)
    , inErrorRecoveryMode_(false)
{
    // Initialize advanced systems
    initializeAdvancedSystems();
    
    // Configure from provided config if available
    if (config_) {
        configureFromConfig(config_);
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeAdvancedSystems() {
    // Initialize development timing
    stageStartTime_ = 0.0;
    stageDuration_ = 60.0; // Initial developmental stage duration
    
    // Initialize resource management
    availableResources_ = 1.0f;
    resourceRegenerationRate_ = 0.01f;
    
    // Initialize environmental tracking
    environmentalSignatures_.resize(10, 0.0f);
    lastEnvironmentalUpdate_ = 0.0;
    
    // Initialize learning systems
    learnedPatterns_.clear();
    patternWeights_.clear();
    patternTimestamps_.clear();
    
    // Initialize stress regulation
    currentStressLevel_ = 0.0f;
    stressRecoveryRate_ = 0.05f;
    lastStressEventTime_ = 0.0f;
    
    // Initialize goal management
    goalProgress_.clear();
    goalPriority_.clear();
    goalCompleted_.clear();
    
    // Initialize performance metrics
    performanceMetrics_.totalTimeActive_ = 0.0;
    performanceMetrics_.totalActionsTaken_ = 0;
    performanceMetrics_.averageActionSuccessRate_ = 0.5f;
    performanceMetrics_.learningEfficiency_ = 0.0;
    performanceMetrics_.explorationEfficiency_ = 0.0;
    performanceMetrics_.performanceHistory_.clear();
    
    // Setup motor and sensory neuron groups
    setupMotorNeuronGroups();
    setupSensoryNeuronGroups();
    
    NLM_LOG_INFO("AgentBrain advanced systems initialized");
}

void AgentBrain::configureFromConfig(const std::shared_ptr<Config>& config) {
    if (!config) return;
    
    // Personality configuration
    if (config->has("agent.personality.curiosity_level")) {
        personality_.curiosityLevel_ = config->getOr<float>("agent.personality.curiosity_level", 0.5f);
    }
    if (config->has("agent.personality.exploration_rate")) {
        personality_.explorationRate_ = config->getOr<float>("agent.personality.exploration_rate", 0.3f);
    }
    if (config->has("agent.personality.risk_tolerance")) {
        personality_.riskTolerance_ = config->getOr<float>("agent.personality.risk_tolerance", 0.5f);
    }
    if (config->has("agent.personality.persistence")) {
        personality_.persistence_ = config->getOr<float>("agent.personality.persistence", 0.5f);
    }
    
    // Neuromodulation configuration
    if (config->has("agent.neuromodulation.reward_sensitivity")) {
        personality_.rewardSensitivity_ = config->getOr<float>("agent.neuromodulation.reward_sensitivity", 0.7f);
    }
    if (config->has("agent.neuromodulation.punishment_sensitivity")) {
        personality_.punishmentSensitivity_ = config->getOr<float>("agent.neuromodulation.punishment_sensitivity", 0.3f);
    }
    
    // Learning configuration
    if (config->has("agent.learning.learning_rate")) {
        personality_.learningRate_ = config->getOr<float>("agent.learning.learning_rate", 0.5f);
    }
    if (config->has("agent.learning.memory_retention")) {
        personality_.memoryRetention_ = config->getOr<float>("agent.learning.memory_retention", 0.8f);
    }
    
    // Development configuration
    if (config->has("agent.development.development_stages")) {
        // Parse number of developmental stages
        int stages = config->getOr<int64_t>("agent.development.development_stages", 5);
        switch (stages) {
            case 1: stageDuration_ = 120.0; break;
            case 2: stageDuration_ = 60.0; break;
            case 3: stageDuration_ = 30.0; break;
            case 4: stageDuration_ = 20.0; break;
            case 5: stageDuration_ = 10.0; break;
            default: stageDuration_ = 60.0; break;
        }
    }
    
    NLM_LOG_INFO("AgentBrain configured from config file");
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Call legacy initialization for backward compatibility
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize advanced systems with proper safety checks
    advancedSystemsInitialized_ = false;
    errorRecoveryInitialized_ = false;
    performanceMetricsInitialized_ = false;
    
    // Setup all advanced systems in safe order
    setupDevelopmentTracking();
    setupResourceManagement();
    setupEnvironmentalTracking();
    setupLearningSystems();
    setupStressRegulation();
    setupGoalManagement();
    setupSocialSystems();
    setupErrorHandling();
    setupPerformanceMetrics();
    
    // Initialize all advanced systems in the correct order
    setupAdvancedNeuromodulation();
    setupAdvancedCuriosity();
    setupAdvancedPersonality();
    setupAdvancedDevelopment();
    setupAdvancedLearning();
    setupAdvancedMemory();
    setupAdvancedSocial();
    
    // Final verification and initialization
    verifySystemConsistency();
    finalizeInitialization();
    
    NLM_LOG_INFO("AgentBrain fully initialized with advanced systems");
}

void AgentBrain::initialize(const SimpleWorld& world, const AgentPersonality& personality) {
    // Set personality
    personality_ = personality;
    
    // Call regular initialization
    initialize(world);
    
    NLM_LOG_INFO("AgentBrain initialized with personality traits applied");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    // Reset to initial state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset advanced systems
    advancedDevelopmentalStage_ = AdvancedDevelopmentalStage::Embryonic;
    stageStartTime_ = 0.0;
    stageDuration_ = 60.0;
    availableResources_ = 1.0f;
    currentStressLevel_ = 0.0f;
    consecutiveErrors_ = 0;
    inErrorRecoveryMode_ = false;
    
    // Reset personality
    personality_ = AgentPersonality();
    
    // Reset behavioral state
    behavioralState_ = BehavioralState();
    
    // Reset performance metrics
    performanceMetrics_ = PerformanceMetrics();
    
    // Reset learning systems
    learnedPatterns_.clear();
    patternWeights_.clear();
    patternTimestamps_.clear();
    
    // Reset goal management
    goalProgress_.clear();
    goalPriority_.clear();
    goalCompleted_.clear();
    
    // Reset social systems
    socialConnections_.clear();
    socialPreferences_.clear();
    
    // Reset environmental tracking
    for (auto& sig : environmentalSignatures_) {
        sig = 0.0f;
    }
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reinitialize neuron groups
    setupMotorNeuronGroups();
    setupSensoryNeuronGroups();
    
    NLM_LOG_INFO("AgentBrain reset to initial state");
}

} // namespace nlm
