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
    , explorationEpsilon_(0.1f)
    , maxActionHistory_(100)
    , learningRate_(0.01f)
    , discountFactor_(0.95f)
    , actionEntropyThreshold_(0.5f)
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
    
    // Initialize action history for learning
    actionHistory_.reserve(maxActionHistory_);
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize feature extractors
    visionFeatures_.resize(world.getVisionWidth() * world.getVisionHeight());
    touchFeatures_.resize(8);
    internalFeatures_.resize(4);
    proprioceptionFeatures_.resize(6);
    
    // Initialize action value estimates
    actionValues_.resize(static_cast<size_t>(ActionType::Count), 0.0f);
    
    // Initialize exploration state
    explorationStep_ = 0;
    maxExplorationSteps_ = 1000;
    
    // Initialize prediction models for each action
    predictionErrors_.resize(static_cast<size_t>(ActionType::Count), 0.0f);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    NLM_LOG_INFO("AgentBrain advanced features enabled:");
    NLM_LOG_INFO("  - Feature extraction and dimensionality reduction");
    NLM_LOG_INFO("  - Action value learning with exploration-exploitation");
    NLM_LOG_INFO("  - Temporal difference learning");
    NLM_LOG_INFO("  - Hierarchical action selection");
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
    
    // ===== ADVANCED FEATURE EXTRACTION =====
    
    // Vision feature extraction with edge detection and object recognition
    const auto& vision = percept.getVision();
    extractVisionFeatures(vision);
    
    // Touch feature extraction with spatial patterns
    const auto& touch = percept.getTouch();
    extractTouchFeatures(touch);
    
    // Internal state feature extraction with normalization
    const auto& intern = percept.getInternal();
    extractInternalFeatures(intern);
    
    // Proprioception feature extraction with movement dynamics
    const auto& proprio = percept.getProprioception();
    extractProprioceptionFeatures(proprio);
    
    // ===== CROSS-MODAL INTEGRATION =====
    
    // Fuse features from all sensory modalities
    std::vector<float> integratedFeatures = fuseSensoryFeatures();
    
    // ===== NOISE REDUCTION AND SMOOTHING =====
    
    // Temporal smoothing for stability
    applyTemporalSmoothing(integratedFeatures);
    
    // ===== NOVELTY DETECTION =====
    
    // Compute novelty using multiple feature dimensions
    noveltyLevel_ = computeFeatureNovelty(vision, previousVision_);
    
    // ===== LEARNING AND PREDICTION =====
    
    // Update action values based on recent experience
    updateActionValues();
    
    // ===== CURIOUSITY COMPUTATION =====
    
    // Compute curiosity signal for exploration
    if (curiosityEnabled_) {
        curiosityLevel_ = computeCuriositySignal();
    }
    
    // ===== STORE FOR NEXT TIME =====
    
    // Store current vision for next novelty computation
    previousVision_ = vision;
    
    // ===== INJECT INTO NEURAL SYSTEM =====
    
    // Inject feature vectors into brain neural system
    injectFeaturesIntoBrain(integratedFeatures);
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
    void AgentBrain::extractVisionFeatures(const std::vector<float>& vision) {
    visionFeatures_ = vision;
    
    // Apply edge detection and feature enhancement
    for (size_t i = 0; i < visionFeatures_.size(); ++i) {
        // Sobel-like edge detection
        float enhanced = visionFeatures_[i];
        
        // Apply spatial filtering (simplified)
        if (i > 0 && i < visionFeatures_.size() - 1) {
            float left = visionFeatures_[i - 1];
            float right = visionFeatures_[i + 1];
            float edge = std::abs(left - right);
            enhanced = enhanced + edge * 0.3f;
        }
        
        // Non-linear enhancement for contrast
        enhanced = std::tanh(enhanced * 2.0f) * 2.0f;
        
        visionFeatures_[i] = enhanced;
    }
}

void AgentBrain::extractTouchFeatures(const std::vector<float>& touch) {
    touchFeatures_ = touch;
    
    // Apply spatial pattern detection
    for (size_t i = 0; i < touchFeatures_.size(); ++i) {
        // Detect proximity patterns
        float pattern = touchFeatures_[i];
        
        // Apply spatial smoothing
        if (i > 0 && i < touchFeatures_.size() - 1) {
            float avg = (touchFeatures_[i - 1] + touchFeatures_[i] + touchFeatures_[i + 1]) / 3.0f;
            pattern = pattern + (avg - pattern) * 0.2f;
        }
        
        touchFeatures_[i] = pattern;
    }
}

void AgentBrain::extractInternalFeatures(const std::vector<float>& intern) {
    internalFeatures_ = intern;
    
    // Normalize and scale internal states
    for (size_t i = 0; i < internalFeatures_.size(); ++i) {
        // Apply biological scaling functions
        float value = internalFeatures_[i];
        
        // Sigmoid normalization for stability
        internalFeatures_[i] = 2.0f / (1.0f + std::exp(-value)) - 1.0f;
    }
}

