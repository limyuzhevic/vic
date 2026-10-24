#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain_(brain)
    , config_(config)
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
    , advancedActionsEnabled_(false)
    , sensoryNoveltyDecay_(0.99f)
    , motorActivityLevel_(0.0f)
    , sensoryProcessingLoad_(0.0f)
    , totalActionSelections_(0)
    , totalExploitationActions_(0)
    , totalExplorationActions_(0)
    , motorActivityThreshold_(0.5f)
    , curiosityExplorationThreshold_(0.3f)
    , maxRandomActionChance_(0.3f)
    , dopamineClampMin_(-1.0f)
    , dopamineClampMax_(1.0f)
    , developmentInitialPlasticity_(1.0f)
    , developmentCriticalPeriodPlasticity_(0.8f)
    , developmentMaturationPlasticity_(0.5f)
    , developmentAdultPlasticity_(0.2f)
    , developmentInitialAge_(0.0)
    , developmentCriticalPeriodAge_(60.0)
    , developmentMaturationAge_(300.0)
    , developmentAdultAge_(900.0)
    , eligibilityTraceThreshold_(0.001f)
    , eligibilityTraceDecay_(0.1f)
    , dopamineModulationWeight_(1.0f)
    , explorationStrategy_("curiosity")
    , exploitationStrategy_("greedy")
    , socialStrategy_("cooperative")
    , maxRandomActions_(7)
    , minActionRepeats_(3)
{
    // Load configuration parameters with defaults
    loadConfiguration();
    
    // Initialize motor and sensory neuron groups with enhanced safety checks
    if (brain_) {
        initializeNeuronGroups();
    }
    
    NLM_LOG_INFO("Advanced AgentBrain initialized with " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size()) + " motor neurons and " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size()) + " sensory neurons");
}

