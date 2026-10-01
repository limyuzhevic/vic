#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

namespace {
    constexpr float DEFAULT_SENSORY_NOVELTY_DECAY = 0.95f;
    constexpr float MAX_NOVELTY_LEVEL = 1.0f;
    constexpr float MIN_NOVELTY_LEVEL = 0.0f;
    constexpr float DEFAULT_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    constexpr size_t MAX_MOTOR_GROUPS = 6;
    constexpr size_t MAX_SENSORY_GROUPS = 4;
    
    bool isValidNeuron(Neuron* neuron) {
        return neuron != nullptr;
    }
    
    NeuronType getNeuronTypeSafely(NeuralPopulation* pop) {
        if (!pop) return NeuronType::Unknown;
        return pop->getNeuronType();
    }
}

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
    , sensoryNoveltyDecay_(DEFAULT_SENSORY_NOVELTY_DECAY)
{
    // Validate input
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize neuron groups with error checking
    initializeNeuronGroups();
}

void AgentBrain::initializeNeuronGroups() {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize neuron groups: brain is null");
        return;
    }
    
    // Ensure all neuron group vectors are empty before initialization
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
    
    // Iterate through brain regions and populations safely
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (const auto& pop : region->getPopulations()) {
            if (!pop) continue;
            
            NeuronType type = getNeuronTypeSafely(pop.get());
            
            if (type == NeuronType::Motor) {
                distributeMotorNeurons(pop.get());
            } else if (type == NeuronType::Sensory) {
                distributeSensoryNeurons(pop.get());
            }
        }
    }
    
    // Log initialization summary
    NLM_LOG_INFO("AgentBrain neuron group initialization complete:");
    NLM_LOG_INFO("  Motor groups: " + std::to_string(motorForward_.size() + motorBackward_.size() + 
                  motorTurnLeft_.size() + motorTurnRight_.size() + 
                  motorInteract_.size() + motorWait_.size()) + " neurons");
    NLM_LOG_INFO("  Sensory groups: " + std::to_string(sensoryVision_.size() + 
                  sensoryTouch_.size() + sensoryInternal_.size() + 
                  sensoryProprioception_.size()) + " neurons");
}

void AgentBrain::distributeMotorNeurons(NeuralPopulation* population) {
    if (!population) return;
    
    for (Neuron* neuron : population->getNeurons()) {
        if (!isValidNeuron(neuron)) continue;
        
        // Check if already distributed
        if (isMotorNeuronDistributed(neuron)) continue;
        
        // Assign to motor group based on distribution algorithm
        assignToMotorGroup(neuron);
    }
}

void AgentBrain::distributeSensoryNeurons(NeuralPopulation* population) {
    if (!population) return;
    
    for (Neuron* neuron : population->getNeurons()) {
        if (!isValidNeuron(neuron)) continue;
        
        // Check if already distributed
        if (isSensoryNeuronDistributed(neuron)) continue;
        
        // Assign to sensory group based on distribution algorithm
        assignToSensoryGroup(neuron);
    }
}

