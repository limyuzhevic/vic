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
    , achLevel_(0.0f)
    , neLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , neuromodulationProfile_{0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f}
    , currentActionPlan_()
    , workingMemoryBuffer_()
    , episodicMemory_()
    , plasticityBoost_(0.0f)
    , developmentSpeedMultiplier_(1.0f)
    , profileName_("")
    , config_(nullptr)
    , performanceMonitoringInterval_(1.0f)
    , performanceStats_{0.0f, 0.0f, 0.0f, {}}
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
    achLevel_ = 0.0f;
    neLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    neuromodulationProfile_ = {0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f};
    workingMemoryBuffer_.clear();
    episodicMemory_.clear();
    plasticityBoost_ = 0.0f;
    developmentSpeedMultiplier_ = 1.0f;
    profileName_ = "";
    config_ = nullptr;
    performanceMonitoringInterval_ = 1.0f;
    performanceStats_ = {0.0f, 0.0f, 0.0f, {}};
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// ========== ENHANCED MOTOR CONTROL ==========

MotorCommand AgentBrain::selectMotorCommandBasedOnGoals(const std::vector<Goal>& goals) {
    if (!brain_) return MotorCommand::Wait;
    
    if (goals.empty()) {
        return decodeMotorCommand();
    }
    
    // Find goal with highest priority
    Goal bestGoal = goals[0];
    for (const auto& goal : goals) {
        if (goal.priority > bestGoal.priority) {
            bestGoal = goal;
        }
    }
    
    // Select action based on goal type
    switch (bestGoal.type) {
        case Goal::Navigation:
            if (!bestGoal.targetPosition.empty()) {
                // Check direction relative to target
                // For simplicity, just move forward
                return MotorCommand::MoveForward;
            }
            break;
        case Goal::Interaction:
            return MotorCommand::Interact;
        case Goal::Exploration:
            // Use curiosity to decide exploration
            if (curiosityEnabled_ && curiosityLevel_ > 0.7f) {
                return selectWithCuriosity(MotorCommand::MoveForward);
            }
            break;
        case Goal::Survival:
            // Implement survival-oriented behavior
            // For now, check if prediction error is high (unexpected situation)
            if (std::abs(predictionError_) > 0.5f) {
                return MotorCommand::Wait; // Default survival response
            }
            break;
    }
    
    return decodeMotorCommand();
}

AgentBrain::ActionPlan AgentBrain::planMultiActionSequence(const std::vector<float>& state, int sequenceLength) {
    ActionPlan plan;
    plan.confidence = 0.5f;
    plan.executionTime = static_cast<float>(sequenceLength) * 0.1f;
    plan.resourceCost = static_cast<float>(sequenceLength) * 0.2f;
    
    // Simple sequence planning based on current state
    if (!brain_) {
        plan.primaryAction = MotorCommand::Wait;
        return plan;
    }
    
    // Calculate motor command confidences
    std::vector<float> confidences = getMotorCommandConfidences();
    
    // Select primary action with highest confidence
    float maxConfidence = 0.0f;
    size_t bestAction = 0;
    for (size_t i = 0; i < confidences.size(); ++i) {
        if (confidences[i] > maxConfidence) {
            maxConfidence = confidences[i];
            bestAction = i;
        }
    }
    
    // Convert index to action
    MotorCommand actions[] = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::Interact, MotorCommand::Wait,
        MotorCommand::LookLeft, MotorCommand::LookRight
    };
    
    plan.primaryAction = actions[bestAction];
    plan.confidence = maxConfidence;
    
    // Generate alternative actions
    for (int i = 1; i < std::min(3, (int)confidences.size()); ++i) {
        size_t altAction = (bestAction + i) % sizeof(actions) / sizeof(actions[0]);
        plan.alternativeActions.push_back(actions[altAction]);
    }
    
    return plan;
}

