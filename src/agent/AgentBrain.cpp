#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
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
                    for (Neuron* n : pop->getNeurons()) {
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
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Advanced initialization with comprehensive validation
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize: Brain pointer is null - cannot initialize");
        return;
    }
    
    // Validate world dimensions to prevent buffer overflows
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    
    if (visionWidth == 0 || visionHeight == 0) {
        NLM_LOG_WARN("AgentBrain::initialize: Invalid world dimensions");
        visionWidth = 16;
        visionHeight = 16;
    }
    
    // Pre-allocate memory efficiently based on world size
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    
    // Initialize neuron group vectors with optimal capacity
    size_t estimatedPopCount = brain_->getRegionCount() * 2;
    motorForward_.reserve(estimatedPopCount);
    motorBackward_.reserve(estimatedPopCount);
    motorTurnLeft_.reserve(estimatedPopCount);
    motorTurnRight_.reserve(estimatedPopCount);
    motorInteract_.reserve(estimatedPopCount);
    motorWait_.reserve(estimatedPopCount);
    
    sensoryVision_.reserve(estimatedPopCount);
    sensoryTouch_.reserve(estimatedPopCount);
    sensoryInternal_.reserve(estimatedPopCount);
    sensoryProprioception_.reserve(estimatedPopCount);
    
    // Advanced neuron group distribution with load balancing
    if (brain_) {
        size_t motorAssigned = 0;
        size_t sensoryAssigned = 0;
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {  // Validate neuron pointer
                            // Smart distribution based on current load
                            if (motorForward_.size() <= motorBackward_.size()) {
                                if (motorTurnLeft_.size() <= motorTurnRight_.size()) {
                                    motorForward_.push_back(n);
                                } else {
                                    motorTurnLeft_.push_back(n);
                                }
                            } else if (motorTurnLeft_.size() <= motorInteract_.size()) {
                                motorTurnLeft_.push_back(n);
                            } else {
                                motorWait_.push_back(n);
                            }
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (n) {  // Validate neuron pointer
                            // Balanced distribution of sensory neurons
                            if (sensoryVision_.size() <= sensoryTouch_.size()) {
                                if (sensoryInternal_.size() <= sensoryProprioception_.size()) {
                                    sensoryVision_.push_back(n);
                                } else {
                                    sensoryInternal_.push_back(n);
                                }
                            } else {
                                sensoryTouch_.push_back(n);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Initialize developmental state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Enhanced logging with performance metrics
    NLM_LOG_INFO("AgentBrain initialized with optimized neuron distribution:");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                       motorTurnLeft_.size() + motorTurnRight_.size() + 
                       motorInteract_.size() + motorWait_.size()) + " total");
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                        sensoryInternal_.size() + sensoryProprioception_.size()) + " total");
    NLM_LOG_INFO("  Vision capacity: " + std::to_string(visionWidth * visionHeight) + " pixels");
    NLM_LOG_INFO("  Novelty decay factor: " + std::to_string(sensoryNoveltyDecay_));
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
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
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Brain is not initialized");
        return MotorCommand::Wait;
    }
    
    // Advanced motor decoding with multiple strategies
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply multiple layers of exploration and decision making
    decoded = applyExplorationStrategy(decoded);
    decoded = applyCuriosityBasedBehavior(decoded);
    
    // Validate the decoded command
    if (!isValidMotorCommand(decoded)) {
        NLM_LOG_WARN("AgentBrain::decodeMotorCommand: Invalid command, defaulting to WAIT");
        return MotorCommand::Wait;
    }
    
    return decoded;
}

MotorCommand AgentBrain::applyExplorationStrategy(MotorCommand baseCommand) {
    // Advanced exploration based on multiple factors
    float explorationScore = calculateExplorationScore();
    
    // Multi-strategy exploration:
    // 1. Random exploration for novelty
    if (explorationScore > 0.7f) {
        return performRandomExploration();
    }
    
    // 2. Strategic exploration for goal achievement
    if (explorationScore > 0.4f) {
        return performStrategicExploration(baseCommand);
    }
    
    // 3. Conservative exploration for safety
    return applyConservativeStrategy(baseCommand);
}

MotorCommand AgentBrain::applyCuriosityBasedBehavior(MotorCommand defaultCmd) {
    // Enhanced curiosity-based behavior with multiple layers
    if (!curiosityEnabled_ || curiosityLevel_ <= 0.1f) {
        return defaultCmd;  // Low curiosity, stick to default
    }
    
    // Different curiosity behaviors based on intensity
    if (curiosityLevel_ > 0.8f) {
        // High curiosity: random creative exploration
        return executeCreativeExploration();
    } else if (curiosityLevel_ > 0.5f) {
        // Medium curiosity: balanced exploration
        return executeBalancedExploration(defaultCmd);
    } else {
        // Low-medium curiosity: targeted exploration
        return executeTargetedExploration(defaultCmd);
    }
}

