#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <chrono>
#include <random>

namespace nlm {

// AgentBrain constructor with enhanced initialization
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : config_(config)
    , brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , learningRate_(0.01f)
    , curiosityThreshold_(0.3f)
    , explorationRate_(0.1f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , developmentalPlasticityPeak_(2.0f)
    , developmentStageDuration_(60.0)  // 60 seconds per developmental stage
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , noveltyDetectionEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , temporalNoveltyWeight_(0.5f)
    , diagnosticStats_({})
{
    logWithContext("Enhanced AgentBrain created", LogLevel::Info);
}

// Move constructor
AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : config_(std::move(other.config_))
    , brain_(std::move(other.brain_))
    , sensoryGroups_(std::move(other.sensoryGroups_))
    , motorGroups_(std::move(other.motorGroups_))
    , sensoryMappings_(std::move(other.sensoryMappings_))
    , motorMappings_(std::move(other.motorMappings_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , learningRate_(other.learningRate_)
    , curiosityThreshold_(other.curiosityThreshold_)
    , explorationRate_(other.explorationRate_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , developmentalPlasticityPeak_(other.developmentalPlasticityPeak_)
    , developmentStageDuration_(other.developmentStageDuration_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , noveltyDetectionEnabled_(other.noveltyDetectionEnabled_)
    , previousVision_(std::move(other.previousVision_))
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , temporalNoveltyWeight_(other.temporalNoveltyWeight_)
    , diagnosticStats_(other.diagnosticStats_)
    , stats_(other.stats_)
{
    logWithContext("Enhanced AgentBrain moved", LogLevel::Info);
}

// Move assignment operator
AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        logWithContext("Enhanced AgentBrain assignment", LogLevel::Info);
        
        config_ = std::move(other.config_);
        brain_ = std::move(other.brain_);
        sensoryGroups_ = std::move(other.sensoryGroups_);
        motorGroups_ = std::move(other.motorGroups_);
        sensoryMappings_ = std::move(other.sensoryMappings_);
        motorMappings_ = std::move(other.motorMappings_);
        
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        learningRate_ = other.learningRate_;
        curiosityThreshold_ = other.curiosityThreshold_;
        explorationRate_ = other.explorationRate_;
        
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        developmentalPlasticityPeak_ = other.developmentalPlasticityPeak_;
        developmentStageDuration_ = other.developmentStageDuration_;
        
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        noveltyDetectionEnabled_ = other.noveltyDetectionEnabled_;
        
        previousVision_ = std::move(other.previousVision_);
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        temporalNoveltyWeight_ = other.temporalNoveltyWeight_;
        
        diagnosticStats_ = other.diagnosticStats_;
        stats_ = other.stats_;
    }
    return *this;
}

AgentBrain::~AgentBrain() {
    logWithContext("Enhanced AgentBrain destroyed", LogLevel::Info);
}

bool AgentBrain::initialize(const SimpleWorld& world) {
    logWithContext("Initializing enhanced AgentBrain", LogLevel::Info);
    
    try {
        if (!brain_) {
            logWithContext("ERROR: No brain available for initialization", LogLevel::Error);
            return false;
        }
        
        // Clear previous state
        reset();
        
        // Configure from global config if available
        if (config_) {
            learningRate_ = config_->getOr<float>("learning_rate", learningRate_);
            curiosityThreshold_ = config_->getOr<float>("curiosity_threshold", curiosityThreshold_);
            explorationRate_ = config_->getOr<float>("exploration_rate", explorationRate_);
            sensoryNoveltyDecay_ = config_->getOr<float>("sensory_novelty_decay", sensoryNoveltyDecay_);
        }
        
        // Initialize neuron groups using brain regions
        initializeNeuronGroups();
        
        // Create mappings between percepts and neurons
        initializeMappings(world);
        
        // Initialize performance stats
        resetPerformanceStats();
        
        logWithContext("Enhanced AgentBrain initialized successfully", LogLevel::Info);
        logWithContext("  - " + std::to_string(sensoryGroups_.size()) + " sensory groups", LogLevel::Info);
        logWithContext("  - " + std::to_string(motorGroups_.size()) + " motor groups", LogLevel::Info);
        logWithContext("  - " + std::to_string(sensoryMappings_.size()) + " sensory mappings", LogLevel::Info);
        logWithContext("  - " + std::to_string(motorMappings_.size()) + " motor mappings", LogLevel::Info);
        
        return true;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR during initialization: ") + e.what(), LogLevel::Error);
        return false;
    }
}

void AgentBrain::initializeNeuronGroups() {
    logWithContext("Initializing neuron groups from brain regions", LogLevel::Debug);
    
    // Clear existing groups
    sensoryGroups_.clear();
    motorGroups_.clear();
    
    if (!brain_) {
        logWithContext("WARNING: No brain available for neuron group initialization", LogLevel::Warning);
        return;
    }
    
    // Define neuron group configurations
    const std::vector<NeuronGroup> defaultSensoryGroups = {
        {NeuronType::SensoryVision, 1.0f, 0.5f, 1.0f},
        {NeuronType::SensoryTouch, 0.8f, 0.4f, 1.2f},
        {NeuronType::SensoryInternal, 0.6f, 0.3f, 1.1f},
        {NeuronType::SensoryProprioception, 0.5f, 0.2f, 0.9f}
    };
    
    const std::vector<NeuronGroup> defaultMotorGroups = {
        {NeuronType::MotorForward, 1.0f, 0.6f, 1.0f},
        {NeuronType::MotorBackward, 1.0f, 0.6f, 1.0f},
        {NeuronType::MotorTurnLeft, 1.0f, 0.6f, 1.0f},
        {NeuronType::MotorTurnRight, 1.0f, 0.6f, 1.0f},
        {NeuronType::MotorInteract, 1.0f, 0.5f, 1.1f},
        {NeuronType::MotorWait, 1.0f, 0.3f, 0.8f},
        {NeuronType::MotorLookLeft, 0.8f, 0.4f, 0.9f},
        {NeuronType::MotorLookRight, 0.8f, 0.4f, 0.9f}
    };
    
    // Copy default groups and redistribute based on actual brain regions
    sensoryGroups_ = defaultSensoryGroups;
    motorGroups_ = defaultMotorGroups;
    
    distributeNeuronsByType();
}

void AgentBrain::distributeNeuronsByType() {
    logWithContext("Distributing neurons by type across groups", LogLevel::Debug);
    
    if (!brain_) {
        logWithContext("WARNING: No brain for neuron distribution", LogLevel::Warning);
        return;
    }
    
    // Clear existing neurons
    for (auto& group : sensoryGroups_) {
        group.neurons.clear();
    }
    for (auto& group : motorGroups_) {
        group.neurons.clear();
    }
    
    // Get all neurons from brain regions
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron);
            }
        }
    }
    
