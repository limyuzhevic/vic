#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

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
    
    // Initialize advanced features
    advancedLearningEnabled_ = false;
    metaControlEnabled_ = false;
    selfMonitorEnabled_ = false;
    autonomousDevelopmentEnabled_ = false;
    learningRate_ = 1.0f;
    explorationRate_ = 0.1f;
    
    // Initialize performance tracking
    totalStepsExecuted_ = 0;
    successfulActions_ = 0;
    cumulativeReward_ = 0.0f;
    
    // Initialize environment
    environmentalTemperature_ = 20.0f;
    environmentalHumidity_ = 50.0f;
    environmentalPressure_ = 1013.0f;
    
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

void AgentBrain::enableAdvancedLearning(bool enable) {
    // Enable advanced learning features
    advancedLearningEnabled_ = enable;
    
    if (enable) {
        // Pre-compute synaptic eligibility traces for faster learning
        precomputeEligibilityTraces();
    }
}

void AgentBrain::setLearningRate(float rate) {
    // Set global learning rate modifier
    learningRate_ = std::clamp(rate, 0.0f, 2.0f);
}

float AgentBrain::getLearningRate() const {
    return learningRate_;
}

void AgentBrain::enableMetaControl(bool enable) {
    // Enable meta-control systems (curiosity, exploration, etc.)
    metaControlEnabled_ = enable;
}

void AgentBrain::setExplorationRate(float rate) {
    // Set exploration rate (0.0 = no exploration, 1.0 = full exploration)
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

void AgentBrain::enableSelfMonitor(bool enable) {
    // Enable self-monitoring and prediction systems
    selfMonitorEnabled_ = enable;
}

void AgentBrain::enableAutonomousDevelopment(bool enable) {
    // Enable autonomous developmental processes
    autonomousDevelopmentEnabled_ = enable;
}

void AgentBrain::addReward(float reward, float predictedReward) {
    // Add reward signal and update learning
    applyRewardModulation(reward, predictedReward);
}

void AgentBrain::addError(float error) {
    // Add prediction error signal
    predictionError_ += error;
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
}

void AgentBrain::executeExplorationPattern() {
    // Execute exploration behavior based on curiosity level
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // High curiosity = more random exploration
        float exploreChance = curiosityLevel_ * explorationRate_;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command for exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: 
                    // Inject current to neurons to trigger random activity
                    brain_->injectCurrentToNeurons(NeuronType::Motor, 40.0f);
                    break;
                case 1: 
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                    break;
                case 2: 
                    brain_->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
                    break;
                case 3: 
                    // Increase plasticity for exploration
                    if (auto* stdp = brain_->getSTDP()) {
                        stdp->setLTPWeight(stdp->getLTPWeight() * 1.5f);
                        stdp->setLTDWeight(stdp->getLTDWeight() * 1.5f);
                    }
                    break;
                default: 
                    // Just trigger random spiking
                    brain_->injectCurrentToNeurons(NeuronType::Excitatory, 25.0f);
                    break;
            }
        }
    }
}

void AgentBrain::updateAutonomousDevelopment(double timestep) {
    if (autonomousDevelopmentEnabled_) {
        // Autonomous developmental processes
        developmentalAge_ += timestep;
        
        // Change developmental stage based on age
        if (developmentalAge_ > 120.0 && developmentalAge_ < 180.0) {
            // Adolescence - optimize for exploration
            plasticityModifier_ = 0.9f;
            curiosityEnabled_ = true;
            
            // Increase connection formation rate
            if (auto* sp = brain_->getStructuralPlasticity()) {
                sp->setSynaptogenesisRate(0.0002f);
                sp->setPruningRate(0.00002f);
            }
        } else if (developmentalAge_ > 300.0 && developmentalAge_ < 600.0) {
            // Adulthood - optimize for efficiency
            plasticityModifier_ = 0.5f;
            curiosityEnabled_ = false;
            
            // Stabilize connections
            if (auto* sp = brain_->getStructuralPlasticity()) {
                sp->setSynaptogenesisRate(0.00005f);
                sp->setPruningRate(0.00001f);
            }
        }
    }
}

