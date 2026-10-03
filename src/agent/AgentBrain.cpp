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
    , visionInputScale_(1.0f)          // New: configurable vision input scaling
    , touchInputScale_(1.0f)           // New: configurable touch input scaling
    , internalInputScale_(1.0f)        // New: configurable internal input scaling
    , proprioceptionInputScale_(1.0f)  // New: configurable proprioception input scaling
    , noveltyDecayRate_(0.99f)          // New: configurable novelty decay
    , curiosityThreshold_(0.5f)        // New: curiosity threshold for exploration
    , explorationFactor_(1.0f)         // New: exploration factor multiplier
    , motorCommandThreshold_(0.5f)     // New: motor command activity threshold
    , behaviorMode_(BehaviorMode::Reactive)  // New: default behavior mode
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
            // Inject current proportional to vision intensity
            float current = vision[i] * visionInputScale_;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * touchInputScale_;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * internalInputScale_;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * proprioInputScale_;
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
        
        // Normalize with improved algorithm
        size_t effectiveSize = std::max<size_t>(vision.size(), previousVision_.size());
        noveltyLevel_ = totalDiff / static_cast<float>(effectiveSize);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error (advanced formula)
    if (curiosityEnabled_) {
        // Sophisticated curiosity model: novelty-weighted exploration plus prediction error
        float noveltyWeight = 1.5f;  // Can be made configurable
        float predictionWeight = 0.3f;  // Can be made configurable
        
        curiosityLevel_ = noveltyLevel_ * noveltyWeight + 
                         std::abs(predictionError_) * predictionWeight;
        
        // Apply sigmoid-like saturation for more gradual learning
        curiosityLevel_ = 1.0f / (1.0f + std::exp(-curiosityLevel_ * 5.0f + 2.5f));
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
            // Random motor command - generate from all available actions
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);  // 0-7 inclusive
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

MotorCommand AgentBrain::decodeMotorCommand(MotorCommandSelectionStrategy strategy) {
    switch (strategy) {
        case MotorCommandSelectionStrategy::Conservative:
            return decodeFromMotorNeurons();
        case MotorCommandSelectionStrategy::Balanced:
            MotorCommand cmd = decodeFromMotorNeurons();
            if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
                cmd = selectWithCuriosity(cmd);
            }
            return cmd;
        case MotorCommandSelectionStrategy::Aggressive:
            MotorCommand cmd = decodeFromMotorNeurons();
            if (curiosityEnabled_) {
                cmd = selectWithCuriosity(cmd);
            }
            return cmd;
        case MotorCommandSelectionStrategy::Adaptive:
            return selectWithAdaptiveExploration(MotorCommand::Wait, motorCommandThreshold_ * explorationFactor_);
        case MotorCommandSelectionStrategy::Statistical:
            return selectWithStatisticalAnalysis(MotorCommand::Wait, motorCommandThreshold_);
        default:
            return decodeFromMotorNeurons();
    }
}

MotorCommand AgentBrain::decodeMotorCommandWithExploration(MotorCommand defaultCmd, float explorationRate) {
    MotorCommand cmd = decodeFromMotorNeurons();
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        float exploreChance = std::min(explorationRate, curiosityLevel_);
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);  // 0-7 inclusive
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
                default: return defaultCmd;
            }
        }
    }
    return cmd;
}

MotorCommand AgentBrain::decodeMotorCommandWithThreshold(MotorCommand defaultCmd, float activityThreshold) {
    MotorCommand cmd = decodeFromMotorNeurons();
    float currentActivity = 0.0f;
    
    // Calculate current activity level
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    currentActivity = calcActivity(motorForward_) + calcActivity(motorBackward_) +
                     calcActivity(motorTurnLeft_) + calcActivity(motorTurnRight_) +
                     calcActivity(motorInteract_) + calcActivity(motorWait_);
    
    // Only return command if activity exceeds threshold
    if (currentActivity < activityThreshold) {
        return MotorCommand::Wait;
    }
    
    return cmd;
}

