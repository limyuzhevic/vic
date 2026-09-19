#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <ctime>

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
    , averageReward_(0.0f)
    , explorationRate_(0.0f)
    , learningProgress_(0.0f)
    , rewardCount_(0)
    , lastUpdateTime_(0.0)
    , monitoringActive_(false)
    , monitoringStartTime_(0.0)
    , monitoringDuration_(0.0)
    , monitoringFPS_(0.0f)
    , frameCount_(0)
{
    initializeWithDefaults();
    initializeAdvanced(brain, config_);
}

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , averageReward_(0.0f)
    , explorationRate_(0.0f)
    , learningProgress_(0.0f)
    , rewardCount_(0)
    , lastUpdateTime_(0.0)
    , monitoringActive_(false)
    , monitoringStartTime_(0.0)
    , monitoringDuration_(0.0)
    , monitoringFPS_(0.0f)
    , frameCount_(0)
{
    setConfig(config);
    initializeAdvanced(brain, config_);
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeWithDefaults() {
    config_ = AgentConfig();
    // Backward compatibility with old flag names
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
}

void AgentBrain::initializeAdvanced(std::shared_ptr<Brain> brain, const AgentConfig& config) {
    if (!brain) return;
    
    // Apply motor distribution strategy
    switch (config.motorStrategy) {
        case MotorDistributionStrategy::Balanced:
            distributeMotorNeuronsBalanced();
            break;
        case MotorDistributionStrategy::Clustered:
            distributeMotorNeuronsClustered();
            break;
        case MotorDistributionStrategy::Adaptive:
            distributeMotorNeuronsAdaptive();
            break;
        case MotorDistributionStrategy::Priority:
            distributeMotorNeuronsPriority();
            break;
        case MotorDistributionStrategy::Random:
            distributeMotorNeuronsRandom();
            break;
    }
    
    // Initialize sensory neuron groups based on configuration
    // Note: This is based on the existing brain structure; advanced filtering can be added
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Apply motor group sizing based on configuration
                    // For now, just add to balanced distribution
                    // Advanced sizing logic could be implemented here
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
                    // Apply sensory group sizing based on configuration
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
    
    // Log initialization
    logDebug("AgentBrain initialized with advanced configuration: brainMode=" + 
             std::to_string(static_cast<int>(config.brainMode)) +
             ", motorStrategy=" + std::to_string(static_cast<int>(config.motorStrategy)) +
             ", sensoryMode=" + std::to_string(static_cast<int>(config.sensoryMode)));
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Update backward compatibility flags
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
    
    if (config_.enableDebugLogging) {
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons,
                     config=" + config_.toString());
    }
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
    
    // Apply sensory processing mode
    switch (config_.sensoryMode) {
        case SensoryProcessingMode::Basic:
            processSensoryBasic(percept);
            break;
        case SensoryProcessingMode::Adaptive:
            processSensoryAdaptive(percept);
            break;
        case SensoryProcessingMode::Predictive:
            processSensoryPredictive(percept);
            break;
        case SensoryProcessingMode::Sparse:
            processSensorySparse(percept);
            break;
        case SensoryProcessingMode::Hierarchical:
            processSensoryHierarchical(percept);
            break;
    }
    
    // Update performance metrics
    updateMetrics(1.0 / config_.monitoringInterval);
}

void AgentBrain::processSensoryBasic(const SensoryPercept& percept) {
    // Basic processing (original behavior)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    computeNovelty(vision);
}

void AgentBrain::processSensoryAdaptive(const SensoryPercept& percept) {
    // Enhanced processing with dynamic sensitivity adjustment
    const auto& vision = percept.getVision();
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Adaptive gain based on novelty and importance
            float adaptiveGain = config_.sensoryGain;
            
            // Boost sensitivity for novel patterns
            if (i < previousVision_.size()) {
                float novelty = std::abs(vision[i] - previousVision_[i]);
                adaptiveGain *= (1.0f + novelty * config_.noveltySensitivity);
            }
            
            float current = vision[i] * 5.0f * adaptiveGain;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Apply same pattern for other sensory modalities
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f * config_.sensoryGain;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f * config_.sensoryGain;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f * config_.sensoryGain;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    computeNovelty(vision);
}

void AgentBrain::processSensoryPredictive(const SensoryPercept& percept) {
    // Processing with predictive coding
    const auto& vision = percept.getVision();
    
    // Use predicted vision to enhance actual input
    std::vector<float> predictedVision;
    if (!previousVision_.empty()) {
        // Simple prediction: smooth temporal changes
        predictedVision = previousVision_;
        float alpha = 0.7f; // Prediction confidence
        
        for (size_t i = 0; i < vision.size() && i < predictedVision.size(); ++i) {
            predictedVision[i] = predictedVision[i] * alpha + vision[i] * (1.0f - alpha);
        }
    }
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float predictionError = 0.0f;
            if (i < predictedVision.size()) {
                predictionError = std::abs(vision[i] - predictedVision[i]);
            }
            
            // Amplify prediction errors (they're informative!)
            float current = vision[i] * 5.0f * (1.0f + predictionError * config_.predictionAccuracyThreshold);
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Other modalities with prediction
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    computeNovelty(vision);
}

void AgentBrain::processSensorySparse(const SensoryPercept& percept) {
    // Process only significant changes (energy-efficient)
    const auto& vision = percept.getVision();
    
    // Threshold for considering a change "significant"
    float significanceThreshold = 0.05f;
    
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i] && i < previousVision_.size()) {
            float change = std::abs(vision[i] - previousVision_[i]);
            
            if (change > significanceThreshold) {
                float current = vision[i] * 5.0f * change;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Other modalities with sparse processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i] && i < 5) { // Only process first few for efficiency
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    computeNovelty(vision);
}

