#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Configuration structure for AgentBrain parameters
struct AgentBrainConfig {
    // Neuron distribution
    static constexpr size_t MOTOR_GROUPS = 6;
    static constexpr size_t SENSORY_GROUPS = 4;
    
    // Sensory processing scale factors
    static constexpr float VISION_SCALE = 5.0f;
    static constexpr float TOUCH_SCALE = 8.0f;
    static constexpr float INTERNAL_SCALE = 10.0f;
    static constexpr float PROPRIOCEPTION_SCALE = 6.0f;
    
    // Sensory input sizes
    static constexpr size_t VISION_SIZE = 256;  // 16x16
    static constexpr size_t TOUCH_SIZE = 8;
    static constexpr size_t INTERNAL_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_SIZE = 6;
    
    // Motor output size
    static constexpr size_t MOTOR_OUTPUT_SIZE = 6;
    
    // Neuromodulation parameters
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float CURIOUS_FACTOR_NOVELTY = 2.0f;
    static constexpr float CURIOUS_FACTOR_PREDICTION = 0.5f;
    static constexpr float CURIOUS_THRESHOLD = 0.3f;
    static constexpr float EXPLORATION_THRESHOLD = 0.5f;
    
    // Reward modulation parameters
    static constexpr float DOSE_ALPHA = 1.0f;  // Simple exponential moving average
    static constexpr float DOSE_BETA = 0.05f;
    static constexpr float DOSE_MIN = -1.0f;
    static constexpr float DOSE_MAX = 1.0f;
    static constexpr float PLASTICITY_BASE = 0.5f;
    static constexpr float PLASTICITY_RANGE = 0.5f;
    
    // STDP parameters
    static constexpr float STDP_LTP_BASE = 0.01f;
    static constexpr float STDP_LTD_BASE = 0.012f;
    
    // Development parameters
    static constexpr double DEVELOPMENT_INITIAL = 60.0;  // seconds
    static constexpr double DEVELOPMENT_CRITICAL = 300.0;  // seconds
    static constexpr double DEVELOPMENT_MATURATION = 900.0;  // seconds
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity rates
    static constexpr float SYNAPTONGENESIS_BASE = 0.0001f;
    static constexpr float PRUNING_BASE = 0.00001f;
    
    // Activity threshold
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Exploration parameters
    static constexpr float EXPLORATION_PROBABILITY = 0.3f;
    static constexpr size_t MOTOR_COMMAND_COUNT = 8;
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
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
    , sensoryNoveltyDecay_(AgentBrainConfig::NOVELTY_DECAY_RATE)
{
    initializeNeuronDistribution();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronDistribution() {
    // Initialize motor and sensory neuron groups
    if (brain_) {
        distributeMotorNeurons();
        distributeSensoryNeurons();
    }
}

void AgentBrain::distributeMotorNeurons() {
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                distributeMotorPopulation(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                distributeSensoryPopulation(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorPopulation(const std::vector<Neuron*>& neurons) {
    size_t currentTotal = motorForward_.size() + motorBackward_.size() +
                          motorTurnLeft_.size() + motorTurnRight_.size() +
                          motorInteract_.size() + motorWait_.size();
    
    for (Neuron* n : neurons) {
        // Use modulo with configurable number of groups
        size_t group = currentTotal % AgentBrainConfig::MOTOR_GROUPS;
        switch (group) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
        ++currentTotal;
    }
}

void AgentBrain::distributeSensoryPopulation(const std::vector<Neuron*>& neurons) {
    size_t currentTotal = sensoryVision_.size() + sensoryTouch_.size() +
                          sensoryInternal_.size() + sensoryProprioception_.size();
    
    for (Neuron* n : neurons) {
        // Use modulo with configurable number of groups
        size_t group = currentTotal % AgentBrainConfig::SENSORY_GROUPS;
        switch (group) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
        ++currentTotal;
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = AgentBrainConfig::PLASTICITY_INITIAL;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return AgentBrainConfig::VISION_SIZE + AgentBrainConfig::TOUCH_SIZE + 
           AgentBrainConfig::INTERNAL_SIZE + AgentBrainConfig::PROPRIOCEPTION_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return AgentBrainConfig::MOTOR_OUTPUT_SIZE;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain_ is null");
        return;
    }
    
    processVisionInput(percept);
    processTouchInput(percept);
    processInternalInput(percept);
    processProprioceptionInput(percept);
    computeNovelty(percept);
    updateCuriosity();
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    for (size_t i = 0; i < std::min(sensoryVision_.size(), vision.size()); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * AgentBrainConfig::VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    
    for (size_t i = 0; i < std::min(sensoryTouch_.size(), touch.size()); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentBrainConfig::TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& internal = percept.getInternal();
    
    for (size_t i = 0; i < std::min(sensoryInternal_.size(), internal.size()); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * AgentBrainConfig::INTERNAL_SCALE;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprioception = percept.getProprioception();
    
    for (size_t i = 0; i < std::min(sensoryProprioception_.size(), proprioception.size()); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprioception[i] * 2.0f - 1.0f) * AgentBrainConfig::PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareSize = std::min(vision.size(), previousVision_.size());
    
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    noveltyLevel_ = totalDiff / std::max<size_t>(compareSize, 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    previousVision_ = vision;
}

void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    curiosityLevel_ = noveltyLevel_ * AgentBrainConfig::CURIOUS_FACTOR_NOVELTY + 
                      std::abs(predictionError_) * AgentBrainConfig::CURIOUS_FACTOR_PREDICTION;
    
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: brain_ is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConfig::CURIOUS_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    float forwardAct = calculateMotorActivity(motorForward_);
    float backwardAct = calculateMotorActivity(motorBackward_);
    float leftAct = calculateMotorActivity(motorTurnLeft_);
    float rightAct = calculateMotorActivity(motorTurnRight_);
    float interactAct = calculateMotorActivity(motorInteract_);
    float waitAct = calculateMotorActivity(motorWait_);
    
    return selectBestCommand(forwardAct, backwardAct, leftAct, rightAct, 
                             interactAct, waitAct);
}

float AgentBrain::calculateMotorActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (n) {  // Additional null check
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::selectBestCommand(float forwardAct, float backwardAct, float leftAct,
                                           float rightAct, float interactAct, float waitAct) {
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
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
    
    if (bestActivity < AgentBrainConfig::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ <= AgentBrainConfig::EXPLORATION_THRESHOLD) {
        return defaultCmd;
    }
    
    float exploreChance = curiosityLevel_ * AgentBrainConfig::EXPLORATION_PROBABILITY;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::getRandomMotorCommand() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConfig::MOTOR_COMMAND_COUNT - 1);
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
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