MotorCommand AgentBrain::selectWithAdaptiveExploration(MotorCommand defaultCmd, float dynamicThreshold) {
    // Advanced exploration strategy that adapts based on environment and internal state
    float explorationScore = curiosityLevel_ * explorationFactor_;
    
    // Consider novelty as an additional exploration driver
    explorationScore += noveltyLevel_ * 0.5f;
    
    // Consider prediction error - high prediction error encourages exploration
    explorationScore += std::abs(predictionError_) * 0.3f;
    
    // Cap exploration score
    explorationScore = std::min(explorationScore, 1.0f);
    
    // Decide on action based on exploration score
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    if (r < explorationScore) {
        // Exploration: choose random action with some bias towards novel actions
        int choice = brain_->getRandomGenerator()->uniformInt(0, 8);  // 0-7 inclusive
        
        // Occasionally prefer less common actions when exploration is high
        if (explorationScore > 0.7f && behaviorMode_ == BehaviorMode::Autonomous) {
            // Bias towards exploration actions
            int explorationChoice = brain_->getRandomGenerator()->uniformInt(4, 8);  // Look, LookRight, Interact, Wait
            choice = explorationChoice;
        }
        
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            case 7: return MotorCommand::Wait;
            default: return defaultCmd;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithStatisticalAnalysis(MotorCommand defaultCmd, float confidenceThreshold) {
    // Statistical approach to motor command selection
    // Analyze the distribution of neural activities and choose with statistical confidence
    
    // Calculate activities with more detailed analysis
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Calculate statistical measures
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    float meanActivity = 0.0f;
    float variance = 0.0f;
    
    for (float act : activities) {
        meanActivity += act;
    }
    meanActivity /= activities.size();
    
    for (float act : activities) {
        variance += (act - meanActivity) * (act - meanActivity);
    }
    variance /= activities.size();
    float stdDev = std::sqrt(variance);
    
    // Statistical decision making
    if (stdDev < confidenceThreshold) {
        // Low confidence - be conservative
        return MotorCommand::Wait;
    }
    
    // High confidence - select based on signal-to-noise ratio
    float bestSignal = 0.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    for (const auto& c : commands) {
        float signalToNoise = c.activity / (stdDev + 0.001f);  // Avoid division by zero
        if (signalToNoise > bestSignal) {
            bestSignal = signalToNoise;
            bestCommand = c.cmd;
        }
    }
    
    return bestCommand;
}

void AgentBrain::saveState(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving state: " + filepath);
        return;
    }
    
    try {
        // Save important state variables
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(predictionError_));
        file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(expectedReward_));
        file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        file.write(reinterpret_cast<const char*>(&visionInputScale_), sizeof(visionInputScale_));
        file.write(reinterpret_cast<const char*>(&touchInputScale_), sizeof(touchInputScale_));
        file.write(reinterpret_cast<const char*>(&internalInputScale_), sizeof(internalInputScale_));
        file.write(reinterpret_cast<const char*>(&proprioceptionInputScale_), sizeof(proprioceptionInputScale_));
        
        file.write(reinterpret_cast<const char*>(&curiosityThreshold_), sizeof(curiosityThreshold_));
        file.write(reinterpret_cast<const char*>(&explorationFactor_), sizeof(explorationFactor_));
        file.write(reinterpret_cast<const char*>(&motorCommandThreshold_), sizeof(motorCommandThreshold_));
        file.write(reinterpret_cast<const char*>(&behaviorMode_), sizeof(behaviorMode_));
        
        // Save neural group sizes
        size_t visionCount = sensoryVision_.size();
        size_t touchCount = sensoryTouch_.size();
        size_t internalCount = sensoryInternal_.size();
        size_t proprioCount = sensoryProprioception_.size();
        
        file.write(reinterpret_cast<const char*>(&visionCount), sizeof(visionCount));
        file.write(reinterpret_cast<const char*>(&touchCount), sizeof(touchCount));
        file.write(reinterpret_cast<const char*>(&internalCount), sizeof(internalCount));
        file.write(reinterpret_cast<const char*>(&proprioCount), sizeof(proprioCount));
        
        // Save previous vision vector
        size_t prevVisionSize = previousVision_.size();
        file.write(reinterpret_cast<const char*>(&prevVisionSize), sizeof(prevVisionSize));
        if (prevVisionSize > 0) {
            file.write(reinterpret_cast<const char*>(previousVision_.data()), prevVisionSize * sizeof(float));
        }
        
        NLM_LOG_INFO("AgentBrain state saved to: " + filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving AgentBrain state: " + std::string(e.what()));
    }
    
    file.close();
}

