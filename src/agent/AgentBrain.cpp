#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

/**
 * @brief Constructor for AgentBrain - initializes the agent brain with a neural brain instance
 * 
 * Sets up the agent brain by analyzing the brain's neural regions and organizing neurons
 * into functional groups for sensory processing and motor control. This distribution
 * allows for efficient, modular processing of sensory inputs and generation of motor
 * commands without requiring global searches through the entire neural network.
 * 
 * The initialization process creates references to neurons organized by type:
 * - Motor neurons are distributed across action groups (forward, backward, turn, interact, wait)
 * - Sensory neurons are distributed across perception modalities (vision, touch, internal, proprioception)
 * 
 * This organization enables efficient, direct access to the neural populations that will be used
 * for processing each type of input and generating appropriate responses.
 * 
 * @param brain Shared pointer to the neural brain instance that provides the neural computation engine
 */
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

/**
 * @brief Destructor - cleans up AgentBrain resources
 * 
 * The AgentBrain destructor performs no explicit cleanup as it uses
 * smart pointers for all managed resources. The neural brain is owned
 * by a shared pointer and will be automatically cleaned up when all
 * shared references are released.
 */
AgentBrain::~AgentBrain() = default;

/**
 * @brief Initialize the agent brain with world configuration
 * 
 * Prepares the internal state for agent-world interaction by:
 * - Allocating memory for visual input history (for novelty detection)
 * - Resetting developmental counters to initial values
 * - Setting initial plasticity modifier for maximum learning capacity
 * - Logging initialization statistics for debugging and monitoring
 * 
 * This method should be called once after constructing the AgentBrain
 * and before it will be used in simulation steps. The agent brain
 * uses this information to process sensory inputs from the world
 * and generate appropriate motor responses.
 * 
 * @param world Reference to the world simulation providing the environment
 *              for agent interaction. The world provides sensory inputs
 *              and receives motor commands.
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
 * @brief Get the expected size of sensory input vector for the brain
 * 
 * Returns the total number of scalar values that must be provided as
 * sensory input to the brain. This value determines the dimensionality
 * of the brain's sensory input layer and is calculated based on the
 * specific sensory modalities the agent uses.
 * 
 * The current input composition is fixed based on the agent's sensory
 * capabilities:
 * - Vision: 16x16 = 256 values (grayscale intensity grid)
 * - Touch: 8 values (binary collision signals)
 * - Internal: 4 values (homeostatic signals)
 * - Proprioception: 6 values (body position/velocity)
 * 
 * @return Total sensory input size (256 + 8 + 4 + 6 = 270)
 */
size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

/**
 * @brief Get the expected size of motor output vector
 * 
 * Returns the number of motor action groups that the brain can activate
 * to produce behavior in the world. Each group corresponds to a specific
 * type of action the agent can perform.
 * 
 * @return Number of available motor commands (6: forward, backward, turn left,
         turn right, interact, wait)
 */
size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

