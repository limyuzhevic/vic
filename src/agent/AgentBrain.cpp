#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
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
    , config_(config)
    , previousNoveltyLevel_(0.0f)
{
    // Initialize random number generator for reproducible but varied behavior
    rng_.seed(std::random_device{}());
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input
    if (config_.strictNullChecks && !world.getVisionWidth()) {
        NLM_LOG_WARN("Initializing AgentBrain with empty vision field");
    }
    
    // Calculate optimal vision buffer size based on world dimensions
    size_t optimalBufferSize = world.getVisionWidth() * world.getVisionHeight();
    if (optimalBufferSize > config_.maxVisionBufferSize) {
        NLM_LOG_WARN("Vision buffer size exceeds maximum, will be truncated");
        optimalBufferSize = config_.maxVisionBufferSize;
    }
    
    // Resize vision buffer with initialization
    previousVision_.clear();
    previousVision_.reserve(optimalBufferSize);
    previousVision_.resize(optimalBufferSize, 0.0f);
    
    // Initialize motor and sensory neuron groups with balanced distribution
    if (brain_) {
        // Pre-collect all neurons by type and population for better distribution
        std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>> neuronsByTypeAndPop;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                neuronsByTypeAndPop[type].push_back(pop->getNeurons());
            }
        }
        
        // Distribute neurons more intelligently
        if (config_.balancedNeuronDistribution) {
            distributeBalancedNeurons(neuronsByTypeAndPop);
        } else {
            // Fallback to original distribution for backward compatibility
            distributeOriginalNeurons(neuronsByTypeAndPop);
        }
    }
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization summary
    size_t totalMotorNeurons = getTotalMotorNeurons();
    size_t totalSensoryNeurons = getTotalSensoryNeurons();
    
    NLM_LOG_INFO("AgentBrain initialized: " + 
                 std::to_string(totalMotorNeurons) + " motor neurons, " +
                 std::to_string(totalSensoryNeurons) + " sensory neurons, " +
                 std::to_string(optimalBufferSize) + " vision buffer");
}

// Rest of the file would be here...

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate input
    if (config_.strictNullChecks && !world.getVisionWidth()) {
        NLM_LOG_WARN("Initializing AgentBrain with empty vision field");
    }
    
    // Calculate optimal vision buffer size based on world dimensions
    size_t optimalBufferSize = world.getVisionWidth() * world.getVisionHeight();
    if (optimalBufferSize > config_.maxVisionBufferSize) {
        NLM_LOG_WARN("Vision buffer size exceeds maximum, will be truncated");
        optimalBufferSize = config_.maxVisionBufferSize;
    }
    
    // Resize vision buffer with initialization
    previousVision_.clear();
    previousVision_.reserve(optimalBufferSize);
    previousVision_.resize(optimalBufferSize, 0.0f);
    
    // Track seen neurons for balanced distribution
    std::unordered_map<size_t, size_t> neuronCounts;
    
    // Initialize motor and sensory neuron groups with balanced distribution
    if (brain_) {
        // Pre-collect all neurons by type and population for better distribution
        std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>> neuronsByTypeAndPop;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                neuronsByTypeAndPop[type].push_back(pop->getNeurons());
            }
        }
        
        // Distribute neurons more intelligently
        if (config_.balancedNeuronDistribution) {
            distributeBalancedNeurons(neuronsByTypeAndPop);
        } else {
            // Fallback to original distribution for backward compatibility
            distributeOriginalNeurons(neuronsByTypeAndPop);
        }
    }
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization summary
    size_t totalMotorNeurons = getTotalMotorNeurons();
    size_t totalSensoryNeurons = getTotalSensoryNeurons();
    
    NLM_LOG_INFO("AgentBrain initialized: " + 
                 std::to_string(totalMotorNeurons) + " motor neurons, " +
                 std::to_string(totalSensoryNeurons) + " sensory neurons, " +
                 std::to_string(optimalBufferSize) + " vision buffer");
}

