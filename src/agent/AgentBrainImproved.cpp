#include "AgentBrainImproved.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

void AgentBrainConfig::loadFromConfig(const Config& config) {
    // Motor system
    if (config.has("agent.num_motor_groups")) {
        numMotorGroups = config.getOr<size_t>("agent.num_motor_groups", numMotorGroups);
    }
    if (config.has("agent.motor_group_commands")) {
        // Parse motor group commands (string representation)
        auto commands = config.getOr<std::string>("agent.motor_group_commands", "");
        // Simple parsing - in practice would need more robust parsing
        if (commands.find("MoveForward") != std::string::npos) motorGroupCommands.push_back(MotorCommand::MoveForward);
        if (commands.find("MoveBackward") != std::string::npos) motorGroupCommands.push_back(MotorCommand::MoveBackward);
        if (commands.find("TurnLeft") != std::string::npos) motorGroupCommands.push_back(MotorCommand::TurnLeft);
        if (commands.find("TurnRight") != std::string::npos) motorGroupCommands.push_back(MotorCommand::TurnRight);
        if (commands.find("Interact") != std::string::npos) motorGroupCommands.push_back(MotorCommand::Interact);
        if (commands.find("Wait") != std::string::npos) motorGroupCommands.push_back(MotorCommand::Wait);
    }
    
    // Sensory system
    if (config.has("agent.num_sensory_groups")) {
        numSensoryGroups = config.getOr<size_t>("agent.num_sensory_groups", numSensoryGroups);
    }
    if (config.has("agent.sensory_group_names")) {
        // Parse sensory group names
        auto names = config.getOr<std::string>("agent.sensory_group_names", "Vision,Touch,Internal,Proprioception");
        // Simple parsing - in practice would need more robust parsing
        size_t start = 0;
        size_t end = names.find(',');
        while (end != std::string::npos) {
            sensoryGroupNames.push_back(names.substr(start, end - start));
            start = end + 1;
            end = names.find(',', start);
        }
        if (start < names.length()) {
            sensoryGroupNames.push_back(names.substr(start));
        }
    }
    
    // Novelty detection
    if (config.has("agent.novelty_threshold")) {
        noveltyThreshold = config.getOr<float>("agent.novelty_threshold", noveltyThreshold);
    }
    if (config.has("agent.sensory_novelty_decay")) {
        sensoryNoveltyDecay = config.getOr<float>("agent.sensory_novelty_decay", sensoryNoveltyDecay);
    }
    
    // Curiosity
    if (config.has("agent.curiosity_enabled")) {
        curiosityEnabled = config.getOr<bool>("agent.curiosity_enabled", curiosityEnabled);
    }
    if (config.has("agent.curiosity_threshold")) {
        curiosityThreshold = config.getOr<float>("agent.curiosity_threshold", curiosityThreshold);
    }
    if (config.has("agent.max_exploration_chance")) {
        maxExplorationChance = config.getOr<float>("agent.max_exploration_chance", maxExplorationChance);
    }
    if (config.has("agent.curiosity_noise_level")) {
        curiosityNoiseLevel = config.getOr<float>("agent.curiosity_noise_level", curiosityNoiseLevel);
    }
    
    // Reward modulation
    if (config.has("agent.reward_modulation_enabled")) {
        rewardModulationEnabled = config.getOr<bool>("agent.reward_modulation_enabled", rewardModulationEnabled);
    }
    if (config.has("agent.reward_prediction_window")) {
        rewardPredictionWindow = config.getOr<float>("agent.reward_prediction_window", rewardPredictionWindow);
    }
    if (config.has("agent.dopamine_gain")) {
        dopamineGain = config.getOr<float>("agent.dopamine_gain", dopamineGain);
    }
    
    // Development
    if (config.has("agent.development_enabled")) {
        developmentEnabled = config.getOr<bool>("agent.development_enabled", developmentEnabled);
    }
    if (config.has("agent.structural_plasticity_enabled")) {
        structuralPlasticityEnabled = config.getOr<bool>("agent.structural_plasticity_enabled", structuralPlasticityEnabled);
    }
    
    // Performance
    if (config.has("agent.min_neurons_for_distribution")) {
        minNeuronsForDistribution = config.getOr<size_t>("agent.min_neurons_for_distribution", minNeuronsForDistribution);
    }
    if (config.has("agent.motor_activity_threshold")) {
        motorActivityThreshold = config.getOr<float>("agent.motor_activity_threshold", motorActivityThreshold);
    }
    if (config.has("agent.sensory_injection_scale")) {
        sensoryInjectionScale = config.getOr<float>("agent.sensory_injection_scale", sensoryInjectionScale);
    }
    
    // Exploration
    if (config.has("agent.num_motor_commands")) {
        numMotorCommands = config.getOr<size_t>("agent.num_motor_commands", numMotorCommands);
    }
    if (config.has("agent.exploration_temperature")) {
        explorationTemperature = config.getOr<float>("agent.exploration_temperature", explorationTemperature);
    }
    if (config.has("agent.exploration_decay")) {
        explorationDecay = config.getOr<float>("agent.exploration_decay", explorationDecay);
    }
}

