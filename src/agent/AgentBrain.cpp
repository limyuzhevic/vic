#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Constants for AgentBrain - extracted from magic numbers
namespace {
    constexpr float DEFAULT_DOPAMINE_LEVEL = 0.0f;
    constexpr float DEFAULT_NOVELTY_LEVEL = 0.0f;
    constexpr float DEFAULT_CURIOSITY_LEVEL = 0.0f;
    constexpr float DEFAULT_PREDICTION_ERROR = 0.0f;
    constexpr float DEFAULT_EXPECTED_REWARD = 0.0f;
    constexpr double DEFAULT_DEVELOPMENTAL_AGE = 0.0;
    constexpr float DEFAULT_PLASTICITY_MODIFIER = 1.0f;
    constexpr float DEFAULT_SENSORY_NOVELTY_DECAY = 0.99f;
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_CENTER_FACTOR = 2.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_CENTER_FACTOR = 2.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    constexpr float NOVELTY_NORMALIZATION_EPS = 0.001f;
    constexpr float REWARD_MOMENTUM_FACTOR = 0.95f;
    constexpr float REWARD_UPDATE_FACTOR = 0.05f;
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_SENSITIVITY = 0.5f;
    constexpr float STDP_LTP_WEIGHT_BASE = 0.01f;
    constexpr float STDP_LTD_WEIGHT_BASE = 0.012f;
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    constexpr float PRUNING_BASE_RATE = 0.00001f;
    constexpr float DEVELOPMENT_INITIAL_STAGE_AGE = 60.0;
    constexpr float DEVELOPMENT_CRITICAL_PERIOD_AGE = 300.0;
    constexpr float DEVELOPMENT_MATURATION_AGE = 900.0;
    
    // Motor neuron distribution groups
    constexpr size_t MOTOR_FORWARD_GROUP = 0;
    constexpr size_t MOTOR_BACKWARD_GROUP = 1;
    constexpr size_t MOTOR_TURN_LEFT_GROUP = 2;
    constexpr size_t MOTOR_TURN_RIGHT_GROUP = 3;
    constexpr size_t MOTOR_INTERACT_GROUP = 4;
    constexpr size_t MOTOR_WAIT_GROUP = 5;
    constexpr size_t NUM_MOTOR_GROUPS = 6;
    
    // Sensory neuron distribution groups
    constexpr size_t SENSORY_VISION_GROUP = 0;
    constexpr size_t SENSORY_TOUCH_GROUP = 1;
    constexpr size_t SENSORY_INTERNAL_GROUP = 2;
    constexpr size_t SENSORY_PROPRIOCEPTION_GROUP = 3;
    constexpr size_t NUM_SENSORY_GROUPS = 4;
    
    // Fixed input/output sizes
    constexpr size_t VISION_INPUT_SIZE = 256;      // 16x16 vision
    constexpr size_t TOUCH_INPUT_SIZE = 8;
    constexpr size_t INTERNAL_INPUT_SIZE = 4;
    constexpr size_t PROPRIOCEPTION_INPUT_SIZE = 6;
    constexpr size_t TOTAL_SENSES_INPUT_SIZE = VISION_INPUT_SIZE + TOUCH_INPUT_SIZE + 
                                            INTERNAL_INPUT_SIZE + PROPRIOCEPTION_INPUT_SIZE;
    
    constexpr size_t NUM_MOTOR_OUTPUTS = 6;
    
    // Activity thresholds
    constexpr float MIN_ACTIVITY_FOR_ACTION = 0.5f;
    constexpr float CURIOSITY_THRESHOLD_FOR_EXPLORATION = 0.3f;
    constexpr float HIGH_CURIOSITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;
    constexpr float NOVELTY_WEIGHT = 2.0f;
    constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
    