/**
 * @brief Process new sensory percept and inject into the neural brain
 * 
 * This method implements the complete sensory transduction pipeline, converting
 * world percepts into neural current injections that drive the brain's computation.
 * It processes multiple sensory modalities according to their specific characteristics
 * and scaling factors, maintaining internal state for novelty detection and curiosity.
 * 
 * The sensory processing includes:
 * 1. **Vision**: 256 grayscale values (16×16 grid) scaled by 5.0x
 * 2. **Touch**: 8 binary collision signals scaled by 8.0x  
 * 3. **Internal signals**: 4 homeostatic values centered and scaled by 5.0x
 * 4. **Proprioception**: 6 body state values centered and scaled by 3.0x
 * 
 * Each modality uses different scaling factors to ensure neural relevance:
 * - Vision: Low-contrast environmental patterns (smooth gradient)
 * - Touch: Discrete collision events (strong, clear signals)
 * - Internal: Continuous homeostatic needs (moderate activity)
 * - Proprioception: Body movement dynamics (moderate, smooth)
 * 
 * The method also maintains a novelty detection system that computes the
 * difference between current and previous visual input, which drives curiosity
 * and exploration behavior. Novelty signals decay over time to maintain
 * balance between exploration and exploitation.
 * 
 * @param percept Sensory percept containing all current world inputs from
 *                the agent's sensors (vision, touch, internal signals,
 *                proprioception, and optionally audio)
 */
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: no brain available");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    // Vision is the primary source of environmental information, processed
    // at relatively low resolution (16×16) to match the agent's visual
    // capabilities and reduce computational load
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            // Scale factor of 5.0 ensures sufficient neural activation for
            // processing visual features while maintaining signal stability
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    // Touch provides high-salience information about immediate contacts
    // with objects, processed with strong scaling to ensure neural responses
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Collision signal with 8.0x scaling for strong neural response
            // Touch events require immediate processing and strong activation
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    // Internal signals represent homeostatic needs and internal states
    // that drive fundamental behaviors like eating, drinking, and resting
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Center and scale internal signals to range [-5.0, 5.0]
            // Homeostatic signals need to be centered around zero to represent
            // both positive and negative needs, scaled for moderate neural impact
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    // Proprioception provides feedback about the agent's body state including
    // position, velocity, and orientation, essential for coordinated movement
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Center and scale proprioceptive signals to range [-3.0, 3.0]
            // Body state signals require moderate scaling to avoid overwhelming
            // the motor system while maintaining sufficient sensitivity
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) for curiosity-driven exploration
    // Novelty detection compares current visual input with recent history to identify
    // changes in the environment that may warrant investigation and learning
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize novelty to range [0.0, 1.0] for consistent processing
        // Normalization allows the novelty signal to be used across different
        // visual scene complexities and lighting conditions
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update novelty signal over time
        // Decay factor of 0.99 provides exponential decay, balancing immediate
        // response to novel stimuli with persistence for ongoing investigation
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store current vision for next time step comparison
        // Maintaining visual history enables continuous novelty detection
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    // Curiosity represents the agent's motivation to explore novel sensory inputs
    // and resolve prediction errors, driving learning and discovery behavior
    if (curiosityEnabled_) {
        // Combine novelty detection (environmental change) and prediction error
        // (unexpected outcomes) to compute overall curiosity level
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        
        // Clamp curiosity to reasonable range [0.0, 1.0]
        // Bounds prevent numerical instability and ensure predictable behavior
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Log curiosity level for debugging and monitoring
        NLM_LOG_DEBUG("Curiosity level: " + std::to_string(curiosityLevel_));
    }
}

/**
 * @brief Decode current neural motor activity into a concrete command
 * 
 * This method implements the motor decoding pipeline, converting neural activity
 * patterns across the brain's motor neuron populations into actionable motor
 * commands. The decoding process includes intelligent defaults and exploration
 * mechanisms to balance goal-directed behavior with curiosity-driven exploration.
 * 
 * The motor decoding consists of three main stages:
 * 1. **Neural decoding**: Analyze motor neuron activity patterns to select the
 *    most active command based on weighted voting across all motor groups
 * 2. **Exploration gating**: Check if curiosity levels justify random exploration
 * 3. **Execution**: Return the final motor command (neural or exploratory)
 * 
 * The neural decoding uses a threshold mechanism (0.5) to prevent weak,
 * ambiguous neural patterns from triggering meaningful actions, ensuring that
 * only strong, confident motor commands are executed.
 * 
 * @return MotorCommand The selected motor command (MoveForward, MoveBackward,
         TurnLeft, TurnRight, Interact, or Wait) for execution by the world
 */
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: no brain available");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

