#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Helper function to calculate average neuron activity
static float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return AgentBrainConstants::DEFAULT_WAIT_ACTIVITY;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// Helper function to find motor command with maximum activity
static MotorCommand findMaxActivityMotorCommand(
    const struct MotorCommandActivity& commands[]) {
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = commands[5].activity; // Wait command index
    
    for (int i = 0; i < 5; ++i) {
        if (commands[i].activity > bestActivity) {
            bestActivity = commands[i].activity;
            best = commands[i].cmd;
        }
    }
    return best;
}

// Motor neuron distribution configuration
struct MotorNeuronDistribution {
    std::vector<Neuron*>& group;
    float scaleFactor;
    const char* name;
};

// Sensory neuron distribution configuration  
struct SensoryNeuronDistribution {
    std::vector<Neuron*>& group;
    float scaleFactor;
    const char* name;
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
    , sensoryNoveltyDecay_(AgentBrainConstants::NOVELTY_DECAY_FACTOR)
{
    // Initialize neuron groups with error checking
    initializeNeuronGroups();
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot initialize without brain");
        return;
    }
    
    // Clear existing groups
    clearNeuronGroups();
    
    // Distribute motor neurons
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
    
    NLM_LOG_INFO("AgentBrain: Distributed " + 
                 std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait motor neurons");
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size();
        
        switch (idx % AgentBrainConstants::MOTOR_GROUPS_COUNT) {
            case AgentBrainConstants::MOTOR_FORWARD:
                motorForward_.push_back(n);
                break;
            case AgentBrainConstants::MOTOR_BACKWARD:
                motorBackward_.push_back(n);
                break;
            case AgentBrainConstants::MOTOR_TURN_LEFT:
                motorTurnLeft_.push_back(n);
                break;
            case AgentBrainConstants::MOTOR_TURN_RIGHT:
                motorTurnRight_.push_back(n);
                break;
            case AgentBrainConstants::MOTOR_INTERACT:
                motorInteract_.push_back(n);
                break;
            case AgentBrainConstants::MOTOR_WAIT:
                motorWait_.push_back(n);
                break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size();
        
        switch (idx % AgentBrainConstants::SENSORY_GROUPS_COUNT) {
            case AgentBrainConstants::SENSORY_VISION:
                sensoryVision_.push_back(n);
                break;
            case AgentBrainConstants::SENSORY_TOUCH:
                sensoryTouch_.push_back(n);
                break;
            case AgentBrainConstants::SENSORY_INTERNAL:
                sensoryInternal_.push_back(n);
                break;
            case AgentBrainConstants::SENSORY_PROPRIOCEPTION:
                sensoryProprioception_.push_back(n);
                break;
        }
    }
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    validateWorld(world);
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::validateWorld(const SimpleWorld& world) {
    if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
        NLM_LOG_WARNING("AgentBrain: Invalid world dimensions");
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;  // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;  // One motor neuron per action
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot process sensory input without brain");
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
    injectScaledCurrent(sensoryVision_, vision, AgentBrainConstants::SENSORY_VISION_SCALE);
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    injectScaledCurrent(sensoryTouch_, touch, AgentBrainConstants::SENSORY_TOUCH_SCALE);
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    // Center and scale: (value * 2.0f - 1.0f) * 5.0f
    injectCenteredScaledCurrent(sensoryInternal_, intern, 2.0f, 5.0f);
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    // Center and scale: (value * 2.0f - 1.0f) * 3.0f
    injectCenteredScaledCurrent(sensoryProprioception_, proprio, 2.0f, 3.0f);
}

void AgentBrain::injectScaledCurrent(const std::vector<Neuron*>& neurons, 
                                     const std::vector<float>& input, 
                                     float scaleFactor) {
    for (size_t i = 0; i < neurons.size() && i < input.size(); ++i) {
        if (neurons[i]) {
            float current = input[i] * scaleFactor;
            neurons[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectCenteredScaledCurrent(const std::vector<Neuron*>& neurons,
                                              const std::vector<float>& input,
                                              float centerFactor,
                                              float scaleFactor) {
    for (size_t i = 0; i < neurons.size() && i < input.size(); ++i) {
        if (neurons[i]) {
            float current = (input[i] * centerFactor - 1.0f) * scaleFactor;
            neurons[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), AgentBrainConstants::MIN_VISION_SIZE);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        previousVision_ = vision;
    }
}

void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) return;
    
    curiosityLevel_ = noveltyLevel_ * AgentBrainConstants::CURIOUSITY_NOVELTY_WEIGHT + 
                     std::abs(predictionError_) * AgentBrainConstants::CURIOUSITY_ERROR_WEIGHT;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, AgentBrainConstants::MAX_CURIOUSITY_LEVEL);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot decode motor command without brain");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::CURIOSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    float forwardAct = calculateAverageActivity(motorForward_);
    float backwardAct = calculateAverageActivity(motorBackward_);
    float leftAct = calculateAverageActivity(motorTurnLeft_);
    float rightAct = calculateAverageActivity(motorTurnRight_);
    float interactAct = calculateAverageActivity(motorInteract_);
    float waitAct = calculateAverageActivity(motorWait_);
    
    struct MotorCommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = findMaxActivityMotorCommand(commands);
    
    if (best != MotorCommand::Wait && calculateActivityForCommand(best) < AgentBrainConstants::MIN_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

float AgentBrain::calculateActivityForCommand(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return calculateAverageActivity(motorForward_);
        case MotorCommand::MoveBackward: return calculateAverageActivity(motorBackward_);
        case MotorCommand::TurnLeft: return calculateAverageActivity(motorTurnLeft_);
        case MotorCommand::TurnRight: return calculateAverageActivity(motorTurnRight_);
        case MotorCommand::Interact: return calculateAverageActivity(motorInteract_);
        case MotorCommand::Wait: return calculateAverageActivity(motorWait_);
        default: return 0.0f;
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ <= AgentBrainConstants::HIGH_CURIOSITY_THRESHOLD) {
        return defaultCmd;
    }
    
    float exploreChance = curiosityLevel_ * AgentBrainConstants::MAX_EXPLORATION_CHANCE;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::getRandomMotorCommand() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConstants::MAX_RANDOM_MOTOR_COMMAND);
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
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConstants::DOPAMINE_CLAMP_MIN, AgentBrainConstants::DOPAMINE_CLAMP_MAX);
    
    applySynapticPlasticity();
    modulatePlasticityBasedOnDopamine();
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

void AgentBrain::modulatePlasticityBasedOnDopamine() {
    float plasticityFactor = AgentBrainConstants::PLASTICITY_FACTOR_BASE + 
                            AgentBrainConstants::PLASTICITY_FACTOR_STDEV * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConstants::STDP_DEFAULT_LTP * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConstants::STDP_DEFAULT_LTD * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_INITIAL) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_INITIAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_CRITICAL) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_MATURATION) {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_MATURATION;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_ADULT;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity();
    }
}

void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    float synRate = AgentBrainConstants::SYNAPTOGENESIS_RATE_INITIAL * plasticityModifier_;
    float pruneRate = AgentBrainConstants::PRUNING_RATE_INITIAL * (2.0f - plasticityModifier_);
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
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
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
