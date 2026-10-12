#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <chrono>
#include <random>

namespace nlm {

struct AgentBrain::Impl {
    // Agent goals and objectives
    std::vector<std::string> goals;
    std::unordered_map<std::string, float> goalValues;
    float overallMotivation;
    
    // Advanced state management
    std::unordered_map<std::string, double> stateVariables;
    std::chrono::system_clock::time_point lastDecisionTime;
    uint64_t decisionCount;
    
    // Enhanced decision making
    std::vector<std::pair<MotorCommand, float>> actionPreferences;
    float explorationFactor;
    
    // Performance tracking
    uint64_t successfulActions;
    uint64_t totalActions;
    double averageActionQuality;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
};

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
    , pImpl(std::make_unique<Impl>()) {
    pImpl->goalValues = {
        {"survival", 1.0f},
        {"exploration", 0.8f},
        {"learning", 0.9f},
        {"development", 0.7f}
    };
    pImpl->overallMotivation = 1.0f;
    pImpl->lastDecisionTime = std::chrono::system_clock::now();
    pImpl->decisionCount = 0;
    pImpl->explorationFactor = 0.1f;
    pImpl->successfulActions = 0;
    pImpl->totalActions = 0;
    pImpl->averageActionQuality = 0.0f;
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = pImpl->motorForward_.size() + pImpl->motorBackward_.size() + 
                                    pImpl->motorTurnLeft_.size() + pImpl->motorTurnRight_.size() +
                                    pImpl->motorInteract_.size() + pImpl->motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: pImpl->motorForward_.push_back(n); break;
                            case 1: pImpl->motorBackward_.push_back(n); break;
                            case 2: pImpl->motorTurnLeft_.push_back(n); break;
                            case 3: pImpl->motorTurnRight_.push_back(n); break;
                            case 4: pImpl->motorInteract_.push_back(n); break;
                            case 5: pImpl->motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = pImpl->sensoryVision_.size() + pImpl->sensoryTouch_.size() +
                                    pImpl->sensoryInternal_.size() + pImpl->sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: pImpl->sensoryVision_.push_back(n); break;
                            case 1: pImpl->sensoryTouch_.push_back(n); break;
                            case 2: pImpl->sensoryInternal_.push_back(n); break;
                            case 3: pImpl->sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
    
    // Initialize action preferences
    updateActionPreferences();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    pImpl->stateVariables.clear();
    pImpl->stateVariables["energy"] = 1.0;
    pImpl->stateVariables["health"] = 1.0;
    pImpl->stateVariables["age"] = 0.0;
    pImpl->stateVariables["safety"] = 1.0;
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(pImpl->sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(pImpl->sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(pImpl->sensoryInternal_.size()) + " internal sensory neurons");
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
    for (size_t i = 0; i < pImpl->sensoryVision_.size() && i < vision.size(); ++i) {
        if (pImpl->sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            pImpl->sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < pImpl->sensoryTouch_.size() && i < touch.size(); ++i) {
        if (pImpl->sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            pImpl->sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < pImpl->sensoryInternal_.size() && i < intern.size(); ++i) {
        if (pImpl->sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            pImpl->sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < pImpl->sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (pImpl->sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            pImpl->sensoryProprioception_[i]->injectCurrent(current);
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
    
    // Update state variables based on sensory input
    updateStateVariables(PerceptToVector(percept));
    updateMotivation();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply goal-driven behavior
    MotorCommand goalDriven = selectWithGoals();
    if (goalDriven != MotorCommand::Wait) {
        return goalDriven;
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
    
    float forwardAct = calcActivity(pImpl->motorForward_);
    float backwardAct = calcActivity(pImpl->motorBackward_);
    float leftAct = calcActivity(pImpl->motorTurnLeft_);
    float rightAct = calcActivity(pImpl->motorTurnRight_);
    float interactAct = calcActivity(pImpl->motorInteract_);
    float waitAct = calcActivity(pImpl->motorWait_);
    
    // Apply action preferences
    forwardAct *= getActionPreference(MotorCommand::MoveForward);
    backwardAct *= getActionPreference(MotorCommand::MoveBackward);
    leftAct *= getActionPreference(MotorCommand::TurnLeft);
    rightAct *= getActionPreference(MotorCommand::TurnRight);
    interactAct *= getActionPreference(MotorCommand::Interact);
    waitAct *= getActionPreference(MotorCommand::Wait);
    
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
    // Enhanced exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with preference for exploration
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

MotorCommand AgentBrain::selectWithGoals() {
    // Check goal priorities and select actions that help achieve goals
    if (pImpl->goals.empty()) return MotorCommand::Wait;
    
    // Calculate goal satisfaction
    float survivalPriority = pImpl->goalValues["survival"] * getStateVariable("safety");
    float explorationPriority = pImpl->goalValues["exploration"] * getCuriosityLevel();
    float learningPriority = pImpl->goalValues["learning"] * getNoveltyLevel();
    float developmentPriority = pImpl->goalValues["development"] * getDevelopmentalStageValue();
    
    // Find highest priority goal
    float maxPriority = std::max({survivalPriority, explorationPriority, learningPriority, developmentPriority});
    
    if (maxPriority < 0.3f) return MotorCommand::Wait;  // Low motivation
    
    // Select action based on goal
    if (survivalPriority >= maxPriority * 0.7f) {
        // Survival-focused actions
        if (getStateVariable("energy") < 0.3f) {
            return MotorCommand::Wait;  // Rest and recover
        } else {
            return MotorCommand::MoveForward;  // Move to safety
        }
    } else if (explorationPriority >= maxPriority * 0.7f) {
        return MotorCommand::MoveForward;  // Explore new areas
    } else if (learningPriority >= maxPriority * 0.7f) {
        return MotorCommand::Interact;  // Learn from environment
    } else if (developmentPriority >= maxPriority * 0.7f) {
        return MotorCommand::MoveForward;  // Develop and grow
    }
    
    return MotorCommand::Wait;
}

void AgentBrain::updateActionPreferences() {
    // Initialize action preferences based on goals
    pImpl->actionPreferences.clear();
    
    // Set preferences based on goals
    if (pImpl->goalValues.find("survival") != pImpl->goalValues.end()) {
        pImpl->actionPreferences.push_back({MotorCommand::Wait, 0.8f});  // Rest for survival
        pImpl->actionPreferences.push_back({MotorCommand::MoveForward, 0.6f});  // Move to safety
    }
    
    if (pImpl->goalValues.find("exploration") != pImpl->goalValues.end()) {
        pImpl->actionPreferences.push_back({MotorCommand::MoveForward, 0.7f});  // Explore
        pImpl->actionPreferences.push_back({MotorCommand::TurnLeft, 0.5f});   // Change direction
        pImpl->actionPreferences.push_back({MotorCommand::TurnRight, 0.5f});  // Change direction
    }
    
    if (pImpl->goalValues.find("learning") != pImpl->goalValues.end()) {
        pImpl->actionPreferences.push_back({MotorCommand::Interact, 0.9f});  // Learn from objects
        pImpl->actionPreferences.push_back({MotorCommand::LookLeft, 0.4f});   // Observe
        pImpl->actionPreferences.push_back({MotorCommand::LookRight, 0.4f});  // Observe
    }
    
    if (pImpl->goalValues.find("development") != pImpl->goalValues.end()) {
        pImpl->actionPreferences.push_back({MotorCommand::MoveForward, 0.7f});  // Develop physically
        pImpl->actionPreferences.push_back({MotorCommand::Interact, 0.5f});  // Develop socially
    }
}

float AgentBrain::getActionPreference(MotorCommand command) const {
    for (const auto& pref : pImpl->actionPreferences) {
        if (pref.first == command) {
            return pref.second;
        }
    }
    return 0.5f;  // Default preference
}

void AgentBrain::updateStateVariables(const std::vector<float>& percept) {
    // Update internal state variables based on percept
    if (percept.size() >= 4) {
        pImpl->stateVariables["energy"] = std::max(0.0, pImpl->stateVariables["energy"] - 0.001);
        pImpl->stateVariables["health"] = std::max(0.0, pImpl->stateVariables["health"] - 0.0005);
        pImpl->stateVariables["age"] += 0.001;
        
        // Energy and health are affected by actions
        if (percept[0] > 0.5) {  // Positive input
            pImpl->stateVariables["energy"] = std::min(1.0, pImpl->stateVariables["energy"] + 0.05);
            pImpl->stateVariables["health"] = std::min(1.0, pImpl->stateVariables["health"] + 0.02);
        }
        
        // Safety based on recent experiences
        pImpl->stateVariables["safety"] = 1.0f - (percept[1] > 0.5 ? 0.5f : 0.0f);
    }
}

void AgentBrain::updateMotivation() {
    // Calculate overall motivation based on goals and state
    pImpl->overallMotivation = 0.0f;
    
    for (const auto& goalPair : pImpl->goalValues) {
        std::string goal = goalPair.first;
        float goalValue = goalPair.second;
        
        float satisfaction = 0.0f;
        if (goal == "survival") {
            satisfaction = getStateVariable("safety") * getStateVariable("health");
        } else if (goal == "exploration") {
            satisfaction = getCuriosityLevel();
        } else if (goal == "learning") {
            satisfaction = getNoveltyLevel();
        } else if (goal == "development") {
            satisfaction = getDevelopmentalStageValue();
        }
        
        pImpl->overallMotivation += goalValue * satisfaction;
    }
    
    pImpl->overallMotivation /= pImpl->goals.size();
}

std::vector<float> AgentBrain::PerceptToVector(const SensoryPercept& percept) const {
    std::vector<float> result;
    
    // Concatenate all sensory signals
    const auto& vision = percept.getVision();
    result.insert(result.end(), vision.begin(), vision.end());
    
    const auto& touch = percept.getTouch();
    result.insert(result.end(), touch.begin(), touch.end());
    
    const auto& intern = percept.getInternal();
    result.insert(result.end(), intern.begin(), intern.end());
    
    const auto& proprio = percept.getProprioception();
    result.insert(result.end(), proprio.begin(), proprio.end());
    
    const auto& audio = percept.getAudio();
    result.insert(result.end(), audio.begin(), audio.end());
    
    return result;
}

float AgentBrain::getStateVariable(const std::string& name) const {
    auto it = pImpl->stateVariables.find(name);
    return (it != pImpl->stateVariables.end()) ? it->second : 0.0f;
}

float AgentBrain::getDevelopmentalStageValue() const {
    switch (getDevelopmentalStage()) {
        case DevelopmentalStage::Initial: return 0.1f;
        case DevelopmentalStage::CriticalPeriod: return 0.5f;
        case DevelopmentalStage::Maturation: return 0.8f;
        case DevelopmentalStage::Adult: return 1.0f;
        case DevelopmentalStage::Aging: return 0.6f;
        default: return 0.5f;
    }
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
    
    // Update action quality based on reward
    pImpl->totalActions++;
    if (reward > 0.0f) {
        pImpl->successfulActions++;
    }
    pImpl->averageActionQuality = static_cast<double>(pImpl->successfulActions) / pImpl->totalActions;
    
    // Adjust goal values based on reward
    for (auto& goalPair : pImpl->goalValues) {
        if (goalPair.first == "survival" && reward > 0.0f) {
            goalPair.second = std::min(1.0f, goalPair.second + 0.01f);
        } else if (goalPair.first == "exploration" && noveltyLevel_ > 0.5f) {
            goalPair.second = std::min(1.0f, goalPair.second + 0.01f);
        }
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
    
    // Update state based on development
    pImpl->stateVariables["energy"] = std::min(1.0, pImpl->stateVariables["energy"] + 0.001);
    pImpl->stateVariables["health"] = std::min(1.0, pImpl->stateVariables["health"] + 0.0005);
    
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
    
    // Update motivation based on development
    updateMotivation();
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
    
    // Reset agent state
    pImpl->stateVariables.clear();
    pImpl->stateVariables["energy"] = 1.0;
    pImpl->stateVariables["health"] = 1.0;
    pImpl->stateVariables["age"] = 0.0;
    pImpl->stateVariables["safety"] = 1.0;
    pImpl->overallMotivation = 1.0f;
    pImpl->decisionCount = 0;
    pImpl->successfulActions = 0;
    pImpl->totalActions = 0;
    pImpl->averageActionQuality = 0.0f;
}

} // namespace nlm