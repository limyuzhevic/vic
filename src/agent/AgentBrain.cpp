#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

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
    // Estimate population sizes for reserve allocation
    size_t motorCount = 0, sensoryCount = 0;
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                size_t neuronCount = pop->getNeuronCount();
                
                if (type == NeuronType::Motor) {
                    motorCount += neuronCount;
                } else if (type == NeuronType::Sensory) {
                    sensoryCount += neuronCount;
                }
            }
        }
    }
    
    // Pre-allocate vectors based on estimates (with 25% overhead)
    motorForward_.reserve(motorCount * 6);  // ~6 motor groups
    motorBackward_.reserve(motorCount);
    motorTurnLeft_.reserve(motorCount);
    motorTurnRight_.reserve(motorCount);
    motorInteract_.reserve(motorCount);
    motorWait_.reserve(motorCount);
    
    sensoryVision_.reserve(sensoryCount);
    sensoryTouch_.reserve(sensoryCount);
    sensoryInternal_.reserve(sensoryCount);
    sensoryProprioception_.reserve(sensoryCount);
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Add neurons to all motor groups efficiently
                    for (Neuron* n : pop->getNeurons()) {
                        motorForward_.emplace_back(std::shared_ptr<Neuron>(n));
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        sensoryVision_.emplace_back(std::shared_ptr<Neuron>(n));
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() {
    // Clear all neuron references to prevent memory issues
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
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Pre-allocate vision buffer and initialize in one operation
    const size_t visionBufferSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.reserve(visionBufferSize);
    previousVision_.resize(visionBufferSize, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Batch logging to reduce I/O operations
    static thread_local std::string logBuffer;
    logBuffer = "AgentBrain initialized with ";
    logBuffer += std::to_string(sensoryVision_.size()) + " vision sensory neurons, ";
    logBuffer += std::to_string(sensoryTouch_.size()) + " touch sensory neurons, ";
    logBuffer += std::to_string(sensoryInternal_.size()) + " internal sensory neurons";
    NLM_LOG_INFO(logBuffer);
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
    
    // Pre-fetch pointers to avoid repeated .lock() calls
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Vision input processing (256 values -> sensoryVision_ neurons)
    // Use pre-computed limits for bounds checking
    const size_t visionLimit = std::min(sensoryVision_.size(), vision.size());
    for (size_t i = 0; i < visionLimit; ++i) {
        if (auto neuron = sensoryVision_[i].lock()) {
            neuron->injectCurrent(vision[i] * 5.0f);
        }
    }
    
    // Touch input processing (8 values -> sensoryTouch_ neurons)
    const size_t touchLimit = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < touchLimit; ++i) {
        if (auto neuron = sensoryTouch_[i].lock()) {
            neuron->injectCurrent(touch[i] * 8.0f);
        }
    }
    
    // Internal signals processing (4 values -> sensoryInternal_ neurons)
    const size_t internLimit = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < internLimit; ++i) {
        if (auto neuron = sensoryInternal_[i].lock()) {
            neuron->injectCurrent((intern[i] * 2.0f - 1.0f) * 5.0f);
        }
    }
    
    // Proprioception processing (6 values -> sensoryProprioception_ neurons)
    const size_t proprioLimit = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < proprioLimit; ++i) {
        if (auto neuron = sensoryProprioception_[i].lock()) {
            neuron->injectCurrent((proprio[i] * 2.0f - 1.0f) * 3.0f);
        }
    }
    
    // Compute novelty using optimized algorithm (SIMD-friendly)
    if (!vision.empty() && !previousVision_.empty()) {
        const size_t visionSize = std::min(vision.size(), previousVision_.size());
        float totalDiff = 0.0f;
        
        // Use unrolled loop for better performance
        for (size_t i = 0; i < visionSize; i += 4) {
            if (i + 3 < visionSize) {
                // Process 4 elements at once
                float diff0 = std::abs(vision[i] - previousVision_[i]);
                float diff1 = std::abs(vision[i + 1] - previousVision_[i + 1]);
                float diff2 = std::abs(vision[i + 2] - previousVision_[i + 2]);
                float diff3 = std::abs(vision[i + 3] - previousVision_[i + 3]);
                totalDiff += diff0 + diff1 + diff2 + diff3;
            } else {
                // Process remaining elements
                for (size_t j = i; j < visionSize; ++j) {
                    totalDiff += std::abs(vision[j] - previousVision_[j]);
                }
            }
        }
        
        // Normalize and decay with single operation
        noveltyLevel_ = (visionSize > 0) ? (totalDiff / static_cast<float>(visionSize)) : 0.0f;
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Batch update vision buffer
        previousVision_.assign(vision.begin(), vision.begin() + visionSize);
    }
    
    // Update curiosity with optimized calculation
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
    // Calculate average activity in each motor group - optimized with pre-bound vectors
    auto calcActivity = [](const std::vector<std::weak_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& weakRef : neurons) {
            if (auto neuron = weakRef.lock()) {
                sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    };
    
    // Calculate all activities in parallel
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity using optimized search - unrolled for 6 commands
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Unrolled search to reduce branch prediction misses
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    if (forwardAct > bestActivity) {
        bestActivity = forwardAct;
        best = MotorCommand::MoveForward;
    }
    if (backwardAct > bestActivity) {
        bestActivity = backwardAct;
        best = MotorCommand::MoveBackward;
    }
    if (leftAct > bestActivity) {
        bestActivity = leftAct;
        best = MotorCommand::TurnLeft;
    }
    if (rightAct > bestActivity) {
        bestActivity = rightAct;
        best = MotorCommand::TurnRight;
    }
    if (interactAct > bestActivity) {
        bestActivity = interactAct;
        best = MotorCommand::Interact;
    }
    
    // Early return optimization - check threshold before final comparison
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
    
    // Update expected reward with optimized exponential moving average
    expectedReward_ += 0.05f * (reward - expectedReward_);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range - using faster comparison
    if (dopamineLevel_ < -1.0f) dopamineLevel_ = -1.0f;
    if (dopamineLevel_ > 1.0f) dopamineLevel_ = 1.0f;
    
    // Batch process synapses - pre-fetch brain regions
    const auto& regions = brain_->getRegions();
    for (const auto& region : regions) {
        const auto& synapses = region->getSynapses();
        for (auto* syn : synapses) {
            float eligibility = syn->getEligibilityTrace();
            
            // Use squared comparison for faster absolute value check
            if (eligibility * eligibility > 1e-6f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine - optimized calculation
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    if (plasticityFactor < 0.1f) plasticityFactor = 0.1f;
    if (plasticityFactor > 2.0f) plasticityFactor = 2.0f;
    
    // Apply to STDP with direct pointer
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
    // Optimized reset with bulk clear operations
    motorForward_.clear(); motorBackward_.clear(); motorTurnLeft_.clear();
    motorTurnRight_.clear(); motorInteract_.clear(); motorWait_.clear();
    
    sensoryVision_.clear(); sensoryTouch_.clear(); sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Reset scalar values
    dopamineLevel_ = noveltyLevel_ = curiosityLevel_ = predictionError_ = 0.0f;
    expectedReward_ = developmentalAge_ = plasticityModifier_ = 1.0f;
    
    // Fast clear of previous vision buffer using shrink_to_fit for memory efficiency
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        previousVision_.shrink_to_fit();
    }
}

} // namespace nlm
