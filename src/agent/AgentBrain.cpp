#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric> // For accumulate

namespace nlm {

// Helper functions implementation
void AgentBrain::distributeNeuronsToGroups() {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = getMotorGroupIndex();
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
                    size_t idx = getSensoryGroupIndex();
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

size_t AgentBrain::getMotorGroupIndex() const {
    return motorForward_.size() + motorBackward_.size() + 
            motorTurnLeft_.size() + motorTurnRight_.size() +
            motorInteract_.size() + motorWait_.size();
}

size_t AgentBrain::getSensoryGroupIndex() const {
    return sensoryVision_.size() + sensoryTouch_.size() +
            sensoryInternal_.size() + sensoryProprioception_.size();
}

// Helper function to calculate average neuron activity
float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// Helper function to compute novelty
void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
}

// Helper function to update curiosity level
void AgentBrain::updateCuriosityLevel() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

// Helper function to find motor command with maximum activity
MotorCommand AgentBrain::findBestMotorCommand(float forwardAct, float backwardAct, float leftAct, 
                                      float rightAct, float interactAct, float waitAct) const {
    
    struct MotorCommandInfo {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandInfo commands[] = {
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
    
    return best;
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
    , visionScale_(5.0f)
    , touchScale_(8.0f)
    , internalScale_(5.0f)
    , proprioceptionScale_(3.0f)
    , activityThreshold_(0.5f)
    , curiosityThreshold_(0.3f)
    , explorationChance_(0.3f)
{
    if (brain_) {
        distributeNeuronsToGroups();
    }
}

AgentBrain::~AgentBrain() = default;

// Configuration method implementations
void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
bool AgentBrain::isDevelopmentEnabled() const { return developmentEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

// Configuration parameter setters
void AgentBrain::setVisionScale(float scale) { visionScale_ = scale; }
void AgentBrain::setTouchScale(float scale) { touchScale_ = scale; }
void AgentBrain::setInternalScale(float scale) { internalScale_ = scale; }
void AgentBrain::setProprioceptionScale(float scale) { proprioceptionScale_ = scale; }
void AgentBrain::setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
void AgentBrain::setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
void AgentBrain::setExplorationChance(float chance) { explorationChance_ = chance; }

// Configuration parameter getters
float AgentBrain::getVisionScale() const { return visionScale_; }
float AgentBrain::getTouchScale() const { return touchScale_; }
float AgentBrain::getInternalScale() const { return internalScale_; }
float AgentBrain::getProprioceptionScale() const { return proprioceptionScale_; }
float AgentBrain::getActivityThreshold() const { return activityThreshold_; }
float AgentBrain::getCuriosityThreshold() const { return curiosityThreshold_; }
float AgentBrain::getExplorationChance() const { return explorationChance_; }

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Update configuration
void AgentBrain::enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
void AgentBrain::enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
void AgentBrain::enableDevelopment(bool enable) { developmentEnabled_ = enable; }
void AgentBrain::enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

bool AgentBrain::isRewardModulationEnabled() const { return rewardModulationEnabled_; }
bool AgentBrain::isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
bool AgentBrain::isDevelopmentEnabled() const { return developmentEnabled_; }
bool AgentBrain::isCuriosityEnabled() const { return curiosityEnabled_; }

// Configuration parameter setters
void AgentBrain::setVisionScale(float scale) { visionScale_ = scale; }
void AgentBrain::setTouchScale(float scale) { touchScale_ = scale; }
void AgentBrain::setInternalScale(float scale) { internalScale_ = scale; }
void AgentBrain::setProprioceptionScale(float scale) { proprioceptionScale_ = scale; }
void AgentBrain::setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
void AgentBrain::setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
void AgentBrain::setExplorationChance(float chance) { explorationChance_ = chance; }

// Configuration parameter getters
float AgentBrain::getVisionScale() const { return visionScale_; }
float AgentBrain::getTouchScale() const { return touchScale_; }
float AgentBrain::getInternalScale() const { return internalScale_; }
float AgentBrain::getProprioceptionScale() const { return proprioceptionScale_; }
float AgentBrain::getActivityThreshold() const { return activityThreshold_; }
float AgentBrain::getCuriosityThreshold() const { return curiosityThreshold_; }
float AgentBrain::getExplorationChance() const { return explorationChance_; }

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
    
    // Process vision input
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            float current = vision[i] * visionScale_;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Process touch input
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * touchScale_;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Process internal signals
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * internalScale_;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * proprioceptionScale_;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty
    computeNovelty(vision);
    
    // Update curiosity based on novelty
    updateCuriosityLevel();
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
    MotorCommand best = findBestMotorCommand(forwardAct, backwardAct, leftAct, 
                                            rightAct, interactAct, waitAct);
    
    // Only act if there's meaningful activity
    if (bestActivity < activityThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationChance_;
        
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
