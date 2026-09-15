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
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Apply curiosity-driven exploration by randomizing action selection
    float rand = static_cast<float>(rand()) / RAND_MAX;
    
    // If curiosity is high, increase exploration
    if (curiosityLevel_ > 0.7f) {
        // Choose random action
        int choice = static_cast<int>(rand() % 6);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    } else if (curiosityLevel_ > 0.4f) {
        // 50% chance of default, 50% chance of random
        if (rand < 0.5f) {
            return defaultCmd;
        } else {
            int choice = static_cast<int>(rand() % 5);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    // Compute prediction error
    float error = reward - predictedReward;
    predictionError_ = error;
    
    // Dopamine-like signal (based on prediction error)
    float dopamineSignal = error * 2.0f;
    dopamineLevel_ = std::max(0.0f, dopamineLevel_ + dopamineSignal * 0.1f);
    dopamineLevel_ = std::min(1.0f, dopamineLevel_);
    
    // Update curiosity based on prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = std::max(curiosityLevel_, std::abs(error) * 0.5f);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Plasticity modifier changes with development
    if (developmentalAge_ < 10.0) {
        plasticityModifier_ = 1.0f + developmentalAge_ * 0.1f;  // Young brain is more plastic
    } else if (developmentalAge_ < 50.0) {
        plasticityModifier_ = 2.0f;  // Prime age
    } else {
        plasticityModifier_ = 2.0f * std::exp(-(developmentalAge_ - 50.0) * 0.05f);  // Aging brain less plastic
    }
}

DevelopmentStage AgentBrain::getDevelopmentalStage() const {
    if (developmentalAge_ < 5.0) return DevelopmentalStage::Initial;
    else if (developmentalAge_ < 15.0) return DevelopmentalStage::CriticalPeriod;
    else if (developmentalAge_ < 40.0) return DevelopmentalStage::Maturation;
    else if (developmentalAge_ < 70.0) return DevelopmentalStage::Adult;
    else return DevelopmentalStage::Aging;
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
    previousVision_.clear();
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    
    // Reset history tracking
    noveltyIntegral_ = 0.0f;
    dopamineHistory_.clear();
    predictionErrorHistory_.clear();
}

Brain* AgentBrain::getBrain() {
    return brain_.get();
}

void AgentBrain::setCuriosityWeight(float weight) {
    // Update curiosity system weight
    curiosityWeight_ = weight;
}

void AgentBrain::setPredictionErrorWeight(float weight) {
    // Update prediction error system weight
    predictionErrorWeight_ = weight;
}

void AgentBrain::setNeuromodulationRate(float rate) {
    neuromodulationRate_ = rate;
}

void AgentBrain::setSensoryNoveltyDecay(float decay) {
    sensoryNoveltyDecay_ = decay;
}

const std::vector<Neuron*>& AgentBrain::getMotorForwardNeurons() const {
    return motorForward_;
}

const std::vector<Neuron*>& AgentBrain::getMotorBackwardNeurons() const {
    return motorBackward_;
}

const std::vector<Neuron*>& AgentBrain::getMotorTurnLeftNeurons() const {
    return motorTurnLeft_;
}

const std::vector<Neuron*>& AgentBrain::getMotorTurnRightNeurons() const {
    return motorTurnRight_;
}

const std::vector<Neuron*>& AgentBrain::getMotorInteractNeurons() const {
    return motorInteract_;
}

const std::vector<Neuron*>& AgentBrain::getMotorWaitNeurons() const {
    return motorWait_;
}

const std::vector<Neuron*>& AgentBrain::getVisionNeurons() const {
    return sensoryVision_;
}

const std::vector<Neuron*>& AgentBrain::getTouchNeurons() const {
    return sensoryTouch_;
}

const std::vector<Neuron*>& AgentBrain::getInternalNeurons() const {
    return sensoryInternal_;
}

const std::vector<Neuron*>& AgentBrain::getProprioceptionNeurons() const {
    return sensoryProprioception_;
}

std::vector<float> AgentBrain::getSensoryData() const {
    std::vector<float> data;
    data.reserve(previousVision_.size() + 8 + 4 + 6);  // Vision + touch + internal + proprioception
    
    // Add vision data
    data.insert(data.end(), previousVision_.begin(), previousVision_.end());
    
    // Add placeholder data for other sensory modalities
    data.insert(data.end(), 8, 0.0f);  // Touch
    data.insert(data.end(), 4, 0.0f);  // Internal
    data.insert(data.end(), 6, 0.0f);  // Proprioception
    
    return data;
}

bool AgentBrain::isExploring() const {
    return curiosityLevel_ > 0.3f;
}

float AgentBrain::getNoveltyIntegral() const {
    // Integral of novelty over time
    return noveltyIntegral_;
}

float AgentBrain::getDopamineHistoryAverage() const {
    // Average of dopamine levels over time
    if (dopamineHistory_.empty()) return 0.0f;
    float sum = 0.0f;
    for (float level : dopamineHistory_) {
        sum += level;
    }
    return sum / dopamineHistory_.size();
}

} // namespace nlm