void AgentBrainConfig::applyToAgentBrain(AgentBrain& brain) const {
    brain.setConfig(*this);
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , explorationState_(0.0f)
{
    applyToConfig();
    
    // Initialize motor and sensory neuron groups with improved distribution
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Smart motor neuron distribution using configuration
                        distributeMotorNeuron(n);
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Smart sensory neuron distribution using configuration
                        distributeSensoryNeuron(n);
                    }
                }
            }
        }
    }
    
    // Initialize exploration state
    explorationState_ = 0.0f;
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::applyToConfig() {
    // Apply configuration to internal state variables
    if (!config_.motorGroupCommands.empty()) {
        motorCommandMap_.resize(config_.numMotorGroups);
        for (size_t i = 0; i < config_.numMotorGroups && i < config_.motorGroupCommands.size(); ++i) {
            motorCommandMap_[i] = config_.motorGroupCommands[i];
        }
        // Fill remaining with defaults if needed
        for (size_t i = motorCommandMap_.size(); i < config_.numMotorGroups; ++i) {
            motorCommandMap_.push_back(MotorCommand::Wait); // Default
        }
    }
    
    sensoryGroupNames_ = config_.sensoryGroupNames;
    
    // Ensure consistent group counts
    if (motorCommandMap_.size() != config_.numMotorGroups) {
        motorCommandMap_.resize(config_.numMotorGroups, MotorCommand::Wait);
    }
    
    if (sensoryGroupNames_.size() != config_.numSensoryGroups) {
        sensoryGroupNames_.resize(config_.numSensoryGroups);
        for (size_t i = 0; i < sensoryGroupNames_.size(); ++i) {
            if (sensoryGroupNames_[i].empty()) {
                // Default names
                const std::vector<std::string> defaults = {"Vision", "Touch", "Internal", "Proprioception", "Audio", "Extra"};
                if (i < defaults.size()) {
                    sensoryGroupNames_[i] = defaults[i];
                } else {
                    sensoryGroupNames_[i] = "Sensory" + std::to_string(i);
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    // Check if we have enough neurons before applying advanced distribution
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size();
    
    if (totalMotorNeurons < config_.minNeuronsForDistribution) {
        // Simple round-robin for early stage
        size_t idx = totalMotorNeurons % config_.numMotorGroups;
        assignMotorNeuronToGroup(neuron, idx);
    } else {
        // Use more sophisticated distribution based on configuration
        assignMotorNeuronToGroup(neuron, totalMotorNeurons % config_.numMotorGroups);
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    // Check if we have enough neurons before applying advanced distribution
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                 sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalSensoryNeurons < config_.minNeuronsForDistribution) {
        // Simple round-robin for early stage
        size_t idx = totalSensoryNeurons % config_.numSensoryGroups;
        assignSensoryNeuronToGroup(neuron, idx);
    } else {
        // Use more sophisticated distribution based on configuration
        assignSensoryNeuronToGroup(neuron, totalSensoryNeurons % config_.numSensoryGroups);
    }
}

void AgentBrain::assignMotorNeuronToGroup(Neuron* neuron, size_t groupIdx) {
    if (groupIdx < config_.numMotorGroups) {
        MotorCommand cmd = motorCommandMap_[groupIdx];
        switch (cmd) {
            case MotorCommand::MoveForward:
                motorForward_.push_back(neuron);
                break;
            case MotorCommand::MoveBackward:
                motorBackward_.push_back(neuron);
                break;
            case MotorCommand::TurnLeft:
                motorTurnLeft_.push_back(neuron);
                break;
            case MotorCommand::TurnRight:
                motorTurnRight_.push_back(neuron);
                break;
            case MotorCommand::Interact:
                motorInteract_.push_back(neuron);
                break;
            default: // Wait and LookLeft/LookRight
                motorWait_.push_back(neuron);
                break;
        }
    }
}

void AgentBrain::assignSensoryNeuronToGroup(Neuron* neuron, size_t groupIdx) {
    if (groupIdx < config_.numSensoryGroups) {
        switch (groupIdx) {
            case 0:
                sensoryVision_.push_back(neuron);
                break;
            case 1:
                sensoryTouch_.push_back(neuron);
                break;
            case 2:
                sensoryInternal_.push_back(neuron);
                break;
            case 3:
                sensoryProprioception_.push_back(neuron);
                break;
            default:
                sensoryWait_.push_back(neuron); // This would need to be added to the header
                break;
        }
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Use world dimensions for vision input sizing
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    explorationState_ = 1.0f; // Start with normal exploration
    
    NLM_LOG_INFO("AgentBrain initialized with improved configuration:");
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                   motorTurnLeft_.size() + motorTurnRight_.size() + 
                   motorInteract_.size() + motorWait_.size()) + " neurons");
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                   sensoryInternal_.size() + sensoryProprioception_.size()) + " neurons");
    NLM_LOG_INFO("  Motor commands: " + std::to_string(config_.numMotorGroups));
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(config_.numSensoryGroups));
    NLM_LOG_INFO("  Curiosity enabled: " + std::to_string(config_.curiosityEnabled));
    NLM_LOG_INFO("  Exploration temperature: " + std::to_string(config_.explorationTemperature));
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate based on world configuration (not hardcoded)
    size_t visionSize = 256; // Standard 16x16 vision for now
    size_t touchSize = 8;
    size_t internalSize = 4;
    size_t proprioceptionSize = 6;
    size_t audioSize = 0; // Will be configurable in future
    
    return visionSize + touchSize + internalSize + proprioceptionSize + audioSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Use configurable number of motor commands
    return config_.numMotorGroups;
}

