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
    
    // Integrate prediction system if available
    if (brain_ && brain_->getPredictionSystem()) {
        // Get prediction error signal for curiosity and exploration
        auto* predictionErr = brain_->getPredictionSystem()->getPredictionErrorSignal();
        if (predictionErr) {
            // Update prediction error from brain system
            predictionError_ = predictionErr->getLevel();
            
            // Use prediction error to modulate curiosity and exploration
            if (curiosityEnabled_ && predictionError_ != 0.0f) {
                // High prediction error -> high curiosity (explore more)
                float predictionCuriosity = std::abs(predictionError_) * 2.0f;
                curiosityLevel_ = std::max(curiosityLevel_, predictionCuriosity);
            }
        }
    }
    
    // Integrate memory systems if available
    if (brain_) {
        // Get working memory for short-term memory integration
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            // Store sensory input in working memory
            if (!vision.empty()) {
                for (size_t i = 0; i < std::min(vision.size(), size_t(10)); ++i) {
                    workingMem->storeToNeuron(sensoryVision_[i] ? sensoryVision_[i]->getId() : NeuronId(), 
                                           vision[i] / 10.0f);
                }
            }
        }
        
        // Get episodic memory for long-term storage
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem && stepNumber % 100 == 0) {  // Store episode every 100 steps
            // Create episodic memory item
            EpisodicMemoryItem episode;
            episode.timestamp = stepNumber;
            episode.reward = reward;
            
            // Store active neurons
            for (const auto& region : brain_->getRegions()) {
                for (const auto& pop : region->getPopulations()) {
                    for (const Neuron* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            episodicMem->storeEpisode(episode);
        }
    }
    
    // Integrate attention system if available
    if (brain_ && brain_->getAttention()) {
        // Get attention system's focus areas
        auto* attentionSys = brain_->getAttention();
        
        // Use attention to filter sensory input
        if (!vision.empty() && attentionSys->getFocusedArea() != -1) {
            // Apply attention filter to vision data
            for (size_t i = 0; i < vision.size(); ++i) {
                // Simple attention modulation based on focal point
                if (i % 50 == attentionSys->getFocusedArea() % 50) {  // Every 50th element
                    vision[i] *= attentionSys->getAttentionLevel();
                }
            }
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Integrate NeuralPlanner for intelligent action selection
    if (brain_ && brain_->getPlanner()) {
        // Get current brain state as sensory input for planning
        std::vector<float> currentState = collectSensoryForPlanning();
        
        // Get planned action from NeuralPlanner
        ActionType plannedAction = brain_->getPlanner()->planAction(currentState, 0.5f);
        
        // Convert ActionType to MotorCommand if planner provided useful guidance
        if (plannedAction != ActionType::Wait) {
            decoded = actionToMotorCommand(plannedAction);
            NLM_LOG_INFO("Planner selected action: " + actionName(plannedAction));
        }
    }
    
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

// Helper methods for NeuralPlanner integration
std::vector<float> AgentBrain::collectSensoryForPlanning() {
    std::vector<float> currentState;
    
    // Collect vision data (256 values)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                // Collect sensory neuron activities as state vector
                if (type == NeuronType::Sensory) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        float activation = std::abs(neuron->getState().membranePotential - 
                                                   neuron->getState().restingPotential);
                        currentState.push_back(activation);
                    }
                }
            }
        }
    }
    
    // Limit to reasonable size for planning
    if (currentState.size() > 100) {
        currentState.resize(100);
    }
    
    return currentState;
}

MotorCommand AgentBrain::actionToMotorCommand(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Look: return MotorCommand::Wait; // No direct motor command for Look
        case ActionType::LookUp: return MotorCommand::Wait;
        case ActionType::LookDown: return MotorCommand::Wait;
        case ActionType::Eat: return MotorCommand::Wait;
        case ActionType::Drink: return MotorCommand::Wait;
        case ActionType::Rest: return MotorCommand::Wait;
        case ActionType::Wait: return MotorCommand::Wait;
        case ActionType::Custom: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

std::string AgentBrain::actionName(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::LookLeft: return "LookLeft";
        case ActionType::LookRight: return "LookRight";
        case ActionType::Interact: return "Interact";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {

// Helper methods for NeuralPlanner integration
std::vector<float> AgentBrain::collectSensoryForPlanning() {
    std::vector<float> currentState;
    
    // Collect vision data (256 values)
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                // Collect sensory neuron activities as state vector
                if (type == NeuronType::Sensory) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        float activation = std::abs(neuron->getState().membranePotential - 
                                                   neuron->getState().restingPotential);
                        currentState.push_back(activation);
                    }
                }
            }
        }
    }
    
    // Limit to reasonable size for planning
    if (currentState.size() > 100) {
        currentState.resize(100);
    }
    
    return currentState;
}

MotorCommand AgentBrain::actionToMotorCommand(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Look: return MotorCommand::Wait; // No direct motor command for Look
        case ActionType::LookUp: return MotorCommand::Wait;
        case ActionType::LookDown: return MotorCommand::Wait;
        case ActionType::Eat: return MotorCommand::Wait;
        case ActionType::Drink: return MotorCommand::Wait;
        case ActionType::Rest: return MotorCommand::Wait;
        case ActionType::Wait: return MotorCommand::Wait;
        case ActionType::Custom: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

std::string AgentBrain::actionName(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::LookLeft: return "LookLeft";
        case ActionType::LookRight: return "LookRight";
        case ActionType::Interact: return "Interact";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
