#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

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
    // Validate input
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    // Initialize motor and sensory neuron groups with validation
    try {
        initializeNeuronGroups();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("AgentBrain: Failed to initialize neuron groups: " + std::string(e.what()));
        throw;
    }
    
    NLM_LOG_INFO("AgentBrain initialized successfully with brain containing " + 
                 std::to_string(brain_->getTotalNeuronCount()) + " neurons");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initializeNeuronGroups() {
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) continue;
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop->getNeurons());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop->getNeurons());
            }
        }
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) continue;
        
        // Validate neuron has valid pointer
        if (!validateNeuronPointer(n)) {
            NLM_LOG_WARNING("AgentBrain: Invalid neuron pointer in motor distribution");
            continue;
        }
        
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
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) continue;
        
        if (!validateNeuronPointer(n)) {
            NLM_LOG_WARNING("AgentBrain: Invalid neuron pointer in sensory distribution");
            continue;
        }
        
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

bool AgentBrain::validateNeuronPointer(Neuron* neuron) {
    if (!neuron) return false;
    
    // Additional validation checks
    NeuronState state = neuron->getState();
    if (std::isnan(state.membranePotential) || std::isinf(state.membranePotential)) {
        return false;
    }
    
    return true;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (world.getVisionWidth() <= 0 || world.getVisionHeight() <= 0) {
        NLM_LOG_ERROR("AgentBrain: Invalid world dimensions for vision");
        throw std::invalid_argument("Invalid world dimensions");
    }
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioceptive sensory neurons");
    
    // Validate that we have sufficient sensory neurons
    size_t expectedVisionNeurons = world.getVisionWidth() * world.getVisionHeight();
    if (sensoryVision_.size() < expectedVisionNeurons) {
        NLM_LOG_WARNING("AgentBrain: Insufficient vision sensory neurons allocated. " +
                       std::to_string(sensoryVision_.size()) + " available, " +
                       std::to_string(expectedVisionNeurons) + " expected");
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot process sensory input - brain pointer is null");
        return;
    }
    
    // Validate percept data
    if (!validatePercept(percept)) {
        NLM_LOG_ERROR("AgentBrain: Invalid sensory percept received");
        return;
    }
    
    // Process vision input
    processVisionInput(percept);
    
    // Process touch input
    processTouchInput(percept);
    
    // Process internal signals
    processInternalInput(percept);
    
    // Process proprioception
    processProprioceptionInput(percept);
    
    // Compute novelty and update curiosity
    updateNoveltyAndCuriosity(percept);
}

bool AgentBrain::validatePercept(const SensoryPercept& percept) const {
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Check for null or invalid data
    if (vision.empty() && touch.empty() && intern.empty() && proprio.empty()) {
        NLM_LOG_WARNING("AgentBrain: Empty percept data");
        return false;
    }
    
    // Validate each component
    if (!vision.empty()) {
        for (float val : vision) {
            if (std::isnan(val) || std::isinf(val)) {
                return false;
            }
        }
    }
    
    // Similar validation for other components...
    return true;
}

void AgentBrain::processVisionInput(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    // Check bounds
    size_t minSize = std::min(sensoryVision_.size(), vision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryVision_[i] && validateNeuronPointer(sensoryVision_[i])) {
            float current = vision[i] * 5.0f;  // Scale factor
            
            // Clamp to reasonable range
            current = std::clamp(current, -100.0f, 100.0f);
            
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const SensoryPercept& percept) {
    const auto& touch = percept.getTouch();
    size_t minSize = std::min(sensoryTouch_.size(), touch.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryTouch_[i] && validateNeuronPointer(sensoryTouch_[i])) {
            float current = (touch[i] * 8.0f - 4.0f) * 5.0f;  // Normalize and scale
            current = std::clamp(current, -50.0f, 50.0f);
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const SensoryPercept& percept) {
    const auto& intern = percept.getInternal();
    size_t minSize = std::min(sensoryInternal_.size(), intern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryInternal_[i] && validateNeuronPointer(sensoryInternal_[i])) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            current = std::clamp(current, -30.0f, 30.0f);
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const SensoryPercept& percept) {
    const auto& proprio = percept.getProprioception();
    size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (sensoryProprioception_[i] && validateNeuronPointer(sensoryProprioception_[i])) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            current = std::clamp(current, -25.0f, 25.0f);
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::updateNoveltyAndCuriosity(const SensoryPercept& percept) {
    const auto& vision = percept.getVision();
    
    if (vision.empty()) return;
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(vision.size(), previousVision_.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Add penalty for different lengths
    if (vision.size() != previousVision_.size()) {
        totalDiff += 10.0f;  // Bonus for significant changes in pattern length
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    
    // Store for next time
    previousVision_ = vision;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot decode motor command - brain pointer is null");
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
    // Calculate average activity in each motor group with bounds checking
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float sum = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n || !validateNeuronPointer(n)) continue;
            
            NeuronState state = n->getState();
            // Use membrane potential deviation from rest as activity measure
            float activity = std::abs(state.membranePotential - state.restingPotential);
            sum += activity;
            validNeurons++;
        }
        
        if (validNeurons == 0) return 0.0f;
        return sum / validNeurons;
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity with threshold
    struct CommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Only select action if there's meaningful activity (above threshold)
    for (const auto& c : commands) {
        if (c.activity > bestActivity && c.activity > 0.5f) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Additional safety check
    if (bestActivity < 0.1f) {
        NLM_LOG_DEBUG("AgentBrain: Motor activity below threshold, selecting wait");
        return MotorCommand::Wait;
    }
    
    NLM_LOG_DEBUG("AgentBrain: Selected motor command " + std::to_string(static_cast<int>(best)) +
                 " with activity " + std::to_string(bestActivity));
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        if (!brain_ || !brain_->getRandomGenerator()) {
            NLM_LOG_WARNING("AgentBrain: Cannot select with curiosity - no random generator");
            return defaultCmd;
        }
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with bias toward exploration
            int choice = brain_->getRandomGenerator()->uniformInt(0, 11); // Extended range for exploration
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Eat;
                case 8: return MotorCommand::Drink;
                case 9: return MotorCommand::Rest;
                case 10: return MotorCommand::Wait;
                case 11: return MotorCommand::Custom;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain: Cannot apply reward modulation - brain pointer is null");
        }
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("AgentBrain: Invalid reward value: " + std::to_string(reward));
        return;
    }
    
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_ERROR("AgentBrain: Invalid predicted reward value: " + std::to_string(predictedReward));
        return;
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
    applyRewardToSynapses(dopamineLevel_, plasticityModifier_);
    
    // Modulate plasticity based on dopamine
    updatePlasticityBasedOnDopamine(dopamineLevel_);
}

void AgentBrain::applyRewardToSynapses(float dopamineLevel, float plasticityModifier) {
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier;
                
                // Clamp delta to prevent unstable weight changes
                delta = std::clamp(delta, -1.0f, 1.0f);
                
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::updatePlasticityBasedOnDopamine(float dopamineLevel) {
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain: Cannot update development - brain pointer is null");
        }
        return;
    }
    
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("AgentBrain: Invalid timestep for development update");
        return;
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
            sp->setSynaptogenesisRate(std::max(0.0f, synRate));
            sp->setPruningRate(std::max(0.0f, pruneRate));
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot get developmental stage - brain pointer is null");
        return DevelopmentalStage::Initial;
    }
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
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

} // namespace nlm
