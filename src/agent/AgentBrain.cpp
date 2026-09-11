#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdexcept>

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
    , initialized_(false)
    , lastDebugStep_(0)
{
    // Initialize motor and sensory neuron groups with validation
    if (brain_) {
        validateBrainStructure();
        populateNeuronGroups();
    } else {
        throw std::invalid_argument("AgentBrain: Brain pointer cannot be null");
    }
}

void AgentBrain::validateBrainStructure() {
    const auto& regions = brain_->getRegions();
    if (regions.empty()) {
        NLM_LOG_WARNING("AgentBrain: Brain has no regions - neural network will be empty");
    }
    
    // Validate that we can access regions and populations
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("AgentBrain: Invalid region found in brain structure");
            continue;
        }
    }
}

void AgentBrain::populateNeuronGroups() {
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Initialize neuron groups with error handling
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            if (!region) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARNING("AgentBrain: Invalid population found");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                try {
                    distributeNeurons(pop.get(), type);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("AgentBrain: Failed to distribute neurons for type " + std::to_string(static_cast<int>(type)) + ": " + e.what());
                }
            }
        }
        
        NLM_LOG_INFO("AgentBrain: Populated neuron groups - Motor: " + 
                     std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() + 
                                   motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) +
                     " neurons, Sensory: " +
                     std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                                   sensoryInternal_.size() + sensoryProprioception_.size()) + " neurons");
    }
}

void AgentBrain::distributeNeurons(NeuralPopulation* pop, NeuronType type) {
    if (!pop) throw std::invalid_argument("NeuralPopulation cannot be null");
    
    const auto& neurons = pop->getNeurons();
    if (neurons.empty()) {
        NLM_LOG_WARNING("AgentBrain: Population has no neurons");
        return;
    }
    
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("AgentBrain: Invalid neuron pointer found");
            continue;
        }
        
        // Set neuron type to match population
        n->setType(type);
        
        // Distribute motor neurons to different action groups
        if (type == NeuronType::Motor) {
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
        } else if (type == NeuronType::Sensory) {
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

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        if (!brain_) {
            throw std::runtime_error("AgentBrain::initialize: Brain pointer is null");
        }
        
        // Validate world dimensions
        int visionWidth = world.getVisionWidth();
        int visionHeight = world.getVisionHeight();
        if (visionWidth <= 0 || visionHeight <= 0) {
            throw std::invalid_argument("AgentBrain::initialize: Invalid world dimensions");
        }
        
        previousVision_.resize(static_cast<size_t>(visionWidth) * visionHeight, 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        initialized_ = true;
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
        lastDebugStep_ = 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::initialize failed: " + std::string(e.what()));
        throw;
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;  // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_ || !initialized_) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Brain not initialized");
        return;
    }
    
    try {
        processVisionInput(percept);
        processTouchInput(percept);
        processInternalInput(percept);
        processProprioceptiveInput(percept);
        computeNoveltyAndCuriosity(percept);
        
        // Update debug counter
        lastDebugStep_++;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput exception: " + std::string(e.what()));
        // Continue with partial processing rather than failing completely
    }
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (sensoryVision_.empty()) return;
    
    // Ensure we don't exceed array bounds
    size_t minSize = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryVision_[i]) {
            // Validate neuron before using it
            if (sensoryVision_[i]->getState().firingState != FiringState::Inactive) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                
                // Clamp current to reasonable range
                current = std::clamp(current, -100.0f, 100.0f);
                
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    if (sensoryTouch_.empty()) return;
    
    size_t minSize = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    if (sensoryInternal_.empty()) return;
    
    size_t minSize = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptiveInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    if (sensoryProprioception_.empty()) return;
    
    size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            current = std::clamp(current, -100.0f, 100.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNoveltyAndCuriosity(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    
    // Compute novelty (difference from previous vision)
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(vision.size(), 1));
    
    // Decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    // Store for next time
    if (!vision.empty() && vision.size() <= previousVision_.size()) {
        std::copy(vision.begin(), vision.end(), previousVision_.begin());
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
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
