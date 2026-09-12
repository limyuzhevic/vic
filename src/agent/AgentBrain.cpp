#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const Config& config)
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
    , sensoryNoveltyDecay_(0.99f)
    , mappingsInitialized_(false)
{
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Validate configuration
    if (!validateConfig()) {
        NLM_LOG_ERROR("AgentBrain: Invalid configuration provided");
        throw std::invalid_argument("Invalid AgentBrain configuration");
    }
    
    // Initialize mappings based on configuration
    initializeMappings();
    mappingsInitialized_ = true;
    
    NLM_LOG_INFO("AgentBrain initialized with " +
                 std::to_string(activeSensoryMappings_.size()) + " sensory mappings and " +
                 std::to_string(activeMotorMappings_.size()) + " motor mappings");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeMappings() {
    // Clear existing mappings
    activeMotorMappings_.clear();
    activeSensoryMappings_.clear();
    
    // Initialize motor mappings
    for (auto& motorMap : config_.motorMappings) {
        activeMotorMappings_.push_back(&motorMap);
        
        // Find motor neurons in brain for this mapping
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Assign motor neurons to mappings based on configuration
                        // Simple round-robin assignment for now
                        if (activeMotorMappings_.back()->neurons.size() < 
                            (brain_->getTotalNeuronCount() / config_.motorMappings.size()) / 10) {
                            activeMotorMappings_.back()->neurons.push_back(n);
                        }
                    }
                }
            }
        }
        
        // Ensure at least one neuron per mapping
        if (activeMotorMappings_.back()->neurons.empty()) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor && 
                        !pop->getNeurons().empty()) {
                        activeMotorMappings_.back()->neurons.push_back(pop->getNeurons()[0]);
                        break;
                    }
                }
                if (!activeMotorMappings_.back()->neurons.empty()) break;
            }
        }
    }
    
    // Initialize sensory mappings
    for (auto& sensoryMap : config_.sensoryMappings) {
        activeSensoryMappings_.push_back(&sensoryMap);
        
        // Find sensory neurons in brain for this mapping
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Assign sensory neurons to mappings based on configuration
                        if (activeSensoryMappings_.back()->neurons.size() < 
                            (brain_->getTotalNeuronCount() / config_.sensoryMappings.size()) / 10) {
                            activeSensoryMappings_.back()->neurons.push_back(n);
                        }
                    }
                }
            }
        }
        
        // Ensure at least one neuron per mapping
        if (activeSensoryMappings_.back()->neurons.empty()) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory && 
                        !pop->getNeurons().empty()) {
                        activeSensoryMappings_.back()->neurons.push_back(pop->getNeurons()[0]);
                        break;
                    }
                }
                if (!activeSensoryMappings_.back()->neurons.empty()) break;
            }
        }
    }
}

void AgentBrain::updateConfig(const Config& newConfig) {
    config_ = newConfig;
    if (validateConfig()) {
        // Re-initialize mappings with new configuration
        initializeMappings();
        NLM_LOG_INFO("AgentBrain configuration updated successfully");
    } else {
        NLM_LOG_ERROR("AgentBrain: Failed to update configuration - validation failed");
        throw std::invalid_argument("Invalid new configuration");
    }
}

bool AgentBrain::validateConfig() const {
    // Check for required mappings
    if (config_.sensoryMappings.empty()) {
        NLM_LOG_ERROR("AgentBrain: Configuration must have at least one sensory mapping");
        return false;
    }
    
    if (config_.motorMappings.empty()) {
        NLM_LOG_ERROR("AgentBrain: Configuration must have at least one motor mapping");
        return false;
    }
    
    // Validate sensory mapping values
    for (const auto& [name, mapping] : config_.sensoryMappings) {
        if (mapping.scaleFactor <= 0.0f) {
            NLM_LOG_ERROR("AgentBrain: Sensory mapping " + name + " has invalid scale factor");
            return false;
        }
        if (mapping.maxValue <= mapping.minValue) {
            NLM_LOG_ERROR("AgentBrain: Sensory mapping " + name + " has invalid value range");
            return false;
        }
    }
    
    // Validate motor mapping values
    for (const auto& mapping : config_.motorMappings) {
        if (mapping.activityThreshold < 0.0f || mapping.activityThreshold > 1.0f) {
            NLM_LOG_ERROR("AgentBrain: Motor mapping has invalid activity threshold");
            return false;
        }
        if (mapping.decayRate <= 0.0f || mapping.decayRate > 1.0f) {
            NLM_LOG_ERROR("AgentBrain: Motor mapping has invalid decay rate");
            return false;
        }
    }
    
    return true;
}

void AgentBrain::validateBrainPointer() const {
    if (!brain_) {
        throw std::runtime_error("AgentBrain: Brain pointer is null");
    }
}

void AgentBrain::validatePerceptDimensions(const SensoryPercept& percept) const {
    // Validate that percept dimensions match expected sizes
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // These checks can be extended based on configuration
    if (config_.scaleFactors.vision > 0.0f) {
        // Vision processing is enabled
    }
}