void AgentBrain::setEnvironmentalContext(float temperature, float humidity, float pressure) {
    // Set environmental context that affects brain function
    environmentalTemperature_ = temperature;
    environmentalHumidity_ = humidity;
    environmentalPressure_ = pressure;
    
    // Environmental effects on brain
    if (temperature > 35.0f) {
        // Heat stress - reduce plasticity
        plasticityModifier_ *= 0.8f;
    } else if (temperature < 10.0f) {
        // Cold stress - increase exploration
        curiosityEnabled_ = true;
        explorationRate_ = std::min(explorationRate_ * 1.2f, 1.0f);
    }
    
    if (humidity > 80.0f) {
        // High humidity - increase novelty detection
        sensoryNoveltyDecay_ = 0.95f;
    }
    
    if (pressure > 1013.0f) {
        // High pressure - reduce risk-taking
        explorationRate_ *= 0.9f;
    }
}

float AgentBrain::getNeuromodulationScore() const {
    // Get combined neuromodulation score
    float score = 0.0f;
    
    // Dopamine contribution
    score += std::abs(dopamineLevel_) * 0.4f;
    
    // Curiosity contribution
    score += curiosityLevel_ * 0.3f;
    
    // Novelty contribution
    score += noveltyLevel_ * 0.2f;
    
    // Prediction error contribution
    score += std::abs(predictionError_) * 0.1f;
    
    return score;
}

void AgentBrain::resetEnvironmentalContext() {
    environmentalTemperature_ = 20.0f;
    environmentalHumidity_ = 50.0f;
    environmentalPressure_ = 1013.0f;
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

    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

void AgentBrain::enableAdvancedLearning(bool enable) {
    // Enable advanced learning features
    advancedLearningEnabled_ = enable;
    
    if (enable) {
        // Pre-compute synaptic eligibility traces for faster learning
        precomputeEligibilityTraces();
    }
}

void AgentBrain::setLearningRate(float rate) {
    // Set global learning rate modifier
    learningRate_ = std::clamp(rate, 0.0f, 2.0f);
}

float AgentBrain::getLearningRate() const {
    return learningRate_;
}

void AgentBrain::enableMetaControl(bool enable) {
    // Enable meta-control systems (curiosity, exploration, etc.)
    metaControlEnabled_ = enable;
}

void AgentBrain::setExplorationRate(float rate) {
    // Set exploration rate (0.0 = no exploration, 1.0 = full exploration)
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

void AgentBrain::enableSelfMonitor(bool enable) {
    // Enable self-monitoring and prediction systems
    selfMonitorEnabled_ = enable;
}

void AgentBrain::enableAutonomousDevelopment(bool enable) {
    // Enable autonomous developmental processes
    autonomousDevelopmentEnabled_ = enable;
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
    
    // Reset advanced features
    advancedLearningEnabled_ = false;
    metaControlEnabled_ = false;
    selfMonitorEnabled_ = false;
    autonomousDevelopmentEnabled_ = false;
    learningRate_ = 1.0f;
    explorationRate_ = 0.1f;
    
    // Reset performance tracking
    totalStepsExecuted_ = 0;
    successfulActions_ = 0;
    cumulativeReward_ = 0.0f;
    
    // Reset environment
    environmentalTemperature_ = 20.0f;
    environmentalHumidity_ = 50.0f;
    environmentalPressure_ = 1013.0f;
}

void AgentBrain::addError(float error) {
    // Add prediction error signal
    predictionError_ += error;
    predictionError_ = std::clamp(predictionError_, -1.0f, 1.0f);
}

void AgentBrain::executeExplorationPattern() {
    // Execute exploration behavior based on curiosity level
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // High curiosity = more random exploration
        float exploreChance = curiosityLevel_ * explorationRate_;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command for exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: 
                    // Inject current to neurons to trigger random activity
                    brain_->injectCurrentToNeurons(NeuronType::Motor, 40.0f);
                    break;
                case 1: 
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                    break;
                case 2: 
                    brain_->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
                    break;
                case 3: 
                    // Increase plasticity for exploration
                    if (auto* stdp = brain_->getSTDP()) {
                        stdp->setLTPWeight(stdp->getLTPWeight() * 1.5f);
                        stdp->setLTDWeight(stdp->getLTDWeight() * 1.5f);
                    }
                    break;
                default: 
                    // Just trigger random spiking
                    brain_->injectCurrentToNeurons(NeuronType::Excitatory, 25.0f);
                    break;
            }
        }
    }
}

