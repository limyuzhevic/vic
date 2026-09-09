#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <functional>

namespace nlm {

// AgentBrain implementation with improved design

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
    , sensoryNoveltyDecay_(0.99f) {
    
    if (brain_) {
        distributeNeurons();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::distributeNeurons() {
    // Group neurons by type for efficient access
    std::unordered_map<NeuronType, std::vector<Neuron*>> typeMap;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            auto& neurons = typeMap[type];
            
            for (Neuron* n : pop->getNeurons()) {
                neurons.push_back(n);
            }
        }
    }
    
    // Distribute motor neurons using a more robust approach
    auto distributeMotorNeurons = [&](const std::vector<Neuron*>& neurons) {
        motorForward_.reserve(neurons.size() / 6 + 1);
        motorBackward_.reserve(neurons.size() / 6 + 1);
        motorTurnLeft_.reserve(neurons.size() / 6 + 1);
        motorTurnRight_.reserve(neurons.size() / 6 + 1);
        motorInteract_.reserve(neurons.size() / 6 + 1);
        motorWait_.reserve(neurons.size() / 6 + 1);
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            switch (i % 6) {
                case 0: motorForward_.push_back(neurons[i]); break;
                case 1: motorBackward_.push_back(neurons[i]); break;
                case 2: motorTurnLeft_.push_back(neurons[i]); break;
                case 3: motorTurnRight_.push_back(neurons[i]); break;
                case 4: motorInteract_.push_back(neurons[i]); break;
                case 5: motorWait_.push_back(neurons[i]); break;
            }
        }
    };
    
    auto distributeSensoryNeurons = [&](const std::vector<Neuron*>& neurons) {
        sensoryVision_.reserve(neurons.size() / 4 + 1);
        sensoryTouch_.reserve(neurons.size() / 4 + 1);
        sensoryInternal_.reserve(neurons.size() / 4 + 1);
        sensoryProprioception_.reserve(neurons.size() / 4 + 1);
        
        for (size_t i = 0; i < neurons.size(); ++i) {
            switch (i % 4) {
                case 0: sensoryVision_.push_back(neurons[i]); break;
                case 1: sensoryTouch_.push_back(neurons[i]); break;
                case 2: sensoryInternal_.push_back(neurons[i]); break;
                case 3: sensoryProprioception_.push_back(neurons[i]); break;
            }
        }
    };
    
    // Distribute neurons by type
    if (typeMap.find(NeuronType::Motor) != typeMap.end()) {
        distributeMotorNeurons(typeMap[NeuronType::Motor]);
    }
    
    if (typeMap.find(NeuronType::Sensory) != typeMap.end()) {
        distributeSensoryNeurons(typeMap[NeuronType::Sensory]);
    }
    
    NLM_LOG_INFO("Neuron distribution completed: " +
                 std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait motor neurons; " +
                 std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
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
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: brain not initialized");
        return;
    }
    
    // Process each sensory modality efficiently
    processVisionInput( percept.getVision());
    processTouchInput(    percept.getTouch());
    processInternalInput( percept.getInternal());
    processProprioceptionInput( percept.getProprioception());
    
    // Compute novelty using optimized difference calculation
    computeNovelty( percept.getVision());
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        updateCuriosity();
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Vision input (256 values -> sensoryVision_ neurons)
    size_t minSize = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    // Touch input (8 values -> sensoryTouch_ neurons)
    size_t minSize = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    // Internal signals (4 values -> sensoryInternal_ neurons)
    size_t minSize = std::min(sensoryInternal_.size(), internal.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    // Compute novelty (difference from previous vision)
    if (!vision.empty() && vision.size() == previousVision_.size()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / static_cast<float>(vision.size());
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
}

void AgentBrain::updateCuriosity() {
    // Update curiosity based on novelty
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        return MotorCommand::Wait;
    }
    
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
        NeuronState restingState;
        
        for (Neuron* n : neurons) {
            if (n && n->getState(&restingState)) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getMembranePotential() - restingState.restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using structured approach
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& ca : commands) {
        if (ca.activity > bestActivity) {
            bestActivity = ca.activity;
            best = ca.cmd;
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