void AgentBrain::distributeBalancedNeurons(const std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>>& neuronsByType) {
    if (!brain_) return;
    
    // Clear existing groups
    clearNeuronGroups();
    
    // Process motor neurons with balanced distribution
    auto motorIt = neuronsByType.find(NeuronType::Motor);
    if (motorIt != neuronsByType.end()) {
        distributeMotorNeuronsBalanced(motorIt->second);
    }
    
    // Process sensory neurons with balanced distribution
    auto sensoryIt = neuronsByType.find(NeuronType::Sensory);
    if (sensoryIt != neuronsByType.end()) {
        distributeSensoryNeuronsBalanced(sensoryIt->second);
    }
}

void AgentBrain::distributeMotorNeuronsBalanced(const std::vector<std::vector<Neuron*>>& neuronPopulations) {
    if (neuronPopulations.empty()) return;
    
    // Flatten all motor neurons
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& pop : neuronPopulations) {
        allMotorNeurons.insert(allMotorNeurons.end(), pop.begin(), pop.end());
    }
    
    if (allMotorNeurons.empty()) return;
    
    // Determine optimal distribution strategy
    size_t numGroups = config_.maxMotorGroups;
    if (numGroups < 2) numGroups = 2;
    
    // Create distribution based on neuron count and group balance
    size_t neuronsPerGroup = allMotorNeurons.size() / numGroups;
    size_t remainder = allMotorNeurons.size() % numGroups;
    
    // Clear groups and redistribute
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Distribute neurons to groups
    size_t currentNeuron = 0;
    std::vector<std::vector<Neuron*>*> groups = {
        &motorForward_, &motorBackward_, &motorTurnLeft_,
        &motorTurnRight_, &motorInteract_, &motorWait_
    };
    
    // Give extra neurons to first groups if there's a remainder
    for (size_t i = 0; i < numGroups && i < groups.size(); ++i) {
        size_t targetCount = neuronsPerGroup + (i < remainder ? 1 : 0);
        
        // Assign neurons to this group
        for (size_t j = 0; j < targetCount && currentNeuron < allMotorNeurons.size(); ++j) {
            groups[i]->push_back(allMotorNeurons[currentNeuron++]);
        }
        
        // If we still have neurons but ran out of defined groups,
        // add them to the largest group
        if (currentNeuron < allMotorNeurons.size() && i == numGroups - 1) {
            // Find largest group and add extras there
            size_t largestGroup = 0;
            size_t maxSize = groups[0]->size();
            for (size_t k = 1; k < groups.size(); ++k) {
                if (groups[k]->size() > maxSize) {
                    largestGroup = k;
                    maxSize = groups[k]->size();
                }
            }
            groups[largestGroup]->push_back(allMotorNeurons[currentNeuron++]);
        }
    }
}

void AgentBrain::distributeSensoryNeuronsBalanced(const std::vector<std::vector<Neuron*>>& neuronPopulations) {
    if (neuronPopulations.empty()) return;
    
    // Flatten all sensory neurons
    std::vector<Neuron*> allSensoryNeurons;
    for (const auto& pop : neuronPopulations) {
        allSensoryNeurons.insert(allSensoryNeurons.end(), pop.begin(), pop.end());
    }
    
    if (allSensoryNeurons.empty()) return;
    
    // Distribute to 4 sensory groups
    size_t numGroups = 4;
    size_t neuronsPerGroup = allSensoryNeurons.size() / numGroups;
    size_t remainder = allSensoryNeurons.size() % numGroups;
    
    // Clear groups and redistribute
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    std::vector<std::vector<Neuron*>*> groups = {
        &sensoryVision_, &sensoryTouch_, &sensoryInternal_, &sensoryProprioception_
    };
    
    size_t currentNeuron = 0;
    for (size_t i = 0; i < numGroups && i < groups.size(); ++i) {
        size_t targetCount = neuronsPerGroup + (i < remainder ? 1 : 0);
        
        for (size_t j = 0; j < targetCount && currentNeuron < allSensoryNeurons.size(); ++j) {
            groups[i]->push_back(allSensoryNeurons[currentNeuron++]);
        }
    }
}

