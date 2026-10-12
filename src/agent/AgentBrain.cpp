#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// AgentBrain configuration constants
namespace AgentBrainConfig {
    // Default sensory input sizes
    constexpr size_t DEFAULT_VISION_INPUT_SIZE = 256;      // 16x16 vision grid
    constexpr size_t DEFAULT_TOUCH_INPUT_SIZE = 8;         // Touch sensors
    constexpr size_t DEFAULT_INTERNAL_INPUT_SIZE = 4;      // Internal signals
    constexpr size_t DEFAULT_PROPRIOCEPTION_INPUT_SIZE = 6; // Proprioception sensors
    
    // Scaling factors for sensory transduction
    constexpr float DEFAULT_VISION_SCALE = 5.0f;
    constexpr float DEFAULT_TOUCH_SCALE = 8.0f;
    constexpr float DEFAULT_INTERNAL_SCALE = 2.0f;
    constexpr float DEFAULT_PROPRIOCEPTION_SCALE = 3.0f;
    
    // Novelty and curiosity thresholds
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float DEFAULT_MAX_EXPLORATION_THRESHOLD = 0.5f;
    constexpr float DEFAULT_EXPLORATION_CHANCE = 0.3f;
    
    // Neuromodulation parameters
    constexpr float DEFAULT_REWARD_DECAY = 0.1f;
    constexpr float DEFAULT_REWARD_WEIGHT = 0.95f;
    constexpr float DEFAULT_REWARD_ADJUSTMENT = 0.05f;
    constexpr float DEFAULT_ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float DEFAULT_ELIGIBILITY_DECAY = 0.1f;
    
    // Plasticity parameters
    constexpr float DEFAULT_BASE_PLASTICITY_FACTOR = 0.5f;
    constexpr float DEFAULT_MAX_PLASTICITY_FACTOR = 2.0f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    
    // Development parameters
    constexpr double DEFAULT_INITIAL_DEVELOPMENT_STAGE = 60.0;
    constexpr double DEFAULT_CRITICAL_PERIOD = 300.0;
    constexpr double DEFAULT_MATURATION = 900.0;
    constexpr float DEFAULT_ADULT_PLASTICITY = 0.2f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Motor command constants
    constexpr size_t DEFAULT_MOTOR_COMMAND_COUNT = 6;
    constexpr float DEFAULT_MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // General thresholds
    constexpr float DEFAULT_CURIOUSITY_CLAMP_MIN = 0.0f;
    constexpr float DEFAULT_CURIOUSITY_CLAMP_MAX = 1.0f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DEFAULT_DOPAMINE_CLAMP_MAX = 1.0f;
    
    // Motor command selections
    constexpr int MAX_RANDOM_MOTOR_COMMAND = 7;
}

// Helper function to validate configuration
inline void validateConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::invalid_argument("Configuration pointer is null");
    }
}

// Helper function to validate sensory input ranges
inline bool validateSensoryInputRange(float value, float min = -1.0f, float max = 1.0f) {
    return value >= min && value <= max;
}

// Helper function to validate developmental age
inline bool validateDevelopmentalAge(double age) {
    return age >= 0.0;
}

// Helper function to validate plasticity factor
inline bool validatePlasticityFactor(float factor) {
    return factor >= 0.1f && factor <= 2.0f;
}

// Helper function to validate reward values
inline bool validateReward(float reward) {
    return !std::isnan(reward) && std::isfinite(reward);
}

// Helper function to validate timestep
inline bool validateTimestep(double timestep) {
    return timestep > 0.0 && timestep < 1.0;
}

namespace nlm {

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
    , sensoryNoveltyDecay_(AgentBrainConfig::DEFAULT_NOVELTY_DECAY)
{
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Brain pointer is null - brain not initialized");
        return;
    }
    
    // Initialize motor and sensory neuron groups
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
    
    // Validate that we have the expected number of sensory neurons
    if (sensoryVision_.size() != AgentBrainConfig::DEFAULT_VISION_INPUT_SIZE) {
        NLM_LOG_WARNING("AgentBrain: Vision sensory neurons count mismatch. Expected: " + 
                        std::to_string(AgentBrainConfig::DEFAULT_VISION_INPUT_SIZE) + 
                        ", Found: " + std::to_string(sensoryVision_.size()));
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize: Brain pointer is null");
        return;
    }
    
    // Validate world dimensions
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    
    if (visionWidth == 0 || visionHeight == 0) {
        NLM_LOG_ERROR("AgentBrain::initialize: Invalid world dimensions");
        return;
    }
    
    // Resize vision buffer based on world dimensions
    size_t expectedVisionSize = visionWidth * visionHeight;
    previousVision_.resize(expectedVisionSize, 0.0f);
    