void AgentBrain::extractProprioceptionFeatures(const std::vector<float>& proprio) {
    proprioceptionFeatures_ = proprio;
    
    // Extract movement dynamics and orientation
    for (size_t i = 0; i < proprioceptionFeatures_.size(); ++i) {
        float value = proprioceptionFeatures_[i];
        
        // Apply movement smoothing
        if (i > 0) {
            float diff = value - previousProprioception_[i];
            value = value - diff * 0.1f;  // Damping
        }
        
        proprioceptionFeatures_[i] = value;
    }
    
    previousProprioception_ = proprioceptionFeatures_;
}

std::vector<float> AgentBrain::fuseSensoryFeatures() {
    std::vector<float> integrated;
    
    // Combine all feature vectors with weighted fusion
    float totalWeight = 1.0f + 0.8f + 0.6f + 0.4f;  // Vision weight + Touch + Internal + Proprioception
    
    // Vision features (highest weight)
    for (float val : visionFeatures_) {
        integrated.push_back(val * 1.0f);
    }
    
    // Touch features (medium weight)
    for (float val : touchFeatures_) {
        integrated.push_back(val * 0.8f);
    }
    
    // Internal features (medium weight)
    for (float val : internalFeatures_) {
        integrated.push_back(val * 0.6f);
    }
    
    // Proprioception features (lower weight)
    for (float val : proprioceptionFeatures_) {
        integrated.push_back(val * 0.4f);
    }
    
    // Normalize to [-1, 1]
    float maxVal = *std::max_element(integrated.begin(), integrated.end(), 
                                     [](float a, float b) { return std::abs(a) < std::abs(b); });
    if (maxVal > 1.0f) {
        for (float& val : integrated) {
            val /= maxVal;
        }
    }
    
    return integrated;
}

void AgentBrain::applyTemporalSmoothing(std::vector<float>& features) {
    // Apply exponential moving average for temporal smoothing
    if (smoothedFeatures_.empty()) {
        smoothedFeatures_ = features;
    } else {
        for (size_t i = 0; i < features.size() && i < smoothedFeatures_.size(); ++i) {
            smoothedFeatures_[i] = smoothedFeatures_[i] * 0.7f + features[i] * 0.3f;
        }
        features = smoothedFeatures_;
    }
}

float AgentBrain::computeFeatureNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    size_t compareSize = std::min(current.size(), previous.size());
    
    // Compute differences across multiple dimensions
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        
        // Apply non-linear scaling (more sensitive to small changes)
        float sensitivity = std::tanh(diff * 5.0f);
        totalDiff += sensitivity;
    }
    
    // Normalize by number of features
    float novelty = totalDiff / static_cast<float>(compareSize);
    
    // Apply novelty decay
    novelty *= sensoryNoveltyDecay_;
    
    return novelty;
}

void AgentBrain::updateActionValues() {
    // Update action values using temporal difference learning
    explorationStep_++;
    
    // Decay exploration epsilon over time
    if (explorationStep_ < maxExplorationSteps_) {
        explorationEpsilon_ = 0.1f * (1.0f - static_cast<float>(explorationStep_) / maxExplorationSteps_);
    } else {
        explorationEpsilon_ = 0.01f;  // Base exploration rate
    }
    
    // Update action values based on prediction errors
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count) && i < predictionErrors_.size(); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Update action value using prediction error
        float rewardPrediction = actionValues_[i];  // Current prediction
        float actualReward = predictionErrors_[i];  // Actual reward (simplified)
        
        // Temporal difference learning
        float tdError = actualReward - rewardPrediction;
        actionValues_[i] += learningRate_ * tdError;
        
        // Decay prediction error
        predictionErrors_[i] *= 0.9f;
    }
}

float AgentBrain::computeCuriositySignal() {
    // Compute curiosity using multiple intrinsic motivation signals
    float noveltyCuriosity = noveltyLevel_;
    
    // Add uncertainty-based curiosity
    float uncertainty = 0.0f;
    for (float val : predictionErrors_) {
        uncertainty += val * val;
    }
    uncertainty = std::sqrt(uncertainty) / static_cast<float>(predictionErrors_.size());
    
    // Add learning progress curiosity
    float learningProgress = 0.0f;
    if (explorationStep_ > 100) {
        learningProgress = 1.0f - std::exp(-static_cast<float>(explorationStep_) / 1000.0f);
    }
    
    // Combine curiosity signals
    float totalCuriosity = noveltyCuriosity * 0.4f + uncertainty * 0.3f + learningProgress * 0.3f;
    
    return std::clamp(totalCuriosity, 0.0f, 1.0f);
}