    logWithContext(std::to_string(allNeurons.size()) + " total neurons found", LogLevel::Debug);
    
    // Distribute neurons to groups based on type
    size_t visionAssigned = 0, touchAssigned = 0, internalAssigned = 0, proprioceptionAssigned = 0;
    size_t forwardAssigned = 0, backwardAssigned = 0, leftAssigned = 0, rightAssigned = 0;
    size_t interactAssigned = 0, waitAssigned = 0, lookLeftAssigned = 0, lookRightAssigned = 0;
    
    for (Neuron* neuron : allNeurons) {
        if (!neuron) continue;
        
        NeuronType type = neuron->getType();
        
        // Assign to appropriate sensory group
        if (type == NeuronType::SensoryVision && visionAssigned < sensoryGroups_[0].neurons.capacity()) {
            sensoryGroups_[0].neurons.push_back(neuron);
            visionAssigned++;
        } else if (type == NeuronType::SensoryTouch && touchAssigned < sensoryGroups_[1].neurons.capacity()) {
            sensoryGroups_[1].neurons.push_back(neuron);
            touchAssigned++;
        } else if (type == NeuronType::SensoryInternal && internalAssigned < sensoryGroups_[2].neurons.capacity()) {
            sensoryGroups_[2].neurons.push_back(neuron);
            internalAssigned++;
        } else if (type == NeuronType::SensoryProprioception && proprioceptionAssigned < sensoryGroups_[3].neurons.capacity()) {
            sensoryGroups_[3].neurons.push_back(neuron);
            proprioceptionAssigned++;
        }
        // Assign to appropriate motor group
        else if (type == NeuronType::MotorForward && forwardAssigned < motorGroups_[0].neurons.capacity()) {
            motorGroups_[0].neurons.push_back(neuron);
            forwardAssigned++;
        } else if (type == NeuronType::MotorBackward && backwardAssigned < motorGroups_[1].neurons.capacity()) {
            motorGroups_[1].neurons.push_back(neuron);
            backwardAssigned++;
        } else if (type == NeuronType::MotorTurnLeft && leftAssigned < motorGroups_[2].neurons.capacity()) {
            motorGroups_[2].neurons.push_back(neuron);
            leftAssigned++;
        } else if (type == NeuronType::MotorTurnRight && rightAssigned < motorGroups_[3].neurons.capacity()) {
            motorGroups_[3].neurons.push_back(neuron);
            rightAssigned++;
        } else if (type == NeuronType::MotorInteract && interactAssigned < motorGroups_[4].neurons.capacity()) {
            motorGroups_[4].neurons.push_back(neuron);
            interactAssigned++;
        } else if (type == NeuronType::MotorWait && waitAssigned < motorGroups_[5].neurons.capacity()) {
            motorGroups_[5].neurons.push_back(neuron);
            waitAssigned++;
        } else if (type == NeuronType::MotorLookLeft && lookLeftAssigned < motorGroups_[6].neurons.capacity()) {
            motorGroups_[6].neurons.push_back(neuron);
            lookLeftAssigned++;
        } else if (type == NeuronType::MotorLookRight && lookRightAssigned < motorGroups_[7].neurons.capacity()) {
            motorGroups_[7].neurons.push_back(neuron);
            lookRightAssigned++;
        }
    }
    
