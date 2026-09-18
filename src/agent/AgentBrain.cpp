#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    , curiosityThreshold_(0.3f)
    , explorationScale_(0.2f)
{
    NLM_LOG_INFO("AgentBrain constructor: brain_ = " + std::to_string(brain_ != nullptr));
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        distributeMotorNeurons(*brain_);
        distributeSensoryNeurons(*brain_);
    }
}
void AgentBrain::validateState() const {
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::validateState: brain_ is nullptr");
        return;
    }
    
    // Validate neuron groups are consistent
    if (!motorForward_.empty() && !brain_->getTotalNeuronCount()) {
        NLM_LOG_WARN("AgentBrain::validateState: Motor neurons exist but brain has no neurons");
    }
}

void AgentBrain::distributeMotorNeurons(const Brain& brain) {
    // More efficient and organized motor neuron distribution
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorLookLeft_.clear();
    motorLookRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    for (const auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type != NeuronType::Motor) continue;
            
            for (Neuron* n : pop->getNeurons()) {
                // Improved distribution algorithm for better balance
                distributeMotorNeuron(n);
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const Brain& brain) {
    // Clear existing sensory groups
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // More systematic distribution
    std::vector<std::vector<Neuron*>&> sensoryGroups = {
        sensoryVision_, sensoryTouch_, sensoryInternal_, sensoryProprioception_
    };
    
    for (const auto& region : brain.getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type != NeuronType::Sensory) continue;
            
            for (Neuron* n : pop->getNeurons()) {
                distributeSensoryNeuron(n, sensoryGroups);
            }
        }
    }
}

void AgentBrain::distributeMotorNeuron(Neuron* neuron) {
    // Improved distribution based on neuron properties and balance
    size_t forwardCount = motorForward_.size();
    size_t backwardCount = motorBackward_.size();
    size_t turnLeftCount = motorTurnLeft_.size();
    size_t turnRightCount = motorTurnRight_.size();
    size_t lookLeftCount = motorLookLeft_.size();
    size_t lookRightCount = motorLookRight_.size();
    size_t interactCount = motorInteract_.size();
    size_t waitCount = motorWait_.size();
    
    // Create total and find minimum for balanced distribution
    size_t total = forwardCount + backwardCount + turnLeftCount + turnRightCount +
                   lookLeftCount + lookRightCount + interactCount + waitCount;
    
    // Choose group based on current balance and neuron type
    size_t choice = total % 8;
    
    switch (choice) {
        case 0: motorForward_.push_back(neuron); break;
        case 1: motorBackward_.push_back(neuron); break;
        case 2: motorTurnLeft_.push_back(neuron); break;
        case 3: motorTurnRight_.push_back(neuron); break;
        case 4: motorLookLeft_.push_back(neuron); break;
        case 5: motorLookRight_.push_back(neuron); break;
        case 6: motorInteract_.push_back(neuron); break;
        case 7: motorWait_.push_back(neuron); break;
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron, std::vector<std::vector<Neuron*>&>& groups) {
    // Distribute sensory neurons based on their role/properties
    size_t totalGroups = groups.size();
    if (totalGroups == 0) return;
    
    // Simple round-robin with consideration for group sizes
    size_t targetGroup = groups.size(); // Will be used modulo
    for (size_t i = 0; i < totalGroups; ++i) {
        if (i == 0) { // Vision group
            sensoryVision_.push_back(neuron);
            return;
        }
    }
    // Fallback to first group if not distributed yet
    sensoryVision_.push_back(neuron);
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    NLM_LOG_DEBUG("Processing vision input: " + std::to_string(vision.size()) + " values");
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (!neuron) {
            NLM_LOG_WARN("AgentBrain::processVisionInput: Null neuron at index " + std::to_string(i));
            continue;
        }
        
        // Enhanced current injection with saturation
        float current = vision[i] * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f); // Prevent excessive currents
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (!neuron) continue;
        
        float current = touch[i] * 8.0f;
        current = std::clamp(current, 0.0f, 5.0f); // Only positive touch signals
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (!neuron) continue;
        
        float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (!neuron) continue;
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        current = std::clamp(current, -5.0f, 5.0f);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Improved novelty computation with better normalization
    float totalDiff = 0.0f;
    size_t validPixels = 0;
    
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
        validPixels++;
    }
    
    if (validPixels > 0) {
        noveltyLevel_ = totalDiff / static_cast<float>(validPixels);
    }
    
    // Apply decay
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next computation
    previousVision_ = vision;
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    NLM_LOG_DEBUG("Processing vision input: " + std::to_string(vision.size()) + " values");
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (!neuron) {
            NLM_LOG_WARN("AgentBrain::processVisionInput: Null neuron at index " + std::to_string(i));
            continue;
        }
        
        // Enhanced current injection with saturation
        float current = vision[i] * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f); // Prevent excessive currents
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (!neuron) continue;
        
        float current = touch[i] * 8.0f;
        current = std::clamp(current, 0.0f, 5.0f); // Only positive touch signals
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (!neuron) continue;
        
        float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (!neuron) continue;
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        current = std::clamp(current, -5.0f, 5.0f);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_INFO("AgentBrain::initialize: Starting initialization");
    
    // Validate brain exists
    validateState();
    
    // Initialize vision buffer
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization info
    size_t totalSensory = sensoryVision_.size() + sensoryTouch_.size() + 
                        sensoryInternal_.size() + sensoryProprioception_.size();
    NLM_LOG_INFO("AgentBrain initialized with " + std::to_string(totalSensory) + 
                 " sensory neurons and " + std::to_string(motorForward_.size() + 
                 motorBackward_.size() + motorTurnLeft_.size() + motorTurnRight_.size() +
                 motorLookLeft_.size() + motorLookRight_.size() + motorInteract_.size() +
                 motorWait_.size() + " motor neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action type
    return 8; // Added look left/right to motor output size
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::processSensoryInput: brain_ is nullptr, skipping");
        return;
    }
    
    NLM_LOG_DEBUG("AgentBrain::processSensoryInput: Processing sensory input");
    
    // Process vision input
    const auto& vision = percept.getVision();
    processVisionInput(vision);
    
    // Process touch input
    const auto& touch = percept.getTouch();
    processTouchInput(touch);
    
    // Process internal signals
    const auto& internal = percept.getInternal();
    processInternalInput(internal);
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    processProprioceptionInput(proprio);
    
    // Compute novelty and update curiosity
    computeNovelty(vision);
    
    // Update curiosity level
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain::decodeMotorCommand: brain_ is nullptr, returning Wait");
        return MotorCommand::Wait;
    }
    
    // Get decoded command from motor neurons
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration if enabled
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Prepare command array with all 8 possible motor commands
    struct MotorCommandInfo {
        MotorCommand cmd;
        float activity;
    };
    
    std::array<MotorCommandInfo, 8> commands = {
        MotorCommandInfo{MotorCommand::MoveForward, 0.0f},
        MotorCommandInfo{MotorCommand::MoveBackward, 0.0f},
        MotorCommandInfo{MotorCommand::TurnLeft, 0.0f},
        MotorCommandInfo{MotorCommand::TurnRight, 0.0f},
        MotorCommandInfo{MotorCommand::LookLeft, 0.0f},
        MotorCommandInfo{MotorCommand::LookRight, 0.0f},
        MotorCommandInfo{MotorCommand::Interact, 0.0f},
        MotorCommandInfo{MotorCommand::Wait, 0.0f}
    };
    
    // Calculate activity for each motor group
    calculateMotorActivity(commands);
    
    // Find command with highest activity
    return findBestMotorCommand(commands);
}