void AgentBrain::distributeOriginalNeurons(const std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>>& neuronsByType) {
    // Original modulo-based distribution for backward compatibility
    if (!brain_) return;
    
    clearNeuronGroups();
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Original distribution logic
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
                for (Neuron* n : pop->getNeurons()) {
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

void AgentBrain::clearNeuronGroups() {
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
}

size_t AgentBrain::getTotalMotorNeurons() const {
    return motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
           motorTurnRight_.size() + motorInteract_.size() + motorWait_.size();
}

size_t AgentBrain::getTotalSensoryNeurons() const {
    return sensoryVision_.size() + sensoryTouch_.size() + 
           sensoryInternal_.size() + sensoryProprioception_.size();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Validate inputs
    if (config_.strictNullChecks) {
        if (!brain_) {
            NLM_LOG_WARN("Processing sensory input with null brain");
            return;
        }
        if (! percept.getVision()) {
            NLM_LOG_WARN("Vision pointer is null in sensory input");
            return;
        }
    }
    
    // Process vision input efficiently
    processVisionInput(percept);
    
    // Process other sensory modalities
    processTouchInput(percept);
    processInternalInput(percept);
    processProprioceptionInput(percept);
    
    // Update novelty and curiosity
    updateNoveltyAndCuriosity(percept);
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (vision.empty()) return;
    
    // Efficient vision processing with bounds checking
    size_t maxNeurons = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < maxNeurons; ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (!neuron) {
            if (config_.strictNullChecks) {
                NLM_LOG_ERROR("Null sensory neuron in vision group");
            }
            continue;
        }
        
        // Scale and inject current with safety clamping
        float current = vision[i] * 5.0f;
        current = std::clamp(current, config_.safetyClampMin, config_.safetyClampMax);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    if (touch.empty()) return;
    
    size_t maxNeurons = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < maxNeurons; ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (!neuron) {
            if (config_.strictNullChecks) {
                NLM_LOG_ERROR("Null sensory neuron in touch group");
            }
            continue;
        }
        
        float current = touch[i] * 8.0f;
        current = std::clamp(current, config_.safetyClampMin, config_.safetyClampMax);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    if (intern.empty()) return;
    
    size_t maxNeurons = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < maxNeurons; ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (!neuron) {
            if (config_.strictNullChecks) {
                NLM_LOG_ERROR("Null sensory neuron in internal group");
            }
            continue;
        }
        
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
        current = std::clamp(current, config_.safetyClampMin, config_.safetyClampMax);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    if (proprio.empty()) return;
    
    size_t maxNeurons = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < maxNeurons; ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (!neuron) {
            if (config_.strictNullChecks) {
                NLM_LOG_ERROR("Null sensory neuron in proprioception group");
            }
            continue;
        }
        
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
        current = std::clamp(current, config_.safetyClampMin, config_.safetyClampMax);
        neuron->injectCurrent(current);
    }
}

void AgentBrain::updateNoveltyAndCuriosity(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (vision.empty()) return;
    
    // Memory-efficient novelty detection
    if (config_.useDeltaEncoding && !previousVision_.empty()) {
        // Use incremental difference detection
        noveltyLevel_ = computeDeltaNovelty(vision);
    } else {
        // Traditional difference calculation (kept for compatibility)
        float totalDiff = 0.0f;
        size_t maxSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < maxSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    }
    
    // Apply smoothing if enabled
    if (config_.noveltySmoothingFactor > 0.0f && config_.noveltySmoothingFactor < 1.0f) {
        noveltyLevel_ = noveltyLevel_ * (1.0f - config_.noveltySmoothingFactor) + 
                       previousNoveltyLevel_ * config_.noveltySmoothingFactor;
    }
    previousNoveltyLevel_ = noveltyLevel_;
    
    // Decay novelty
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Clamp novelty level
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    
    // Update curiosity based on novelty and prediction error
    updateCuriositySystem();
    
    // Store vision for next iteration with size optimization
    if (vision.size() > previousVision_.size()) {
        previousVision_.resize(vision.size(), 0.0f);
    }
    std::copy(vision.begin(), vision.begin() + std::min(vision.size(), previousVision_.size()), 
              previousVision_.begin());
}

float AgentBrain::computeDeltaNovelty(const std::vector<float>& currentVision) {
    // Memory-efficient delta encoding for novelty detection
    float totalDiff = 0.0f;
    size_t count = 0;
    
    // Process vision vector incrementally
    size_t maxSize = std::min(currentVision.size(), previousVision_.size());
    
    for (size_t i = 0; i < maxSize; ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        
        // Only count significant changes
        if (diff > config_.noveltyMinDifference) {
            totalDiff += diff;
            count++;
        }
    }
    
    // Add contribution from new elements
    if (currentVision.size() > previousVision_.size()) {
        for (size_t i = previousVision_.size(); i < currentVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - 0.0f); // Previous is 0
            if (diff > config_.noveltyMinDifference) {
                totalDiff += diff;
                count++;
            }
        }
    }
    
    // Normalize by effective count
    float novelty = count > 0 ? (totalDiff / count) : 0.0f;
    
    // Apply nonlinearity to emphasize significant novelties
    novelty = std::tanh(novelty * 2.0f);
    
    return novelty;
}

