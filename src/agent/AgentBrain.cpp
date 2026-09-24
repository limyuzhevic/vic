// Improved AgentBrain.cpp with enhanced error handling and logging
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
    try {
        // Initialize with error checking
        if (!brain_) {
            NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
            throw std::invalid_argument("Brain pointer cannot be null");
        }
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                    std::to_string(brain_->getTotalNeuronCount()) + " neurons");
        
        // Initialize motor and sensory neuron groups
        initializeNeuronGroups();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain initialization failed: ") + e.what());
        throw;
    }
}

void AgentBrain::initializeNeuronGroups() {
    // Validate brain has regions
    auto regions = brain_->getRegions();
    if (regions.empty()) {
        NLM_LOG_WARNING("No regions found in brain - agent will have no neurons");
        return;
    }
    
    // Initialize motor and sensory neuron groups with better error handling
    for (const auto& region : regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered in brain");
            continue;
        }
        
        auto populations = region->getPopulations();
        for (auto& pop : populations) {
            if (!pop) {
                NLM_LOG_WARNING("Null population encountered");
                continue;
            }
            
            NeuronType type = pop->getNeuronType();
            distributeNeurons(type, pop->getNeurons());
        }
    }
}

void AgentBrain::distributeNeurons(NeuronType type, const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return; // Nothing to distribute
    }
    
    switch (type) {
        case NeuronType::Motor:
            distributeMotorNeurons(neurons);
            break;
        case NeuronType::Sensory:
            distributeSensoryNeurons(neurons);
            break;
        default:
            // Internal and other types don't need special distribution
            break;
    }
}

void AgentBrain::distributeMotorNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("Null neuron encountered in motor distribution");
            continue;
        }
        
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
}

void AgentBrain::distributeSensoryNeurons(const std::vector<Neuron*>& neurons) {
    for (Neuron* n : neurons) {
        if (!n) {
            NLM_LOG_WARNING("Null neuron encountered in sensory distribution");
            continue;
        }
        
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

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        // Validate inputs
        if (!world.getVisionWidth() || !world.getVisionHeight()) {
            NLM_LOG_ERROR("Invalid world dimensions");
            throw std::invalid_argument("World dimensions cannot be zero");
        }
        
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Log successful initialization
        size_t totalSensory = sensoryVision_.size() + sensoryTouch_.size() +
                           sensoryInternal_.size() + sensoryProprioception_.size();
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                    std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                    std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                    std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                    std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
                    
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain initialization failed: ") + e.what());
        throw;
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
        NLM_LOG_WARNING("Cannot process sensory input: brain is null");
        return;
    }
    
    if (!percept.isValid()) {
        NLM_LOG_WARNING("Processing invalid sensory percept");
        return;
    }
    
    try {
        // Vision input processing
        processVisionInput(percept.getVision());
        
        // Touch input processing
        processTouchInput(percept.getTouch());
        
        // Internal signals processing
        processInternalInput(percept.getInternal());
        
        // Proprioception processing
        processProprioceptionInput(percept.getProprioception());
        
        // Compute novelty with error handling
        computeNovelty(percept.getVision());
        
        // Update curiosity based on novelty
        updateCuriosity();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Sensory processing failed: ") + e.what());
        // Continue with other processing even if one modality fails
    }
}

void AgentBrain::processVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = sensoryVision_[i];
        if (!neuron) {
            NLM_LOG_WARNING("Null vision neuron encountered during processing");
            continue;
        }
        
        // Inject current proportional to vision intensity
        float current = vision[i] * 5.0f;  // Scale factor
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = sensoryTouch_[i];
        if (!neuron) {
            NLM_LOG_WARNING("Null touch neuron encountered during processing");
            continue;
        }
        
        float current = touch[i] * 8.0f;  // Collision signal
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processInternalInput(const std::vector<float>& internal) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = sensoryInternal_[i];
        if (!neuron) {
            NLM_LOG_WARNING("Null internal neuron encountered during processing");
            continue;
        }
        
        float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
        neuron->injectCurrent(current);
    }
}

void AgentBrain::processProprioceptionInput(const std::vector<float>& proprioception) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        Neuron* neuron = sensoryProprioception_[i];
        if (!neuron) {
            NLM_LOG_WARNING("Null proprioception neuron encountered during processing");
            continue;
        }
        
        float current = (proprioception[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
        neuron->injectCurrent(current);
    }
}

void AgentBrain::computeNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
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

void AgentBrain::updateCuriosity() {
    if (!curiosityEnabled_) {
        curiosityLevel_ = 0.0f;
        return;
    }
    
    curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot decode motor command: brain is null");
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration with validation
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        MotorCommand curiousCmd = selectWithCuriosity(decoded);
        if (curiousCmd != MotorCommand::Wait) {
            return curiousCmd;
        }
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (!n) continue;  // Skip null neurons
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
    
    // Find maximum activity with tie-breaking logic
    struct CommandWithActivity {
        MotorCommand cmd;
        float activity;
    };
    
    CommandWithActivity commands[] = {
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
        } else if (c.activity == bestActivity && bestActivity > 0.5f) {
            // Tie-breaking: prefer simpler actions
            if (c.cmd == MotorCommand::MoveForward && best == MotorCommand::Wait) {
                best = c.cmd;
            }
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
        
        if (brain_) {
            float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            if (r < exploreChance) {
                // Random motor command with balanced exploration
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot apply reward modulation: brain is null");
        } else if (!rewardModulationEnabled_) {
            NLM_LOG_DEBUG("Reward modulation is disabled");
        }
        return;
    }
    
    try {
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to all synapses with eligibility traces
        applyRewardToSynapses();
        
        // Modulate plasticity based on dopamine
        modulatePlasticity();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Reward modulation failed: ") + e.what());
    }
}

void AgentBrain::applyRewardToSynapses() {
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_WARNING("Null synapse encountered during reward modulation");
                continue;
            }
            
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
}

void AgentBrain::modulatePlasticity() {
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
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("Cannot update development: brain is null");
        } else if (!developmentEnabled_) {
            NLM_LOG_DEBUG("Development is disabled");
        }
        return;
    }
    
    try {
        developmentalAge_ += timestep;
        
        // Simple developmental stages based on age
        // This is a biologically inspired approximation
        updateDevelopmentalStage();
        
        // Structural plasticity changes with development
        if (structuralPlasticityEnabled_) {
            updateStructuralPlasticity();
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Development update failed: ") + e.what());
    }
}

void AgentBrain::updateDevelopmentalStage() {
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
}

void AgentBrain::updateStructuralPlasticity() {
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        // Higher synaptogenesis in early development
        float synRate = 0.0001f * plasticityModifier_;
        float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
        sp->setSynaptogenesisRate(synRate);
        sp->setPruningRate(pruneRate);
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot get developmental stage: brain is null");
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
    try {
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
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("AgentBrain reset failed: ") + e.what());
    }
}

} // namespace nlm