    logWithContext("Neuron distribution complete:", LogLevel::Info);
    logWithContext("  Sensory: Vision=" + std::to_string(visionAssigned) + 
                   " Touch=" + std::to_string(touchAssigned) + 
                   " Internal=" + std::to_string(internalAssigned) + 
                   " Proprioception=" + std::to_string(proprioceptionAssigned), LogLevel::Info);
    logWithContext("  Motor: Forward=" + std::to_string(forwardAssigned) + 
                   " Backward=" + std::to_string(backwardAssigned) + 
                   " Left=" + std::to_string(leftAssigned) + 
                   " Right=" + std::to_string(rightAssigned) + 
                   " Interact=" + std::to_string(interactAssigned) + 
                   " Wait=" + std::to_string(waitAssigned) + 
                   " LookLeft=" + std::to_string(lookLeftAssigned) + 
                   " LookRight=" + std::to_string(lookRightAssigned), LogLevel::Info);
}

void AgentBrain::optimizeNeuronGroupAssignments() {
    logWithContext("Optimizing neuron group assignments", LogLevel::Debug);
    
    // Redistribute neurons if groups are unbalanced
    for (auto& group : sensoryGroups_) {
        if (group.neurons.size() > group.neurons.capacity() * 0.8f) {
            // Too many neurons in this group, redistribute
            logWithContext("Redistributing " + std::to_string(group.neurons.size()) + 
                          " neurons from " + neuronTypeToString(group.type) + " group", LogLevel::Debug);
            group.neurons.clear();
        }
    }
    
    // Refresh distribution if necessary
    if (diagnosticStats_.totalSensoryInputs > 1000) {
        distributeNeuronsByType();
        resetPerformanceStats();
        diagnosticStats_.totalSensoryInputs = 0;
    }
}

