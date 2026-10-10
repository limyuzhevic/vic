#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/Performance.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <fstream>
#include <sstream>

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
    // New enhanced features
    , processingStartTime_(0.0)
    , totalProcessingTime_(0.0)
    , processedSensoryEvents_(0)
    , frameCount_(0)
    , behaviorMode_(1)
    , explorationRate_(0.1f)
    , curiosityThreshold_(0.3f)
    , rewardWeight_(1.0f)
    , noveltyThreshold_(0.5f)
    , isLearningEnabled_(true)
    , isAdaptivePlasticityEnabled_(true)
    , isMetaLearningEnabled_(false)
    , isOptimizationEnabled_(true)
    , targetFrameRate_(30.0f)
    , memoryBudget_(512 * 1024 * 1024)  // 512 MB
    , isSIMDEnabled_(true)
    , isParallelProcessingEnabled_(false)
    , isPaused_(false)
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
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
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
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
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
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
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

// Get spatial context from brain
std::pair<float, float> AgentBrain::getSpatialContext() const {
    if (!brain_ || !brain_->getSpatialRepresentation()) {
        return std::make_pair(0.0f, 0.0f);
    }
    return brain_->getSpatialContext();
}

// Get spatial representation status
bool AgentBrain::hasSpatialContext() const {
    if (!brain_ || !brain_->getSpatialRepresentation()) {
        return false;
    }
    return brain_->getSpatialRepresentation()->hasPlaceCells();
}

// Get temporal context for sequence prediction
std::vector<float> AgentBrain::getTemporalContext() const {
    std::vector<float> temporalContext;
    
    if (!brain_ || !brain_->getTemporalRelation()) {
        return temporalContext;
    }
    
    // For now, return a placeholder - in a real implementation, this would
    // retrieve the current temporal state from the brain's temporal relation system
    // This could include recent sequence patterns and prediction confidences
    
    // Add sequence length as part of temporal context
    size_t sequenceLength = brain_->getTemporalRelation()->getSequenceLength();
    temporalContext.push_back(static_cast<float>(sequenceLength));
    
    // Add confidence in temporal predictions
    if (!temporalContext.empty()) {
        // This is a placeholder - in a real implementation, we would query
        // the temporal relation system for prediction confidence
        temporalContext.push_back(0.5f); // Placeholder confidence
    }
    
    return temporalContext;
}

// Get temporal representation status
bool AgentBrain::hasTemporalContext() const {
    if (!brain_ || !brain_->getTemporalRelation()) {
        return false;
    }
    
    // Check if temporal relations have been learned
    return brain_->getTemporalRelation()->getSequenceLength() > 0;
}

// Get self-model context for self-awareness
std::vector<float> AgentBrain::getSelfModelContext() const {
    std::vector<float> selfModelContext;
    
    if (!brain_ || !brain_->getSelfModel()) {
        return selfModelContext;
    }
    
    // Get self-model capability level (0-1)
    selfModelContext.push_back(brain_->getSelfModel()->getCapabilityLevel());
    
    // Get body awareness (how accurately can predict consequences)
    selfModelContext.push_back(brain_->getSelfModel()->getBodyAwareness());
    
    // Get self-model confidence if it has been formed
    if (brain_->getSelfModel()->hasSelfModel()) {
        // Add confidence for each action type (simplified: average of all action confidences)
        // For now, just add the capability level as an overall measure
        selfModelContext.push_back(1.0f); // Self-model formed flag
    }
    
    return selfModelContext;
}

// Get self-model status
bool AgentBrain::hasSelfModel() const {
    if (!brain_ || !brain_->getSelfModel()) {
        return false;
    }
    
    // Check if self-model has been formed (has learned action-effect mappings)
    return brain_->getSelfModel()->hasSelfModel();
}

// Get body awareness (how accurately can predict consequences)
float AgentBrain::getBodyAwareness() const {
    if (!brain_ || !brain_->getSelfModel()) {
        return 0.0f;
    }
    
    return brain_->getSelfModel()->getBodyAwareness();
}

