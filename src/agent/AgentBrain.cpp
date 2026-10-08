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

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
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
    , neuromodulationSensitivity_(1.0f)
    , curiosityThreshold_(0.3f)
    , developmentalRate_(1.0f)
    , episodeActive_(false)
    , episodeStartTime_(0.0)
    , currentEpisodeDuration_(0.0)
    , episodeTotalReward_(0.0f)
    , episodeActionCount_(0)
    , episodeExplorationRate_(0.0f)
    , episodeLearningRate_(0.0f)
    , fatigueLevel_(0.0f)
    , recoveryRate_(0.1f)
    , lastRestTime_(0.0)
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
    episodeActive_ = false;
    currentEpisodeDuration_ = 0.0;
    episodeTotalReward_ = 0.0f;
    episodeActionCount_ = 0;
    episodeExplorationRate_ = 0.0f;
    episodeLearningRate_ = 0.0f;
    fatigueLevel_ = 0.0f;
    lastRestTime_ = 0.0;
    
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
    
    lastPercept_ = percept;
    lastUpdateTime_ = 0.0;  // Would be current time in real usage
    
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
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
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

// ========== CONVENIENCE METHODS ==========

void AgentBrain::takeAction(const SensoryPercept& percept, MotorCommand& chosenAction) {
    // Process sensory input
    processSensoryInput(percept);
    
    // Get motor command
    MotorCommand action = decodeMotorCommand();
    
    // Apply habituation/exploration logic
    action = exploreOrExploit(action, curiosityThreshold_);
    
    // Update episode stats
    episodeActionCount_++;
    if (action != MotorCommand::Wait) {
        episodeExplorationRate_ += 1.0f;
    }
    
    chosenAction = action;
    
    // Apply fatigue based on action
    if (action != MotorCommand::Wait) {
        fatigueLevel_ += 0.1f;
        fatigueLevel_ = std::clamp(fatigueLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::processAndAct(const SensoryPercept& percept) {
    MotorCommand action;
    takeAction(percept, action);
    return action;
}

MotorCommand AgentBrain::exploreOrExploit(MotorCommand bestAction, float threshold) {
    // If curiosity is low, stick with best action
    if (curiosityLevel_ < threshold) {
        return bestAction;
    }
    
    // Otherwise, might choose to explore
    float exploreChance = (curiosityLevel_ - threshold) / (1.0f - threshold);
    exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        // Random exploration
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
    
    return bestAction;
}

std::string AgentBrain::getStatusString() const {
    std::stringstream ss;
    ss << "AgentBrain Status:" << std::endl;
    ss << "  - Neuromodulation level: " << getNeuromodulationLevel() << std::endl;
    ss << "  - Curiosity level: " << getCuriosityLevel() << std::endl;
    ss << "  - Novelty level: " << getNoveltyLevel() << std::endl;
    ss << "  - Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    ss << "  - Reward modulation: " << (isRewardModulationEnabled() ? "enabled" : "disabled") << std::endl;
    ss << "  - Curiosity enabled: " << (isCuriosityEnabled() ? "enabled" : "disabled") << std::endl;
    ss << "  - Development enabled: " << (isDevelopmentEnabled() ? "enabled" : "disabled") << std::endl;
    ss << "  - Fatigue level: " << fatigueLevel_ << std::endl;
    ss << "  - Episode active: " << (isEpisodeActive() ? "yes" : "no") << std::endl;
    if (isEpisodeActive()) {
        ss << "  - Episode duration: " << currentEpisodeDuration_ << "s" << std::endl;
        ss << "  - Episode reward: " << episodeTotalReward_ << std::endl;
    }
    return ss.str();
}

bool AgentBrain::isStable() const {
    // Agent is stable if not actively learning or developing much
    return (curiosityLevel_ < 0.1f && developmentalAge_ > 60.0 && 
            fatigueLevel_ < 0.1f && episodeExplorationRate_ < 0.5f);
}

AgentBrain::AgentCapabilities AgentBrain::getCapabilities() const {
    AgentCapabilities caps;
    caps.rewardLearning = isRewardModulationEnabled();
    caps.curiosityDriven = isCuriosityEnabled();
    caps.developmentalGrowth = isDevelopmentEnabled();
    caps.memoryBased = !previousVision_.empty();  // Has some memory
    caps.plasticityRate = plasticityModifier_;
    return caps;
}

// ========== ADVANCED CONFIGURATION ==========

void AgentBrain::setNeuromodulationSensitivity(float sensitivity) {
    neuromodulationSensitivity_ = std::clamp(sensitivity, 0.0f, 2.0f);
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::setDevelopmentalRate(float rate) {
    developmentalRate_ = std::clamp(rate, 0.1f, 2.0f);
}

void AgentBrain::setCapability(AgentCapabilities::RewardLearning enabled) {
    enableRewardModulation(enabled);
}

void AgentBrain::setCapability(AgentCapabilities::CuriosityDriven enabled) {
    enableCuriosity(enabled);
}

void AgentBrain::setCapability(AgentCapabilities::DevelopmentalGrowth enabled) {
    enableDevelopment(enabled);
}

void AgentBrain::setCapability(AgentCapabilities::MemoryBased enabled) {
    // Can't really disable memory, just clear it
    if (!enabled) {
        previousVision_.clear();
        lastPercept_ = SensoryPercept();
    }
}

void AgentBrain::setCapability(AgentCapabilities::PlasticityRate rate) {
    plasticityModifier_ = std::clamp(rate, 0.1f, 2.0f);
}

float AgentBrain::getNeuromodulationSensitivity() const {
    return neuromodulationSensitivity_;
}

float AgentBrain::getCuriosityThreshold() const {
    return curiosityThreshold_;
}

float AgentBrain::getDevelopmentalRate() const {
    return developmentalRate_;
}

float AgentBrain::getPlasticityRate() const {
    return plasticityModifier_;
}

// ========== EPISODE MANAGEMENT ==========

void AgentBrain::startEpisode(double initialTime) {
    episodeActive_ = true;
    episodeStartTime_ = initialTime;
    currentEpisodeDuration_ = 0.0;
    episodeTotalReward_ = 0.0f;
    episodeActionCount_ = 0;
    episodeExplorationRate_ = 0.0f;
    episodeLearningRate_ = 0.0f;
    
    NLM_LOG_INFO("AgentBrain episode started");
}

void AgentBrain::endEpisode() {
    if (episodeActive_) {
        episodeActive_ = false;
        NLM_LOG_INFO("AgentBrain episode ended")
        NLM_LOG_INFO("  Duration: " + std::to_string(currentEpisodeDuration_) + "s");
        NLM_LOG_INFO("  Total reward: " + std::to_string(episodeTotalReward_));
        NLM_LOG_INFO("  Action count: " + std::to_string(episodeActionCount_));
        NLM_LOG_INFO("  Exploration rate: " + std::to_string(episodeExplorationRate_));
        NLM_LOG_INFO("  Learning rate: " + std::to_string(episodeLearningRate_));
    }
}

bool AgentBrain::isEpisodeActive() const {
    return episodeActive_;
}

AgentBrain::EpisodeStats AgentBrain::getEpisodeStats() const {
    EpisodeStats stats;
    stats.duration = currentEpisodeDuration_;
    stats.totalReward = episodeTotalReward_;
    stats.actionCount = episodeActionCount_;
    stats.explorationRate = episodeExplorationRate_ / std::max<size_t>(episodeActionCount_, 1);
    stats.learningRate = episodeLearningRate_;
    return stats;
}

double AgentBrain::getEpisodeStartTime() const {
    return episodeStartTime_;
}

// ========== STATE MANAGEMENT ==========

bool AgentBrain::saveState(const std::string& filepath) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
            return false;
        }
        
        // Write state to file (simplified serialization)
        file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(double));
        file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&fatigueLevel_), sizeof(float));
        file.write(reinterpret_cast<const char*>(&episodeActive_), sizeof(bool));
        file.write(reinterpret_cast<const char*>(&episodeStartTime_), sizeof(double));
        file.write(reinterpret_cast<const char*>(&currentEpisodeDuration_), sizeof(double));
        
        if (!file) {
            NLM_LOG_ERROR("Failed to write state to file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("AgentBrain state saved to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving agent state: ") + e.what());
        return false;
    }
}

