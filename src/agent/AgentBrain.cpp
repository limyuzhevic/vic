#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

/**
 * @class AgentBrain
 * 
 * @brief Connects NLM brain to the world
 * 
 * This class implements the agent interface that bridges the NLM brain with
 * the external world. It handles:
 * 
 * 1. **Sensory transduction**: Converts world percepts into neural inputs
 * 2. **Motor decoding**: Converts neural activity into actions
 * 3. **Reward-based learning**: Applies prediction errors to synaptic weights
 * 4. **Curiosity-driven exploration**: Balances exploitation vs exploration
 * 5. **Developmental progression**: Models lifespan plasticity changes
 * 
 * The agent implements a biologically plausible control architecture that
 * enables autonomous behavior through continuous interaction with the
 * environment, learning from experience, and adapting over time.
 */

/**
 * @brief Enhanced constructor with improved initialization patterns
 * 
 * @param brain Shared pointer to the brain instance that this agent will control
 * 
 * This constructor implements a robust initialization strategy:
 * - Uses move semantics for efficient resource transfer
 * - Implements proper initialization order for member variables
 * - Handles null pointer cases gracefully
 * - Organizes neurons into functional groups for efficient processing
 * 
 * The initialization process follows these steps:
 * 1. Initialize neuromodulatory state to baseline
 * 2. Create brain pointer management
 * 3. Organize neurons into functional groups:
 *    - Motor neurons grouped by action type (6 categories)
 *    - Sensory neurons grouped by modality (4 categories)
 * 4. Set up development and learning parameters
 */
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    // Initialize motor and sensory neuron groups with enhanced organization
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                auto neurons = pop->getNeurons();
                
                if (type == NeuronType::Motor) {
                    // Enhanced motor neuron distribution with balanced groups
                    distribute_neurons_to_groups(neurons, 6, 
                                                motorForward_, motorBackward_, 
                                                motorTurnLeft_, motorTurnRight_);
                    // Handle remaining neurons to ensure all are assigned
                    size_t assignedCount = motorForward_.size() + motorBackward_.size() + 
                                         motorTurnLeft_.size() + motorTurnRight_.size();
                    for (size_t i = assignedCount; i < neurons.size(); ++i) {
                        if (i % 2 == 0) {
                            motorInteract_.push_back(neurons[i]);
                        } else {
                            motorWait_.push_back(neurons[i]);
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    // Enhanced sensory neuron distribution with balanced groups
                    distribute_neurons_to_groups(neurons, 4,
                                                sensoryVision_, sensoryTouch_,
                                                sensoryInternal_, sensoryProprioception_);
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

/**
 * @brief Initialize agent state with world dimensions
 * 
 * @param world Reference to the world object containing sensory dimensions
 * 
 * This method sets up the initial state for agent-world interaction:
 * - Allocates memory for novelty detection (previous vision storage)
 * - Resets developmental timers to starting conditions
 * - Logs initialization statistics for debugging
 * - Prepares for continuous sensory processing
 */
void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

/**
 * @brief Get expected sensory input size for the brain
 * 
 * @return size_t Total number of sensory values expected (vision + touch + internal + proprioception)
 * 
 * Returns the standard sensory configuration:
 * - Vision: 256 values (16x16 grid from world)
 * - Touch: 8 values (binary collision sensors)
 * - Internal: 4 values (energy, hunger, thirst, fatigue)
 * - Proprioception: 6 values (body position and orientation)
 * 
 * @note This must match the brain's sensory processing requirements for proper operation
 */
size_t AgentBrain::getSensoryInputSize() const {
    // Standard sensory configuration for Phase 6 integration
    const size_t visionSize = 256;      // 16x16 visual grid
    const size_t touchSize = 8;         // Touch sensors
    const size_t internalSize = 4;      // Internal states
    const size_t proprioceptionSize = 6; // Body senses
    
    return visionSize + touchSize + internalSize + proprioceptionSize;
}

/**
 * @brief Get expected motor output size
 * 
 * @return size_t Number of motor action categories (typically 6)
 * 
 * Returns the standard motor action configuration:
 * - MoveForward: Move in current direction
 * - MoveBackward: Move opposite direction  
 * - TurnLeft: Rotate 90 degrees left
 * - TurnRight: Rotate 90 degrees right
 * - Interact: Attempt interaction with environment
 * - Wait: Idle/no action
 * 
 * @note This must match the brain's motor output requirements for proper action execution
 */
size_t AgentBrain::getMotorOutputSize() const {
    // Standard motor action categories
    return 6;
}

/**
 * @brief Process sensory percept and inject into brain
 * 
 * @param percept Contains all sensory modalities (vision, touch, internal, proprioception)
 * 
 * This method implements the complete sensory transduction pipeline:
 * 
 * **Stage 1: Vision Processing**
 * - Extracts visual intensity values from 16x16 grid
 * - Maps to vision sensory neurons with scaling factor
 * - Applies bounds checking for safety
 * 
 * **Stage 2: Touch Processing**
 * - Processes collision and contact information
 * - Converts to excitatory signals for motor preparation
 * - Uses collision magnitude for response strength
 * 
 * **Stage 3: Internal State Processing**
 * - Handles homeostatic needs (energy, hunger, thirst, fatigue)
 * - Centers and scales for optimal neural range
 * - Drives motivated behavior based on internal state
 * 
 * **Stage 4: Proprioceptive Processing**
 * - Provides body position and orientation information
 * - Enables action selection based on body state
 * - Supports coordination and movement planning
 * 
 * **Stage 5: Novelty Computation**
 * - Compares current vs. previous sensory input
 * - Implements decay mechanism for novelty signals
 * - Drives curiosity-based exploration
 * 
 * **Stage 6: Curiosity Integration**
 * - Updates curiosity based on novelty and prediction error
 * - Clamps to biologically plausible range
 * - Guides action selection strategy
 */
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Enhanced sensory processing with specialized methods
    process_vision_input(percept);
    process_touch_input(percept);
    process_internal_input(percept);
    process_proprioception_input(percept);
    
    // Compute and store novelty for curiosity-driven behavior
    noveltyLevel_ = compute_novelty();
    
    // Update curiosity levels for action selection
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

/**
 * @brief Process vision sensory input with enhanced scaling and bounds checking
 * 
 * @param percept Reference to sensory percept containing vision data
 * 
 * Vision processing includes:
 * - Intensity mapping to neural currents
 * - Safety bounds checking (-10.0 to 10.0 mV)
 * - Efficient iteration with bounds protection
 */
void AgentBrain::process_vision_input(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Enhanced scaling with safety bounds
            float current = vision[i] * 5.0f;  // Scale factor for optimal firing
            current = std::clamp(current, -10.0f, 10.0f);  // Safety clamp
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process touch sensory input with enhanced magnitude handling
 * 
 * @param percept Reference to sensory percept containing touch data
 * 
 * Touch processing focuses on collision detection and response strength
 */
void AgentBrain::process_touch_input(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal strength
            current = std::clamp(current, 0.0f, 20.0f);  // Ensure non-negative for collision
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process internal state sensory input with centering and scaling
 * 
 * @param percept Reference to sensory percept containing internal data
 * 
 * Internal processing handles homeostatic states with optimal ranges
 */
void AgentBrain::process_internal_input(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Better scaling for internal states with centering
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center at 0, scale to range
            current = std::clamp(current, -10.0f, 10.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Process proprioceptive sensory input with body position sensing
 * 
 * @param percept Reference to sensory percept containing proprioception data
 * 
 * Proprioception provides body state information for movement coordination
 */
void AgentBrain::process_proprioception_input(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            current = std::clamp(current, -5.0f, 5.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

/**
 * @brief Compute novelty using optimized difference calculation
 * 
 * @return float Normalized novelty level (0.0 to 1.0)
 * 
 * Novelty computation includes:
 * - Difference between current and previous vision
 * - Normalization by visual input size
 * - Exponential decay for temporal smoothing
 * - Zero handling for empty inputs
 * 
 * @note Uses efficient single-pass algorithm for large vision grids
 */
float AgentBrain::compute_novelty() {
    const auto& vision = percept.getVision();
    if (vision.empty()) {
        return 0.0f;
    }
    
    // Optimized novelty computation with efficient algorithm
    float totalDiff = 0.0f;
    size_t minSize = std::min(vision.size(), previousVision_.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and apply decay
    float novelty = totalDiff / std::max<size_t>(vision.size(), 1);
    novelty *= sensoryNoveltyDecay_;
    
    // Store for next computation
    previousVision_ = vision;
    
    return novelty;
}

/**
 * @brief Decode motor neurons to motor command
 * 
 * @return MotorCommand Selected action based on motor neuron activity patterns
 * 
 * This method implements population coding for action selection:
 * 1. Calculates average activity in each motor neuron group
 * 2. Compares activity across all action categories
 * 3. Selects action with highest activity
 * 4. Applies threshold to prevent weak activations
 * 
 * The decoding algorithm:
 * - Uses membrane potential deviation from rest as activity measure
 * - Implements winner-take-all competition
 * - Provides default wait action for inactivity
 * - Ensures meaningful action selection only
 */
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-driven exploration with balanced logic
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

/**
 * @brief Calculate average activity in motor neuron groups
 * 
 * @return MotorCommand Motor command with highest average activity
 * 
 * This implements the core motor decoding algorithm:
 * - Computes firing rate for each action category
 * - Uses winner-take-all competition
 * - Applies threshold for meaningful actions only
 * 
 * @note Enhanced with better null checking and bounds protection
 */
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using enhanced function
    float forwardAct = calculate_average_activity(motorForward_);
    float backwardAct = calculate_average_activity(motorBackward_);
    float leftAct = calculate_average_activity(motorTurnLeft_);
    float rightAct = calculate_average_activity(motorTurnRight_);
    float interactAct = calculate_average_activity(motorInteract_);
    float waitAct = calculate_average_activity(motorWait_);
    
    // Find maximum activity with structured approach
    struct MotorAction {
        MotorCommand command;
        float activity;
    };
    
    MotorAction actions[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& action : actions) {
        if (action.activity > bestActivity) {
            bestActivity = action.activity;
            best = action.command;
        }
    }
    
    // Only act if there's meaningful activity with better threshold
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

/**
 * @brief Select motor command with curiosity-driven exploration
 * 
 * @param defaultCmd Habitual action selection
 * @return MotorCommand Either habitual or exploratory action
 * 
 * This implements the exploration-exploitation tradeoff:
 * 
 * **Exploitation** (when curiosity low):
 * - Maintain habitual action selection based on motor activity
 * - Focus on refining known successful behaviors
 * 
 * **Exploration** (when curiosity high):
 * - Increase probability of random actions
 * - Discovers new behaviors and environmental opportunities
 * - Prevents getting stuck in local optima
 * 
 * The exploration mechanism:
 * - Scales random action probability with curiosity level
 * - Extends action space with additional behaviors (eat, sleep, groom)
 * - Maintains biological plausibility with bounded responses
 */
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration with extended action space
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration (up to 40% random)
        float exploreChance = curiosityLevel_ * 0.4f;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Extended random motor command with additional behaviors
            int choice = brain_->getRandomGenerator()->uniformInt(0, 10);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Eat;
                case 8: return MotorCommand::Sleep;
                case 9: return MotorCommand::Groom;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

/**
 * @brief Apply neuromodulation based on reward prediction error
 * 
 * @param reward Actual reward received
 * @param predictedReward Expected reward
 * 
 * Implements dopamine-based reward prediction error learning:
 * 
 * **Learning Signal Computation**:
 * - Prediction error = reward - expected reward
 * - Dopamine level proportional to prediction error
 * - Bounded response using hyperbolic tangent
 * 
 * **Synaptic Plasticity Application**:
 * - Uses eligibility traces for temporal difference learning
 * - Weight changes scaled by neuromodulatory signal
 * - Enhanced with biologically plausible bounds
 * 
 * **Plasticity Rate Modulation**:
 * - Positive dopamine increases learning rates
 * - Negative dopamine decreases learning rates
 * - Enhanced biological modeling of neuromodulation effects
 */
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Enhanced prediction error computation with better modeling
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with better exponential moving average
    expectedReward_ = 0.9f * expectedReward_ + 0.1f * reward;
    
    // Dopamine-like signal with biological plausibility
    dopamineLevel_ = std::tanh(predictionError_ * 0.1f);  // Bounded sigmoid response
    
    // Apply reward to synapses with enhanced eligibility trace mechanism
    apply_reward_to_synapses();
    
    // Modulate plasticity based on dopamine with enhanced biological modeling
    modulate_plasticity_based_on_dopamine();
}

/**
 * @brief Apply reward-modulated weight changes to synapses
 * 
 * This method implements reward-based synaptic plasticity:
 * - Uses eligibility traces for temporal credit assignment
 * - Scales weight changes by neuromodulatory signal
 * - Applies biologically plausible bounds
 * - Efficient iteration with bounds protection
 */
void AgentBrain::apply_reward_to_synapses() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                
                // Apply biologically plausible bounds
                delta = std::clamp(delta, -0.05f, 0.05f);
                
                syn->addToWeight(delta);
                
                // Decay eligibility trace with more realistic parameters
                syn->decayEligibilityTrace(0.15f);
            }
        }
    }
}

/**
 * @brief Modulate plasticity rates based on dopamine level
 * 
 * This method implements neuromodulation of plasticity:
 * - Positive dopamine increases learning rates
 * - Negative dopamine decreases learning rates
 * - Enhanced biological modeling of developmental changes
 * - Maintains biologically plausible parameter ranges
 */
void AgentBrain::modulate_plasticity_based_on_dopamine() {
    // Enhanced neuromodulation with biological plausibility
    float plasticityFactor = 0.3f + 0.7f * (0.5f + 0.5f * dopamineLevel_);  // Range [0.3, 1.0]
    
    // Apply to STDP with better biological plausibility
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Scale STDP weights based on dopamine level
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        
        float ltpWeight = baseLTP * plasticityFactor;
        float ltdWeight = baseLTD * plasticityFactor;
        
        // Ensure reasonable weight ranges
        ltpWeight = std::clamp(ltpWeight, 0.001f, 0.05f);
        ltdWeight = std::clamp(ltdWeight, 0.001f, 0.05f);
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
}

/**
 * @brief Update development system parameters based on age
 * 
 * @param timestep Time elapsed since last development update
 * 
 * Implements staged development with changing plasticity and connectivity:
 * - Initial stage: high plasticity, rapid learning
 * - Critical period: moderate plasticity, sensitive learning
 * - Maturation: decreasing plasticity, stable behavior
 * - Adult: low plasticity, long-term memory consolidation
 * - Aging: declining function and plasticity
 * 
 * Also updates structural plasticity parameters based on developmental stage
 */
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages with more gradual transitions
    update_developmental_stages();
    
    // Structural plasticity changes with development using better modeling
    if (structuralPlasticityEnabled_) {
        update_structural_plasticity();
    }
}

/**
 * @brief Update developmental stage parameters
 * 
 * Implements enhanced developmental progression with more realistic timing:
 * - Initial stage: 0-60 simulation time (high plasticity)
 * - Critical period: 60-180 simulation time (moderate plasticity)
 * - Maturation: 180-450 simulation time (decreasing plasticity)
 * - Adult: 450+ simulation time (stable, low plasticity)
 */
void AgentBrain::update_developmental_stages() {
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 180.0) {
        plasticityModifier_ = 0.7f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 450.0) {
        plasticityModifier_ = 0.4f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.15f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

/**
 * @brief Update structural plasticity based on developmental stage
 * 
 * Implements sophisticated structural connectivity changes:
 * - Synaptogenesis decreases with development
 * - Pruning increases with age
 * - Maintains biologically realistic parameter ranges
 * - Enhances early developmental rapid growth
 */
void AgentBrain::update_structural_plasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Enhanced structural plasticity modeling
        float synaptogenesisBase = 0.0001f;
        float pruningBase = 0.00001f;
        
        // Synaptogenesis decreases with development, pruning increases
        float synaptogenesisRate = synaptogenesisBase * plasticityModifier_;
        float pruningRate = pruningBase * (1.5f - plasticityModifier_);
        
        // Apply biologically realistic bounds
        synaptogenesisRate = std::clamp(synaptogenesisRate, 0.00001f, 0.0002f);
        pruningRate = std::clamp(pruningRate, 0.000001f, 0.00002f);
        
        sp->setSynaptogenesisRate(synaptogenesisRate);
        sp->setPruningRate(pruningRate);
    }
}

/**
 * @brief Get current developmental stage
 * 
 * @return DevelopmentalStage Current developmental stage
 * 
 * Provides access to the brain's current developmental state for debugging
 * and analysis of learning progression.
 */
DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

/**
 * @brief Get neuromodulation level
 * 
 * @return float Current dopamine level (-1.0 to 1.0)
 * 
 * Returns the current neuromodulatory state which influences plasticity
 * and learning rates throughout the brain.
 */
float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

/**
 * @brief Get curiosity level
 * 
 * @return float Current curiosity level (0.0 to 1.0)
 * 
 * Returns the agent's exploration drive, which affects action selection
 * and exploration behavior.
 */
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

/**
 * @brief Get novelty level
 * 
 * @return float Current novelty level (0.0 to 1.0)
 * 
 * Returns the environmental novelty signal, which is the primary driver
 * of curiosity and exploration behavior.
 */
float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

/**
 * @brief Get prediction error
 * 
 * @return float Current prediction error (-1.0 to 1.0)
 * 
 * Returns the reward prediction error signal, which drives reward-based
 * learning and dopamine release.
 */
float AgentBrain::getPredictionError() const {
    return predictionError_;
}

/**
 * @brief Reset agent for new episode
 * 
 * Resets all neuromodulatory, developmental, and sensory state variables
 * to initial conditions for a fresh learning episode.
 * 
 * This method is typically called at the start of each training episode
 * to ensure consistent initial conditions.
 */
void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision with efficient operation
    previousVision_.assign(previousVision_.size(), 0.0f);
}

} // namespace nlm