MotorCommand AgentBrain::selectMotorCommandBasedOnMemory(const std::vector<float>& state) {
    if (!brain_) return MotorCommand::Wait;
    
    // Use working memory to influence action selection
    MotorCommand currentAction = decodeMotorCommand();
    
    // Check episodic memory for similar states
    MemoryContext memory = retrieveEpisodicMemory(0.6f);
    
    if (memory.reward > 0.5f) {
        // High reward memory - reinforce current action
        return currentAction;
    } else if (memory.reward < -0.5f) {
        // Low reward memory - try different action
        if (curiosityLevel_ > 0.7f) {
            return selectWithCuriosity(currentAction);
        }
    }
    
    return currentAction;
}

std::vector<float> AgentBrain::getMotorCommandConfidences() const {
    std::vector<float> confidences;
    
    // Calculate confidence based on neural activity
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
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
    
    // Normalize activities to confidence scores
    float maxAct = std::max({forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct});
    if (maxAct > 0.0f) {
        confidences.push_back(forwardAct / maxAct);
        confidences.push_back(backwardAct / maxAct);
        confidences.push_back(leftAct / maxAct);
        confidences.push_back(rightAct / maxAct);
        confidences.push_back(interactAct / maxAct);
        confidences.push_back(waitAct / maxAct);
    } else {
        confidences = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    }
    
    return confidences;
}

AgentBrain::ActionPlan AgentBrain::getCurrentActionPlan() const {
    ActionPlan plan;
    // In a real implementation, this would return the stored action plan
    // For now, return an empty plan
    return plan;
}

void AgentBrain::updateActionPlan(const SensoryPercept& percept) {
    // Update the action plan with new sensory input
    // This would typically involve re-planning based on updated perception
    // For now, it's a placeholder
}

// ========== ADVANCED NEUROMODULATION ==========

float AgentBrain::getAChLevel() const {
    // In a real implementation, this would read from the acetylcholine neuromodulator
    // For now, return a computed value based on attention needs
    float attentionNeeds = 0.5f;
    if (!previousVision_.empty()) {
        // Higher novelty requires more attention
        attentionNeeds = noveltyLevel_ * 0.5f + 0.5f;
    }
    return std::clamp(attentionNeeds, 0.0f, 1.0f);
}

float AgentBrain::getNELevel() const {
    // In a real implementation, this would read from the norepinephrine neuromodulator
    // For now, return a computed value based on arousal needs
    float arousalNeeds = 0.1f;
    if (curiosityLevel_ > 0.7f) {
        arousalNeeds = 0.8f;
    }
    return std::clamp(arousalNeeds, 0.0f, 1.0f);
}

float AgentBrain::get5HTLevel() const {
    // In a real implementation, this would read from the serotonin neuromodulator
    // For now, return a computed value based on mood/state
    float mood = 0.5f;
    if (predictionError_ > 0.3f) {
        mood = 0.3f; // Negative prediction error = lower mood
    }
    return std::clamp(mood, 0.0f, 1.0f);
}

void AgentBrain::setAChLevel(float level) {
    // In a real implementation, this would set the acetylcholine neuromodulator level
    // For now, store it for use in attention control
    achLevel_ = level;
}

void AgentBrain::setNELevel(float level) {
    // In a real implementation, this would set the norepinephrine neuromodulator level
    // For now, store it for use in arousal control
    neLevel_ = level;
}

void AgentBrain::set5HTLevel(float level) {
    // In a real implementation, this would set the serotonin neuromodulator level
    // For now, store it for use in mood/state control
    serotoninLevel_ = level;
}

void AgentBrain::setNeuromodulationProfile(const NeuromodulationProfile& profile) {
    neuromodulationProfile_ = profile;
}

AgentBrain::NeuromodulationProfile AgentBrain::getNeuromodulationProfile() const {
    return neuromodulationProfile_;
}

void AgentBrain::modulateAttention(float novelty, float predictionError, float reward) {
    // Adjust neuromodulator levels based on context
    setAChLevel(std::clamp(novelty * neuromodulationProfile_.noveltyAmplifier, 0.0f, 1.0f));
    setNELevel(std::clamp(reward * neuromodulationProfile_.rewardSensitivity, 0.0f, 1.0f));
    set5HTLevel(std::clamp(predictionError * neuromodulationProfile_.predictionErrorAmplifier, 0.0f, 1.0f));
}