void AgentBrain::initializeMappings(const SimpleWorld& world) {
    logWithContext("Initializing sensor-motor mappings", LogLevel::Debug);
    
    sensoryMappings_.clear();
    motorMappings_.clear();
    
    // Configure sensory mappings based on world properties
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    size_t visionSize = visionWidth * visionHeight;
    
    // Map vision percept to sensoryVision_ neurons
    if (!sensoryGroups_[0].neurons.empty() && visionSize > 0) {
        SensoryMapping visionMapping;
        visionMapping.perceptIndex = 0;  // Vision is first in percept
        visionMapping.neuronGroupIndex = 0;  // First sensory group (vision)
        visionMapping.scalingFactor = 1.0f / visionSize;
        visionMapping.offset = 0.0f;
        visionMapping.enabled = true;
        sensoryMappings_.push_back(visionMapping);
    }
    
    // Map touch percept to sensoryTouch_ neurons
    if (!sensoryGroups_[1].neurons.empty() && !world.getTouchPoints().empty()) {
        SensoryMapping touchMapping;
        touchMapping.perceptIndex = 1;  // Touch is second in percept
        touchMapping.neuronGroupIndex = 1;  // Second sensory group (touch)
        touchMapping.scalingFactor = 1.0f;
        touchMapping.offset = 0.0f;
        touchMapping.enabled = true;
        sensoryMappings_.push_back(touchMapping);
    }
    
    // Configure motor mappings based on available motor commands
    const std::vector<MotorCommand> motorCommands = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait,
        MotorCommand::LookLeft, MotorCommand::LookRight
    };
    
    for (size_t i = 0; i < motorCommands.size() && i < motorGroups_.size(); ++i) {
        MotorMapping mapping;
        mapping.command = motorCommands[i];
        mapping.neuronGroupIndex = i;
        mapping.activationThreshold = 0.5f;
        mapping.confidence = 0.0f;
        motorMappings_.push_back(mapping);
    }
    
    logWithContext(std::to_string(sensoryMappings_.size()) + " sensory mappings, " + 
                   std::to_string(motorMappings_.size()) + " motor mappings created", LogLevel::Debug);
}

bool AgentBrain::validateSensoryInput(const SensoryPercept& percept) const {
    // Basic validation of sensory input
    if (percept.getVision().size() > 0 && 
        percept.getVision().size() != previousVision_.size()) {
        // Vision size should be consistent
        return false;
    }
    
    // Check for NaN or infinite values
    for (float val : percept.getVision()) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    return true;
}

bool AgentBrain::validateNeuronGroups() const {
    // Check that all neuron groups have valid neurons
    for (const auto& group : sensoryGroups_) {
        for (Neuron* neuron : group.neurons) {
            if (!neuron || neuron->getType() != group.type) {
                return false;
            }
        }
    }
    
    for (const auto& group : motorGroups_) {
        for (Neuron* neuron : group.neurons) {
            if (!neuron || neuron->getType() != group.type) {
                return false;
            }
        }
    }
    
    return true;
}

void AgentBrain::cleanupInvalidNeurons() {
    logWithContext("Cleaning up invalid neurons", LogLevel::Debug);
    
    // Remove null pointers and invalid neurons
    for (auto& group : sensoryGroups_) {
        group.neurons.erase(std::remove(group.neurons.begin(), group.neurons.end(), nullptr), group.neurons.end());
    }
    
    for (auto& group : motorGroups_) {
        group.neurons.erase(std::remove(group.neurons.begin(), group.neurons.end(), nullptr), group.neurons.end());
    }
    
    // Re-optimize distribution if necessary
    optimizeNeuronGroupAssignments();
}

bool AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Validate input
        if (!validateSensoryInput(percept)) {
            logWithContext("Invalid sensory input detected", LogLevel::Warning);
            return false;
        }
        
        // Clean up invalid neurons
        cleanupInvalidNeurons();
        
        // Process vision input
        if (!sensoryGroups_[0].neurons.empty()) {
            const auto& vision = percept.getVision();
            for (size_t i = 0; i < sensoryGroups_[0].neurons.size() && i < vision.size(); ++i) {
                Neuron* neuron = sensoryGroups_[0].neurons[i];
                if (neuron) {
                    float current = vision[i] * 5.0f;  // Scale factor with configurable sensitivity
                    neuron->injectCurrent(current);
                }
            }
        }
        
        // Process touch input
        if (!sensoryGroups_[1].neurons.empty()) {
            const auto& touch = percept.getTouch();
            for (size_t i = 0; i < sensoryGroups_[1].neurons.size() && i < touch.size(); ++i) {
                Neuron* neuron = sensoryGroups_[1].neurons[i];
                if (neuron) {
                    float current = touch[i] * 8.0f;  // Collision signal
                    neuron->injectCurrent(current);
                }
            }
        }
        
        // Process internal signals
        if (!sensoryGroups_[2].neurons.empty()) {
            const auto& intern = percept.getInternal();
            for (size_t i = 0; i < sensoryGroups_[2].neurons.size() && i < intern.size(); ++i) {
                Neuron* neuron = sensoryGroups_[2].neurons[i];
                if (neuron) {
                    float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                    neuron->injectCurrent(current);
                }
            }
        }
        
        // Process proprioception
        if (!sensoryGroups_[3].neurons.empty()) {
            const auto& proprio = percept.getProprioception();
            for (size_t i = 0; i < sensoryGroups_[3].neurons.size() && i < proprio.size(); ++i) {
                Neuron* neuron = sensoryGroups_[3].neurons[i];
                if (neuron) {
                    float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                    neuron->injectCurrent(current);
                }
            }
        }
        
        // Update novelty detection
        if (noveltyDetectionEnabled_) {
            updateNoveltyDetection(percept.getVision());
        }
        
        // Update performance metrics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        updatePerformanceStats(duration.count() * 0.001, 0);  // 0 = sensory processing
        
        diagnosticStats_.totalSensoryInputs++;
        
        logWithContext("Sensory input processed successfully", LogLevel::Debug);
        return true;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR processing sensory input: ") + e.what(), LogLevel::Error);
        return false;
    }
}

