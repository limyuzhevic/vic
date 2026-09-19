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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: No brain pointer set");
        return;
    }
    
    if (percept.getVision().empty() && percept.getTouch().empty() && 
        percept.getInternal().empty() && percept.getProprioception().empty()) {
        NLM_LOG_DEBUG("AgentBrain: Empty percept received");
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            // Clamp current to prevent excessive excitation
            current = std::clamp(current, -50.0f, 50.0f);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            current = std::clamp(current, 0.0f, 20.0f);  // Touch is excitatory
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            // Internal signals can be excitatory or inhibitory
            current = std::clamp(current, -20.0f, 20.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            // Proprioception is typically subtle feedback
            current = std::clamp(current, -10.0f, 10.0f);
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
        
        // Decay novelty (biological plausibility)
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Cap novelty at reasonable levels
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        // More sophisticated curiosity formula based on biological models
        // Cur • curiosity = novelty × attentional_weight + prediction_error × surprise_weight
        float attentionWeight = 1.5f - developmentalAge_ / 1000.0f;  // Young brains attend more
        float surpriseWeight = 0.5f + std::abs(predictionError_) * 2.0f;  // Higher surprise = more curiosity
        
        // Normalize weights
        float totalWeight = attentionWeight + surpriseWeight;
        if (totalWeight > 0.0f) {
            attentionWeight /= totalWeight;
            surpriseWeight /= totalWeight;
        }
        
        // Compute curiosity
        curiosityLevel_ = noveltyLevel_ * attentionWeight + std::abs(predictionError_) * surpriseWeight;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 2.0f);  // Allow higher than 1 for exploration
        
        // Apply developmental modulation
        if (developmentalAge_ < 60.0f) {  // Critical period
            curiosityLevel_ *= 1.5f;  // More exploration when young
        } else if (developmentalAge_ > 900.0f) {  // Adult
            curiosityLevel_ *= 0.7f;  // Less exploration when mature
        }
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
    // Exploration: sophisticated curiosity-driven behavior
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        // Multiple exploration strategies based on curiosity level
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (curiosityLevel_ > 0.7f) {
            // High curiosity: random exploration with broader action space
            // Higher chance to try unusual combinations
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
        } else if (curiosityLevel_ > 0.5f) {
            // Medium curiosity: balanced exploration
            // Bias toward novel actions when curiosity is moderate
            if (r < 0.6f) {
                // Continue with default action for stability
                return defaultCmd;
            } else {
                // Try an action not currently being selected
                return selectWithAlternativeCuriosity(defaultCmd);
            }
        } else {
            // Low curiosity (0.3-0.5): minimal exploration
            // Only explore if confidence in current action is low
            if (calculateActionConfidence(defaultCmd) < 0.3f) {
                return selectWithAlternativeCuriosity(defaultCmd);
            } else {
                return defaultCmd;
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithAlternativeCuriosity(MotorCommand defaultCmd) {
    // Select an action different from the default for exploration
    std::vector<MotorCommand> allCommands = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    // Remove default command from consideration
    auto it = std::find(allCommands.begin(), allCommands.end(), defaultCmd);
    if (it != allCommands.end()) {
        allCommands.erase(it);
    }
    
    // Randomly select one of the remaining commands
    int choice = brain_->getRandomGenerator()->uniformInt(0, static_cast<int>(allCommands.size()) - 1);
    return allCommands[choice];
}

float AgentBrain::calculateActionConfidence(MotorCommand cmd) const {
    // Calculate confidence in the selected motor command based on neural activity patterns
    if (!brain_) return 0.0f;
    
    // Get the motor neurons corresponding to this command
    std::vector<Neuron*> relevantNeurons;
    switch (cmd) {
        case MotorCommand::MoveForward: relevantNeurons = motorForward_; break;
        case MotorCommand::MoveBackward: relevantNeurons = motorBackward_; break;
        case MotorCommand::TurnLeft: relevantNeurons = motorTurnLeft_; break;
        case MotorCommand::TurnRight: relevantNeurons = motorTurnRight_; break;
        case MotorCommand::Interact: relevantNeurons = motorInteract_; break;
        case MotorCommand::Wait: relevantNeurons = motorWait_; break;
        case MotorCommand::LookLeft: 
        case MotorCommand::LookRight: 
            // Look commands don't have dedicated neuron groups
            return 0.5f;
    }
    
    if (relevantNeurons.empty()) return 0.5f;
    
    // Calculate average activity of relevant neurons
    float totalActivity = 0.0f;
    for (Neuron* neuron : relevantNeurons) {
        if (neuron) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
    }
    
    float avgActivity = totalActivity / relevantNeurons.size();
    
    // Normalize activity to confidence (0.0 to 1.0)
    // Higher activity = higher confidence
    float maxExpectedActivity = 5.0f; // Based on typical neuron firing patterns
    return std::min(avgActivity / maxExpectedActivity, 1.0f);
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

} // namespace nlm
