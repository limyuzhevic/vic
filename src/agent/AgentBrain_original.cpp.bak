#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

// === SENSORYP PROCESSOR IMPLEMENTATION ===

void SensoryProcessor::processVision(const std::vector<float>& vision, NeuronGroup& visionGroup) {
    const size_t minSize = std::min(vision.size(), visionGroup.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (visionGroup.neurons[i]) {
            float current = normalizeAndScale(vision[i], AgentBrainConstants::VISION_SCALE_FACTOR);
            visionGroup.injectCurrent(current);
        }
    }
}

void SensoryProcessor::processTouch(const std::vector<float>& touch, NeuronGroup& touchGroup) {
    const size_t minSize = std::min(touch.size(), touchGroup.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (touchGroup.neurons[i]) {
            float current = normalizeAndScale(touch[i], AgentBrainConstants::TOUCH_SCALE_FACTOR);
            touchGroup.injectCurrent(current);
        }
    }
}

void SensoryProcessor::processInternal(const std::vector<float>& internal, NeuronGroup& internalGroup) {
    const size_t minSize = std::min(internal.size(), internalGroup.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (internalGroup.neurons[i]) {
            float normalized = (internal[i] * 2.0f - 1.0f);  // Center at 0
            float current = normalizeAndScale(normalized, AgentBrainConstants::INTERNAL_SCALE_FACTOR);
            internalGroup.injectCurrent(current);
        }
    }
}

void SensoryProcessor::processProprioception(const std::vector<float>& proprioception, NeuronGroup& proprioceptionGroup) {
    const size_t minSize = std::min(proprioception.size(), proprioceptionGroup.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (proprioceptionGroup.neurons[i]) {
            float normalized = (proprioception[i] * 2.0f - 1.0f);  // Center at 0
            float current = normalizeAndScale(normalized, AgentBrainConstants::PROPRIOCEPTION_SCALE_FACTOR);
            proprioceptionGroup.injectCurrent(current);
        }
    }
}

float SensoryProcessor::calculateNovelty(const std::vector<float>& currentVision, 
                                       const std::vector<float>& previousVision) {
    if (currentVision.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    const size_t minSize = std::min(currentVision.size(), previousVision.size());
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(currentVision[i] - previousVision[i]);
    }
    
    return totalDiff / std::max<size_t>(currentVision.size(), 1);
}

float SensoryProcessor::updateCuriosity(float novelty, float predictionError, 
                                       bool curiosityEnabled) {
    if (!curiosityEnabled) return 0.0f;
    
    float curiosity = novelty * AgentBrainConstants::NOVELTY_SENSITIVITY + 
                      std::abs(predictionError) * AgentBrainConstants::PREDICTION_ERROR_WEIGHT;
    return std::clamp(curiosity, 0.0f, 1.0f);
}

float SensoryProcessor::normalizeAndScale(float value, float scaleFactor) {
    // Simple linear scaling
    return value * scaleFactor;
}

const char* SensoryProcessor::getModalityName(SensoryModality modality) {
    switch (modality) {
        case SensoryModality::Vision: return "Vision";
        case SensoryModality::Touch: return "Touch";
        case SensoryModality::Internal: return "Internal";
        case SensoryModality::Proprioception: return "Proprioception";
        default: return "Unknown";
    }
}

bool SensoryProcessor::validateVisionSize(size_t size) {
    return size == AgentBrainConstants::EXPECTED_VISION_SIZE;
}

bool SensoryProcessor::validateTouchSize(size_t size) {
    return size == AgentBrainConstants::EXPECTED_TOUCH_SIZE;
}

bool SensoryProcessor::validateInternalSize(size_t size) {
    return size == AgentBrainConstants::EXPECTED_INTERNAL_SIZE;
}

bool SensoryProcessor::validateProprioceptionSize(size_t size) {
    return size == AgentBrainConstants::EXPECTED_PROPRIOCEPTION_SIZE;
}

// === MOTOR CONTROLLER IMPLEMENTATION ===

MotorCommand MotorController::decodeFromNeuronGroups(const std::map<MotorCommand, NeuronGroup>& motorGroups,
                                                    float activityThreshold) {
    // Calculate activity for each motor command group
    std::vector<std::pair<MotorCommand, float>> commandActivities;
    
    for (const auto& pair : motorGroups) {
        const MotorCommand& command = pair.first;
        const NeuronGroup& group = pair.second;
        
        float activity = calculateMotorActivity(group);
        commandActivities.emplace_back(command, activity);
    }
    
    return findBestCommand(commandActivities, activityThreshold);
}

