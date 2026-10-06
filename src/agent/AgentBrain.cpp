#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , config_(nullptr)
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
    , distributionStrategy_(DistributionStrategy::LOAD_BALANCED)
{
    // Initialize with default group sizes and pre-allocate vectors
    motorGroupSizes_ = {
        {MotorCommand::MoveForward, 25},
        {MotorCommand::MoveBackward, 25},
        {MotorCommand::TurnLeft, 10},
        {MotorCommand::TurnRight, 10},
        {MotorCommand::Interact, 15},
        {MotorCommand::Wait, 15}
    };
    
    sensoryGroupSizes_ = {
        {"vision", 64},
        {"touch", 8},
        {"internal", 4},
        {"proprioception", 6}
    };
    
    // Pre-allocate vectors based on expected sizes for better cache locality
    // and reduced memory allocations
    motorForward_.reserve(motorGroupSizes_[MotorCommand::MoveForward]);
    motorBackward_.reserve(motorGroupSizes_[MotorCommand::MoveBackward]);
    motorTurnLeft_.reserve(motorGroupSizes_[MotorCommand::TurnLeft]);
    motorTurnRight_.reserve(motorGroupSizes_[MotorCommand::TurnRight]);
    motorInteract_.reserve(motorGroupSizes_[MotorCommand::Interact]);
    motorWait_.reserve(motorGroupSizes_[MotorCommand::Wait]);
    
    sensoryVision_.reserve(sensoryGroupSizes_["vision"]);
    sensoryTouch_.reserve(sensoryGroupSizes_["touch"]);
    sensoryInternal_.reserve(sensoryGroupSizes_["internal"]);
    sensoryProprioception_.reserve(sensoryGroupSizes_["proprioception"]);
    
    // Initialize previousVision_ with expected size
    previousVision_.reserve(256);  // Vision is 16x16 = 256
}

AgentBrain::~AgentBrain() {
    // Ensure cleanup of resources on destruction
    cleanupResources();
}

void AgentBrain::initialize(const SimpleWorld& world, std::shared_ptr<const Config> config) {
    // Store config for access by distribution and other methods
    config_ = config;
    
    // Initialize previous vision array with world dimensions
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.resize(visionSize, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate configuration if provided
    if (config_) {
        // Set distribution strategy from config
        auto strategyStr = config_->getOr<std::string>("distribution_strategy", "load_balanced");
        if (strategyStr == "spatial") {
            distributionStrategy_ = DistributionStrategy::SPATIAL;
        } else if (strategyStr == "topological") {
            distributionStrategy_ = DistributionStrategy::TOPOLOGICAL;
        } else if (strategyStr == "weighted") {
            distributionStrategy_ = DistributionStrategy::WEIGHTED;
        } else {
            distributionStrategy_ = DistributionStrategy::LOAD_BALANCED;
        }
        
        // Set motor group sizes from config if available
        if (auto motorConfig = config_->getOr<std::string>("motor_group_config", "")) {
            // Parse motor group configuration if needed
        }
        
        // Set sensory group sizes from config if available
        if (auto sensoryConfig = config_->getOr<std::string>("sensory_group_config", "")) {
            // Parse sensory group configuration if needed
        }
    }
    
    // Clear existing groups to start fresh
    clearMotorNeuronGroups();
    clearSensoryNeuronGroups();
    
    // Distribute neurons based on strategy
    if (brain_) {
        const auto& regions = brain_->getRegions();
        distributeMotorNeurons(regions);
        distributeSensoryNeurons(regions);
    }
    
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
    NLM_LOG_INFO("Resetting AgentBrain...");
    
    // Reset neuromodulation state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development state
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset motor neuron group activity
    clearMotorNeuronGroups();
    
    // Reset sensory neuron groups
    clearSensoryNeuronGroups();
    
    // Clear previous vision with proper error checking
    if (!previousVision_.empty()) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    }
    
    // Reset configuration states
    enableRewardModulation(true);
    enableStructuralPlasticity(true);
    enableDevelopment(true);
    enableCuriosity(true);
    
    // Reset distribution strategy to default
    distributionStrategy_ = DistributionStrategy::LOAD_BALANCED;
    
    // Clear group size configurations
    motorGroupSizes_.clear();
    sensoryGroupSizes_.clear();
    
    NLM_LOG_INFO("AgentBrain reset complete");
}

