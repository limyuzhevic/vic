#include "AgentBrainCore.hpp"
#include "AgentBrainSensory.hpp"
#include "AgentBrainMotor.hpp"
#include "AgentBrainLearning.hpp"
#include "AgentBrainDevelopment.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrainCore::AgentBrainCore(std::shared_ptr<Brain> brain)
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
    // Initialize components
    sensory_ = std::make_unique<AgentBrainSensory>(this);
    motor_ = std::make_unique<AgentBrainMotor>(this);
    learning_ = std::make_unique<AgentBrainLearning>(this);
    development_ = std::make_unique<AgentBrainDevelopment>(this);
    
    // Distribute neurons
    if (brain_) {
        motor_->distributeMotorNeurons(brain_.get());
        sensory_->distributeSensoryNeurons(brain_.get());
    }
}

AgentBrainCore::~AgentBrainCore() = default;

void AgentBrainCore::initialize(const SimpleWorld& world) {
    // Initialize sensory processing
    sensory_->initializeSensoryProcessing();
    development_->reset();
    
    NLM_LOG_INFO("AgentBrain initialized with vision sensory neurons, " +
                 std::to_string(sensory_->getTouch().size()) + " touch sensory neurons, " +
                 std::to_string(sensory_->getInternal().size()) + " internal sensory neurons");
}

size_t AgentBrainCore::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrainCore::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrainCore::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Extract sensory data
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    const auto& audio = percept.getAudio();
    
    // Update last vision for thought trace
    setLastVision(vision);
    
    // Process sensory inputs
    sensory_->processVisionInput(vision);
    sensory_->processTouchInput(touch);
    sensory_->processInternalInput(internal);
    sensory_->processProprioceptionInput(proprio);
    
    // Update thought percept (simplified for now)
    float visionMean = 0.0f;
    for (float v : vision) visionMean += v;
    if (!vision.empty()) visionMean /= static_cast<float>(vision.size());
    setThoughtPercept("visionMean=" + std::to_string(visionMean));
    
    // Update prediction and novelty
    sensory_->updatePrediction(vision);
    sensory_->computeNovelty(vision);
    
    // Apply winner-take-all lateral inhibition
    sensory_->applyWinnerTakeAllInhibition(vision);
    
    // Update curiosity
    learning_->updatePredictionError(0.0f, 0.0f);  // Will be updated by applyRewardModulation
    sensory_->updateCuriosity(0.0f);  // Will be updated by applyRewardModulation
}

MotorCommand AgentBrainCore::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Get motor group activities
    auto activities = motor_->getMotorGroupActivities();
    
    // Get planner suggestion (advisory)
    if (brain_->getPlanner() && !lastVision_.empty()) {
        ActionType planned = brain_->getPlanner()->planAction(lastVision_);
        switch (planned) {
            case ActionType::MoveForward: setThoughtPlanned(MotorCommand::MoveForward); break;
            case ActionType::MoveBackward: setThoughtPlanned(MotorCommand::MoveBackward); break;
            case ActionType::TurnLeft: setThoughtPlanned(MotorCommand::TurnLeft); break;
            case ActionType::TurnRight: setThoughtPlanned(MotorCommand::TurnRight); break;
            case ActionType::Interact: setThoughtPlanned(MotorCommand::Interact); break;
            default: setThoughtPlanned(MotorCommand::Wait); break;
        }
        setHasThoughtPlanned(true);
    }
    
    // Compute motor command with all biases
    MotorCommand decoded = motor_->computeMotorCommandWithBiases(activities);
    
    // Update winner-take-all state
    motor_->updateWinnerTakeAllState(activities);
    
    // Apply curiosity-based exploration
    decoded = motor_->applyCuriosity(decoded);
    
    // Apply diversity counters
    motor_->applyDiversityCounters(decoded);
    
    setLastDecoded(decoded);
    setHasLastDecoded(true);
    
    return decoded;
}

void AgentBrainCore::applyRewardModulation(float reward, float predictedReward) {
    learning_->applyRewardModulation(reward, predictedReward);
    learning_->updatePredictionError(reward, predictedReward);
    learning_->updateExpectedReward(reward);
    learning_->updateDopamineLevel();
}

void AgentBrainCore::updateDevelopment(double timestep) {
    development_->updateDevelopment(timestep);
    learning_->updateDevelopment(timestep);
}

DevelopmentalStage AgentBrainCore::getDevelopmentalStage() const {
    return development_->getDevelopmentalStage();
}

float AgentBrainCore::getNeuromodulationLevel() const {
    return learning_->getDopamineLevel();
}

float AgentBrainCore::getCuriosityLevel() const {
    return sensory_->getCuriosityLevel();
}

float AgentBrainCore::getNoveltyLevel() const {
    return sensory_->getNoveltyLevel();
}

float AgentBrainCore::getPredictionError() const {
    return learning_->getPredictionError();
}

std::string AgentBrainCore::getThoughtTrace() const {
    std::string out = "perceived=[" + thoughtPercept_ + "] -> planned=[";
    out += hasThoughtPlanned_ ? phase5CmdName(thoughtPlanned_) : "none";
    out += "] -> acted=[" + std::string(phase5CmdName(lastDecoded_)) + "] -> outcome=[";
    out += hasThoughtOutcome_ ? ("reward=" + std::to_string(thoughtReward_)) : "none";
    out += "] novelty=" + std::to_string(noveltyLevel_) +
           " curiosity=" + std::to_string(curiosityLevel_);
    return out;
}

float AgentBrainCore::getLearnedPreference() const {
    return brain_ ? brain_->getLearnedPreference() : 0.0f;
}

void AgentBrainCore::reset() {
    learning_->setDopamineLevel(0.0f);
    sensory_->setNoveltyLevel(0.0f);
    sensory_->setCuriosityLevel(0.0f);
    learning_->setPredictionError(0.0f);
    learning_->setExpectedReward(0.0f);
    development_->reset();
    
    // Clear visual prediction
    predictedVision_.clear();
    maxTouch_ = 0.0f;
    forwardTouch_ = 0.0f;
    forwardObjectVision_ = 1.0f;
    lastVision_.clear();
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    hasLastDecoded_ = false;
    hasThoughtPlanned_ = false;
    hasThoughtOutcome_ = false;
}

Brain* AgentBrainCore::getBrain() {
    return brain_.get();
}

// Helper function for thought trace naming
namespace {
const char* phase5CmdName(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        default: return "Wait";
    }
}
}  // namespace

} // namespace nlm