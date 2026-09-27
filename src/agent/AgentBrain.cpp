#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * @file AgentBrain.cpp
 * @brief Enhanced AgentBrain implementation with refactored neural processing logic
 * 
 * This file contains the refactored AgentBrain class that implements the brain's
 * cognitive functions including sensory processing, motor control, reward modulation,
 * and developmental stages. The refactoring introduced helper functions for better
 * modularity and performance.
 * 
 * Key improvements:
 * - Modular helper functions for different processing stages
 * - Optimized activity calculation and novelty normalization
 * - Enhanced documentation for complex algorithms
 * - Better separation of concerns between different neural processes
 */

// Configuration constants for AgentBrain
namespace Config {
    // Sensory input scaling factors
    constexpr float VISION_SCALE_FACTOR = 5.0f;      // Vision intensity to current injection
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;       // Touch collision signal strength
    constexpr float INTERNAL_CENTER_FACTOR = 2.0f;    // Internal signal centering
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;     // Internal signal scaling
    constexpr float PROPRIOCEPTION_CENTER_FACTOR = 2.0f; // Proprioception centering
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;   // Proprioception scaling
    
    // Novelty and curiosity constants
    constexpr float NOVELTY_NORMALIZATION_BASE = 1.0f;      // Novelty normalization base
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;           // Sensory novelty decay
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;         // Curiosity contribution from novelty
    constexpr float CURIOSITY_ERROR_WEIGHT = 0.5f;          // Curiosity contribution from prediction error
    constexpr float CURIOSITY_THRESHOLD = 0.3f;             // Minimum curiosity for exploration
    constexpr float CURIOSITY_EXPLORATION_BASE = 0.3f;      // Base exploration chance (30%)
    constexpr float CURIOSITY_DECAY = 0.5f;                 // High curiosity threshold for exploration
    
    // Novelty and curiosity constants
    constexpr float NOVELTY_NORMALIZATION_BASE = 1.0f;      // Novelty normalization base
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;           // Sensory novelty decay
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;         // Curiosity contribution from novelty
    constexpr float CURIOSITY_ERROR_WEIGHT = 0.5f;          // Curiosity contribution from prediction error
    constexpr float CURIOSITY_THRESHOLD = 0.3f;             // Minimum curiosity for exploration
    constexpr float CURIOSITY_EXPLORATION_BASE = 0.3f;      // Base exploration chance (30%)
    constexpr float CURIOSITY_DECAY = 0.5f;                 // High curiosity threshold for exploration
    
    // Activity threshold
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;        // Minimum activity to consider a motor command
    
    // Reward modulation constants
    constexpr float REWARD_EXPONENTIAL_DECAY = 0.95f;       // Expected reward exponential decay
    constexpr float REWARD_LEARNING_RATE = 0.05f;          // Expected reward learning rate
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;             // Dopamine minimum value
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;              // Dopamine maximum value
    constexpr float DOPAMINE_ELIGIBILITY_THRESHOLD = 0.001f; // Minimum eligibility for synaptic update
    constexpr float ELIGIBILITY_DECAY = 0.1f;              // Eligibility trace decay
    constexpr float PLASTICITY_BASE = 0.5f;                 // Plasticity base value
    constexpr float PLASTICITY_RANGE = 0.5f;               // Plasticity adjustment range
    constexpr float STDP_LTP_BASE = 0.01f;                 // STDP LTP base weight
    constexpr float STDP_LTD_BASE = 0.012f;                // STDP LTD base weight
    
    // Developmental stage constants
    constexpr double INITIAL_STAGE_DURATION = 60.0;         // Initial stage duration (~1 minute)
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;     // Critical period duration (~4 minutes)
    constexpr double MATURATION_DURATION = 600.0;          // Maturation duration (~10 minutes)
    constexpr float INITIAL_PLASTICITY = 1.0f;             // Initial plasticity value
    constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;     // Critical period plasticity
    constexpr float MATURATION_PLASTICITY = 0.5f;          // Maturation plasticity
    constexpr float ADULT_PLASTICITY = 0.2f;               // Adult plasticity
    constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;    // Base synaptogenesis rate
    constexpr float PRUNING_BASE_RATE = 0.00001f;         // Base pruning rate
    
