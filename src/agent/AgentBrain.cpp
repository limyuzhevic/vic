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
        // Create balanced distribution: ~16.7% per action, ~25% per sensory modality
        std::vector<std::vector<Neuron*>&> motorGroups = {
            motorForward_, motorBackward_, motorTurnLeft_, 
            motorTurnRight_, motorInteract_, motorWait_
        };
        std::vector<std::vector<Neuron*>&> sensoryGroups = {
            sensoryVision_, sensoryTouch_, sensoryInternal_, 
            sensoryProprioception_
        };
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Distribute motor neurons to groups based on current group sizes
                    for (Neuron* n : pop->getNeurons()) {
                        // Find the group with the fewest neurons for balanced distribution
                        auto minmaxIt = std::min_element(motorGroups.begin(), motorGroups.end(),
                            [](const std::vector<Neuron*>& a, const std::vector<Neuron*>& b) {
                                return a.size() < b.size();
                            });
                        minmaxIt->push_back(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    // Distribute sensory neurons to groups based on current group sizes
                    for (Neuron* n : pop->getNeurons()) {
                        // Find the group with the fewest neurons for balanced distribution
                        auto minmaxIt = std::min_element(sensoryGroups.begin(), sensoryGroups.end(),
                            [](const std::vector<Neuron*>& a, const std::vector<Neuron*>& b) {
                                return a.size() < b.size();
                            });
                        minmaxIt->push_back(n);
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
    
    // Vision input with enhanced validation
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Clamp vision intensity to valid range [0,1] before processing
                float intensity = std::clamp(vision[i], 0.0f, 1.0f);
                // Inject current proportional to vision intensity with gain
                float current = intensity * 5.0f;  // Scale factor
                // Prevent excessive current that could cause instability
                if (std::abs(current) < 100.0f) {
                    sensoryVision_[i]->injectCurrent(current);
                }
            }
        }
    }
    
    // Touch input with validation for proximity sensors
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float proximity = std::clamp(touch[i], 0.0f, 1.0f);
                // Binary collision signal with hysteresis to prevent noise
                float current = proximity * 8.0f * (proximity > 0.1f ? 1.0f : 0.0f);
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals with homeostatic range validation
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float internalSignal = std::clamp(intern[i], -1.0f, 1.0f);
                // Center at zero and scale, maintaining biological plausibility
                float current = (internalSignal * 2.0f - 1.0f) * 5.0f;
                // Limit to prevent runaway excitation
                current = std::clamp(current, -10.0f, 10.0f);
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception with movement threshold validation
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float proprioception = std::clamp(proprio[i], 0.0f, 1.0f);
                // Position/velocity signals centered and scaled appropriately
                float current = (proprioception * 2.0f - 1.0f) * 3.0f;
                // Proprioception signals are typically smaller
                current = std::clamp(current, -5.0f, 5.0f);
                sensoryProprioception_[i]->injectCurrent(current);
            }
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
    
    // Apply curiosity-based exploration with configurable threshold
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate activity for each motor group using template function for consistency
    auto calculateActivity = [](const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            // Add bounds checking to prevent numerical instability
            float deviation = std::abs(n->getState().membranePotential - n->getState().restingPotential);
            deviation = std::clamp(deviation, 0.0f, 10.0f);  // Cap at reasonable values
            sum += deviation;
        }
        return sum / neurons.size();
    };
    
    // Vector of all motor commands for cleaner max finding
    std::vector<std::pair<MotorCommand, float>> commandActivities = {
        {MotorCommand::MoveForward, calculateActivity(motorForward_)},
        {MotorCommand::MoveBackward, calculateActivity(motorBackward_)},
        {MotorCommand::TurnLeft, calculateActivity(motorTurnLeft_)},
        {MotorCommand::TurnRight, calculateActivity(motorTurnRight_)},
        {MotorCommand::Interact, calculateActivity(motorInteract_)},
        {MotorCommand::Wait, calculateActivity(motorWait_)}
    };
    
    // Find command with maximum activity using STL algorithms for cleaner code
    auto maxIt = std::max_element(commandActivities.begin(), commandActivities.end(),
        [](const std::pair<MotorCommand, float>& a, const std::pair<MotorCommand, float>& b) {
            return a.second < b.second;
        });
    
    MotorCommand best = maxIt->first;
    float bestActivity = maxIt->second;
    
    // Only act if there's meaningful activity with configurable threshold
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration (maximum 30% chance)
        float exploreChance = curiosityLevel_ * 0.3f;
        
        // Generate random motor command using valid MotorCommand enum values
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - include all 8 enum values for true randomness
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;  // Pan sensor left
                case 5: return MotorCommand::LookRight;  // Pan sensor right
                case 6: return MotorCommand::Interact;  // Interact with nearby object
                case 7: return MotorCommand::Wait;  // No movement
                default: return MotorCommand::Wait;  // Safety fallback
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

AgentBrain::~AgentBrain() = default;

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

bool AgentBrain::isNeuronInGroup(Neuron* neuron) const {
    if (!neuron) return false;
    
    // Check all motor groups
    const std::vector<std::vector<Neuron*>>& motorGroups = {
        motorForward_, motorBackward_, motorTurnLeft_, 
        motorTurnRight_, motorInteract_, motorWait_
    };
    for (const auto& group : motorGroups) {
        if (std::find(group.begin(), group.end(), neuron) != group.end()) {
            return true;
        }
    }
    
    // Check all sensory groups
    const std::vector<std::vector<Neuron*>>& sensoryGroups = {
        sensoryVision_, sensoryTouch_, sensoryInternal_, 
        sensoryProprioception_
    };
    for (const auto& group : sensoryGroups) {
        if (std::find(group.begin(), group.end(), neuron) != group.end()) {
            return true;
        }
    }
    
    return false;
}

size_t AgentBrain::getTotalMotorNeurons() const {
    size_t total = 0;
    total += motorForward_.size();
    total += motorBackward_.size();
    total += motorTurnLeft_.size();
    total += motorTurnRight_.size();
    total += motorInteract_.size();
    total += motorWait_.size();
    return total;
}

size_t AgentBrain::getTotalSensoryNeurons() const {
    size_t total = 0;
    total += sensoryVision_.size();
    total += sensoryTouch_.size();
    total += sensoryInternal_.size();
    total += sensoryProprioception_.size();
    return total;
}

void AgentBrain::logGroupStats() const {
    NLM_LOG_INFO("Neuron Group Statistics:");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(getTotalMotorNeurons()));
    NLM_LOG_INFO("    Forward: " + std::to_string(motorForward_.size()));
    NLM_LOG_INFO("    Backward: " + std::to_string(motorBackward_.size()));
    NLM_LOG_INFO("    Turn Left: " + std::to_string(motorTurnLeft_.size()));
    NLM_LOG_INFO("    Turn Right: " + std::to_string(motorTurnRight_.size()));
    NLM_LOG_INFO("    Interact: " + std::to_string(motorInteract_.size()));
    NLM_LOG_INFO("    Wait: " + std::to_string(motorWait_.size()));
    
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(getTotalSensoryNeurons()));
    NLM_LOG_INFO("    Vision: " + std::to_string(sensoryVision_.size()));
    NLM_LOG_INFO("    Touch: " + std::to_string(sensoryTouch_.size()));
    NLM_LOG_INFO("    Internal: " + std::to_string(sensoryInternal_.size()));
    NLM_LOG_INFO("    Proprioception: " + std::to_string(sensoryProprioception_.size()));
}

} // namespace nlm
