#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Performance monitoring state
    bool performanceMonitoringEnabled_;
    size_t stepsProcessed_;
    double totalTime_;
    float maxFiringRate_;
    float avgFiringRate_;

    // Simulation control
    bool simulationPaused_;
    double simulationSpeedMultiplier_;
    size_t replaySpeedMultiplier_;

    // Debugging and logging
    bool debugLoggingEnabled_;
    size_t actionSequenceMaxLength_;
    std::vector<MotorCommand> actionSequence_;

    // Episode tracking
    size_t currentEpisodeSteps_;
    float episodeReward_;
    float episodeNovelty_;

    AgentBrain(std::shared_ptr<Brain> brain)
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
    , performanceMonitoringEnabled_(false)
    , stepsProcessed_(0)
    , totalTime_(0.0)
    , maxFiringRate_(0.0f)
    , avgFiringRate_(0.0f)
    , simulationPaused_(false)
    , simulationSpeedMultiplier_(1.0)
    , replaySpeedMultiplier_(1)
    , debugLoggingEnabled_(false)
    , actionSequenceMaxLength_(1000)
    , currentEpisodeSteps_(0)
    , episodeReward_(0.0f)
    , episodeNovelty_(0.0f)
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

// Advanced features implementation
void AgentBrain::enablePerformanceMonitoring(bool enable) {
    performanceMonitoringEnabled_ = enable;
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("Performance monitoring " + std::string(enable ? "enabled" : "disabled"));
    }
}

void AgentBrain::setSimulationSpeed(double speed) {
    if (speed <= 0.0) {
        NLM_LOG_ERROR("Simulation speed must be positive");
        return;
    }
    simulationSpeedMultiplier_ = speed;
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("Simulation speed set to " + std::to_string(speed) + "x");
    }
}

void AgentBrain::pauseSimulation() {
    simulationPaused_ = true;
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("Simulation paused");
    }
}

void AgentBrain::resumeSimulation() {
    simulationPaused_ = false;
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("Simulation resumed");
    }
}

bool AgentBrain::isSimulationPaused() const {
    return simulationPaused_;
}

void AgentBrain::enableDebugLogging(bool enable) {
    debugLoggingEnabled_ = enable;
    if (enable) {
        NLM_LOG_INFO("Debug logging enabled");
    } else {
        NLM_LOG_INFO("Debug logging disabled");
    }
}

// Performance monitoring
void AgentBrain::recordStep(double dt) {
    if (!performanceMonitoringEnabled_) return;
    
    stepsProcessed_++;
    totalTime_ += dt;
    
    // Update firing rate statistics
    if (brain_) {
        float firingRate = brain_->getAverageFiringRate();
        avgFiringRate_ = (avgFiringRate_ * (stepsProcessed_ - 1) + firingRate) / stepsProcessed_;
        maxFiringRate_ = std::max(maxFiringRate_, firingRate);
    }
    
    if (debugLoggingEnabled_ && stepsProcessed_ % 100 == 0) {
        NLM_LOG_DEBUG("Step " + std::to_string(stepsProcessed_) + 
                     " | Time: " + std::to_string(totalTime_) + 
                     " | Avg firing: " + std::to_string(avgFiringRate_));
    }
}

PerformanceStats AgentBrain::getPerformanceStats() const {
    PerformanceStats stats;
    
    if (performanceMonitoringEnabled_ && stepsProcessed_ > 0) {
        stats.totalSteps = stepsProcessed_;
        stats.totalTime = totalTime_;
        stats.avgStepsPerSecond = stepsProcessed_ / totalTime_;
        stats.avgFiringRate = avgFiringRate_;
        stats.maxFiringRate = maxFiringRate_;
        stats.episodeSteps = currentEpisodeSteps_;
        stats.episodeReward = episodeReward_;
        stats.episodeNovelty = episodeNovelty_;
    }
    
    return stats;
}

// Action sequence recording
void AgentBrain::recordAction(MotorCommand action) {
    if (actionSequence_.size() >= actionSequenceMaxLength_) {
        actionSequence_.erase(actionSequence_.begin());
    }
    actionSequence_.push_back(action);
}

std::vector<MotorCommand> AgentBrain::getActionSequence() const {
    return actionSequence_;
}

void AgentBrain::setReplaySpeed(size_t speed) {
    if (speed == 0) {
        NLM_LOG_ERROR("Replay speed cannot be zero");
        return;
    }
    replaySpeedMultiplier_ = speed;
}

// Batch processing
void AgentBrain::batchProcessSensoryInput(const std::vector<SensoryPercept>& percepts) {
    size_t successful = 0;
    for (const auto& percept : percepts) {
        if (brain_ && percept.getTimestamp() > 0) {
            processSensoryInput(percept);
            successful++;
        }
    }
    
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("Batch processed " + std::to_string(successful) + "/" + 
                     std::to_string(percepts.size()) + " percepts");
    }
}

// Episode management
void AgentBrain::startEpisode() {
    reset();
    currentEpisodeSteps_ = 0;
    episodeReward_ = 0.0f;
    episodeNovelty_ = 0.0f;
}

size_t AgentBrain::runEpisode(SimpleWorld& world, size_t maxSteps) {
    startEpisode();
    
    for (currentEpisodeSteps_ = 0; currentEpisodeSteps_ < maxSteps && !simulationPaused_; ++currentEpisodeSteps_) {
        // Get sensory percept from world
        const SensoryPercept& percept = world.getSensoryPercept();
        
        // Process sensory input
        processSensoryInput(percept);
        
        // Decode motor command
        MotorCommand cmd = decodeMotorCommand();
        recordAction(cmd);
        
        // Apply to world
        world.applyMotorCommand(cmd, currentEpisodeSteps_ * 0.001);
        
        // Update development
        if (developmentEnabled_) {
            updateDevelopment(0.001);
        }
        
        // Record performance
        if (performanceMonitoringEnabled_) {
            recordStep(0.001);
        }
        
        // Small delay for simulation pacing
        if (debugLoggingEnabled_ && currentEpisodeSteps_ % 100 == 0) {
            NLM_LOG_DEBUG("Episode step " + std::to_string(currentEpisodeSteps_));
        }
    }
    
    return currentEpisodeSteps_;
}

