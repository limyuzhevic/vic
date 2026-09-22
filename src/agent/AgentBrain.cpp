#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Reward.hpp"
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
    
    // Reset all integrated systems
    if (brain_) {
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* predictionSys = brain_->getPredictionSystem();
        auto* planner = brain_->getPlanner();
        auto* conceptForm = brain_->getConceptFormation();
        auto* attention = brain_->getAttention();
        
        if (workingMem) workingMem->clear();
        if (episodicMem) episodicMem->clear();
        if (predictionSys) predictionSys->reset();
        if (planner) planner->reset();
        if (conceptForm) conceptForm->reset();
        if (attention) attention->reset();
        
        // Reset neuromodulators
        auto* dopamine = brain_->getDopamine();
        auto* acetylcholine = brain_->getAcetylcholine();
        auto* norepinephrine = brain_->getNorepinephrine();
        auto* serotonin = brain_->getSerotonin();
        auto* curiosity = brain_->getCuriosity();
        auto* novelty = brain_->getNovelty();
        auto* predictionErrorSys = brain_->getPredictionErrorSignal();
        
        if (dopamine) dopamine->reset();
        if (acetylcholine) acetylcholine->reset();
        if (norepinephrine) norepinephrine->reset();
        if (serotonin) serotonin->reset();
        if (curiosity) curiosity->reset();
        if (novelty) novelty->reset();
        if (predictionErrorSys) predictionErrorSys->reset();
    }
}

