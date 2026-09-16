#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

constexpr float kVisionScaleFactor = 5.0f;
constexpr float kTouchScaleFactor = 8.0f;
constexpr float kInternalScaleFactor = 5.0f;
constexpr float kProprioceptionScaleFactor = 3.0f;

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain created with null brain pointer");
        return;
    }
    
    try {
        initializeNeuronGroups();
        logInitializationStats();
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("AgentBrain initialization failed: " + std::string(e.what()));
        brain_.reset();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain with null brain");
        return;
    }
    
    try {
        // Validate world parameters
        if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
            NLM_LOG_WARNING("Invalid world vision dimensions, using defaults");
            previousVision_.clear();
            developmentalAge_ = 0.0;
            plasticityModifier_ = 1.0f;
            return;
        }
        
        // Calculate vision buffer size with bounds checking
        size_t visionSize = static_cast<size_t>(world.getVisionWidth() * world.getVisionHeight());
        if (visionSize > 1000000) { // Reasonable upper limit
            NLM_LOG_WARNING("Large vision buffer size (" + std::to_string(visionSize) + "), may impact performance");
        }
        
        previousVision_.resize(visionSize, 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Log initialization success with detailed stats
        logInitializationStats();
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Exception during AgentBrain initialization: " + std::string(e.what()));
        // Reset to safe state
        previousVision_.clear();
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
    }
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
    if (!brain_) {
        NLM_LOG_WARNING("Cannot process sensory input: null brain");
        return;
    }
    
    try {
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        // Validate sensory data dimensions
        if (vision.empty() && touch.empty() && intern.empty() && proprio.empty()) {
            NLM_LOG_WARNING("Empty sensory percept received");
            return;
        }
        
        // Process vision input
        processVisionInput(vision);
        
        // Process touch input
        processTouchInput(touch);
        
        // Process internal signals
        processInternalInput(intern);
        
        // Process proprioception
        processProprioceptionInput(proprio);
        
        // Compute novelty
        computeNovelty(vision);
        
        // Update curiosity
        updateCuriosity();
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Exception in processSensoryInput: " + std::string(e.what()));
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

void AgentBrain::initializeNeuronGroups() {
    const auto& regions = brain_->getRegions();
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("Skipping null region in AgentBrain initialization");
            continue;
        }
        const auto& populations = region->getPopulations();
        for (const auto& pop : populations) {
            if (!pop) {
                NLM_LOG_WARNING("Skipping null population in AgentBrain initialization");
                continue;
            }
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (const Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_WARNING("Skipping null neuron in motor population");
                        continue;
                    }
                    // Distribute motor neurons to different action groups
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                 motorTurnLeft_.size() + motorTurnRight_.size() +
                                 motorInteract_.size() + motorWait_.size();
                    
                    switch (idx % 6) {
                        case 0: motorForward_.push_back(const_cast<Neuron*>(n)); break;
                        case 1: motorBackward_.push_back(const_cast<Neuron*>(n)); break;
                        case 2: motorTurnLeft_.push_back(const_cast<Neuron*>(n)); break;
                        case 3: motorTurnRight_.push_back(const_cast<Neuron*>(n)); break;
                        case 4: motorInteract_.push_back(const_cast<Neuron*>(n)); break;
                        case 5: motorWait_.push_back(const_cast<Neuron*>(n)); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (const Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_WARNING("Skipping null neuron in sensory population");
                        continue;
                    }
                    // Distribute sensory neurons
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                 sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(const_cast<Neuron*>(n)); break;
                        case 1: sensoryTouch_.push_back(const_cast<Neuron*>(n)); break;
                        case 2: sensoryInternal_.push_back(const_cast<Neuron*>(n)); break;
                        case 3: sensoryProprioception_.push_back(const_cast<Neuron*>(n)); break;
                    }
                }
            }
        }
    }
}

void AgentBrain::logInitializationStats() const {
    size_t totalMotor = motorForward_.size() + motorBackward_.size() +
                       motorTurnLeft_.size() + motorTurnRight_.size() +
                       motorInteract_.size() + motorWait_.size();
    size_t totalSensory = sensoryVision_.size() + sensoryTouch_.size() +
                         sensoryInternal_.size() + sensoryProprioception_.size();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(totalMotor) + " motor neurons, " +
                 std::to_string(totalSensory) + " sensory neurons, " +
                 std::to_string(previousVision_.size()) + " vision cells");
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (!sensoryVision_[i]) continue;
        
        float current = vision[i] * kVisionScaleFactor;
        current = std::max(-100.0f, std::min(100.0f, current));
        sensoryVision_[i]->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (!sensoryTouch_[i]) continue;
        
        float current = touch[i] * kTouchScaleFactor;
        current = std::max(-50.0f, std::min(50.0f, current));
        sensoryTouch_[i]->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (!sensoryInternal_[i]) continue;
        
        float current = (intern[i] * 2.0f - 1.0f) * kInternalScaleFactor;
        current = std::max(-25.0f, std::min(25.0f, current));
        sensoryInternal_[i]->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (!sensoryProprioception_[i]) continue;
        
        float current = (proprio[i] * 2.0f - 1.0f) * kProprioceptionScaleFactor;
        current = std::max(-15.0f, std::min(15.0f, current));
        sensoryProprioception_[i]->injectCurrent(current);
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.empty()) return;
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(vision.size(), previousVision_.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        diff = std::min(diff, 1.0f);
        totalDiff += diff;
    }
    
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    
    previousVision_ = vision;
    
    if (noveltyLevel_ > 0.5f) {
        NLM_LOG_DEBUG("High novelty detected: " + std::to_string(noveltyLevel_));
    }
}

void AgentBrain::updateCuriosity() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

} // namespace nlm
