#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/RandomGenerator.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>

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
    // Configuration parameters with default values
    , sensoryNoveltyDecay_(0.99f)
    , curiosityNoveltyWeight_(2.0f)
    , curiosityPredictionErrorWeight_(0.5f)
    , expectedRewardDecayFactor_(0.95f)
    , expectedRewardUpdateFactor_(0.05f)
    , plasticityFactorBase_(0.5f)
    , plasticityFactorMultiplier_(0.5f)
    , actionThreshold_(0.5f)
    , curiosityThreshold_(0.5f)
    , developmentMaturationModifier_(0.5f)
    // Debug logging
    , debugLoggingEnabled_(false)
    , lastDebugLogTime_(std::chrono::steady_clock::now())
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        // Note: MotorCommand enum now has 15 values (0-14)
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size() +
                                    motorLook_.size() + motorLookUp_.size() +
                                    motorLookDown_.size() + motorInteract_.size() +
                                    motorEat_.size() + motorDrink_.size() +
                                    motorRest_.size() + motorWait_.size() +
                                    motorCustom_.size();
                        
                        switch (idx % 15) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorLookLeft_.push_back(n); break;
                            case 5: motorLookRight_.push_back(n); break;
                            case 6: motorLook_.push_back(n); break;
                            case 7: motorLookUp_.push_back(n); break;
                            case 8: motorLookDown_.push_back(n); break;
                            case 9: motorInteract_.push_back(n); break;
                            case 10: motorEat_.push_back(n); break;
                            case 11: motorDrink_.push_back(n); break;
                            case 12: motorRest_.push_back(n); break;
                            case 13: motorWait_.push_back(n); break;
                            case 14: motorCustom_.push_back(n); break;
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
    // One motor neuron per action (including all 15 motor commands)
    return 15;
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
        curiosityLevel_ = noveltyLevel_ * curiosityNoveltyWeight_ + std::abs(predictionError_) * curiosityPredictionErrorWeight_;
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    float lookAct = calcActivity(motorLook_);
    float lookUpAct = calcActivity(motorLookUp_);
    float lookDownAct = calcActivity(motorLookDown_);
    float interactAct = calcActivity(motorInteract_);
    float eatAct = calcActivity(motorEat_);
    float drinkAct = calcActivity(motorDrink_);
    float restAct = calcActivity(motorRest_);
    float waitAct = calcActivity(motorWait_);
    float customAct = calcActivity(motorCustom_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct},
        {MotorCommand::Look, lookAct},
        {MotorCommand::LookUp, lookUpAct},
        {MotorCommand::LookDown, lookDownAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Eat, eatAct},
        {MotorCommand::Drink, drinkAct},
        {MotorCommand::Rest, restAct},
        {MotorCommand::Wait, waitAct},
        {MotorCommand::Custom, customAct}
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
    if (bestActivity < actionThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > curiosityThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - ensure range matches MotorCommand enum (0-14)
            int choice = brain_->getRandomGenerator()->uniformInt(0, 14);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Look;
                case 7: return MotorCommand::LookUp;
                case 8: return MotorCommand::LookDown;
                case 9: return MotorCommand::Interact;
                case 10: return MotorCommand::Eat;
                case 11: return MotorCommand::Drink;
                case 12: return MotorCommand::Rest;
                case 13: return MotorCommand::Wait;
                case 14: return MotorCommand::Custom;
                default: return MotorCommand::Wait;  // Safety fallback
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
    expectedReward_ = expectedRewardDecayFactor_ * expectedReward_ + expectedRewardUpdateFactor_ * reward;
    
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
    float plasticityFactor = plasticityFactorBase_ + plasticityFactorMultiplier_ * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
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
        plasticityModifier_ = developmentMaturationModifier_;  // Configurable maturation modifier
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

#include <sstream>

namespace nlm {

// ... (rest of existing code)

void AgentBrain::setDevelopmentStage(DevelopmentalStage stage) {
    if (brain_) brain_->setDevelopmentalStage(stage);
}

void AgentBrain::setPlasticityModifier(float modifier) {
    plasticityModifier_ = modifier;
    if (structuralPlasticityEnabled_ && brain_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * modifier;
            float pruneRate = 0.00001f * (2.0f - modifier);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AgentBrain::setNeuromodulationLevel(float level) {
    dopamineLevel_ = std::clamp(level, -1.0f, 1.0f);
}

// Get brain statistics
float AgentBrain::getAverageFiringRate() const {
    if (!brain_) return 0.0f;
    return brain_->getAverageFiringRate();
}

float AgentBrain::getExcitationInhibitionRatio() const {
    if (!brain_) return 0.0f;
    return brain_->getExcitationInhibitionRatio();
}

float AgentBrain::getDevelopmentProgress() const {
    // Simple progress based on developmental age (0-100%)
    const float maxAge = 900.0f; // Maturation stage
    return std::min(developmentalAge_ / maxAge * 100.0f, 100.0f);
}

// Apply common neuromodulation presets
void AgentBrain::applyCuriosityBoost() {
    setCuriosityLevel(1.0f);
}

void AgentBrain::applyExplorationMode() {
    enableCuriosity(true);
    setCuriosityThreshold(0.2f);
    setActionThreshold(0.3f);
}

void AgentBrain::applyExploitationMode() {
    enableCuriosity(false);
    setCuriosityThreshold(0.5f);
    setActionThreshold(0.7f);
}

void AgentBrain::applyDevelopmentAcceleration() {
    enableDevelopment(true);
    setDevelopmentMaturationModifier(0.8f);
    setPlasticityFactorBase(0.6f);
}

void AgentBrain::applyMaturationDeceleration() {
    enableDevelopment(true);
    setDevelopmentMaturationModifier(0.3f);
    setPlasticityFactorBase(0.4f);
}

// Get configuration summary
std::string AgentBrain::getConfigurationSummary() const {
    std::ostringstream oss;
    oss << "=== NLM AgentBrain Configuration ===\n";
    oss << "Reward Modulation: " << (rewardModulationEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Structural Plasticity: " << (structuralPlasticityEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Development: " << (developmentEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "Curiosity: " << (curiosityEnabled_ ? "Enabled" : "Disabled") << "\n";
    oss << "\nParameters:\n";
    oss << "  Sensory Novelty Decay: " << sensoryNoveltyDecay_ << "\n";
    oss << "  Curiosity Novelty Weight: " << curiosityNoveltyWeight_ << "\n";
    oss << "  Curiosity Prediction Error Weight: " << curiosityPredictionErrorWeight_ << "\n";
    oss << "  Expected Reward Decay Factor: " << expectedRewardDecayFactor_ << "\n";
    oss << "  Expected Reward Update Factor: " << expectedRewardUpdateFactor_ << "\n";
    oss << "  Plasticity Factor Base: " << plasticityFactorBase_ << "\n";
    oss << "  Plasticity Factor Multiplier: " << plasticityFactorMultiplier_ << "\n";
    oss << "  Action Threshold: " << actionThreshold_ << "\n";
    oss << "  Curiosity Threshold: " << curiosityThreshold_ << "\n";
    oss << "  Development Maturation Modifier: " << developmentMaturationModifier_ << "\n";
    oss << "\nCurrent State:\n";
    oss << "  Development Progress: " << getDevelopmentProgress() << "%\n";
    oss << "  Neuromodulation Level: " << getNeuromodulationLevel() << "\n";
    oss << "  Curiosity Level: " << getCuriosityLevel() << "\n";
    oss << "  Novelty Level: " << getNoveltyLevel() << "\n";
    oss << "  Prediction Error: " << getPredictionError() << "\n";
    oss << "  Average Firing Rate: " << getAverageFiringRate() << " Hz\n";
    return oss.str();
}

// ... (rest of existing code)

} // namespace nlm