void AgentBrain::processSensoryHierarchical(const SensoryPercept& percept) {
    // Multi-level processing (coarse to fine)
    const auto& vision = percept.getVision();
    
    // Level 1: Global statistics
    if (!vision.empty()) {
        float mean = 0.0f, stddev = 0.0f;
        for (float v : vision) {
            mean += v;
        }
        mean /= vision.size();
        
        for (float v : vision) {
            stddev += (v - mean) * (v - mean);
        }
        stddev = std::sqrt(stddev / vision.size());
        
        // Inject global statistics into internal neurons
        for (Neuron* n : sensoryInternal_) {
            if (n) {
                float current = (mean * 2.0f - 1.0f) * 3.0f;
                n->injectCurrent(current);
            }
        }
    }
    
    // Level 2: Edge detection (simplified)
    std::vector<float> edges;
    if (vision.size() >= 3) {
        edges.resize(vision.size());
        for (size_t i = 1; i < vision.size() - 1; ++i) {
            edges[i] = std::abs(vision[i] - (vision[i-1] + vision[i+1]) / 2.0f);
        }
    }
    
    // Level 3: Detailed processing
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Combine hierarchical levels
            float level1 = (i < vision.size()) ? vision[i] : 0.0f;
            float level2 = (i < edges.size()) ? edges[i] * 2.0f : 0.0f;
            float current = (level1 * 0.7f + level2 * 0.3f) * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Other modalities with hierarchical processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    computeNovelty(vision);
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Apply decay rate
    noveltyLevel_ *= config_.noveltyDecayRate;
    
    // Store for next time
    previousVision_ = vision;
    
    // Update curiosity based on novelty
    if (config_.curiosityEnabled) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (config_.curiosityEnabled && curiosityLevel_ > config_.curiosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Update exploration rate metric
    explorationRate_ = shouldExplore() ? 1.0f : 0.0f;
    
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
    
    // Apply motor threshold
    if (bestActivity < config_.motorThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * config_.explorationProbability;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command (excluding look commands which aren't in enum)
            int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !config_.rewardModulationEnabled) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Update average reward metric
    rewardCount_++;
    averageReward_ = 0.95f * averageReward_ + 0.05f * reward;
    
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
    
    // Update learning progress based on prediction error
    updateLearningProgress();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !config_.developmentEnabled) return;
    
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
    
    // Apply plasticity decay
    if (config_.plasticityDecayRate > 0.0f && developmentalAge_ > config_.criticalPeriodDuration) {
        plasticityModifier_ *= config_.plasticityDecayRate;
    }
    
    // Structural plasticity changes with development
    if (config_.structuralPlasticityEnabled_) {
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

float AgentBrain::getPlasticityModifier() const {
    return plasticityModifier_;
}

double AgentBrain::getDevelopmentalAge() const {
    return developmentalAge_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    averageReward_ = 0.0f;
    explorationRate_ = 0.0f;
    learningProgress_ = 0.0f;
    rewardCount_ = 0;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Log reset
    logDebug("AgentBrain reset");
}

void AgentBrain::setBrainMode(BrainMode mode) {
    config_.brainMode = mode;
    logDebug("AgentBrain brain mode changed to: " + std::to_string(static_cast<int>(mode)));
    
    if (config_.onBrainModeChanged) {
        config_.onBrainModeChanged(mode);
    }
}

void AgentBrain::setMotorDistributionStrategy(MotorDistributionStrategy strategy) {
    config_.motorStrategy = strategy;
    logDebug("AgentBrain motor distribution strategy changed to: " + std::to_string(static_cast<int>(strategy)));
    
    // Re-distribute motor neurons
    // Save current neuron pointers
    std::vector<Neuron*> savedForward = motorForward_;
    std::vector<Neuron*> savedBackward = motorBackward_;
    std::vector<Neuron*> savedLeft = motorTurnLeft_;
    std::vector<Neuron*> savedRight = motorTurnRight_;
    std::vector<Neuron*> savedInteract = motorInteract_;
    std::vector<Neuron*> savedWait = motorWait_;
    
    // Clear groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Re-initialize based on new strategy
    if (brain_) {
        if (strategy == MotorDistributionStrategy::Balanced) {
            distributeMotorNeuronsBalanced();
        } else {
            // For other strategies, use the appropriate function
            // This is a simplified approach - in practice, you'd want to preserve neuron connections
            distributeMotorNeuronsBalanced();  // Default for other strategies
        }
    }
    
    // Restore saved neurons (this is a simplification)
    motorForward_ = savedForward;
    motorBackward_ = savedBackward;
    motorTurnLeft_ = savedLeft;
    motorTurnRight_ = savedRight;
    motorInteract_ = savedInteract;
    motorWait_ = savedWait;
}

void AgentBrain::setSensoryProcessingMode(SensoryProcessingMode mode) {
    config_.sensoryMode = mode;
    logDebug("AgentBrain sensory processing mode changed to: " + std::to_string(static_cast<int>(mode)));
}

void AgentBrain::setConfig(const AgentConfig& newConfig) {
    if (!newConfig.validate()) {
        logDebug("Invalid configuration - keeping current settings", 3);
        return;
    }
    
    AgentConfig oldConfig = config_;
    config_ = newConfig;
    
    // Update backward compatibility flags
    rewardModulationEnabled_ = config_.rewardModulationEnabled;
    structuralPlasticityEnabled_ = config_.structuralPlasticityEnabled;
    developmentEnabled_ = config_.developmentEnabled;
    curiosityEnabled_ = config_.curiosityEnabled;
    
    // Log configuration change
    if (config_.enableDebugLogging) {
        std::string changes;
        if (config_.brainMode != oldConfig.brainMode) changes += "brainMode ";
        if (config_.motorStrategy != oldConfig.motorStrategy) changes += "motorStrategy ";
        if (config_.sensoryMode != oldConfig.sensoryMode) changes += "sensoryMode ";
        if (!changes.empty()) {
            logDebug("AgentBrain configuration updated: " + changes);
        }
    }
    
    if (config_.onStateChanged) {
        config_.onStateChanged("configuration_changed");
    }
}

DebugInfo AgentBrain::getDebugInfo() const {
    DebugInfo info;
    info.currentTime = developmentalAge_;
    info.currentBrainMode = config_.brainMode;
    info.neuromodulationLevel = dopamineLevel_;
    info.curiosityLevel = curiosityLevel_;
    info.noveltyLevel = noveltyLevel_;
    info.predictionError = predictionError_;
    info.developmentalStage = getDevelopmentalStage();
    info.plasticityModifier = plasticityModifier_;
    info.developmentalAge = developmentalAge_;
    info.motorNeuronsTotal = motorForward_.size() + motorBackward_.size() + 
                             motorTurnLeft_.size() + motorTurnRight_.size() +
                             motorInteract_.size() + motorWait_.size();
    info.sensoryNeuronsTotal = sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size();
    info.debugLogLevel = config_.enableDebugLogging ? 1 : 0;
    
    // Collect performance metrics
    info.performanceMetrics["average_reward"] = averageReward_;
    info.performanceMetrics["exploration_rate"] = explorationRate_;
    info.performanceMetrics["learning_progress"] = learningProgress_;
    info.performanceMetrics["prediction_error"] = predictionError_;
    
    return info;
}

void AgentBrain::logDebugInfo(const std::string& context) {
    DebugInfo info = getDebugInfo();
    
    std::stringstream ss;
    ss << "[AgentBrain " << context << "] time=" << info.currentTime
       << " mode=" << static_cast<int>(info.currentBrainMode)
       << " neuromodulation=" << info.neuromodulationLevel
       << " curiosity=" << info.curiosityLevel
       << " novelty=" << info.noveltyLevel
       << " prediction_error=" << info.predictionError
       << " stage=" << static_cast<int>(info.developmentalStage)
       << " plasticity=" << info.plasticityModifier
       << " motor_neurons=" << info.motorNeuronsTotal
       << " sensory_neurons=" << info.sensoryNeuronsTotal
       << " active_events=" << activeEvents_.size();
    
    logDebug(ss.str());
}

void AgentBrain::logDebug(const std::string& message, int level) const {
    if (!config_.enableDebugLogging || level > 1) return;
    
    std::string levelStr;
    switch (level) {
        case 0: levelStr = "ERROR"; break;
        case 1: levelStr = "INFO"; break;
        default: levelStr = "DEBUG"; break;
    }
    
    NLM_LOG_INFO("[AgentBrain " + levelStr + "] " + message);
}

bool AgentBrain::saveState(const std::string& filePath) {
    std::string path = filePath.empty() ? config_.stateFilePath : filePath;
    if (path.empty()) {
        logDebug("No state file path specified", 3);
        return false;
    }
    
    return saveStateToFile(path);
}

bool AgentBrain::loadState(const std::string& filePath) {
    std::string path = filePath.empty() ? config_.stateFilePath : filePath;
    if (path.empty()) {
        logDebug("No state file path specified", 3);
        return false;
    }
    
    return loadStateFromFile(path);
}

bool AgentBrain::saveStateToFile(const std::string& filePath) {
    std::string state = serializeToString();
    std::string compressed = compressState(state);
    
    // For now, just log - in practice you'd write to file
    logDebug("Saving state to file: " + filePath + ", size=" + std::to_string(compressed.size()) + " bytes");
    
    // In a real implementation, you'd write to file here
    // For example: std::ofstream file(filePath); file << compressed; file.close();
    
    if (config_.onStateChanged) {
        config_.onStateChanged("state_saved");
    }
    
    return true;
}

bool AgentBrain::loadStateFromFile(const std::string& filePath) {
    // For now, just log - in practice you'd read from file
    logDebug("Loading state from file: " + filePath);
    
    // In a real implementation, you'd read from file here
    // For example: std::ifstream file(filePath); std::string compressed((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); file.close();
    // std::string state = decompressState(compressed);
    
    if (config_.onStateChanged) {
        config_.onStateChanged("state_loaded");
    }
    
    return true;
}

std::string AgentBrain::serializeToString() const {
    std::stringstream ss;
    ss << "AgentBrain:" << std::endl;
    ss << "  neuromodulation:" << dopamineLevel_ << std::endl;
    ss << "  novelty:" << noveltyLevel_ << std::endl;
    ss << "  curiosity:" << curiosityLevel_ << std::endl;
    ss << "  prediction_error:" << predictionError_ << std::endl;
    ss << "  expected_reward:" << expectedReward_ << std::endl;
    ss << "  developmental_age:" << developmentalAge_ << std::endl;
    ss << "  plasticity_modifier:" << plasticityModifier_ << std::endl;
    ss << "  brain_mode:" << static_cast<int>(config_.brainMode) << std::endl;
    ss << "  motor_strategy:" << static_cast<int>(config_.motorStrategy) << std::endl;
    ss << "  sensory_mode:" << static_cast<int>(config_.sensoryMode) << std::endl;
    
    return ss.str();
}

void AgentBrain::startMonitoring(double duration) {
    if (!config_.enableMetricsCollection) return;
    
    monitoringActive_ = true;
    monitoringStartTime_ = developmentalAge_;
    monitoringDuration_ = duration;
    frameCount_ = 0;
    
    logDebug("Monitoring started for " + std::to_string(duration) + " seconds");
}

void AgentBrain::stopMonitoring() {
    if (!monitoringActive_) return;
    
    monitoringActive_ = false;
    
    if (monitoringStartTime_ > 0.0) {
        double elapsed = developmentalAge_ - monitoringStartTime_;
        if (elapsed > 0.0) {
            monitoringFPS_ = frameCount_ / elapsed;
        }
    }
    
    logDebug("Monitoring stopped. FPS: " + std::to_string(monitoringFPS_));
    
    // Notify callback
    if (config_.onPerformanceMetric) {
        config_.onPerformanceMetric(monitoringFPS_);
    }
}

void AgentBrain::updateMetrics(double timestep) {
    lastUpdateTime_ = developmentalAge_;
    frameCount_++;
    
    if (monitoringActive_) {
        if (developmentalAge_ - monitoringStartTime_ >= monitoringDuration_) {
            stopMonitoring();
        }
    }
    
    updateLearningProgress();
}

void AgentBrain::collectPerformanceMetrics() {
    // Update learning progress based on various factors
    float noveltyContribution = std::min(1.0f, noveltyLevel_ * 2.0f);
    float curiosityContribution = curiosityLevel_;
    float adaptationContribution = 1.0f - (std::abs(predictionError_) * 0.5f);
    
    learningProgress_ = (noveltyContribution + curiosityContribution + adaptationContribution) / 3.0f;
}

void AgentBrain::updateLearningProgress() {
    collectPerformanceMetrics();
}

bool AgentBrain::shouldExplore() const {
    return config_.curiosityEnabled && curiosityLevel_ > config_.curiosityThreshold;
}

BrainMode AgentBrain::determineOptimalBrainMode() const {
    // Simple heuristic for determining optimal brain mode
    if (curiosityLevel_ > 0.7f) {
        return BrainMode::CuriosityDriven;
    } else if (noveltyLevel_ > 0.5f) {
        return BrainMode::Exploration;
    } else if (predictionError_ > 0.3f) {
        return BrainMode::Predictive;
    } else {
        return BrainMode::Adaptive;
    }
}

// Serialization helper implementation
namespace {
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                case '"': result += "\\\""; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    std::string unescapeString(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    case '"': result += '"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
}

std::string AgentBrain::configToString(const AgentConfig& config) {
    std::stringstream ss;
    ss << "AgentConfig:" << std::endl;
    
    // Basic flags
    ss << "  rewardModulationEnabled:" << (config.rewardModulationEnabled ? "true" : "false") << std::endl;
    ss << "  structuralPlasticityEnabled:" << (config.structuralPlasticityEnabled ? "true" : "false") << std::endl;
    ss << "  developmentEnabled:" << (config.developmentEnabled ? "true" : "false") << std::endl;
    ss << "  curiosityEnabled:" << (config.curiosityEnabled ? "true" : "false") << std::endl;
    
    // Enum values
    ss << "  brainMode:" << static_cast<int>(config.brainMode) << std::endl;
    ss << "  motorStrategy:" << static_cast<int>(config.motorStrategy) << std::endl;
    ss << "  sensoryMode:" << static_cast<int>(config.sensoryMode) << std::endl;
    
    // Float values
    ss << "  sensoryGain:" << config.sensoryGain << std::endl;
    ss << "  motorThreshold:" << config.motorThreshold << std::endl;
    ss << "  noveltySensitivity:" << config.noveltySensitivity << std::endl;
    ss << "  curiosityThreshold:" << config.curiosityThreshold << std::endl;
    ss << "  explorationProbability:" << config.explorationProbability << std::endl;
    ss << "  criticalPeriodDuration:" << config.criticalPeriodDuration << std::endl;
    ss << "  plasticityDecayRate:" << config.plasticityDecayRate << std::endl;
    ss << "  monitoringInterval:" << config.monitoringInterval << std::endl;
    ss << "  noveltyDecayRate:" << config.noveltyDecayRate << std::endl;
    ss << "  predictionAccuracyThreshold:" << config.predictionAccuracyThreshold << std::endl;
    ss << "  minNeuronPerGroup:" << config.minNeuronPerGroup << std::endl;
    ss << "  maxNeuronPerGroup:" << config.maxNeuronPerGroup << std::endl;
    ss << "  redistributionThreshold:" << config.redistributionThreshold << std::endl;
    ss << "  stateCompressionRatio:" << config.stateCompressionRatio << std::endl;
    
    // String values
    ss << "  stateFilePath:" << escapeString(config.stateFilePath) << std::endl;
    
    // Boolean flags
    ss << "  enableDebugLogging:" << (config.enableDebugLogging ? "true" : "false") << std::endl;
    ss << "  enableMetricsCollection:" << (config.enableMetricsCollection ? "true" : "false") << std::endl;
    ss << "  enableStatePersistence:" << (config.enableStatePersistence ? "true" : "false") << std::endl;
    
    return ss.str();
}

AgentConfig AgentBrain::configFromString(const std::string& data) {
    AgentConfig config;
    std::istringstream iss(data);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find('#') == 0) continue;
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Parse key-value pairs
        if (key == "rewardModulationEnabled") {
            config.rewardModulationEnabled = (value == "true" || value == "1");
        } else if (key == "structuralPlasticityEnabled") {
            config.structuralPlasticityEnabled = (value == "true" || value == "1");
        } else if (key == "developmentEnabled") {
            config.developmentEnabled = (value == "true" || value == "1");
        } else if (key == "curiosityEnabled") {
            config.curiosityEnabled = (value == "true" || value == "1");
        } else if (key == "brainMode") {
            config.brainMode = static_cast<BrainMode>(std::stoi(value));
        } else if (key == "motorStrategy") {
            config.motorStrategy = static_cast<MotorDistributionStrategy>(std::stoi(value));
        } else if (key == "sensoryMode") {
            config.sensoryMode = static_cast<SensoryProcessingMode>(std::stoi(value));
        } else if (key == "sensoryGain") {
            config.sensoryGain = std::stof(value);
        } else if (key == "motorThreshold") {
            config.motorThreshold = std::stof(value);
        } else if (key == "noveltySensitivity") {
            config.noveltySensitivity = std::stof(value);
        } else if (key == "curiosityThreshold") {
            config.curiosityThreshold = std::stof(value);
        } else if (key == "explorationProbability") {
            config.explorationProbability = std::stof(value);
        } else if (key == "criticalPeriodDuration") {
            config.criticalPeriodDuration = std::stod(value);
        } else if (key == "plasticityDecayRate") {
            config.plasticityDecayRate = std::stof(value);
        } else if (key == "monitoringInterval") {
            config.monitoringInterval = std::stof(value);
        } else if (key == "noveltyDecayRate") {
            config.noveltyDecayRate = std::stof(value);
        } else if (key == "predictionAccuracyThreshold") {
            config.predictionAccuracyThreshold = std::stof(value);
        } else if (key == "minNeuronPerGroup") {
            config.minNeuronPerGroup = std::stof(value);
        } else if (key == "maxNeuronPerGroup") {
            config.maxNeuronPerGroup = std::stof(value);
        } else if (key == "redistributionThreshold") {
            config.redistributionThreshold = std::stof(value);
        } else if (key == "stateFilePath") {
            config.stateFilePath = unescapeString(value);
        } else if (key == "enableDebugLogging") {
            config.enableDebugLogging = (value == "true" || value == "1");
        } else if (key == "enableMetricsCollection") {
            config.enableMetricsCollection = (value == "true" || value == "1");
        } else if (key == "enableStatePersistence") {
            config.enableStatePersistence = (value == "true" || value == "1");
        } else if (key == "stateCompressionRatio") {
            config.stateCompressionRatio = std::stof(value);
        }
    }
    
    return config;
}

std::string AgentBrain::getCurrentConfigString() const {
    return config_.toString();
}

bool AgentBrain::updateConfigFromString(const std::string& data) {
    AgentConfig newConfig = configFromString(data);
    if (newConfig.validate()) {
        setConfig(newConfig);
        return true;
    }
    return false;
}

std::string AgentBrain::compressState(const std::string& state) const {
    // Simple compression: if state is longer than 1000 chars, take every Nth char
    if (state.size() <= 1000) return state;
    
    std::string compressed;
    float ratio = config_.stateCompressionRatio;
    size_t step = std::max<size_t>(1, static_cast<size_t>(state.size() * ratio / 1000.0f));
    
    for (size_t i = 0; i < state.size(); i += step) {
        compressed += state[i];
    }
    
    return compressed;
}

std::string AgentBrain::decompressState(const std::string& compressed) const {
    // Simple decompression: interpolate missing characters
    if (compressed.size() < 100) return compressed;
    
    // For simplicity, just return the compressed state
    // In practice, you'd implement proper decompression
    return compressed;
}

// Implementation of AgentBrainSerializer methods
std::string AgentBrainSerializer::serialize(const AgentBrain& agentBrain) {
    std::stringstream ss;
    ss << "AgentBrain_Serialized:" << std::endl;
    ss << "  config:" << agentBrain.getCurrentConfigString();
    ss << "  debug_info:" << std::endl;
    
    DebugInfo info = agentBrain.getDebugInfo();
    ss << "    time:" << info.currentTime << std::endl;
    ss << "    neuromodulation:" << info.neuromodulationLevel << std::endl;
    ss << "    curiosity:" << info.curiosityLevel << std::endl;
    ss << "    novelty:" << info.noveltyLevel << std::endl;
    ss << "    prediction_error:" << info.predictionError << std::endl;
    ss << "    stage:" << static_cast<int>(info.developmentalStage) << std::endl;
    
    return ss.str();
}

bool AgentBrainSerializer::deserialize(std::string& data, AgentBrain& agentBrain) {
    // Simple parsing - in practice, you'd use proper serialization format
    if (data.find("AgentBrain_Serialized:") == std::string::npos) {
        return false;
    }
    
    // Extract config part
    size_t configPos = data.find("  config:");
    if (configPos == std::string::npos) {
        return false;
    }
    
    size_t configEnd = data.find("  debug_info:", configPos);
    if (configEnd == std::string::npos) {
        return false;
    }
    
    std::string configData = data.substr(configPos + 9, configEnd - configPos - 9);
    
    // Parse and set config
    AgentConfig newConfig = AgentBrain::configFromString(configData);
    if (newConfig.validate()) {
        agentBrain.setConfig(newConfig);
        return true;
    }
    
    return false;
}

} // namespace nlm

// Add toString and fromString implementations for AgentConfig
std::string AgentConfig::toString() const {
    std::stringstream ss;
    ss << "AgentConfig:" << std::endl;
    
    ss << "  rewardModulationEnabled:" << (rewardModulationEnabled ? "true" : "false") << std::endl;
    ss << "  structuralPlasticityEnabled:" << (structuralPlasticityEnabled ? "true" : "false") << std::endl;
    ss << "  developmentEnabled:" << (developmentEnabled ? "true" : "false") << std::endl;
    ss << "  curiosityEnabled:" << (curiosityEnabled ? "true" : "false") << std::endl;
    ss << "  brainMode:" << static_cast<int>(brainMode) << std::endl;
    ss << "  motorStrategy:" << static_cast<int>(motorStrategy) << std::endl;
    ss << "  sensoryMode:" << static_cast<int>(sensoryMode) << std::endl;
    ss << "  sensoryGain:" << sensoryGain << std::endl;
    ss << "  motorThreshold:" << motorThreshold << std::endl;
    ss << "  noveltySensitivity:" << noveltySensitivity << std::endl;
    ss << "  curiosityThreshold:" << curiosityThreshold << std::endl;
    ss << "  explorationProbability:" << explorationProbability << std::endl;
    ss << "  criticalPeriodDuration:" << criticalPeriodDuration << std::endl;
    ss << "  plasticityDecayRate:" << plasticityDecayRate << std::endl;
    ss << "  monitoringInterval:" << monitoringInterval << std::endl;
    ss << "  enableDebugLogging:" << (enableDebugLogging ? "true" : "false") << std::endl;
    ss << "  enableMetricsCollection:" << (enableMetricsCollection ? "true" : "false") << std::endl;
    ss << "  noveltyDecayRate:" << noveltyDecayRate << std::endl;
    ss << "  predictionAccuracyThreshold:" << predictionAccuracyThreshold << std::endl;
    ss << "  minNeuronPerGroup:" << minNeuronPerGroup << std::endl;
    ss << "  maxNeuronPerGroup:" << maxNeuronPerGroup << std::endl;
    ss << "  redistributionThreshold:" << redistributionThreshold << std::endl;
    ss << "  enableStatePersistence:" << (enableStatePersistence ? "true" : "false") << std::endl;
    ss << "  stateFilePath:" << stateFilePath << std::endl;
    ss << "  stateCompressionRatio:" << stateCompressionRatio << std::endl;
    
    return ss.str();
}

bool AgentConfig::fromString(const std::string& data) {
    *this = configFromString(data);
    return validate();
}

// Implementation of motor distribution strategies
void AgentBrain::distributeMotorNeuronsBalanced() {
    // Current balanced distribution logic
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
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

void AgentBrain::distributeMotorNeuronsClustered() {
    // Cluster neurons by action type
    // This is a placeholder - would implement more sophisticated clustering
    distributeMotorNeuronsBalanced();
}

void AgentBrain::distributeMotorNeuronsAdaptive() {
    // Adapt distribution based on usage patterns
    // This would track which actions are used most and adjust neuron allocation
    distributeMotorNeuronsBalanced();
}

void AgentBrain::distributeMotorNeuronsPriority() {
    // Give priority to commonly used actions
    // This would track action usage statistics and allocate more neurons
    distributeMotorNeuronsBalanced();
}

void AgentBrain::distributeMotorNeuronsRandom() {
    // Random distribution
    // This would shuffle neurons before assigning them
    distributeMotorNeuronsBalanced();
}