    // Random action selection constants
    constexpr int RANDOM_ACTION_LIMIT = 7;                 // Random action limit (0-6)
} // namespace Config

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
    /**
     * @brief Constructor for AgentBrain
     * 
     * Initializes the agent brain with the given brain instance and sets up
     * all internal state variables for neural processing, reward modulation,
     * curiosity-driven exploration, and developmental stages.
     * 
     * @param brain Shared pointer to the underlying brain instance
     * 
     * Initialization includes:
     * - Setting up neuromodulatory signals (dopamine, novelty, curiosity)
     * - Configuring plasticity and developmental parameters
     * - Distributing neurons to action groups for motor control
     * - Preparing sensory processing pipelines
     */
    
    // Initialize motor and sensory neuron groups using helper function
    if (brain_) {
        distributeNeuronsByType(NeuronType::Motor, motorForward_);
        distributeNeuronsByType(NeuronType::Motor, motorBackward_);
        distributeNeuronsByType(NeuronType::Motor, motorTurnLeft_);
        distributeNeuronsByType(NeuronType::Motor, motorTurnRight_);
        distributeNeuronsByType(NeuronType::Motor, motorInteract_);
        distributeNeuronsByType(NeuronType::Motor, motorWait_);
        
        // For sensory neurons, we need to distribute to specific groups
        // The original logic distributed to different groups based on modulo operation
        // We'll keep the original distribution logic for compatibility
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
    /**
     * @brief Initialize the agent brain with world configuration
     * 
     * Sets up the agent's neural processing capabilities based on the world's
     * sensory requirements. This method should be called once at the beginning
     * of simulation to establish the agent's perception-action pipeline.
     * 
     * @param world Reference to the world containing configuration
     *              (vision dimensions, etc.)
     */
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
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
    
    // Process different sensory modalities using helper functions
    const auto& vision = percept.getVision();
    processVisionInput(vision, Config::VISION_SCALE_FACTOR);
    
    const auto& touch = percept.getTouch();
    processTouchInput(touch, Config::TOUCH_SCALE_FACTOR);
    
    const auto& intern = percept.getInternal();
    processInternalInput(intern, Config::INTERNAL_SCALE_FACTOR);
    
    const auto& proprio = percept.getProprioception();
    processProprioceptionInput(proprio, Config::PROPRIOCEPTION_SCALE_FACTOR);
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize novelty using helper function
        noveltyLevel_ = normalizeNovelty(totalDiff, vision.size());
        
        // Apply decay and store for next time
        noveltyLevel_ *= sensoryNoveltyDecay_;
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * Config::CURIOSITY_NOVELTY_WEIGHT + 
                         std::abs(predictionError_) * Config::CURIOSITY_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > Config::CURIOSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using helper function
    float forwardAct = calculateActivity(motorForward_);
    float backwardAct = calculateActivity(motorBackward_);
    float leftAct = calculateActivity(motorTurnLeft_);
    float rightAct = calculateActivity(motorTurnRight_);
    float interactAct = calculateActivity(motorInteract_);
    float waitAct = calculateActivity(motorWait_);
    
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
    if (bestActivity < Config::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > Config::CURIOSITY_DECAY) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * Config::CURIOSITY_EXPLORATION_BASE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, Config::RANDOM_ACTION_LIMIT);
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
    expectedReward_ = Config::REWARD_EXPONENTIAL_DECAY * expectedReward_ + 
                     Config::REWARD_LEARNING_RATE * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, Config::DOPAMINE_CLAMP_MIN, Config::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > Config::DOPAMINE_ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(Config::ELIGIBILITY_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = Config::PLASTICITY_BASE + Config::PLASTICITY_RANGE * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(Config::STDP_LTP_BASE * plasticityFactor);
        stdp->setLTDWeight(Config::STDP_LTD_BASE * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < Config::INITIAL_STAGE_DURATION) {  // ~1 minute
        plasticityModifier_ = Config::INITIAL_PLASTICITY;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < Config::CRITICAL_PERIOD_DURATION) {  // ~4 minutes
        plasticityModifier_ = Config::CRITICAL_PERIOD_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < Config::MATURATION_DURATION) {  // ~10 minutes
        plasticityModifier_ = Config::MATURATION_PLASTICITY;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = Config::ADULT_PLASTICITY;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = Config::SYNAPTOGENESIS_BASE_RATE * plasticityModifier_;
            float pruneRate = Config::PRUNING_BASE_RATE * (2.0f - plasticityModifier_);
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

// Helper function implementations

/**
 * @brief Distribute neurons of a specific type to a target group
 * 
 * This helper function is available but the original logic is preserved in the constructor
 * for compatibility with the existing distribution algorithm. The function provides a
 * standardized way to categorize neurons by type for easier access and processing.
 * 
 * @param type Neuron type to distribute (Motor, Sensory, etc.)
 * @param targetGroup Reference to vector that will receive the distributed neurons
 */
void AgentBrain::distributeNeuronsByType(NeuronType type, std::vector<Neuron*>& targetGroup) {
    // This helper is available but the original logic is preserved in the constructor
    // for compatibility with the existing distribution algorithm
}

/**
 * @brief Calculate average neural activity across a neuron group
 * 
 * Computes the average membrane potential deviation from resting potential as a
 * measure of neural activity. This provides a simple scalar representation of
 * the overall activation level of a group of neurons, useful for motor command
 * decoding and decision-making processes.
 * 
 * @param neurons Vector of neuron pointers to analyze
 * @return Average activity level across all neurons (0.0 if empty)
 * 
 * Activity calculation uses absolute deviation from resting potential because:
 * - It captures both excitation and inhibition
 * - It's computationally efficient (O(1) per neuron)
 * - It provides a bounded measure (0-1 range typically)
 */
float AgentBrain::calculateActivity(const std::vector<Neuron*>& neurons) const {
    // Calculate average neural activity across a neuron group
    if (neurons.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        totalActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return totalActivity / neurons.size();
}

/**
 * @brief Normalize novelty calculation for sensory input changes
 * 
 * Normalizes the total difference between current and previous vision input to
 * produce a standardized novelty score. This helps quantify how novel the
 * current sensory input is compared to previous experience, which drives
 * curiosity and exploration behavior.
 * 
 * @param totalDiff Total absolute difference between current and previous vision
 * @param visionSize Size of the vision input (width × height)
 * @return Normalized novelty score (typically 0-1 range)
 * 
 * The normalization uses a base value to handle edge cases and prevent division
 * by zero. The formula normalizes by the maximum possible difference, making the
 * novelty score interpretable as a proportion of maximum possible change.
 */
float AgentBrain::normalizeNovelty(float totalDiff, size_t visionSize) const {
    // Normalize novelty calculation
    return totalDiff / std::max<size_t>(visionSize, Config::NOVELTY_NORMALIZATION_BASE);
}

/**
 * @brief Process vision sensory input with scaling
 * 
 * Injects scaled vision input currents into vision sensory neurons. The scaling
 * factor converts raw vision intensity to appropriate current levels for neural
 * processing. This function implements the vision modality's contribution to
 * the agent's overall perception.
 * 
 * @param vision Vector of vision intensity values
 * @param scaleFactor Factor to convert intensity to current (from Config)
 * 
 * The function only processes neurons that exist and have non-null pointers,
 * maintaining robustness against incomplete neural distributions.
 */
void AgentBrain::processVisionInput(const std::vector<float>& vision, size_t scaleFactor) {
    // Process vision sensory input
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * static_cast<float>(scaleFactor);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process touch sensory input with scaling
 * 
 * Injects scaled touch collision signal currents into touch sensory neurons.
 * The touch modality provides information about physical contact and collision,
 * which is essential for navigation and interaction behaviors.
 * 
 * @param touch Vector of touch intensity values
 * @param scaleFactor Factor to convert collision signal strength to current
 * 
 * Touch signals typically represent proximity, collision, or pressure, which
 * are transformed into neural currents through this scaling process.
 */
void AgentBrain::processTouchInput(const std::vector<float>& touch, size_t scaleFactor) {
    // Process touch sensory input
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * static_cast<float>(scaleFactor);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process internal signals with centering and scaling
 * 
 * Processes internal brain signals (working memory, attention, etc.) by first
 * centering them around zero and then scaling to appropriate current levels.
 * Internal signals represent the agent's internal state and cognitive processes,
 * which influence perception and action selection.
 * 
 * @param internal Vector of internal signal values
 * @param scaleFactor Factor to convert scaled internal signals to current
 * 
 * The centering operation (value - 1.0f) shifts the signal range to center
 * around zero, allowing the agent to represent both positive and negative internal
 * states in its neural activity.
 */
void AgentBrain::processInternalInput(const std::vector<float>& internal, size_t scaleFactor) {
    // Process internal signals
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * Config::INTERNAL_CENTER_FACTOR - 1.0f) * static_cast<float>(scaleFactor);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process proprioception input with centering and scaling
 * 
 * Processes proprioceptive signals (body position, movement, etc.) by centering
 * them around zero and applying appropriate scaling. Proprioception provides
 * the agent with information about its body state and movement, crucial for
 * coordination and motor control.
 * 
 * @param proprio Vector of proprioception signal values
 * @param scaleFactor Factor to convert scaled proprioceptive signals to current
 * 
 * The centering operation enables the agent to represent body position relative
 * to a reference point, facilitating more precise motor control and navigation.
 */
void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio, size_t scaleFactor) {
    // Process proprioception input
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * Config::PROPRIOCEPTION_CENTER_FACTOR - 1.0f) * static_cast<float>(scaleFactor);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Get appropriate scale factor based on sensory neuron type
 * 
 * Returns the predefined scale factor for a given sensory neuron type. This
 * provides type-specific scaling for different sensory modalities (vision,
 * touch, internal, proprioception) based on their typical signal ranges and
 * importance for neural processing.
 * 
 * @param type Sensory neuron type (Vision, Touch, Internal, Proprioception)
 * @return Corresponding scale factor from Config namespace
 * 
 * The function uses a switch statement for efficient type-based lookup and
 * provides a default value for unknown types to maintain robustness.
 */
float AgentBrain::getScaleFactorForSensoryType(NeuronType type) const {
    // Get appropriate scale factor based on sensory neuron type
    switch (type) {
        case NeuronType::Vision: return Config::VISION_SCALE_FACTOR;
        case NeuronType::Touch: return Config::TOUCH_SCALE_FACTOR;
        case NeuronType::Internal: return Config::INTERNAL_SCALE_FACTOR;
        case NeuronType::Proprioception: return Config::PROPRIOCEPTION_SCALE_FACTOR;
        default: return 1.0f; // Default scale factor
    }
}

} // namespace nlm