void AgentBrain::loadConfiguration() {
    if (!config_) {
        NLM_LOG_WARNING("No configuration provided to AgentBrain, using defaults");
        return;
    }
    
    // Load configuration parameters with type-safe access
    // Sensory processing parameters
    if (config_->has("agent.novelty.decay_rate")) {
        sensoryNoveltyDecay_ = config_->getOr<float>("agent.novelty.decay_rate", 0.99f);
    }
    
    // Motor selection parameters
    if (config_->has("agent.motor.activity_threshold")) {
        motorActivityThreshold_ = config_->getOr<float>("agent.motor.activity_threshold", 0.5f);
    }
    
    if (config_->has("agent.motor.exploration.curiosity_threshold")) {
        curiosityExplorationThreshold_ = config_->getOr<float>("agent.motor.exploration.curiosity_threshold", 0.3f);
    }
    
    if (config_->has("agent.motor.exploration.max_random_action_chance")) {
        maxRandomActionChance_ = config_->getOr<float>("agent.motor.exploration.max_random_action_chance", 0.3f);
    }
    
    // Neuromodulation parameters
    if (config_->has("agent.neuromod.dopamine.clamping.min")) {
        dopamineClampMin_ = config_->getOr<float>("agent.neuromod.dopamine.clamping.min", -1.0f);
    }
    
    if (config_->has("agent.neuromod.dopamine.clamping.max")) {
        dopamineClampMax_ = config_->getOr<float>("agent.neuromod.dopamine.clamping.max", 1.0f);
    }
    
    // Development parameters
    if (config_->has("agent.development.initial_plasticity")) {
        developmentInitialPlasticity_ = config_->getOr<float>("agent.development.initial_plasticity", 1.0f);
    }
    
    if (config_->has("agent.development.critical_period_plasticity")) {
        developmentCriticalPeriodPlasticity_ = config_->getOr<float>("agent.development.critical_period_plasticity", 0.8f);
    }
    
    if (config_->has("agent.development.maturation_plasticity")) {
        developmentMaturationPlasticity_ = config_->getOr<float>("agent.development.maturation_plasticity", 0.5f);
    }
    
    if (config_->has("agent.development.adult_plasticity")) {
        developmentAdultPlasticity_ = config_->getOr<float>("agent.development.adult_plasticity", 0.2f);
    }
    
    if (config_->has("agent.development.initial_age")) {
        developmentInitialAge_ = config_->getOr<double>("agent.development.initial_age", 0.0);
    }
    
    if (config_->has("agent.development.critical_period_age")) {
        developmentCriticalPeriodAge_ = config_->getOr<double>("agent.development.critical_period_age", 60.0);
    }
    
    if (config_->has("agent.development.maturation_age")) {
        developmentMaturationAge_ = config_->getOr<double>("agent.development.maturation_age", 300.0);
    }
    
    if (config_->has("agent.development.adult_age")) {
        developmentAdultAge_ = config_->getOr<double>("agent.development.adult_age", 900.0);
    }
    
    // Plasticity parameters
    if (config_->has("agent.plasticity.elgibility_trace_threshold")) {
        eligibilityTraceThreshold_ = config_->getOr<float>("agent.plasticity.elgibility_trace_threshold", 0.001f);
    }
    
    if (config_->has("agent.plasticity.eligibility_trace_decay")) {
        eligibilityTraceDecay_ = config_->getOr<float>("agent.plasticity.eligibility_trace_decay", 0.1f);
    }
    
    if (config_->has("agent.plasticity.dopamine.modulation")) {
        dopamineModulationWeight_ = config_->getOr<float>("agent.plasticity.dopamine.modulation", 1.0f);
    }
    
    // Enhanced exploration parameters
    if (config_->has("agent.exploration.exploration_strategy")) {
        explorationStrategy_ = config_->getOr<std::string>("agent.exploration.exploration_strategy", "curiosity");
    }
    
    if (config_->has("agent.exploration.max_random_actions")) {
        maxRandomActions_ = config_->getOr<int>("agent.exploration.max_random_actions", 7);
    }
    
    if (config_->has("agent.exploration.min_action_repeats")) {
        minActionRepeats_ = config_->getOr<int>("agent.exploration.min_action_repeats", 3);
    }
    
    // Advanced action configuration
    if (config_->has("agent.advanced_actions.enable")) {
        advancedActionsEnabled_ = config_->getOr<bool>("agent.advanced_actions.enable", false);
    }
    
    NLM_LOG_INFO("AgentBrain configuration loaded: " +
                 std::to_string(motorActivityThreshold_) + " motor threshold, " +
                 std::to_string(curiosityExplorationThreshold_) + " curiosity threshold, " +
                 std::to_string(maxRandomActionChance_) + " max explore chance");
}

void AgentBrain::initializeNeuronGroups() {
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered in brain initialization");
            continue;
        }
        
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_WARNING("Null population encountered in region initialization");
                continue;
            }
            
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_WARNING("Null neuron encountered in motor population");
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
                        default: NLM_LOG_WARNING("Unexpected distribution index: " + std::to_string(idx % 6)); break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_WARNING("Null neuron encountered in sensory population");
                        continue;
                    }
                    // Distribute sensory neurons with validation
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
                    
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(n); break;
                        case 1: sensoryTouch_.push_back(n); break;
                        case 2: sensoryInternal_.push_back(n); break;
                        case 3: sensoryProprioception_.push_back(n); break;
                        default: NLM_LOG_WARNING("Unexpected sensory distribution index: " + std::to_string(idx % 4)); break;
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("AgentBrain neuron groups initialized: " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + 
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size()) + " motor neurons and " +
                 std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                               sensoryInternal_.size() + sensoryProprioception_.size()) + " sensory neurons");
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
    // Safety check: ensure vectors are properly initialized before resetting
    if (previousVision_.size() != (sensoryVision_.size() > 0 ? 16 * 16 : 0)) {
        // Reallocate if sizes don't match expected pattern
        previousVision_.clear();
        if (sensoryVision_.size() > 0) {
            previousVision_.resize(16 * 16, 0.0f);
        }
    }
    
    // Reset all state variables safely
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Safely clear previous vision
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
}

} // namespace nlm