bool AgentBrain::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to open file for loading: " + filepath);
            return false;
        }
        
        // Read state from file (simplified deserialization)
        file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&predictionError_), sizeof(float));
        file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(float));
        file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(double));
        file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(float));
        file.read(reinterpret_cast<char*>(&fatigueLevel_), sizeof(float));
        file.read(reinterpret_cast<char*>(&episodeActive_), sizeof(bool));
        file.read(reinterpret_cast<char*>(&episodeStartTime_), sizeof(double));
        file.read(reinterpret_cast<char*>(&currentEpisodeDuration_), sizeof(double));
        
        if (!file) {
            NLM_LOG_ERROR("Failed to read state from file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("AgentBrain state loaded from " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading agent state: ") + e.what());
        return false;
    }
}

bool AgentBrain::createBackup(const std::string& backupPath) const {
    return saveState(backupPath);
}

bool AgentBrain::restoreFromBackup(const std::string& backupPath) {
    return loadState(backupPath);
}

void AgentBrain::resetToDefaults() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    fatigueLevel_ = 0.0f;
    
    episodeActive_ = false;
    episodeStartTime_ = 0.0;
    currentEpisodeDuration_ = 0.0;
    episodeTotalReward_ = 0.0f;
    episodeActionCount_ = 0;
    episodeExplorationRate_ = 0.0f;
    episodeLearningRate_ = 0.0f;
    
    lastPercept_ = SensoryPercept();
    lastUpdateTime_ = 0.0;
    
    NLM_LOG_INFO("AgentBrain state reset to defaults");
}