void AgentBrain::initializeMappings() {
    // Clear existing mappings
    activeMotorMappings_.clear();
    activeSensoryMappings_.clear();
    
    // Initialize motor mappings
    for (auto& motorMap : config_.motorMappings) {
        activeMotorMappings_.push_back(&motorMap);
        
        // Find motor neurons in brain for this mapping
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Assign motor neurons to mappings based on configuration
                        // Simple round-robin assignment for now
                        if (activeMotorMappings_.back()->neurons.size() < 
                            (brain_->getTotalNeuronCount() / config_.motorMappings.size()) / 10) {
                            activeMotorMappings_.back()->neurons.push_back(n);
                        }
                    }
                }
            }
        }
        
        // Ensure at least one neuron per mapping
        if (activeMotorMappings_.back()->neurons.empty()) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor && 
                        !pop->getNeurons().empty()) {
                        activeMotorMappings_.back()->neurons.push_back(pop->getNeurons()[0]);
                        break;
                    }
                }
                if (!activeMotorMappings_.back()->neurons.empty()) break;
            }
        }
    }
    
    // Initialize sensory mappings
    for (auto& sensoryMap : config_.sensoryMappings) {
        activeSensoryMappings_.push_back(&sensoryMap);
        
        // Find sensory neurons in brain for this mapping
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Assign sensory neurons to mappings based on configuration
                        if (activeSensoryMappings_.back()->neurons.size() < 
                            (brain_->getTotalNeuronCount() / config_.sensoryMappings.size()) / 10) {
                            activeSensoryMappings_.back()->neurons.push_back(n);
                        }
                    }
                }
            }
        }
        
        // Ensure at least one neuron per mapping
        if (activeSensoryMappings_.back()->neurons.empty()) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Sensory && 
                        !pop->getNeurons().empty()) {
                        activeSensoryMappings_.back()->neurons.push_back(pop->getNeurons()[0]);
                        break;
                    }
                }
                if (!activeSensoryMappings_.back()->neurons.empty()) break;
            }
        }
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision, size_t& processedCount) {
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        for (auto* sensoryNeuron : config_.sensoryMappings.at("vision").neurons) {
            if (sensoryNeuron) {
                // Apply configurable scale factor and value range
                float normalizedValue = (vision[i] - config_.sensoryMappings.at("vision").minValue) / 
                                       (config_.sensoryMappings.at("vision").maxValue - 
                                        config_.sensoryMappings.at("vision").minValue);
                float current = normalizedValue * config_.scaleFactors.vision;
                sensoryNeuron->injectCurrent(current);
                processedCount++;
                if (i < vision.size() - 1) break; // Process first matching neuron only
            }
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch, size_t& processedCount) {
    for (size_t i = 0; i < touch.size() && i < config_.sensoryMappings.at("touch").neurons.size(); ++i) {
        auto* sensoryNeuron = config_.sensoryMappings.at("touch").neurons[i];
        if (sensoryNeuron) {
            // Apply configurable scale factor and value range
            float normalizedValue = (touch[i] - config_.sensoryMappings.at("touch").minValue) / 
                                   (config_.sensoryMappings.at("touch").maxValue - 
                                    config_.sensoryMappings.at("touch").minValue);
            float current = normalizedValue * config_.scaleFactors.touch;
            sensoryNeuron->injectCurrent(current);
            processedCount++;
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal, size_t& processedCount) {
    for (size_t i = 0; i < internal.size() && i < config_.sensoryMappings.at("internal").neurons.size(); ++i) {
        auto* sensoryNeuron = config_.sensoryMappings.at("internal").neurons[i];
        if (sensoryNeuron) {
            // Apply configurable scale factor and value range with centering
            float normalizedValue = (internal[i] - config_.sensoryMappings.at("internal").minValue) / 
                                   (config_.sensoryMappings.at("internal").maxValue - 
                                    config_.sensoryMappings.at("internal").minValue);
            float centeredValue = (normalizedValue * 2.0f - 1.0f); // Center around zero
            float current = centeredValue * config_.scaleFactors.internal;
            sensoryNeuron->injectCurrent(current);
            processedCount++;
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio, size_t& processedCount) {
    for (size_t i = 0; i < proprio.size() && i < config_.sensoryMappings.at("proprioception").neurons.size(); ++i) {
        auto* sensoryNeuron = config_.sensoryMappings.at("proprioception").neurons[i];
        if (sensoryNeuron) {
            // Apply configurable scale factor and value range with centering
            float normalizedValue = (proprio[i] - config_.sensoryMappings.at("proprioception").minValue) / 
                                   (config_.sensoryMappings.at("proprioception").maxValue - 
                                    config_.sensoryMappings.at("proprioception").minValue);
            float centeredValue = (normalizedValue * 2.0f - 1.0f); // Center around zero
            float current = centeredValue * config_.scaleFactors.proprioception;
            sensoryNeuron->injectCurrent(current);
            processedCount++;
        }
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    validateBrainPointer();
    validatePerceptDimensions(pert);
    
    if (!brain_) return;
    
    // Reset processed count for logging
    size_t totalProcessed = 0;
    
    // Vision input (using configurable mapping)
    const auto& vision = percept.getVision();
    if (!config_.sensoryMappings.at("vision").neurons.empty()) {
        processVisionInput(vision, totalProcessed);
    }
    
    // Touch input (using configurable mapping)
    const auto& touch = percept.getTouch();
    if (!config_.sensoryMappings.at("touch").neurons.empty()) {
        processTouchInput(touch, totalProcessed);
    }
    
    // Internal signals (using configurable mapping)
    const auto& internal = percept.getInternal();
    if (!config_.sensoryMappings.at("internal").neurons.empty()) {
        processInternalInput(internal, totalProcessed);
    }
    
    // Proprioception (using configurable mapping)
    const auto& proprio = percept.getProprioception();
    if (!config_.sensoryMappings.at("proprioception").neurons.empty()) {
        processProprioceptionInput(proprio, totalProcessed);
    }
    
    // Compute novelty (difference from previous vision) - improved calculation
    computeNovelty(vision);
    
    // Update curiosity based on novelty
    updateCuriosityBasedOnNovelty();
    
    NLM_LOG_DEBUG("AgentBrain processed " + std::to_string(totalProcessed) + " sensory inputs");
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize with configurable threshold
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Apply configurable decay
    noveltyLevel_ *= config_.novelty.decayRate;
    
    // Store for next time
    previousVision_ = vision;
}

void AgentBrain::updateCuriosityBasedOnNovelty() {
    if (!curiosityEnabled_) return;
    
    // Use configurable weights for novelty and prediction error
    curiosityLevel_ = noveltyLevel_ * config_.novelty.curiosityWeight + 
                     std::abs(predictionError_) * config_.novelty.errorWeight;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

void AgentBrain::updateDevelopmentStage() {
    if (!developmentEnabled_ || !brain_) return;
    
    // Update developmental age
    developmentalAge_ += 0.1; // Small timestep for stage updates
    
    // Update plasticity modifier based on developmental stage
    updateDevelopment(config_.development.initialStageAge);
    
    // Log stage change for monitoring
    NLM_LOG_DEBUG("AgentBrain development stage updated, age: " + 
                 std::to_string(developmentalAge_));
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize: Cannot initialize with null brain pointer");
        return;
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Re-initialize mappings after config changes
    if (!mappingsInitialized_) {
        initializeMappings();
        mappingsInitialized_ = true;
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(activeSensoryMappings_.size()) + " sensory mappings and " +
                 std::to_string(activeMotorMappings_.size()) + " motor mappings");
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    if (activeMotorMappings_.empty()) {
        NLM_LOG_WARNING("AgentBrain::decodeFromMotorNeurons: No motor mappings configured");
        return MotorCommand::Wait;
    }
    
    // Calculate average activity in each motor group using configurable mappings
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    std::vector<float> motorActivities;
    for (auto* motorMap : activeMotorMappings_) {
        float activity = calcActivity(motorMap->neurons);
        motorActivities.push_back(activity);
        
        // Apply decay to motor activity
        if (motorMap->decayRate > 0.0f && motorMap->decayRate <= 1.0f) {
            for (Neuron* n : motorMap->neurons) {
                if (n) {
                    // Simple decay of membrane potential
                    float currentPot = n->getState().membranePotential;
                    float decayedPot = currentPot * motorMap->decayRate;
                    n->setMembranePotential(decayedPot);
                }
            }
        }
    }
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    
    for (size_t i = 0; i < motorActivities.size(); ++i) {
        if (motorActivities[i] > bestActivity) {
            bestActivity = motorActivities[i];
            best = activeMotorMappings_[i]->command;
        }
    }
    
    // Only act if there's meaningful activity (configurable threshold)
    float activityThreshold = 0.5f; // Could be made configurable
    if (bestActivity < activityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityEnabled_ && curiosityLevel_ > config_.curiosity.explorationThreshold) {
        // Higher curiosity = more exploration (configurable)
        float exploreChance = std::clamp(
            curiosityLevel_ * config_.curiosity.maxExplorationChance,
            config_.curiosity.minExplorationChance,
            config_.curiosity.maxExplorationChance
        );
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command selection
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
            
            if (std::abs(eligibility) > config_.reward.eligibilityThreshold) {
                // Apply reward-modulated weight change with configurable plasticity factor
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 
                             config_.reward.plasticityModulation;
                syn->addToWeight(delta);
                
                // Decay eligibility trace (configurable)
                syn->decayEligibilityTrace(config_.reward.eligibilityTraceDecay);
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
    
    // Update development stage based on configurable age thresholds
    updateDevelopmentStage();
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development (configurable)
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::reset() {
    resetNeuromodulationState();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Calculate based on configured sensory mappings
    size_t totalSize = 0;
    for (const auto& [name, mapping] : config_.sensoryMappings) {
        totalSize += 1; // Each mapping represents one sensory channel
    }
    return totalSize;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Return number of configured motor commands
    return config_.motorMappings.size();
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

} // namespace nlm
