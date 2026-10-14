#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Helper function to create random distribution for neurons across groups
// When custom distribution is enabled, we use a more sophisticated approach
std::vector<size_t> createRandomDistribution(const NeuronGroupConfig& config, const std::vector<Neuron*>& neurons, NeuronType type) {
    std::vector<size_t> groupSizes;
    
    if (type == NeuronType::Motor) {
        for (const auto& group : config.motorGroups) {
            if (group.type == NeuronType::Motor) {
                groupSizes.push_back(group.count);
            }
        }
    } else if (type == NeuronType::Sensory) {
        for (const auto& group : config.sensoryGroups) {
            if (group.type == NeuronType::Sensory) {
                groupSizes.push_back(group.count);
            }
        }
    }
    
    // If we have neurons and groups, distribute them proportionally
    if (!neurons.empty() && !groupSizes.empty()) {
        std::vector<size_t> distribution;
        size_t totalAllocated = 0;
        
        for (size_t i = 0; i < groupSizes.size(); ++i) {
            if (totalAllocated < neurons.size()) {
                size_t count = std::min(groupSizes[i], neurons.size() - totalAllocated);
                distribution.push_back(count);
                totalAllocated += count;
            } else {
                distribution.push_back(0);
            }
        }
        
        // Fill any remaining neurons
        if (totalAllocated < neurons.size()) {
            // Distribute remaining neurons randomly
            std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
            std::vector<size_t> indices(neurons.size() - totalAllocated);
            std::iota(indices.begin(), indices.end(), totalAllocated);
            std::shuffle(indices.begin(), indices.end(), rng);
            
            for (size_t idx : indices) {
                // Find first non-full group
                for (size_t j = 0; j < distribution.size(); ++j) {
                    if (distribution[j] < groupSizes[j]) {
                        distribution[j]++;
                        break;
                    }
                }
            }
        }
        
        return distribution;
    }
    
    return std::vector<size_t>(config.motorGroups.size() + config.sensoryGroups.size(), 0);
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const NeuronGroupConfig& groupConfig)
    : brain_(brain)
    , groupConfig_(groupConfig)
    , customNeuronDistribution_(groupConfig.hasCustomGroups())
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
    NLM_LOG_INFO("Creating AgentBrain with " + (customNeuronDistribution_ ? "custom" : "default") + " neuron group configuration");
    
    // Initialize neuron groups based on configuration
    if (brain_) {
        // Clear legacy groups first
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
        
        // Create dynamic groups based on configuration
        std::vector<Neuron*> allNeurons;
        
        // Collect all neurons from brain
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (Neuron* n : pop->getNeurons()) {
                    allNeurons.push_back(n);
                }
            }
        }
        
        if (!allNeurons.empty()) {
            // Distribute neurons to motor groups
            auto motorDist = createRandomDistribution(groupConfig_, allNeurons, NeuronType::Motor);
            size_t neuronIndex = 0;
            
            for (const auto& group : groupConfig_.motorGroups) {
                if (group.type == NeuronType::Motor) {
                    motorGroups_.push_back({});  // Create group container
                    for (int i = 0; i < motorDist[neuronIndex] && neuronIndex < allNeurons.size(); ++i, ++neuronIndex) {
                        motorGroups_.back().push_back(allNeurons[neuronIndex]);
                    }
                    neuronIndex = std::min(neuronIndex, allNeurons.size());
                }
            }
            
            // Distribute neurons to sensory groups  
            auto sensoryDist = createRandomDistribution(groupConfig_, allNeurons, NeuronType::Sensory);
            size_t sensoryIndex = 0;
            
            for (const auto& group : groupConfig_.sensoryGroups) {
                if (group.type == NeuronType::Sensory) {
                    sensoryGroups_.push_back({});  // Create group container
                    for (int i = 0; i < sensoryDist[sensoryIndex] && sensoryIndex < allNeurons.size(); ++i, ++sensoryIndex) {
                        sensoryGroups_.back().push_back(allNeurons[sensoryIndex]);
                    }
                    sensoryIndex = std::min(sensoryIndex, allNeurons.size());
                }
            }
            
            NLM_LOG_INFO("Distributed " + std::to_string(allNeurons.size()) + " neurons into " +
                        std::to_string(motorGroups_.size()) + " motor and " + std::to_string(sensoryGroups_.size()) + " sensory groups");
            
            // Update legacy groups for backward compatibility
            updateLegacyGroups();
        }
    }
    
    NLM_LOG_INFO("AgentBrain created with " + std::to_string(motorGroups_.size()) + " motor groups and " + 
                 std::to_string(sensoryGroups_.size()) + " sensory groups");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::updateLegacyGroups() {
    // Reset legacy groups
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
    
    // Map new groups to legacy names if using custom configuration
    if (customNeuronDistribution_) {
        NLM_LOG_INFO("Updating legacy groups for backward compatibility");
        
        // Map motor groups to legacy names based on group names
        for (const auto& group : groupConfig_.motorGroups) {
            if (group.name == "Forward") {
                // Assign neurons from forward group to legacy forward
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorForward_.push_back(n);
                }
            } else if (group.name == "Backward") {
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorBackward_.push_back(n);
                }
            } else if (group.name == "TurnLeft") {
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorTurnLeft_.push_back(n);
                }
            } else if (group.name == "TurnRight") {
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorTurnRight_.push_back(n);
                }
            } else if (group.name == "Interact") {
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorInteract_.push_back(n);
                }
            } else if (group.name == "Wait") {
                for (Neuron* n : getNeuronsFromGroup(motorGroups_, group.name)) {
                    motorWait_.push_back(n);
                }
            }
        }
        
        // Map sensory groups to legacy names based on group names
        for (const auto& group : groupConfig_.sensoryGroups) {
            if (group.name == "Vision") {
                for (Neuron* n : getNeuronsFromGroup(sensoryGroups_, group.name)) {
                    sensoryVision_.push_back(n);
                }
            } else if (group.name == "Touch") {
                for (Neuron* n : getNeuronsFromGroup(sensoryGroups_, group.name)) {
                    sensoryTouch_.push_back(n);
                }
            } else if (group.name == "Internal") {
                for (Neuron* n : getNeuronsFromGroup(sensoryGroups_, group.name)) {
                    sensoryInternal_.push_back(n);
                }
            } else if (group.name == "Proprioception") {
                for (Neuron* n : getNeuronsFromGroup(sensoryGroups_, group.name)) {
                    sensoryProprioception_.push_back(n);
                }
            }
        }
    } else {
        // For default configuration, use the traditional mapping
        if (motorGroups_.size() >= 6) {
            motorForward_ = motorGroups_[0];
            motorBackward_ = motorGroups_[1];
            motorTurnLeft_ = motorGroups_[2];
            motorTurnRight_ = motorGroups_[3];
            motorInteract_ = motorGroups_[4];
            motorWait_ = motorGroups_[5];
        }
        
        if (sensoryGroups_.size() >= 4) {
            sensoryVision_ = sensoryGroups_[0];
            sensoryTouch_ = sensoryGroups_[1];
            sensoryInternal_ = sensoryGroups_[2];
            sensoryProprioception_ = sensoryGroups_[3];
        }
    }
}

std::vector<Neuron*> AgentBrain::getNeuronsFromGroup(const std::vector<std::vector<Neuron*>>& groups, const std::string& targetName) {
    for (const auto& group : groups) {
        if (!group.empty()) {
            // Find a neuron from this group to return (any neuron from the group)
            return group;
        }
    }
    return {};
}

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

} // namespace nlm
