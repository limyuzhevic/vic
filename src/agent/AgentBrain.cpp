// ===== AGENTBRAIN.CPP - CLEAN IMPLEMENTATION WITH ADVANCED FEATURES =====

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <memory>

namespace nlm {

// ===== CONSTANTS FOR MAINTAINABILITY =====

namespace {
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float MAX_CURIOUSNESS_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE = 0.3f;
    constexpr float PLASTICITY_MIN = 0.1f;
    constexpr float PLASTICITY_MAX = 2.0f;
    constexpr float NOISE_THRESHOLD = 0.5f;
    constexpr float ELIGIBILITY_DECAY_RATE = 0.1f;
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float INITIAL_PLATICITY_MODIFIER = 1.0f;
    constexpr float CRITICALLY_HIGH_NOVELTY = 0.3f;
    constexpr float INITIAL_NOVELTY = 0.0f;
    constexpr float INITIAL_CURIOUSNESS = 0.0f;
    constexpr float INITIAL_PREDICTION_ERROR = 0.0f;
    constexpr float INITIAL_EXPECTED_REWARD = 0.0f;
    constexpr float INITIAL_DEVELOPMENTAL_AGE = 0.0;
    constexpr float INITIAL_DOPAMINE_LEVEL = 0.0f;
    constexpr float INITIAL_ACTVITY_THRESHOLD = 0.3f;
} // namespace

// ===== CONSTRUCTOR WITH PROPER INITIALIZATION =====

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    // Initialize state with default values
    , dopamineLevel_(INITIAL_DOPAMINE_LEVEL)
    , noveltyLevel_(INITIAL_NOVELTY)
    , curiosityLevel_(INITIAL_CURIOUSNESS)
    , predictionError_(INITIAL_PREDICTION_ERROR)
    , expectedReward_(INITIAL_EXPECTED_REWARD)
    , developmentalAge_(INITIAL_DEVELOPMENTAL_AGE)
    , plasticityModifier_(INITIAL_PLATICITY_MODIFIER)
    // Initialize configuration flags
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    // Initialize novelty detection
    , sensoryNoveltyDecay_(DEFAULT_NOVELTY_DECAY)
    // Initialize exploration system
    , explorationStrategy_(ExplorationStrategy::ThompsonSampling)
    , useEntropyBonus_(true)
    , ucbExplorationFactor_(2.0)
    , metacognitiveMonitoringEnabled_(true)
    // Initialize advanced features flags
    , hierarchicalStateEnabled_(false)
    , workingMemoryEnabled_(false)
    , predictiveCodingEnabled_(false)
    , homeostaticEnabled_(false)
    , crossModalIntegrationEnabled_(false)
    , batchProcessingEnabled_(false)
    , memoryPoolingEnabled_(false)
    , earlyTerminationEnabled_(false)
    // Initialize pointers to null
    , explorationSystem_(nullptr)
    , enhancedLearning_(nullptr)
    , stateManagement_(nullptr)
    , neuromodulation_(nullptr)
    , performanceOpt_(nullptr)
    , randomGenerator_(nullptr)
{
    // Critical null pointer validation
    if (!brain_) {
        NLM_LOG_ERROR("Critical: Cannot create AgentBrain with null brain pointer");
        assert(brain_ != nullptr);
    }
    
    // Initialize random generator for advanced features
    randomGenerator_ = std::make_shared<RandomGenerator>();
    
    // Initialize neuron groups if brain is available
    if (brain_) {
        initializeNeuronGroups();
    }
    
    // Initialize advanced feature systems
    initializeAdvancedFeatures();
}

// ===== INITIALIZATION HELPERS =====

void AgentBrain::initializeNeuronGroups() {
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                motorTurnLeft_.size() + motorTurnRight_.size() +
                                motorInteract_.size() + motorWait_.size();
                    
                    switch (idx % 6) {
                        case 0: motorForward_.push_back(n); break;
                        case 1: motorBackward_.push_back(n); break;
                        case 2: motorTurnLeft_.push_back(n); break;
                        case 3: motorTurnRight_.push_back(n); break;
                        case 4: motorInteract_.push_back(n); break;
                        case 5: motorWait_.push_back(n); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(n); break;
                        case 1: sensoryTouch_.push_back(n); break;
                        case 2: sensoryInternal_.push_back(n); break;
                        case 3: sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
}

void AgentBrain::initializeAdvancedFeatures() {
    // Initialize exploration system
    explorationSystem_ = std::make_unique<AdaptiveExplorationSystem>(randomGenerator_);
    explorationSystem_->initializeActionSpace(getMotorOutputSize());
    explorationSystem_->setExplorationStrategy(explorationStrategy_);
    explorationSystem_->setUseEntropyBonus(useEntropyBonus_);
    explorationSystem_->setUCBExplorationFactor(ucbExplorationFactor_);
    
    // Initialize other advanced features if enabled
    if (metacognitiveMonitoringEnabled_) {
        enhancedLearning_ = std::make_unique<EnhancedLearningMechanisms>(brain_, randomGenerator_);
    }
    
    if (hierarchicalStateEnabled_ || workingMemoryEnabled_) {
        stateManagement_ = std::make_unique<ImprovedStateManagement>(brain_, randomGenerator_);
    }
    
    if (predictiveCodingEnabled_ || homeostaticEnabled_ || crossModalIntegrationEnabled_) {
        neuromodulation_ = std::make_unique<AdvancedNeuromodulation>(brain_, randomGenerator_);
    }
    
    if (batchProcessingEnabled_ || memoryPoolingEnabled_ || earlyTerminationEnabled_) {
        performanceOpt_ = std::make_unique<PerformanceOptimization>(brain_);
    }
}

// ===== DESTRUCTOR FOR MEMORY CLEANUP =====

AgentBrain::~AgentBrain() {
    // Clean up all neuron group vectors
    clearNeuronGroups();
    
    // Release advanced feature systems
    explorationSystem_.reset();
    enhancedLearning_.reset();
    stateManagement_.reset();
    neuromodulation_.reset();
    performanceOpt_.reset();
}

void AgentBrain::clearNeuronGroups() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
}

// ===== INITIALIZATION =====

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize with world configuration
    initializeWithWorld(world);
    
