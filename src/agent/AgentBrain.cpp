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
    , curiosityMemoryEnabled_(true)
    , explorationBiasEnabled_(false)
    , sensoryNoveltyDecay_(0.99f)
    , noveltyHistoryDecay_(0.95f)
    , lastLearningTime_(0.0)
    , explorationRate_(0.1f)
    , exploitationRate_(0.9f)
{
    // Initialize previous sensory buffers
    // Note: These will be resized in initialize() method
    previousVision_.clear();
    previousTouch_.clear();
    previousInternal_.clear();
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        organizeNeurons();
    }
}

void AgentBrain::organizeNeurons() {
    if (!brain_) return;
    
    // Clear existing groups
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
    
    // Organize neurons by type and function for better performance
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            const std::string& name = pop->getName();
            
            if (type == NeuronType::Motor) {
                organizeMotorPopulation(pop, name);
            } else if (type == NeuronType::Sensory) {
                organizeSensoryPopulation(pop, name);
            }
        }
    }
}

void AgentBrain::organizeMotorPopulation(std::shared_ptr<NeuralPopulation> pop, const std::string& name) {
    for (Neuron* n : pop->getNeurons()) {
        // More systematic distribution based on population name
        if (name.find("forward") != std::string::npos || 
            name.find("move") != std::string::npos) {
            motorForward_.push_back(n);
        } else if (name.find("backward") != std::string::npos) {
            motorBackward_.push_back(n);
        } else if (name.find("left") != std::string::npos) {
            motorTurnLeft_.push_back(n);
        } else if (name.find("right") != std::string::npos) {
            motorTurnRight_.push_back(n);
        } else if (name.find("interact") != std::string::npos || 
                   name.find("grab") != std::string::npos) {
            motorInteract_.push_back(n);
        } else if (name.find("wait") != std::string::npos || 
                   name.find("idle") != std::string::npos) {
            motorWait_.push_back(n);
        } else {
            // Default fallback - distribute evenly
            size_t totalMotor = motorForward_.size() + motorBackward_.size() +
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
            size_t idx = totalMotor % 6;
            switch (idx) {
                case 0: motorForward_.push_back(n); break;
                case 1: motorBackward_.push_back(n); break;
                case 2: motorTurnLeft_.push_back(n); break;
                case 3: motorTurnRight_.push_back(n); break;
                case 4: motorInteract_.push_back(n); break;
                case 5: motorWait_.push_back(n); break;
            }
        }
    }
}

void AgentBrain::organizeSensoryPopulation(std::shared_ptr<NeuralPopulation> pop, const std::string& name) {
    for (Neuron* n : pop->getNeurons()) {
        // Organize based on sensory modality
        if (name.find("vision") != std::string::npos || 
            name.find("visual") != std::string::npos) {
            sensoryVision_.push_back(n);
        } else if (name.find("touch") != std::string::npos || 
                   name.find("proprio") != std::string::npos) {
            sensoryTouch_.push_back(n);
        } else if (name.find("internal") != std::string::npos || 
                   name.find("homeostatic") != std::string::npos) {
            sensoryInternal_.push_back(n);
        } else if (name.find("proprio") != std::string::npos) {
            sensoryProprioception_.push_back(n);
        } else {
            // Default fallback
            size_t totalSensory = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
            size_t idx = totalSensory % 4;
            switch (idx) {
                case 0: sensoryVision_.push_back(n); break;
                case 1: sensoryTouch_.push_back(n); break;
                case 2: sensoryInternal_.push_back(n); break;
                case 3: sensoryProprioception_.push_back(n); break;
            }
        }
    }
}

size_t AgentBrain::getMotorGroupCount(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return motorForward_.size();
        case MotorCommand::MoveBackward: return motorBackward_.size();
        case MotorCommand::TurnLeft: return motorTurnLeft_.size();
        case MotorCommand::TurnRight: return motorTurnRight_.size();
        case MotorCommand::Interact: return motorInteract_.size();
        case MotorCommand::Wait: return motorWait_.size();
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
        default: return 0;
    }
}

size_t AgentBrain::getSensoryGroupCount() const {
    return sensoryVision_.size() + sensoryTouch_.size() + 
           sensoryInternal_.size() + sensoryProprioception_.size();
}