void AgentBrain::updateCuriositySystem() {
    if (!curiosityEnabled_) return;
    
    // Adaptive curiosity calculation
    float noveltyComponent = noveltyLevel_ * config_.noveltyInfluence;
    float errorComponent = std::abs(predictionError_) * config_.predictionErrorInfluence;
    
    // Combine with adaptive weighting
    float rawCuriosity = noveltyComponent + errorComponent;
    
    // Apply adaptive learning rate
    curiosityLevel_ = curiosityLevel_ * (1.0f - config_.curiosityAdaptationRate) + 
                     rawCuriosity * config_.curiosityAdaptationRate;
    
    // Apply decay
    curiosityLevel_ *= config_.curiosityDecayRate;
    
    // Clamp to reasonable range
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Cap exploration at configurable max
    float maxExploration = 0.3f; // Could be made configurable
    curiosityLevel_ = std::min(curiosityLevel_, maxExploration);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (config_.strictNullChecks && !brain_) {
        NLM_LOG_WARN("Decoding motor command with null brain");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with adaptive thresholds
    if (curiosityEnabled_ && curiosityLevel_ > config_.motorThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        int validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) {
                if (config_.strictNullChecks) {
                    NLM_LOG_ERROR("Null neuron in activity calculation");
                }
                continue;
            }
            
            // Get state with safety checks
            NeuronState state = n->getState();
            sum += std::abs(state.membranePotential - state.restingPotential);
            validNeurons++;
        }
        
        return validNeurons > 0 ? (sum / validNeurons) : 0.0f;
    };
    
    // Precompute group activities
    std::array<float, 6> activities = {
        calcActivity(motorForward_),
        calcActivity(motorBackward_),
        calcActivity(motorTurnLeft_),
        calcActivity(motorTurnRight_),
        calcActivity(motorInteract_),
        calcActivity(motorWait_)
    };
    
    // Determine dynamic threshold based on overall activity
    float overallActivity = 0.0f;
    float totalValidGroups = 0.0f;
    
    for (float act : activities) {
        if (act > 0.0f) {
            overallActivity += act;
            totalValidGroups++;
        }
    }
    
    float avgActivity = totalValidGroups > 0 ? (overallActivity / totalValidGroups) : 0.0f;
    
    // Adapt threshold based on overall activity
    float dynamicThreshold = config_.motorThreshold;
    if (config_.motorThresholdAdaptRate > 0.0f) {
        float thresholdAdjustment = (avgActivity - dynamicThreshold) * config_.motorThresholdAdaptRate;
        dynamicThreshold += thresholdAdjustment;
        dynamicThreshold = std::clamp(dynamicThreshold, config_.motorThresholdMin, config_.motorThresholdMax);
    }
    
    // Find maximum activity
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    std::vector<CommandWithActivity> commands = {
        {MotorCommand::MoveForward, activities[0]},
        {MotorCommand::MoveBackward, activities[1]},
        {MotorCommand::TurnLeft, activities[2]},
        {MotorCommand::TurnRight, activities[3]},
        {MotorCommand::Interact, activities[4]},
        {MotorCommand::Wait, activities[5]}
    };
    
    // Sort by activity (simple selection for efficiency)
    std::sort(commands.begin(), commands.end(),
              [](const CommandWithActivity& a, const CommandWithActivity& b) {
                  return a.activity > b.activity;
              });
    
    // Use adapted threshold
    if (commands.empty() || commands[0].activity < dynamicThreshold) {
        return MotorCommand::Wait;
    }
    
    return commands[0].cmd;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (config_.strictNullChecks && !brain_) {
        NLM_LOG_WARN("Selecting with curiosity with null brain");
        return defaultCmd;
    }
    
    // Exploration with adaptive curiosity
    if (curiosityLevel_ > 0.5f) {
        // Adaptive exploration rate based on curiosity
        float exploreChance = curiosityLevel_ * config_.curiosityBaseExplorationRate;
        
        // Generate random number with better distribution
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < exploreChance) {
            // Random motor command selection
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            
            // Map to commands including Look commands
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
    if (config_.strictNullChecks && (!brain_ || !rewardModulationEnabled_)) {
        if (!brain_) NLM_LOG_WARN("Applying reward modulation with null brain");
        if (!rewardModulationEnabled_) NLM_LOG_WARN("Reward modulation disabled");
        return;
    }
    
    // Compute prediction error with safety
    predictionError_ = reward - predictedReward;
    predictionError_ = std::clamp(predictionError_, config_.safetyClampMin, config_.safetyClampMax);
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    expectedReward_ = std::clamp(expectedReward_, config_.safetyClampMin, config_.safetyClampMax);
    
    // Dopamine-like signal with adaptive scaling
    dopamineLevel_ = predictionError_ * plasticityModifier_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                if (config_.strictNullChecks) {
                    NLM_LOG_ERROR("Null synapse in reward modulation");
                }
                continue;
            }
            
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
    
    // Modulate plasticity based on dopamine (adaptive)
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
    if (config_.strictNullChecks && (!brain_ || !developmentEnabled_)) {
        if (!brain_) NLM_LOG_WARN("Updating development with null brain");
        if (!developmentEnabled_) NLM_LOG_WARN("Development disabled");
        return;
    }
    
    // Update developmental age
    developmentalAge_ += timestep;
    
    // Adaptive developmental stages with configurable timing
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
    
    // Structural plasticity changes with development (adaptive)
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Adaptive synaptogenesis and pruning rates
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (config_.strictNullChecks && !brain_) {
        NLM_LOG_WARN("Getting developmental stage with null brain");
        return DevelopmentalStage::Initial;
    }
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
    // Reset all state variables
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear vision buffer efficiently
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset novelty tracking
    previousNoveltyLevel_ = 0.0f;
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

