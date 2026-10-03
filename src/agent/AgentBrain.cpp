#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// NeuronGroup implementation

NeuronGroup::NeuronGroup(const std::string& name, NeuronGroupType type,
                         size_t capacity, bool active)
    : name_(name), type_(type), neurons_(capacity), active_(active) {
    if (name.empty()) {
        throw std::invalid_argument("NeuronGroup name cannot be empty");
    }
}

NeuronGroup::~NeuronGroup() {
    clear();
}

NeuronGroup::NeuronGroup(NeuronGroup&& other) noexcept
    : name_(std::move(other.name_)), type_(other.type_),
      neurons_(std::move(other.neurons_)), active_(other.active_) {
    other.active_ = false;
}

NeuronGroup& NeuronGroup::operator=(NeuronGroup&& other) noexcept {
    if (this != &other) {
        clear();
        name_ = std::move(other.name_);
        type_ = other.type_;
        neurons_ = std::move(other.neurons_);
        active_ = other.active_;
        other.active_ = false;
    }
    return *this;
}

Neuron* NeuronGroup::at(size_t index) {
    validateNeuronGroupIndex(index);
    return neurons_[index];
}

const Neuron* NeuronGroup::at(size_t index) const {
    validateNeuronGroupIndex(index);
    return neurons_[index];
}

void NeuronGroup::addNeuron(Neuron* neuron) {
    if (!neuron) {
        throw std::invalid_argument("Neuron cannot be null");
    }
    neurons_.push_back(neuron);
}

void NeuronGroup::removeNeuron(Neuron* neuron) {
    auto it = std::find(neurons_.begin(), neurons_.end(), neuron);
    if (it != neurons_.end()) {
        neurons_.erase(it);
    }
}

void NeuronGroup::clear() {
    neurons_.clear();
}

void NeuronGroup::injectCurrentToAll(float current) const {
    for (Neuron* neuron : neurons_) {
        if (neuron) {
            neuron->injectCurrent(current);
        }
    }
}

float NeuronGroup::calculateAverageActivity() const {
    if (neurons_.empty()) return 0.0f;
    float total = calculateTotalActivity();
    return total / neurons_.size();
}

float NeuronGroup::calculateTotalActivity() const {
    float total = 0.0f;
    for (Neuron* neuron : neurons_) {
        if (neuron) {
            total += calculateNeuronActivity(neuron);
        }
    }
    return total;
}

bool NeuronGroup::isValid() const {
    if (neurons_.empty()) return false;
    for (Neuron* neuron : neurons_) {
        if (!neuron) return false;
    }
    return true;
}

float NeuronGroup::calculateNeuronActivity(Neuron* neuron) const {
    if (!neuron) return 0.0f;
    const auto& state = neuron->getState();
    return std::abs(state.membranePotential - state.restingPotential);
}

void NeuronGroup::validateNeuronGroupIndex(size_t index) const {
    if (index >= neurons_.size()) {
        throw std::out_of_range("NeuronGroup index out of range");
    }
}

// AgentBrain implementation

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , dopamineLevel_(AgentBrainConfig::DEFAULT_DOPAMINE_LEVEL)
    , noveltyLevel_(AgentBrainConfig::DEFAULT_NOVELTY_LEVEL)
    , curiosityLevel_(AgentBrainConfig::DEFAULT_CURIOUSITY_LEVEL)
    , predictionError_(AgentBrainConfig::DEFAULT_PREDICTION_ERROR)
    , expectedReward_(AgentBrainConfig::DEFAULT_EXPECTED_REWARD)
    , developmentalAge_(AgentBrainConfig::INITIAL_DEVELOPMENTAL_AGE)
    , plasticityModifier_(AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER)
    , rewardModulationEnabled_(AgentBrainConfig::DEFAULT_REWARD_MODULATION_ENABLED)
    , structuralPlasticityEnabled_(AgentBrainConfig::DEFAULT_STRUCTURAL_PLASTICITY_ENABLED)
    , developmentEnabled_(AgentBrainConfig::DEFAULT_DEVELOPMENT_ENABLED)
    , curiosityEnabled_(AgentBrainConfig::DEFAULT_CURIOUSITY_ENABLED)
    , sensoryNoveltyDecay_(AgentBrainConfig::DEFAULT_NOVELTY_DECAY) {
    // Initialize neuron groups
    initializeNeuronGroups();
    
    // Validate parameters
    validateParameters();
}

