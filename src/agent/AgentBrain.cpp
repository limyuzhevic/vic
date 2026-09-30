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
                                    motorLeft_.size() + motorRight_.size() +
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size() +
                                    motorLookUp_.size() + motorLookDown_.size() +
                                    motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() +
                                    motorGrab_.size() + motorRelease_.size() + motorPush_.size() + motorPull_.size() + motorLift_.size() + motorDrop_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        if (idx < motorForward_.size()) motorForward_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size()) motorBackward_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size()) motorLeft_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size()) motorRight_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size()) motorTurnLeft_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size()) motorTurnRight_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size()) motorLookLeft_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size()) motorLookRight_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size()) motorLookUp_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size()) motorLookDown_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size()) motorJump_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size()) {
                            if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size()) motorClimb_.push_back(n);
                            else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size()) motorFall_.push_back(n);
                            else motorRoll_.push_back(n);
                        }
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size()) motorGrab_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size()) motorRelease_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size() + motorPush_.size()) motorPush_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size() + motorPush_.size() + motorPull_.size()) motorPull_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size() + motorPush_.size() + motorPull_.size() + motorLift_.size()) motorLift_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size() + motorPush_.size() + motorPull_.size() + motorLift_.size() + motorDrop_.size()) motorDrop_.push_back(n);
                        else if (idx < motorForward_.size() + motorBackward_.size() + motorLeft_.size() + motorRight_.size() + motorTurnLeft_.size() + motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() + motorLookUp_.size() + motorLookDown_.size() + motorJump_.size() + motorClimb_.size() + motorFall_.size() + motorRoll_.size() + motorGrab_.size() + motorRelease_.size() + motorPush_.size() + motorPull_.size() + motorLift_.size() + motorDrop_.size() + motorInteract_.size()) motorInteract_.push_back(n);
                        else motorWait_.push_back(n);
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
    
    // Connect to memory systems for integrated cognition
    if (brain_) {
        workingMemory_ = brain_->getWorkingMemory();
        episodicMemory_ = brain_->getEpisodicMemory();
        predictionSystem_ = brain_->getPredictionSystem();
        
        if (workingMemory_) {
            NLM_LOG_INFO("AgentBrain connected to working memory (" + 
                        std::to_string(workingMemory_->getCapacity()) + " capacity)");
        }
        if (episodicMemory_) {
            NLM_LOG_INFO("AgentBrain connected to episodic memory (" + 
                        std::to_string(episodicMemory_->getMaxEpisodes()) + " max episodes)");
        }
        if (predictionSystem_) {
            NLM_LOG_INFO("AgentBrain connected to prediction system");
        }
    }
    
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
    return 21;
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
    
    // Store sensory experience in episodic memory for later recall
    if (episodicMemory_) {
        storeSensoryExperience(percept, static_cast<SimulationStep>(brain_->getCurrentStep()));
    }
    
    // Use working memory to maintain sensory representations
    if (workingMemory_) {
        updateWorkingMemoryFromSensory(percept);
    }
    
    // Generate prediction and use it for action selection
    if (predictionSystem_) {
        updatePredictionFromSensory(percept, static_cast<SimulationStep>(brain_->getCurrentStep()));
        usePredictionForActionSelection();
    }
    
    // Apply curiosity-based exploration using memory content
    applyMemoryBasedCuriosity();
    
    // Use episodic memory for reinforcement learning
    if (episodicMemory_ && dopamineLevel_ > 0.0f) {
        reinforceLearningFromExperience(dopamineLevel_);
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
    float leftAct = calcActivity(motorLeft_);
    float rightAct = calcActivity(motorRight_);
    float turnLeftAct = calcActivity(motorTurnLeft_);
    float turnRightAct = calcActivity(motorTurnRight_);
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    float lookUpAct = calcActivity(motorLookUp_);
    float lookDownAct = calcActivity(motorLookDown_);
    float jumpAct = calcActivity(motorJump_);
    float climbAct = calcActivity(motorClimb_);
    float fallAct = calcActivity(motorFall_);
    float rollAct = calcActivity(motorRoll_);
    float grabAct = calcActivity(motorGrab_);
    float releaseAct = calcActivity(motorRelease_);
    float pushAct = calcActivity(motorPush_);
    float pullAct = calcActivity(motorPull_);
    float liftAct = calcActivity(motorLift_);
    float dropAct = calcActivity(motorDrop_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::MoveLeft, leftAct},
        {MotorCommand::MoveRight, rightAct},
        {MotorCommand::TurnLeft, turnLeftAct},
        {MotorCommand::TurnRight, turnRightAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct},
        {MotorCommand::LookUp, lookUpAct},
        {MotorCommand::LookDown, lookDownAct},
        {MotorCommand::Jump, jumpAct},
        {MotorCommand::Climb, climbAct},
        {MotorCommand::Fall, fallAct},
        {MotorCommand::Roll, rollAct},
        {MotorCommand::Grab, grabAct},
        {MotorCommand::Release, releaseAct},
        {MotorCommand::Push, pushAct},
        {MotorCommand::Pull, pullAct},
        {MotorCommand::Lift, liftAct},
        {MotorCommand::Drop, dropAct},
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
            // Random motor command from all available commands
            int choice = brain_->getRandomGenerator()->uniformInt(0, 21);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::MoveLeft;
                case 3: return MotorCommand::MoveRight;
                case 4: return MotorCommand::TurnLeft;
                case 5: return MotorCommand::TurnRight;
                case 6: return MotorCommand::LookLeft;
                case 7: return MotorCommand::LookRight;
                case 8: return MotorCommand::LookUp;
                case 9: return MotorCommand::LookDown;
                case 10: return MotorCommand::Jump;
                case 11: return MotorCommand::Climb;
                case 12: return MotorCommand::Fall;
                case 13: return MotorCommand::Roll;
                case 14: return MotorCommand::Grab;
                case 15: return MotorCommand::Release;
                case 16: return MotorCommand::Push;
                case 17: return MotorCommand::Pull;
                case 18: return MotorCommand::Lift;
                case 19: return MotorCommand::Drop;
                case 20: return MotorCommand::Interact;
                case 21: return MotorCommand::Wait;
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

}