void AgentBrain::calculateMotorActivity(std::array<MotorCommandInfo, 8>& commands) const {
    // Helper lambda to calculate average activity for a neuron group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (const Neuron* neuron : neurons) {
            if (!neuron) continue;
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Calculate activities for all motor groups
    commands[0].activity = calcActivity(motorForward_);
    commands[1].activity = calcActivity(motorBackward_);
    commands[2].activity = calcActivity(motorTurnLeft_);
    commands[3].activity = calcActivity(motorTurnRight_);
    commands[4].activity = calcActivity(motorLookLeft_);
    commands[5].activity = calcActivity(motorLookRight_);
    commands[6].activity = calcActivity(motorInteract_);
    commands[7].activity = calcActivity(motorWait_);
}

MotorCommand AgentBrain::findBestMotorCommand(const std::array<MotorCommandInfo, 8>& commands) const {
    // Find command with highest activity level
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = commands[7].activity; // Default to Wait
    
    for (const auto& cmdInfo : commands) {
        if (cmdInfo.activity > bestActivity) {
            bestActivity = cmdInfo.activity;
            best = cmdInfo.cmd;
        }
    }
    
    // Only act if there's meaningful activity above threshold
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration with configurable parameters
    if (!curiosityEnabled_ || curiosityLevel_ <= 0.0f) {
        return defaultCmd;
    }
    
    // Calculate exploration probability based on curiosity level and scale
    float exploreChance = curiosityLevel_ * explorationScale_ * 0.5f;
    
    // Add some randomness to exploration
    exploreChance = std::min(exploreChance, 0.8f); // Cap at 80%
    
    // Generate random number for exploration decision
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < exploreChance) {
        // Return random motor command for exploration
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    // Improved novelty computation with better normalization
    float totalDiff = 0.0f;
    size_t validPixels = 0;
    
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
        validPixels++;
    }
    
    if (validPixels > 0) {
        noveltyLevel_ = totalDiff / static_cast<float>(validPixels);
    }
    
    // Apply decay
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next computation
    previousVision_ = vision;
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    NLM_LOG_DEBUG("Processing vision input: " + std::to_string(vision.size()) + " values");
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (!neuron) {
            NLM_LOG_WARN("AgentBrain::processVisionInput: Null neuron at index " + std::to_string(i));
            continue;
        }
        
        // Enhanced current injection with saturation
        float current = vision[i] * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f); // Prevent excessive currents
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (!neuron) continue;
        
        float current = touch[i] * 8.0f;
        current = std::clamp(current, 0.0f, 5.0f); // Only positive touch signals
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (!neuron) continue;
        
        float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
        current = std::clamp(current, -10.0f, 10.0f);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (!neuron) continue;
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        current = std::clamp(current, -5.0f, 5.0f);
        neuron->injectCurrent(current);
    }
}

MotorCommand AgentBrain::getRandomMotorCommand() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, 8);
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