/**
 * @brief Motor decoding: convert neural activity to motor command
 * 
 * This method implements the core neural decoding mechanism, analyzing the
 * activity patterns across motor neuron groups to determine which action
 * the brain is intending to execute. It uses a weighted voting system where
 * each motor command group contributes based on the average activity of
 * its neurons, providing a robust method for mapping distributed neural
 * activity to specific behaviors.
 * 
 * The decoding process involves:
 * 1. Computing average activity for each motor command group
 * 2. Finding the command with maximum activity (with wait as fallback)
 * 3. Applying a confidence threshold to prevent weak actions from executing
 * 
 * The use of lambda functions allows for clean, reusable activity computation
 * across all motor groups, ensuring consistent calculation methodology.
 * 
 * @return MotorCommand The most active motor command, or Wait if activity
         is below threshold
 */
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            // This captures how far the neuron is from its resting potential,
            // which correlates with firing readiness and commitment to action
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
    
    // Find maximum activity using structured binding for clarity
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
    
    // Iterate through commands to find the most active one
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    // This threshold prevents weak neural patterns from triggering actions,
    // ensuring that only confident, strong intentions result in behavior
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

/**
 * @brief Motor command selection with curiosity/exploration
 * 
 * This method implements the curiosity-driven exploration component of the
 * agent's behavioral selection. When curiosity levels are high, it introduces
 * controlled randomness to promote exploration of novel behaviors and sensory
 * inputs, complementing the goal-directed neural decoding process.
 * 
 * The exploration mechanism works as follows:
 * 1. Check if curiosity exceeds the exploration threshold (0.5)
 * 2. Calculate random exploration probability based on curiosity level
 * 3. Generate random motor command if exploration trigger occurs
 * 4. Otherwise, return the default (neural) command
 * 
 * This approach implements the exploration-exploitation tradeoff, where
 * higher curiosity (from novel sensory inputs or prediction errors) increases
 * the likelihood of trying new actions, supporting continuous learning and
 * adaptation to changing environments.
 * 
 * @param defaultCmd The motor command selected by pure neural decoding
 * @return MotorCommand Final motor command (may be modified by curiosity)
 */
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

/**
 * @brief Apply reward prediction error neuromodulation to plasticity
 * 
 * This method implements the reward-based learning system, updating neuromodulatory
 * signals based on the difference between actual and predicted rewards. This prediction
 * error serves as a teaching signal that modulates synaptic plasticity throughout the
 * brain, enabling the agent to learn from its experiences.
 * 
 * The reward modulation process includes:
 * 1. Computing prediction error (actual - predicted reward)
 * 2. Updating expected reward using exponential moving average
 * 3. Generating dopamine-like signal based on prediction error
 * 4. Applying weight changes to synapses with eligibility traces
 * 5. Modulating plasticity rates based on neuromodulatory signal
 * 
 * This biological-inspired mechanism allows the agent to learn which actions
 * lead to good outcomes and which lead to bad outcomes, shaping future behavior
 * through synaptic modifications.
 * 
 * @param reward The actual reward received from the world for current action
 * @param predictedReward The brain's prediction of expected reward for that action
 */
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_DEBUG("Reward modulation skipped: disabled or no brain");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    // This creates a running estimate of expected rewards
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    // Prevents numerical issues and maintains biological plausibility
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Log prediction error for monitoring
    NLM_LOG_DEBUG("Prediction error: " + std::to_string(predictionError_));
    NLM_LOG_DEBUG("Dopamine level: " + std::to_string(dopamineLevel_));
    
    // Apply to all synapses with eligibility traces
    // Only synapses with recent activity will be modified
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                // Weight change is proportional to eligibility, dopamine level,
                // and current plasticity modifier
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                // Prevents unbounded growth and maintains temporal credit assignment
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    // This provides a global regulation of learning rate based on reward prediction
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    // Spike-Timing-Dependent Plasticity is modulated by reward prediction error
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
        NLM_LOG_DEBUG("STDP weights modified - LTP: " + std::to_string(0.01f * plasticityFactor) +
                     ", LTD: " + std::to_string(0.012f * plasticityFactor));
    }
}

