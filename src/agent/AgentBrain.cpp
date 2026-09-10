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
            // Apply multi-scale feature detection for richer sensory processing
            float processedInput = vision[i];
            
            // Multi-scale feature extraction
            if (i > 0 && i < sensoryVision_.size() - 1) {
                // Edge detection enhancement
                float leftNeighbor = (i > 0) ? vision[i-1] : 0.0f;
                float rightNeighbor = (i < vision.size() - 1) ? vision[i+1] : 0.0f;
                float edgeResponse = std::abs(leftNeighbor - vision[i]) + std::abs(rightNeighbor - vision[i]);
                processedInput += edgeResponse * 0.1f;
            }
            
            // Contrast normalization
            float maxVision = *std::max_element(vision.begin(), vision.end());
            float minVision = *std::min_element(vision.begin(), vision.end());
            if (maxVision - minVision > 0.1f) {
                processedInput = (processedInput - minVision) / (maxVision - minVision) * 2.0f - 0.5f;
            }
            
            // Inject current proportional to processed vision intensity
            float current = processedInput * 6.0f;  // Enhanced scaling
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Multi-dimensional touch processing
            float processedTouch = touch[i];
            
            // Normalize touch pressure (0-1 range)
            processedTouch = std::clamp(processedTouch, 0.0f, 1.0f);
            
            // Apply non-linear response (Weber-Fechner law)
            processedTouch = std::log(processedTouch * 10.0f + 1.0f) / std::log(11.0f);
            
            float current = processedTouch * 10.0f;  // Enhanced scaling
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Complex internal signal processing
            float processedInternal = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            
            // Apply non-linear transformation based on homeostatic regulation
            float homeostaticFactor = 1.0f + (std::sin(intern[i] * 10.0f) * 0.2f);
            processedInternal *= homeostaticFactor;
            
            // Temporal filtering for smooth state changes
            static float previousInternal = 0.0f;
            float filteredInternal = processedInternal * 0.7f + previousInternal * 0.3f;
            previousInternal = filteredInternal;
            
            sensoryInternal_[i]->injectCurrent(filteredInternal);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Advanced proprioceptive processing
            float processedProprio = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            
            // Velocity and acceleration integration
            static float previousProprio = 0.0f;
            float velocity = processedProprio - previousProprio;
            float acceleration = velocity - (previousProprio - processedProprio);
            previousProprio = processedProprio;
            
            // Combine position, velocity, and acceleration
            float enrichedProprio = processedProprio + velocity * 0.1f + acceleration * 0.01f;
            
            // Smoothing filter
            static float filteredProprio = 0.0f;
            filteredProprio = enrichedProprio * 0.8f + filteredProprio * 0.2f;
            
            sensoryProprioception_[i]->injectCurrent(filteredProprio);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            
            // Emphasize differences in high-contrast areas
            float avgNeighbor = 0.0f;
            size_t count = 0;
            if (i > 0) { avgNeighbor += vision[i-1]; count++; }
            if (i < vision.size() - 1) { avgNeighbor += vision[i+1]; count++; }
            if (count > 0) avgNeighbor /= count;
            
            float contrast = std::abs(vision[i] - avgNeighbor);
            diff *= (1.0f + contrast);  // Emphasize contrast changes
            
            totalDiff += diff;
        }
        
        // Enhanced novelty computation with temporal filtering
        float temporalSmoothing = 0.9f;
        noveltyLevel_ = noveltyLevel_ * temporalSmoothing + totalDiff / std::max<size_t>(vision.size(), 1) * (1.0f - temporalSmoothing);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity with more sophisticated computation
    if (curiosityEnabled_) {
        // Multi-factor curiosity computation
        float noveltyContribution = noveltyLevel_ * 2.0f;
        
        // Prediction error contribution (enhanced)
        float predictionErrorContribution = std::abs(predictionError_) * 0.8f;
        
        // Intrinsic motivation factor (exploration vs exploitation)
        float intrinsicFactor = 1.0f - std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Adaptive curiosity scaling based on recent rewards
        float recentRewardFactor = std::min(2.0f, std::max(0.1f, 1.0f + totalReward * 0.1f));
        
        // Combined curiosity with adaptive weighting
        curiosityLevel_ = (noveltyContribution * 0.5f + 
                          predictionErrorContribution * 0.3f + 
                          intrinsicFactor * 0.1f + 
                          recentRewardFactor * 0.1f);
        
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

// Advanced Learning Methods
void AgentBrain::enableAdvancedLearning(bool enable) {
    advancedLearningEnabled_ = enable;
    
    if (enable && brain_) {
        // Initialize advanced learning components
        initializeMetaLearning();
        initializeSkillSystem();
        initializeSocialLearning();
        initializeHierarchicalLearning();
    }
}

void AgentBrain::initializeMetaLearning() {
    metaLearningState_.learningRate = 0.01f;
    metaLearningState_.adaptationRate = 0.005f;
    metaLearningState_.explorationRate = 0.1f;
    metaLearningState_.exploitationRate = 0.9f;
    metaLearningState_.taskUncertainty = 1.0f;
    metaLearningState_.metaGradient = 0.0f;
}

void AgentBrain::initializeSkillSystem() {
    skillSystem_.maxSkills = 20;
    skillSystem_.skillAcquisitionRate = 0.001f;
    skillSystem_.skillTransferRate = 0.3f;
    skillSystem_.skillDecayRate = 0.0005f;
    skillSystem_.currentSkillLevel = 0.0f;
}

void AgentBrain::initializeSocialLearning() {
    socialLearningState_.socialObservationEnabled = true;
    socialLearningState_.imitationStrength = 0.3f;
    socialLearningState_.teachableMomentThreshold = 0.7f;
    socialLearningState_.observerNeuronsActive = 0;
    socialLearningState_.modelTeacherNeuronsActive = 0;
}

void AgentBrain::initializeHierarchicalLearning() {
    hierarchicalState_.levels = 3;
    hierarchicalState_.topDownControl = 1.0f;
    hierarchicalState_.bottomUpSignals = 0.0f;
    hierarchicalState_.subgoalProgress = 0.0f;
}

void AgentBrain::updateAdvancedLearning(double timestep) {
    if (!advancedLearningEnabled_ || !brain_) return;
    
    // Update meta-learning
    updateMetaLearningState(timestep);
    
    // Update skill acquisition
    updateSkillSystem(timestep);
    
    // Update social learning
    updateSocialLearning(timestep);
    
    // Update hierarchical control
    updateHierarchicalLearning(timestep);
}

void AgentBrain::updateMetaLearningState(double timestep) {
    // Adaptive learning rate based on performance
    float performance = calculatePerformance();
    float targetLearningRate = 0.1f / std::max(1.0f, performance);
    
    // Smooth update
    metaLearningState_.learningRate += 
        (targetLearningRate - metaLearningState_.learningRate) * 0.1f;
    
    // Meta-gradient descent for learning rate adaptation
    float errorSignal = 1.0f - performance;
    metaLearningState_.metaGradient = errorSignal * metaLearningState_.learningRate;
    
    // Update exploration-exploitation balance based on uncertainty
    metaLearningState_.taskUncertainty = 1.0f - performance;
    metaLearningState_.explorationRate = std::min(0.3f, metaLearningState_.taskUncertainty * 0.3f);
    metaLearningState_.exploitationRate = 1.0f - metaLearningState_.explorationRate;
    
    // Apply meta-gradient to learning
    float effectiveLearning = metaLearningState_.learningRate + 
                              metaLearningState_.metaGradient * 0.01f;
    
    // Apply meta-learning to plasticity systems
    auto* stdp = brain_->getSTDP();
    auto* hebbian = brain_->getHebbian();
    if (stdp && hebbian) {
        float plasticityModulation = 1.0f + effectiveLearning * 2.0f;
        stdp->setLTPWeight(0.01f * plasticityModulation);
        stdp->setLTDWeight(0.012f * plasticityModulation);
        hebbian->setLearningRate(0.01f * plasticityModulation);
    }
}

void AgentBrain::updateSkillSystem(double timestep) {
    // Update current skill level based on performance
    skillSystem_.currentSkillLevel += skillSystem_.skillAcquisitionRate * performance * timestep;
    skillSystem_.currentSkillLevel = std::min(1.0f, skillSystem_.currentSkillLevel);
    
    // Skill decay over time if not used
    skillSystem_.currentSkillLevel *= (1.0f - skillSystem_.skillDecayRate * timestep);
    
    // Skill transfer between related tasks
    if (dopamineLevel_ > 0.5f) {
        // High dopamine facilitates skill transfer
        skillSystem_.skillTransfer *= (1.0f + skillSystem_.skillTransferRate * timestep);
    }
    
    // Check for new skill opportunities
    if (curiosityLevel_ > 0.8f && performance < 0.5f) {
        // High curiosity and low performance may indicate need for new skills
        skillSystem_.skillAcquisitionRate *= 1.5f;
    }
}

void AgentBrain::updateSocialLearning(double timestep) {
    // Update social learning state based on observed behaviors
    if (socialLearningState_.socialObservationEnabled) {
        // Count active teacher and observer neurons
        size_t teacherNeurons = 0;
        size_t observerNeurons = 0;
        
        for (const auto& neuron : brain_->getRegions()[0]->getAllNeurons()) {
            // Simplified check - in real implementation would use actual neuron types
            if (neuron->getState().membranePotential > 0.0f) {
                if (teacherNeurons < 10) teacherNeurons++;
                if (observerNeurons < 5) observerNeurons++;
            }
        }
        
        socialLearningState_.modelTeacherNeuronsActive = teacherNeurons;
        socialLearningState_.observerNeuronsActive = observerNeurons;
        
        // Social learning activation based on teacher availability
        if (teacherNeurons > 0) {
            float socialLearningActivation = 
                socialLearningState_.imitationStrength * 
                (static_cast<float>(observerNeurons) / std::max(1, teacherNeurons));
            
            // Apply social learning to policy
            applySocialPolicy(socialLearningActivation, timestep);
        }
    }
}

void AgentBrain::updateHierarchicalLearning(double timestep) {
    // Update hierarchical control with error signals
    float predictionError = std::abs(predictionError_);
    
    // Bottom-up signals from prediction error
    hierarchicalState_.bottomUpSignals = predictionError * 0.5f;
    
    // Top-down control for goal achievement
    hierarchicalState_.topDownControl = std::clamp(
        hierarchicalState_.topDownControl + (1.0f - hierarchicalState_.subgoalProgress) * 0.1f,
        0.0f, 1.0f
    );
    
    // Update subgoal progress
    if (dopamineLevel_ > 0.0f) {
        hierarchicalState_.subgoalProgress += dopamineLevel_ * 0.05f * timestep;
        hierarchicalState_.subgoalProgress = std::min(1.0f, hierarchicalState_.subgoalProgress);
    }
    
    // Hierarchical policy selection
    int selectedLevel = selectHierarchicalLevel();
    applyHierarchicalPolicy(selectedLevel, timestep);
}

void AgentBrain::applySocialPolicy(float socialStrength, double timestep) {
    // Apply social learning to motor command selection
    if (socialStrength > 0.1f && curiosityLevel_ > 0.3f) {
        // Consider social models in action selection
        float socialBias = socialStrength * curiosityLevel_;
        
        // Modify motor command probabilities based on social observation
        // In real implementation, would influence neural populations
        socialPolicyInfluence_ = socialBias;
    }
}

void AgentBrain::applyHierarchicalPolicy(int level, double timestep) {
    // Apply hierarchical control to action selection
    float hierarchicalBias = hierarchicalState_.topDownControl * (level + 1) / hierarchicalState_.levels;
    
    // Modify action selection based on level
    if (level == 0) {
        // Low-level: motor execution
        // Apply precise motor control
        motorPrecision_ = hierarchicalBias * 2.0f;
    } else if (level == 1) {
        // Mid-level: action selection
        // Apply goal-directed selection
        goalDirectedness_ = hierarchicalBias * 2.0f;
    } else if (level == 2) {
        // High-level: strategic planning
        // Apply long-term planning
        planningHorizon_ = hierarchicalBias * 10.0f;  // Time steps
    }
}

int AgentBrain::selectHierarchicalLevel() {
    // Select which hierarchical level to emphasize based on task demands
    float errorSignal = std::abs(predictionError_);
    
    if (errorSignal > 0.8f) {
        // High error - need more flexible, exploratory behavior
        return 2;  // High-level planning
    } else if (errorSignal > 0.3f) {
        // Moderate error - balanced approach
        return 1;  // Mid-level action selection
    } else {
        // Low error - rely on learned policies
        return 0;  // Low-level execution
    }
}

float AgentBrain::calculatePerformance() const {
    // Calculate overall performance based on multiple metrics
    float rewardPerformance = std::clamp(expectedReward_, 0.0f, 1.0f);
    float noveltyPerformance = noveltyLevel_;  // Novelty indicates exploration
    float efficiencyPerformance = 1.0f - (predictionError_ * predictionError_);  // Lower error = better
    
    // Weighted combination
    float performance = rewardPerformance * 0.5f + 
                       noveltyPerformance * 0.2f + 
                       efficiencyPerformance * 0.3f;
    
    return performance;
}

void AgentBrain::enableMetaLearning(bool enable) {
    metaLearningEnabled_ = enable;
}

void AgentBrain::enableSkillAcquisition(bool enable) {
    skillAcquisitionEnabled_ = enable;
}

void AgentBrain::enableSocialLearning(bool enable) {
    socialLearningEnabled_ = enable;
}

void AgentBrain::enableHierarchicalControl(bool enable) {
    hierarchicalControlEnabled_ = enable;
}

bool AgentBrain::isAdvancedLearningEnabled() const {
    return advancedLearningEnabled_;
}

void AgentBrain::resetAdvancedLearning() {
    // Reset advanced learning states for new episode
    metaLearningState_.learningRate = 0.01f;
    metaLearningState_.explorationRate = 0.1f;
    metaLearningState_.exploitationRate = 0.9f;
    metaLearningState_.taskUncertainty = 1.0f;
    metaLearningState_.metaGradient = 0.0f;
    
    skillSystem_.currentSkillLevel = 0.0f;
    skillSystem_.skillTransfer = 1.0f;
    
    socialLearningState_.observerNeuronsActive = 0;
    socialLearningState_.modelTeacherNeuronsActive = 0;
    
    hierarchicalState_.topDownControl = 1.0f;
    hierarchicalState_.bottomUpSignals = 0.0f;
    hierarchicalState_.subgoalProgress = 0.0f;
    
    // Reset curiosity and exploration
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
}
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

} // namespace nlm
