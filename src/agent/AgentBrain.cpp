#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace nlm;

// MotorAction implementations
void MoveForwardAction::execute() {}
void MoveBackwardAction::execute() {}
void TurnLeftAction::execute() {}
void TurnRightAction::execute() {}
void InteractAction::execute() {}
void WaitAction::execute() {}
void LookLeftAction::execute() {}
void LookRightAction::execute() {}

// Neuromodulator implementations
void DopamineNeuromodulator::update(float dt) {
    // Simple exponential decay and prediction error integration
    float influence = std::max(0.0f, predictionError_);
    current_ = baseline_ + influence;
    current_ = std::clamp(current_, -1.0f, 1.0f);
}

void CuriosityNeuromodulator::update(float dt) {
    // Curiosity increases with novelty and decays over time
    current_ = noveltyBase_ + novelty_ * 2.0f;
    current_ = std::clamp(current_, 0.0f, 1.0f);
}

// DevelopmentStage implementations
void InitialDevelopmentStage::enter() {}
void InitialDevelopmentStage::exit() {}
void InitialDevelopmentStage::update(float dt) {}

void CriticalPeriodDevelopmentStage::enter() {}
void CriticalPeriodDevelopmentStage::exit() {}
void CriticalPeriodDevelopmentStage::update(float dt) {}

void MaturationDevelopmentStage::enter() {}
void MaturationDevelopmentStage::exit() {}
void MaturationDevelopmentStage::update(float dt) {}

void AdultDevelopmentStage::enter() {}
void AdultDevelopmentStage::exit() {}
void AdultDevelopmentStage::update(float dt) {}

// CommandFactory implementations
std::unique_ptr<MotorAction> CommandFactory::createCommand(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return std::make_unique<MoveForwardAction>();
        case MotorCommand::MoveBackward: return std::make_unique<MoveBackwardAction>();
        case MotorCommand::TurnLeft: return std::make_unique<TurnLeftAction>();
        case MotorCommand::TurnRight: return std::make_unique<TurnRightAction>();
        case MotorCommand::Interact: return std::make_unique<InteractAction>();
        case MotorCommand::Wait: return std::make_unique<WaitAction>();
        case MotorCommand::LookLeft: return std::make_unique<LookLeftAction>();
        case MotorCommand::LookRight: return std::make_unique<LookRightAction>();
        default: return std::make_unique<WaitAction>();
    }
}

std::vector<MotorCommand> CommandFactory::getAllAvailableCommands() {
    return {MotorCommand::MoveForward, MotorCommand::MoveBackward,
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::Interact, MotorCommand::Wait,
            MotorCommand::LookLeft, MotorCommand::LookRight};
}

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
    // Initialize neuromodulators with strategy pattern
    neuromodulators_.push_back(std::make_unique<DopamineNeuromodulator>(0.1f));
    neuromodulators_.push_back(std::make_unique<CuriosityNeuromodulator>(0.0f));
    
    // Initialize development state machine with state pattern
    currentDevelopmentStage_ = std::make_unique<InitialDevelopmentStage>();
    currentDevelopmentStage_->enter();
    
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
                                    motorInteract_.size() + motorWait_.size() +
                                    motorInteract_.size(); // Added for LookLeft and LookRight
                        
                        switch (idx % 8) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorTurnLeft_.push_back(n); break; // LookLeft
                            case 7: motorTurnRight_.push_back(n); break; // LookRight
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