    // Reinitialize advanced features
    reinitializeAdvancedFeatures(world);
}

void AgentBrain::initializeWithWorld(const SimpleWorld& world) {
    // Configure novelty detection
    sensoryNoveltyDecay_ = DEFAULT_NOVELTY_DECAY;
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = INITIAL_PLATICITY_MODIFIER;
}

void AgentBrain::reinitializeAdvancedFeatures(const SimpleWorld& world) {
    // Reinitialize exploration system
    if (explorationSystem_) {
        explorationSystem_->initializeActionSpace(getMotorOutputSize());
        explorationSystem_->setExplorationStrategy(explorationStrategy_);
        explorationSystem_->setUseEntropyBonus(useEntropyBonus_);
        explorationSystem_->setUCBExplorationFactor(ucbExplorationFactor_);
    }
    
    // Reinitialize other advanced features
    if (enhancedLearning_) {
        enhancedLearning_->setCurrentSkill(0);
    }
    
    if (stateManagement_) {
        stateManagement_->clearOldStates();
    }
    
    if (performanceOpt_) {
        performanceOpt_->setBatchSize(batchProcessingEnabled_ ? 1000 : 1);
        if (memoryPoolingEnabled_) {
            performanceOpt_->createMemoryPool(brain_->getTotalNeuronCount());
        }
    }
}

// ===== RESET =====

void AgentBrain::reset() {
    // Reset all state variables
    resetStateVariables();
    
    // Clear neuron groups
    clearNeuronGroups();
    
    // Reset advanced features
    resetAdvancedFeatures();
}

void AgentBrain::resetStateVariables() {
    dopamineLevel_ = INITIAL_DOPAMINE_LEVEL;
    noveltyLevel_ = INITIAL_NOVELTY;
    curiosityLevel_ = INITIAL_CURIOUSNESS;
    predictionError_ = INITIAL_PREDICTION_ERROR;
    expectedReward_ = INITIAL_EXPECTED_REWARD;
    developmentalAge_ = INITIAL_DEVELOPMENTAL_AGE;
    plasticityModifier_ = INITIAL_PLATICITY_MODIFIER;
    
    // Clear vision buffer
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

void AgentBrain::resetAdvancedFeatures() {
    // Reset exploration system
    if (explorationSystem_) {
        explorationSystem_->reset();
    }
    
    // Reset state management
    if (stateManagement_) {
        stateManagement_->clearOldStates();
    }
    
    // Release memory pool
    if (performanceOpt_ && memoryPoolingEnabled_) {
        performanceOpt_->releaseMemoryPool();
    }
}

// ===== SENSORY PROCESSING =====

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!validateBrain()) {
        return;
    }
    
    // Process all sensory modalities
    processVisionInput(percept);
    processTouchInput(percept);
    processInternalInput(percept);
    processProprioceptiveInput(percept);
    
    // Update advanced features
    updateAdvancedFeaturesWithPercept(percept);
}

