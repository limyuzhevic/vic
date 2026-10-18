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
    // Initialize the three managers
    neuromodulationManager_.connectBrain(brain_);
    
    // Initialize neuron groups and pass them to managers
    initializeMotorGroups();
    initializeSensoryGroups();
    
    // Connect managers with appropriate parameters
    // Note: CuriosityManager and NeuromodulationManager have default parameters
    // that match the original implementation
    
    NLM_LOG_INFO("AgentBrain: Constructed with 3 manager components");
}

AgentBrain::~AgentBrain() {
    NLM_LOG_INFO("AgentBrain: Destructed");
}

void AgentBrain::initializeMotorGroups() {
    if (!brain_) return;
    
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
            }
        }
    }
    
    // Pass motor groups to MotorDecoder
    motorDecoder_.setMotorGroups(
        motorForward_, motorBackward_, motorTurnLeft_, motorTurnRight_,
        motorInteract_, motorWait_
    );
    
    NLM_LOG_INFO("AgentBrain: Initialized " + std::to_string(motorForward_.size()) + 
                 " forward, " + std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait motor neurons");
}

void AgentBrain::initializeSensoryGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
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

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    curiosityManager_.setPreviousVision(previousVision_);
    neuromodulationManager_.reset();
    
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
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
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
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Update curiosity manager with novelty
        float curiosity = curiosityManager_.computeCuriosity(noveltyLevel_, predictionError_);
        curiosityLevel_ = curiosityManager_.clampCuriosity(curiosity);
        
        // Store for next time
        previousVision_ = vision;
        curiosityManager_.setPreviousVision(previousVision_);
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
    return motorDecoder_.decodeMotorCommand();
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) return defaultCmd;
    
    // Check if exploration should be triggered based on curiosity
    if (curiosityManager_.shouldExplore(curiosityLevel_, 0.5f)) {
        // Get exploration chance based on curiosity level
        float exploreChance = curiosityLevel_ * 0.3f;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Use curiosity manager to select random motor command
            return curiosityManager_.selectRandomMotorCommand(*brain_->getRandomGenerator());
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    neuromodulationManager_.applyRewardModulation(reward, predictedReward);
    
    // Update local state from neuromodulation manager
    predictionError_ = neuromodulationManager_.getPredictionError();
    expectedReward_ = neuromodulationManager_.getExpectedReward();
    dopamineLevel_ = neuromodulationManager_.getDopamineLevel();
    plasticityModifier_ = neuromodulationManager_.getPlasticityModifier();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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
    
    // Reset managers
    motorDecoder_.reset();
    curiosityManager_.reset();
    neuromodulationManager_.reset();
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain: Reset completed");
}

// ========== ADVANCED AGENT CONTROL ==========

void AgentBrain::setExplorationMode(const std::string& mode) {
    // Set exploration strategy based on mode
    NLM_LOG_DEBUG("Setting exploration mode: " + mode);
    
    if (mode == "random") {
        // Random exploration strategy
        curiosityManager_.setExplorationMode(ExplorationMode::Random);
    } else if (mode == "epsilon_greedy") {
        // Epsilon-greedy exploration strategy
        curiosityManager_.setExplorationMode(ExplorationMode::EpsilonGreedy);
    } else if (mode == "ucb") {
        // UCB (Upper Confidence Bound) exploration strategy
        curiosityManager_.setExplorationMode(ExplorationMode::UCB);
    } else if (mode == "thompson") {
        // Thompson sampling exploration strategy
        curiosityManager_.setExplorationMode(ExplorationMode::ThompsonSampling);
    } else {
        // Default to epsilon-greedy
        curiosityManager_.setExplorationMode(ExplorationMode::EpsilonGreedy);
        NLM_LOG_WARNING("Unknown exploration mode: " + mode + ". Defaulting to epsilon_greedy");
    }
}

std::vector<std::pair<MotorCommand, int>> AgentBrain::getActionSelectionStats() const {
    std::vector<std::pair<MotorCommand, int>> stats;
    
    // Get action selection statistics from motor decoder
    stats = motorDecoder_.getActionSelectionStats();
    
    return stats;
}

void AgentBrain::setCuriosityStrategy(const std::string& strategy) {
    // Set curiosity calculation strategy
    NLM_LOG_DEBUG("Setting curiosity strategy: " + strategy);
    
    if (strategy == "noveltys_based") {
        // Novelty-based curiosity
        curiosityManager_.setCuriosityStrategy(CuriosityStrategy::NoveltyBased);
    } else if (strategy == "pred_error_based") {
        // Prediction error-based curiosity
        curiosityManager_.setCuriosityStrategy(CuriosityStrategy::PredErrorBased);
    } else if (strategy == "combined") {
        // Combined curiosity strategy
        curiosityManager_.setCuriosityStrategy(CuriosityStrategy::Combined);
    } else if (strategy == "intrinsic") {
        // Intrinsic motivation curiosity
        curiosityManager_.setCuriosityStrategy(CuriosityStrategy::Intrinsic);
    } else {
        // Default to novelty-based
        curiosityManager_.setCuriosityStrategy(CuriosityStrategy::NoveltyBased);
        NLM_LOG_WARNING("Unknown curiosity strategy: " + strategy + ". Defaulting to noveltys_based");
    }
}

std::vector<MotorCommand> AgentBrain::getBehaviorHistory() const {
    std::vector<MotorCommand> history;
    
    // Get behavior history from motor decoder
    history = motorDecoder_.getBehaviorHistory();
    
    return history;
}

void AgentBrain::enableLearningMode(bool enable) {
    // Enable/disable learning mechanisms
    NLM_LOG_DEBUG("Learning mode " + std::string(enable ? "enabled" : "disabled"));
    
    if (enable) {
        // Enable learning mechanisms
        curiosityManager_.enableLearning(true);
        neuromodulationManager_.enableLearning(true);
        // Enable structural plasticity if already enabled
        if (structuralPlasticityEnabled_) {
            if (brain_ && brain_->getStructuralPlasticity()) {
                brain_->getStructuralPlasticity()->enableLearning(true);
            }
        }
    } else {
        // Disable learning mechanisms
        curiosityManager_.enableLearning(false);
        neuromodulationManager_.enableLearning(false);
        // Disable structural plasticity if currently enabled
        if (structuralPlasticityEnabled_) {
            if (brain_ && brain_->getStructuralPlasticity()) {
                brain_->getStructuralPlasticity()->enableLearning(false);
            }
        }
    }
}

} // namespace nlm
