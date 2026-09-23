// AgentBrainEnhanced.cpp - Implementation of enhanced AgentBrain class

#include "AgentBrainEnhanced.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    : brain_(brain), config_(config)
    , dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f)
    , predictionError_(0.0f), expectedReward_(0.0f), plasticityModifier_(config.plasticityModifierBase)
    , developmentalAge_(0.0), totalSpikesProcessed_(0.0f), sensoryEventsProcessed_(0), motorCommandsGenerated_(0)
    , rewardModulationEnabled_(true), structuralPlasticityEnabled_(true)
    , developmentEnabled_(true), curiosityEnabled_(config_.curiosityThreshold > 0.0f)
{
    if (!validateBrain()) {
        logError("Invalid brain provided to AgentBrain constructor", true);
    }
    
    // Initialize neuron groups based on configuration
    initializeNeuronGroups();
}

AgentBrain::~AgentBrain() = default;

bool AgentBrain::initialize(const SimpleWorld& world, bool logInitialization) {
    try {
        if (!brain_) {
            logError("Cannot initialize AgentBrain: brain is null", true);
            return false;
        }
        
        // Validate brain state
        if (!validateBrain()) {
            logError("Cannot initialize AgentBrain: brain validation failed", true);
            return false;
        }
        
        // Initialize previous vision buffer
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        
        // Reset developmental state
        developmentalAge_ = 0.0;
        plasticityModifier_ = config_.plasticityModifierBase;
        totalSpikesProcessed_ = 0.0f;
        sensoryEventsProcessed_ = 0;
        motorCommandsGenerated_ = 0;
        
        // Clear error messages
        errorMessages_.clear();
        
        if (logInitialization) {
            logStatistics();
        }
        
        NLM_LOG_INFO("AgentBrain initialized successfully");
        if (!motorGroups_.empty() || !sensoryGroups_.empty()) {
            NLM_LOG_INFO("  Motor groups: " + std::to_string(motorGroups_.size()) + 
                        ", Sensory groups: " + std::to_string(sensoryGroups_.size()));
        }
        
        return true;
    } catch (const std::exception& e) {
        logError(std::string("Exception during initialization: ") + e.what(), true);
        return false;
    }
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) return;
    
    motorGroups_.clear();
    sensoryGroups_.clear();
    motorGroupNames_.clear();
    sensoryGroupNames_.clear();
    
    // Initialize motor neuron groups
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                auto neurons = pop->getNeurons();
                
                if (config_.enablePerformanceOptimizations && neurons.size() > config_.maxMotorNeurons) {
                    // Sample neurons for performance
                    size_t step = neurons.size() / std::max<size_t>(config_.maxMotorNeurons, 1);
                    for (size_t i = 0; i < neurons.size(); i += step) {
                        if (motorGroups_.size() < config_.maxMotorNeurons) {
                            motorGroups_.push_back({neurons[i]});
                            motorGroupNames_.push_back("motor_" + std::to_string(motorGroups_.size() - 1));
                        }
                    }
                } else {
                    // Use all neurons
                    motorGroups_.push_back(neurons);
                    motorGroupNames_.push_back("motor_group_" + std::to_string(motorGroups_.size() - 1));
                }
            } else if (type == NeuronType::Sensory) {
                auto neurons = pop->getNeurons();
                
                if (config_.enablePerformanceOptimizations && neurons.size() > config_.maxSensoryNeurons) {
                    // Sample neurons for performance
                    size_t step = neurons.size() / std::max<size_t>(config_.maxSensoryNeurons, 1);
                    for (size_t i = 0; i < neurons.size(); i += step) {
                        if (sensoryGroups_.size() < config_.maxSensoryNeurons) {
                            sensoryGroups_.push_back({neurons[i]});
                            sensoryGroupNames_.push_back("sensory_" + std::to_string(sensoryGroups_.size() - 1));
                        }
                    }
                } else {
                    // Use all neurons
                    sensoryGroups_.push_back(neurons);
                    sensoryGroupNames_.push_back("sensory_group_" + std::to_string(sensoryGroups_.size() - 1));
                }
            }
        }
    }
}

bool AgentBrain::validateBrain() const {
    if (!brain_) {
        logError("Brain pointer is null", false);
        return false;
    }
    
    if (brain_->getTotalNeuronCount() == 0) {
        logError("Brain has no neurons", false);
        return false;
    }
    
    // Check if brain has been initialized
    if (!brain_->getRegionCount()) {
        logError("Brain has no regions", false);
        return false;
    }
    
    return true;
}