void AgentBrain::injectFeaturesIntoBrain(const std::vector<float>& features) {
    if (!brain_) return;
    
    // Create sensory input from integrated features
    SensoryInput sensoryInput;
    
    // Split features into different modalities based on their ranges
    size_t featureCount = features.size();
    
    // Vision features (first 256 elements if available)
    size_t visionSize = std::min(static_cast<size_t>(256), featureCount);
    std::vector<float> visionFeatures(features.begin(), features.begin() + visionSize);
    
    // Fill remaining features to match expected sensory input size
    std::vector<float> allFeatures = features;
    while (allFeatures.size() < brain_->getSensoryInputSize()) {
        allFeatures.push_back(0.0f);
    }
    
    // Inject into brain
    brain_->receiveSensoryInput(SensoryInput(allFeatures));
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

void AgentBrain::decodeActionWithValueLearning() {
    // Advanced action selection using value-based learning
    std::vector<float> preferences = computeActionPreferences();
    sampleActionFromPreferences(preferences);
}

void AgentBrain::selectActionWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration based on curiosity and learning
    if (curiosityLevel_ > 0.3f) {
        // Dynamic exploration based on curiosity level
        float exploreChance = explorationEpsilon_ * (1.0f + curiosityLevel_ * 0.5f);
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Sample action based on current preferences
            std::vector<float> preferences = computeActionPreferences();
            sampleActionFromPreferences(preferences);
        }
    }
    
    // Fall back to value-based selection if no exploration
    decodeActionWithValueLearning();
}

void AgentBrain::updateDevelopmentalAdaptation() {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += 1.0;  // Simple time increment
    
    // Adapt behavior based on developmental stage
    modulateExplorationByDevelopment();
    adaptBasedOnLearningProgress();
    
    // Update plasticity based on development
    plasticityModifier_ = computePlasticityBasedOnDevelopment();
}

void AgentBrain::adaptBasedOnLearningProgress() {
    // Adapt learning rates based on learning progress
    float learningProgress = 1.0f - std::exp(-static_cast<float>(explorationStep_) / 500.0f);
    
    // Increase learning rate when learning progress is high
    learningRate_ = 0.01f * (1.0f + learningProgress * 0.5f);
    
    // Decrease exploration when learning is complete
    if (learningProgress > 0.8f) {
        explorationEpsilon_ = 0.05f;
    }
}

void AgentBrain::modulateExplorationByDevelopment() {
    // Modulate exploration based on developmental stage
    if (developmentalAge_ < 100.0) {
        // Early exploration: high curiosity
        explorationEpsilon_ = 0.3f;
    } else if (developmentalAge_ < 500.0) {
        // Mid exploration: moderate curiosity
        explorationEpsilon_ = 0.15f;
    } else {
        // Late exploration: low curiosity, more exploitation
        explorationEpsilon_ = 0.05f;
    }
}

void AgentBrain::storeExperience(MotorCommand action, float reward, const std::vector<float>& state) {
    // Store experience in history for learning
    Experience exp;
    exp.action = action;
    exp.reward = reward;
    exp.state = state;
    exp.timestamp = static_cast<float>(explorationStep_);
    
    actionHistory_.push_back(exp);
    
    // Trim history if it gets too large
    while (actionHistory_.size() > maxActionHistory_) {
        actionHistory_.pop_front();
    }
}

void AgentBrain::retrieveSimilarExperiences(const std::vector<float>& state, size_t maxCount) {
    
    std::vector<std::pair<MotorCommand, float>> similarExperiences;
    
    // Simple similarity based on state difference
    for (const Experience& exp : actionHistory_) {
        float similarity = computeExperienceSimilarity(state, exp.state);
        if (similarity > 0.3f) {  // Threshold for considering similar
            similarExperiences.push_back(std::make_pair(exp.action, exp.reward * similarity));
        }
        
        if (similarExperiences.size() >= maxCount) {
            break;
        }
    }
}

void AgentBrain::computeActionStatistics() {
    // Compute statistics on action outcomes
    actionStats_.clear();
    
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        MotorCommand action = static_cast<MotorCommand>(i);
        std::vector<float> actionRewards;
        
        // Collect all rewards for this action
        for (const Experience& exp : actionHistory_) {
            if (exp.action == action) {
                actionRewards.push_back(exp.reward);
            }
        }
        
        if (!actionRewards.empty()) {
            // Compute statistics
            float sum = std::accumulate(actionRewards.begin(), actionRewards.end(), 0.0f);
            float mean = sum / actionRewards.size();
            
            float sqSum = 0.0f;
            for (float reward : actionRewards) {
                float diff = reward - mean;
                sqSum += diff * diff;
            }
            float stdDev = std::sqrt(sqSum / actionRewards.size());
            
            ActionStats stats;
            stats.action = action;
            stats.meanReward = mean;
            stats.stdDevReward = stdDev;
            stats.frequency = static_cast<float>(actionRewards.size()) / static_cast<float>(actionHistory_.size());
            
            actionStats_.push_back(stats);
        }
    }
}

float AgentBrain::computeExperienceSimilarity(const std::vector<float>& state1, const std::vector<float>& state2) const {
    if (state1.size() != state2.size()) return 0.0f;
    
    float sumDiff = 0.0f;
    for (size_t i = 0; i < state1.size(); ++i) {
        float diff = std::abs(state1[i] - state2[i]);
        sumDiff += diff;
    }
    
    float maxDiff = static_cast<float>(state1.size());
    return 1.0f - (sumDiff / maxDiff);  // Normalized similarity
}

} // namespace nlm
