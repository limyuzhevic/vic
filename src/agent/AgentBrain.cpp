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
    // Initialize motor and sensory neuron groups with null pointers for safety
    motorForward_.reserve(50);
    motorBackward_.reserve(50);
    motorTurnLeft_.reserve(50);
    motorTurnRight_.reserve(50);
    motorInteract_.reserve(50);
    motorWait_.reserve(50);
    
    sensoryVision_.reserve(100);
    sensoryTouch_.reserve(20);
    sensoryInternal_.reserve(10);
    sensoryProprioception_.reserve(20);
    
    // Ensure brain exists before trying to access it
    if (brain_) {
        initializeNeuronGroups();
    }
}

void AgentBrain::initializeNeuronGroups() {
    // Initialize motor and sensory neuron groups safely
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize neuron groups: brain pointer is null");
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute motor neurons to different action groups
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                motorTurnLeft_.size() + motorTurnRight_.size() +
                                motorInteract_.size() + motorWait_.size();
                    
                    // Safe modulo operation
                    if (!pop->getNeurons().empty()) {
                        int caseIdx = idx % static_cast<int>(pop->getNeurons().size());
                        switch (caseIdx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    // Safe modulo operation
                    if (!pop->getNeurons().empty()) {
                        int caseIdx = idx % static_cast<int>(pop->getNeurons().size());
                        switch (caseIdx % 4) {
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
    
    NLM_LOG_INFO("Neuron groups initialized: " +
                 std::to_string(motorForward_.size()) + " motorForward, " +
                 std::to_string(motorBackward_.size()) + " motorBackward, " +
                 std::to_string(motorTurnLeft_.size()) + " motorTurnLeft, " +
                 std::to_string(motorTurnRight_.size()) + " motorTurnRight, " +
                 std::to_string(motorInteract_.size()) + " motorInteract, " +
                 std::to_string(motorWait_.size()) + " motorWait, " +
                 std::to_string(sensoryVision_.size()) + " sensoryVision, " +
                 std::to_string(sensoryTouch_.size()) + " sensoryTouch, " +
                 std::to_string(sensoryInternal_.size()) + " sensoryInternal, " +
                 std::to_string(sensoryProprioception_.size()) + " sensoryProprioception");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(visionSize) + " vision pixels");
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
        NLM_LOG_WARNING("Cannot process sensory input: brain pointer is null");
        return;
    }
    
    // Process vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        size_t visionSize = vision.size();
        size_t numVisionNeurons = std::min(sensoryVision_.size(), visionSize);
        
        for (size_t i = 0; i < numVisionNeurons; ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
        
        // Compute novelty (difference from previous vision)
        float totalDiff = 0.0f;
        size_t compareSize = std::min(visionSize, previousVision_.size());
        
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and update novelty level
        if (!vision.empty()) {
            noveltyLevel_ = totalDiff / static_cast<float>(std::max<size_t>(compareSize, 1));
            noveltyLevel_ *= sensoryNoveltyDecay_;
            
            // Store for next time
            previousVision_ = vision;
        }
    }
    
    // Process touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    size_t numTouchNeurons = std::min(sensoryTouch_.size(), touch.size());
    for (size_t i = 0; i < numTouchNeurons; ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Process internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    size_t numInternalNeurons = std::min(sensoryInternal_.size(), intern.size());
    for (size_t i = 0; i < numInternalNeurons; ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Process proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    size_t numProprioceptionNeurons = std::min(sensoryProprioception_.size(), proprio.size());
    for (size_t i = 0; i < numProprioceptionNeurons; ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f && brain_) {
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

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: brain pointer is null");
        return MotorCommand::Wait;
    }
    
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
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Validate inputs
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward values: reward=" + std::to_string(reward) + 
                        ", predictedReward=" + std::to_string(predictedReward));
        reward = 0.0f;
        predictedReward = 0.0f;
    }
    
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
            if (!syn) continue;  // Safety check
            
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
    
    // Validate timestep
    if (timestep < 0.0) {
        NLM_LOG_WARNING("Negative timestep in updateDevelopment, using absolute value");
        timestep = std::abs(timestep);
    }
    
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

void AgentBrain::logNeuronDistribution() const {
    if (!brain_) {
        NLM_LOG_INFO("AgentBrain: No brain available for neuron distribution log");
        return;
    }
    
    NLM_LOG_INFO("AgentBrain Neuron Distribution Report:");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(motorForward_.size()) + " forward, " +
                 std::to_string(motorBackward_.size()) + " backward, " +
                 std::to_string(motorTurnLeft_.size()) + " left, " +
                 std::to_string(motorTurnRight_.size()) + " right, " +
                 std::to_string(motorInteract_.size()) + " interact, " +
                 std::to_string(motorWait_.size()) + " wait");
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(sensoryVision_.size()) + " vision, " +
                 std::to_string(sensoryTouch_.size()) + " touch, " +
                 std::to_string(sensoryInternal_.size()) + " internal, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception");
    
    // Log brain statistics
    NLM_LOG_INFO("  Brain statistics: " + std::to_string(brain_->getTotalNeuronCount()) + " total neurons, " +
                 std::to_string(brain_->getTotalSynapseCount()) + " synapses");
}

void AgentBrain::reserveMemoryForNeuronGroups() {
    // Pre-allocate memory for better performance
    // Reserve reasonable amounts based on expected population sizes
    motorForward_.reserve(50);
    motorBackward_.reserve(50);
    motorTurnLeft_.reserve(50);
    motorTurnRight_.reserve(50);
    motorInteract_.reserve(50);
    motorWait_.reserve(50);
    
    sensoryVision_.reserve(100);
    sensoryTouch_.reserve(20);
    sensoryInternal_.reserve(10);
    sensoryProprioception_.reserve(20);
}