bool AgentBrain::isMotorNeuronDistributed(Neuron* neuron) const {
    if (!neuron) return true;
    
    // Check all motor groups
    return std::any_of(motorForward_.begin(), motorForward_.end(), 
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(motorBackward_.begin(), motorBackward_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(motorTurnLeft_.begin(), motorTurnLeft_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(motorTurnRight_.begin(), motorTurnRight_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(motorInteract_.begin(), motorInteract_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(motorWait_.begin(), motorWait_.end(),
                      [neuron](const Neuron* n) { return n == neuron; });
}

bool AgentBrain::isSensoryNeuronDistributed(Neuron* neuron) const {
    if (!neuron) return true;
    
    // Check all sensory groups
    return std::any_of(sensoryVision_.begin(), sensoryVision_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(sensoryTouch_.begin(), sensoryTouch_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(sensoryInternal_.begin(), sensoryInternal_.end(),
                      [neuron](const Neuron* n) { return n == neuron; }) ||
           std::any_of(sensoryProprioception_.begin(), sensoryProprioception_.end(),
                      [neuron](const Neuron* n) { return n == neuron; });
}

void AgentBrain::assignToMotorGroup(Neuron* neuron) {
    if (!neuron) return;
    
    // Calculate current distribution index
    size_t idx = motorForward_.size() + motorBackward_.size() + 
                 motorTurnLeft_.size() + motorTurnRight_.size() +
                 motorInteract_.size() + motorWait_.size();
    
    // Assign to appropriate group
    switch (idx % MAX_MOTOR_GROUPS) {
        case 0: motorForward_.push_back(neuron); break;
        case 1: motorBackward_.push_back(neuron); break;
        case 2: motorTurnLeft_.push_back(neuron); break;
        case 3: motorTurnRight_.push_back(neuron); break;
        case 4: motorInteract_.push_back(neuron); break;
        case 5: motorWait_.push_back(neuron); break;
        default: NLM_LOG_WARNING("Unexpected motor group assignment index: " + std::to_string(idx));
    }
}

void AgentBrain::assignToSensoryGroup(Neuron* neuron) {
    if (!neuron) return;
    
    // Calculate current distribution index
    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                 sensoryInternal_.size() + sensoryProprioception_.size();
    
    // Assign to appropriate group
    switch (idx % MAX_SENSORY_GROUPS) {
        case 0: sensoryVision_.push_back(neuron); break;
        case 1: sensoryTouch_.push_back(neuron); break;
        case 2: sensoryInternal_.push_back(neuron); break;
        case 3: sensoryProprioception_.push_back(neuron); break;
        default: NLM_LOG_WARNING("Unexpected sensory group assignment index: " + std::to_string(idx));
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
    
    // Input validation
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("Invalid reward values: reward=" + std::to_string(reward) + 
                        ", predictedReward=" + std::to_string(predictedReward));
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
    
    // Apply reward modulation to all synapses
    applyRewardToSynapses();
    
    // Modulate plasticity based on dopamine level
    modulatePlasticityBasedOnDopamine();
}

void AgentBrain::applyRewardToSynapses() {
    if (!brain_) return;
    
    size_t modifiedSynapses = 0;
    
    for (const auto& region : brain_->getRegions()) {
        if (!region) continue;
        
        for (auto* syn : region->getSynapses()) {
            if (!syn) continue;
            
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
                
                modifiedSynapses++;
            }
        }
    }
    
    if (modifiedSynapses > 0) {
        NLM_LOG_DEBUG("Applied reward modulation to " + std::to_string(modifiedSynapses) + " synapses");
    }
}

void AgentBrain::modulatePlasticityBasedOnDopamine() {
    if (!brain_) return;
    
    // Calculate plasticity factor based on dopamine level
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP system if available
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
    
    // Update structural plasticity rates if enabled
    updateStructuralPlasticityRates();
}

void AgentBrain::updateStructuralPlasticityRates() {
    if (!brain_ || !structuralPlasticityEnabled_) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    // Higher synaptogenesis in early development (based on plasticity modifier)
    float synRate = 0.0001f * plasticityModifier_;
    // Higher pruning in mature brain (plasticity modifier < 1.0)
    float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
    
    sp->setSynaptogenesisRate(synRate);
    sp->setPruningRate(pruneRate);
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

// Callback for when brain changes - trigger updates
void AgentBrain::onBrainChange() {
    if (!brain_) {
        NLM_LOG_ERROR("onBrainChange called with null brain");
        return;
    }
    
    // Re-initialize neuron groups when brain becomes available
    initializeNeuronGroups();
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    // Input validation
    if (timestep < 0.0) {
        NLM_LOG_WARNING("Negative timestep provided to updateDevelopment: " + std::to_string(timestep));
        timestep = 0.0;
    }
    
    developmentalAge_ += timestep;
    
    // Update developmental stage based on age
    updateDevelopmentalStage();
    
    // Update structural plasticity based on development
    if (structuralPlasticityEnabled_) {
        updateStructuralPlasticityRates();
    }
}

void AgentBrain::updateDevelopmentalStage() {
    if (!brain_) return;
    
    // Biologically inspired developmental stages
    // Approximate time-based progression
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity in initial phase
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

void AgentBrain::reset() {
    // Reset all internal state to initial values
    resetNeuromodulationState();
    resetDevelopmentState();
    resetSensoryState();
    
    // Re-initialize with current brain if available
    if (brain_) {
        initializeNeuronGroups();
    }
}

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

void AgentBrain::resetDevelopmentState() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

void AgentBrain::resetSensoryState() {
    // Clear previous vision data
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
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
    // Reset all internal state to initial values
    resetNeuromodulationState();
    resetDevelopmentState();
    resetSensoryState();
    
    // Re-initialize with current brain if available
    if (brain_) {
        initializeNeuronGroups();
    }
}

void AgentBrain::resetNeuromodulationState() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

void AgentBrain::resetDevelopmentState() {
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
}

void AgentBrain::resetSensoryState() {
    // Clear previous vision data
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Debug utility method for inspecting agent state
void AgentBrain::logDebugInfo() const {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot log debug info: brain is null");
        return;
    }
    
    NLM_LOG_DEBUG("=== AgentBrain Debug Info ===");
    NLM_LOG_DEBUG("Neuromodulation Levels:");
    NLM_LOG_DEBUG("  Dopamine: " + std::to_string(dopamineLevel_));
    NLM_LOG_DEBUG("  Curiosity: " + std::to_string(curiosityLevel_));
    NLM_LOG_DEBUG("  Novelty: " + std::to_string(noveltyLevel_));
    NLM_LOG_DEBUG("  Prediction Error: " + std::to_string(predictionError_));
    
    NLM_LOG_DEBUG("Development State:");
    NLM_LOG_DEBUG("  Age: " + std::to_string(developmentalAge_));
    NLM_LOG_DEBUG("  Plasticity Modifier: " + std::to_string(plasticityModifier_));
    
    NLM_LOG_DEBUG("Neuron Distribution:");
    NLM_LOG_DEBUG("  Motor neurons: " + std::to_string(motorForward_.size() + motorBackward_.size() +
                  motorTurnLeft_.size() + motorTurnRight_.size() +
                  motorInteract_.size() + motorWait_.size()));
    NLM_LOG_DEBUG("  Sensory neurons: " + std::to_string(sensoryVision_.size() + 
                  sensoryTouch_.size() + sensoryInternal_.size() +
                  sensoryProprioception_.size()));
}

// Configuration validation utility
bool AgentBrain::validateConfiguration() const {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot validate configuration: brain is null");
        return false;
    }
    
    // Check for critical components
    if (brain_->getWorkingMemory() == nullptr) {
        NLM_LOG_WARNING("Working memory not available");
    }
    
    if (brain_->getEpisodicMemory() == nullptr) {
        NLM_LOG_WARNING("Episodic memory not available");
    }
    
    if (brain_->getDopamine() == nullptr) {
        NLM_LOG_WARNING("Dopamine system not available");
    }
    
    if (brain_->getCuriosity() == nullptr) {
        NLM_LOG_WARNING("Curiosity system not available");
    }
    
    if (brain_->getPredictionSystem() == nullptr) {
        NLM_LOG_WARNING("Prediction system not available");
    }
    
    if (brain_->getDevelopmentSystem() == nullptr) {
        NLM_LOG_WARNING("Development system not available");
    }
    
    // Validate neuromodulator ranges
    if (dopamineLevel_ < -1.0f || dopamineLevel_ > 1.0f) {
        NLM_LOG_WARNING("Dopamine level out of expected range: " + std::to_string(dopamineLevel_));
    }
    
    if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
        NLM_LOG_WARNING("Curiosity level out of expected range: " + std::to_string(curiosityLevel_));
    }
    
    if (noveltyLevel_ < 0.0f || noveltyLevel_ > 1.0f) {
        NLM_LOG_WARNING("Novelty level out of expected range: " + std::to_string(noveltyLevel_));
    }
    
    return true;
}

} // namespace nlm
