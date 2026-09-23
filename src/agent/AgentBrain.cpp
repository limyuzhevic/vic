#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// AgentBrain implementation with improved organization and comprehensive error handling

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<const Config> config)
    : brain_(brain)
    , config_(config)
    , motorSelector_(config)
    , neuromodulationState_(config)
    , noveltyDetector_(config)
    , developmentManager_(config)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , developmentalAge_(0.0) {
    // Validate inputs
    NLM_CHECK_NULL_PTR(brain.get(), "brain", "AgentBrain constructor: brain is null");
    NLM_CHECK_NULL_PTR(config.get(), "config", "AgentBrain constructor: config is null");
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        auto regions = brain_->getRegions();
        for (const auto& region : regions) {
            NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in AgentBrain constructor");
            auto populations = region->getPopulations();
            for (auto& pop : populations) {
                NLM_CHECK_NULL_PTR(pop.get(), "pop", "Null population in AgentBrain constructor");
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    auto neurons = pop->getNeurons();
                    for (Neuron* n : neurons) {
                        NLM_CHECK_NULL_PTR(n, "n", "Null motor neuron in AgentBrain constructor");
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                     motorTurnLeft_.size() + motorTurnRight_.size() +
                                     motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    auto neurons = pop->getNeurons();
                    for (Neuron* n : neurons) {
                        NLM_CHECK_NULL_PTR(n, "n", "Null sensory neuron in AgentBrain constructor");
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                     sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("AgentBrain constructor completed successfully");
}

AgentBrain::~AgentBrain() {
    NLM_LOG_INFO("AgentBrain destructor called");
}

void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_DEBUG("Initializing AgentBrain with configuration validation");
    
    if (!validateConfiguration()) {
        NLM_LOG_WARNING("AgentBrain configuration validation failed");
    }
    
    // Validate world
    NLM_CHECK_NULL_PTR(&world, "world", "AgentBrain::initialize: world is null");
    
    // Initialize neuromodulation state
    neuromodulationState_.reset();
    
    // Initialize previous vision for novelty detection
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset development state
    developmentalAge_ = 0.0;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Helper implementations for forward declarations
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    // Validate input
    NLM_CHECK_NULL_PTR(&vision, "vision", "AgentBrain::processVisionInput: vision is null");
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Validate current computation
            float current = vision[i] * 5.0f;  // Scale factor
            if (!nlm::Validation::isFinite(current, "Invalid current in processVisionInput")) {
                NLM_LOG_WARNING("Invalid current value in processVisionInput: " + std::to_string(current));
                continue;
            }
            
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    // Validate input
    NLM_CHECK_NULL_PTR(&touch, "touch", "AgentBrain::processTouchInput: touch is null");
    
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Validate current computation
            float current = touch[i] * 8.0f;  // Collision signal
            if (!nlm::Validation::isFinite(current, "Invalid current in processTouchInput")) {
                NLM_LOG_WARNING("Invalid current value in processTouchInput: " + std::to_string(current));
                continue;
            }
            
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    // Validate input
    NLM_CHECK_NULL_PTR(&internal, "internal", "AgentBrain::processInternalInput: internal is null");
    
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Validate current computation
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            if (!nlm::Validation::isFinite(current, "Invalid current in processInternalInput")) {
                NLM_LOG_WARNING("Invalid current value in processInternalInput: " + std::to_string(current));
                continue;
            }
            
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    // Validate input
    NLM_CHECK_NULL_PTR(&proprio, "proprio", "AgentBrain::processProprioceptionInput: proprio is null");
    
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Validate current computation
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            if (!nlm::Validation::isFinite(current, "Invalid current in processProprioceptionInput")) {
                NLM_LOG_WARNING("Invalid current value in processProprioceptionInput: " + std::to_string(current));
                continue;
            }
            
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeAndUpdateNovelty(const std::vector<float>& vision) {
    // Validate input
    NLM_CHECK_NULL_PTR(&vision, "vision", "AgentBrain::computeAndUpdateNovelty: vision is null");
    
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        
        // Calculate spatial novelty with optimized computation
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize using max_element for efficiency
        noveltyLevel_ = totalDiff / std::max(minSize, 1);
        
        // Validate novelty level
        if (!nlm::Validation::isFinite(noveltyLevel_, "Invalid novelty level")) {
            NLM_LOG_WARNING("Invalid novelty level calculated: " + std::to_string(noveltyLevel_));
            noveltyLevel_ = 0.0f;
        }
        
        // Decay and update
        noveltyLevel_ *= neuromodulationState_.getSensoryNoveltyDecay();
        
        // Validate decay result
        if (!nlm::Validation::isFinite(noveltyLevel_, "Invalid novelty level after decay")) {
            NLM_LOG_WARNING("Invalid novelty level after decay: " + std::to_string(noveltyLevel_));
            noveltyLevel_ = 0.0f;
        }
        
        // Store for next time
        updatePreviousVision(vision);
    }
}

float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    // Use single pass for efficiency instead of nested loops
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        NLM_CHECK_NULL_PTR(n, "n", "Null neuron in calculateNeuronActivity");
        
        // Use membrane potential deviation from rest as activity measure
        const auto& state = n->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        if (!nlm::Validation::isFinite(activity, "Invalid neuron activity")) {
            NLM_LOG_WARNING("Invalid neuron activity calculated: " + std::to_string(activity));
            activity = 0.0f;
        }
        
        sum += activity;
    }
    
    return sum / neurons.size();
}

void AgentBrain::extractMotorActivities(std::vector<float>& activities) const {
    // Validate output parameter
    NLM_CHECK_NULL_PTR(&activities, "activities", "AgentBrain::extractMotorActivities: activities is null");
    
    // Pre-allocate memory for efficiency
    activities.clear();
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size();
    activities.reserve(totalMotorNeurons);
    
    // Calculate activities in single pass
    if (!motorForward_.empty()) {
        activities.push_back(calculateNeuronActivity(motorForward_));
    }
    if (!motorBackward_.empty()) {
        activities.push_back(calculateNeuronActivity(motorBackward_));
    }
    if (!motorTurnLeft_.empty()) {
        activities.push_back(calculateNeuronActivity(motorTurnLeft_));
    }
    if (!motorTurnRight_.empty()) {
        activities.push_back(calculateNeuronActivity(motorTurnRight_));
    }
    if (!motorInteract_.empty()) {
        activities.push_back(calculateNeuronActivity(motorInteract_));
    }
    if (!motorWait_.empty()) {
        activities.push_back(calculateNeuronActivity(motorWait_));
    }
}

MotorCommand AgentBrain::selectWithCuriosityOptimized(MotorCommand defaultCmd) {
    float curiosityLevel = neuromodulationState_.getCuriosityLevel();
    
    // Validate curiosity level
    if (!nlm::Validation::isFinite(curiosityLevel, "Invalid curiosity level")) {
        NLM_LOG_WARNING("Invalid curiosity level: " + std::to_string(curiosityLevel));
        curiosityLevel = 0.0f;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            return getRandomExploratoryAction();
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::getRandomExploratoryAction() const {
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

void AgentBrain::applyRewardModulationOptimized(float reward, float predictedReward) {
    // Validate input parameters
    if (!nlm::Validation::isFinite(reward, "Invalid reward value") || 
        !nlm::Validation::isFinite(predictedReward, "Invalid predicted reward value")) {
        NLM_LOG_WARNING("Invalid reward values: " + std::to_string(reward) + ", " + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error with validation
    float predictionError = calculatePredictionError(reward, predictedReward);
    neuromodulationState_.setPredictionError(predictionError);
    
    // Update expected reward (exponential moving average)
    neuromodulationState_.setExpectedReward(
        0.95f * neuromodulationState_.getExpectedReward() + 0.05f * reward);
    
    // Apply to all synapses with eligibility traces
    applyRewardModulationToSynapses(predictionError);
    
    // Update development effects based on neuromodulation
    updateDevelopmentBasedOnNeuromodulation(predictionError);
}

float AgentBrain::calculatePredictionError(float reward, float predicted) const {
    return reward - predicted;
}

void AgentBrain::applyRewardModulationToSynapses(float predictionError) {
    // Validate prediction error
    if (!nlm::Validation::isFinite(predictionError, "Invalid prediction error")) {
        NLM_LOG_WARNING("Invalid prediction error: " + std::to_string(predictionError));
        predictionError = 0.0f;
    }
    
    for (const auto& region : brain_->getRegions()) {
        NLM_CHECK_NULL_PTR(region.get(), "region", "Null region in applyRewardModulationToSynapses");
        
        auto synapses = region->getSynapses();
        for (auto* syn : synapses) {
            NLM_CHECK_NULL_PTR(syn, "syn", "Null synapse in applyRewardModulationToSynapses");
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with configurable plasticity
                float delta = eligibility * neuromodulationState_.getDopamineLevel() * neuromodulationState_.getPlasticityFactor();
                syn->addToWeight(delta);
                
                // Decay eligibility trace with optimized decay
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::updateDevelopmentBasedOnNeuromodulation(float predictionError) {
    // Modulate plasticity based on dopamine (prediction error)
    // Positive prediction error increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * predictionError;
    
    // Validate and clamp plasticity factor
    if (!nlm::Validation::isFinite(plasticityFactor, "Invalid plasticity factor")) {
        NLM_LOG_WARNING("Invalid plasticity factor calculated: " + std::to_string(plasticityFactor));
        plasticityFactor = 0.5f;
    }
    
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    neuromodulationState_.updatePlasticityFactor();
    configureStructuralPlasticity(neuromodulationState_.getPlasticityFactor());
}

void AgentBrain::updateDevelopmentOptimized(double timestep) {
    developmentalAge_ += timestep;
    
    // Validate developmental age
    if (!nlm::Validation::isFinite(developmentalAge_, "Invalid developmental age")) {
        NLM_LOG_WARNING("Invalid developmental age: " + std::to_string(developmentalAge_));
        developmentalAge_ = 0.0;
    }
    
    // Get developmental stage info from development manager
    auto stageInfo = developmentManager_.getStageInfo(developmentalAge_);
    
    // Update brain developmental stage (using development system if available)
    auto* devSystem = brain_->getDevelopmentSystem();
    if (devSystem) {
        devSystem->update(brain_.get(), *brain_->getRandomGenerator(), timestep * 1000);
    }
    
    // Update plasticity modifier
    neuromodulationState_.setPlasticityModifier(stageInfo.plasticityModifier);
    
    // Apply structural plasticity effects if enabled
    if (structuralPlasticityEnabled_) {
        configureStructuralPlasticity(stageInfo.plasticityModifier);
    }
}

void AgentBrain::configureStructuralPlasticity(float plasticityModifier) {
    if (!brain_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Calculate rates using development manager
        float synRate = developmentManager_.calculateSynaptogenesisRate(plasticityModifier);
        float pruneRate = developmentManager_.calculatePruningRate(plasticityModifier);
        
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

// Helper methods for DevelopmentManager interface
DevelopmentManager::DevelopmentalStageInfo DevelopmentManager::getStageInfo(double age) const {
    DevelopmentManager::DevelopmentalStageInfo info;
    
    if (age < INITIAL_DURATION) {
        info.plasticityModifier = INITIAL_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION) {
        info.plasticityModifier = CRITICAL_PERIOD_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION + MATURATION_DURATION) {
        info.plasticityModifier = MATURATION_PLASTICITY;
    } else {
        info.plasticityModifier = ADULT_PLASTICITY;
    }
    
    // Calculate rates using the helper methods
    info.synaptogenesisRate = calculateSynaptogenesisRate(info.plasticityModifier);
    info.pruningRate = calculatePruningRate(info.plasticityModifier);
    
    return info;
    
}

} // namespace nlm


AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_DEBUG("Initializing AgentBrain with configuration validation");
    
    if (!validateConfiguration()) {
        NLM_LOG_WARNING("AgentBrain configuration validation failed");
    }
    
    // Initialize neuromodulation state
    neuromodulationState_.reset();
    
    // Initialize previous vision for novelty detection
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset development state
    developmentalAge_ = 0.0;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Helper implementations for forward declarations
void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeAndUpdateNovelty(const std::vector<float>& vision) {
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t minSize = std::min(vision.size(), previousVision_.size());
        
        // Calculate spatial novelty with optimized computation
        for (size_t i = 0; i < minSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize using max_element for efficiency
        noveltyLevel_ = totalDiff / std::max(minSize, 1);
        
        // Decay and update
        noveltyLevel_ *= neuromodulationState_.getSensoryNoveltyDecay();
        
        // Store for next time
        updatePreviousVision(vision);
    }
}

float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    // Use single pass for efficiency instead of nested loops
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    
    return sum / neurons.size();
}

void AgentBrain::extractMotorActivities(std::vector<float>& activities) const {
    // Pre-allocate memory for efficiency
    activities.clear();
    activities.reserve(motorForward_.size() + motorBackward_.size() + 
                      motorTurnLeft_.size() + motorTurnRight_.size() +
                      motorInteract_.size() + motorWait_.size());
    
    // Calculate activities in single pass
    if (!motorForward_.empty()) {
        activities.push_back(calculateNeuronActivity(motorForward_));
    }
    if (!motorBackward_.empty()) {
        activities.push_back(calculateNeuronActivity(motorBackward_));
    }
    if (!motorTurnLeft_.empty()) {
        activities.push_back(calculateNeuronActivity(motorTurnLeft_));
    }
    if (!motorTurnRight_.empty()) {
        activities.push_back(calculateNeuronActivity(motorTurnRight_));
    }
    if (!motorInteract_.empty()) {
        activities.push_back(calculateNeuronActivity(motorInteract_));
    }
    if (!motorWait_.empty()) {
        activities.push_back(calculateNeuronActivity(motorWait_));
    }
}

MotorCommand AgentBrain::selectWithCuriosityOptimized(MotorCommand defaultCmd) {
    float curiosityLevel = neuromodulationState_.getCuriosityLevel();
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            return getRandomExploratoryAction();
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::getRandomExploratoryAction() const {
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

void AgentBrain::applyRewardModulationOptimized(float reward, float predictedReward) {
    // Validate input parameters
    if (!std::isfinite(reward) || !std::isfinite(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward values: " + std::to_string(reward) + ", " + std::to_string(predictedReward));
        return;
    }
    
    // Compute prediction error with validation
    float predictionError = calculatePredictionError(reward, predictedReward);
    neuromodulationState_.setPredictionError(predictionError);
    
    // Update expected reward (exponential moving average)
    neuromodulationState_.setExpectedReward(
        0.95f * neuromodulationState_.getExpectedReward() + 0.05f * reward);
    
    // Apply to all synapses with eligibility traces
    applyRewardModulationToSynapses(predictionError);
    
    // Update development effects based on neuromodulation
    updateDevelopmentBasedOnNeuromodulation(predictionError);
}

float AgentBrain::calculatePredictionError(float reward, float predicted) const {
    return reward - predicted;
}

void AgentBrain::applyRewardModulationToSynapses(float predictionError) {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with configurable plasticity
                float delta = eligibility * neuromodulationState_.getDopamineLevel() * neuromodulationState_.getPlasticityFactor();
                syn->addToWeight(delta);
                
                // Decay eligibility trace with optimized decay
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::updateDevelopmentBasedOnNeuromodulation(float predictionError) {
    // Modulate plasticity based on dopamine (prediction error)
    // Positive prediction error increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * predictionError;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    neuromodulationState_.updatePlasticityFactor();
    configureStructuralPlasticity(neuromodulationState_.getPlasticityFactor());
}

void AgentBrain::updateDevelopmentOptimized(double timestep) {
    developmentalAge_ += timestep;
    
    // Get developmental stage info from development manager
    auto stageInfo = developmentManager_.getStageInfo(developmentalAge_);
    
    // Update brain developmental stage (using development system if available)
    auto* devSystem = brain_->getDevelopmentSystem();
    if (devSystem) {
        devSystem->update(brain_.get(), *brain_->getRandomGenerator(), timestep * 1000);
    }
    
    // Update plasticity modifier
    neuromodulationState_.setPlasticityModifier(stageInfo.plasticityModifier);
    
    // Apply structural plasticity effects if enabled
    if (structuralPlasticityEnabled_) {
        configureStructuralPlasticity(stageInfo.plasticityModifier);
    }
}

void AgentBrain::configureStructuralPlasticity(float plasticityModifier) {
    if (!brain_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Calculate rates using development manager
        float synRate = developmentManager_.calculateSynaptogenesisRate(plasticityModifier);
        float pruneRate = developmentManager_.calculatePruningRate(plasticityModifier);
        
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

// Helper methods for DevelopmentManager interface
DevelopmentManager::DevelopmentalStageInfo DevelopmentManager::getStageInfo(double age) const {
    DevelopmentManager::DevelopmentalStageInfo info;
    
    if (age < INITIAL_DURATION) {
        info.plasticityModifier = INITIAL_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION) {
        info.plasticityModifier = CRITICAL_PERIOD_PLASTICITY;
    } else if (age < INITIAL_DURATION + CRITICAL_PERIOD_DURATION + MATURATION_DURATION) {
        info.plasticityModifier = MATURATION_PLASTICITY;
    } else {
        info.plasticityModifier = ADULT_PLASTICITY;
    }
    
    // Calculate rates using the helper methods
    info.synaptogenesisRate = calculateSynaptogenesisRate(info.plasticityModifier);
    info.pruningRate = calculatePruningRate(info.plasticityModifier);
    
    return info;
    
}