// Configuration method implementations
void AgentBrain::setNeuronDistributionStrategy(BalancedDistributionStrategy strategy) {
    distributionStrategy_ = strategy;
}

BalancedDistributionStrategy AgentBrain::getNeuronDistributionStrategy() const {
    return distributionStrategy_;
}

void AgentBrain::setMotorThreshold(float threshold) {
    config_.motorThreshold = std::clamp(threshold, config_.motorThresholdMin, config_.motorThresholdMax);
}

void AgentBrain::setCuriosityParameters(float baseRate, float adaptationRate, float decayRate) {
    config_.curiosityBaseExplorationRate = baseRate;
    config_.curiosityAdaptationRate = adaptationRate;
    config_.curiosityDecayRate = decayRate;
}

void AgentBrain::enableMemoryEfficiency(bool enable) {
    config_.useDeltaEncoding = enable;
    config_.precomputeGroupStatistics = enable;
}

void AgentBrain::setSafetyMode(bool strict) {
    config_.strictNullChecks = strict;
}

AgentBrainConfig AgentBrain::getConfig() const {
    return config_;
}

void AgentBrain::updateConfig(const AgentBrainConfig& newConfig) {
    config_ = newConfig;
    
    // Apply safety constraints
    config_.motorThreshold = std::clamp(config_.motorThreshold, 
                                       config_.motorThresholdMin, 
                                       config_.motorThresholdMax);
    
    NLM_LOG_INFO("AgentBrain configuration updated");
}

// Private helper method
void AgentBrain::reinitializeGroups() {
    if (!brain_) return;
    
    // Recreate groups based on current brain structure
    clearNeuronGroups();
    
    // Collect neurons by type and distribution strategy
    std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>> neuronsByType;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            neuronsByType[type].push_back(pop->getNeurons());
        }
    }
    
    // Use configured distribution strategy
    switch (distributionStrategy_) {
        case BalancedDistributionStrategy::Balanced:
            distributeBalancedNeurons(neuronsByType);
            break;
        case BalancedDistributionStrategy::Original:
            distributeOriginalNeurons(neuronsByType);
            break;
    }
}

} // namespace nlm