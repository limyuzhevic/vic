#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Motor and Sensory Group Management

void AgentBrain::initializeMotorNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = getNextMotorNeuronIndex();
                    distributeMotorNeuron(n, idx);
                }
            }
        }
    }
}

void AgentBrain::initializeSensoryNeuronGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = getNextSensoryNeuronIndex();
                    distributeSensoryNeuron(n, idx);
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeuron(Neuron* neuron, size_t index) {
    switch (index % 6) {
        case 0: motorForward_.push_back(neuron); break;
        case 1: motorBackward_.push_back(neuron); break;
        case 2: motorTurnLeft_.push_back(neuron); break;
        case 3: motorTurnRight_.push_back(neuron); break;
        case 4: motorInteract_.push_back(neuron); break;
        case 5: motorWait_.push_back(neuron); break;
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron, size_t index) {
    switch (index % 4) {
        case 0: sensoryVision_.push_back(neuron); break;
        case 1: sensoryTouch_.push_back(neuron); break;
        case 2: sensoryInternal_.push_back(neuron); break;
        case 3: sensoryProprioception_.push_back(neuron); break;
    }
}

size_t AgentBrain::getNextMotorNeuronIndex() const {
    return motorForward_.size() + motorBackward_.size() + 
            motorTurnLeft_.size() + motorTurnRight_.size() +
            motorInteract_.size() + motorWait_.size();
}

size_t AgentBrain::getNextSensoryNeuronIndex() const {
    return sensoryVision_.size() + sensoryTouch_.size() +
            sensoryInternal_.size() + sensoryProprioception_.size();
}

// State Management

void AgentBrain::calculateNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = calculateVisionDifference(vision);
    noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), 1));
    noveltyLevel_ *= sensoryNoveltyDecay_;
    previousVision_ = vision;
}

void AgentBrain::calculateCuriosity(float predictionError) {
    if (!curiosityEnabled_) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::updateNeuromodulationState() {
    // This is called by applyRewardModulation()
    // State updates are handled there
}

// Helper for novelty calculation
float AgentBrain::calculateVisionDifference(const std::vector<float>& vision) {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        totalDiff += std::abs(vision[i] - previousVision_[i]);
    }
    return totalDiff;
}

// Motor Decoding Logic

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate activity in each motor group
    auto forwardAct = calculateMotorActivity(motorForward_);
    auto backwardAct = calculateMotorActivity(motorBackward_);
    auto leftAct = calculateMotorActivity(motorTurnLeft_);
    auto rightAct = calculateMotorActivity(motorTurnRight_);
    auto interactAct = calculateMotorActivity(motorInteract_);
    auto waitAct = calculateMotorActivity(motorWait_);
    
    return selectCommandWithHighestActivity(
        forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct
    );
}

float AgentBrain::calculateMotorActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::selectCommandWithHighestActivity(
    float forwardAct, float backwardAct, float leftAct, 
    float rightAct, float interactAct, float waitAct) {
    
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ <= 0.5f) {
        return defaultCmd;
    }
    
    float exploreChance = curiosityLevel_ * 0.3f;
    
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        return generateRandomMotorCommand();
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::generateRandomMotorCommand() {
    int choice = brain_->getRandomGenerator()->uniformInt(0, 10);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        case 7: return MotorCommand::LookUp;
        case 8: return MotorCommand::LookDown;
        case 9: return MotorCommand::StrafeLeft;
        case 10: return MotorCommand::StrafeRight;
        default: return MotorCommand::Wait;
    }
}

// Neuromodulation

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = calculateDopamineFromPredictionError(predictionError_);
    
    applyWeightUpdatesWithEligibilityTraces();
    updatePlasticityBasedOnDopamine(dopamineLevel_);
}

float AgentBrain::calculateDopamineFromPredictionError(float error) {
    float dopamine = error;
    dopamine = std::clamp(dopamine, -1.0f, 1.0f);
    return dopamine;
}

void AgentBrain::applyWeightUpdatesWithEligibilityTraces() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            processSynapseWithEligibilityTrace(syn);
        }
    }
}

void AgentBrain::processSynapseWithEligibilityTrace(Synapse* synapse) {
    float eligibility = synapse->getEligibilityTrace();
    
    if (std::abs(eligibility) > 0.001f) {
        float delta = eligibility * dopamineLevel_ * plasticityModifier_;
        synapse->addToWeight(delta);
        synapse->decayEligibilityTrace(0.1f);
    }
}

void AgentBrain::updatePlasticityBasedOnDopamine(float dopamine) {
    float plasticityFactor = calculatePlasticityFactor(dopamine);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

float AgentBrain::calculatePlasticityFactor(float dopamine) {
    float factor = 0.5f + 0.5f * dopamine;
    factor = std::clamp(factor, 0.1f, 2.0f);
    return factor;
}

// Development Management

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    DevelopmentalStage stage = calculateDevelopmentalStage(developmentalAge_);
    
    brain_->setDevelopmentalStage(stage);
    plasticityModifier_ = calculatePlasticityModifierForStage(stage);
    
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticity(plasticityModifier_);
    }
}

DevelopmentalStage AgentBrain::calculateDevelopmentalStage(double age) {
    if (age < 60.0) {
        return DevelopmentalStage::Initial;
    } else if (age < 300.0) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (age < 900.0) {
        return DevelopmentalStage::Maturation;
    } else {
        return DevelopmentalStage::Adult;
    }
}

float AgentBrain::calculatePlasticityModifierForStage(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;
        default:
            return 0.2f;
    }
}

void AgentBrain::updateStructuralPlasticity(float plasticityModifier) {
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    float synRate = 0.0001f * plasticityModifier;
    float pruneRate = 0.00001f * (2.0f - plasticityModifier);
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

// Reset Logic

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// State getters

float AgentBrain::getNeuromodulationLevel() const { return dopamineLevel_; }
float AgentBrain::getCuriosityLevel() const { return curiosityLevel_; }
float AgentBrain::getNoveltyLevel() const { return noveltyLevel_; }
float AgentBrain::getPredictionError() const { return predictionError_; }

// Constructor

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
    initializeMotorNeuronGroups();
    initializeSensoryNeuronGroups();
}

// Other methods

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
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    calculateNovelty(vision);
    calculateCuriosity(predictionError_);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

AgentBrain::~AgentBrain() = default;

} // namespace nlm