// ========== MEMORY INTEGRATION ==========

const std::vector<float>* AgentBrain::getWorkingMemoryBuffer() const {
    return &workingMemoryBuffer_;
}

bool AgentBrain::hasWorkingMemoryEntry(NeuronId neuron) const {
    return workingMemoryBuffer_.find(neuron) != workingMemoryBuffer_.end();
}

float AgentBrain::getWorkingMemoryEntry(NeuronId neuron) const {
    auto it = workingMemoryBuffer_.find(neuron);
    if (it != workingMemoryBuffer_.end()) {
        return it->second;
    }
    return 0.0f;
}

void AgentBrain::storeInWorkingMemory(NeuronId neuron, float value) {
    workingMemoryBuffer_[neuron] = value;
}

AgentBrain::MemoryContext AgentBrain::retrieveEpisodicMemory(float similarityThreshold) const {
    MemoryContext context;
    // In a real implementation, this would search episodic memory
    // For now, return empty context
    return context;
}

std::vector<AgentBrain::MemoryContext> AgentBrain::retrieveMemorySequence(size_t count) const {
    std::vector<MemoryContext> contexts;
    // In a real implementation, this would retrieve recent episodes
    // For now, return empty list
    return contexts;
}

MotorCommand AgentBrain::selectActionBasedOnEpisodicMemory(const std::vector<float>& currentState) const {
    // Select action based on similar past experiences
    return MotorCommand::Wait;
}

float AgentBrain::getMemoryBasedActionConfidence(const std::vector<float>& state) const {
    // Calculate confidence based on memory similarity
    return 0.5f;
}

void AgentBrain::clearWorkingMemory() {
    workingMemoryBuffer_.clear();
}

void AgentBrain::clearEpisodicMemory() {
    episodicMemory_.clear();
}

// ========== PREDICTION SYSTEM ==========

float AgentBrain::getPredictionConfidence() const {
    if (!brain_) return 0.0f;
    
    // Get prediction system from brain if available
    PredictionSystem* predSys = brain_->getPredictionSystem();
    if (predSys) {
        return predSys->getConfidence();
    }
    
    // Fallback based on prediction error
    return 1.0f - std::abs(predictionError_);
}

const std::vector<float>* AgentBrain::getPredictionErrorHistory() const {
    if (!brain_) return nullptr;
    
    PredictionSystem* predSys = brain_->getPredictionSystem();
    if (predSys) {
        return &predSys->getErrorHistory();
    }
    
    return nullptr;
}

AgentBrain::PredictionErrorVisualization AgentBrain::visualizePredictionError() const {
    PredictionErrorVisualization viz;
    viz.visualizationType = "heatmap";
    
    // Create a simple 2D error map (placeholder)
    viz.errorMap.resize(16 * 16, 0.0f);
    viz.maxError = std::abs(predictionError_);
    viz.minError = 0.0f;
    
    return viz;
}

AgentBrain::PredictionMetrics AgentBrain::getPredictionMetrics() const {
    PredictionMetrics metrics;
    
    // Default metrics (placeholder)
    metrics.accuracy = 0.75f;
    metrics.precision = 0.8f;
    metrics.recall = 0.7f;
    metrics.f1Score = 0.75f;
    
    // Adjust based on prediction error
    metrics.accuracy = std::clamp(1.0f - std::abs(predictionError_), 0.0f, 1.0f);
    
    return metrics;
}

void AgentBrain::trainPredictionSystem(const SensoryInput& observation) {
    if (!brain_) return;
    
    PredictionSystem* predSys = brain_->getPredictionSystem();
    if (predSys) {
        predSys->train(observation);
    }
}

// ========== DEVELOPMENT CONTROLS ==========

void AgentBrain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (brain_) {
        brain_->setDevelopmentalStage(stage);
    }
}