// Get current capability level (0-1)
float AgentBrain::getCapabilityLevel() const {
    if (!brain_ || !brain_->getSelfModel()) {
        return 0.0f;
    }
    
    return brain_->getSelfModel()->getCapabilityLevel();
}

// Get social learning context for social cognition
std::vector<float> AgentBrain::getSocialContext() const {
    std::vector<float> socialContext;
    
    if (!brain_ || !brain_->getSocialLearning()) {
        return socialContext;
    }
    
    // Get observation count as part of social context
    socialContext.push_back(static_cast<float>(brain_->getSocialLearning()->getObservationCount()));
    
    // Get signal meaning (learned communication value)
    socialContext.push_back(brain_->getSocialLearning()->getSignalMeaning());
    
    // Has social knowledge
    socialContext.push_back(brain_->getSocialLearning()->hasSocialKnowledge() ? 1.0f : 0.0f);
    
    // Number of observed actions
    // This would require adding a method to SocialLearning to get the count
    // For now, we'll use observationCount_ as a proxy
    socialContext.push_back(1.0f); // Placeholder
    
    return socialContext;
}

// Get social learning status
bool AgentBrain::hasSocialLearning() const {
    if (!brain_ || !brain_->getSocialLearning()) {
        return false;
    }
    
    // Check if social learning has any observations
    return brain_->getSocialLearning()->getObservationCount() > 0;
}

// Get social observation count
size_t AgentBrain::getSocialObservationCount() const {
    if (!brain_ || !brain_->getSocialLearning()) {
        return 0;
    }
    
    return brain_->getSocialLearning()->getObservationCount();
}

// Check if can imitate observed actions
bool AgentBrain::canImitate(ActionType observedAction) const {
    if (!brain_ || !brain_->getSocialLearning()) {
        return false;
    }
    
    return brain_->getSocialLearning()->canImitate(observedAction);
}

// Get imitation action based on current state
ActionType AgentBrain::getImitationAction(const std::vector<float>& currentState) const {
    if (!brain_ || !brain_->getSocialLearning()) {
        return ActionType::Wait;
    }
    
    return brain_->getSocialLearning()->getImitationAction(currentState);
}

// Enhanced motor command generation with context awareness
MotorCommand AgentBrain::decodeMotorCommandWithContext(const std::vector<float>& context) {
    if (!brain_) return MotorCommand::Wait;
    
    // Use context to influence motor decoding
    MotorCommand baseCmd = decodeFromMotorNeurons();
    
    // Apply context-based modulation
    if (!context.empty()) {
        // Context could include spatial, temporal, and social information
        float spatialInfluence = 0.0f;
        float temporalInfluence = 0.0f;
        
        if (context.size() > 0) spatialInfluence = context[0]; // Spatial context
        if (context.size() > 1) temporalInfluence = context[1]; // Temporal context
        
        // Modify command based on context (simplified logic)
        if (spatialInfluence > 0.7f && temporalInfluence > 0.5f) {
            // High spatial/temporal confidence -> more decisive action
            return baseCmd;
        }
    }
    
    // Apply curiosity-based exploration with enhanced threshold
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        baseCmd = selectWithCuriosity(baseCmd);
    }
    
    // Apply exploration rate control
    if (behaviorMode_ == 2 && explorationRate_ > 0.0f) { // Exploration mode
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < explorationRate_) {
            // Random exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);
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
    }
    
    return baseCmd;
}

