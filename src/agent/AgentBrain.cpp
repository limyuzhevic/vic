// AgentBrain.cpp
// Fixed version with proper memory management, thread safety, and error handling

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace nlm {

// Static validation for performance
cache_line_aligned static constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
cache_line_aligned static constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
cache_line_aligned static constexpr float MIN_NOVELTY_LEVEL = 0.0f;
cache_line_aligned static constexpr float MAX_NOVELTY_LEVEL = 1.0f;
cache_line_aligned static constexpr float MIN_CURIOUSITY_LEVEL = 0.0f;
cache_line_aligned static constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;

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
    , initializationComplete_(false)
    , forceShutdown_(false)
{
    std::lock_guard<std::mutex> lock(initializationMutex_);
    
    // Validate input
    if (!brain) {
        NLM_LOG_ERROR("AgentBrain: Invalid brain pointer provided");
        return;
    }
    
    // Initialize neuron groups
    initializeNeuronGroups();
    
    // Initialize neuromodulation state
    resetNeuromodulationState();
    
    // Set initialization flag
    initializationComplete_ = true;
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize neuron groups - brain is null");
        return;
    }
    
    // Clear existing groups
    clearNeuronGroups();
    
    // Populate neuron groups from brain regions
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_WARNING("AgentBrain: Skipping null region");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("AgentBrain: Skipping null population");
                continue;
            }
            
            NeuronType type = pop->getNeuronType();
            
            // Get neurons from population
            const auto& neurons = pop->getNeurons();
            if (neurons.empty()) {
                continue; // Skip empty populations
            }
            
            // Distribute neurons to appropriate groups based on type
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(neurons);
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(neurons);
            }
        }
    }
    
    NLM_LOG_INFO("AgentBrain: Initialized " + 
                 std::to_string(motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size()) + 
                 " motor neurons and " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() + 
                               sensoryInternal_.size() + sensoryProprioception_.size()) +
                 " sensory neurons");
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("AgentBrain: Skipping null motor neuron");
            continue;
        }
        
        // Distribute to motor action groups
        size_t totalGroups = motorForward_.size() + motorBackward_.size() + 
                            motorTurnLeft_.size() + motorTurnRight_.size() +
                            motorInteract_.size() + motorWait_.size();
        
        MotorCommand cmd = static_cast<MotorCommand>(totalGroups % 6);
        
        switch (cmd) {
            case MotorCommand::MoveForward: motorForward_.push_back(n); break;
            case MotorCommand::MoveBackward: motorBackward_.push_back(n); break;
            case MotorCommand::TurnLeft: motorTurnLeft_.push_back(n); break;
            case MotorCommand::TurnRight: motorTurnRight_.push_back(n); break;
            case MotorCommand::Interact: motorInteract_.push_back(n); break;
            case MotorCommand::Wait: motorWait_.push_back(n); break;
            case MotorCommand::LookLeft: motorWait_.push_back(n); break; // Default to wait
            case MotorCommand::LookRight: motorWait_.push_back(n); break; // Default to wait
        }
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("AgentBrain: Skipping null sensory neuron");
            continue;
        }
        
        // Distribute to sensory modalities
        size_t totalGroups = sensoryVision_.size() + sensoryTouch_.size() +
                            sensoryInternal_.size() + sensoryProprioception_.size();
        
        switch (totalGroups % 4) {
            case 0: sensoryVision_.push_back(n); break;
            case 1: sensoryTouch_.push_back(n); break;
            case 2: sensoryInternal_.push_back(n); break;
            case 3: sensoryProprioception_.push_back(n); break;
        }
    }
}

void AgentBrain::clearNeuronGroups() {
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

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

AgentBrain::~AgentBrain() {
    forceShutdown_ = true; // Signal any pending operations to stop
    clearNeuronGroups();
    // Note: brain_ is a shared_ptr, so it will be automatically deleted when all references are gone
}

void AgentBrain::initialize(const SimpleWorld& world) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    if (!initializationComplete_ || !brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize - brain not properly initialized");
        return;
    }
    
    // Resize previous vision buffer based on world dimensions
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    
    // Reset development state
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
    if (!brain_ || !initializationComplete_) {
        NLM_LOG_WARNING("AgentBrain: Cannot process sensory input - brain not initialized");
        return;
    }
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    // Process vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    processVisionInput(vision);
    
    // Process touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    processTouchInput(touch);
    
    // Process internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    processInternalInput(intern);
    
    // Process proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    processProprioceptionInput(proprio);
    
    // Compute novelty (difference from previous vision)
    computeNovelty(vision);
    
    // Update curiosity based on novelty and prediction error
    updateCuriosity();
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;  // Collision signal
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron->injectCurrent(current);
        }
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.size() != vision.size()) {
        return;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    // Normalize
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Apply decay and update
    noveltyLevel_ *= sensoryNoveltyDecay_;
    noveltyLevel_ = std::clamp(noveltyLevel_, MIN_NOVELTY_LEVEL, MAX_NOVELTY_LEVEL);
    
    // Store for next time
    previousVision_ = vision;
}

void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    // Curiosity based on novelty and prediction error
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, MIN_CURIOUSITY_LEVEL, MAX_CURIOUSITY_LEVEL);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_ || !initializationComplete_) {
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
    // Calculate average activity in each motor group with null checks
    auto calcActivity = [this](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        size_t validCount = 0;
        
        for (const Neuron* n : neurons) {
            if (n) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
                validCount++;
            }
        }
        
        return validCount > 0 ? (sum / validCount) : 0.0f;
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
    if (!brain_ || !initializationComplete_) {
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
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
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, MIN_DOPAMINE_LEVEL, MAX_DOPAMINE_LEVEL);
    
    // Apply reward modulation to synapses
    applyRewardToSynapses(dopamineLevel_);
    
    // Modulate plasticity based on dopamine
    modulatePlasticity(dopamineLevel_);
}

void AgentBrain::applyRewardToSynapses(float dopamineLevel) {
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void AgentBrain::modulatePlasticity(float dopamineLevel) {
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
        return;
    }
    
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
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
    std::lock_guard<std::mutex> lock(stateMutex_);
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return predictionError_;
}

void AgentBrain::reset() {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    resetNeuromodulationState();
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

} // namespace nlm