    // Initialize development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return AgentBrainConfig::DEFAULT_VISION_INPUT_SIZE +
           AgentBrainConfig::DEFAULT_TOUCH_INPUT_SIZE +
           AgentBrainConfig::DEFAULT_INTERNAL_INPUT_SIZE +
           AgentBrainConfig::DEFAULT_PROPRIOCEPTION_INPUT_SIZE;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentBrainConfig::DEFAULT_MOTOR_COMMAND_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Brain pointer is null");
        return;
    }
    
    // Validate percept exists
    if (!percept.getVision().data() && percept.getVision().empty()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid vision data in percept");
        return;
    }
    
    const auto& vision = percept.getVision();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& visionData = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < visionData.size(); ++i) {
        if (sensoryVision_[i]) {
            // Validate input value
            float visionValue = visionData[i];
            if (!validateSensoryInputRange(visionValue)) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid vision value at index " + 
                               std::to_string(i) + ": " + std::to_string(visionValue));
                visionValue = std::clamp(visionValue, -1.0f, 1.0f);
            }
            
            // Inject current proportional to vision intensity
            float current = visionValue * AgentBrainConfig::DEFAULT_VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float touchValue = touch[i];
            if (!validateSensoryInputRange(touchValue)) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid touch value at index " + 
                               std::to_string(i) + ": " + std::to_string(touchValue));
                touchValue = std::clamp(touchValue, -1.0f, 1.0f);
            }
            
            float current = touchValue * AgentBrainConfig::DEFAULT_TOUCH_SCALE;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float internalValue = intern[i];
            if (!validateSensoryInputRange(internalValue)) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid internal value at index " + 
                               std::to_string(i) + ": " + std::to_string(internalValue));
                internalValue = std::clamp(internalValue, -1.0f, 1.0f);
            }
            
            // Center and scale
            float current = (internalValue * AgentBrainConfig::DEFAULT_INTERNAL_SCALE - 1.0f) * 
                           AgentBrainConfig::DEFAULT_VISION_SCALE;  // Use vision scale as base
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float proprioValue = proprio[i];
            if (!validateSensoryInputRange(proprioValue)) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid proprioception value at index " + 
                               std::to_string(i) + ": " + std::to_string(proprioValue));
                proprioValue = std::clamp(proprioValue, -1.0f, 1.0f);
            }
            
            // Center and scale
            float current = (proprioValue * AgentBrainConfig::DEFAULT_PROPRIOCEPTION_SCALE - 1.0f) * 
                           AgentBrainConfig::DEFAULT_PROPRIOCEPTION_SCALE;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        // Use stored vision data from percept for novelty detection
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * AgentBrainConfig::DEFAULT_MAX_PLASTICITY_FACTOR + 
                         std::abs(predictionError_) * AgentBrainConfig::DEFAULT_MAX_EXPLORATION_THRESHOLD;
        curiosityLevel_ = std::clamp(curiosityLevel_, 
                                     AgentBrainConfig::DEFAULT_CURIOUSTY_CLAMP_MIN,
                                     AgentBrainConfig::DEFAULT_CURIOUSTY_CLAMP_MAX);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Brain pointer is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConfig::DEFAULT_EXPLORATION_THRESHOLD) {
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
    if (bestActivity < AgentBrainConfig::DEFAULT_MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::selectWithCuriosity: Brain pointer is null");
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConfig::DEFAULT_MAX_EXPLORATION_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConfig::DEFAULT_EXPLORATION_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, AgentBrainConfig::MAX_RANDOM_MOTOR_COMMAND);
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
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Brain pointer is null or reward modulation disabled");
        return;
    }
    
    // Validate reward values
    if (!validateReward(reward) || !validateReward(predictedReward)) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Invalid reward values");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = AgentBrainConfig::DEFAULT_REWARD_WEIGHT * expectedReward_ +
                     AgentBrainConfig::DEFAULT_REWARD_ADJUSTMENT * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                                AgentBrainConfig::DEFAULT_DOPAMINE_CLAMP_MIN,
                                AgentBrainConfig::DEFAULT_DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConfig::DEFAULT_ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConfig::DEFAULT_ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConfig::DEFAULT_BASE_PLASTICITY_FACTOR +
                            AgentBrainConfig::DEFAULT_BASE_PLASTICITY_FACTOR * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 
                                  AgentBrainConfig::DEFAULT_DOPAMINE_CLAMP_MIN,
                                  AgentBrainConfig::DEFAULT_MAX_PLASTICITY_FACTOR);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(AgentBrainConfig::DEFAULT_STDP_LTP_WEIGHT * plasticityFactor);
        stdp->setLTDWeight(AgentBrainConfig::DEFAULT_STDP_LTD_WEIGHT * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Brain pointer is null or development disabled");
        return;
    }
    
    // Validate timestep
    if (!validateTimestep(timestep)) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Invalid timestep");
        return;
    }
    
    // Validate developmental age before update
    if (!validateDevelopmentalAge(developmentalAge_)) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Invalid developmental age");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < AgentBrainConfig::DEFAULT_INITIAL_DEVELOPMENT_STAGE) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConfig::DEFAULT_CRITICAL_PERIOD) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConfig::DEFAULT_MATURATION) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConfig::DEFAULT_ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Validate plasticity factor after update
    if (!validatePlasticityFactor(plasticityModifier_)) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment: Invalid plasticity factor after update");
        return;
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConfig::DEFAULT_SYNAPTOGENESIS_RATE * plasticityModifier_;
            float pruneRate = AgentBrainConfig::DEFAULT_PRUNING_RATE * 
                             (AgentBrainConfig::DEFAULT_MAX_PLASTICITY_FACTOR - plasticityModifier_);
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
    
    NLM_LOG_INFO("AgentBrain reset complete");
}

} // namespace nlm