AgentBrain::~AgentBrain() {
    if (currentDevelopmentStage_) {
        currentDevelopmentStage_->exit();
    }
}

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : brain_(std::move(other.brain_))
    , neuromodulators_(std::move(other.neuromodulators_))
    , currentDevelopmentStage_(std::move(other.currentDevelopmentStage_))
    , developmentalAge_(other.developmentalAge_)
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , sensoryVision_(std::move(other.sensoryVision_))
    , sensoryTouch_(std::move(other.sensoryTouch_))
    , sensoryInternal_(std::move(other.sensoryInternal_))
    , sensoryProprioception_(std::move(other.sensoryProprioception_))
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , previousVision_(std::move(other.previousVision_))
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , commandHistory_(std::move(other.commandHistory_))
    , commandCounts_(std::move(other.commandCounts_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , plasticityModifier_(other.plasticityModifier_)
    , curiosityTriggered_(other.curiosityTriggered_)
    , currentNovelty_(other.currentNovelty_)
    , currentPredictionError_(other.currentPredictionError_) {
    other.observers_.swap(observers_);
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        if (currentDevelopmentStage_) {
            currentDevelopmentStage_->exit();
        }
        
        brain_ = std::move(other.brain_);
        neuromodulators_ = std::move(other.neuromodulators_);
        currentDevelopmentStage_ = std::move(other.currentDevelopmentStage_);
        currentDevelopmentStage_->enter(); // Enter new stage
        developmentalAge_ = other.developmentalAge_;
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        sensoryVision_ = std::move(other.sensoryVision_);
        sensoryTouch_ = std::move(other.sensoryTouch_);
        sensoryInternal_ = std::move(other.sensoryInternal_);
        sensoryProprioception_ = std::move(other.sensoryProprioception_);
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        previousVision_ = std::move(other.previousVision_);
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        commandHistory_ = std::move(other.commandHistory_);
        commandCounts_ = std::move(other.commandCounts_);
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        plasticityModifier_ = other.plasticityModifier_;
        curiosityTriggered_ = other.curiosityTriggered_;
        currentNovelty_ = other.currentNovelty_;
        currentPredictionError_ = other.currentPredictionError_;
        
        other.observers_.swap(observers_);
    }
    return *this;
}

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
    return 8; // Added for LookLeft and LookRight
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
        currentNovelty_ = noveltyLevel_;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update neuromodulators with strategy pattern
    for (auto& modulator : neuromodulators_) {
        modulator->update(0.001f); // Default timestep
    }
    
    // Update curiosity based on novelty and neuromodulators
    if (curiosityEnabled_) {
        float totalCuriosity = 0.0f;
        for (const auto& modulator : neuromodulators_) {
            if (modulator->getName() == "Curiosity") {
                totalCuriosity = modulator->getSignal();
                break;
            }
        }
        curiosityLevel_ = totalCuriosity * 2.0f + std::abs(predictionError_) * 0.5f;
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
    
    // Record command
    commandHistory_.push_back(decoded);
    commandCounts_[decoded]++;
    
    return decoded;
}

void AgentBrain::executeMotorCommand(MotorCommand cmd) {
    auto action = CommandFactory::createCommand(cmd);
    action->execute();
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
            int choice = brain_->getRandomGenerator()->uniformInt(0, 9);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
                case 8: return MotorCommand::MoveForward;
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
    currentPredictionError_ = predictionError_;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Update neuromodulators
    for (auto& modulator : neuromodulators_) {
        if (modulator->getName() == "Dopamine") {
            static_cast<DopamineNeuromodulator*>(modulator.get())->setPredictionError(predictionError_);
            break;
        }
    }
    
    // Notify observers
    notifyObservers();
    
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
    
    // Update current development stage
    currentDevelopmentStage_->update(static_cast<float>(timestep));
    
    // Check for developmental stage transition
    std::string currentStageName = currentDevelopmentStage_->getName();
    if (developmentalAge_ >= 60.0 && currentStageName == "Initial") {
        currentDevelopmentStage_->exit();
        currentDevelopmentStage_ = std::make_unique<CriticalPeriodDevelopmentStage>();
        currentDevelopmentStage_->enter();
    } else if (developmentalAge_ >= 300.0 && currentStageName == "CriticalPeriod") {
        currentDevelopmentStage_->exit();
        currentDevelopmentStage_ = std::make_unique<MaturationDevelopmentStage>();
        currentDevelopmentStage_->enter();
    } else if (developmentalAge_ >= 900.0 && currentStageName == "Maturation") {
        currentDevelopmentStage_->exit();
        currentDevelopmentStage_ = std::make_unique<AdultDevelopmentStage>();
        currentDevelopmentStage_->enter();
    }
    
    // Apply development effects
    plasticityModifier_ = currentDevelopmentStage_->getPlasticityModifier();
    
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

DevelopmentalStage AgentBrain::getCurrentDevelopmentStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

std::string AgentBrain::getDevelopmentStageName() const {
    if (currentDevelopmentStage_) {
        return currentDevelopmentStage_->getName();
    }
    return "Unknown";
}

float AgentBrain::getNeuromodulationLevel() const {
    float total = 0.0f;
    for (const auto& modulator : neuromodulators_) {
        total += modulator->getSignal();
    }
    return std::clamp(total / neuromodulators_.size(), -1.0f, 1.0f);
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

size_t AgentBrain::getCommandCount(MotorCommand cmd) const {
    auto it = commandCounts_.find(cmd);
    return it != commandCounts_.end() ? it->second : 0;
}

void AgentBrain::addObserver(std::shared_ptr<NeuromodulationObserver> observer) {
    observers_.push_back(observer);
}

void AgentBrain::removeObserver(std::shared_ptr<NeuromodulationObserver> observer) {
    observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

void AgentBrain::notifyObservers() {
    for (const auto& observer : observers_) {
        observer->onDopamineChange(dopamineLevel_);
        observer->onReward(0.5f, predictionError_); // Example values
        observer->onNovelty(noveltyLevel_);
    }
}

void AgentBrain::updateNeuromodulators(float dt) {
    for (auto& modulator : neuromodulators_) {
        modulator->update(dt);
    }
}

void AgentBrain::updateDevelopmentState(float dt) {
    currentDevelopmentStage_->update(dt);
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    curiosityTriggered_ = false;
    currentNovelty_ = 0.0f;
    currentPredictionError_ = 0.0f;
    
    // Reset neuromodulators
    for (auto& modulator : neuromodulators_) {
        if (modulator->getName() == "Dopamine") {
            static_cast<DopamineNeuromodulator*>(modulator.get())->setPredictionError(0.0f);
        } else if (modulator->getName() == "Curiosity") {
            static_cast<CuriosityNeuromodulator*>(modulator.get())->setNovelty(0.0f);
        }
    }
    
    // Reset development stage
    if (currentDevelopmentStage_) {
        currentDevelopmentStage_->exit();
        currentDevelopmentStage_ = std::make_unique<InitialDevelopmentStage>();
        currentDevelopmentStage_->enter();
    }
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear command history
    commandHistory_.clear();
    commandCounts_.clear();
}

void AgentBrain::setDopamineBaseline(float baseline) {
    for (auto& modulator : neuromodulators_) {
        if (modulator->getName() == "Dopamine") {
            static_cast<DopamineNeuromodulator*>(modulator.get())->baseline_ = baseline;
            break;
        }
    }
}

void AgentBrain::setCuriosityNoveltyBase(float base) {
    for (auto& modulator : neuromodulators_) {
        if (modulator->getName() == "Curiosity") {
            static_cast<CuriosityNeuromodulator*>(modulator.get())->noveltyBase_ = base;
            break;
        }
    }
}

void AgentBrain::addNeuromodulator(std::unique_ptr<Neuromodulator> modulator) {
    neuromodulators_.push_back(std::move(modulator));
}

std::vector<std::string> AgentBrain::getNeuromodulatorNames() const {
    std::vector<std::string> names;
    for (const auto& modulator : neuromodulators_) {
        names.push_back(modulator->getName());
    }
    return names;
}