void AgentBrain::updateEpisodeReward(float reward) {
    episodeReward_ += reward;
    if (debugLoggingEnabled_ && currentEpisodeSteps_ > 0) {
        NLM_LOG_DEBUG("Episode reward: " + std::to_string(episodeReward_) + 
                     " (avg: " + std::to_string(episodeReward_ / currentEpisodeSteps_) + ")");
    }
}

float AgentBrain::getEpisodeReward() const {
    return episodeReward_;
}

void AgentBrain::updateEpisodeNovelty(float novelty) {
    episodeNovelty_ = (episodeNovelty_ * 0.9f) + (novelty * 0.1f); // Exponential moving average
}

float AgentBrain::getEpisodeNovelty() const {
    return episodeNovelty_;
}

// Action replay
void AgentBrain::replayActions(const std::vector<MotorCommand>& actions, double speed) {
    if (actions.empty()) {
        NLM_LOG_WARN("No actions to replay");
        return;
    }
    
    // Save current state
    size_t savedSteps = stepsProcessed_;
    double savedTime = totalTime_;
    float savedDopamine = dopamineLevel_;
    
    // Replay actions
    double timeStep = 0.001 / speed;
    for (size_t i = 0; i < actions.size(); ++i) {
        // Apply action (simplified - in real implementation would integrate with world)
        // For now, just record performance
        recordStep(timeStep);
        
        if (debugLoggingEnabled_ && i % 100 == 0) {
            NLM_LOG_DEBUG("Replaying action " + std::to_string(i) + "/" + 
                         std::to_string(actions.size()));
        }
    }
    
    // Restore state
    stepsProcessed_ = savedSteps;
    totalTime_ = savedTime;
    dopamineLevel_ = savedDopamine;
}

// System status
void AgentBrain::resetSystem() {
    reset();
    // Clear performance stats
    stepsProcessed_ = 0;
    totalTime_ = 0.0;
    maxFiringRate_ = 0.0f;
    avgFiringRate_ = 0.0f;
    actionSequence_.clear();
    
    if (debugLoggingEnabled_) {
        NLM_LOG_DEBUG("System reset completed");
    }
}

SystemStatus AgentBrain::getSystemStatus() const {
    SystemStatus status;
    
    status.totalSteps = stepsProcessed_;
    status.totalTime = totalTime_;
    status.simulationSpeed = simulationSpeedMultiplier_;
    status.simulationPaused = simulationPaused_;
    status.performanceMonitoringEnabled = performanceMonitoringEnabled_;
    status.debugLoggingEnabled = debugLoggingEnabled_;
    status.rewardModulationEnabled = rewardModulationEnabled_;
    status.structuralPlasticityEnabled = structuralPlasticityEnabled_;
    status.developmentEnabled = developmentEnabled_;
    status.curiosityEnabled = curiosityEnabled_;
    status.developmentalStage = getDevelopmentalStage();
    status.currentNeuromodulation = getNeuromodulationLevel();
    status.currentCuriosity = getCuriosityLevel();
    status.currentNovelty = getNoveltyLevel();
    status.currentPredictionError = getPredictionError();
    status.avgFiringRate = avgFiringRate_;
    status.maxFiringRate = maxFiringRate_;
    status.currentActionSequenceLength = actionSequence_.size();
    status.episodeSteps = currentEpisodeSteps_;
    status.episodeReward = episodeReward_;
    status.episodeNovelty = episodeNovelty_;
    
    return status;
}

void AgentBrain::logDetailedStatus() const {
    SystemStatus status = getSystemStatus();
    
    NLM_LOG_INFO("=== DETAILED AGENT BRAIN STATUS ===");
    NLM_LOG_INFO("Simulation: " + std::to_string(status.totalSteps) + " steps in " + 
                 std::to_string(status.totalTime) + "s (" + 
                 std::to_string(status.avgStepsPerSecond) + " Hz)");
    NLM_LOG_INFO("Controls: speed=" + std::to_string(status.simulationSpeed) + 
                 "x paused=" + std::to_string(status.simulationPaused));
    NLM_LOG_INFO("Learning: neuromodulation=" + std::to_string(status.rewardModulationEnabled_) +
                 " plasticity=" + std::to_string(status.structuralPlasticityEnabled_) +
                 " development=" + std::to_string(status.developmentEnabled_));
    NLM_LOG_INFO("Exploration: curiosity=" + std::to_string(status.curiosityEnabled_));
    NLM_LOG_INFO("Neurals: stage=" + std::to_string(static_cast<int>(status.developmentalStage)) +
                 " dopamine=" + std::to_string(status.currentNeuromodulation) +
                 " curiosity=" + std::to_string(status.currentCuriosity));
    NLM_LOG_INFO("Activity: firing rate avg=" + std::to_string(status.avgFiringRate) +
                 " max=" + std::to_string(status.maxFiringRate) +
                 " episode reward=" + std::to_string(status.episodeReward));
    NLM_LOG_INFO("Memory: " + std::to_string(status.currentActionSequenceLength) + " actions recorded");
    NLM_LOG_INFO("==================================");
}

} // namespace nlm
