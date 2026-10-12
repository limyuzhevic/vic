#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

// AgentBrain Implementation - Enhanced Organization

namespace nlm {

// AgentBrain Constructor: Initialize motor and sensory neuron groups
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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain created with null brain pointer");
        return;
    }
    
    // Initialize neuron groups for motor and sensory processing
    initializeNeuronGroups();
}

// AgentBrain Destructor
AgentBrain::~AgentBrain() = default;

// Initialize agent brain with world configuration
void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Initialize motor and sensory neuron groups
void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

// Distribute motor neurons to action groups
void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = getMotorGroupIndex();
        
        switch (idx % 6) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
        }
    }
}

// Distribute sensory neurons to sensory modalities
void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = getSensoryGroupIndex();
        
        switch (idx % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
    }
}

// Get next motor neuron group index
size_t AgentBrain::getMotorGroupIndex() const {
    return motorForward_.size() + motorBackward_.size() + 
           motorTurnLeft_.size() + motorTurnRight_.size() +
           motorInteract_.size() + motorWait_.size();
}

// Get next sensory neuron group index
size_t AgentBrain::getSensoryGroupIndex() const {
    return sensoryVision_.size() + sensoryTouch_.size() +
           sensoryInternal_.size() + sensoryProprioception_.size();
}

// Process sensory percept and inject into brain
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARN("Cannot process sensory input - brain not initialized");
        return;
    }
    
    // Process each sensory modality
    processVisionInput(percept.getVision());
    processTouchInput(percept.getTouch());
    processInternalInput(percept.getInternal());
    processProprioceptiveInput(percept.getProprioception());
    
    // Compute novelty from vision changes
    computeNovelty(percept.getVision());
    
    // Update curiosity based on novelty and prediction error
    updateCuriosity();
}

// Process vision input
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    const auto& visionNeurons = sensoryVision_;
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            float current = vision[i] * 5.0f;
            visionNeurons[i]->injectCurrent(current);
        }
    }
}

// Process touch input
void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    const auto& touchNeurons = sensoryTouch_;
    for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
        if (touchNeurons[i]) {
            float current = touch[i] * 8.0f;
            touchNeurons[i]->injectCurrent(current);
        }
    }
}

// Process internal signals
void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    const auto& internalNeurons = sensoryInternal_;
    for (size_t i = 0; i < internalNeurons.size() && i < intern.size(); ++i) {
        if (internalNeurons[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            internalNeurons[i]->injectCurrent(current);
        }
    }
}

// Process proprioception
void AgentBrain::processProprioceptiveInput(const std::vector<float>& proprio) {
    const auto& proprioceptiveNeurons = sensoryProprioception_;
    for (size_t i = 0; i < proprioceptiveNeurons.size() && i < proprio.size(); ++i) {
        if (proprioceptiveNeurons[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            proprioceptiveNeurons[i]->injectCurrent(current);
        }
    }
}

// Compute novelty from vision changes
void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and apply decay
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Update for next time
    previousVision_ = vision;
}

// Update curiosity based on novelty and prediction error
void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) return;
    
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

// Calculate motor command from neural activity
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARN("Cannot decode motor command - brain not initialized");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration if enabled
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

// Decode motor command from motor neuron activity
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Calculate activity for each motor group
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find command with highest activity
    return findBestMotorCommand(forwardAct, backwardAct, leftAct, rightAct, 
                               interactAct, waitAct);
}

// Find best motor command based on activity levels
MotorCommand AgentBrain::findBestMotorCommand(float forward, float backward, 
                                             float left, float right, 
                                             float interact, float wait) {
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forward},
        {MotorCommand::MoveBackward, backward},
        {MotorCommand::TurnLeft, left},
        {MotorCommand::TurnRight, right},
        {MotorCommand::Interact, interact},
        {MotorCommand::Wait, wait}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = wait;  // Default to wait
    
    for (const auto& cmd : commands) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            best = cmd.cmd;
        }
    }
    
    // Only act if there's meaningful neural activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// Select motor command with curiosity/exploration
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!curiosityEnabled_ || curiosityLevel_ <= 0.5f) {
        return defaultCmd;
    }
    
    // Calculate exploration probability
    float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random exploration
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Choose random action for exploration
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

// Get random motor command for exploration
MotorCommand AgentBrain::getRandomMotorCommand() {
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

// Apply reward modulation and learning
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error signal
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with exponential moving average
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal based on prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated plasticity to all synapses
    applyRewardToSynapses(dopamineLevel_);
    
    // Modulate plasticity rules based on neuromodulation
    modulatePlasticityRules(dopamineLevel_);
}

// Apply reward-modulated weight changes to synapses
void AgentBrain::applyRewardToSynapses(float dopamine) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamine * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

// Modulate plasticity rules based on neuromodulation level
void AgentBrain::modulatePlasticityRules(float dopamine) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamine;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP rule if available
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// Update development system
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Determine developmental stage based on age
    DevelopmentalStage stage = determineDevelopmentalStage();
    brain_->setDevelopmentalStage(stage);
    
    // Update plasticity modifier based on stage
    updatePlasticityForDevelopment(stage);
    
    // Apply structural plasticity changes
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity();
    }
}

// Determine developmental stage based on age
DevelopmentalStage AgentBrain::determineDevelopmentalStage() {
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        return DevelopmentalStage::Initial;
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        return DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        return DevelopmentalStage::Maturation;
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        return DevelopmentalStage::Adult;
    }
}

// Update structural plasticity based on developmental stage
void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    // Higher synaptogenesis in early development
    float synRate = 0.0001f * plasticityModifier_;
    float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

// Reset agent for new episode
void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous sensory state
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
