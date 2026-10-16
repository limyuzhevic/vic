#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config)
    // Add new member variables to the constructor
    , explorationLevel_(0.0f)
    , lastExplorationTime_(0.0)
    , currentTime_(0.0)
    , timeStep_(0.001)
    , socialBehaviorPattern_()
    , socialModels_()
    , learningHistory_()
    , patterns_()
    , explorationLevel_(0.0f)
    , lastExplorationTime_(0.0)
    , currentTime_(0.0)
    , timeStep_(0.001)
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

// Extract features for pattern recognition and memory integration
    std::vector<float> AgentBrain::extractFeatures(const std::vector<float>& vision) const {
        std::vector<float> features;
        
        if (vision.empty()) return features;
        
        // Apply simple feature extraction: downsampling and basic statistics
        size_t sampleStep = std::max<size_t>(1, vision.size() / 32);
        for (size_t i = 0; i < vision.size(); i += sampleStep) {
            if (i < vision.size()) {
                features.push_back(vision[i]);
            }
        }
        
        // Add statistical features for better pattern recognition
        if (!vision.empty()) {
            float sum = std::accumulate(vision.begin(), vision.end(), 0.0f);
            float mean = sum / vision.size();
            
            float variance = 0.0f;
            for (float val : vision) {
                variance += (val - mean) * (val - mean);
            }
            variance /= vision.size();
            
            features.push_back(mean);
            features.push_back(std::sqrt(variance));
            
            // Add edge detection features
            float maxVal = *std::max_element(vision.begin(), vision.end());
            float minVal = *std::min_element(vision.begin(), vision.end());
            features.push_back(maxVal - minVal);  // contrast
        }
        
        return features;
    }
    
    // Record patterns for long-term memory and prediction
    void AgentBrain::recordPattern(const std::vector<float>& features) {
        if (features.empty() || features.size() < 8) return;
        
        // Check for similar existing patterns
        float bestSimilarity = 0.0f;
        int bestIndex = -1;
        
        for (size_t i = 0; i < patterns_.size(); ++i) {
            float similarity = computePatternSimilarity(features, patterns_[i].features);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestIndex = i;
            }
        }
        
        if (bestSimilarity > 0.6f && bestIndex >= 0) {
            // Update existing pattern with new experience
            patterns_[bestIndex].strength = patterns_[bestIndex].strength * 0.7f + bestSimilarity * 0.3f;
            patterns_[bestIndex].lastSeen = currentTime_;
            
            // Add to learning history
            LearningEpisode episode;
            episode.timestamp = currentTime_;
            episode.curiosity = curiosityLevel_;
            episode.novelty = noveltyLevel_;
            episode.action = MotorCommand::Wait; // Will be updated when action is taken
            episode.predictionError = predictionError_;
            episode.dopamineLevel = dopamineLevel_;
            learningHistory_.push_back(episode);
            
            // Keep learning history limited
            if (learningHistory_.size() > 1000) {
                learningHistory_.erase(learningHistory_.begin());
            }
        } else {
            // Create new pattern
            Pattern newPattern;
            newPattern.features = features;
            newPattern.strength = bestSimilarity;
            newPattern.lastSeen = currentTime_;
            patterns_.push_back(newPattern);
            
            // Keep patterns limited to top performers
            if (patterns_.size() > 100) {
                std::sort(patterns_.begin(), patterns_.end(),
                         [](const Pattern& a, const Pattern& b) {
                             return a.strength > b.strength;
                         });
                patterns_.resize(100);
            }
        }
    }
    
    float AgentBrain::computePatternSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.empty() || b.empty()) return 0.0f;
        
        size_t minSize = std::min(a.size(), b.size());
        if (minSize == 0) return 0.0f;
        
        float similarity = 0.0f;
        for (size_t i = 0; i < minSize; ++i) {
            similarity += 1.0f - std::abs(a[i] - b[i]);  // Cosine-like similarity
        }
        
        return similarity / minSize;
    }
    
    // Update social learning models based on observed behaviors
    void AgentBrain::updateSocialModels(const std::vector<float>& behavior) {
        if (behavior.empty()) return;
        
        // Update social behavior pattern for future imitation
        socialBehaviorPattern_ = behavior;
        
        // Update reliability of observed behaviors based on prediction accuracy
        for (auto& model : socialModels_) {
            float predictedReliability = 0.0f;
            
            if (!socialBehaviorPattern_.empty() && !model.behaviorPattern.empty()) {
                float similarity = computePatternSimilarity(socialBehaviorPattern_, model.behaviorPattern);
                predictedReliability = similarity * model.reliability;
            }
            
            // Decay reliability and add prediction accuracy
            model.reliability = model.reliability * 0.8f + predictedReliability * 0.2f;
            model.reliability = std::clamp(model.reliability, 0.1f, 1.0f);
            
            // Update observation time if pattern changed
            double timeSinceLastUpdate = currentTime_ - model.lastObserved;
            if (timeSinceLastUpdate > config_.socialObservationWindow) {
                model.lastObserved = currentTime_;
                model.behaviorPattern = socialBehaviorPattern_;
            }
        }
    }
    
    // Observe and learn from other agents' behaviors
    void AgentBrain::observeSocialBehavior(const std::string& modelId, const std::vector<float>& behavior) {
        // Check if we already have a model for this agent
        for (auto& model : socialModels_) {
            if (model.id == modelId) {
                // Update existing model with new observation
                model.behaviorPattern = behavior;
                model.lastObserved = currentTime_;
                model.reliability = std::min(1.0f, model.reliability + 0.15f);  // Boost reliability
                return;
            }
        }
        
        // Create new social model
        SocialModel newModel;
        newModel.id = modelId;
        newModel.behaviorPattern = behavior;
        newModel.reliability = 0.6f;  // Start with medium-high reliability for observed behaviors
        newModel.lastObserved = currentTime_;
        socialModels_.push_back(newModel);
    }
    
    // Use learned social behaviors to guide our own actions
    MotorCommand AgentBrain::imitateSocialBehavior() {
        if (socialModels_.empty()) return MotorCommand::Wait;
        
        // Find most reliable and recently observed model
        SocialModel* bestModel = nullptr;
        float bestScore = 0.0f;
        
        for (auto& model : socialModels_) {
            // Calculate score based on reliability and recency
            double recencyScore = 1.0;  // Higher score for recent observations
            double timeSinceObservation = currentTime_ - model.lastObserved;
            if (timeSinceObservation > config_.socialObservationWindow) {
                recencyScore *= std::pow(0.5, timeSinceObservation / config_.socialObservationWindow);
            }
            
            float totalScore = model.reliability * recencyScore;
            
            if (totalScore > bestScore && model.reliability > config_.socialModelConfidenceThreshold) {
                bestScore = totalScore;
                bestModel = &model;
            }
        }
        
        if (!bestModel) return MotorCommand::Wait;
        
        // Convert social behavior pattern to motor command using learned mapping
        if (!bestModel->behaviorPattern.empty() && bestModel->behaviorPattern.size() >= 3) {
            float patternSum = std::accumulate(bestModel->behaviorPattern.begin(), 
                                              bestModel->behaviorPattern.end(), 0.0f);
            float averageBehavior = patternSum / bestModel->behaviorPattern.size();
            
            // Apply sigmoid function to map to action probabilities
            float actionProb = 1.0f / (1.0f + std::exp(-10.0f * (averageBehavior - 0.5f)));
            
            // Choose action based on mapped probability
            if (actionProb > 0.7f) {
                return MotorCommand::MoveForward;
            } else if (actionProb > 0.4f) {
                return MotorCommand::TurnLeft;
            } else if (actionProb > 0.2f) {
                return MotorCommand::Wait;
            } else {
                return MotorCommand::TurnRight;
            }
        }
        
        return MotorCommand::Wait;
    }
    
    // Update exploration level based on curiosity and internal state
    void AgentBrain::updateExplorationLevel() {
        double timeSinceExploration = currentTime_ - lastExplorationTime_;
        
        if (curiosityLevel_ > config_.explorationThreshold) {
            // Increase exploration level when curiosity is high
            explorationLevel_ = std::min(1.0f, explorationLevel_ + config_.explorationRandomFactor * curiosityLevel_);
            lastExplorationTime_ = currentTime_;
        } else {
            // Gradually decrease exploration level over time (exploration decay)
            explorationLevel_ = std::max(0.0f, explorationLevel_ * (1.0f - config_.explorationCoolDown * timeStep_));
        }
    }
    
    // Get exploration level
    float AgentBrain::getExplorationLevel() const {
        return explorationLevel_;
    }
    
    // Get developmental stage with enhanced information
    devMentalStage AgentBrain::getDevelopmentalStage() const {
        if (!brain_) return DevelopmentalStage::Initial;
        return brain_->getDevelopmentalStage();
    }
    
    // Get neuromodulation level with enhanced information
    float AgentBrain::getNeuromodulationLevel() const {
        return dopamineLevel_;
    }
    
    // Get curiosity level with enhanced information
    float AgentBrain::getCuriosityLevel() const {
        return curiosityLevel_;
    }
    
    // Get novelty level with enhanced information
    float AgentBrain::getNoveltyLevel() const {
        return noveltyLevel_;
    }
    
    // Get prediction error with enhanced information
    float AgentBrain::getPredictionError() const {
        return predictionError_;
    }
    
    // Get developmental age with enhanced information
    double AgentBrain::getDevelopmentalAge() const {
        return developmentalAge_;
    }
    
    // Get plasticity modifier with enhanced information
    float AgentBrain::getPlasticityModifier() const {
        return plasticityModifier_;
    }
    
    // Reset all state including learning history and patterns
    void AgentBrain::reset() {
        // Reset core neuromodulation state
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = config_.curiosityBaseLevel;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        explorationLevel_ = 0.0f;
        lastExplorationTime_ = 0.0;
        currentTime_ = 0.0;
        
        // Clear sensory history
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        
        // Clear learning components
        learningHistory_.clear();
        patterns_.clear();
        socialModels_.clear();
        socialBehaviorPattern_.clear();
    }
    
    // Get learning history for analysis and debugging
    std::vector<AgentBrain::LearningEpisode> AgentBrain::getLearningHistory() const {
        return learningHistory_;
    }
    
    // Clear learning history
    void AgentBrain::clearLearningHistory() {
        learningHistory_.clear();
    }
    
    // Get learned patterns for analysis
    std::vector<AgentBrain::Pattern> AgentBrain::getPatterns() const {
        return patterns_;
    }
    
    // Clear learned patterns
    void AgentBrain::clearPatterns() {
        patterns_.clear();
    }