void AgentBrain::advanceDevelopmentToStage(DevelopmentalStage targetStage) {
    if (!brain_) return;
    
    DevelopmentalStage current = brain_->getDevelopmentalStage();
    
    // Advance stages in order
    if (current < targetStage) {
        while (current < targetStage && current != DevelopmentalStage::Count) {
            advanceDevelopmentStage();
            current = brain_->getDevelopmentalStage();
        }
    } else {
        // Go backwards (in reverse order)
        while (current > targetStage && current != DevelopmentalStage::Initial) {
            setDevelopmentalStage(static_cast<DevelopmentalStage>(static_cast<int>(current) - 1));
            current = brain_->getDevelopmentalStage();
        }
    }
}

std::vector<DevelopmentalStage> AgentBrain::getDevelopmentalTrajectory() const {
    std::vector<DevelopmentalStage> trajectory;
    if (brain_) {
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        for (int i = 0; i < static_cast<int>(DevelopmentalStage::Count); ++i) {
            trajectory.push_back(static_cast<DevelopmentalStage>(i));
        }
    }
    return trajectory;
}

void AgentBrain::setCustomDevelopmentalTrajectory(const DevelopmentalTrajectory& trajectory) {
    customDevelopmentalTrajectory_ = trajectory;
}

void AgentBrain::applyDevelopmentalIntervention(const std::string& interventionType, float intensity) {
    if (!brain_) return;
    
    DevelopmentSystem* devSys = brain_->getDevelopmentSystem();
    if (!devSys) return;
    
    if (interventionType == "plasticity_boost") {
        // Boost plasticity temporarily
        plasticityBoost_ = intensity;
    } else if (interventionType == "accelerated_growth") {
        // Accelerate development
        developmentSpeedMultiplier_ = 1.0f + intensity;
    } else if (interventionType == "reset_plasticity") {
        // Reset plasticity
        plasticityBoost_ = 0.0f;
    }
}

void AgentBrain::resetDevelopmentalStage() {
    if (brain_) {
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    }
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    developmentSpeedMultiplier_ = 1.0f;
    plasticityBoost_ = 0.0f;
}

// ========== CONFIGURATION API ==========

template<typename T>
void AgentBrain::setAdvancedParameter(const std::string& category, const std::string& name, const T& value) {
    std::string key = category + "." + name;
    ConfigValue configValue;
    
    if constexpr (std::is_same_v<T, int>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = value;
    }
    
    config_->set(key, configValue);
}

template<typename T>
T AgentBrain::getAdvancedParameter(const std::string& category, const std::string& name) const {
    std::string key = category + "." + name;
    auto value = config_->get<T>(key);
    
    if (value.has_value()) {
        return value.value();
    }
    
    // Return default value
    if constexpr (std::is_same_v<T, int>) return 0;
    else if constexpr (std::is_same_v<T, double>) return 0.0;
    else if constexpr (std::is_same_v<T, bool>) return false;
    else if constexpr (std::is_same_v<T, std::string>) return "";
    else if constexpr (std::is_same_v<T, std::vector<int>>) return std::vector<int>{};
    else if constexpr (std::is_same_v<T, std::vector<double>>) return std::vector<double>{};
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::vector<std::string>{};
    
    // Unreachable but keeps compiler happy
    static_assert(!std::is_same_v<T, T>, "Unsupported type");
}

void AgentBrain::loadProfile(const std::string& profileName) {
    // Load configuration profile
    // In a real implementation, this would load from a file or database
    profileName_; // Store for reference
}

void AgentBrain::saveProfile(const std::string& profileName) const {
    // Save current configuration as a profile
    // In a real implementation, this would save to a file or database
    profileName_; // Store for reference
}

std::vector<std::string> AgentBrain::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    // In a real implementation, this would list available profiles
    return profiles;
}

void AgentBrain::updateConfiguration(const std::string& key, const ConfigValue& value) {
    if (config_) {
        config_->set(key, value);
    }
}