bool AgentBrain::validateBrain() const {
    if (!brain_) {
        NLM_LOG_WARNING("processSensoryInput: brain_ is null - ignoring sensory input");
        return false;
    }
    return true;
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptiveInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateAdvancedFeaturesWithPercept(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    // Update state management
    if (stateManagement_) {
        std::string rawVision(vision.begin(), vision.end());
        stateManagement_->addRecentState(rawVision, 0.0);
        
        if (hierarchicalStateEnabled_) {
            // Process hierarchical state abstraction
        }
        
        if (workingMemoryEnabled_) {
            stateManagement_->consolidateWorkingMemory();
        }
    }
    
    // Update neuromodulation with predictive coding
    if (neuromodulation_ && predictiveCodingEnabled_) {
        neuromodulation_->computePredictionErrors("", 0.0f);
    }
    
    // Update exploration with curiosity-driven learning
    if (curiosityEnabled_ && enhancedLearning_) {
        updateNoveltyAndCuriosity(vision);
    }
}

void AgentBrain::updateNoveltyAndCuriosity(const std::vector<float>& vision) {
    if (vision.empty()) {
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareSize = std::min(vision.size(), previousVision_.size());
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    size_t divisor = compareSize > 0 ? compareSize : 1;
    noveltyLevel_ = totalDiff / static_cast<float>(divisor);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    previousVision_ = vision;
}

// ===== MOTOR COMMAND DECODING =====

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrain()) {
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply advanced exploration
    if (explorationSystem_) {
        MotorCommand explored = explorationSystem_->selectActionThompsonSampling();
        if (explored != MotorCommand::Wait) {
            decoded = explored;
        }
    }
    
    // Apply curiosity-driven exploration
    if (curiosityEnabled_ && curiosityLevel_ > NOISE_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
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
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
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
    
    if (bestActivity < 0.3f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityEnabled_ && curiosityLevel_ > MAX_CURIOUSNESS_THRESHOLD) {
        if (explorationSystem_) {
            // Advanced exploration system handles this
            return explorationSystem_->selectActionThompsonSampling();
        }
    }
    
    return defaultCmd;
}

// ===== REWARD MODULATION =====

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!validateBrain()) {
        return;
    }
    
    if (!rewardModulationEnabled_) {
        NLM_LOG_DEBUG("Reward modulation disabled - ignoring reward signals");
        return;
    }
    
    // Compute prediction error and dopamine signal
    computeDopamineSignal(reward, predictedReward);
    
    // Apply synaptic changes
    applySynapticPlasticity();
    
    // Apply advanced neuromodulation
    if (neuromodulation_) {
        neuromodulation_->computePredictionErrors("", predictedReward);
        neuromodulation_->regulateHomeostasis(0.001);
        neuromodulation_->integrateCrossModal({}, {});
    }
    
    // Apply reward shaping
    if (enhancedLearning_) {
        float shapedReward = enhancedLearning_->applyRewardShaping(reward);
    }
}

void AgentBrain::computeDopamineSignal(float reward, float predictedReward) {
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
}

void AgentBrain::applySynapticPlasticity() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

// ===== DEVELOPMENT =====

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrain()) {
        return;
    }
    
    if (!developmentEnabled_) {
        NLM_LOG_DEBUG("Development disabled - skipping developmental updates");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Update developmental stage based on age
    updateDevelopmentalStage();
    
    // Apply advanced learning mechanisms
    if (enhancedLearning_) {
        enhancedLearning_->monitorLearning();
    }
    
    // Apply performance optimization
    if (performanceOpt_) {
        if (earlyTerminationEnabled_) {
            // Check for convergence
        }
        
        if (batchProcessingEnabled_) {
            performanceOpt_->executeBatchOperation({}, timestep);
        }
    }
    
    // Update stress and recovery
    if (stateManagement_) {
        double stress = stateManagement_->assessStress();
        if (stress > 0.7 && homeostaticEnabled_) {
            stateManagement_->performRecoveryAction();
        }
    }
}

void AgentBrain::updateDevelopmentalStage() {
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

// ===== GETTERS =====

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_WARNING("getDevelopmentalStage: brain_ is null");
        return DevelopmentalStage::Initial;
    }
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

} // namespace nlm