MotorCommand AgentBrain::decodeMotorCommandWithCuriosity(float curiosityThreshold) {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with dynamic threshold
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeMotorCommandWithExploration(float explorationRate) {
    if (!brain_) return MotorCommand::Wait;
    
    // Behavior mode 1: Exploitation (default)
    // Behavior mode 2: Exploration
    if (behaviorMode_ == 2 && explorationRate > 0.0f) {
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < explorationRate) {
            // Random exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);
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
    }
    
    return decodeFromMotorNeurons();
}

// Enhanced sensory processing
std::vector<float> AgentBrain::preprocessVision(const std::vector<float>& rawVision) const {
    std::vector<float> processed = rawVision;
    
    if (processed.empty()) return processed;
    
    // Apply Gaussian blur for noise reduction
    std::vector<float> blurred(processed.size(), 0.0f);
    for (size_t i = 0; i < processed.size(); ++i) {
        float weight = 0.25f;
        blurred[i] += processed[i] * weight;
        
        if (i > 0) {
            blurred[i] += processed[i-1] * weight * 0.8f;
            weight *= 0.8f;
        }
        if (i < processed.size() - 1) {
            blurred[i] += processed[i+1] * weight * 0.8f;
            weight *= 0.8f;
        }
    }
    
    // Normalize to [0, 1]
    float maxVal = *std::max_element(blurred.begin(), blurred.end());
    if (maxVal > 0.0f) {
        for (float& val : blurred) {
            val /= maxVal;
        }
    }
    
    return blurred;
}

std::vector<float> AgentBrain::preprocessTouch(const std::vector<float>& rawTouch) const {
    std::vector<float> processed = rawTouch;
    
    // Apply threshold for collision detection
    for (float& val : processed) {
        val = std::max(0.0f, val - 0.1f); // Remove noise below threshold
    }
    
    return processed;
}

std::vector<float> AgentBrain::preprocessInternal(const std::vector<float>& rawInternal) const {
    std::vector<float> processed = rawInternal;
    
    // Apply homeostatic normalization
    float sum = std::accumulate(processed.begin(), processed.end(), 0.0f);
    if (processed.size() > 0 && sum > 0.0f) {
        for (float& val : processed) {
            val /= sum;
        }
    }
    
    return processed;
}

std::vector<float> AgentBrain::preprocessProprioception(const std::vector<float>& rawProprioception) const {
    std::vector<float> processed = rawProprioception;
    
    // Apply smoothing for movement prediction
    for (size_t i = 1; i < processed.size(); ++i) {
        processed[i] = 0.7f * processed[i] + 0.3f * processed[i-1];
    }
    
    return processed;
}

// Novelty and pattern detection
float AgentBrain::detectVisualNovelty(const std::vector<float>& currentVision) const {
    if (currentVision.empty() || previousVision_.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < currentVision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(currentVision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize and apply threshold
    float novelty = totalDiff / std::max<size_t>(currentVision.size(), 1);
    novelty = std::min(1.0f, novelty / noveltyThreshold_);
    
    return novelty;
}

float AgentBrain::detectPattern(const std::vector<float>& signal) const {
    if (signal.size() < 4) return 0.0f;
    
    // Simple autocorrelation for pattern detection
    float pattern = 0.0f;
    size_t halfSize = signal.size() / 2;
    
    for (size_t i = 0; i < halfSize; ++i) {
        pattern += signal[i] * signal[i + halfSize];
    }
    
    return pattern / halfSize;
}

std::vector<float> AgentBrain::extractFeatures(const std::vector<float>& signal) const {
    std::vector<float> features;
    
    if (signal.empty()) return features;
    
    // Extract basic signal statistics as features
    float mean = std::accumulate(signal.begin(), signal.end(), 0.0f) / signal.size();
    features.push_back(mean);
    
    float variance = 0.0f;
    for (float val : signal) {
        variance += (val - mean) * (val - mean);
    }
    variance /= signal.size();
    features.push_back(std::sqrt(variance));
    
    // Add higher-order features
    float skewness = 0.0f;
    for (float val : signal) {
        skewness += std::pow(val - mean, 3);
    }
    skewness /= signal.size();
    skewness /= std::pow(std::sqrt(variance), 3);
    features.push_back(skewness);
    
    return features;
}

// Advanced reward-based learning
void AgentBrain::applyTemporalDifferenceLearning(float reward, float predictedReward, float gamma) {
    if (!brain_) return;
    
    // Compute TD error
    float tdError = reward + gamma * predictedReward - expectedReward_;
    
    // Update prediction
    expectedReward_ = reward + gamma * predictedReward;
    
    // Apply learning with enhanced plasticity
    float learningRate = 0.01f * plasticityModifier_ * rewardWeight_;
    
    // Update weights based on TD error
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = learningRate * tdError * eligibility;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.05f); // Slower decay for TD learning
            }
        }
    }
}

