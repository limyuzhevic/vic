#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

    /**
     * @brief Constructor for AgentBrain
     * 
     * @param brain Shared pointer to the NLM Brain instance
     * 
     * Initializes the agent with a reference to the brain and populates
     * motor and sensory neuron groups for action selection and processing.
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

AgentBrain::~AgentBrain() = default;

    /**
     * @brief Initialize the agent with world information
     * 
     * @param world The SimpleWorld instance containing environment data
     * 
     * Prepares the agent for operation by setting up internal state and
     * resizing the previous vision buffer to match world dimensions.
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
     * @brief Get the size of the sensory input vector
     * 
     * @return Number of sensory input values expected
     * 
     * The sensory input includes vision (16x16), touch (8), internal (4), 
     * and proprioception (6) inputs, totaling 256 + 8 + 4 + 6 = 274 values.
     */
    size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

    /**
     * @brief Get the size of the motor output vector
     * 
     * @return Number of motor output values (one per action type)
     * 
     * The agent has 6 possible actions: MoveForward, MoveBackward, TurnLeft, 
     * TurnRight, Interact, and Wait.
     */
    size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

    /**
     * @brief Process incoming sensory percepts from the environment
     * 
     * @param percept Contains vision, touch, internal, and proprioception data
     * 
     * Injects sensory currents into corresponding neurons and updates neuromodulation
     * levels based on novelty and prediction error.
     */
    void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot process sensory input: brain pointer is null");
            return;
        }
    
        // Vision input (256 values -> sensoryVision_ neurons)
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    
        // Touch input (8 values -> sensoryTouch_ neurons)
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    
        // Internal signals (4 values -> sensoryInternal_ neurons)
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    
        // Proprioception (6 values -> sensoryProprioception_ neurons)
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
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
            noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
            
            // Decay and update
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Store for next time
            previousVision_ = vision;
        }
    
        // Update curiosity based on novelty
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
    }

    /**
     * @brief Decode motor command from brain activity
     * 
     * @return Selected motor command based on neural activity
     * 
     * Analyzes motor neuron activity to select the most appropriate action,
     * with curiosity-driven exploration when enabled.
     */
    MotorCommand AgentBrain::decodeMotorCommand() {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot decode motor command: brain pointer is null");
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
     * @brief Decode motor command directly from motor neuron activity
     * 
     * @return Motor command based on maximum activity in motor neuron groups
     * 
     * Calculates the average activity level for each motor neuron group
     * (forward, backward, turn left, turn right, interact, wait) and selects
     * the action with the highest activity level.
     */
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
        if (bestActivity < 0.5f) {
            return MotorCommand::Wait;
        }
    
        return best;
    }

    /**
     * @brief Select motor command with curiosity-driven exploration
     * 
     * @param defaultCmd The default motor command to return if no exploration occurs
     * @return Motor command, possibly random if curiosity is high
     * 
     * When curiosity level is high, the agent occasionally selects random actions
     * to encourage exploration. Higher curiosity increases the probability
     * of random action selection.
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
     * @brief Apply reward-modulated learning based on prediction error
     * 
     * @param reward Actual reward received from the environment
     * @param predictedReward Expected reward based on prediction
     * 
     * Updates the agent's dopamine level based on prediction error and applies
     * reward-modulated weight changes to synapses with eligibility traces.
     */
    void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !rewardModulationEnabled_) {
            return;
        }
    
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

    /**
     * @brief Update developmental stage based on agent age
     * 
     * @param timestep Time elapsed since last update
     * 
     * Simulates biological development by transitioning through different
     * developmental stages (Initial, CriticalPeriod, Maturation, Adult)
     * based on accumulated time. Each stage has different plasticity characteristics.
     */
    void AgentBrain::updateDevelopment(double timestep) {
        if (!brain_ || !developmentEnabled_) {
            return;
        }
    
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

    /**
     * @brief Get the current developmental stage
     * 
     * @return Current developmental stage of the agent
     * 
     * Returns the developmental stage set by the brain or defaults to Initial
     * if brain pointer is null.
     */
    DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }

    /**
     * @brief Get the current neuromodulation level (dopamine)
     * 
     * @return Current dopamine level (-1.0 to 1.0)
     */
    float AgentBrain::getNeuromodulationLevel() const {
        return dopamineLevel_;
    }

    /**
     * @brief Get the current curiosity level
     * 
     * @return Current curiosity level (0.0 to 1.0)
     */
    float AgentBrain::getCuriosityLevel() const {
        return curiosityLevel_;
    }

    /**
     * @brief Get the current novelty level
     * 
     * @return Current novelty level (0.0 to 1.0)
     */
    float AgentBrain::getNoveltyLevel() const {
        return noveltyLevel_;
    }

    /**
     * @brief Get the current prediction error
     * 
     * @return Current prediction error (negative for worse-than-expected reward)
     */
    float AgentBrain::getPredictionError() const {
        return predictionError_;
    }

    /**
     * @brief Reset agent state to initial conditions
     * 
     * Resets all neuromodulation levels, memory, and developmental state
     * to their initial values. Also clears the previous vision buffer.
     */
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
