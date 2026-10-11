// AgentBrain.cpp - Optimized implementation for NLM AgentBrain

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Optimize memory allocation - pre-allocate vectors
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    
    previousVision_.reserve(visionWidth * visionHeight);
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Log initialization with optimized string formatting
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() + 
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    NLM_LOG_INFO(std::format(
        "AgentBrain initialized with {} vision, {} touch, {} internal, {} proprioceptive sensory neurons",
        sensoryVision_.size(), sensoryTouch_.size(), sensoryInternal_.size(),
        sensoryProprioception_.size()
    ));
}

size_t AgentBrain::getSensoryInputSize() const {
    return static_cast<size_t>(256 + 8 + 4 + 6);
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Pre-calculate vector sizes for better performance
    size_t visionSize = std::min(sensoryVision_.size(), percept.getVision().size());
    size_t touchSize = std::min(sensoryTouch_.size(), percept.getTouch().size());
    size_t internalSize = std::min(sensoryInternal_.size(), percept.getInternal().size());
    size_t proprioSize = std::min(sensoryProprioception_.size(), percept.getProprioception().size());
    
    // Vision input processing (256 values)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < visionSize; ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron) {
            float current = vision[i] * 5.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input processing (8 values)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touchSize; ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals processing (4 values)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < internalSize; ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception processing (6 values)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprioSize; ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Novelty detection with optimized computation
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        const size_t compareSize = std::min(vision.size(), previousVision_.size());
        
        // Vectorized computation for performance
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply decay
        noveltyLevel_ = (vision.size() > 0) ? (totalDiff / static_cast<float>(vision.size())) : 0.0f;
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time (optimized assignment)
        if (previousVision_.size() >= vision.size()) {
            std::copy(vision.begin(), vision.begin() + vision.size(), previousVision_.begin());
        } else {
            previousVision_ = vision;
        }
    }
    
    // Curiosity calculation with optimized bounds checking
    if (curiosityEnabled_) {
        curiosityLevel_ = std::clamp(
            noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f,
            0.0f, 1.0f
        );
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Optimized curiosity check
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Optimized activity calculation using local functions
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            float potential = n->getState().membranePotential;
            float resting = n->getState().restingPotential;
            sum += std::abs(potential - resting);
        }
        return sum / static_cast<float>(neurons.size());
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Structured binding for cleaner code
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    const CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Optimized search for maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& cmdAct : commands) {
        if (cmdAct.activity > bestActivity) {
            bestActivity = cmdAct.activity;
            best = cmdAct.cmd;
        }
    }
    
    // Threshhold check
    return (bestActivity < 0.5f) ? MotorCommand::Wait : best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Optimized exploration with pre-calculated bounds
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f;
        
        if (exploreChance > 1.0f) exploreChance = 1.0f; // Clamp
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
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
    
    // Optimized prediction error calculation
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Clamp prediction error
    dopamineLevel_ = std::clamp(predictionError_, -1.0f, 1.0f);
    
    // Apply to synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Optimized plasticity modulation
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
}

} // namespace nlm