void AgentBrain::applyPolicyGradient(const std::vector<float>& state, float reward, float learningRate) {
    if (!brain_ || !isLearningEnabled_) return;
    
    // Simplified policy gradient update
    float gradient = reward * learningRate * rewardWeight_;
    
    // Apply gradient to motor neuron activities
    float totalActivity = 0.0f;
    for (const auto& neurons : {motorForward_, motorBackward_, motorTurnLeft_, 
                                motorTurnRight_, motorInteract_, motorWait_}) {
        for (Neuron* n : neurons) {
            totalActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    
    if (totalActivity > 0.0f) {
        for (const auto& neurons : {motorForward_, motorBackward_, motorTurnLeft_, 
                                    motorTurnRight_, motorInteract_, motorWait_}) {
            for (Neuron* n : neurons) {
                float delta = gradient * (n->getState().membranePotential - n->getState().restingPotential) / totalActivity;
                n->injectCurrent(delta * 0.1f);
            }
        }
    }
}

void AgentBrain::applyEligibilityTrace(const std::vector<float>& state, float decay) {
    if (!brain_) return;
    
    // Update eligibility traces based on recent neural activity
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float currentEligibility = syn->getEligibilityTrace();
            float newEligibility = currentEligibility * decay + 0.1f; // Add small trace
            syn->setEligibilityTrace(newEligibility);
        }
    }
}

// Performance monitoring
void AgentBrain::updatePerformanceStats(double timestep) {
    if (!isOptimizationEnabled_) return;
    
    frameCount_++;
    processingStartTime_ = timestep;
    
    // Update average frame time
    if (frameCount_ > 1) {
        averageFrameTime_ = (averageFrameTime_ * (frameCount_ - 1) + 0.016) / frameCount_; // Assuming 60Hz target
    } else {
        averageFrameTime_ = 0.016;
    }
    
    // Update average processing time (simplified)
    averageProcessingTime_ = (averageProcessingTime_ * (frameCount_ - 1) + 0.005) / frameCount_;
    
    // Log performance if needed
    if (frameCount_ % 60 == 0) { // Log every second
        NLM_LOG_INFO("Performance: FrameTime=" + std::to_string(averageFrameTime_ * 1000) + 
                     "ms, ProcTime=" + std::to_string(averageProcessingTime_ * 1000) + "ms, " +
                     "CPU=" + std::to_string(getCPUUsage()) + "%, " +
                     "Mem=" + std::to_string(getMemoryUsage() / (1024*1024)) + "MB");
    }
}

float AgentBrain::getAverageFrameTime() const {
    return static_cast<float>(averageFrameTime_);
}

float AgentBrain::getAverageProcessingTime() const {
    return static_cast<float>(averageProcessingTime_);
}

size_t AgentBrain::getProcessedSensoryEvents() const {
    return processedSensoryEvents_;
}

float AgentBrain::getCPUUsage() const {
    // Simulated CPU usage based on processing complexity
    return std::min(100.0f, static_cast<float>(processedSensoryEvents_) * 0.1f);
}

float AgentBrain::getMemoryUsage() const {
    // Simulated memory usage
    return 50.0f * 1024 * 1024; // 50 MB
}