/**
 * @brief Update development system based on agent age
 * 
 * This method implements the developmental trajectory of the agent,
 * tracking its age and updating its developmental stage. Developmental
 * stages affect plasticity rates and other physiological parameters,
 * allowing for biologically-inspired learning curves where the agent
 * is more plastic (changeable) in early stages and more stable in later stages.
 * 
 * The developmental system includes:
 * 1. Advancing developmental age based on simulation time
 * 2. Determining current developmental stage based on age
 * 3. Adjusting plasticity modifiers according to stage
 * 4. Updating structural plasticity parameters for synaptogenesis and pruning
 * 
 * This developmental progression allows for learning that changes over
 * time, starting with rapid learning in early stages and converging to
 * more stable behavior in adult stages, mimicking biological development.
 * 
 * @param timestep Time elapsed since last development update
 */
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_DEBUG("Development skipped: disabled or no brain");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation of developmental progression
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        NLM_LOG_DEBUG("Developmental stage: Initial (high plasticity)");
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        NLM_LOG_DEBUG("Developmental stage: CriticalPeriod (moderate plasticity)");
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        NLM_LOG_DEBUG("Developmental stage: Maturation (reduced plasticity)");
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        NLM_LOG_DEBUG("Developmental stage: Adult (stable)");
    }
    
    // Structural plasticity changes with development
    // Different developmental stages have different rates of synaptogenesis
    // and pruning, creating a biologically realistic developmental trajectory
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            // More neuron creation and connection establishment in early stages
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
            
            NLM_LOG_DEBUG("Structural plasticity updated - synaptogenesis: " + std::to_string(synRate) +
                         ", pruning: " + std::to_string(pruneRate));
        }
    }
}

/**
 * @brief Get the current developmental stage of the agent
 * 
 * Returns the developmental stage (Initial, CriticalPeriod, Maturation, Adult)
 * which determines plasticity rates and behavioral capabilities. The developmental
 * stage affects how the agent learns and adapts to its environment.
 * 
 * @return DevelopmentalStage Current developmental stage
 */
DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot get developmental stage: no brain available");
        return DevelopmentalStage::Initial;
    }
    return brain_->getDevelopmentalStage();
}

/**
 * @brief Get current neuromodulation level (dopamine-like signal)
 * 
 * Returns the current prediction error signal that modulates neural
 * excitability and synaptic plasticity. This signal represents the difference
 * between received rewards and expected rewards, serving as a teaching signal
 * for learning and behavioral adjustment.
 * 
 * @return float Neuromodulation level (-1.0 to 1.0, negative = prediction error)
 */
float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

/**
 * @brief Get current curiosity level driving exploration behavior
 * 
 * Curiosity represents the agent's motivation to explore novel sensory inputs
 * and resolve prediction errors. It combines novelty detection and prediction
 * error to determine the agent's tendency toward exploration versus exploitation.
 * 
 * High curiosity levels drive random motor command selection, promoting
 * discovery of new behaviors and sensory inputs. Low curiosity levels favor
 * exploitation of known effective behaviors.
 * 
 * @return float Curiosity level (0.0 to 1.0, higher = more exploration)
 */
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

/**
 * @brief Get current novelty level based on sensory changes
 * 
 * Novelty is computed as the difference between current and previous visual
 * input, providing a measure of environmental change that can drive curiosity
 * and exploration behavior. Novelty signals decay over time to balance
 * immediate response to new inputs with persistent motivation for exploration.
 * 
 * @return float Novelty level (0.0 to 1.0, higher = more environmental change)
 */
float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

/**
 * @brief Get current prediction error signal
 * 
 * Prediction error represents the difference between received rewards and
 * expected rewards, serving as a teaching signal for learning and behavioral
 * adjustment. Positive prediction error indicates better-than-expected outcomes,
 * while negative prediction error indicates worse-than-expected outcomes.
 * 
 * @return float Prediction error value
 */
float AgentBrain::getPredictionError() const {
    return predictionError_;
}

/**
 * @brief Reset agent state for a new episode
 * 
 * Clears all neuromodulatory signals, developmental age, plasticity
 * modifiers, and sensory history to prepare for fresh learning. This method
 * is called at the beginning of each new episode to ensure the agent starts
 * with a clean slate and can learn from new experiences without interference
 * from previous episode's accumulated state.
 */
void AgentBrain::reset() {
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision for novelty detection
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset for new episode");
}

} // namespace nlm