bool AgentBrain::validateInputRanges(const SensoryPercept& percept) const {
    if (!config_.validateInputRanges) return true;
    
    bool valid = true;
    
    // Validate vision input
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        for (float value : vision) {
            if (value < 0.0f || value > 1.0f) {
                logError("Invalid vision input value: " + std::to_string(value), false);
                valid = false;
            }
        }
    }
    
    // Validate touch input
    const auto& touch = percept.getTouch();
    for (float value : touch) {
        if (value < 0.0f || value > 1.0f) {
            logError("Invalid touch input value: " + std::to_string(value), false);
            valid = false;
        }
    }
    
    return valid;
}

bool AgentBrain::processSensoryInput(const SensoryPercept& percept, bool validateInput) {
    try {
        if (!brain_) {
            logError("Cannot process input: brain is null", true);
            return false;
        }
        
        // Validate input if requested
        if (validateInput && !validateInputRanges(percept)) {
            if (config_.continueOnError) {
                NLM_LOG_WARNING("Input validation failed but continuing (continueOnError=true)");
            } else {
                logError("Input validation failed", true);
                return false;
            }
        }
        
        // Process vision input
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryGroups_.size() && i < vision.size(); ++i) {
            if (sensoryGroups_[i].empty()) continue;
            
            // Use average activity across group for scaling
            float current = vision[i] * 5.0f;  // Scale factor
            for (Neuron* n : sensoryGroups_[i]) {
                n->injectCurrent(current / sensoryGroups_[i].size());
            }
        }
        
        // Process touch input
        const auto& touch = percept.getTouch();
        size_t touchGroupIndex = 0;
        for (size_t i = 0; i < sensoryGroups_.size() && touchGroupIndex < config_.sensoryTouchCount && i < touch.size(); ++i) {
            if (sensoryGroups_[i].empty()) continue;
            
            float current = touch[i] * 8.0f;  // Collision signal
            for (Neuron* n : sensoryGroups_[i]) {
                n->injectCurrent(current / sensoryGroups_[i].size());
            }
            touchGroupIndex++;
        }
        
        // Process internal signals
        const auto& intern = percept.getInternal();
        size_t internalGroupIndex = 0;
        for (size_t i = 0; i < sensoryGroups_.size() && internalGroupIndex < config_.sensoryInternalCount && i < intern.size(); ++i) {
            if (sensoryGroups_[i].empty()) continue;
            
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            for (Neuron* n : sensoryGroups_[i]) {
                n->injectCurrent(current / sensoryGroups_[i].size());
            }
            internalGroupIndex++;
        }
        
        // Process proprioception
        const auto& proprio = percept.getProprioception();
        size_t proprioGroupIndex = 0;
        for (size_t i = 0; i < sensoryGroups_.size() && proprioGroupIndex < config_.sensoryProprioceptionCount && i < proprio.size(); ++i) {
            if (sensoryGroups_[i].empty()) continue;
            
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            for (Neuron* n : sensoryGroups_[i]) {
                n->injectCurrent(current / sensoryGroups_[i].size());
            }
            proprioGroupIndex++;
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
            noveltyLevel_ *= config_.noveltyDecayFactor;
            
            // Store for next time
            previousVision_ = vision;
            
            sensoryEventsProcessed_++;
        }
        
        // Update curiosity based on novelty and prediction error
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        updateNeuromodulationState();
        
        return true;
    } catch (const std::exception& e) {
        logError(std::string("Exception during sensory processing: ") + e.what(), true);
        return false;
    }
}

void AgentBrain::updateNeuromodulationState() {
    // Update dopamine based on prediction error
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, config_.dopamineMinClamp, config_.dopamineMaxClamp);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = config_.rewardPredictionDecay * expectedReward_ + (1.0f - config_.rewardPredictionDecay) * predictionError_;
    
    // Apply to synapses with eligibility traces
    if (rewardModulationEnabled_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > config_.rewardMinChange) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(0.1f);
                    
                    totalSpikesProcessed_ += std::abs(delta);
                }
            }
        }
    }
}

