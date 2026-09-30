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
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Batch vision input processing (256 values)
    const auto& vision = percept.getVision();
    if (!vision.empty() && !sensoryVision_.empty()) {
        size_t minSize = std::min(sensoryVision_.size(), vision.size());
        // Use local reference for faster access
        const std::vector<Neuron*>& visionNeurons = sensoryVision_;
        // Pre-calculate scaling factor
        const float scaleFactor = 5.0f;
        
        // Process vision in a single pass with bounds checking
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = visionNeurons[i];
            if (neuron) {
                float current = vision[i] * scaleFactor;
                neuron->injectCurrent(current);
            }
        }
        
        // Compute novelty using incremental update
        if (minSize > 0) {
            float totalDiff = 0.0f;
            size_t count = 0;
            for (size_t i = 0; i < minSize; ++i) {
                float diff = std::abs(vision[i] - previousVision_[i]);
                totalDiff += diff;
                ++count;
            }
            
            // Normalize and apply decay
            noveltyLevel_ = (count > 0) ? (totalDiff / count) : 0.0f;
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Store vision for next iteration (swap buffer)
            std::swap(previousVision_, const_cast<std::vector<float>&>(vision));
        }
    }
    
    // Batch touch input processing (8 values)
    const auto& touch = percept.getTouch();
    if (!touch.empty() && !sensoryTouch_.empty()) {
        size_t minSize = std::min(sensoryTouch_.size(), touch.size());
        const float scaleFactor = 8.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryTouch_[i];
            if (neuron) {
                neuron->injectCurrent(touch[i] * scaleFactor);
            }
        }
    }
    
    // Batch internal signals processing (4 values)
    const auto& intern = percept.getInternal();
    if (!intern.empty() && !sensoryInternal_.empty()) {
        size_t minSize = std::min(sensoryInternal_.size(), intern.size());
        const float scaleFactor = 5.0f;
        const float offset = -1.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryInternal_[i];
            if (neuron) {
                float current = (intern[i] * 2.0f + offset) * scaleFactor;
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Batch proprioception processing (6 values)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty() && !sensoryProprioception_.empty()) {
        size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
        const float scaleFactor = 3.0f;
        const float offset = -1.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            Neuron* neuron = sensoryProprioception_[i];
            if (neuron) {
                float current = (proprio[i] * 2.0f + offset) * scaleFactor;
                neuron->injectCurrent(current);
            }
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        float predErrorAbs = std::abs(predictionError_);
        curiosityLevel_ = noveltyLevel_ * 2.0f + predErrorAbs * 0.5f;
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
    // Cache motor neuron vectors locally to avoid repeated access
    const std::vector<Neuron*>& forwardNeurons = motorForward_;
    const std::vector<Neuron*>& backwardNeurons = motorBackward_;
    const std::vector<Neuron*>& leftNeurons = motorTurnLeft_;
    const std::vector<Neuron*>& rightNeurons = motorTurnRight_;
    const std::vector<Neuron*>& interactNeurons = motorInteract_;
    const std::vector<Neuron*>& waitNeurons = motorWait_;
    
    // Calculate average activity in each motor group using lambda directly
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(forwardNeurons);
    float backwardAct = calcActivity(backwardNeurons);
    float leftAct = calcActivity(leftNeurons);
    float rightAct = calcActivity(rightNeurons);
    float interactAct = calcActivity(interactNeurons);
    float waitAct = calcActivity(waitNeurons);
    
    // Find maximum activity (optimize comparison order - check strongest actions first)
    if (forwardAct > 0.5f) return MotorCommand::MoveForward;
    if (backwardAct > 0.5f) return MotorCommand::MoveBackward;
    if (rightAct > 0.5f) return MotorCommand::TurnRight;
    if (leftAct > 0.5f) return MotorCommand::TurnLeft;
    if (interactAct > 0.5f) return MotorCommand::Interact;
    
    return MotorCommand::Wait;
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
    
    // Apply to all synapses with eligibility traces
    // Pre-calculate commonly used values
    const float eligibilityThreshold = 0.001f;
    const float eligibilityDecay = 0.1f;
    const float weightFactor = plasticityFactor;
    
    for (const auto& region : brain_->getRegions()) {
        const auto& synapses = region->getSynapses();
        for (auto* syn : synapses) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > eligibilityThreshold) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * weightFactor;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(eligibilityDecay);
            }
        }
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
    
    // Clear previous vision efficiently
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
}

} // namespace nlm
