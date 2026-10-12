#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Forward declarations for helper functions
static bool validateNeuronPopulation(const std::vector<Neuron*>& neurons, const std::string& name);
static size_t getNeuronCountFromConfig(const Config& config, const std::string& key, size_t defaultValue);

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain, Config{})
{
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const Config& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(config_.baselinePlasticityModifier)
    , rewardModulationEnabled_(config_.rewardModulationEnabled)
    , structuralPlasticityEnabled_(config_.structuralPlasticityEnabled)
    , developmentEnabled_(config_.developmentEnabled)
    , curiosityEnabled_(config_.curiosityEnabled)
    , sensoryNoveltyDecay_(config_.noveltySmoothingFactor)
    , lastStepCount_(0)
{
    if (!brain_) {
        throw std::invalid_argument("AgentBrain: Brain pointer cannot be null");
    }
    
    setupNeuronGroups();
    setupSensoryBuffers({}); // Empty world for initialization
    
    if (config_.verboseLogging) {
        NLM_LOG_INFO("AgentBrain initialized with " + std::to_string(config_.visionNeurons) + " vision neurons");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize - brain not available");
        return;
    }
    
    try {
        setupSensoryBuffers(world);
        developmentalAge_ = config_.initialDevelopmentalAge;
        plasticityModifier_ = config_.baselinePlasticityModifier;
        
        reset(); // Reset state
        
        // Validate neuron populations
        bool hasValidMotor = validateNeuronPopulation(motorForward_, "Forward") &&
                           validateNeuronPopulation(motorTurnLeft_, "Left") &&
                           validateNeuronPopulation(motorTurnRight_, "Right") &&
                           validateNeuronPopulation(motorLookLeft_, "LookLeft") &&
                           validateNeuronPopulation(motorLookRight_, "LookRight") &&
                           validateNeuronPopulation(motorInteract_, "Interact") &&
                           validateNeuronPopulation(motorWait_, "Wait") &&
                           validateNeuronPopulation(motorBackward_, "Backward");
        
        bool hasValidSensory = validateNeuronPopulation(sensoryVision_, "Vision") &&
                            validateNeuronPopulation(sensoryTouch_, "Touch") &&
                            validateNeuronPopulation(sensoryInternal_, "Internal") &&
                            validateNeuronPopulation(sensoryProprioception_, "Proprioception") &&
                            validateNeuronPopulation(sensoryAudio_, "Audio");
        
        if (!hasValidMotor || !hasValidSensory) {
            NLM_LOG_WARNING("AgentBrain: Some neuron populations are empty - may affect behavior");
        }
        
        if (config_.verboseLogging) {
            NLM_LOG_INFO("AgentBrain initialized with " + 
                        std::to_string(sensoryVision_.size()) + " vision neurons, " +
                        std::to_string(sensoryTouch_.size()) + " touch neurons, " +
                        std::to_string(sensoryInternal_.size()) + " internal neurons, " +
                        std::to_string(sensoryProprioception_.size()) + " proprioception neurons, " +
                        std::to_string(sensoryAudio_.size()) + " audio neurons, " +
                        std::to_string(getTotalMotorNeurons()) + " motor neurons");
        }
        
        emitEvent(EventType::DevelopmentalStageChange, 
                 static_cast<float>(getDevelopmentalStage()),
                 "Agent initialized");
                 
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain initialization failed: " + std::string(e.what()));
    }
}

bool AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot process sensory input - brain not available");
        return false;
    }
    
    try {
        // Process vision input with bounds checking and normalization
        const auto& vision = percept.getVision();
        size_t visionSize = std::min(vision.size(), sensoryVision_.size());
        
        for (size_t i = 0; i < visionSize; ++i) {
            if (sensoryVision_[i]) {
                float normalizedInput = std::clamp(vision[i], 0.0f, 1.0f);
                float current = normalizedInput * 5.0f; // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Process touch input
        const auto& touch = percept.getTouch();
        size_t touchSize = std::min(touch.size(), sensoryTouch_.size());
        
        for (size_t i = 0; i < touchSize; ++i) {
            if (sensoryTouch_[i]) {
                float collisionSignal = touch[i] * 8.0f; // Collision signal
                sensoryTouch_[i]->injectCurrent(collisionSignal);
            }
        }
        
        // Process internal signals
        const auto& intern = percept.getInternal();
        size_t internSize = std::min(intern.size(), sensoryInternal_.size());
        
        for (size_t i = 0; i < internSize; ++i) {
            if (sensoryInternal_[i]) {
                float processed = (intern[i] * 2.0f - 1.0f) * 5.0f; // Center and scale
                sensoryInternal_[i]->injectCurrent(processed);
            }
        }
        
        // Process proprioception
        const auto& proprio = percept.getProprioception();
        size_t proprioSize = std::min(proprio.size(), sensoryProprioception_.size());
        
        for (size_t i = 0; i < proprioSize; ++i) {
            if (sensoryProprioception_[i]) {
                float processed = (proprio[i] * 2.0f - 1.0f) * 3.0f; // Center and scale
                sensoryProprioception_[i]->injectCurrent(processed);
            }
        }
        
        // Compute novelty with enhanced detection
        computeNovelty(percept);
        
        // Update curiosity with configurable parameters
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
            
            // Check for curiosity threshold crossing
            if (curiosityLevel_ > config_.curiosityThreshold &&
                lastCuriosityLevel_ <= config_.curiosityThreshold) {
                emitEvent(EventType::CuriosityUpdate, curiosityLevel_, "Curiosity threshold crossed");
            }
        }
        
        // Update performance stats
        lastStepCount_++;
        updateStats();
        
        // Execute callbacks
        if (config_.onSensoryProcessed) {
            std::vector<float> allSignals = percept.getAllSignals();
            config_.onSensoryProcessed(allSignals);
        }
        
        emitEvent(EventType::BehaviorChange, 0.0f, "Sensory processed");
        
        lastCuriosityLevel_ = curiosityLevel_;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput failed: " + std::string(e.what()));
        return false;
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot decode motor command - brain not available");
        return MotorCommand::Wait;
    }
    
    try {
        MotorCommand decoded = decodeFromMotorNeurons();
        
        // Apply curiosity-based exploration with enhanced logic
        if (curiosityEnabled_ && curiosityLevel_ > config_.curiosityThreshold) {
            decoded = selectWithCuriosity(decoded);
            
            // Emit event on exploration
            if (decoded != MotorCommand::Wait &&
                lastDecodedCommand_ != MotorCommand::Wait &&
                decoded != lastDecodedCommand_) {
                emitEvent(EventType::BehaviorChange, 0.0f, "Exploration behavior change");
            }
        }
        
        lastDecodedCommand_ = decoded;
        
        // Update stats
        stats_.motorCommandsIssued++;
        stats_.commandHistory.push_back(decoded);
        
        // Keep history manageable
        if (stats_.commandHistory.size() > 1000) {
            stats_.commandHistory.erase(stats_.commandHistory.begin());
        }
        
        return decoded;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand failed: " + std::string(e.what()));
        return MotorCommand::Wait;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    try {
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward with exponential moving average
        expectedReward_ = config_.rewardPredictionDecay * expectedReward_ + 
                         (1.0f - config_.rewardPredictionDecay) * reward;
        
        // Dopamine-like signal with enhanced processing
        dopamineLevel_ = predictionError_ * config_.dopamineSensitivity;
        
        // Clamp to reasonable range with finer granularity
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply reward-modulated weight changes to synapses with eligibility traces
        applyRewardToSynapses();
        
        // Modulate plasticity based on dopamine
        if (config_.onRewardProcessed) {
            config_.onRewardProcessed(reward, predictedReward);
        }
        
        // Check for significant learning events
        if (std::abs(predictionError_) > 0.1f) {
            emitEvent(EventType::RewardReceived, predictionError_, 
                     "Prediction error " + std::to_string(predictionError_));
        }
        
        updateStats();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation failed: " + std::string(e.what()));
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    try {
        developmentalAge_ += timestep;
        
        // Enhanced developmental stages with finer granularity
        DevelopmentalStage newStage;
        float newPlasticityModifier;
        
        if (developmentalAge_ < 60.0) {  // ~1 minute
            newStage = DevelopmentalStage::Initial;
            newPlasticityModifier = 1.0f;  // High plasticity
        } else if (developmentalAge_ < 300.0) {  // ~5 minutes
            newStage = DevelopmentalStage::CriticalPeriod;
            newPlasticityModifier = 0.8f;
        } else if (developmentalAge_ < 900.0) {  // ~15 minutes
            newStage = DevelopmentalStage::Maturation;
            newPlasticityModifier = 0.5f;
        } else {
            newStage = DevelopmentalStage::Adult;
            newPlasticityModifier = 0.2f;  // Adult - more stable
        }
        
        // Check for developmental stage change
        if (newStage != getDevelopmentalStage()) {
            brain_->setDevelopmentalStage(newStage);
            emitEvent(EventType::DevelopmentalStageChange, 
                     static_cast<float>(newStage),
                     "Developmental stage changed");
        }
        
        // Smooth plasticity transition
        float decayFactor = std::exp(-timestep * 0.01); // Smooth transition
        plasticityModifier_ = decayFactor * plasticityModifier_ + 
                            (1.0f - decayFactor) * newPlasticityModifier;
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_) {
            updateStructuralPlasticity();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment failed: " + std::string(e.what()));
    }
}

// Helper method implementations
void AgentBrain::setupNeuronGroups() {
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

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = motorForward_.size() + motorBackward_.size() + 
                    motorTurnLeft_.size() + motorTurnRight_.size() +
                    motorInteract_.size() + motorWait_.size() +
                    motorLookLeft_.size() + motorLookRight_.size();
        
        switch (idx % 8) {
            case 0: motorForward_.push_back(n); break;
            case 1: motorBackward_.push_back(n); break;
            case 2: motorTurnLeft_.push_back(n); break;
            case 3: motorTurnRight_.push_back(n); break;
            case 4: motorInteract_.push_back(n); break;
            case 5: motorWait_.push_back(n); break;
            case 6: motorLookLeft_.push_back(n); break;
            case 7: motorLookRight_.push_back(n); break;
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                    sensoryInternal_.size() + sensoryProprioception_.size() +
                    sensoryAudio_.size();
        
        switch (idx % 5) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
            case 4: sensoryAudio_.push_back(n); break;
        }
    }
}

void AgentBrain::setupSensoryBuffers(const SimpleWorld& world) {
    // Initialize vision buffer based on world size if available
    if (!world.isEmpty()) {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    } else {
        previousVision_.resize(config_.visionNeurons, 0.0f);
    }
}

void AgentBrain::computeNovelty(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t compareSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply smoothing
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ = noveltyLevel_ * config_.noveltySmoothingFactor + 
                       (1.0f - config_.noveltySmoothingFactor) * noveltyLevel_;
        
        // Check for significant novelty
        if (noveltyLevel_ > config_.noveltyThreshold) {
            emitEvent(EventType::NoveltyDetected, noveltyLevel_, 
                     "Significant novelty detected");
        }
        
        // Store for next time
        previousVision_ = vision;
    }
}

