#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , acetylcholineLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , serotoninLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , attentionModulationEnabled_(true)
    , memoryModulationEnabled_(true)
    , socialModulationEnabled_(true)
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
    
    // Update neuromodulation levels from brain
    if (brain_) {
        dopamineLevel_ = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
        acetylcholineLevel_ = brain_->getAcetylcholine() ? brain_->getAcetylcholine()->getLevel() : 0.0f;
        norepinephrineLevel_ = brain_->getNorepinephrine() ? brain_->getNorepinephrine()->getLevel() : 0.0f;
        serotoninLevel_ = brain_->getSerotonin() ? brain_->getSerotonin()->getLevel() : 0.0f;
        curiosityLevel_ = brain_->getCuriosity() ? brain_->getCuriosity()->getLevel() : 0.0f;
        predictionError_ = brain_->getPredictionErrorSignal() ? brain_->getPredictionErrorSignal()->getError() : 0.0f;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply neuromodulation-based selection
    decoded = selectWithNeuromodulation(decoded);
    
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

MotorCommand AgentBrain::selectWithNeuromodulation(MotorCommand defaultCmd) {
    // Combine default command with neuromodulatory influences
    // Higher dopamine increases approach behavior
    // Higher acetylcholine enhances attention and action selection
    // Higher norepinephrine promotes exploration
    // Higher serotonin reduces impulsivity
    
    float selectionScore[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    
    // Base scores from default command
    switch (defaultCmd) {
        case MotorCommand::MoveForward: selectionScore[0] = 1.0f; break;
        case MotorCommand::MoveBackward: selectionScore[1] = 1.0f; break;
        case MotorCommand::TurnLeft: selectionScore[2] = 1.0f; break;
        case MotorCommand::TurnRight: selectionScore[3] = 1.0f; break;
        case MotorCommand::Interact: selectionScore[4] = 1.0f; break;
        case MotorCommand::Wait: selectionScore[5] = 1.0f; break;
    }
    
    // Modulate with neuromodulators
    // Dopamine enhances approach behaviors (MoveForward, Interact)
    float dopamineApproach = dopamineLevel_ * 0.5f;
    selectionScore[0] += dopamineApproach * 0.3f;  // MoveForward
    selectionScore[4] += dopamineApproach * 0.2f;  // Interact
    
    // Acetylcholine enhances attention to all options
    float acetylcholineFocus = acetylcholineLevel_ * 0.5f;
    for (int i = 0; i < 6; ++i) {
        selectionScore[i] += acetylcholineFocus * 0.1f;
    }
    
    // Norepinephrine promotes exploration (turn behaviors)
    float norepinephrineExplore = norepinephrineLevel_ * 0.5f;
    selectionScore[2] += norepinephrineExplore * 0.2f;  // TurnLeft
    selectionScore[3] += norepinephrineExplore * 0.2f;  // TurnRight
    
    // Serotonin reduces impulsivity (increases threshold for strong responses)
    float serotoninImpulse = serotoninLevel_ * 0.5f;
    for (int i = 0; i < 3; ++i) {  // Reduce approach scores
        selectionScore[i] *= (1.0f - serotoninImpulse * 0.2f);
    }
    
    // Curiosity promotes exploration
    float curiosityExplore = curiosityLevel_ * 0.5f;
    selectionScore[2] += curiosityExplore * 0.3f;  // TurnLeft
    selectionScore[3] += curiosityExplore * 0.3f;  // TurnRight
    
    // Find maximum score
    MotorCommand best = MotorCommand::Wait;
    float bestScore = selectionScore[5];  // Default to wait
    
    if (selectionScore[0] > bestScore) { bestScore = selectionScore[0]; best = MotorCommand::MoveForward; }
    if (selectionScore[1] > bestScore) { bestScore = selectionScore[1]; best = MotorCommand::MoveBackward; }
    if (selectionScore[2] > bestScore) { bestScore = selectionScore[2]; best = MotorCommand::TurnLeft; }
    if (selectionScore[3] > bestScore) { bestScore = selectionScore[3]; best = MotorCommand::TurnRight; }
    if (selectionScore[4] > bestScore) { bestScore = selectionScore[4]; best = MotorCommand::Interact; }
    
    return best;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_) return;
    
    // Compute reward prediction error
    float error = reward - predictedReward;
    predictionError_ = error;
    expectedReward_ = reward;
    
    // Apply reward modulation to dopamine system
    if (brain_->getDopamine()) {
        // Dopamine responds to prediction error
        brain_->getDopamine()->signalRewardPredictionError(error);
    }
    
    // Acetylcholine modulates attention based on reward prediction error
    if (brain_->getAcetylcholine()) {
        float attentionLevel = std::min(1.0f, std::abs(error) * 2.0f);
        brain_->getAcetylcholine()->updateAttention(attentionLevel, noveltyLevel_);
    }
    
    // Norepinephrine responds to salience (magnitude of prediction error)
    if (brain_->getNorepinephrine()) {
        float salience = std::abs(error);
        brain_->getNorepinephrine()->updateArousal(salience, 0.0f);
    }
    
    // Serotonin modulates social behavior based on prediction error
    if (brain_->getSerotonin()) {
        brain_->getSerotonin()->updateImpulseControl(0.0f, error);
    }
    
    // Update development based on reward experience
    developmentalAge_ += 0.1; // Increment age based on reward experience
    plasticityModifier_ = 1.0f + dopamineLevel_ * 0.5f; // Dopamine enhances plasticity
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Development affects plasticity rates based on age
    if (developmentalAge_ > 10.0 && developmentalAge_ < 20.0) {
        // Critical period - high plasticity
        if (brain_->getDevelopmentSystem()) {
            brain_->getDevelopmentSystem()->setPlasticityMultiplier(2.0f);
        }
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0002f);
            brain_->getStructuralPlasticity()->setPruningRate(0.000005f);
        }
    } else if (developmentalAge_ >= 20.0) {
        // Maturity - stable plasticity
        if (brain_->getDevelopmentSystem()) {
            brain_->getDevelopmentSystem()->setPlasticityMultiplier(1.0f);
        }
        if (brain_->getStructuralPlasticity()) {
            brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f);
            brain_->getStructuralPlasticity()->setPruningRate(0.00001f);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getDopamineLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getAcetylcholineLevel() const {
    return acetylcholineLevel_;
}

float AgentBrain::getNorepinephrineLevel() const {
    return norepinephrineLevel_;
}

float AgentBrain::getSerotoninLevel() const {
    return serotoninLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    if (!brain_) return;
    
    brain_->reset();
    
    dopamineLevel_ = 0.0f;
    acetylcholineLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    previousVision_.clear();
}

} // namespace nlm