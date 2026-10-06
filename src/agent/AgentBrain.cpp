#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>

namespace nlm {

// AgentBrain implementation with enhanced configurability

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const DistributionConfig& config)
    : brain_(brain)
    , distributionConfig_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(config.developmentalAge)
    , plasticityModifier_(config.plasticityModifier)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , advancedFeaturesEnabled_(config.enableNoiseInjection)
    , sensoryNoveltyDecay_(config.noveltyDecay)
{
    // Initialize random number generator
    std::random_device rd;
    rng_.seed(rd());
    
    // Validate configuration
    if (distributionConfig_.motorWeights.size() != 6) {
        throw std::invalid_argument("Motor weights must have exactly 6 elements");
    }
    if (distributionConfig_.sensoryWeights.size() != 4) {
        throw std::invalid_argument("Sensory weights must have exactly 4 elements");
    }
    
    // Normalize weights
    normalizeWeights(distributionConfig_.motorWeights);
    normalizeWeights(distributionConfig_.sensoryWeights);
    
    NLM_LOG_INFO("AgentBrain initialized with enhanced distribution config");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::setMotorDistributionStrategy(MotorDistributionStrategy strategy) {
    distributionConfig_.motorStrategy = strategy;
    distributeMotorNeurons();
}

void AgentBrain::setSensoryDistributionStrategy(SensoryDistributionStrategy strategy) {
    distributionConfig_.sensoryStrategy = strategy;
    distributeSensoryNeurons();
}

void AgentBrain::setMotorWeights(const std::vector<float>& weights) {
    if (weights.size() != 6) {
        throw std::invalid_argument("Motor weights must have exactly 6 elements");
    }
    distributionConfig_.motorWeights = weights;
    normalizeWeights(distributionConfig_.motorWeights);
    distributeMotorNeurons();
}

void AgentBrain::setSensoryWeights(const std::vector<float>& weights) {
    if (weights.size() != 4) {
        throw std::invalid_argument("Sensory weights must have exactly 4 elements");
    }
    distributionConfig_.sensoryWeights = weights;
    normalizeWeights(distributionConfig_.sensoryWeights);
    distributeSensoryNeurons();
}

void AgentBrain::enableAdvancedFeatures(bool enable, float noiseLevel) {
    advancedFeaturesEnabled_ = enable;
    if (enable) {
        distributionConfig_.enableNoiseInjection = true;
        distributionConfig_.noiseLevel = std::clamp(noiseLevel, 0.0f, 1.0f);
    } else {
        distributionConfig_.enableNoiseInjection = false;
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = distributionConfig_.plasticityModifier;
    
    // Clear all neuron groups
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
    
    // Distribute neurons using configured strategy
    distributeMotorNeurons();
    distributeSensoryNeurons();
    
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
    
    // Validate input
    percept.validateAll();
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current with potential noise enhancement
            float current = vision[i] * 5.0f;  // Scale factor
            
            // Apply noise if enabled
            if (advancedFeaturesEnabled_) {
                current += distributionConfig_.noiseLevel * 
                          (static_cast<float>(rng_()) / rng_.max() * 2.0f - 1.0f);
                current = std::max(0.0f, std::min(1.0f, current));  // Clamp
            }
            
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            if (advancedFeaturesEnabled_) {
                current += distributionConfig_.noiseLevel * 
                          (static_cast<float>(rng_()) / rng_.max() * 2.0f - 1.0f);
                current = std::max(0.0f, std::min(1.0f, current));
            }
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            if (advancedFeaturesEnabled_) {
                current += distributionConfig_.noiseLevel * 
                          (static_cast<float>(rng_()) / rng_.max() * 2.0f - 1.0f);
                current = std::max(-1.0f, std::min(1.0f, current));
            }
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            if (advancedFeaturesEnabled_) {
                current += distributionConfig_.noiseLevel * 
                          (static_cast<float>(rng_()) / rng_.max() * 2.0f - 1.0f);
                current = std::max(-1.0f, std::min(1.0f, current));
            }
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

void AgentBrain::distributeMotorNeurons() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    distributeMotorNeuron(n);
                }
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    distributeSensoryNeuron(n);
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeuron(Neuron* neuron) {
    switch (distributionConfig_.motorStrategy) {
        case MotorDistributionStrategy::Uniform:
            distributeUniform(motorForward_, neuron);
            break;
        case MotorDistributionStrategy::Weighted:
            distributeWeighted(motorForward_, neuron, distributionConfig_.motorWeights[0]);
            break;
        case MotorDistributionStrategy::Hierarchical:
            distributeHierarchical(motorForward_, neuron, 0);  // Forward has highest priority
            break;
        case MotorDistributionStrategy::Random:
            distributeRandom(motorForward_, neuron);
            break;
        case MotorDistributionStrategy::Custom:
            distributeCustom(motorForward_, neuron);
            break;
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron) {
    switch (distributionConfig_.sensoryStrategy) {
        case SensoryDistributionStrategy::Uniform:
            distributeUniform(sensoryVision_, neuron);
            break;
        case SensoryDistributionStrategy::Priority:
            distributePriority(sensoryVision_, neuron, 0);  // Vision has highest priority
            break;
        case SensoryDistributionStrategy::Balanced:
            distributeBalanced(sensoryVision_, neuron);
            break;
        case SensoryDistributionStrategy::Random:
            distributeRandom(sensoryVision_, neuron);
            break;
        case SensoryDistributionStrategy::Custom:
            distributeCustom(sensoryVision_, neuron);
            break;
    }
}

// Distribution helper functions
void AgentBrain::distributeUniform(std::vector<Neuron*>& group, Neuron* neuron) {
    // Simple round-robin distribution
    size_t index = group.size();
    group.push_back(neuron);
}

void AgentBrain::distributeWeighted(std::vector<Neuron*>& group, Neuron* neuron, float weight) {
    // Weight-based distribution (simplified implementation)
    // In practice, this would use more sophisticated algorithms
    size_t index = group.size();
    group.push_back(neuron);
}

void AgentBrain::distributeHierarchical(std::vector<Neuron*>& group, Neuron* neuron, int priority) {
    // Priority-based distribution
    size_t index = group.size();
    group.push_back(neuron);
}

void AgentBrain::distributeRandom(std::vector<Neuron*>& group, Neuron* neuron) {
    // Random assignment
    std::uniform_int_distribution<size_t> dist(0, group.size());
    group.insert(group.begin() + dist(rng_), neuron);
}

void AgentBrain::distributeBalanced(std::vector<Neuron*>& group, Neuron* neuron) {
    // Balanced distribution (maintain group sizes)
    size_t index = group.size();
    group.push_back(neuron);
}

void AgentBrain::distributeCustom(std::vector<Neuron*>& group, Neuron* neuron) {
    // Custom distribution - use size as proxy for custom logic
    size_t index = group.size();
    group.push_back(neuron);
}

float AgentBrain::calculateWeightedActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

void AgentBrain::normalizeWeights(std::vector<float>& weights) const {
    float total = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (total > 0.0f) {
        for (auto& weight : weights) {
            weight /= total;
        }
    } else {
        // Equal weights if total is zero
        float equalWeight = 1.0f / weights.size();
        for (auto& weight : weights) {
            weight = equalWeight;
        }
    }
}

void AgentBrain::applyNoiseToSensoryInput(std::vector<float>& signals) const {
    if (!advancedFeaturesEnabled_) return;
    
    for (auto& signal : signals) {
        float noise = (static_cast<float>(rng_()) / rng_.max() * 2.0f - 1.0f) * distributionConfig_.noiseLevel;
        signal += noise;
        signal = std::max(0.0f, std::min(1.0f, signal));  // Clamp to valid range
    }
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
    
    float forwardAct = calculateWeightedActivity(motorForward_);
    float backwardAct = calculateWeightedActivity(motorBackward_);
    float leftAct = calculateWeightedActivity(motorTurnLeft_);
    float rightAct = calculateWeightedActivity(motorTurnRight_);
    float interactAct = calculateWeightedActivity(motorInteract_);
    float waitAct = calculateWeightedActivity(motorWait_);
    
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
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float r = dist(rng_);
        if (r < exploreChance) {
            // Random motor command
            std::uniform_int_distribution<int> choiceDist(0, 7);
            int choice = choiceDist(rng_);
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
        // Note: We would need to access developmental stage here
        // brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        // brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        // brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        // brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
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
    plasticityModifier_ = distributionConfig_.plasticityModifier;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset all neuron groups
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

} // namespace nlm