// Implement the Phase 6 integrated artificial brain system
// Integrates NeuralPlanner, ConceptFormation, Attention system, and all neuromodulators

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
    // Phase 6: Initialize integration with higher cognition systems
    if (brain_) {
        // Verify all higher cognition systems are available
        auto* planner = brain_->getPlanner();
        auto* conceptForm = brain_->getConceptFormation();
        auto* attention = brain_->getAttention();
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* predictionSys = brain_->getPredictionSystem();
        
        // Log integration status
        NLM_LOG_INFO("Phase 6 AgentBrain initialized with integrated cognition systems:");
        if (planner) NLM_LOG_INFO("- NeuralPlanner: integrated");
        if (conceptForm) NLM_LOG_INFO("- ConceptFormation: integrated");
        if (attention) NLM_LOG_INFO("- Attention system: integrated");
        if (workingMem) NLM_LOG_INFO("- Working memory: integrated");
        if (episodicMem) NLM_LOG_INFO("- Episodic memory: integrated");
        if (predictionSys) NLM_LOG_INFO("- Prediction system: integrated");
        
        // Check all neuromodulators
        if (brain_->getDopamine()) NLM_LOG_INFO("- Dopamine: integrated");
        if (brain_->getAcetylcholine()) NLM_LOG_INFO("- Acetylcholine: integrated");
        if (brain_->getNorepinephrine()) NLM_LOG_INFO("- Norepinephrine: integrated");
        if (brain_->getSerotonin()) NLM_LOG_INFO("- Serotonin: integrated");
        if (brain_->getCuriosity()) NLM_LOG_INFO("- Curiosity: integrated");
        if (brain_->getNovelty()) NLM_LOG_INFO("- Novelty: integrated");
        if (brain_->getPredictionErrorSignal()) NLM_LOG_INFO("- PredictionError: integrated");
        
        // Initialize motor and sensory neuron groups
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
    
    // Log integrated system initialization status
    if (brain_) {
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* predictionSys = brain_->getPredictionSystem();
        auto* planner = brain_->getPlanner();
        auto* conceptForm = brain_->getConceptFormation();
        auto* attention = brain_->getAttention();
        
        if (workingMem) workingMem->initialize(brain_.get());
        if (episodicMem) episodicMem->initialize(brain_.get());
        if (predictionSys) predictionSys->initialize();
        if (planner) planner->initialize(brain_.get());
        if (conceptForm) conceptForm->initialize(brain_.get());
        if (attention) attention->initialize(brain_.get());
        
        NLM_LOG_INFO("Phase 6 integrated systems initialized");
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
    if (!brain_) return;
    
    // ========== ENHANCED SENSORY PROCESSING (Phase 6) ==========
    
    // 1. Store sensory input in working memory for concept formation
    auto* workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        // Convert percept to neural pattern
        std::vector<float> sensoryPattern;
        
        // Vision input
        const auto& vision = percept.getVision();
        sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
        
        // Touch input  
        const auto& touch = percept.getTouch();
        sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
        
        // Internal signals
        const auto& intern = percept.getInternal();
        sensoryPattern.insert(sensoryPattern.end(), intern.begin(), intern.end());
        
        // Proprioception
        const auto& proprio = percept.getProprioception();
        sensoryPattern.insert(sensoryPattern.end(), proprio.begin(), proprio.end());
        
        // Store in working memory with neuromodulation influence
        float neuromodulationLevel = getNeuromodulationLevel();
        workingMem->storePattern(sensoryPattern, neuromodulationLevel);
    }
    
    // 2. Apply attention selection to sensory inputs
    auto* attention = brain_->getAttention();
    if (attention && workingMem) {
        // Get working memory contents for attentional selection
        const auto& memoryNeurons = workingMem->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            // Apply attentional selection
            attention->processCompetition(memoryNeurons);
            
            // Get attention-modulated sensory activity
            std::vector<float> attentionWeights;
            for (const auto& neuronId : memoryNeurons) {
                float activation = workingMem->getNeuronActivation(neuronId);
                float attentionStrength = attention->getInhibitionFor(neuronId);
                attentionWeights.push_back(activation * attentionStrength);
            }
            
            // Apply attention weights to sensory processing
            for (size_t i = 0; i < attentionWeights.size() && i < vision.size(); ++i) {
                if (sensoryVision_[i]) {
                    float current = vision[i] * 5.0f * attentionWeights[i];
                    sensoryVision_[i]->injectCurrent(current);
                }
            }
        }
    }
    
    // 3. Apply concept formation to sensory input
    auto* conceptForm = brain_->getConceptFormation();
    if (conceptForm && workingMem) {
        // Update concept formation with current sensory pattern
        std::vector<float> currentPattern;
        const auto& vision = percept.getVision();
        const auto& touch = percept.getTouch();
        const auto& intern = percept.getInternal();
        const auto& proprio = percept.getProprioception();
        
        currentPattern.insert(currentPattern.end(), vision.begin(), vision.end());
        currentPattern.insert(currentPattern.end(), touch.begin(), touch.end());
        currentPattern.insert(currentPattern.end(), intern.begin(), intern.end());
        currentPattern.insert(currentPattern.end(), proprio.begin(), proprio.end());
        
        // Present to concept formation with neuromodulation
        float neuromodulation = getNeuromodulationLevel();
        conceptForm->presentExperience(currentPattern, currentPattern, neuromodulation, 0);
    }
    
    // 4. Update prediction system with current state
    auto* predictionSys = brain_->getPredictionSystem();
    if (predictionSys) {
        // Update prediction system for forward modeling
        predictionSys->updatePredictions(previousVision_, percept.getVision());
    }
    
    // ========== TRADITIONAL SENSORY PROCESSING (Phase 1) ==========
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Apply attention-modulated current injection
            float baseCurrent = vision[i] * 5.0f;
            sensoryVision_[i]->injectCurrent(baseCurrent);
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
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Phase 6: Enhanced motor decoding with integrated cognition
    MotorCommand decoded;
    
    // 1. Try concept-based action selection via NeuralPlanner
    decoded = decodeWithPlanner();
    
    // If planner didn't produce a strong decision, use traditional decoding
    if (decoded == MotorCommand::Wait) {
        decoded = decodeFromMotorNeurons();
    }
    
    // 2. Apply neuromodulation effects on decision making
    applyNeuromodulationToDecision(decoded);
    
    // 3. Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeWithPlanner() {
    if (!brain_) return MotorCommand::Wait;
    
    auto* planner = brain_->getPlanner();
    auto* conceptForm = brain_->getConceptFormation();
    
    if (!planner || !conceptForm) return MotorCommand::Wait;
    
    // Get current working memory for planning
    auto* workingMem = brain_->getWorkingMemory();
    if (!workingMem) return MotorCommand::Wait;
    
    // Get current neural state for planning
    std::vector<float> currentState;
    const auto& memoryNeurons = workingMem->getMemoryNeurons();
    
    // Extract current state from working memory
    for (const auto& neuronId : memoryNeurons) {
        float activation = workingMem->getNeuronActivation(neuronId);
        currentState.push_back(activation);
    }
    
    // Get concept-based action preferences
    std::vector<float> conceptPreferences;
    const auto& concepts = conceptForm->getActiveConcepts();
    for (const auto& concept : concepts) {
        conceptPreferences.push_back(concept.relevance);
    }
    
    // Use NeuralPlanner for action planning
    MotorCommand plannedAction = planner->planAction(currentState, conceptPreferences, getNeuromodulationLevel());
    
    // Apply neuromodulation effects on planned action
    if (getNeuromodulationLevel() > 0.2f) {  // Positive dopamine promotes action
        return plannedAction;
    } else if (getNeuromodulationLevel() < -0.2f) {  // Negative dopamine promotes hesitation
        return MotorCommand::Wait;
    }
    
    return plannedAction;
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
    
    // Apply neuromodulation to integrated systems
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        dopamine->signalRewardPredictionError(predictionError_);
    }
    
    // Update curiosity based on prediction error
    auto* curiosity = brain_->getCuriosity();
    if (curiosity) {
        curiosity->update(predictionError_ * 0.1f);
    }
    
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