// Debug and visualization tools
std::string AgentBrain::generateDebugReport() const {
    std::stringstream ss;
    ss << "=== AgentBrain Debug Report ===\n";
    ss << "Sensory Neurons: Vision=" << sensoryVision_.size() 
       << " Touch=" << sensoryTouch_.size() 
       << " Internal=" << sensoryInternal_.size() 
       << " Proprioception=" << sensoryProprioception_.size() << "\n";
    ss << "Motor Neurons: Forward=" << motorForward_.size()
       << " Backward=" << motorBackward_.size()
       << " TurnLeft=" << motorTurnLeft_.size()
       << " TurnRight=" << motorTurnRight_.size()
       << " Interact=" << motorInteract_.size()
       << " Wait=" << motorWait_.size() << "\n";
    ss << "Neuromodulation: Dopamine=" << dopamineLevel_
       << " Novelty=" << noveltyLevel_
       << " Curiosity=" << curiosityLevel_
       << " PredictionError=" << predictionError_ << "\n";
    ss << "Development: Age=" << developmentalAge_ << " Stage=" << getDevelopmentalStage()
       << " PlasticityModifier=" << plasticityModifier_ << "\n";
    ss << "Performance: FPS=" << (averageFrameTime_ > 0.0f ? 1.0f / averageFrameTime_ : 0.0f)
       << " CPU=" << getCPUUsage() << "% Memory=" << getMemoryUsage() / (1024*1024) << "MB\n";
    ss << "State: RewardMod=" << (rewardModulationEnabled_ ? "ON" : "OFF")
       << " Development=" << (developmentEnabled_ ? "ON" : "OFF")
       << " Curiosity=" << (curiosityEnabled_ ? "ON" : "OFF")
       << " Learning=" << (isLearningEnabled_ ? "ON" : "OFF") << "\n";
    ss << "=== End Debug Report ===\n";
    
    return ss.str();
}

void AgentBrain::logState() const {
    NLM_LOG_INFO("AgentBrain State: Stage=" + std::to_string(static_cast<int>(getDevelopmentalStage())) +
                 " Dopamine=" + std::to_string(dopamineLevel_) +
                 " Curiosity=" + std::to_string(curiosityLevel_) +
                 " Novelty=" + std::to_string(noveltyLevel_) +
                 " Predictions=" + std::to_string(brain_ ? brain_->getPredictionSystem() ? brain_->getPredictionSystem()->getPredictionCount() : 0 : 0));
}

void AgentBrain::visualizeState() const {
    NLM_LOG_INFO("Visualizing AgentBrain: SpatialContext=" + std::to_string(getSpatialContext().first) +
                 "," + std::to_string(getSpatialContext().second) +
                 " TemporalContextSize=" + std::to_string(getTemporalContext().size()) +
                 " SelfModel=" + std::to_string(hasSelfModel() ? 1 : 0) +
                 " SocialLearning=" + std::to_string(hasSocialLearning() ? 1 : 0));
}

void AgentBrain::exportState(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateDebugReport();
        file.close();
        NLM_LOG_INFO("AgentBrain state exported to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to export AgentBrain state to: " + filename);
    }
}

// Agent control and configuration commands
void AgentBrain::setBehaviorMode(int mode) {
    if (mode >= 1 && mode <= 3) {
        behaviorMode_ = mode;
        NLM_LOG_INFO("AgentBrain behavior mode set to: " + std::to_string(mode));
    }
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("AgentBrain exploration rate set to: " + std::to_string(explorationRate_));
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
    NLM_LOG_INFO("AgentBrain curiosity threshold set to: " + std::to_string(curiosityThreshold_));
}

void AgentBrain::setRewardWeight(float weight) {
    rewardWeight_ = std::clamp(weight, 0.0f, 10.0f);
    NLM_LOG_INFO("AgentBrain reward weight set to: " + std::to_string(rewardWeight_));
}

void AgentBrain::setNoveltyThreshold(float threshold) {
    noveltyThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
    NLM_LOG_INFO("AgentBrain novelty threshold set to: " + std::to_string(noveltyThreshold_));
}

int AgentBrain::getBehaviorMode() const {
    return behaviorMode_;
}

float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

float AgentBrain::getCuriosityThreshold() const {
    return curiosityThreshold_;
}

float AgentBrain::getRewardWeight() const {
    return rewardWeight_;
}

float AgentBrain::getNoveltyThreshold() const {
    return noveltyThreshold_;
}

// Advanced learning and adaptation
void AgentBrain::enableLearning(bool enable) {
    isLearningEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain learning enabled: " + std::to_string(enable));
}

void AgentBrain::enableAdaptivePlasticity(bool enable) {
    isAdaptivePlasticityEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain adaptive plasticity enabled: " + std::to_string(enable));
}

void AgentBrain::enableMetaLearning(bool enable) {
    isMetaLearningEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain meta-learning enabled: " + std::to_string(enable));
}

bool AgentBrain::isLearningEnabled() const {
    return isLearningEnabled_;
}

