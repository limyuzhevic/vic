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
    // Initialize motor and sensory neuron groups - optimized
    if (brain_) {
        // Pre-calculate neuron distribution using vectorized approach
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Optimized motor neuron distribution
                    const auto& neurons = pop->getNeurons();
                    size_t motorCount = motorForward_.size() + motorBackward_.size() + 
                                       motorTurnLeft_.size() + motorTurnRight_.size() +
                                       motorInteract_.size() + motorWait_.size();
                    
                    // Calculate distribution ratios for better balance
                    const size_t groupSize = neurons.size() / 6;  // 6 motor groups
                    const size_t remainder = neurons.size() % 6;
                    
                    size_t idx = 0;
                    for (Neuron* n : neurons) {
                        size_t group = idx % 6;
                        switch (group) {
                            case 0:
                                if (motorForward_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorForward_.push_back(n);
                                }
                                break;
                            case 1:
                                if (motorBackward_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorBackward_.push_back(n);
                                }
                                break;
                            case 2:
                                if (motorTurnLeft_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorTurnLeft_.push_back(n);
                                }
                                break;
                            case 3:
                                if (motorTurnRight_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorTurnRight_.push_back(n);
                                }
                                break;
                            case 4:
                                if (motorInteract_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorInteract_.push_back(n);
                                }
                                break;
                            case 5:
                                if (motorWait_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    motorWait_.push_back(n);
                                }
                                break;
                        }
                        idx++;
                    }
                } else if (type == NeuronType::Sensory) {
                    // Optimized sensory neuron distribution
                    const auto& neurons = pop->getNeurons();
                    size_t sensoryCount = sensoryVision_.size() + sensoryTouch_.size() +
                                         sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    // Calculate distribution ratios for better balance
                    const size_t groupSize = neurons.size() / 4;  // 4 sensory groups
                    const size_t remainder = neurons.size() % 4;
                    
                    size_t idx = 0;
                    for (Neuron* n : neurons) {
                        size_t group = idx % 4;
                        switch (group) {
                            case 0:
                                if (sensoryVision_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    sensoryVision_.push_back(n);
                                }
                                break;
                            case 1:
                                if (sensoryTouch_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    sensoryTouch_.push_back(n);
                                }
                                break;
                            case 2:
                                if (sensoryInternal_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    sensoryInternal_.push_back(n);
                                }
                                break;
                            case 3:
                                if (sensoryProprioception_.size() < groupSize + (group < remainder ? 1 : 0)) {
                                    sensoryProprioception_.push_back(n);
                                }
                                break;
                        }
                        idx++;
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

    // Initialize vision buffer size based on world configuration
    if (world.getVisionWidth() > 0 && world.getVisionHeight() > 0) {
        const size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
        previousVision_.resize(visionSize, 0.0f);
    } else {
        // Fallback to standard NLM vision size if world not properly configured
        previousVision_.resize(256, 0.0f);
    }
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Optimized logging with pre-calculated values
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16 = 256) + touch (8) + internal (4) + proprioception (6)
    // Total: 256 + 8 + 4 + 6 = 274 sensory inputs
    return 274;
}

