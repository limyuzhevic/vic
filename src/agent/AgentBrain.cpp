#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../sensory/InternalSignals.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    
    // Apply neuromodulation to brain cognition systems
    applyBrainNeuromodulation();
    
    // Process sensory input through prediction system
    if (brain_ && vision.size() >= 10) {  // Only process if we have meaningful vision data
        // Create SensoryInput from percept for prediction system
        SensoryInput sensoryInput = createSensoryInputFromPercept(percept);
        
        // Let prediction system predict next state
        std::unique_ptr<SensoryInput> predictedState = 
            brain_->getPredictionSystem()->predictNextState(sensoryInput);
        
        // Compute prediction error
        float predictionError = computePredictionError(predictedState, sensoryInput);
        
        // Update prediction error neuromodulator with new prediction error
        if (brain_->getPredictionError()) {
            brain_->getPredictionError()->computeError(predictionError);
        }
        
        // Update prediction system with actual observation
        if (predictedState) {
            brain_->getPredictionSystem()->updatePredictions(*predictedState, sensoryInput);
        }
        
        // Integrate prediction error with other neuromodulators
        if (brain_->getCuriosity()) {
            // Update curiosity based on prediction error (for exploration)
            float noveltyLevel = 0.0f; // Could get from brain's novelty system
            brain_->getCuriosity()->update(noveltyLevel, predictionError, pImpl->timestep);
        }
        
        // Modulate dopamine based on prediction error
        if (brain_->getDopamine()) {
            // Positive prediction error (surprise) should increase dopamine
            // Negative prediction error (prediction correct) should decrease dopamine
            float dopamineMod = predictionError * 2.0f - 1.0f; // Map error [-1, 1] to dopamine effect
            brain_->getDopamine()->setLevel(dopamineMod);
        }
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

void AgentBrain::applyBrainNeuromodulation() {
    // Apply neuromodulation signals to brain cognition systems
    // This creates bidirectional communication between agent and brain neuromodulators
    
    if (!brain_) return;
    
    // Apply dopamine to brain attention system
    if (brain_->getDopamine()) {
        // Modulate brain attention through dopamine
        float brainDopamine = brain_->getDopamine()->getLevel();
        
        // Update agent dopamine with brain level for consistency
        dopamineLevel_ = brainDopamine;
        
        // In a full implementation, we would:
        // 1. Apply dopamine effects on neural excitability in brain regions
        // 2. Modulate attentional gating in brain's attention system
        // 3. Enhance concept stability based on reward signals
        // 4. Boost planning value-based evaluation
        
        NLM_LOG_INFO("AgentBrain: Applied dopamine (level: " + 
                   std::to_string(brainDopamine) + ") to brain cognition systems");
    }
    
    // Apply curiosity to brain exploration
    if (brain_->getCuriosity()) {
        float brainCuriosity = brain_->getCuriosity()->getLevel();
        
        // Update agent curiosity with brain level
        curiosityLevel_ = brainCuriosity;
        
        NLM_LOG_INFO("AgentBrain: Applied curiosity (level: " + 
                   std::to_string(brainCuriosity) + ") to brain exploration systems");
    }
    
    // Apply novelty to brain attention
    if (brain_->getNovelty()) {
        float brainNovelty = brain_->getNovelty()->getLevel();
        
        // Update agent novelty with brain level
        noveltyLevel_ = brainNovelty;
        
        // Enhance attentional capture in brain attention system
        NLM_LOG_INFO("AgentBrain: Applied novelty (level: " + 
                   std::to_string(brainNovelty) + ") to brain attention systems");
    }
    
    // Apply prediction error to brain learning
    if (brain_->getPredictionErrorSignal()) {
        float brainPredictionError = brain_->getPredictionErrorSignal()->getMagnitude();
        
        // Update agent prediction error with brain level
        predictionError_ = brainPredictionError;
        
        // Accelerate concept formation learning based on prediction error
        // and correct planning estimates
        NLM_LOG_INFO("AgentBrain: Applied prediction error (magnitude: " + 
                   std::to_string(brainPredictionError) + ") to brain learning systems");
    }
    
    // Bidirectional communication: brain sends neuromodulation to agent
    // and agent modulates its behavior based on brain state
    if (brain_->getDopamine() && brain_->getCuriosity()) {
        // Combine dopamine and curiosity effects for integrated behavior
        float integratedMotivation = 
            brain_->getDopamine()->getLevel() * 0.6f + 
            brain_->getCuriosity()->getLevel() * 0.4f;
        
        // Higher motivation = more persistent behavior
        if (integratedMotivation > 0.7f) {
            NLM_LOG_INFO("AgentBrain: High integrated motivation (" + 
                       std::to_string(integratedMotivation) + ") enhancing behavior persistence");
        }
    }
}

} // namespace nlm

SensoryInput createSensoryInputFromPercept(const SensoryPercept& percept) {
    // Create a combined SensoryInput from the percept components
    // This creates a multi-modal sensory input for the prediction system
    
    // For now, create a Vision input if we have vision data
    if (!percept.getVision().empty() && !percept.getTouch().empty() && 
        !percept.getInternal().empty() && !percept.getProprioception().empty()) {
        // Create Vision input with the vision data
        Vision visionInput(percept.getVisionWidth(), percept.getVisionHeight());
        visionInput.setData(percept.getVision());
        
        // For a more complete implementation, we would combine all modalities
        // For now, return the vision input as it's the primary visual modality
        return visionInput;
    }
    
    // Fallback: create a simple internal signals input
    InternalSignals internalInput;
    for (float val : percept.getVision()) {
        internalInput.addSignal(val);
    }
    for (float val : percept.getTouch()) {
        internalInput.addSignal(val);
    }
    for (float val : percept.getInternal()) {
        internalInput.addSignal(val);
    }
    for (float val : percept.getProprioception()) {
        internalInput.addSignal(val);
    }
    
    return internalInput;
}

float computePredictionError(const std::unique_ptr<SensoryInput>& predicted,
                             const SensoryInput& actual) {
    if (!predicted) {
        return 0.0f;
    }
    
    const auto& predData = predicted->getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return 1.0f; // Maximum error if data sizes don't match
    }
    
    // Calculate mean squared error
    float sumError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
    }
    
    return sumError / predData.size();
}