    // Development stage age boundaries
    constexpr float DEVELOPMENT_STAGE_1_END = 60.0;   // Initial stage
    constexpr float DEVELOPMENT_STAGE_2_END = 300.0; // Critical period
    constexpr float DEVELOPMENT_STAGE_3_END = 900.0; // Maturation
} // namespace

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(DEFAULT_DOPAMINE_LEVEL)
    , noveltyLevel_(DEFAULT_NOVELTY_LEVEL)
    , curiosityLevel_(DEFAULT_CURIOSITY_LEVEL)
    , predictionError_(DEFAULT_PREDICTION_ERROR)
    , expectedReward_(DEFAULT_EXPECTED_REWARD)
    , developmentalAge_(DEFAULT_DEVELOPMENTAL_AGE)
    , plasticityModifier_(DEFAULT_PLASTICITY_MODIFIER)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(DEFAULT_SENSORY_NOVELTY_DECAY)
{
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot create with null brain pointer");
    }
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("AgentBrain: Found null neuron in motor population");
                            continue;
                        }
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        // Convert to group ID using modulo
                        size_t groupId = idx % NUM_MOTOR_GROUPS;
                        switch (groupId) {
                            case MOTOR_FORWARD_GROUP: motorForward_.push_back(n); break;
                            case MOTOR_BACKWARD_GROUP: motorBackward_.push_back(n); break;
                            case MOTOR_TURN_LEFT_GROUP: motorTurnLeft_.push_back(n); break;
                            case MOTOR_TURN_RIGHT_GROUP: motorTurnRight_.push_back(n); break;
                            case MOTOR_INTERACT_GROUP: motorInteract_.push_back(n); break;
                            case MOTOR_WAIT_GROUP: motorWait_.push_back(n); break;
                            default: NLM_LOG_WARNING("AgentBrain: Invalid motor group ID: " + std::to_string(groupId));
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARNING("AgentBrain: Found null neuron in sensory population");
                            continue;
                        }
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        // Convert to group ID using modulo
                        size_t groupId = idx % NUM_SENSORY_GROUPS;
                        switch (groupId) {
                            case SENSORY_VISION_GROUP: sensoryVision_.push_back(n); break;
                            case SENSORY_TOUCH_GROUP: sensoryTouch_.push_back(n); break;
                            case SENSORY_INTERNAL_GROUP: sensoryInternal_.push_back(n); break;
                            case SENSORY_PROPRIOCEPTION_GROUP: sensoryProprioception_.push_back(n); break;
                            default: NLM_LOG_WARNING("AgentBrain: Invalid sensory group ID: " + std::to_string(groupId));
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + std::to_string(motorForward_.size()) + " forward neurons, " +
                 std::to_string(motorBackward_.size()) + " backward neurons, " +
                 std::to_string(motorTurnLeft_.size()) + " left neurons, " +
                 std::to_string(motorTurnRight_.size()) + " right neurons, " +
                 std::to_string(motorInteract_.size()) + " interact neurons, " +
                 std::to_string(motorWait_.size()) + " wait neurons");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world dimensions
    size_t expectedVisionSize = world.getVisionWidth() * world.getVisionHeight();
    if (previousVision_.size() != expectedVisionSize) {
        previousVision_.resize(expectedVisionSize, 0.0f);
    }
    
    developmentalAge_ = DEFAULT_DEVELOPMENTAL_AGE;
    plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    return TOTAL_SENSES_INPUT_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    return NUM_MOTOR_OUTPUTS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot process sensory input with null brain");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty() && sensoryVision_.size() > 0) {
        size_t minSize = std::min(vision.size(), sensoryVision_.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * VISION_SCALE_FACTOR;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            } else {
                NLM_LOG_DEBUG("AgentBrain: Null sensoryVision_[" + std::to_string(i) + "]");
            }
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty() && sensoryTouch_.size() > 0) {
        size_t minSize = std::min(touch.size(), sensoryTouch_.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * TOUCH_SCALE_FACTOR;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            } else {
                NLM_LOG_DEBUG("AgentBrain: Null sensoryTouch_[" + std::to_string(i) + "]");
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty() && sensoryInternal_.size() > 0) {
        size_t minSize = std::min(intern.size(), sensoryInternal_.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * INTERNAL_CENTER_FACTOR - 1.0f) * INTERNAL_SCALE_FACTOR;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            } else {
                NLM_LOG_DEBUG("AgentBrain: Null sensoryInternal_[" + std::to_string(i) + "]");
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && sensoryProprioception_.size() > 0) {
        size_t minSize = std::min(proprio.size(), sensoryProprioception_.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * PROPRIOCEPTION_CENTER_FACTOR - 1.0f) * PROPRIOCEPTION_SCALE_FACTOR;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            } else {
                NLM_LOG_DEBUG("AgentBrain: Null sensoryProprioception_[" + std::to_string(i) + "]");
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
        
        NLM_LOG_DEBUG("AgentBrain: Novelty computed: " + std::to_string(noveltyLevel_));
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * NOVELTY_WEIGHT + std::abs(predictionError_) * PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Cannot decode motor command with null brain");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOSITY_THRESHOLD_FOR_EXPLORATION) {
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
            if (!n) {
                NLM_LOG_WARNING("AgentBrain: Null neuron in motor group activity calculation");
                continue;
            }
            // Use membrane potential deviation from rest as activity measure
            float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
            sum += activity;
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
    if (bestActivity < MIN_ACTIVITY_FOR_ACTION) {
        return MotorCommand::Wait;
    }
    
    NLM_LOG_DEBUG("AgentBrain: Selected command " + std::to_string(static_cast<int>(best)) + 
                 " with activity " + std::to_string(bestActivity));
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > HIGH_CURIOSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_CHANCE_FACTOR;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            NLM_LOG_DEBUG("AgentBrain: Exploring (curiosity=" + std::to_string(curiosityLevel_) + ")");
            
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
                default: NLM_LOG_DEBUG("AgentBrain: Random choice " + std::to_string(choice) + " - using Wait");
                         return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_DEBUG("AgentBrain: Reward modulation disabled or null brain");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = REWARD_MOMENTUM_FACTOR * expectedReward_ + REWARD_UPDATE_FACTOR * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, DOPAMINE_CLAMP_MIN, DOPAMINE_CLAMP_MAX);
    
    NLM_LOG_DEBUG("AgentBrain: Reward modulation - reward: " + std::to_string(reward) +
                 ", predicted: " + std::to_string(predictedReward) +
                 ", error: " + std::to_string(predictionError_) +
                 ", dopamine: " + std::to_string(dopamineLevel_));
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_WARNING("AgentBrain: Null synapse encountered");
                continue;
            }
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
                
                NLM_LOG_DEBUG("AgentBrain: Applied delta " + std::to_string(delta) +
                             " to synapse, eligibility: " + std::to_string(eligibility));
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = PLASTICITY_BASE_FACTOR + PLASTICITY_SENSITIVITY * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(STDP_LTP_WEIGHT_BASE * plasticityFactor);
        stdp->setLTDWeight(STDP_LTD_WEIGHT_BASE * plasticityFactor);
        
        NLM_LOG_DEBUG("AgentBrain: Adjusted STDP weights - LTP: " + std::to_string(STDP_LTP_WEIGHT_BASE * plasticityFactor) +
                     ", LTD: " + std::to_string(STDP_LTD_WEIGHT_BASE * plasticityFactor));
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_DEBUG("AgentBrain: Development disabled or null brain");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < DEVELOPMENT_STAGE_1_END) {  // ~1 minute
        plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        NLM_LOG_DEBUG("AgentBrain: Development - Initial stage (age " + std::to_string(developmentalAge_) + ")");
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_2_END) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        NLM_LOG_DEBUG("AgentBrain: Development - Critical period (age " + std::to_string(developmentalAge_) + ")");
    } else if (developmentalAge_ < DEVELOPMENT_STAGE_3_END) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        NLM_LOG_DEBUG("AgentBrain: Development - Maturation (age " + std::to_string(developmentalAge_) + ")");
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        NLM_LOG_DEBUG("AgentBrain: Development - Adult stage (age " + std::to_string(developmentalAge_) + ")");
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
            
            NLM_LOG_DEBUG("AgentBrain: Adjusted structural plasticity - synaptogenesis: " + std::to_string(synRate) +
                         ", pruning: " + std::to_string(pruneRate));
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
    dopamineLevel_ = DEFAULT_DOPAMINE_LEVEL;
    noveltyLevel_ = DEFAULT_NOVELTY_LEVEL;
    curiosityLevel_ = DEFAULT_CURIOSITY_LEVEL;
    predictionError_ = DEFAULT_PREDICTION_ERROR;
    expectedReward_ = DEFAULT_EXPECTED_REWARD;
    developmentalAge_ = DEFAULT_DEVELOPMENTAL_AGE;
    plasticityModifier_ = DEFAULT_PLASTICITY_MODIFIER;
    
    // Clear previous vision - this prevents unbounded growth
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset - all state cleared");
}

} // namespace nlm