size_t AgentBrain::getTotalMotorSpikeCount() const {
    size_t total = 0;
    for (Neuron* n : motorForward_) total += n->getState().spikeCount;
    for (Neuron* n : motorBackward_) total += n->getState().spikeCount;
    for (Neuron* n : motorTurnLeft_) total += n->getState().spikeCount;
    for (Neuron* n : motorTurnRight_) total += n->getState().spikeCount;
    for (Neuron* n : motorInteract_) total += n->getState().spikeCount;
    for (Neuron* n : motorWait_) total += n->getState().spikeCount;
    return total;
}

size_t AgentBrain::getTotalSensorySpikeCount() const {
    size_t total = 0;
    for (Neuron* n : sensoryVision_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryTouch_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryInternal_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryProprioception_) total += n->getState().spikeCount;
    return total;
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

void AgentBrain::updateExplorationRates(double timestep) {
    // Adaptive exploration-exploitation based on learning progress
    double ageFactor = developmentalAge_ / 900.0; // Normalize to developmental stages
    
    // Early development: more exploration
    if (developmentalAge_ < 60.0) {
        explorationRate_ = 0.3f;
        exploitationRate_ = 0.7f;
    } else if (developmentalAge_ < 300.0) {
        explorationRate_ = 0.2f;
        exploitationRate_ = 0.8f;
    } else if (developmentalAge_ < 900.0) {
        explorationRate_ = 0.15f;
        exploitationRate_ = 0.85f;
    } else {
        explorationRate_ = 0.05f + 0.15f * (1.0 - ageFactor); // Decrease exploration with age
        exploitationRate_ = 0.95f - 0.15f * (1.0 - ageFactor);
    }
    
    // Adjust based on curiosity and novelty
    if (curiosityMemoryEnabled_) {
        float curiosityFactor = std::min(1.0f, curiosityLevel_ / 0.5f);
        explorationRate_ += curiosityFactor * 0.2f * (1.0f - explorationRate_);
        explorationRate_ = std::clamp(explorationRate_, 0.0f, 1.0f);
        exploitationRate_ = 1.0f - explorationRate_;
    }
    
    // Apply exploration bias if enabled
    if (explorationBiasEnabled_) {
        // Bias towards exploration for diversity
        explorationRate_ = std::min(explorationRate_ * 1.5f, 1.0f);
        exploitationRate_ = 1.0f - explorationRate_;
    }
    
    // Gradually converge to target rates
    static const float convergenceRate = 0.1f;
    explorationRate_ = explorationRate_ * (1.0f - convergenceRate) + 
                       targetExplorationRate_ * convergenceRate;
    exploitationRate_ = exploitationRate_ * (1.0f - convergenceRate) + 
                        targetExploitationRate_ * convergenceRate;
}

void AgentBrain::applyAdvancedLearning(double timestep) {
    lastLearningTime_ += timestep;
    
    // More sophisticated learning based on neuromodulation
    if (rewardModulationEnabled_) {
        // Enhanced reward modulation with eligibility traces
        applyEnhancedRewardModulation(timestep);
    }
    
    // Adaptive synaptic strengthening based on co-activity patterns
    if (structuralPlasticityEnabled_) {
        applyStructuralPlasticity(timestep);
    }
    
    // Development-driven changes
    if (developmentEnabled_) {
        updateDevelopment(timestep);
    }
}

void AgentBrain::applyEnhancedRewardModulation(double timestep) {
    if (!brain_) return;
    
    // Multiple time-scale learning
    const float immediateLearning = 0.3f;
    const float mediumLearning = 0.5f;
    const float longLearning = 0.2f;
    
    // Immediate reward effect
    float immediateDelta = predictionError_ * immediateLearning * plasticityModifier_;
    
    // Medium-term effect based on eligibility traces
    float mediumDelta = 0.0f;
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            if (std::abs(eligibility) > 0.001f) {
                mediumDelta += eligibility * dopamineLevel_ * plasticityModifier_ * mediumLearning;
            }
        }
    }
    mediumDelta /= std::max<size_t>(brain_->getTotalSynapseCount(), 1);
    
    // Apply immediate changes
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            syn->addToWeight(immediateDelta * (syn->getWeight() / std::max<float>(syn->getWeight(), 0.1f)));
        }
    }
    
    // Modulate plasticity based on learning history
    float learningHistoryFactor = std::min(1.0f, static_cast<float>(lastLearningTime_) / 60.0f);
    float plasticityAdjustment = 0.5f + 0.5f * learningHistoryFactor;
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float ltpWeight = 0.01f * plasticityAdjustment;
        float ltdWeight = 0.012f * plasticityAdjustment;
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
}

