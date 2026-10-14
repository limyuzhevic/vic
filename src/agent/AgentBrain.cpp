#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <unordered_map>

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
    , useCaching_(true)
    , curiosityThreshold_(0.3f)
    , explorationRate_(0.2f)
    , sensoryNoveltyDecay_(0.99f)
    , lastNoveltyComputed_(0.0f)
    , noveltyCacheValid_(false)
    , motorGroupsInitialized_(false)
    , sensoryGroupsInitialized_(false)
{
    NLM_LOG_INFO("AgentBrain constructor: Initializing neural interfaces");
    validateBrainPtr();
}

AgentBrain::~AgentBrain() {
    NLM_LOG_INFO("AgentBrain destructor: Cleaning up neural interfaces");
}

void AgentBrain::validateBrainPtr() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Brain pointer is null!");
        throw std::runtime_error("AgentBrain initialized with null brain pointer");
    }
}

void AgentBrain::validateNeuronGroups() const {
    if (!motorGroupsInitialized_) {
        NLM_LOG_WARNING("AgentBrain: Motor neuron groups not initialized properly");
    }
    if (!sensoryGroupsInitialized_) {
        NLM_LOG_WARNING("AgentBrain: Sensory neuron groups not initialized properly");
    }
}

void AgentBrain::logConfiguration() const {
    NLM_LOG_INFO("AgentBrain Configuration:");
    NLM_LOG_INFO("  Reward modulation: " + std::string(rewardModulationEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Structural plasticity: " + std::string(structuralPlasticityEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Development: " + std::string(developmentEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Curiosity: " + std::string(curiosityEnabled_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Caching: " + std::string(useCaching_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("  Curiosity threshold: " + std::to_string(curiosityThreshold_));
    NLM_LOG_INFO("  Exploration rate: " + std::to_string(explorationRate_));
    NLM_LOG_INFO("  Novelty decay: " + std::to_string(sensoryNoveltyDecay_));
}

size_t AgentBrain::getMotorNeuronsCount() const {
    if (!motorGroupsInitialized_) {
        const_cast<AgentBrain*>(this)->initializeMotorGroups();
    }
    size_t total = 0;
    for (size_t count : motorGroupSizes_) total += count;
    return total;
}

size_t AgentBrain::getSensoryNeuronsCount() const {
    if (!sensoryGroupsInitialized_) {
        const_cast<AgentBrain*>(this)->initializeSensoryGroups();
    }
    size_t total = 0;
    for (size_t count : sensoryGroupSizes_) total += count;
    return total;
}

void AgentBrain::initializeMotorGroups() {
    if (!brain_) return;
    
    motorGroupSizes_ = {0, 0, 0, 0, 0, 0}; // forward, backward, turnLeft, turnRight, interact, wait
    motorGroupsInitialized_ = false;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = motorForward_.size() + motorBackward_.size() + 
                                motorTurnLeft_.size() + motorTurnRight_.size() +
                                motorInteract_.size() + motorWait_.size();
                    switch (idx % 6) {
                        case 0: motorForward_.push_back(n); motorGroupSizes_[0]++; break;
                        case 1: motorBackward_.push_back(n); motorGroupSizes_[1]++; break;
                        case 2: motorTurnLeft_.push_back(n); motorGroupSizes_[2]++; break;
                        case 3: motorTurnRight_.push_back(n); motorGroupSizes_[3]++; break;
                        case 4: motorInteract_.push_back(n); motorGroupSizes_[4]++; break;
                        case 5: motorWait_.push_back(n); motorGroupSizes_[5]++; break;
                    }
                }
            }
        }
    }
    motorGroupsInitialized_ = true;
}