AgentBrain::~AgentBrain() = default;

AgentBrain::AgentBrain(AgentBrain&& other) noexcept
    : brain_(std::move(other.brain_))
    , neuronGroups_(std::move(other.neuronGroups_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , previousVision_(std::move(other.previousVision_))
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_) {
    // Clear other state
    other.dopamineLevel_ = AgentBrainConfig::DEFAULT_DOPAMINE_LEVEL;
    other.noveltyLevel_ = AgentBrainConfig::DEFAULT_NOVELTY_LEVEL;
    other.curiosityLevel_ = AgentBrainConfig::DEFAULT_CURIOUSITY_LEVEL;
    other.predictionError_ = AgentBrainConfig::DEFAULT_PREDICTION_ERROR;
    other.expectedReward_ = AgentBrainConfig::DEFAULT_EXPECTED_REWARD;
    other.developmentalAge_ = AgentBrainConfig::INITIAL_DEVELOPMENTAL_AGE;
    other.plasticityModifier_ = AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER;
    other.rewardModulationEnabled_ = AgentBrainConfig::DEFAULT_REWARD_MODULATION_ENABLED;
    other.structuralPlasticityEnabled_ = AgentBrainConfig::DEFAULT_STRUCTURAL_PLASTICITY_ENABLED;
    other.developmentEnabled_ = AgentBrainConfig::DEFAULT_DEVELOPMENT_ENABLED;
    other.curiosityEnabled_ = AgentBrainConfig::DEFAULT_CURIOUSITY_ENABLED;
    other.sensoryNoveltyDecay_ = AgentBrainConfig::DEFAULT_NOVELTY_DECAY;
}

AgentBrain& AgentBrain::operator=(AgentBrain&& other) noexcept {
    if (this != &other) {
        // Clean up current state
        for (auto& pair : neuronGroups_) {
            pair.second.clear();
        }
        previousVision_.clear();
        
        // Move from other
        brain_ = std::move(other.brain_);
        neuronGroups_ = std::move(other.neuronGroups_);
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        previousVision_ = std::move(other.previousVision_);
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        
        // Reset other state
        other.dopamineLevel_ = AgentBrainConfig::DEFAULT_DOPAMINE_LEVEL;
        other.noveltyLevel_ = AgentBrainConfig::DEFAULT_NOVELTY_LEVEL;
        other.curiosityLevel_ = AgentBrainConfig::DEFAULT_CURIOUSITY_LEVEL;
        other.predictionError_ = AgentBrainConfig::DEFAULT_PREDICTION_ERROR;
        other.expectedReward_ = AgentBrainConfig::DEFAULT_EXPECTED_REWARD;
        other.developmentalAge_ = AgentBrainConfig::INITIAL_DEVELOPMENTAL_AGE;
        other.plasticityModifier_ = AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER;
        other.rewardModulationEnabled_ = AgentBrainConfig::DEFAULT_REWARD_MODULATION_ENABLED;
        other.structuralPlasticityEnabled_ = AgentBrainConfig::DEFAULT_STRUCTURAL_PLASTICITY_ENABLED;
        other.developmentEnabled_ = AgentBrainConfig::DEFAULT_DEVELOPMENT_ENABLED;
        other.curiosityEnabled_ = AgentBrainConfig::DEFAULT_CURIOUSITY_ENABLED;
        other.sensoryNoveltyDecay_ = AgentBrainConfig::DEFAULT_NOVELTY_DECAY;
    }
    return *this;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Validate world
    if (!validateBrain()) {
        NLM_LOG_WARNING("AgentBrain: Invalid brain state during initialization");
        return;
    }
    
    // Initialize sensory state
    resetSensoryState(world);
    
    // Reset development state
    resetDevelopmentState();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(getNeuronGroup(NeuronGroupType::SensoryVision).size()) + " vision sensory neurons, " +
                 std::to_string(getNeuronGroup(NeuronGroupType::SensoryTouch).size()) + " touch sensory neurons, " +
                 std::to_string(getNeuronGroup(NeuronGroupType::SensoryInternal).size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return AgentBrainConfig::VISION_SIZE + 
           AgentBrainConfig::TOUCH_SENSOR_COUNT + 
           AgentBrainConfig::INTERNAL_SENSOR_COUNT + 
           AgentBrainConfig::PROPRIOCEPTION_COUNT;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentBrainConfig::MOTOR_ACTION_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    validateBrain();
    
    // Validate percept
    if (!validatePercept(percept)) {
        NLM_LOG_WARNING("AgentBrain: Invalid sensory percept");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    auto& visionGroup = getNeuronGroup(NeuronGroupType::SensoryVision);
    
    for (size_t i = 0; i < visionGroup.size() && i < vision.size(); ++i) {
        Neuron* neuron = visionGroup.at(i);
        if (neuron) {
            // Inject current proportional to vision intensity
            float current = vision[i] * AgentBrainConfig::VISION_SCALE_FACTOR;
            neuron->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    auto& touchGroup = getNeuronGroup(NeuronGroupType::SensoryTouch);
    
    for (size_t i = 0; i < touchGroup.size() && i < touch.size(); ++i) {
        Neuron* neuron = touchGroup.at(i);
        if (neuron) {
            float current = touch[i] * AgentBrainConfig::TOUCH_SCALE_FACTOR;
            neuron->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    auto& internalGroup = getNeuronGroup(NeuronGroupType::SensoryInternal);
    
    for (size_t i = 0; i < internalGroup.size() && i < intern.size(); ++i) {
        Neuron* neuron = internalGroup.at(i);
        if (neuron) {
            float current = (intern[i] * 2.0f - 1.0f) * AgentBrainConfig::INTERNAL_SCALE_FACTOR;
            neuron->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    auto& proprioceptionGroup = getNeuronGroup(NeuronGroupType::SensoryProprioception);
    
    for (size_t i = 0; i < proprioceptionGroup.size() && i < proprio.size(); ++i) {
        Neuron* neuron = proprioceptionGroup.at(i);
        if (neuron) {
            float current = (proprio[i] * 2.0f - 1.0f) * AgentBrainConfig::PROPRIOCEPTION_SCALE_FACTOR;
            neuron->injectCurrent(current);
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
        curiosityLevel_ = noveltyLevel_ * AgentBrainConfig::NOVELTY_SENSITIVITY + 
                         std::abs(predictionError_) * AgentBrainConfig::PREDICTION_ERROR_SENSITIVITY;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!validateBrain()) {
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConfig::MIN_MEAN_ACTIVITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [this](NeuronGroupType type) -> float {
        const auto& group = getNeuronGroupInternal(type);
        if (group.empty()) return 0.0f;
        float sum = 0.0f;
        for (size_t i = 0; i < group.size(); ++i) {
            Neuron* neuron = group.at(i);
            if (neuron) {
                const auto& state = neuron->getState();
                sum += std::abs(state.membranePotential - state.restingPotential);
            }
        }
        return sum / group.size();
    };
    
    float forwardAct = calcActivity(NeuronGroupType::MotorForward);
    float backwardAct = calcActivity(NeuronGroupType::MotorBackward);
    float leftAct = calcActivity(NeuronGroupType::MotorTurnLeft);
    float rightAct = calcActivity(NeuronGroupType::MotorTurnRight);
    float interactAct = calcActivity(NeuronGroupType::MotorInteract);
    float waitAct = calcActivity(NeuronGroupType::MotorWait);
    
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
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConfig::MIN_MEAN_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConfig::EXPLORATION_BASE_CHANCE;  // Up to 30% random
        
        if (brain_) {
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    validateBrain();
    
    if (!rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    updateDopamineLevel(predictionError_);
    
    // Apply to all synapses with eligibility traces
    if (brain_) {
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
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = getPlasticityFactorFromDopamine(dopamineLevel_);
    
    // Apply to STDP
    if (brain_) {
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!validateBrain() || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Update developmental stage
    updateDevelopmentalStage(timestep);
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_ && brain_) {
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
    if (!validateBrain()) return DevelopmentalStage::Initial;
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
    resetNeuromodulationState();
    resetDevelopmentState();
    // Note: sensory state is reset during initialize()
}

// Private helper methods implementation

void AgentBrain::initializeNeuronGroups() {
    // Motor groups
    neuronGroups_[NeuronGroupType::MotorForward] = NeuronGroup("MotorForward", NeuronGroupType::MotorForward);
    neuronGroups_[NeuronGroupType::MotorBackward] = NeuronGroup("MotorBackward", NeuronGroupType::MotorBackward);
    neuronGroups_[NeuronGroupType::MotorTurnLeft] = NeuronGroup("MotorTurnLeft", NeuronGroupType::MotorTurnLeft);
    neuronGroups_[NeuronGroupType::MotorTurnRight] = NeuronGroup("MotorTurnRight", NeuronGroupType::MotorTurnRight);
    neuronGroups_[NeuronGroupType::MotorInteract] = NeuronGroup("MotorInteract", NeuronGroupType::MotorInteract);
    neuronGroups_[NeuronGroupType::MotorWait] = NeuronGroup("MotorWait", NeuronGroupType::MotorWait);
    
    // Sensory groups
    neuronGroups_[NeuronGroupType::SensoryVision] = NeuronGroup("SensoryVision", NeuronGroupType::SensoryVision);
    neuronGroups_[NeuronGroupType::SensoryTouch] = NeuronGroup("SensoryTouch", NeuronGroupType::SensoryTouch);
    neuronGroups_[NeuronGroupType::SensoryInternal] = NeuronGroup("SensoryInternal", NeuronGroupType::SensoryInternal);
    neuronGroups_[NeuronGroupType::SensoryProprioception] = NeuronGroup("SensoryProprioception", NeuronGroupType::SensoryProprioception);
}

NeuronGroup& AgentBrain::getOrCreateNeuronGroup(NeuronGroupType type) {
    auto it = neuronGroups_.find(type);
    if (it != neuronGroups_.end()) {
        return it->second;
    }
    // This should not happen if initializeNeuronGroups() was called
    return neuronGroups_[type]; // Create if not exists
}

const NeuronGroup& AgentBrain::getNeuronGroupInternal(NeuronGroupType type) const {
    auto it = neuronGroups_.find(type);
    if (it == neuronGroups_.end()) {
        throw std::out_of_range("NeuronGroup type not found");
    }
    return it->second;
}

NeuronGroup& AgentBrain::getNeuronGroup(NeuronGroupType type) {
    return const_cast<NeuronGroup&>(static_cast<const AgentBrain*>(this)->getNeuronGroupInternal(type));
}

const NeuronGroup& AgentBrain::getNeuronGroup(NeuronGroupType type) const {
    return getNeuronGroupInternal(type);
}

std::vector<NeuronGroupType> AgentBrain::getActiveNeuronGroups() const {
    std::vector<NeuronGroupType> activeGroups;
    for (const auto& pair : neuronGroups_) {
        if (pair.second.isActive() && pair.second.isValid()) {
            activeGroups.push_back(pair.first);
        }
    }
    return activeGroups;
}

void AgentBrain::validateBrain() const {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain: Brain pointer is null");
    }
}

bool AgentBrain::validateNeuronGroupIndex(size_t index) const {
    return index < AgentBrainConfig::MOTOR_GROUP_COUNT + AgentBrainConfig::SENSORY_GROUP_COUNT;
}

bool AgentBrain::validateSensoryIndex(size_t groupIndex, size_t signalIndex) const {
    if (groupIndex >= AgentBrainConfig::SENSORY_GROUP_COUNT) {
        return false;
    }
    
    switch (groupIndex) {
        case 0: // Vision
            return signalIndex < AgentBrainConfig::VISION_SIZE;
        case 1: // Touch
            return signalIndex < AgentBrainConfig::TOUCH_SENSOR_COUNT;
        case 2: // Internal
            return signalIndex < AgentBrainConfig::INTERNAL_SENSOR_COUNT;
        case 3: // Proprioception
            return signalIndex < AgentBrainConfig::PROPRIOCEPTION_COUNT;
        default:
            return false;
    }
}

void AgentBrain::updateDopamineLevel(float predictionError) {
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, 
                                AgentBrainConfig::PREDICTION_ERROR_CLAMP_MIN,
                                AgentBrainConfig::PREDICTION_ERROR_CLAMP_MAX);
}

void AgentBrain::updateNoveltyLevel(const std::vector<float>& vision) {
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision_[i]);
        totalDiff += diff;
    }
    
    noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel_ *= sensoryNoveltyDecay_;
}

void AgentBrain::updateCuriosityLevel() {
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * AgentBrainConfig::NOVELTY_SENSITIVITY + 
                         std::abs(predictionError_) * AgentBrainConfig::PREDICTION_ERROR_SENSITIVITY;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::updateDevelopmentalStage(double timestep) {
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < AgentBrainConfig::INITIAL_STAGE_DURATION) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
    } else if (developmentalAge_ < AgentBrainConfig::CRITICAL_PERIOD_DURATION) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }
    } else if (developmentalAge_ < AgentBrainConfig::MATURATION_DURATION) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        if (brain_) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
}

void AgentBrain::updatePlasticityModifier() {
    // Placeholder for plasticity updates
}

float AgentBrain::getPlasticityFactorFromDopamine(float dopamineLevel) {
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
    return std::clamp(plasticityFactor, 
                     AgentBrainConfig::PLASTICITY_FACTOR_CLAMP_MIN,
                     AgentBrainConfig::PLASTICITY_FACTOR_CLAMP_MAX);
}

float AgentBrain::calculateExplorationChance(float curiosityLevel) {
    return curiosityLevel * AgentBrainConfig::EXPLORATION_BASE_CHANCE;
}

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = AgentBrainConfig::DEFAULT_DOPAMINE_LEVEL;
    noveltyLevel_ = AgentBrainConfig::DEFAULT_NOVELTY_LEVEL;
    curiosityLevel_ = AgentBrainConfig::DEFAULT_CURIOUSITY_LEVEL;
    predictionError_ = AgentBrainConfig::DEFAULT_PREDICTION_ERROR;
    expectedReward_ = AgentBrainConfig::DEFAULT_EXPECTED_REWARD;
}

void AgentBrain::resetDevelopmentState() {
    developmentalAge_ = AgentBrainConfig::INITIAL_DEVELOPMENTAL_AGE;
    plasticityModifier_ = AgentBrainConfig::INITIAL_PLASTICITY_MODIFIER;
    updateDevelopmentalStage(0.0);
}

void AgentBrain::resetSensoryState(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
}

void AgentBrain::validateParameters() const {
    // Validate configuration parameters
    if (sensoryNoveltyDecay_ <= 0.0f || sensoryNoveltyDecay_ >= 1.0f) {
        throw std::invalid_argument("sensoryNoveltyDecay_ must be between 0.0 and 1.0");
    }
    
    // Validate other parameters
}

void AgentBrain::validateNeuromodulationParameters() const {
    if (dopamineLevel_ < AgentBrainConfig::PREDICTION_ERROR_CLAMP_MIN ||
        dopamineLevel_ > AgentBrainConfig::PREDICTION_ERROR_CLAMP_MAX) {
        throw std::invalid_argument("dopamineLevel_ out of valid range");
    }
}

bool AgentBrain::validatePercept(const SensoryPercept& percept) const {
    // Basic validation for sensory percept
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Check for null vectors
    if (vision.empty() || touch.empty() || internal.empty() || proprio.empty()) {
        NLM_LOG_WARNING("AgentBrain: Empty sensory percept data");
        return false;
    }
    
    return true;
}

} // namespace nlm