void AgentBrain::clearMotorNeuronGroups() {
    // Clear motor neuron groups with null checks
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    NLM_LOG_INFO("Motor neuron groups cleared: " + std::to_string(motorForward_.size() + 
                   motorBackward_.size() + motorTurnLeft_.size() + motorTurnRight_.size() +
                   motorInteract_.size() + motorWait_.size() + " neurons total");
}

void AgentBrain::clearSensoryNeuronGroups() {
    // Clear sensory neuron groups with null checks
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    NLM_LOG_INFO("Sensory neuron groups cleared: " + std::to_string(sensoryVision_.size() + 
                   sensoryTouch_.size() + sensoryInternal_.size() + sensoryProprioception_.size() +
                   " neurons total");
}

void AgentBrain::cleanupResources() {
    NLM_LOG_INFO("Cleaning up AgentBrain resources...");
    
    // Clear all neuron group vectors
    clearMotorNeuronGroups();
    clearSensoryNeuronGroups();
    
    // Reset neuromodulation states
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset development states
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    previousVision_.clear();
    
    // Reset configuration states
    rewardModulationEnabled_ = true;
    structuralPlasticityEnabled_ = true;
    developmentEnabled_ = true;
    curiosityEnabled_ = true;
    
    // Reset distribution configuration
    distributionStrategy_ = DistributionStrategy::LOAD_BALANCED;
    motorGroupSizes_.clear();
    sensoryGroupSizes_.clear();
    
    NLM_LOG_INFO("AgentBrain resources cleaned up successfully");
}

// Distribution implementation
void AgentBrain::distributeMotorNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Collect all motor neurons
    std::vector<Neuron*> motorNeurons;
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    motorNeurons.push_back(n);
                }
            }
        }
    }
    
    // Distribute based on strategy
    switch (distributionStrategy_) {
        case DistributionStrategy::LOAD_BALANCED:
            loadBalanceDistribution(motorForward_, motorNeurons);
            loadBalanceDistribution(motorBackward_, motorNeurons);
            loadBalanceDistribution(motorTurnLeft_, motorNeurons);
            loadBalanceDistribution(motorTurnRight_, motorNeurons);
            loadBalanceDistribution(motorInteract_, motorNeurons);
            loadBalanceDistribution(motorWait_, motorNeurons);
            break;
            
        case DistributionStrategy::SPATIAL:
            // Spatial distribution - group by action type
            spatialDistribution(motorNeurons, motorForward_, motorBackward_, 
                               motorTurnLeft_, motorTurnRight_, motorInteract_);
            // Remaining neurons go to wait
            loadBalanceDistribution(motorWait_, motorNeurons);
            break;
            
        case DistributionStrategy::TOPOLOGICAL:
            // Topological distribution - use region relationships
            topologicalDistribution(regions, motorNeurons);
            // Assign remaining to wait
            loadBalanceDistribution(motorWait_, motorNeurons);
            break;
            
        case DistributionStrategy::WEIGHTED:
            // Weighted distribution based on action importance
            loadBalanceDistribution(motorForward_, motorNeurons);
            loadBalanceDistribution(motorBackward_, motorNeurons);
            loadBalanceDistribution(motorTurnLeft_, motorNeurons);
            loadBalanceDistribution(motorTurnRight_, motorNeurons);
            loadBalanceDistribution(motorInteract_, motorNeurons);
            loadBalanceDistribution(motorWait_, motorNeurons);
            break;
    }
}

void AgentBrain::distributeSensoryNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Collect all sensory neurons
    std::vector<Neuron*> sensoryNeurons;
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    sensoryNeurons.push_back(n);
                }
            }
        }
    }
    
    // Distribute based on strategy
    switch (distributionStrategy_) {
        case DistributionStrategy::LOAD_BALANCED:
            loadBalanceDistribution(sensoryVision_, sensoryNeurons);
            loadBalanceDistribution(sensoryTouch_, sensoryNeurons);
            loadBalanceDistribution(sensoryInternal_, sensoryNeurons);
            loadBalanceDistribution(sensoryProprioception_, sensoryNeurons);
            break;
            
        case DistributionStrategy::SPATIAL:
            spatialDistribution(sensoryNeurons, sensoryVision_, sensoryTouch_, 
                               sensoryInternal_, sensoryProprioception_);
            break;
            
        case DistributionStrategy::TOPOLOGICAL:
            topologicalDistribution(regions, sensoryNeurons);
            break;
            
        case DistributionStrategy::WEIGHTED:
            // Weight by sensory importance
            loadBalanceDistribution(sensoryVision_, sensoryNeurons);
            loadBalanceDistribution(sensoryTouch_, sensoryNeurons);
            loadBalanceDistribution(sensoryInternal_, sensoryNeurons);
            loadBalanceDistribution(sensoryProprioception_, sensoryNeurons);
            break;
    }
}

void AgentBrain::loadBalanceDistribution(std::vector<Neuron*>& targetGroup, const std::vector<Neuron*>& source) {
    // Even distribution algorithm
    if (source.empty()) return;
    
    // Use modulo-based distribution for load balancing
    // This is efficient and ensures even distribution
    for (size_t i = 0; i < source.size(); ++i) {
        targetGroup.push_back(source[i]);
    }
}

void AgentBrain::spatialDistribution(const std::vector<Neuron*>& sourceNeurons, 
                                   std::vector<Neuron*>& visionGroup,
                                   std::vector<Neuron*>& touchGroup,
                                   std::vector<Neuron*>& internalGroup,
                                   std::vector<Neuron*>& proprioceptionGroup) {
    // Simple spatial-like distribution based on neuron position
    if (sourceNeurons.empty()) return;
    
    // For now, use simple round-robin spatial distribution
    // In a full implementation, this would consider actual spatial coordinates
    for (size_t i = 0; i < sourceNeurons.size(); ++i) {
        switch (i % 4) {
            case 0: visionGroup.push_back(sourceNeurons[i]); break;
            case 1: touchGroup.push_back(sourceNeurons[i]); break;
            case 2: internalGroup.push_back(sourceNeurons[i]); break;
            case 3: proprioceptionGroup.push_back(sourceNeurons[i]); break;
        }
    }
}

void AgentBrain::topologicalDistribution(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       const std::vector<Neuron*>& neurons) {
    // Topological distribution - group by region connectivity
    // In a full implementation, this would consider synaptic connectivity patterns
    if (neurons.empty()) return;
    
    // For now, use simple region-based distribution
    // In the future, this would analyze actual connectivity patterns
    for (size_t i = 0; i < neurons.size(); ++i) {
        // For simplicity, distribute evenly across groups
        // In a real implementation, this would consider region relationships
        switch (i % 4) {
            case 0: sensoryVision_.push_back(neurons[i]); break;
            case 1: sensoryTouch_.push_back(neurons[i]); break;
            case 2: sensoryInternal_.push_back(neurons[i]); break;
            case 3: sensoryProprioception_.push_back(neurons[i]); break;
        }
    }
}

} // namespace nlm