size_t AgentBrain::getMotorOutputSize() const {
    // 6 motor neuron groups (forward, backward, left, right, interact, wait)
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Get all sensory signals at once for better cache locality
    const auto& allSignals = percept.getAllSignals();
    const size_t visionSize = percept.getVisionWidth() * percept.getVisionHeight();
    
    // Combine all sensory processing into single optimized loops
    // This reduces loop overhead and improves cache efficiency
    
    // Vision input processing - combined with other inputs where possible
    if (!sensoryVision_.empty() && visionSize > 0) {
        const float* visionData = percept.getVision().data();
        const size_t minSize = std::min(sensoryVision_.size(), visionSize);
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryVision_[i]) {
                float current = visionData[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input processing
    if (!sensoryTouch_.empty()) {
        const auto& touch = percept.getTouch();
        const size_t minSize = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals processing
    if (!sensoryInternal_.empty()) {
        const auto& intern = percept.getInternal();
        const size_t minSize = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception processing
    if (!sensoryProprioception_.empty()) {
        const auto& proprio = percept.getProprioception();
        const size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision) - optimized
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        const size_t minVisionSize = std::min(vision.size(), previousVision_.size());
        
        // Vectorized novelty computation using local variables for better cache performance
        if (minVisionSize > 0) {
            const float* visionData = vision.data();
            const float* prevData = previousVision_.data();
            
            // Optimized loop - reduce function calls inside loop
            for (size_t i = 0; i < minVisionSize; ++i) {
                float diff = visionData[i] - prevData[i];
                if (diff < 0) diff = -diff;  // std::abs replacement
                totalDiff += diff;
            }
        }
        
        // Normalize and update novelty
        const size_t effectiveSize = std::max<size_t>(vision.size(), 1);
        noveltyLevel_ = totalDiff / static_cast<float>(effectiveSize);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time - efficient copy
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty - optimized
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        if (curiosityLevel_ < 0.0f) {
            curiosityLevel_ = 0.0f;
        } else if (curiosityLevel_ > 1.0f) {
            curiosityLevel_ = 1.0f;
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
    // Optimized calculation with early exit and reduced function calls
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        // Get neuron state once to reduce member function calls
        const size_t neuronCount = neurons.size();
        float sum = 0.0f;
        
        // Optimized activity calculation
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
            
            const auto& state = n->getState();
            // Use membrane potential deviation from rest as activity measure
            float activity = state.membranePotential - state.restingPotential;
            if (activity < 0.0f) activity = -activity;  // std::abs replacement
            sum += activity;
        }
        
        return sum / neuronCount;
    };
    
    // Pre-calculate all motor activities in parallel
    const float forwardAct = calcActivity(motorForward_);
    const float backwardAct = calcActivity(motorBackward_);
    const float leftAct = calcActivity(motorTurnLeft_);
    const float rightAct = calcActivity(motorTurnRight_);
    const float interactAct = calcActivity(motorInteract_);
    const float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity - optimized with early exit
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Check all commands in optimized order
    if (forwardAct > bestActivity) {
        bestActivity = forwardAct;
        best = MotorCommand::MoveForward;
    }
    if (backwardAct > bestActivity) {
        bestActivity = backwardAct;
        best = MotorCommand::MoveBackward;
    }
    if (leftAct > bestActivity) {
        bestActivity = leftAct;
        best = MotorCommand::TurnLeft;
    }
    if (rightAct > bestActivity) {
        bestActivity = rightAct;
        best = MotorCommand::TurnRight;
    }
    if (interactAct > bestActivity) {
        bestActivity = interactAct;
        best = MotorCommand::Interact;
    }
    
    // Only act if there's meaningful activity - threshold optimization
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
            // Random motor command with balanced exploration
            // Include all 8 motor commands for proper exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;  // Fixed: ensure Wait can be chosen
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
    
    // Clamp to reasonable range - optimized
    if (dopamineLevel_ < -1.0f) {
        dopamineLevel_ = -1.0f;
    } else if (dopamineLevel_ > 1.0f) {
        dopamineLevel_ = 1.0f;
    }
    
    // Apply to all synapses with eligibility traces - optimized
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // Early exit if eligibility trace is negligible
            if (eligibility <= 0.001f && eligibility >= -0.001f) {
                continue;
            }
            
            // Apply reward-modulated weight change
            float delta = eligibility * dopamineLevel_ * plasticityModifier_;
            syn->addToWeight(delta);
            
            // Decay eligibility trace
            syn->decayEligibilityTrace(0.1f);
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    if (plasticityFactor < 0.1f) {
        plasticityFactor = 0.1f;
    } else if (plasticityFactor > 2.0f) {
        plasticityFactor = 2.0f;
    }
    
    // Apply to STDP - optimized null check
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
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
