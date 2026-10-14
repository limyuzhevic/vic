#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

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
    // Initialize motor and sensory neuron groups using safe approach
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Store motor neurons with their command associations
                    for (Neuron* n : pop->getNeurons()) {
                        // Determine motor command based on population type and neuron index
                        MotorCommand cmd = getMotorCommandFromIndex(motorNeuronsByType_.size());
                        motorNeuronsByType_.push_back({cmd, n});
                    }
                } else if (type == NeuronType::Sensory) {
                    // Store sensory neurons with their sensory type
                    for (Neuron* n : pop->getNeurons()) {
                        // Determine sensory type based on population and index
                        SensoryType sensoryType = getSensoryTypeFromIndex(sensoryNeuronsByType_.size());
                        sensoryNeuronsByType_.push_back({sensoryType, n});
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
                 std::to_string(sensoryNeuronsByType_.size()) + " sensory neurons, " +
                 std::to_string(motorNeuronsByType_.size()) + " motor neurons");
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
    
    // Process all stored sensory neurons using their IDs and types
    for (const auto& sensor : sensoryNeuronsByType_) {
        NeuronId neuronId = sensor.second;  // Get neuron ID
        SensoryType sensorType = sensor.first;  // Get sensory type
        
        // Find the neuron by ID
        Neuron* neuron = findNeuronById(neuronId);
        if (!neuron) continue;
        
        // Inject appropriate current based on sensory type and data
        float current = 0.0f;
        switch (sensorType) {
            case SensoryType::Vision:
                if (!vision_.empty()) {
                    current = vision_[neuronId.index()] * 5.0f;
                }
                break;
            case SensoryType::Touch:
                if (!touch_.empty()) {
                    current = touch_[neuronId.index()] * 8.0f;
                }
                break;
            case SensoryType::Internal:
                if (!internal_.empty()) {
                    current = (internal_[neuronId.index()] * 2.0f - 1.0f) * 5.0f;
                }
                break;
            case SensoryType::Proprioception:
                if (!proprioception_.empty()) {
                    current = (proprioception_[neuronId.index()] * 2.0f - 1.0f) * 3.0f;
                }
                break;
        }
        
        if (current != 0.0f) {
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision_.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision_.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision_[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision_.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision_;
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
    auto calcActivity = [](Neuron* neuron) -> float {
        if (!neuron) return 0.0f;
        // Use membrane potential deviation from rest as activity measure
        return std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
    };
    
    // Collect all motor neurons
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& motorNeuron : motorNeuronsByType_) {
        allMotorNeurons.push_back(motorNeuron.second);
    }
    
    if (allMotorNeurons.empty()) {
        return MotorCommand::Wait;
    }
    
    // Calculate average activity across all motor neurons
    float totalActivity = 0.0f;
    for (Neuron* neuron : allMotorNeurons) {
        totalActivity += calcActivity(neuron);
    }
    float avgActivity = totalActivity / allMotorNeurons.size();
    
    // Only act if there's meaningful activity
    if (avgActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    // Determine which command is most active based on distribution
    // This is a simplified version - in reality you'd want more sophisticated decoding
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, 0.0f},
        {MotorCommand::MoveBackward, 0.0f},
        {MotorCommand::TurnLeft, 0.0f},
        {MotorCommand::TurnRight, 0.0f},
        {MotorCommand::Interact, 0.0f},
        {MotorCommand::Wait, 0.0f}
    };
    
    // Distribute activity proportionally to improve decoding
    size_t motorCount = motorNeuronsByType_.size();
    for (size_t i = 0; i < motorCount; ++i) {
        commands[i % 6].activity += avgActivity * 0.1f;  // Distribute activity
    }
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
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

// Helper methods
MotorCommand AgentBrain::getMotorCommandFromIndex(size_t index) const {
    switch (index % 6) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        case 5: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

SensoryType AgentBrain::getSensoryTypeFromIndex(size_t index) const {
    switch (index % 4) {
        case 0: return SensoryType::Vision;
        case 1: return SensoryType::Touch;
        case 2: return SensoryType::Internal;
        case 3: return SensoryType::Proprioception;
        default: return SensoryType::Vision;
    }
}

Neuron* AgentBrain::findNeuronById(NeuronId neuronId) {
    if (!brain_) return nullptr;
    
    // Search through all regions and populations to find the neuron
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
    }
    
    return nullptr;
}

} // namespace nlm