void AgentBrain::applyRewardToSynapses() {
    if (!brain_) return;
    
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
}

void AgentBrain::updateStructuralPlasticity() {
    if (!brain_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Higher synaptogenesis in early development
        float synRate = config_.synaptogenesisRate * plasticityModifier_;
        float pruneRate = config_.pruningRate * (2.0f - plasticityModifier_);
        
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

void AgentBrain::updateEventSystem() {
    for (const auto& callback : eventCallbacks_) {
        try {
            callback(EventType::BehaviorChange, 0.0f, "");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Event callback failed: " + std::string(e.what()));
        }
    }
}

void AgentBrain::updateStats() {
    if (!brain_) return;
    
    size_t currentStepCount = brain_->getTotalSpikeCount();
    size_t stepDiff = currentStepCount - lastStepCount_;
    stats_.totalSteps += stepDiff;
    lastStepCount_ = currentStepCount;
    
    if (stats_.totalSteps > 0) {
        stats_.averageCuriosity += curiosityLevel_;
        stats_.averageNovelty += noveltyLevel_;
    }
}

// Configuration methods
definition<void> AgentBrain::configure(const Config& config) {
    config_ = config;
    
    // Update legacy flags for backward compatibility
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
    
    emitEvent(EventType::BehaviorChange, 0.0f, "Configuration updated");
}

definition<void> AgentBrain::enableSystem(const std::string& systemName, bool enable) {
    if (systemName == "reward_modulation") {
        rewardModulationEnabled_ = enable;
    } else if (systemName == "structural_plasticity") {
        structuralPlasticityEnabled_ = enable;
    } else if (systemName == "development") {
        developmentEnabled_ = enable;
    } else if (systemName == "curiosity") {
        curiosityEnabled_ = enable;
    } else {
        NLM_LOG_WARNING("AgentBrain: Unknown system name: " + systemName);
    }
}

definition<bool> AgentBrain::isSystemEnabled(const std::string& systemName) const {
    if (systemName == "reward_modulation") {
        return rewardModulationEnabled_;
    } else if (systemName == "structural_plasticity") {
        return structuralPlasticityEnabled_;
    } else if (systemName == "development") {
        return developmentEnabled_;
    } else if (systemName == "curiosity") {
        return curiosityEnabled_;
    }
    return false;
}

// Motor command convenience methods
MotorCommand AgentBrain::getEmergencyCommand() const {
    return MotorCommand::Wait; // Default safety response
}

MotorCommand AgentBrain::getDefaultCommand() const {
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::getExploreCommand() const {
    if (curiosityLevel_ > 0.7f) {
        return MotorCommand::MoveForward;
    }
    return MotorCommand::Wait;
}

// Developmental stage helpers
bool AgentBrain::isInCriticalPeriod() const {
    return getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod;
}

bool AgentBrain::isMature() const {
    return getDevelopmentalStage() == DevelopmentalStage::Adult;
}

// Event system methods
definition<void> AgentBrain::addEventCallback(EventCallback callback) {
    eventCallbacks_.push_back(callback);
}

definition<void> AgentBrain::removeEventCallback(EventCallback callback) {
    eventCallbacks_.erase(std::remove(eventCallbacks_.begin(), eventCallbacks_.end(), callback),
                         eventCallbacks_.end());
}

definition<void> AgentBrain::emitEvent(EventType type, float value, const std::string& message) {
    EventData event;
    event.type = type;
    event.value = value;
    event.message = message;
    event.timestamp = brain_->getClock()->getTime();
    
    for (const auto& callback : eventCallbacks_) {
        try {
            callback(type, value, message);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Event callback failed: " + std::string(e.what()));
        }
    }
}

// Statistics methods
AgentBrain::PerformanceStats AgentBrain::getStats() const {
    PerformanceStats stats = stats_;
    
    if (stats_.totalSteps > 0) {
        stats_.averageCuriosity /= stats_.totalSteps;
        stats_.averageNovelty /= stats_.totalSteps;
    }
    
    return stats;
}

definition<void> AgentBrain::resetStats() {
    stats_ = PerformanceStats{};
    lastStepCount_ = 0;
}

// Quality of life methods
bool AgentBrain::isHealthy() const {
    if (!brain_) return false;
    
    // Check for critical issues
    if (brain_->getTotalSpikeCount() > 1000000) {
        NLM_LOG_WARNING("AgentBrain: Extremely high spike count detected");
    }
    
    if (std::isnan(dopamineLevel_) || std::isinf(dopamineLevel_)) {
        NLM_LOG_ERROR("AgentBrain: Invalid dopamine level");
        return false;
    }
    
    return true;
}

std::string AgentBrain::getStatusSummary() const {
    std::stringstream ss;
    ss << "AgentBrain Status:";
    ss << "\n  Developmental Stage: " << static_cast<int>(getDevelopmentalStage());
    ss << "\n  Neuromodulation Level: " << std::fixed << std::setprecision(3) << getNeuromodulationLevel();
    ss << "\n  Curiosity Level: " << std::fixed << std::setprecision(3) << getCuriosityLevel();
    ss << "\n  Novelty Level: " << std::fixed << std::setprecision(3) << getNoveltyLevel();
    ss << "\n  Total Steps: " << stats_.totalSteps;
    ss << "\n  Motor Commands: " << stats_.motorCommandsIssued;
    ss << "\n  Healthy: " << (isHealthy() ? "Yes" : "No");
    
    return ss.str();
}

// Helper function implementations
static bool validateNeuronPopulation(const std::vector<Neuron*>& neurons, const std::string& name) {
    if (neurons.empty()) {
        NLM_LOG_WARNING("AgentBrain: " + name + " neuron group is empty");
        return false;
    }
    return true;
}

static size_t getNeuronCountFromConfig(const Config& config, const std::string& key, size_t defaultValue) {
    auto configPtr = brain_->getConfig();
    if (!configPtr) return defaultValue;
    
    int64_t count = configPtr->getOr<int64_t>(key.c_str(), static_cast<int64_t>(defaultValue));
    return static_cast<size_t>(count);
}

} // namespace nlm