void AgentBrain::updateDevelopmentalParameters() {
    // Simple developmental stages based on age
    if (developmentalAge_ < config_.developmentStage1Duration) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.developmentStage1Duration + config_.developmentStage2Duration) {
        plasticityModifier_ = config_.plasticityModifierBase * 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.developmentStage1Duration + config_.developmentStage2Duration + config_.developmentStage3Duration) {
        plasticityModifier_ = config_.plasticityModifierBase * 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = config_.plasticityModifierBase * 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        logError("Cannot decode motor command: brain is null", true);
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > config_.curiosityThreshold) {
        decoded = selectWithAdvancedCuriosity(decoded);
    }
    
    motorCommandsGenerated_++;
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    // Calculate activity for each motor group
    std::vector<float> activities;
    for (const auto& group : motorGroups_) {
        activities.push_back(calcActivity(group));
    }
    
    // Find maximum activity
    float maxActivity = 0.0f;
    MotorCommand best = MotorCommand::Wait;
    
    // Map group index to motor command (assuming groups are in order: forward, backward, left, right, interact, wait)
    MotorCommand commands[] = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    for (size_t i = 0; i < activities.size() && i < 6; ++i) {
        if (activities[i] > maxActivity) {
            maxActivity = activities[i];
            best = commands[i];
        }
    }
    
    // Only act if there's meaningful activity
    if (maxActivity < config_.curiosityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithAdvancedCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration based on curiosity level
    if (curiosityLevel_ > config_.explorationThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * config_.maxExplorationChance;
        
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
    expectedReward_ = config_.rewardPredictionDecay * expectedReward_ + (1.0f - config_.rewardPredictionDecay) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to configurable range
    dopamineLevel_ = std::clamp(dopamineLevel_, config_.dopamineMinClamp, config_.dopamineMaxClamp);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > config_.rewardMinChange) {
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
    
    updateDevelopmentalParameters();
    
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

bool AgentBrain::reset(bool logReset) {
    try {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        plasticityModifier_ = config_.plasticityModifierBase;
        developmentalAge_ = 0.0;
        
        // Clear previous vision
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
        errorMessages_.clear();
        
        if (logReset) {
            logStatistics();
        }
        
        return true;
    } catch (const std::exception& e) {
        logError(std::string("Exception during reset: ") + e.what(), true);
        return false;
    }
}

void AgentBrain::logError(const std::string& error, bool fatal) {
    if (config_.logErrors) {
        std::string fullError = "[AgentBrain] " + error;
        if (fatal) {
            NLM_LOG_ERROR(fullError);
        } else {
            NLM_LOG_WARNING(fullError);
        }
        errorMessages_.push_back(fullError);
    }
}

void AgentBrain::debugNeuronActivity() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== AgentBrain Neuron Activity Debug ===");
    
    for (size_t i = 0; i < motorGroups_.size(); ++i) {
        float activity = 0.0f;
        if (!motorGroups_[i].empty()) {
            for (const Neuron* n : motorGroups_[i]) {
                activity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
            activity /= motorGroups_[i].size();
        }
        NLM_LOG_INFO(std::string("Motor Group ") + motorGroupNames_[i] + ": " + std::to_string(activity));
    }
    
    NLM_LOG_INFO("=== End Debug ===");
}

void AgentBrain::logStatistics() const {
    NLM_LOG_INFO("=== AgentBrain Statistics ===");
    NLM_LOG_INFO("  Developmental Age: " + std::to_string(developmentalAge_) + "s");
    NLM_LOG_INFO("  Neuromodulation Level: " + std::to_string(dopamineLevel_));
    NLM_LOG_INFO("  Curiosity Level: " + std::to_string(curiosityLevel_));
    NLM_LOG_INFO("  Novelty Level: " + std::to_string(noveltyLevel_));
    NLM_LOG_INFO("  Prediction Error: " + std::to_string(predictionError_));
    NLM_LOG_INFO("  Plasticity Modifier: " + std::to_string(plasticityModifier_));
    NLM_LOG_INFO("  Motor Commands Generated: " + std::to_string(motorCommandsGenerated_));
    NLM_LOG_INFO("  Sensory Events Processed: " + std::to_string(sensoryEventsProcessed_));
    NLM_LOG_INFO("  Total Spike Changes: " + std::to_string(totalSpikesProcessed_));
    NLM_LOG_INFO("  Error Count: " + std::to_string(errorMessages_.size()));
    NLM_LOG_INFO("=== End Statistics ===");
}

AgentBrainConfig AgentBrain::loadFromFile(const std::string& filepath) {
    AgentBrainConfig config;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = Config::trim(line);
        if (line.empty() || Config::isComment(line)) {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        
        std::string key = Config::trim(line.substr(0, pos));
        std::string value = Config::trim(line.substr(pos + 1));
        
        // Remove quotes
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse configuration values
        if (key == "motorForwardCount") {
            config.motorForwardCount = std::stoi(value);
        } else if (key == "motorBackwardCount") {
            config.motorBackwardCount = std::stoi(value);
        } else if (key == "motorTurnLeftCount") {
            config.motorTurnLeftCount = std::stoi(value);
        } else if (key == "motorTurnRightCount") {
            config.motorTurnRightCount = std::stoi(value);
        } else if (key == "motorInteractCount") {
            config.motorInteractCount = std::stoi(value);
        } else if (key == "motorWaitCount") {
            config.motorWaitCount = std::stoi(value);
        } else if (key == "sensoryVisionCount") {
            config.sensoryVisionCount = std::stoi(value);
        } else if (key == "sensoryTouchCount") {
            config.sensoryTouchCount = std::stoi(value);
        } else if (key == "sensoryInternalCount") {
            config.sensoryInternalCount = std::stoi(value);
        } else if (key == "sensoryProprioceptionCount") {
            config.sensoryProprioceptionCount = std::stoi(value);
        } else if (key == "curiosityThreshold") {
            config.curiosityThreshold = std::stof(value);
        } else if (key == "explorationThreshold") {
            config.explorationThreshold = std::stof(value);
        } else if (key == "maxExplorationChance") {
            config.maxExplorationChance = std::stof(value);
        } else if (key == "noveltyDecayFactor") {
            config.noveltyDecayFactor = std::stof(value);
        } else if (key == "plasticityModifierBase") {
            config.plasticityModifierBase = std::stof(value);
        }
    }
    
    file.close();
    return config;
}

bool AgentBrain::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# AgentBrain Configuration\n";
        file << "# Generated by AgentBrain\n\n";
        
        file << "motorForwardCount = " << config_.motorForwardCount << "\n";
        file << "motorBackwardCount = " << config_.motorBackwardCount << "\n";
        file << "motorTurnLeftCount = " << config_.motorTurnLeftCount << "\n";
        file << "motorTurnRightCount = " << config_.motorTurnRightCount << "\n";
        file << "motorInteractCount = " << config_.motorInteractCount << "\n";
        file << "motorWaitCount = " << config_.motorWaitCount << "\n\n";
        
        file << "sensoryVisionCount = " << config_.sensoryVisionCount << "\n";
        file << "sensoryTouchCount = " << config_.sensoryTouchCount << "\n";
        file << "sensoryInternalCount = " << config_.sensoryInternalCount << "\n";
        file << "sensoryProprioceptionCount = " << config_.sensoryProprioceptionCount << "\n\n";
        
        file << "curiosityThreshold = " << config_.curiosityThreshold << "\n";
        file << "explorationThreshold = " << config_.explorationThreshold << "\n";
        file << "maxExplorationChance = " << config_.maxExplorationChance << "\n";
        file << "noveltyDecayFactor = " << config_.noveltyDecayFactor << "\n";
        file << "plasticityModifierBase = " << config_.plasticityModifierBase << "\n\n";
        
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save AgentBrain config: ") + e.what());
        return false;
    }
}