void AgentBrain::initializeSensoryGroups() {
    if (!brain_) return;
    
    sensoryGroupSizes_ = {0, 0, 0, 0}; // vision, touch, internal, proprioception
    sensoryGroupsInitialized_ = false;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
                    switch (idx % 4) {
                        case 0: sensoryVision_.push_back(n); sensoryGroupSizes_[0]++; break;
                        case 1: sensoryTouch_.push_back(n); sensoryGroupSizes_[1]++; break;
                        case 2: sensoryInternal_.push_back(n); sensoryGroupSizes_[2]++; break;
                        case 3: sensoryProprioception_.push_back(n); sensoryGroupSizes_[3]++; break;
                    }
                }
            }
        }
    }
    sensoryGroupsInitialized_ = true;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) return;
    
    initializeMotorGroups();
    initializeSensoryGroups();
    
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset cache
    noveltyCacheValid_ = false;
    cachedVisionDifferences_.clear();
    
    logConfiguration();
    
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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput: Brain pointer is null");
        return;
    }
    
    // Validate sensory data
    if (percept.getVision().size() != previousVision_.size() && !previousVision_.empty()) {
        NLM_LOG_WARNING("AgentBrain: Vision size changed, resetting novelty cache");
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (useCaching_ && !vision.empty()) {
        // Optimized processing with caching
        if (!noveltyCacheValid_ || cachedVisionDifferences_.empty()) {
            computeVisionDifferences(vision);
        }
        lastNoveltyComputed_ = cachedVisionDifferences_.empty() ? 0.0f : cachedVisionDifferences_.front();
    } else {
        computeVisionDifferences(vision);
        lastNoveltyComputed_ = noveltyLevel_;
    }
    
    // Inject vision input into neurons
    injectVisionInput(vision);
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    injectTouchInput(touch);
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    injectInternalInput(intern);
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    injectProprioceptionInput(proprio);
    
    // Update curiosity based on novelty
    if (curiosityEnabled_ && !vision.empty()) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Store previous vision for next frame
    previousVision_ = vision;
    noveltyCacheValid_ = false;
    cachedVisionDifferences_.clear();
}

void AgentBrain::computeVisionDifferences(const std::vector<float>& vision) {
    if (vision.empty() || previousVision_.empty()) {
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
    
    // Store for caching if enabled
    if (useCaching_) {
        cachedVisionDifferences_.push_back(noveltyLevel_);
        if (cachedVisionDifferences_.size() > 100) {
            cachedVisionDifferences_.erase(cachedVisionDifferences_.begin());
        }
    }
}

void AgentBrain::injectVisionInput(const std::vector<float>& vision) {
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectTouchInput(const std::vector<float>& touch) {
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectInternalInput(const std::vector<float>& intern) {
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
}

void AgentBrain::injectProprioceptionInput(const std::vector<float>& proprio) {
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::decodeMotorCommand: Brain pointer is null");
        return MotorCommand::Wait;
    }
    
    validateNeuronGroups();
    
    MotorCommand decoded = decodeOptimized();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeOptimized() {
    // Optimized motor decoding using pre-computed group indices
    if (!motorGroupsInitialized_) {
        initializeMotorGroups();
    }
    
    float forwardAct = computeMotorActivation(motorForward_);
    float backwardAct = computeMotorActivation(motorBackward_);
    float leftAct = computeMotorActivation(motorTurnLeft_);
    float rightAct = computeMotorActivation(motorTurnRight_);
    float interactAct = computeMotorActivation(motorInteract_);
    float waitAct = computeMotorActivation(motorWait_);
    
    // Find maximum activity using optimized comparison
    struct CommandInfo {
        MotorCommand cmd;
        float activity;
    };
    
    CommandInfo commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    // Find maximum activity
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

float AgentBrain::computeMotorActivation(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationRate_;  // Up to explorationRate_ random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command with better distribution
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
        if (!brain_) NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Brain pointer is null");
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
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) NLM_LOG_ERROR("AgentBrain::updateDevelopment: Brain pointer is null");
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
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::getDevelopmentalStage: Brain pointer is null");
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
    
    // Reset cache
    noveltyCacheValid_ = false;
    cachedVisionDifferences_.clear();
    
    NLM_LOG_INFO("AgentBrain reset completed");
}

} // namespace nlm