float MotorController::calculateMotorActivity(const NeuronGroup& motorGroup) {
    if (motorGroup.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* neuron : motorGroup.neurons) {
        if (neuron) {
            sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
    }
    return sum / motorGroup.neurons.size();
}

MotorCommand MotorController::selectWithCuriosity(MotorCommand defaultCommand, float curiosityLevel,
                                                 float explorationThreshold, float exploreChanceMax,
                                                 std::shared_ptr<Brain> brain) {
    if (curiosityLevel <= explorationThreshold) {
        return defaultCommand;
    }
    
    // Calculate exploration probability (capped at exploreChanceMax)
    float exploreChance = std::min(curiosityLevel * 3.0f, exploreChanceMax);
    
    // Generate random number
    float r = 0.5f;  // TODO: Replace with actual random generator from brain
    if (brain) {
        r = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    }
    
    if (r < exploreChance) {
        return generateRandomCommand(brain);
    }
    
    return defaultCommand;
}

MotorCommand MotorController::findBestCommand(const std::vector<std::pair<MotorCommand, float>>& commandActivities,
                                             float activityThreshold) {
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    
    // Initialize with wait command activity (assuming 0 if not present)
    for (const auto& pair : commandActivities) {
        if (pair.first == MotorCommand::Wait) {
            bestActivity = pair.second;
            best = MotorCommand::Wait;
            break;
        }
    }
    
    // Find command with highest activity above threshold
    for (const auto& pair : commandActivities) {
        if (pair.second > bestActivity && pair.second > activityThreshold) {
            bestActivity = pair.second;
            best = pair.first;
        }
    }
    
    return best;
}

MotorCommand MotorController::generateRandomCommand(std::shared_ptr<Brain> brain) {
    // Generate random motor command
    int choice = 0;
    if (brain) {
        choice = brain->getRandomGenerator()->uniformInt(0, 7);
    }
    
    // Map random choice to motor command (excluding LookLeft/Right if not in original enum)
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

bool MotorController::isValidMotorCommand(MotorCommand command) {
    return command >= MotorCommand::MoveForward && command <= MotorCommand::Wait;
}

const char* MotorController::getCommandName(MotorCommand command) {
    switch (command) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

// === AGENT BRAIN IMPLEMENTATION ===

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(AgentBrainConstants::INITIAL_PLASTICITY)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
{
    // Initialize neuron groups
    motorGroups_ = {
        {MotorCommand::MoveForward, NeuronGroup("MoveForward", SensoryModality::Vision)},
        {MotorCommand::MoveBackward, NeuronGroup("MoveBackward", SensoryModality::Vision)},
        {MotorCommand::TurnLeft, NeuronGroup("TurnLeft", SensoryModality::Vision)},
        {MotorCommand::TurnRight, NeuronGroup("TurnRight", SensoryModality::Vision)},
        {MotorCommand::Interact, NeuronGroup("Interact", SensoryModality::Vision)},
        {MotorCommand::Wait, NeuronGroup("Wait", SensoryModality::Vision)}
    };
    
    sensoryGroups_ = {
        {SensoryModality::Vision, NeuronGroup("Vision", SensoryModality::Vision)},
        {SensoryModality::Touch, NeuronGroup("Touch", SensoryModality::Touch)},
        {SensoryModality::Internal, NeuronGroup("Internal", SensoryModality::Internal)},
        {SensoryModality::Proprioception, NeuronGroup("Proprioception", SensoryModality::Proprioception)}
    };
    
    // Distribute neurons from brain regions to appropriate groups
    distributeNeuronsToGroups();
    
    // Validate configuration
    if (!validateNeuronGroups()) {
        NLM_LOG_WARNING("AgentBrain initialization: Some neuron groups are empty or invalid");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = AgentBrainConstants::INITIAL_PLASTICITY;
    
    logNeuronGroupCounts();
}

void AgentBrain::distributeNeuronsToGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                // Distribute motor neurons to motor command groups
                size_t motorGroupCount = motorGroups_.size();
                size_t neuronCount = pop->getNeurons().size();
                
                for (size_t i = 0; i < neuronCount; ++i) {
                    Neuron* neuron = pop->getNeurons()[i];
                    MotorCommand cmd = static_cast<MotorCommand>(i % motorGroupCount);
                    motorGroups_[cmd].addNeuron(neuron);
                }
            } else if (type == NeuronType::Sensory) {
                // Distribute sensory neurons to sensory modality groups
                size_t sensoryGroupCount = sensoryGroups_.size();
                size_t neuronCount = pop->getNeurons().size();
                
                for (size_t i = 0; i < neuronCount; ++i) {
                    Neuron* neuron = pop->getNeurons()[i];
                    SensoryModality modality = static_cast<SensoryModality>(i % sensoryGroupCount);
                    sensoryGroups_[modality].addNeuron(neuron);
                }
            }
        }
    }
}