void AgentBrain::updateNoveltyDetection(const std::vector<float>& currentVision) {
    if (previousVision_.empty()) {
        previousVision_ = currentVision;
        return;
    }
    
    float novelty = computeNovelty(currentVision);
    noveltyLevel_ = novelty;
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Decay novelty
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next comparison
    previousVision_ = currentVision;
    
    if (novelty > 0.3f) {
        diagnosticStats_.adaptationEvents++;
        logWithContext("Novel stimulus detected: novelty=" + std::to_string(novelty), LogLevel::Info);
    }
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
    
    // Normalize
    return totalDiff / std::max<size_t>(minSize, 1);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Use enhanced weighted decoding
        MotorCommand command = decodeFromMotorNeuronsWeighted();
        
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
            command = selectWithCuriosityAdvanced(command);
        }
        
        // Update performance metrics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        updatePerformanceStats(duration.count() * 0.001, 1);  // 1 = motor decoding
        
        diagnosticStats_.totalMotorOutputs++;
        if (command != MotorCommand::Wait) {
            diagnosticStats_.successfulDecodings++;
        }
        
        logWithContext("Motor command decoded: " + motorCommandToString(command), LogLevel::Debug);
        return command;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR decoding motor command: ") + e.what(), LogLevel::Error);
        return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::decodeFromMotorNeuronsWeighted() {
    if (!brain_) {
        logWithContext("ERROR: No brain for motor decoding", LogLevel::Error);
        return MotorCommand::Wait;
    }
    
    // Calculate weighted activity for each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                // Use membrane potential deviation from rest as activity measure
                float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
                sum += activity;
            }
        }
        return sum / neurons.size();
    };
    
    std::vector<float> groupActivities;
    std::vector<MotorCommand> groupCommands;
    
    for (size_t i = 0; i < motorGroups_.size(); ++i) {
        float activity = calcActivity(motorGroups_[i].neurons);
        groupActivities.push_back(activity);
        
        // Map group index to command (adjust based on actual motor groups)
        if (i < 6) {
            groupCommands.push_back(static_cast<MotorCommand>(i));
        } else {
            // Additional motor commands
            switch (i) {
                case 6: groupCommands.push_back(MotorCommand::LookLeft); break;
                case 7: groupCommands.push_back(MotorCommand::LookRight); break;
                default: groupCommands.push_back(MotorCommand::Wait); break;
            }
        }
    }
    
    // Find maximum activity with weighted voting
    float maxActivity = 0.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (size_t i = 0; i < groupActivities.size(); ++i) {
        if (groupActivities[i] > maxActivity) {
            maxActivity = groupActivities[i];
            bestCommand = groupCommands[i];
        }
    }
    
    // Only act if there's meaningful activity
    if (maxActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return bestCommand;
}

