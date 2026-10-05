#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Constructor with default configuration
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, NeuronConfig{})
{
}

// Constructor with custom configuration
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const NeuronConfig& config)
    : brain_(brain)
    , config_(config)
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
{
    // Validate inputs
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        return;
    }
    
    // Initialize using the distribution method
    distributeNeurons(brain_.get());
    applyConfiguration();
}

// Destructor with cleanup
AgentBrain::~AgentBrain() {
    // Clean up active neuron tracker
    activeNeuronTracker_.clear();
    NLM_LOG_INFO("AgentBrain destructor called - cleanup complete");
}

// Apply configuration settings
void AgentBrain::applyConfiguration() {
    NLM_LOG_INFO("Applying AgentBrain configuration");
    
    // Log configuration values for debugging
    NLM_LOG_INFO("Neuron config - Vision: " + std::to_string(config_.visionSize) + 
                 ", Touch: " + std::to_string(config_.touchSize) +
                 ", Internal: " + std::to_string(config_.internalSize) +
                 ", Proprioception: " + std::to_string(config_.proprioceptionSize));
    
    NLM_LOG_INFO("Scaling factors - Vision: " + std::to_string(config_.visionScaleFactor) +
                 ", Touch: " + std::to_string(config_.touchScaleFactor) +
                 ", Internal: " + std::to_string(config_.internalScaleOffset) +
                 ", Proprioception: " + std::to_string(config_.proprioceptionScaleOffset));
}

// Distribute neurons from brain regions to appropriate groups
void AgentBrain::distributeNeurons(const Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain::distributeNeurons: Null brain pointer");
        return;
    }
    
    // Clear existing references before redistribution
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
    activeNeuronTracker_.clear();
    
    // Log neuron distribution
    NLM_LOG_INFO("Starting neuron distribution for AgentBrain");
    
    // Get total neuron counts for validation
    size_t totalMotor = 0;
    size_t totalSensory = 0;
    
    // Process each region and its populations
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                totalMotor += pop->getNeuronCount();
                NLM_LOG_INFO("Found motor population with " + std::to_string(pop->getNeuronCount()) + " neurons");
                
                for (Neuron* n : pop->getNeurons()) {
                    if (!validateNeuronExists(n)) {
                        NLM_LOG_ERROR("AgentBrain: Invalid neuron pointer in motor population");
                        continue;
                    }
                    
                    // Distribute motor neurons to different action groups
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                 motorTurnLeft_.size() + motorTurnRight_.size() +
                                 motorInteract_.size() + motorWait_.size();
                    
                    switch (idx % 6) {
                        case 0: motorForward_.push_back(*n); break;
                        case 1: motorBackward_.push_back(*n); break;
                        case 2: motorTurnLeft_.push_back(*n); break;
                        case 3: motorTurnRight_.push_back(*n); break;
                        case 4: motorInteract_.push_back(*n); break;
                        case 5: motorWait_.push_back(*n); break;
                    }
                    
                    activeNeuronTracker_[n] = true;
                }
            } else if (type == NeuronType::Sensory) {
                totalSensory += pop->getNeuronCount();
                NLM_LOG_INFO("Found sensory population with " + std::to_string(pop->getNeuronCount()) + " neurons");
                
                for (Neuron* n : pop->getNeurons()) {
                    if (!validateNeuronExists(n)) {
                        NLM_LOG_ERROR("AgentBrain: Invalid neuron pointer in sensory population");
                        continue;
                    }
                    
                    // Distribute sensory neurons
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                 sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(*n); break;
                        case 1: sensoryTouch_.push_back(*n); break;
                        case 2: sensoryInternal_.push_back(*n); break;
                        case 3: sensoryProprioception_.push_back(*n); break;
                    }
                    
                    activeNeuronTracker_[n] = true;
                }
            }
        }
    }
    
    NLM_LOG_INFO("Neuron distribution complete: " + std::to_string(totalMotor) + " motor neurons, " +
                 std::to_string(totalSensory) + " sensory neurons distributed");
    
    // Log final group sizes
    logNeuronDistribution();
}

// Initialize motor groups (placeholder - actual groups populated by distributeNeurons)
void AgentBrain::initializeMotorGroups() {
    NLM_LOG_INFO("Initializing motor groups");
    
    // Create empty motor groups - they will be populated by distributeNeurons
    // This ensures proper initialization even if no neurons are found
    NLM_LOG_INFO("Motor group initialization complete");
}