void AgentBrain::addCustomMotorAction(const std::string& name, const std::vector<Neuron*>& neurons) {
    motorGroups_.push_back(neurons);
    motorGroupNames_.push_back(name);
    
    customMotorActions_.push_back([this, name]() {
        // Custom action logic here
        return MotorCommand::Wait;
    });
}

void AgentBrain::addCustomSensoryAction(const std::string& name, const std::vector<Neuron*>& neurons) {
    sensoryGroups_.push_back(neurons);
    sensoryGroupNames_.push_back(name);
    
    customSensoryActions_.push_back([this, name](const SensoryPercept& percept) {
        // Custom sensory processing logic here
    });
}

MotorCommand AgentBrain::customMotorAction(const std::string& name) {
    for (size_t i = 0; i < motorGroupNames_.size(); ++i) {
        if (motorGroupNames_[i] == name) {
            if (customMotorActions_.size() > i) {
                return customMotorActions_[i]();
            }
        }
    }
    return MotorCommand::Wait;
}

size_t AgentBrain::getMotorGroupCount(MotorCommand cmd) const {
    // Count neurons in motor groups (simplified implementation)
    size_t count = 0;
    for (const auto& group : motorGroups_) {
        count += group.size();
    }
    return count;
}

size_t AgentBrain::getSensoryGroupCount(NeuronType type) const {
    // Count neurons in sensory groups (simplified implementation)
    size_t count = 0;
    for (const auto& group : sensoryGroups_) {
        count += group.size();
    }
    return count;
}

float AgentBrain::getAverageFiringRate() const {
    if (!brain_) return 0.0f;
    
    float totalFiring = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            totalFiring += pop->getFiringRate();
            neuronCount += pop->getNeuronCount();
        }
    }
    
    return neuronCount > 0 ? totalFiring / neuronCount : 0.0f;
}

float AgentBrain::getTotalSpikesProcessed() const {
    return totalSpikesProcessed_;
}

void AgentBrain::enableCuriosity(bool enable, float threshold) {
    curiosityEnabled_ = enable;
    if (threshold > 0.0f) {
        config_.curiosityThreshold = threshold;
    }
}

void AgentBrain::enableRewardModulation(bool enable) {
    rewardModulationEnabled_ = enable;
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableDevelopment(bool enable) {
    developmentEnabled_ = enable;
}

void AgentBrain::update(std::shared_ptr<Brain> brain, const SimpleWorld& world) {
    // Convenience method for updating all systems
    if (brain) {
        brain_ = brain;
    }
    if (!initialize(world, false)) {
        logError("Failed to initialize AgentBrain in update", true);
    }
}

} // namespace nlm