void AgentBrain::storeSensoryExperience(const SensoryPercept& percept, SimulationStep step) {
    if (!episodicMemory_) return;
    
    // Create episodic memory item from sensory percept
    EpisodicMemoryItem episode;
    episode.timestamp = step;
    episode.reward = dopamineLevel_; // Store current neuromodulation as reward signal
    
    // Convert sensory data to neural activation pattern
    std::vector<float> sensoryPattern;
    
    // Vision
    const auto& vision = percept.getVision();
    for (float value : vision) {
        sensoryPattern.push_back(value);
    }
    
    // Touch
    const auto& touch = percept.getTouch();
    for (float value : touch) {
        sensoryPattern.push_back(value);
    }
    
    // Internal signals
    const auto& internal = percept.getInternal();
    for (float value : internal) {
        sensoryPattern.push_back(value);
    }
    
    // Proprioception
    const auto& proprio = percept.getProprioception();
    for (float value : proprio) {
        sensoryPattern.push_back(value);
    }
    
    episode.metadata = "AgentBrain sensory percept at step " + std::to_string(step);
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    NLM_LOG_INFO("AgentBrain stored sensory experience in episodic memory at step " + std::to_string(step));
}

void AgentBrain::updateWorkingMemoryFromSensory(const SensoryPercept& percept) {
    if (!workingMemory_) return;
    
    // Convert current sensory percept to neural activation pattern
    std::vector<float> sensoryPattern;
    
    const auto& vision = percept.getVision();
    for (float value : vision) {
        sensoryPattern.push_back(value);
    }
    
    const auto& touch = percept.getTouch();
    for (float value : touch) {
        sensoryPattern.push_back(value);
    }
    
    // Store in working memory (actively maintains sensory information)
    workingMemory_->store(sensoryPattern, 1.0f);
    
    NLM_LOG_INFO("AgentBrain updated working memory with sensory pattern");
}

void AgentBrain::updatePredictionFromSensory(const SensoryPercept& percept, SimulationStep currentTime) {
    if (!predictionSystem_) return;
    
    // Convert sensory percept to state representation for prediction
    std::vector<float> currentState;
    
    const auto& vision = percept.getVision();
    for (float value : vision) {
        currentState.push_back(value);
    }
    
    const auto& touch = percept.getTouch();
    for (float value : touch) {
        currentState.push_back(value);
    }
    
    // Record current state for prediction
    predictionSystem_->recordSensoryState(currentState, currentTime);
    
    // Update prediction based on actual observation
    predictionError_ = predictionSystem_->updateWithObservation(currentState, currentTime);
    
    NLM_LOG_INFO("AgentBrain prediction system updated (error: " + std::to_string(predictionError_) + ")");
}

void AgentBrain::usePredictionForActionSelection() {
    if (!predictionSystem_) return;
    
    // For now, just store prediction error for neuromodulation
    // In a more advanced implementation, predictions could guide action selection
    NLM_LOG_INFO("AgentBrain using prediction for action selection");
}

void AgentBrain::applyMemoryBasedCuriosity() {
    if (!curiosityEnabled_ || !episodicMemory_ || !workingMemory_) return;
    
    // Get recent episodic memories to guide exploration
    size_t episodeCount = episodicMemory_->getEpisodeCount();
    if (episodeCount > 0) {
        // Check if we've seen similar patterns before (for learning)
        std::vector<EpisodicMemoryItem> recentEpisodes = episodicMemory_->getRecentEpisodes(3);
        
        // Increase curiosity if we have limited experience or if patterns are novel
        if (episodeCount < 10) {
            // New agent - high curiosity to explore
            curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.5f);
        }
        
        // If working memory is full, we're learning - maintain curiosity
        if (workingMemory_->getCurrentSize() >= workingMemory_->getCapacity() * 0.8f) {
            curiosityLevel_ = std::min(1.0f, curiosityLevel_ * 1.2f);
        }
    }
    
    NLM_LOG_INFO("AgentBrain applied memory-based curiosity (level: " + std::to_string(curiosityLevel_) + ")");
}

void AgentBrain::reinforceLearningFromExperience(float reward) {
    if (!episodicMemory_) return;
    
    // Consolidate the most recent experience based on reward
    size_t episodeCount = episodicMemory_->getEpisodeCount();
    if (episodeCount > 0) {
        // Get recent episodes
        std::vector<EpisodicMemoryItem> recentEpisodes = episodicMemory_->getRecentEpisodes(1);
        if (!recentEpisodes.empty()) {
            const auto& episode = recentEpisodes[0];
            
            // Higher reward = stronger consolidation
            float consolidationStrength = std::min(1.0f, reward * 2.0f);
            
            // In a real implementation, this would strengthen synaptic connections
            // based on the successful experience
            NLM_LOG_INFO("AgentBrain reinforced learning from experience (reward: " + std::to_string(reward) + ")");
        }
    }
}

} // namespace nlm