void AgentBrain::applyNeuromodulationToDecision(float& activationBias) {
    // Apply neuromodulation effects on decision making
    auto* dopamine = brain_->getDopamine();
    auto* acetylcholine = brain_->getAcetylcholine();
    auto* norepinephrine = brain_->getNorepinephrine();
    
    float neuromodulationLevel = 0.0f;
    
    if (dopamine) {
        neuromodulationLevel += dopamine->getLevel() * 0.5f;
    }
    if (acetylcholine) {
        neuromodulationLevel += acetylcholine->getLevel() * 0.3f;
    }
    if (norepinephrine) {
        neuromodulationLevel += norepinephrine->getLevel() * 0.2f;
    }
    
    // Apply to activation bias
    activationBias *= (1.0f + neuromodulationLevel);
    activationBias = std::clamp(activationBias, 0.0f, 1.0f);
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // ========== DEVELOPMENT INTEGRATION (Phase 6) ==========
    
    // 1. Update all integrated systems with developmental stage
    auto* workingMem = brain_->getWorkingMemory();
    auto* episodicMem = brain_->getEpisodicMemory();
    auto* predictionSys = brain_->getPredictionSystem();
    auto* planner = brain_->getPlanner();
    auto* conceptForm = brain_->getConceptFormation();
    auto* attention = brain_->getAttention();
    
    DevelopmentalStage stage = getDevelopmentalStage();
    
    // Update working memory capacity based on development
    if (workingMem) {
        float capacityModifier = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial: capacityModifier = 1.0f; break;
            case DevelopmentalStage::CriticalPeriod: capacityModifier = 0.8f; break;
            case DevelopmentalStage::Maturation: capacityModifier = 0.6f; break;
            case DevelopmentalStage::Adult: capacityModifier = 0.4f; break;
        }
        workingMem->setCapacity(workingMem->getCapacity() * capacityModifier);
    }
    
    // Update episodic memory consolidation based on development
    if (episodicMem) {
        float consolidationStrength = 0.5f;
        switch (stage) {
            case DevelopmentalStage::Initial: consolidationStrength = 0.3f; break;
            case DevelopmentalStage::CriticalPeriod: consolidationStrength = 0.6f; break;
            case DevelopmentalStage::Maturation: consolidationStrength = 0.8f; break;
            case DevelopmentalStage::Adult: consolidationStrength = 0.4f; break;
        }
        episodicMem->setConsolidationStrength(consolidationStrength);
    }
    
    // Update prediction system learning rate
    if (predictionSys) {
        float learningRate = 0.1f;
        switch (stage) {
            case DevelopmentalStage::Initial: learningRate = 0.3f; break;
            case DevelopmentalStage::CriticalPeriod: learningRate = 0.2f; break;
            case DevelopmentalStage::Maturation: learningRate = 0.1f; break;
            case DevelopmentalStage::Adult: learningRate = 0.05f; break;
        }
        predictionSys->setLearningRate(learningRate);
    }
    
    // Update planner planning depth
    if (planner) {
        int planningDepth = 3;
        switch (stage) {
            case DevelopmentalStage::Initial: planningDepth = 2; break;
            case DevelopmentalStage::CriticalPeriod: planningDepth = 3; break;
            case DevelopmentalStage::Maturation: planningDepth = 5; break;
            case DevelopmentalStage::Adult: planningDepth = 7; break;
        }
        planner->setPlanningDepth(planningDepth);
    }
    
    // Update concept formation learning rate
    if (conceptForm) {
        float learningRate = 0.1f;
        switch (stage) {
            case DevelopmentalStage::Initial: learningRate = 0.3f; break;
            case DevelopmentalStage::CriticalPeriod: learningRate = 0.2f; break;
            case DevelopmentalStage::Maturation: learningRate = 0.1f; break;
            case DevelopmentalStage::Adult: learningRate = 0.05f; break;
        }
        conceptForm->setLearningRate(learningRate);
    }
    
    // Update attentional selection strengths
    if (attention) {
        float inhibitionStrength = 0.5f;
        float excitationStrength = 1.5f;
        switch (stage) {
            case DevelopmentalStage::Initial: 
                inhibitionStrength = 0.3f; 
                excitationStrength = 1.2f; 
                break;
            case DevelopmentalStage::CriticalPeriod: 
                inhibitionStrength = 0.4f; 
                excitationStrength = 1.3f; 
                break;
            case DevelopmentalStage::Maturation: 
                inhibitionStrength = 0.5f; 
                excitationStrength = 1.5f; 
                break;
            case DevelopmentalStage::Adult: 
                inhibitionStrength = 0.6f; 
                excitationStrength = 1.8f; 
                break;
        }
        attention->setInhibitionStrength(inhibitionStrength);
        attention->setExcitationStrength(excitationStrength);
    }
    
    // Apply neuromodulator baseline levels based on development
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        float baseline = 0.1f;
        switch (stage) {
            case DevelopmentalStage::Initial: baseline = 0.5f; break;
            case DevelopmentalStage::CriticalPeriod: baseline = 0.3f; break;
            case DevelopmentalStage::Maturation: baseline = 0.2f; break;
            case DevelopmentalStage::Adult: baseline = 0.1f; break;
        }
        dopamine->setBaseline(baseline);
    }
    
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
    
    // Reset all integrated systems
    if (brain_) {
        auto* workingMem = brain_->getWorkingMemory();
        auto* episodicMem = brain_->getEpisodicMemory();
        auto* predictionSys = brain_->getPredictionSystem();
        auto* planner = brain_->getPlanner();
        auto* conceptForm = brain_->getConceptFormation();
        auto* attention = brain_->getAttention();
        
        if (workingMem) workingMem->clear();
        if (episodicMem) episodicMem->clear();
        if (predictionSys) predictionSys->reset();
        if (planner) planner->reset();
        if (conceptForm) conceptForm->reset();
        if (attention) attention->reset();
        
        // Reset neuromodulators
        auto* dopamine = brain_->getDopamine();
        auto* acetylcholine = brain_->getAcetylcholine();
        auto* norepinephrine = brain_->getNorepinephrine();
        auto* serotonin = brain_->getSerotonin();
        auto* curiosity = brain_->getCuriosity();
        auto* novelty = brain_->getNovelty();
        auto* predictionErrorSys = brain_->getPredictionErrorSignal();
        
        if (dopamine) dopamine->reset();
        if (acetylcholine) acetylcholine->reset();
        if (norepinephrine) norepinephrine->reset();
        if (serotonin) serotonin->reset();
        if (curiosity) curiosity->reset();
        if (novelty) novelty->reset();
        if (predictionErrorSys) predictionErrorSys->reset();
    }
}

} // namespace nlm