MotorCommand AgentBrain::selectWithCuriosityAdvanced(MotorCommand defaultCmd) {
    // Enhanced exploration with probabilistic sampling
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationRate_;  // Up to configurable percentage
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        float r = dis(gen);
        if (r < exploreChance) {
            // Probabilistic exploration
            std::uniform_int_distribution<size_t> actionDist(0, 7); // 8 possible actions
            int choice = actionDist(gen);
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
    if (!brain_) {
        logWithContext("ERROR: No brain for reward modulation", LogLevel::Error);
        return;
    }
    
    applyAdvancedRewardModulation(reward, predictedReward);
}

void AgentBrain::applyAdvancedRewardModulation(float reward, float predictedReward) {
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
                float delta = eligibility * dopamineLevel_ * learningRate_ * plasticityModifier_;
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
        //brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        //brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        //brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        //brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
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

void AgentBrain::updatePerformanceStats(double processingTime, size_t operationType) {
    stats_.totalSensoryProcessingTime += (operationType == 0) ? processingTime : 0.0;
    stats_.totalMotorDecodingTime += (operationType == 1) ? processingTime : 0.0;
    stats_.sensoryProcessingCount += (operationType == 0) ? 1 : 0;
    stats_.motorDecodingCount += (operationType == 1) ? 1 : 0;
}

bool AgentBrain::saveAgentState(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        logWithContext("ERROR: Cannot open file for saving: " + filename, LogLevel::Error);
        return false;
    }
    
    try {
        // Save basic state (simplified for demonstration)
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(predictionError_));
        file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(expectedReward_));
        file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        file.close();
        logWithContext("Agent state saved to: " + filename, LogLevel::Info);
        return true;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR saving agent state: ") + e.what(), LogLevel::Error);
        file.close();
        return false;
    }
}

bool AgentBrain::loadAgentState(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        logWithContext("ERROR: Cannot open file for loading: " + filename, LogLevel::Error);
        return false;
    }
    
    try {
        // Load basic state
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(predictionError_));
        file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(expectedReward_));
        file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        file.close();
        logWithContext("Agent state loaded from: " + filename, LogLevel::Info);
        return true;
        
    } catch (const std::exception& e) {
        logWithContext(std::string("ERROR loading agent state: ") + e.what(), LogLevel::Error);
        file.close();
        return false;
    }
}

void AgentBrain::reset() {
    logWithContext("Resetting AgentBrain state", LogLevel::Info);
    
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear sensory history
    previousVision_.clear();
    
    // Reset diagnostics
    diagnosticStats_ = DiagnosticStats();
    
    // Reset performance stats
    resetPerformanceStats();
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    // Simple developmental stage calculation based on age
    if (developmentalAge_ < 60.0) {
        return DevelopmentalStage::Initial;
    } else if (developmentalAge_ < 300.0) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < 900.0) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

// Helper functions
std::string AgentBrain::neuronTypeToString(NeuronType type) const {
    switch (type) {
        case NeuronType::SensoryVision: return "Vision";
        case NeuronType::SensoryTouch: return "Touch";
        case NeuronType::SensoryInternal: return "Internal";
        case NeuronType::SensoryProprioception: return "Proprioception";
        case NeuronType::MotorForward: return "Forward";
        case NeuronType::MotorBackward: return "Backward";
        case NeuronType::MotorTurnLeft: return "TurnLeft";
        case NeuronType::MotorTurnRight: return "TurnRight";
        case NeuronType::MotorInteract: return "Interact";
        case NeuronType::MotorWait: return "Wait";
        case NeuronType::MotorLookLeft: return "LookLeft";
        case NeuronType::MotorLookRight: return "LookRight";
        default: return "Unknown";
    }
}

std::string AgentBrain::motorCommandToString(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        default: return "Unknown";
    }
}

void AgentBrain::logWithContext(const std::string& message, LogLevel level, const std::string& context) {
    std::ostringstream oss;
    if (!context.empty()) {
        oss << "[AgentBrain] " << context << ": " << message;
    } else {
        oss << "[AgentBrain] " << message;
    }
    
    if (level == LogLevel::Info) {
        NLM_LOG_INFO(oss.str());
    } else if (level == LogLevel::Warning) {
        NLM_LOG_WARN(oss.str());
    } else if (level == LogLevel::Error) {
        NLM_LOG_ERROR(oss.str());
    }
}

// Note: Some getters that are needed but may need to be implemented based on actual requirements
// The above implementation provides a solid foundation for an enhanced AgentBrain class

} // namespace nlm

// Helper function to make AgentBrain.hpp complete
namespace nlm {
    inline size_t AgentBrain::getSensoryInputSize() const {
        return 256 + 8 + 4 + 6;  // Vision + Touch + Internal + Proprioception
    }
    
    inline size_t AgentBrain::getMotorOutputSize() const {
        return 6;  // One motor neuron per basic action
    }
}