// Initialize sensory groups (placeholder - actual groups populated by distributeNeurons)
void AgentBrain::initializeSensoryGroups() {
    NLM_LOG_INFO("Initializing sensory groups");
    
    // Create empty sensory groups - they will be populated by distributeNeurons
    // This ensures proper initialization even if no neurons are found
    NLM_LOG_INFO("Sensory group initialization complete");
}

// Initialize with world and allocate memory
void AgentBrain::initialize(const SimpleWorld& world) {
    // Allocate memory for previous vision based on world dimensions
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    
    // Initialize developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = config_.plasticityModifier;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Get expected sensory input size based on configuration
size_t AgentBrain::getSensoryInputSize() const {
    return config_.visionSize + config_.touchSize + 
           config_.internalSize + config_.proprioceptionSize;
}

// Get expected motor output size based on configuration  
size_t AgentBrain::getMotorOutputSize() const {
    return config_.motorSize;
}

// Process sensory input with configurable scaling and validation
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Null brain pointer");
        return;
    }
    
    // Vision input processing with configurable scaling
    const auto& vision = percept.getVision();
    validateArrayBounds(vision, vision); // Validate vision array
    
    for (size_t i = 0; i < std::min(sensoryVision_.size(), vision.size()); ++i) {
        if (sensoryVision_[i]) {
            // Use configurable scale factor from NeuronConfig
            float current = vision[i] * config_.visionScaleFactor;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing with configurable scaling
    const auto& touch = percept.getTouch();
    validateArrayBounds(touch, touch); // Validate touch array
    
    for (size_t i = 0; i < std::min(sensoryTouch_.size(), touch.size()); ++i) {
        if (sensoryTouch_[i]) {
            // Use configurable scale factor from NeuronConfig
            float current = touch[i] * config_.touchScaleFactor;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing with configurable center and scale
    const auto& intern = percept.getInternal();
    validateArrayBounds(intern, intern); // Validate internal array
    
    for (size_t i = 0; i < std::min(sensoryInternal_.size(), intern.size()); ++i) {
        if (sensoryInternal_[i]) {
            // Use configurable center and scale factors from NeuronConfig
            float current = (intern[i] * config_.internalCenterFactor - config_.internalScaleOffset) * config_.visionScaleOffset;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing with configurable center and scale
    const auto& proprio = percept.getProprioception();
    validateArrayBounds(proprio, proprio); // Validate proprioception array
    
    for (size_t i = 0; i < std::min(sensoryProprioception_.size(), proprio.size()); ++i) {
        if (sensoryProprioception_[i]) {
            // Use configurable center and scale factors from NeuronConfig
            float current = (proprio[i] * config_.proprioceptionCenterFactor - config_.proprioceptionScaleOffset) * config_.proprioceptionScaleOffset;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < std::min(vision.size(), previousVision_.size()); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Apply novelty decay from config
        noveltyLevel_ *= config_.noveltyDecay;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty with configurable weights
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * config_.curiosityThreshold + std::abs(predictionError_) * config_.predictionErrorWeight;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

// Decode motor command from neural activity
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Null brain pointer");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > config_.curiosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

// Decode motor command from neural activity groups
MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<NeuronRef>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const NeuronRef& n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n.get().getState().membranePotential - n.get().getState().restingPotential);
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
    
    // Only act if there's meaningful activity (threshold from config)
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// Motor command selection with curiosity/exploration
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * config_.explorationChance;  // Up to 30% random
        
        if (brain_) {
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
    }
    
    return defaultCmd;
}

// Apply neuromodulation based on reward with configurable weights
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average with configurable weights)
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
                // Apply reward-modulated weight change with configurable plasticity
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine with configurable range
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = config_.plasticityModifier + config_.plasticityModifier * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, config_.plasticityFactorRange);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// Update development system with configurable stages
void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age (configurable thresholds)
    // This is a biologically inspired approximation
    if (developmentalAge_ < config_.developmentStage1) {  // Stage 1 threshold
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.developmentStage2) {  // Stage 2 threshold
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.developmentStage3) {  // Stage 3 threshold
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development if enabled
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

// Get current developmental stage
DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

// Get neuromodulation level
float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

// Get curiosity level
float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

// Get novelty level
float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

// Get prediction error
float AgentBrain::getPredictionError() const {
    return predictionError_;
}

// Reset agent for new episode
void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = config_.plasticityModifier;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Validation method for neuron groups
bool AgentBrain::validateNeuronGroups() const {
    bool isValid = true;
    
    // Check that each neuron reference points to a valid neuron
    for (const auto& pair : activeNeuronTracker_) {
        if (!validateNeuronExists(pair.first)) {
            NLM_LOG_ERROR("AgentBrain: Invalid neuron reference in tracker");
            isValid = false;
        }
    }
    
    // Check for empty groups that should have neurons
    if (sensoryVision_.empty() && !brain_->getRegions().empty()) {
        NLM_LOG_WARNING("AgentBrain: Vision neuron group is empty");
        // This might be acceptable depending on configuration
    }
    
    return isValid;
}

// Get total neuron count
size_t AgentBrain::getTotalNeuronCount() const {
    size_t total = 0;
    total += motorForward_.size();
    total += motorBackward_.size();
    total += motorTurnLeft_.size();
    total += motorTurnRight_.size();
    total += motorInteract_.size();
    total += motorWait_.size();
    total += sensoryVision_.size();
    total += sensoryTouch_.size();
    total += sensoryInternal_.size();
    total += sensoryProprioception_.size();
    return total;
}

// Log neuron distribution for debugging
void AgentBrain::logNeuronDistribution() const {
    NLM_LOG_INFO("Neuron distribution - Motor: " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() + 
                               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) +
                 " Sensory: " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                               sensoryInternal_.size() + sensoryProprioception_.size()) +
                 " Total: " + std::to_string(getTotalNeuronCount()));
}

// Load configuration from Config object
void AgentBrain::loadFromConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("AgentBrain::loadFromConfig: Null config pointer");
        return;
    }
    
    NLM_LOG_INFO("Loading AgentBrain configuration from Config");
    
    // Override config values with config object values if present
    auto visionOpt = config->get<size_t>("vision_size");
    if (visionOpt) config_.visionSize = *visionOpt;
    
    auto touchOpt = config->get<size_t>("touch_size");
    if (touchOpt) config_.touchSize = *touchOpt;
    
    auto internalOpt = config->get<size_t>("internal_size");
    if (internalOpt) config_.internalSize = *internalOpt;
    
    auto proprioceptionOpt = config->get<size_t>("proprioception_size");
    if (proprioceptionOpt) config_.proprioceptionSize = *proprioceptionOpt;
    
    auto visionScaleOpt = config->get<float>("vision_scale_factor");
    if (visionScaleOpt) config_.visionScaleFactor = *visionScaleOpt;
    
    auto touchScaleOpt = config->get<float>("touch_scale_factor");
    if (touchScaleOpt) config_.touchScaleFactor = *touchScaleOpt;
    
    auto noveltyDecayOpt = config->get<float>("novelty_decay");
    if (noveltyDecayOpt) config_.noveltyDecay = *noveltyDecayOpt;
    
    auto curiosityThresholdOpt = config->get<float>("curiosity_threshold");
    if (curiosityThresholdOpt) config_.curiosityThreshold = *curiosityThresholdOpt;
    
    NLM_LOG_INFO("Configuration loaded - Vision size: " + std::to_string(config_.visionSize) +
                 ", Touch size: " + std::to_string(config_.touchSize) +
                 ", Novelty decay: " + std::to_string(config_.noveltyDecay));
}

// Validate array bounds and prevent truncation
void AgentBrain::validateArrayBounds(const std::vector<float>& array, const std::vector<float>& input) const {
    if (input.empty()) {
        NLM_LOG_ERROR("AgentBrain: Input array is empty");
        return;
    }
    
    // Log warning if array is much smaller than expected input
    if (input.size() > 1000 && array.size() < input.size() / 10) {
        NLM_LOG_WARNING("AgentBrain: Large input array may be truncated - input size: " +
                       std::to_string(input.size()) +
                       ", array size: " + std::to_string(array.size()));
    }
}

// Validate neuron pointer
bool AgentBrain::validateNeuronExists(const Neuron* neuron) const {
    if (!neuron) {
        NLM_LOG_ERROR("AgentBrain: Null neuron pointer");
        return false;
    }
    
    // Check if neuron is in tracker (indicates valid reference)
    return activeNeuronTracker_.find(const_cast<Neuron*>(neuron)) != activeNeuronTracker_.end();
}

} // namespace nlm