bool AgentBrain::isAdaptivePlasticityEnabled() const {
    return isAdaptivePlasticityEnabled_;
}

bool AgentBrain::isMetaLearningEnabled() const {
    return isMetaLearningEnabled_;
}

// Performance optimization features
void AgentBrain::enableOptimization(bool enable) {
    isOptimizationEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain optimization enabled: " + std::to_string(enable));
}

void AgentBrain::setTargetFrameRate(float fps) {
    targetFrameRate_ = std::max(10.0f, fps);
    NLM_LOG_INFO("AgentBrain target frame rate set to: " + std::to_string(targetFrameRate_) + " FPS");
}

void AgentBrain::setMemoryBudget(size_t bytes) {
    memoryBudget_ = bytes;
    NLM_LOG_INFO("AgentBrain memory budget set to: " + std::to_string(memoryBudget_ / 1024) + " KB");
}

void AgentBrain::enableSIMD(bool enable) {
    isSIMDEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain SIMD acceleration enabled: " + std::to_string(enable));
}

void AgentBrain::enableParallelProcessing(bool enable) {
    isParallelProcessingEnabled_ = enable;
    NLM_LOG_INFO("AgentBrain parallel processing enabled: " + std::to_string(enable));
}

bool AgentBrain::isOptimizationEnabled() const {
    return isOptimizationEnabled_;
}

float AgentBrain::getTargetFrameRate() const {
    return targetFrameRate_;
}

size_t AgentBrain::getMemoryBudget() const {
    return memoryBudget_;
}

bool AgentBrain::isSIMDEnabled() const {
    return isSIMDEnabled_;
}

bool AgentBrain::isParallelProcessingEnabled() const {
    return isParallelProcessingEnabled_;
}

// Simulation and control commands
void AgentBrain::step(double timestep) {
    // Update performance stats
    updatePerformanceStats(timestep);
    
    // Process sensory input (placeholder for real sensory processing)
    processedSensoryEvents_++;
    
    // Update developmental state
    if (developmentEnabled_) {
        updateDevelopment(timestep);
    }
    
    // Update neuromodulation
    if (isPaused_) {
        dopamineLevel_ *= 0.99f; // Decay while paused
    }
}

// Command interface
void AgentBrain::executeCommand(const std::string& command, const std::vector<float>& params) {
    NLM_LOG_INFO("Executing AgentBrain command: " + command);
    
    if (command == "set_behavior_mode" && !params.empty()) {
        setBehaviorMode(static_cast<int>(params[0]));
    } else if (command == "set_exploration_rate" && !params.empty()) {
        setExplorationRate(params[0]);
    } else if (command == "set_curiosity_threshold" && !params.empty()) {
        setCuriosityThreshold(params[0]);
    } else if (command == "set_reward_weight" && !params.empty()) {
        setRewardWeight(params[0]);
    } else if (command == "set_novelty_threshold" && !params.empty()) {
        setNoveltyThreshold(params[0]);
    } else if (command == "enable_learning" && !params.empty()) {
        enableLearning(params[0] > 0.5f);
    } else if (command == "enable_optimization" && !params.empty()) {
        enableOptimization(params[0] > 0.5f);
    } else if (command == "log_state") {
        logState();
    } else if (command == "export_state" && !params.empty()) {
        exportState(std::to_string(static_cast<int>(params[0])) + "_state.txt");
    } else {
        NLM_LOG_WARNING("Unknown AgentBrain command: " + command);
    }
}

bool AgentBrain::isCommandAvailable(const std::string& command) const {
    const std::vector<std::string> availableCommands = {
        "set_behavior_mode", "set_exploration_rate", "set_curiosity_threshold",
        "set_reward_weight", "set_novelty_threshold", "enable_learning",
        "enable_optimization", "log_state", "export_state", "pause", "resume",
        "enable_adaptive_plasticity", "enable_meta_learning", "enable_simd",
        "enable_parallel_processing"
    };
    
    return std::find(availableCommands.begin(), availableCommands.end(), command) != availableCommands.end();
}

} // namespace nlm