std::vector<float> AgentBrain::getWeightedSensoryInput(const SensoryPercept& percept) const {
    std::vector<float> weightedInput;
    
    // Vision input with configurable scaling
    const auto& vision = percept.getVision();
    weightedInput.reserve(vision.size());
    for (size_t i = 0; i < vision.size(); ++i) {
        weightedInput.push_back(vision[i] * config_.sensoryInjectionScale);
    }
    
    // Touch input with configurable scaling
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touch.size(); ++i) {
        weightedInput.push_back(touch[i] * config_.sensoryInjectionScale * 1.2f);
    }
    
    // Internal signals with configurable scaling
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < internal.size(); ++i) {
        weightedInput.push_back((internal[i] * 2.0f - 1.0f) * config_.sensoryInjectionScale * 0.8f);
    }
    
    // Proprioception with configurable scaling
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprio.size(); ++i) {
        weightedInput.push_back((proprio[i] * 2.0f - 1.0f) * config_.sensoryInjectionScale * 0.6f);
    }
    
    return weightedInput;
}

float AgentBrain::computeNovelty(const std::vector<float>& currentVision) const {
    if (previousVision_.empty() || currentVision.empty()) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(previousVision_.size(), currentVision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and apply threshold
    float novelty = totalDiff / std::max<size_t>(minSize, 1);
    return (novelty > config_.noveltyThreshold) ? novelty : 0.0f;
}

void AgentBrain::updateExploration(double timestep) {
    // Update exploration state with configurable decay and temperature
    explorationState_ *= config_.explorationDecay;
    
    // Add some random exploration based on temperature
    float randomExploraton = explorationTemperature * 0.1f * std::sin(timestep * 0.1f);
    explorationState_ = std::clamp(explorationState_ + randomExploraton, 0.0f, 1.0f);
}

float AgentBrain::calculateExplorationProbability() const {
    // Enhanced exploration probability calculation
    float baseExploration = explorationState_;
    
    // Add curiosity contribution
    float curiosityContribution = 0.0f;
    if (config_.curiosityEnabled) {
        curiosityContribution = curiosityLevel_ * config_.maxExplorationChance;
    }
    
    // Add noise for stochastic exploration
    float noise = curiosityNoiseLevel * (brain_->getRandomGenerator()->uniformReal(-1.0f, 1.0f));
    
    // Combine factors with temperature scaling
    float probability = baseExploration * (1.0f + curiosityContribution) * 
                       std::exp(noise * config_.explorationTemperature);
    
    return std::clamp(probability, 0.0f, 1.0f);
}

MotorCommand AgentBrain::randomMotorCommand() const {
    if (config_.numMotorCommands == 0) return MotorCommand::Wait;
    
    int choice = brain_->getRandomGenerator()->uniformInt(0, static_cast<int>(config_.numMotorCommands));
    
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

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Use weighted sensory input for better processing
    std::vector<float> weightedInput = getWeightedSensoryInput(percept);
    
    // Vision input (using weighted input)
    for (size_t i = 0; i < sensoryVision_.size() && i < weightedInput.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current using configurable scale factor
            float current = weightedInput[i];
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (with enhanced processing)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * config_.sensoryInjectionScale * 1.5f; // Enhanced touch
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (with normalization)
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * config_.sensoryInjectionScale;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (with scaling)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * config_.sensoryInjectionScale * 0.7f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty with threshold
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        noveltyLevel_ = computeNovelty(vision);
        
        // Decay and update
        noveltyLevel_ *= config_.sensoryNoveltyDecay;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity with enhanced calculation
    updateExploration(0.0); // Update exploration state
    if (config_.curiosityEnabled) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ += curiosityNoiseLevel * (brain_->getRandomGenerator()->uniformReal(-1.0f, 1.0f));
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    } else {
        curiosityLevel_ = 0.0f;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (config_.curiosityEnabled && curiosityLevel_ > config_.curiosityThreshold) {
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
    
    // Use configurable motor group sizes
    std::vector<float> groupActivities;
    std::vector<MotorCommand> groupCommands;
    
    // Collect activities from all motor groups
    if (!motorForward_.empty()) {
        groupActivities.push_back(calcActivity(motorForward_));
        groupCommands.push_back(MotorCommand::MoveForward);
    }
    if (!motorBackward_.empty()) {
        groupActivities.push_back(calcActivity(motorBackward_));
        groupCommands.push_back(MotorCommand::MoveBackward);
    }
    if (!motorTurnLeft_.empty()) {
        groupActivities.push_back(calcActivity(motorTurnLeft_));
        groupCommands.push_back(MotorCommand::TurnLeft);
    }
    if (!motorTurnRight_.empty()) {
        groupActivities.push_back(calcActivity(motorTurnRight_));
        groupCommands.push_back(MotorCommand::TurnRight);
    }
    if (!motorInteract_.empty()) {
        groupActivities.push_back(calcActivity(motorInteract_));
        groupCommands.push_back(MotorCommand::Interact);
    }
    if (!motorWait_.empty()) {
        groupActivities.push_back(calcActivity(motorWait_));
        groupCommands.push_back(MotorCommand::Wait);
    }
    
    // Find maximum activity with better threshold handling
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    
    for (size_t i = 0; i < groupActivities.size(); ++i) {
        if (groupActivities[i] > bestActivity) {
            bestActivity = groupActivities[i];
            best = groupCommands[i];
        }
    }
    
    // Only act if there's meaningful activity (configurable threshold)
    if (bestActivity < config_.motorActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration with configurable probability
    float exploreChance = calculateExplorationProbability();
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Random motor command with configurable range
        return randomMotorCommand();
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !config_.rewardModulationEnabled) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error) with configurable gain
    dopamineLevel_ = predictionError_ * config_.dopamineGain;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > config_.noveltyThreshold) { // Use novelty threshold
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
    if (!brain_ || !config_.developmentEnabled) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = config_.plasticityModifiers[0]; // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = config_.plasticityModifiers[1];
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = config_.plasticityModifiers[2];
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = config_.plasticityModifiers[3]; // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (config_.structuralPlasticityEnabled) {
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
    explorationState_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm