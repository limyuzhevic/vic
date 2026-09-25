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
    , physicsEnabled_(true)
    , actionLoggingEnabled_(false)
    , x_(0.0f)
    , y_(0.0f)
    , orientation_(0.0f)
    , velocityX_(0.0f)
    , velocityY_(0.0f)
    , angularVelocity_(0.0f)
    , maxSpeed_(1.0f)
    , health_(100.0f)
    , energy_(100.0f)
    , activeTime_(0.0)
    , lastPositionUpdate_(0.0)
    , motorThreshold_(0.5f)
    , explorationRate_(0.2f)
    , developmentSpeed_(1.0f)
    , sensoryGain_(1.0f)
    , spikeCount_(0)
    , lastSpikeTime_(0.0)
    , cumulativeScore_(0.0f)
    , stateChanged_(false)
    , stateVersion_(1)
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

size_t AgentBrain::getActiveMotorNeurons() const {
    size_t count = 0;
    for (const auto& neuron : motorForward_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : motorBackward_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : motorTurnLeft_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : motorTurnRight_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : motorInteract_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : motorWait_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    return count;
}

size_t AgentBrain::getActiveSensoryNeurons() const {
    size_t count = 0;
    for (const auto& neuron : sensoryVision_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : sensoryTouch_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : sensoryInternal_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    for (const auto& neuron : sensoryProprioception_) {
        if (neuron && neuron->getState().isActive) count++;
    }
    return count;
}

std::string AgentBrain::getNeuralActivityStats() const {
    std::string stats = "Neural Activity Statistics:\n";
    stats += "  Motor neurons: " + std::to_string(getActiveMotorNeurons()) + "/" + 
             std::to_string(motorForward_.size() + motorBackward_.size() + 
                           motorTurnLeft_.size() + motorTurnRight_.size() +
                           motorInteract_.size() + motorWait_.size()) + " active\n";
    stats += "  Sensory neurons: " + std::to_string(getActiveSensoryNeurons()) + "/" +
             std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                           sensoryInternal_.size() + sensoryProprioception_.size()) + " active\n";
    stats += "  Neuromodulation: dopamine=" + std::to_string(dopamineLevel_) + ", " +
             "curiosity=" + std::to_string(curiosityLevel_) + ", " +
             "novelty=" + std::to_string(noveltyLevel_) + "\n";
    stats += "  Development stage: " + std::to_string(static_cast<int>(developmentalAge_ / 60.0)) + "\n";
    return stats;
}

void AgentBrain::updatePhysics(double timestep) {
    if (!physicsEnabled_) return;
    
    // Update position based on velocity
    x_ += velocityX_ * timestep;
    y_ += velocityY_ * timestep;
    orientation_ += angularVelocity_ * timestep;
    
    // Clamp orientation
    orientation_ = std::fmod(orientation_, 2.0f * M_PI);
    if (orientation_ < 0.0f) orientation_ += 2.0f * M_PI;
    
    // Apply energy decay
    energy_ = std::max(0.0f, energy_ - energy_ * 0.01f * timestep);
    
    // Update active time
    activeTime_ += timestep;
    
    // Log position if enabled
    if (actionLoggingEnabled_) {
        NLM_LOG_DEBUG("Agent position: x=" + std::to_string(x_) + ", y=" + 
                     std::to_string(y_) + ", heading=" + std::to_string(orientation_));
    }
}

void AgentBrain::applyForce(float fx, float fy) {
    if (!physicsEnabled_) return;
    
    // Apply force (simplified physics)
    velocityX_ += fx * 0.01f;
    velocityY_ += fy * 0.01f;
    
    // Limit speed
    float speed = std::sqrt(velocityX_ * velocityX_ + velocityY_ * velocityY_);
    if (speed > maxSpeed_) {
        velocityX_ = velocityX_ * maxSpeed_ / speed;
        velocityY_ = velocityY_ * maxSpeed_ / speed;
    }
}

void AgentBrain::setPosition(float x, float y) {
    x_ = x;
    y_ = y;
    lastPositionUpdate_ = activeTime_;
}

std::pair<float, float> AgentBrain::getPosition() const {
    return std::make_pair(x_, y_);
}

float AgentBrain::getDirection() const {
    return orientation_;
}

void AgentBrain::setDirection(float heading) {
    orientation_ = heading;
    std::fmod(orientation_, 2.0f * M_PI);
    if (orientation_ < 0.0f) orientation_ += 2.0f * M_PI;
}

std::pair<float, float> AgentBrain::getVelocity() const {
    return std::make_pair(velocityX_, velocityY_);
}

void AgentBrain::setMaxSpeed(float speed) {
    maxSpeed_ = speed;
}

float AgentBrain::getMaxSpeed() const {
    return maxSpeed_;
}

std::vector<Action> AgentBrain::getActionHistory(size_t maxHistory) const {
    if (actionHistory_.size() <= maxHistory) {
        return actionHistory_;
    }
    
    size_t startIdx = actionHistory_.size() - maxHistory;
    return std::vector<Action>(actionHistory_.begin() + startIdx, actionHistory_.end());
}

std::vector<SensoryPercept> AgentBrain::getSensoryHistory(size_t maxHistory) const {
    if (sensoryHistory_.size() <= maxHistory) {
        return sensoryHistory_;
    }
    
    size_t startIdx = sensoryHistory_.size() - maxHistory;
    return std::vector<SensoryPercept>(sensoryHistory_.begin() + startIdx, sensoryHistory_.end());
}

AgentBrain::PerformanceMetrics AgentBrain::getPerformanceMetrics() const {
    PerformanceMetrics metrics = metrics_;
    
    // Calculate efficiency (reward per action)
    if (metrics.actionCount > 0) {
        metrics.efficiency = metrics.totalReward / metrics.actionCount;
    }
    
    // Calculate exploration score (based on random actions vs planned actions)
    metrics.explorationScore = curiosityLevel_ * 0.7f + 
                              (noveltyLevel_ > 0.5f ? 0.3f : 0.0f);
    
    // Calculate learning progress (based on prediction error reduction)
    static float previousPredictionError = predictionError_;
    if (activeTime_ > 0.0) {
        float errorChange = std::abs(predictionError_ - previousPredictionError_);
        metrics.learningProgress = std::min(1.0f, errorChange * 10.0f);
    }
    
    // Calculate session duration
    metrics.sessionDuration = activeTime_;
    
    return metrics;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    physicsEnabled_ = true;
    actionLoggingEnabled_ = false;
    x_ = 0.0f;
    y_ = 0.0f;
    orientation_ = 0.0f;
    velocityX_ = 0.0f;
    velocityY_ = 0.0f;
    angularVelocity_ = 0.0f;
    maxSpeed_ = 1.0f;
    health_ = 100.0f;
    energy_ = 100.0f;
    activeTime_ = 0.0;
    lastPositionUpdate_ = 0.0;
    motorThreshold_ = 0.5f;
    explorationRate_ = 0.2f;
    developmentSpeed_ = 1.0f;
    sensoryGain_ = 1.0f;
    
    // Clear history
    actionHistory_.clear();
    sensoryHistory_.clear();
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset metrics
    resetMetrics();
}

} // namespace nlm