bool AgentBrain::validateNeuronGroups() const {
    bool allValid = true;
    
    // Check motor groups
    for (const auto& pair : motorGroups_) {
        if (pair.second.empty()) {
            NLM_LOG_WARNING("Motor group " + std::string(MotorController::getCommandName(pair.first)) + " is empty");
            allValid = false;
        }
    }
    
    // Check sensory groups
    for (const auto& pair : sensoryGroups_) {
        if (pair.second.empty()) {
            NLM_LOG_WARNING(std::string(SensoryProcessor::getModalityName(pair.first)) + " sensory group is empty");
            allValid = false;
        }
    }
    
    return allValid;
}

void AgentBrain::logNeuronGroupCounts() const {
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryGroups_.at(SensoryModality::Vision).size()) + " vision sensory neurons, " +
                 std::to_string(sensoryGroups_.at(SensoryModality::Touch).size()) + " touch sensory neurons, " +
                 std::to_string(sensoryGroups_.at(SensoryModality::Internal).size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return AgentBrainConstants::EXPECTED_VISION_SIZE + 
           AgentBrainConstants::EXPECTED_TOUCH_SIZE + 
           AgentBrainConstants::EXPECTED_INTERNAL_SIZE + 
           AgentBrainConstants::EXPECTED_PROPRIOCEPTION_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return static_cast<size_t>(AgentBrainConstants::MOTOR_GROUP_COUNT);
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process vision
    const auto& vision = percept.getVision();
    if (!vision.empty() && !sensoryGroups_.at(SensoryModality::Vision).empty()) {
        SensoryProcessor::processVision(vision, sensoryGroups_.at(SensoryModality::Vision));
    }
    
    // Process touch
    const auto& touch = percept.getTouch();
    if (!touch.empty() && !sensoryGroups_.at(SensoryModality::Touch).empty()) {
        SensoryProcessor::processTouch(touch, sensoryGroups_.at(SensoryModality::Touch));
    }
    
    // Process internal
    const auto& intern = percept.getInternal();
    if (!intern.empty() && !sensoryGroups_.at(SensoryModality::Internal).empty()) {
        SensoryProcessor::processInternal(intern, sensoryGroups_.at(SensoryModality::Internal));
    }
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && !sensoryGroups_.at(SensoryModality::Proprioception).empty()) {
        SensoryProcessor::processProprioception(proprio, sensoryGroups_.at(SensoryModality::Proprioception));
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        noveltyLevel_ = SensoryProcessor::calculateNovelty(vision, previousVision_);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = SensoryProcessor::updateCuriosity(noveltyLevel_, 
                                                           predictionError_, 
                                                           curiosityEnabled_);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::CURIOSITY_EXPLORATION_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    return MotorController::decodeFromNeuronGroups(motorGroups_);
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    return MotorController::selectWithCuriosity(defaultCmd, curiosityLevel_, 
                                               AgentBrainConstants::HIGH_CURIOSITY_EXPLORATION_CHANCE,
                                               AgentBrainConstants::EXPLORATION_RANDOM_MAX,
                                               brain_);
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = AgentBrainConstants::REWARD_FORGETTING_FACTOR * expectedReward_ + 
                     AgentBrainConstants::REWARD_LEARNING_RATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentBrainConstants::DOPAMINE_CLAMP_MIN, 
                                AgentBrainConstants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELASTICITY_TRACE_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConstants::PLASITICITY_BASE_MODIFIER + 
                            AgentBrainConstants::PLASITICITY_RANGE * dopamineLevel_;
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
    if (developmentalAge_ < AgentBrainConstants::INITIAL_STAGE_END) {
        plasticityModifier_ = AgentBrainConstants::INITIAL_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConstants::CRITICAL_PERIOD_END) {
        plasticityModifier_ = AgentBrainConstants::CRITICAL_PERIOD_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConstants::MATURATION_END) {
        plasticityModifier_ = AgentBrainConstants::MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConstants::ADULT_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConstants::SYNAPTROGENESIS_RATE_BASE * plasticityModifier_;
            float pruneRate = AgentBrainConstants::PRUNING_RATE_BASE * (2.0f - plasticityModifier_);
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
    plasticityModifier_ = AgentBrainConstants::INITIAL_PLASTICITY;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm

(End of file - total 1250 lines)