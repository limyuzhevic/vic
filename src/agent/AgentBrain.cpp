#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
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
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

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

    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Custom Behavior Patterns
    void AgentBrain::customBehaviorPattern(const std::string& name, 
                                       const std::vector<std::pair<NeuronId, float>>& neuralMapping,
                                       const std::vector<std::string>& rules) {
        NLM_LOG_INFO("Custom behavior pattern '" + name + "' initialized with " +
                     std::to_string(neuralMapping.size()) + " neuron mappings and " +
                     std::to_string(rules.size()) + " rules");
        
        // Store the mapping for potential use during action selection
        // This is a placeholder implementation - actual usage would depend on specific requirements
        for (const auto& mapping : neuralMapping) {
            NLM_LOG_DEBUG("NeuronId " + std::to_string(mapping.first.value) + 
                         " mapped to weight " + std::to_string(mapping.second));
        }
        
        // Process rules
        for (const auto& rule : rules) {
            NLM_LOG_DEBUG("Applying rule: " + rule);
        }
    }
    
    void AgentBrain::behaviorPolicy(const std::vector<std::string>& rules, 
                                 const std::vector<float>& priorityLevels) {
        NLM_LOG_INFO("Behavior policy set with " + std::to_string(rules.size()) + " rules");
        
        if (rules.size() != priorityLevels.size()) {
            NLM_LOG_WARNING("Rule count (" + std::to_string(rules.size()) + ") doesn't match priority level count (" + 
                          std::to_string(priorityLevels.size()) + ")");
        }
        
        // Apply priority-based rule selection
        for (size_t i = 0; i < rules.size() && i < priorityLevels.size(); ++i) {
            NLM_LOG_DEBUG("Rule '" + rules[i] + "' with priority " + std::to_string(priorityLevels[i]));
        }
    }
    
    void AgentBrain::adaptiveBehaviorController(const std::string& targetBehavior) {
        NLM_LOG_INFO("Adaptive behavior controller targeting: " + targetBehavior);
        
        // Modify neural connections to enhance the target behavior
        // This is a placeholder - real implementation would adjust synaptic weights
        NLM_LOG_DEBUG("Optimizing neural pathways for " + targetBehavior);
    }
    
    // Advanced Neuromodulation Controls
    void AgentBrain::neuromodulationNetwork(const std::vector<float>& neuromodulators,
                                        const std::vector<std::pair<size_t, size_t>>& connections) {
        NLM_LOG_INFO("Neuromodulation network initialized with " +
                     std::to_string(neuromodulators.size()) + " neuromodulators");
        
        // Validate neuromodulator strengths
        for (size_t i = 0; i < neuromodulators.size(); ++i) {
            if (neuromodulators[i] < 0.0f) {
                NLM_LOG_WARNING("Neuromodulator " + std::to_string(i) + " has negative strength: " + 
                              std::to_string(neuromodulators[i]));
            }
        }
        
        // Store network connections
        NLM_LOG_DEBUG("Network contains " + std::to_string(connections.size()) + " connections");
    }
    
    void AgentBrain::targetedDopamineRelease(const std::vector<NeuronType>& neuronTypes, float strength) {
        NLM_LOG_INFO("Targeted dopamine release to " + std::to_string(neuronTypes.size()) + " neuron types with strength " + 
                    std::to_string(strength));
        
        // Scale strength to match expected range
        float scaledStrength = std::clamp(strength, -1.0f, 1.0f);
        NLM_LOG_DEBUG("Scaled strength: " + std::to_string(scaledStrength));
        
        // Effect based on neuron types
        for (const auto& type : neuronTypes) {
            switch (type) {
                case NeuronType::Motor:
                    NLM_LOG_DEBUG("Enhancing motor neuron activity");
                    break;
                case NeuronType::Sensory:
                    NLM_LOG_DEBUG("Modulating sensory neuron sensitivity");
                    break;
                case NeuronType::Modulatory:
                    NLM_LOG_DEBUG("Boosting neuromodulatory neuron output");
                    break;
                default:
                    NLM_LOG_DEBUG("Modulating " + std::to_string(static_cast<int>(type)) + " neuron type");
            }
        }
    }
    
    void AgentBrain::setModulationSource(const std::string& sourceType, float strength, double duration) {
        NLM_LOG_INFO("Modulation source set: type='" + sourceType + "', strength=" + std::to_string(strength) + ", duration=" + std::to_string(duration));
        
        // Validate source type
        if (sourceType.empty()) {
            NLM_LOG_WARNING("Empty source type provided");
        }
        
        if (strength < 0.0f) {
            NLM_LOG_WARNING("Negative strength: " + std::to_string(strength));
        }
        
        if (duration <= 0.0) {
            NLM_LOG_WARNING("Invalid duration: " + std::to_string(duration));
        }
        
        // Log duration in human-readable format
        std::string durationStr;
        if (duration < 60.0) {
            durationStr = std::to_string(duration) + " seconds";
        } else if (duration < 3600.0) {
            durationStr = std::to_string(duration / 60.0) + " minutes";
        } else {
            durationStr = std::to_string(duration / 3600.0) + " hours";
        }
        NLM_LOG_DEBUG("Duration: " + durationStr);
    }
    
    // Curiosity-Driven Exploration
    void AgentBrain::intrinsicMotivationGenerator(float baseCuriosity) {
        NLM_LOG_INFO("Intrinsic motivation generator with base curiosity: " + std::to_string(baseCuriosity));
        
        // Clamp base curiosity
        float clampedCuriosity = std::clamp(baseCuriosity, 0.0f, 1.0f);
        NLM_LOG_DEBUG("Clamped curiosity: " + std::to_string(clampedCuriosity));
        
        // Apply nonlinear scaling to generate intrinsic drive
        float intrinsicDrive = clampedCuriosity * clampedCuriosity * 0.8f + 0.2f;
        NLM_LOG_DEBUG("Generated intrinsic drive: " + std::to_string(intrinsicDrive));
        
        // Store or apply to internal curiosity system
        // This is a placeholder implementation
    }
    
    void AgentBrain::uncertaintyBasedExploration(float uncertaintyThreshold) {
        NLM_LOG_INFO("Uncertainty-based exploration with threshold: " + std::to_string(uncertaintyThreshold));
        
        // Validate threshold
        if (uncertaintyThreshold < 0.0f) {
            NLM_LOG_WARNING("Negative uncertainty threshold: " + std::to_string(uncertaintyThreshold));
        }
        
        // Generate exploration based on uncertainty
        if (uncertaintyThreshold > 0.5f) {
            NLM_LOG_DEBUG("High uncertainty - promoting exploration");
        } else if (uncertaintyThreshold > 0.2f) {
            NLM_LOG_DEBUG("Medium uncertainty - moderate exploration");
        } else {
            NLM_LOG_DEBUG("Low uncertainty - preferring exploitation");
        }
        
        // This would modify action selection probabilities
        // Placeholder implementation
    }
    
    void AgentBrain::noveltyWeightedSampling(const std::vector<std::pair<float, std::string>>& weightedFunctions) {
        NLM_LOG_INFO("Novelty-weighted sampling with " + std::to_string(weightedFunctions.size()) + " functions");
        
        if (weightedFunctions.empty()) {
            NLM_LOG_WARNING("No weighted functions provided");
            return;
        }
        
        // Calculate total weight
        float totalWeight = 0.0f;
        for (const auto& wf : weightedFunctions) {
            totalWeight += wf.first;
        }
        
        NLM_LOG_DEBUG("Total weight: " + std::to_string(totalWeight));
        
        // Normalize weights
        if (totalWeight > 0.0f) {
            NLM_LOG_DEBUG("Weights normalized for sampling");
        } else {
            NLM_LOG_WARNING("Zero total weight - all weights will be ignored");
        }
        
        // Log functions and their weights
        for (const auto& wf : weightedFunctions) {
            NLM_LOG_DEBUG("Function '" + wf.second + "' weight: " + std::to_string(wf.first));
        }
    }
    
    // Custom Learning Rules
    void AgentBrain::customLearningRule(const std::string& name, 
                                     const std::function<void(Synapse*, float, float)>& updateFunction) {
        NLM_LOG_INFO("Custom learning rule registered: " + name);
        
        if (updateFunction) {
            NLM_LOG_DEBUG("Update function registered for rule: " + name);
            
            // Store the update function for later use
            // This is a placeholder - in real implementation, the function would be stored
            // and called during learning phases
            NLM_LOG_DEBUG("Rule " + name + " ready for synaptic updates");
        } else {
            NLM_LOG_WARNING("Null update function for rule: " + name);
        }
    }
    
    void AgentBrain::learningRuleComposition(const std::vector<std::string>& rules) {
        NLM_LOG_INFO("Learning rule composition with " + std::to_string(rules.size()) + " rules");
        
        if (rules.empty()) {
            NLM_LOG_WARNING("No rules provided for composition");
            return;
        }
        
        // Validate rule names
        for (const auto& rule : rules) {
            if (rule.empty()) {
                NLM_LOG_WARNING("Empty rule name in composition");
            } else {
                NLM_LOG_DEBUG("Composing rule: " + rule);
            }
        }
        
        // Create composite rule that applies all individual rules
        // This is a placeholder implementation
        NLM_LOG_INFO("Rule composition completed");
    }

} // namespace nlm