MotorCommand AgentBrain::performRandomExploration() {
    // Advanced random exploration with correlation awareness
    float randomValue = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    // Bias towards certain actions based on context
    if (randomValue < 0.2f) {
        // Sometimes prefer movement for energy
        return selectMovementAction();
    } else if (randomValue < 0.4f) {
        // Sometimes prefer interaction for learning
        return MotorCommand::Interact;
    } else if (randomValue < 0.6f) {
        // Sometimes prefer looking for information
        return selectLookingAction();
    } else {
        // Default random selection
        int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
        return getRandomMotorCommand(choice);
    }
}

MotorCommand AgentBrain::executeCreativeExploration() {
    // Very high curiosity: try novel combinations
    std::vector<MotorCommand> creativeOptions;
    
    // Combine actions for creative behavior
    creativeOptions.push_back(MotorCommand::MoveForward);
    creativeOptions.push_back(MotorCommand::TurnLeft);
    creativeOptions.push_back(MotorCommand::TurnRight);
    creativeOptions.push_back(MotorCommand::Interact);
    
    // Randomly select creative combination
    int choice = brain_->getRandomGenerator()->uniformInt(0, (int)creativeOptions.size() - 1);
    return creativeOptions[choice];
}

MotorCommand AgentBrain::executeBalancedExploration(MotorCommand defaultCmd) {
    // Balance exploration with exploitation
    float explorationWeight = curiosityLevel_ * 0.6f;  // Higher curiosity = more exploration
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < explorationWeight) {
        // Explore: try new actions
        return performTargetedExploration();
    } else {
        // Exploit: use default command
        return defaultCmd;
    }
}

MotorCommand AgentBrain::executeTargetedExploration(MotorCommand defaultCmd) {
    // Targeted exploration based on recent experiences
    float noveltyBonus = calculateNoveltyBonus();
    
    if (noveltyBonus > 0.3f) {
        // High novelty: explore more
        return performTargetedExplorationWithBonus();
    } else {
        // Low novelty: conservative approach
        return defaultCmd;
    }
}

MotorCommand AgentBrain::performStrategicExploration(MotorCommand baseCommand) {
    // Strategy-based exploration for goal-oriented behavior
    if (brain_->getTotalSpikeCount() > 1000) {
        // High activity: explore new strategies
        return exploreNewStrategy();
    } else {
        // Normal activity: follow base strategy with slight variation
        return applyStrategyVariation(baseCommand);
    }
}

MotorCommand AgentBrain::applyConservativeStrategy(MotorCommand baseCommand) {
    // Conservative strategy to avoid dangerous actions
    float safetyScore = calculateSafetyScore();
    
    if (safetyScore < 0.3f) {
        // Low safety: avoid risky actions
        return MotorCommand::Wait;
    } else if (safetyScore < 0.6f) {
        // Moderate safety: only safe actions
        return selectSafeAction(baseCommand);
    } else {
        // High safety: proceed with action
        return baseCommand;
    }
}

bool AgentBrain::isValidMotorCommand(MotorCommand cmd) const {
    // Comprehensive validation of motor commands
    switch (cmd) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
        case MotorCommand::Interact:
        case MotorCommand::Wait:
            return true;
        default:
            return false;
    }
}

MotorCommand AgentBrain::getRandomMotorCommand(int choice) {
    // Map random choice to motor command
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        case 7: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

MotorCommand AgentBrain::selectMovementAction() {
    // Select movement action with potential bias
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < 0.5f) return MotorCommand::MoveForward;
    else return MotorCommand::MoveBackward;
}

MotorCommand AgentBrain::selectLookingAction() {
    // Select looking action with potential bias
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < 0.5f) return MotorCommand::LookLeft;
    else return MotorCommand::LookRight;
}

float AgentBrain::calculateExplorationScore() const {
    // Calculate overall exploration score based on multiple factors
    float noveltyScore = noveltyLevel_ * 0.4f;
    float curiosityScore = curiosityLevel_ * 0.3f;
    float predictionScore = std::abs(predictionError_) * 0.2f;
    float developmentScore = std::min(developmentalAge_ / 100.0f, 1.0f) * 0.1f;
    
    return noveltyScore + curiosityScore + predictionScore + developmentScore;
}

float AgentBrain::calculateNoveltyBonus() const {
    // Calculate bonus for exploring novel situations
    float baseNovelty = noveltyLevel_;
    
    // Amplify for recently experienced high novelty
    float recentNoveltyBonus = (noveltyLevel_ > 0.5f) ? 0.2f : 0.0f;
    
    // Decay over time
    float timeDecay = std::exp(-developmentalAge_ * 0.01f);
    
    return (baseNovelty + recentNoveltyBonus) * timeDecay;
}

float AgentBrain::calculateSafetyScore() const {
    // Calculate safety score based on energy, health, and environment
    float energySafety = energy_ / 2.0f;  // Normalized energy safety
    float healthSafety = health_ / 2.0f;  // Normalized health safety
    float noveltySafety = 1.0f - noveltyLevel_;  // Less novelty = safer
    
    return (energySafety + healthSafety + noveltySafety) / 3.0f;
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