void AgentBrain::updateAutonomousDevelopment(double timestep) {
    if (autonomousDevelopmentEnabled_) {
        // Autonomous developmental processes
        developmentalAge_ += timestep;
        
        // Change developmental stage based on age
        if (developmentalAge_ > 120.0 && developmentalAge_ < 180.0) {
            // Adolescence - optimize for exploration
            plasticityModifier_ = 0.9f;
            curiosityEnabled_ = true;
            
            // Increase connection formation rate
            if (auto* sp = brain_->getStructuralPlasticity()) {
                sp->setSynaptogenesisRate(0.0002f);
                sp->setPruningRate(0.00002f);
            }
        } else if (developmentalAge_ > 300.0 && developmentalAge_ < 600.0) {
            // Adulthood - optimize for efficiency
            plasticityModifier_ = 0.5f;
            curiosityEnabled_ = false;
            
            // Stabilize connections
            if (auto* sp = brain_->getStructuralPlasticity()) {
                sp->setSynaptogenesisRate(0.00005f);
                sp->setPruningRate(0.00001f);
            }
        }
    }
}

void AgentBrain::setEnvironmentalContext(float temperature, float humidity, float pressure) {
    // Set environmental context that affects brain function
    environmentalTemperature_ = temperature;
    environmentalHumidity_ = humidity;
    environmentalPressure_ = pressure;
    
    // Environmental effects on brain
    if (temperature > 35.0f) {
        // Heat stress - reduce plasticity
        plasticityModifier_ *= 0.8f;
    } else if (temperature < 10.0f) {
        // Cold stress - increase exploration
        curiosityEnabled_ = true;
        explorationRate_ = std::min(explorationRate_ * 1.2f, 1.0f);
    }
    
    if (humidity > 80.0f) {
        // High humidity - increase novelty detection
        sensoryNoveltyDecay_ = 0.95f;
    }
    
    if (pressure > 1013.0f) {
        // High pressure - reduce risk-taking
        explorationRate_ *= 0.9f;
    }
}

float AgentBrain::getNeuromodulationScore() const {
    // Get combined neuromodulation score
    float score = 0.0f;
    
    // Dopamine contribution
    score += std::abs(dopamineLevel_) * 0.4f;
    
    // Curiosity contribution
    score += curiosityLevel_ * 0.3f;
    
    // Novelty contribution
    score += noveltyLevel_ * 0.2f;
    
    // Prediction error contribution
    score += std::abs(predictionError_) * 0.1f;
    
    return score;
}

void AgentBrain::resetEnvironmentalContext() {
    environmentalTemperature_ = 20.0f;
    environmentalHumidity_ = 50.0f;
    environmentalPressure_ = 1013.0f;
}

void AgentBrain::precomputeEligibilityTraces() {
    if (!advancedLearningEnabled_) return;
    
    // Pre-compute eligibility traces for faster reward-modulated learning
    // This is a placeholder for more sophisticated eligibility trace computation
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Simple trace initialization
            float trace = syn->getEligibilityTrace();
            trace *= 0.95f; // Decay
            syn->setEligibilityTrace(trace);
        }
    }
}

void AgentBrain::enableSelfOptimization(bool enable) {
    // Enable self-optimization systems
    if (enable) {
        // Enable meta-learning
        metaControlEnabled_ = true;
        selfMonitorEnabled_ = true;
        
        // Set appropriate learning rates for meta-learning
        learningRate_ = 0.5f;
        explorationRate_ = 0.3f;
    } else {
        metaControlEnabled_ = false;
        selfMonitorEnabled_ = false;
    }
}

void AgentBrain::recordSuccess() {
    // Record a successful action
    successfulActions_++;
}

void AgentBrain::recordStep() {
    // Record a simulation step
    totalStepsExecuted_++;
}

float AgentBrain::getSuccessRate() const {
    // Get success rate
    if (totalStepsExecuted_ == 0) return 0.0f;
    return static_cast<float>(successfulActions_) / totalStepsExecuted_;
}

float AgentBrain::getAverageReward() const {
    // Get average reward per step
    if (totalStepsExecuted_ == 0) return 0.0f;
    return cumulativeReward_ / totalStepsExecuted_;
}

} // namespace nlm