// ========== MONITORING AND DEBUGGING ==========

size_t AgentBrain::getSpikeCount() const {
    if (brain_) {
        return brain_->getTotalSpikeCount();
    }
    return 0;
}

size_t AgentBrain::getActiveNeuronCount() const {
    if (brain_) {
        return brain_->getActiveNeuronCount();
    }
    return 0;
}

AgentBrain::ActivityStats AgentBrain::getActivityStats() const {
    ActivityStats stats;
    stats.averageFiringRate = (brain_ ? brain_->getAverageFiringRate() : 0.0f);
    stats.peakActivity = 1.0f;  // Simplified
    stats.timeStepsActive = 0;  // Simplified
    stats.recentActivity = recentActivity_;  // Already stored
    return stats;
}

bool AgentBrain::hasSensoryInput() const {
    return !lastPercept_.getVision().empty() || 
           !lastPercept_.getTouch().empty() ||
           !lastPercept_.getInternal().empty();
}

SensoryPercept AgentBrain::getLastSensoryPercept() const {
    return lastPercept_;
}

// ========== HIGH-LEVEL AGENT BEHAVIORS ==========

void AgentBrain::applyHabituation(const SensoryPercept& percept, float decayRate) {
    // Reduce response to frequent stimuli
    float habituationStrength = decayRate;
    
    // Apply to vision neurons
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        previousVision_[i] *= (1.0f - habituationStrength);
    }
    
    // Store current perception for future comparison
    previousVision_ = std::vector<float>(vision.begin(), vision.end());
}

void AgentBrain::applySensitization(const SensoryPercept& percept, float amplification) {
    // Increase response to novel stimuli
    float sensitizationStrength = amplification;
    
    // Apply to sensory neurons to amplify response
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float novelty = std::abs(vision[i] - previousVision_[i]);
        if (novelty > 0.1f) {  // Consider it novel if difference is significant
            // Amplify current injection
            if (sensoryVision_[i]) {
                sensoryVision_[i]->injectCurrent(vision[i] * sensitizationStrength);
            }
        }
    }
}

void AgentBrain::applyFatigue(float decayRate, float recoveryRate) {
    // Reduce motor output after prolonged activity
    fatigueLevel_ *= (1.0f - recoveryRate);
    
    // Apply fatigue to motor neurons
    float fatigueReduction = decayRate * fatigueLevel_;
    
    // Could reduce motor neuron excitability here
    // For simplicity, just track the fatigue level
}

void AgentBrain::applyRest(double restDuration) {
    // Allow agent to recover and reset state
    float recovery = std::min(1.0f, static_cast<float>(restDuration) * recoveryRate_);
    fatigueLevel_ *= (1.0f - recovery);
    
    lastRestTime_ = 0.0;  // Would be current time
}

void AgentBrain::emergencyReset() {
    // Stop all activity and reset completely
    resetToDefaults();
    
    // Clear all neuron activity
    if (brain_) {
        brain_->reset();
    }
    
    NLM_LOG_INFO("AgentBrain emergency reset performed");
}

void AgentBrain::updateDevelopment(double timestep) {
    // Call the base implementation first
    AgentBrain::updateDevelopment(timestep);
    
    // Update development metrics
    developmentalAge_ += timestep * developmentalRate_;
    
    // Update episode statistics
    currentEpisodeDuration_ += timestep;
    
    // Simple learning rate calculation based on activity
    float activityLevel = (brain_ ? brain_->getAverageFiringRate() : 0.0f);
    episodeLearningRate_ = activityLevel * plasticityModifier_ * 0.01f;
    
    // Update fatigue
    if (fatigueLevel_ > 0.0f) {
        fatigueLevel_ *= 0.99f;  // Natural recovery
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // Call the base implementation
    AgentBrain::applyRewardModulation(reward, predictedReward);
    
    // Update episode reward statistics
    episodeTotalReward_ += reward;
    
    // Apply exploration bonus for novel rewards
    if (reward > 0.0f && noveltyLevel_ > 0.5f) {
        episodeExplorationRate_ += 2.0f;  // Bonus for exploring novel rewards
    }
}

} // namespace nlm