void AgentBrain::applyStructuralPlasticity(double timestep) {
    if (!brain_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    // Activity-dependent structural changes
    float totalActivity = getTotalMotorSpikeCount() + getTotalSensorySpikeCount();
    float activityRatio = totalActivity / std::max<size_t>(brain_->getTotalSpikeCount(), 1);
    
    // Synaptogenesis: more connections when there's high activity
    float synRate = 0.0001f * (1.0f + activityRatio) * plasticityModifier_;
    sp->setSynaptogenesisRate(synRate);
    
    // Pruning: more pruning when there's predictable activity (low novelty)
    float pruneRate = 0.00001f * (1.5f - activityRatio) * (1.0f - noveltyLevel_) * (2.0f - plasticityModifier_);
    sp->setPruningRate(pruneRate);
}

void AgentBrain::setTargetExplorationRates(float exploration, float exploitation) {
    targetExplorationRate_ = std::clamp(exploration, 0.0f, 1.0f);
    targetExploitationRate_ = std::clamp(exploitation, 0.0f, 1.0f);
    // Ensure they sum to 1.0
    float total = targetExplorationRate_ + targetExploitationRate_;
    if (total > 0.0f) {
        targetExplorationRate_ /= total;
        targetExploitationRate_ /= total;
    } else {
        targetExplorationRate_ = 0.1f;
        targetExploitationRate_ = 0.9f;
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getAverageSensoryGain() const {
    float totalGain = 0.0f;
    int count = 0;
    
    // Calculate average sensory gain from neuron activities
    for (Neuron* n : sensoryVision_) {
        float gain = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        totalGain += gain;
        count++;
    }
    for (Neuron* n : sensoryTouch_) {
        float gain = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        totalGain += gain;
        count++;
    }
    for (Neuron* n : sensoryInternal_) {
        float gain = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        totalGain += gain;
        count++;
    }
    for (Neuron* n : sensoryProprioception_) {
        float gain = std::abs(n->getState().membranePotential - n->getState().restingPotential);
        totalGain += gain;
        count++;
    }
    
    return count > 0 ? totalGain / count : 0.0f;
}

std::vector<float> AgentBrain::getMotorBiasVector(int index) const {
    std::vector<float> biasVector;
    
    switch (index) {
        case 0: // Movement direction bias
            biasVector.push_back(motorForward_.size() > 0 ? 1.0f : 0.0f);
            biasVector.push_back(motorBackward_.size() > 0 ? 1.0f : 0.0f);
            break;
        case 1: // Turning direction bias
            biasVector.push_back(motorTurnLeft_.size() > 0 ? 1.0f : 0.0f);
            biasVector.push_back(motorTurnRight_.size() > 0 ? 1.0f : 0.0f);
            break;
        case 2: // Interaction bias
            biasVector.push_back(motorInteract_.size() > 0 ? 1.0f : 0.0f);
            biasVector.push_back(motorWait_.size() > 0 ? 1.0f : 0.0f);
            break;
        default:
            break;
    }
    
    return biasVector;
}

size_t AgentBrain::getTotalSpikeCount() const {
    size_t total = 0;
    
    // Count spikes from all neuron groups
    for (Neuron* n : motorForward_) total += n->getState().spikeCount;
    for (Neuron* n : motorBackward_) total += n->getState().spikeCount;
    for (Neuron* n : motorTurnLeft_) total += n->getState().spikeCount;
    for (Neuron* n : motorTurnRight_) total += n->getState().spikeCount;
    for (Neuron* n : motorInteract_) total += n->getState().spikeCount;
    for (Neuron* n : motorWait_) total += n->getState().spikeCount;
    
    // Count spikes from all sensory neuron groups
    for (Neuron* n : sensoryVision_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryTouch_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryInternal_) total += n->getState().spikeCount;
    for (Neuron* n : sensoryProprioception_) total += n->getState().spikeCount;
    
    return total;
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
