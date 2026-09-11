#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

const float AgentBrain::VISION_INPUT_SCALE_FACTOR = 5.0f;
const float AgentBrain::TOUCH_INPUT_SCALE_FACTOR = 8.0f;
const float AgentBrain::INTERNAL_INPUT_CENTER = 1.0f;
const float AgentBrain::INTERNAL_INPUT_SCALE = 2.0f;
const float AgentBrain::PROPRIOCEPTION_INPUT_CENTER = 1.0f;
const float AgentBrain::PROPRIOCEPTION_INPUT_SCALE = 2.0f;
const float AgentBrain::PROPRIOCEPTION_OUTPUT_SCALE = 3.0f;

const float AgentBrain::NOVELTY_DETECTION_DECAY_RATE = 0.99f;
const float AgentBrain::NOVELTY_NORMALIZATION_DIVISOR = 1.0f;
const float AgentBrain::CURIOUSITY_WEIGHT_NOVELTY = 2.0f;
const float AgentBrain::CURIOUSITY_WEIGHT_PREDICTION_ERROR = 0.5f;
const float AgentBrain::CURIOUSITY_THRESHOLD_FOR_EXPLORATION = 0.3f;
const float AgentBrain::MAX_CURIOUSITY_LEVEL = 1.0f;

const float AgentBrain::EXPLORATION_CURIOUSITY_THRESHOLD = 0.5f;
const float AgentBrain::EXPLORATION_PROBABILITY_MAX = 0.3f;
const int AgentBrain::EXPLORATION_RANDOM_CHOICE_MIN = 0;
const int AgentBrain::EXPLORATION_RANDOM_CHOICE_MAX = 7;

const float AgentBrain::ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
const float AgentBrain::ELIGIBILITY_TRACE_DECAY_RATE = 0.1f;
const float AgentBrain::PLASTICITY_BASE_FACTOR = 0.5f;
const float AgentBrain::PLASTICITY_MAX_FACTOR = 2.0f;
const float AgentBrain::PLASTICITY_MIN_FACTOR = 0.1f;
const float AgentBrain::STDP_POTENTIATION_BASE_WEIGHT = 0.01f;
const float AgentBrain::STDP_DEPRESSION_BASE_WEIGHT = 0.012f;
const float AgentBrain::EXPECTED_REWARD_DECAY_FACTOR = 0.95f;
const float AgentBrain::EXPECTED_REWARD_LEARNING_RATE = 0.05f;

const float AgentBrain::SYNAPTOGENESIS_BASE_RATE = 0.0001f;
const float AgentBrain::PRUNING_BASE_RATE = 0.00001f;

const double AgentBrain::DEVELOPMENTAL_STAGE_CRITICAL_PERIOD_START = 60.0;
const double AgentBrain::DEVELOPMENTAL_STAGE_MATURATION_END = 300.0;
const double AgentBrain::DEVELOPMENTAL_STAGE_ADULT_END = 900.0;

const float AgentBrain::STAGE_INITIAL_PLASTICITY = 1.0f;
const float AgentBrain::STAGE_CRITICAL_PERIOD_PLASTICITY = 0.8f;
const float AgentBrain::STAGE_MATURATION_PLASTICITY = 0.5f;
const float AgentBrain::STAGE_ADULT_PLASTICITY = 0.2f;

const float AgentBrain::MOTOR_ACTIVITY_THRESHOLD = 0.5f;
const float AgentBrain::MOTOR_ACTIVITY_RESTING_DEVIATION = 0.0f;

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
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
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
                        // Distribute sensory neurons
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
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = STAGE_INITIAL_PLASTICITY;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
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
            float current = vision[i] * VISION_INPUT_SCALE_FACTOR;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * TOUCH_INPUT_SCALE_FACTOR;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * INTERNAL_INPUT_SCALE - INTERNAL_INPUT_CENTER) * VISION_INPUT_SCALE_FACTOR;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * INTERNAL_INPUT_SCALE - PROPRIOCEPTION_INPUT_CENTER) * PROPRIOCEPTION_OUTPUT_SCALE;
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
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), NOVELTY_NORMALIZATION_DIVISOR);
        
        // Decay and update
        noveltyLevel_ *= NOVELTY_DETECTION_DECAY_RATE;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * CURIOUSITY_WEIGHT_NOVELTY + std::abs(predictionError_) * CURIOUSITY_WEIGHT_PREDICTION_ERROR;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, MAX_CURIOUSITY_LEVEL);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOUSITY_THRESHOLD_FOR_EXPLORATION) {
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
    
    // Find maximum activity using a simple search
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
    
    // Find command with maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& cmd : commands) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            best = cmd.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > EXPLORATION_CURIOUSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_PROBABILITY_MAX;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(EXPLORATION_RANDOM_CHOICE_MIN, EXPLORATION_RANDOM_CHOICE_MAX);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case MOTOR_COMMAND_LOOK_LEFT: return MotorCommand::LookLeft;
                case MOTOR_COMMAND_LOOK_RIGHT: return MotorCommand::LookRight;
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
    expectedReward_ = EXPECTED_REWARD_DECAY_FACTOR * expectedReward_ + EXPECTED_REWARD_LEARNING_RATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > ELIGIBILITY_TRACE_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(ELIGIBILITY_TRACE_DECAY_RATE);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = PLASTICITY_BASE_FACTOR + PLASTICITY_MAX_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, PLASTICITY_MIN_FACTOR, PLASTICITY_MAX_FACTOR);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(STDP_POTENTIATION_BASE_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(STDP_DEPRESSION_BASE_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < DEVELOPMENTAL_STAGE_CRITICAL_PERIOD_START) {  // ~1 minute
        plasticityModifier_ = STAGE_INITIAL_PLASTICITY;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DEVELOPMENTAL_STAGE_MATURATION_END) {  // ~5 minutes
        plasticityModifier_ = STAGE_CRITICAL_PERIOD_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DEVELOPMENTAL_STAGE_ADULT_END) {  // ~15 minutes
        plasticityModifier_ = STAGE_MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = STAGE_ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = PRUNING_BASE_RATE * (PLASTICITY_MAX_FACTOR - plasticityModifier_);
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
    // Reset all neuromodulation signals to baseline
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset developmental state to initial stage
    developmentalAge_ = 0.0;
    plasticityModifier_ = STAGE_INITIAL_PLASTICITY;
    
    // Clear previous sensory input to force fresh processing in next episode
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Note: The brain pointer (brain_) remains valid as it references the
    // underlying neural substrate that persists across agent resets
}

} // namespace nlm