std::vector<ConfigEntry> AgentBrain::getConfigurationSnapshot() const {
    std::vector<ConfigEntry> snapshot;
    if (config_) {
        snapshot = config_->getKeys();
        // In a real implementation, this would also get values
    }
    return snapshot;
}

void AgentBrain::resetToDefaults() {
    if (config_) {
        config_->clear();
        // In a real implementation, would reinitialize with defaults
    }
}

// ========== PERFORMANCE MONITORING ==========

AgentBrain::PerformanceMetrics AgentBrain::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Basic metrics (placeholders)
    metrics.motorCommandLatency = 0.005f; // 5ms
    metrics.sensoryProcessingTime = 0.003f; // 3ms
    metrics.predictionUpdateTime = 0.002f; // 2ms
    metrics.memoryAccessTime = 0.001f; // 1ms
    metrics.neuromodulationTime = 0.001f; // 1ms
    
    // Calculate derived metrics
    metrics.memoryUtilization = workingMemoryBuffer_.size() / 100.0f; // Assuming capacity 100
    metrics.predictionAccuracy = 1.0f - std::abs(predictionError_);
    metrics.explorationRatio = curiosityEnabled_ ? curiosityLevel_ : 0.0f;
    
    return metrics;
}

AgentBrain::BottleneckInfo AgentBrain::detectBottleneck() const {
    BottleneckInfo bottleneck;
    bottleneck.threshold = 0.7f;
    
    // Simple bottleneck detection based on performance metrics
    PerformanceMetrics metrics = getPerformanceMetrics();
    
    // Find slowest component
    float maxTime = std::max({metrics.motorCommandLatency, 
                              metrics.sensoryProcessingTime,
                              metrics.predictionUpdateTime,
                              metrics.memoryAccessTime,
                              metrics.neuromodulationTime});
    
    if (maxTime == metrics.motorCommandLatency) {
        bottleneck.component = "Motor Command Selection";
    } else if (maxTime == metrics.sensoryProcessingTime) {
        bottleneck.component = "Sensory Processing";
    } else if (maxTime == metrics.predictionUpdateTime) {
        bottleneck.component = "Prediction Update";
    } else if (maxTime == metrics.memoryAccessTime) {
        bottleneck.component = "Memory Access";
    } else {
        bottleneck.component = "Neuromodulation";
    }
    
    bottleneck.currentLoad = maxTime / 0.01f; // Normalized to 0-1 scale
    bottleneck.suggestedImprovement = 0.3f; // 30% improvement possible
    
    return bottleneck;
}

std::vector<std::string> AgentBrain::getPerformanceSuggestions() const {
    std::vector<std::string> suggestions;
    
    BottleneckInfo bottleneck = detectBottleneck();
    
    if (bottleneck.currentLoad > bottleneck.threshold) {
        suggestions.push_back("Optimize " + bottleneck.component + " - current load: " + 
                            std::to_string(bottleneck.currentLoad * 100) + "%");
    }
    
    PerformanceMetrics metrics = getPerformanceMetrics();
    
    if (metrics.predictionAccuracy < 0.8f) {
        suggestions.push_back("Improve prediction accuracy through additional training data");
    }
    
    if (metrics.explorationRatio > 0.8f) {
        suggestions.push_back("Consider reducing exploration to focus on exploitation");
    }
    
    if (metrics.memoryUtilization > 0.9f) {
        suggestions.push_back("Consider increasing memory capacity or implementing better memory management");
    }
    
    return suggestions;
}

void AgentBrain::setPerformanceMonitoringInterval(float interval) {
    performanceMonitoringInterval_ = interval;
}

AgentBrain::PerformanceStatistics AgentBrain::getPerformanceStatistics() const {
    PerformanceStatistics stats;
    
    // Default statistics (placeholder)
    stats.averageLatency = 0.004f; // 4ms average
    stats.peakLatency = 0.01f; // 10ms peak
    stats.averageAccuracy = 0.85f;
    stats.history = {0.9f, 0.85f, 0.87f, 0.88f, 0.86f}; // Recent history
    
    return stats;
}

} // namespace nlm