void AgentBrain::loadState(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for loading state: " + filepath);
        return;
    }
    
    try {
        // Load important state variables
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(dopamineLevel_));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(noveltyLevel_));
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(curiosityLevel_));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(predictionError_));
        file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(expectedReward_));
        file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(developmentalAge_));
        file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(plasticityModifier_));
        
        file.read(reinterpret_cast<char*>(&visionInputScale_), sizeof(visionInputScale_));
        file.read(reinterpret_cast<char*>(&touchInputScale_), sizeof(touchInputScale_));
        file.read(reinterpret_cast<char*>(&internalInputScale_), sizeof(internalInputScale_));
        file.read(reinterpret_cast<char*>(&proprioceptionInputScale_), sizeof(proprioceptionInputScale_));
        
        file.read(reinterpret_cast<char*>(&curiosityThreshold_), sizeof(curiosityThreshold_));
        file.read(reinterpret_cast<char*>(&explorationFactor_), sizeof(explorationFactor_));
        file.read(reinterpret_cast<char*>(&motorCommandThreshold_), sizeof(motorCommandThreshold_));
        file.read(reinterpret_cast<char*>(&behaviorMode_), sizeof(behaviorMode_));
        
        // Load neural group sizes
        size_t visionCount, touchCount, internalCount, proprioCount;
        file.read(reinterpret_cast<char*>(&visionCount), sizeof(visionCount));
        file.read(reinterpret_cast<char*>(&touchCount), sizeof(touchCount));
        file.read(reinterpret_cast<char*>(&internalCount), sizeof(internalCount));
        file.read(reinterpret_cast<char*>(&proprioCount), sizeof(proprioCount));
        
        // For loading, we'll just update the sizes (actual neurons remain the same)
        // In a real implementation, you might want to copy more state
        
        // Load previous vision vector
        size_t prevVisionSize;
        file.read(reinterpret_cast<char*>(&prevVisionSize), sizeof(prevVisionSize));
        if (prevVisionSize > 0 && prevVisionSize <= previousVision_.size()) {
            file.read(reinterpret_cast<char*>(previousVision_.data()), prevVisionSize * sizeof(float));
        }
        
        NLM_LOG_INFO("AgentBrain state loaded from: " + filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading AgentBrain state: " + std::string(e.what()));
    }
    
    file.close();
}

bool AgentBrain::validateState() const {
    // Validate internal state consistency
    bool isValid = true;
    
    if (!brain_) {
        NLM_LOG_ERROR("Invalid state: brain_ pointer is null");
        isValid = false;
    }
    
    // Check for NaN or infinite values
    if (std::isnan(dopamineLevel_) || std::isinf(dopamineLevel_)) {
        NLM_LOG_ERROR("Invalid state: dopamineLevel_ has invalid value");
        isValid = false;
    }
    
    if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
        NLM_LOG_ERROR("Invalid state: curiosityLevel_ out of bounds");
        isValid = false;
    }
    
    if (noveltyLevel_ < 0.0f || noveltyLevel_ > 1.0f) {
        NLM_LOG_ERROR("Invalid state: noveltyLevel_ out of bounds");
        isValid = false;
    }
    
    if (developmentalAge_ < 0.0) {
        NLM_LOG_ERROR("Invalid state: developmentalAge_ negative");
        isValid = false;
    }
    
    if (plasticityModifier_ < 0.0f || plasticityModifier_ > 2.0f) {
        NLM_LOG_ERROR("Invalid state: plasticityModifier_ out of bounds");
        isValid = false;
    }
    
    // Check for consistency in neuron group sizes
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size();
    
    if (totalMotorNeurons == 0) {
        NLM_LOG_WARNING("Warning: No motor neurons allocated");
    }
    
    return isValid;
}

void AgentBrain::emergencyReset() {
    NLM_LOG_WARNING("Emergency reset triggered for AgentBrain");
    
    // Reset all state variables
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset configuration to safe defaults
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
    
    visionInputScale_ = 1.0f;
    touchInputScale_ = 1.0f;
    internalInputScale_ = 1.0f;
    proprioceptionInputScale_ = 1.0f;
    
    curiosityThreshold_ = 0.5f;
    explorationFactor_ = 1.0f;
    motorCommandThreshold_ = 0.5f;
    behaviorMode_ = BehaviorMode::Reactive;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain emergency reset completed");
}

} // namespace nlm
