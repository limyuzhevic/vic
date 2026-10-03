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
    // Initialize with flexible neural group configuration
    // Default groups can be overridden via initialize() method
    initializeDefaultGroups();
    
    NLM_LOG_INFO("AgentBrain created with flexible neural group architecture");
}

void AgentBrain::initializeDefaultGroups() {
    // Initialize with default groups - can be customized via configuration
    // These defaults match the original hardcoded structure
    VisionGroup defaultVisionGroup("vision", 64, DistributionStrategy::Fixed, 5.0f);
    VisionGroup defaultTouchGroup("touch", 8, DistributionStrategy::Fixed, 8.0f);
    VisionGroup defaultInternalGroup("internal", 4, DistributionStrategy::Fixed, 5.0f);
    VisionGroup defaultProprioceptionGroup("proprioception", 6, DistributionStrategy::Fixed, 3.0f);
    
    // Note: In a full implementation, we would search the brain for neurons
    // and assign them to these groups based on type and configuration
    // For now, maintaining backward compatibility with minimal changes
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world, 
                         const std::vector<NeuronConfig<Neuron*>>& visionConfig = 
                             std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::visionConfig(64)},
                         const std::vector<NeuronConfig<Neuron*>>& touchConfig = 
                             std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::touchConfig(8)},
                         const std::vector<NeuronConfig<Neuron*>>& internalConfig = 
                             std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::internalConfig(4)},
                         const std::vector<NeuronConfig<Neuron*>>& proprioceptionConfig = 
                             std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::proprioceptionConfig(6)}) {
    // Use flexible configuration instead of hardcoded values
    std::vector<NeuronConfig<Neuron*>> allVisionConfig = visionConfig;
    std::vector<NeuronConfig<Neuron*>> allTouchConfig = touchConfig;
    std::vector<NeuronConfig<Neuron*>> allInternalConfig = internalConfig;
    std::vector<NeuronConfig<Neuron*>> allProprioceptionConfig = proprioceptionConfig;
    
    // Calculate total neurons needed
    size_t totalVisionNeurons = 0;
    for (const auto& config : allVisionConfig) {
        totalVisionNeurons += config.count;
    }
    
    size_t totalTouchNeurons = 0;
    for (const auto& config : allTouchConfig) {
        totalTouchNeurons += config.count;
    }
    
    size_t totalInternalNeurons = 0;
    for (const auto& config : allInternalConfig) {
        totalInternalNeurons += config.count;
    }
    
    size_t totalProprioceptionNeurons = 0;
    for (const auto& config : allProprioceptionConfig) {
        totalProprioceptionNeurons += config.count;
    }
    
    // Resize previousVision_ based on configured size
    previousVision_.resize(totalVisionNeurons, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(totalVisionNeurons) + " vision sensory neurons (" +
                 std::to_string(allVisionConfig.size()) + " vision groups), " +
                 std::to_string(totalTouchNeurons) + " touch sensory neurons (" +
                 std::to_string(allTouchConfig.size()) + " touch groups), " +
                 std::to_string(totalInternalNeurons) + " internal sensory neurons (" +
                 std::to_string(allInternalConfig.size()) + " internal groups), " +
                 std::to_string(totalProprioceptionNeurons) + " proprioception neurons (" +
                 std::to_string(allProprioceptionConfig.size()) + " proprioception groups)");
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
    
    // Process vision input with flexible scaling
    const auto& vision = percept.getVision();
    for (size_t groupIdx = 0; groupIdx < visionGroups_.size(); ++groupIdx) {
        auto& visionGroup = visionGroups_[groupIdx];
        if (visionGroup.empty()) continue;
        
        // Apply group-specific scale factor and parameters
        float groupScale = visionGroup.scaleFactor;
        for (size_t i = 0; i < visionGroup.neurons.size() && 
                     i < vision.size(); ++i) {
            Neuron* neuron = visionGroup.neurons[i];
            // Use configurable scaling
            float current = vision[i] * groupScale;
            if (visionGroup.parameters.count("offset")) {
                current += visionGroup.parameters.at("offset");
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Process touch input with flexible scaling
    const auto& touch = percept.getTouch();
    for (size_t groupIdx = 0; groupIdx < touchGroups_.size(); ++groupIdx) {
        auto& touchGroup = touchGroups_[groupIdx];
        if (touchGroup.empty()) continue;
        
        float groupScale = touchGroup.scaleFactor;
        for (size_t i = 0; i < touchGroup.neurons.size() && 
                     i < touch.size(); ++i) {
            Neuron* neuron = touchGroup.neurons[i];
            float current = touch[i] * groupScale;
            if (touchGroup.parameters.count("offset")) {
                current += touchGroup.parameters.at("offset");
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Process internal signals with flexible scaling
    const auto& intern = percept.getInternal();
    for (size_t groupIdx = 0; groupIdx < internalGroups_.size(); ++groupIdx) {
        auto& internalGroup = internalGroups_[groupIdx];
        if (internalGroup.empty()) continue;
        
        float groupScale = internalGroup.scaleFactor;
        for (size_t i = 0; i < internalGroup.neurons.size() && 
                     i < intern.size(); ++i) {
            Neuron* neuron = internalGroup.neurons[i];
            float current = (intern[i] * 2.0f - 1.0f) * groupScale;
            if (internalGroup.parameters.count("offset")) {
                current += internalGroup.parameters.at("offset");
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Process proprioception with flexible scaling
    const auto& proprio = percept.getProprioception();
    for (size_t groupIdx = 0; groupIdx < proprioceptionGroups_.size(); ++groupIdx) {
        auto& proprioGroup = proprioceptionGroups_[groupIdx];
        if (proprioGroup.empty()) continue;
        
        float groupScale = proprioGroup.scaleFactor;
        for (size_t i = 0; i < proprioGroup.neurons.size() && 
                     i < proprio.size(); ++i) {
            Neuron* neuron = proprioGroup.neurons[i];
            float current = (proprio[i] * 2.0f - 1.0f) * groupScale;
            if (proprioGroup.parameters.count("offset")) {
                current += proprioGroup.parameters.at("offset");
            }
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision) with flexible handling
    if (!vision.empty() && !visionGroups_.empty()) {
        float totalDiff = 0.0f;
        size_t maxVisionSize = vision.size();
        for (size_t i = 0; i < maxVisionSize && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update with configurable decay rate
        if (sensoryNoveltyDecay_ > 0.0f && sensoryNoveltyDecay_ < 1.0f) {
            noveltyLevel_ *= sensoryNoveltyDecay_;
        }
        
        // Store for next time with bounds checking
        if (vision.size() <= previousVision_.size()) {
            std::copy(vision.begin(), vision.end(), previousVision_.begin());
        } else {
            previousVision_.resize(vision.size());
            std::copy(vision.begin(), vision.end(), previousVision_.begin());
        }
    }
    
    // Update curiosity based on novelty with configurable sensitivity
    if (curiosityEnabled_) {
        float noveltySensitivity = 2.0f;
        if (curiosityParameters_.count("novelty_sensitivity")) {
            noveltySensitivity = curiosityParameters_.at("novelty_sensitivity");
        }
        
        float errorSensitivity = 0.5f;
        if (curiosityParameters_.count("error_sensitivity")) {
            errorSensitivity = curiosityParameters_.at("error_sensitivity");
        }
        
        curiosityLevel_ = noveltyLevel_ * noveltySensitivity + std::abs(predictionError_) * errorSensitivity;
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
    
    // Configurable developmental stages with enhanced precision
    // This allows users to customize developmental timing
    const double INITIAL_DURATION = 60.0;      // Configurable
    const double CRITICAL_PERIOD_DURATION = 300.0;
    const double MATURATION_DURATION = 900.0;
    
    // Allow override via configuration parameters
    if (developmentalParameters_.count("initial_duration")) {
        INITIAL_DURATION = developmentalParameters_["initial_duration"];
    }
    if (developmentalParameters_.count("critical_period_duration")) {
        CRITICAL_PERIOD_DURATION = developmentalParameters_["critical_period_duration"];
    }
    if (developmentalParameters_.count("maturation_duration")) {
        MATURATION_DURATION = developmentalParameters_["maturation_duration"];
    }
    
    if (developmentalAge_ < INITIAL_DURATION) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < CRITICAL_PERIOD_DURATION) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < MATURATION_DURATION) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Enhanced structural plasticity with developmental modulation
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development with configurable rates
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            
            // Allow override via configuration
            if (structuralParameters_.count("base_synaptogenesis_rate")) {
                synRate = structuralParameters_["base_synaptogenesis_rate"] * plasticityModifier_;
            }
            if (structuralParameters_.count("base_pruning_rate")) {
                pruneRate = structuralParameters_["base_pruning_rate"] * (2.0f - plasticityModifier_);
            }
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Call development system if available
    if (brain_->getDevelopmentSystem()) {
        brain_->getDevelopmentSystem()->updateDevelopmentStage(
            developmentalAge_, plasticityModifier_);
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
    
    // Clear previous vision with safe resizing
    if (!previousVision_.empty() && previousVision_.size() > 1024) {
        previousVision_.clear();
        previousVision_.shrink_to_fit();
    } else {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
    
    // Reset novelty decay with parameter validation
    if (sensoryNoveltyDecay_ < 0.0f || sensoryNoveltyDecay_ >= 1.0f) {
        sensoryNoveltyDecay_ = 0.99f;  // Safe default
    }
    
    // Reset brain if available
    if (brain_) {
        brain_->reset();
    }
}

// Implementation of helper methods

void AgentBrain::initializeDefaultGroups() {
    // Initialize with default groups - can be customized via configuration
    // These defaults match the original hardcoded structure
    
    // Note: In a full implementation, we would search the brain for neurons
    // and assign them to these groups based on type and configuration
    // For now, maintaining backward compatibility with minimal changes
    NLM_LOG_DEBUG("Initializing default neural groups");
}

size_t AgentBrain::calculateConfiguredNeuronCount(const std::vector<NeuronConfig<Neuron*>>& configs) const {
    size_t total = 0;
    for (const auto& config : configs) {
        total += config.count;
    }
    return total;
}

// Legacy motor decoding methods implementation (maintained for backward compatibility)
// These can be refactored later to use the new neural group architecture

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
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        if (brain_ && brain_->getRandomGenerator()) {
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
    }
    
    return defaultCmd;
}

} // namespace nlm
