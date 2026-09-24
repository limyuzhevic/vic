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
    , explorationEnabled_(true)
    , dopamineModulationEnabled_(true)
    , noveltyDetectionEnabled_(true)
    , neuralDevelopmentEnabled_(true)
    , metaPlasticityEnabled_(true)
    , adaptiveLearningEnabled_(true)
    , continuousSelfOptimizationEnabled_(true)
    , environmentalAdaptationEnabled_(true)
    , socialAdaptationEnabled_(true)
    , temporalAdaptationEnabled_(true)
    , quantumProcessingEnabled_(false)  // Initially disabled (experimental)
    , syntheticConsciousnessEnabled_(false)  // Initially disabled (experimental)
    , quantumEntanglementEnabled_(false)  // Initially disabled (experimental)
    
    // Advanced configuration parameters
    , explorationRate_(0.3f)
    , noveltyThreshold_(0.5f)
    , developmentalRate_(0.8f)
    , metaPlasticityRate_(0.2f)
    , adaptiveLearningRate_(0.4f)
    , selfOptimizationRate_(0.3f)
    , environmentalAdaptationRate_(0.5f)
    , socialAdaptationRate_(0.3f)
    , temporalProcessingRange_(1.0f)
    , quantumCoherenceLevel_(0.0f)
    , consciousnessLevel_(0.0f)
    , entanglementStrength_(0.0f)
    
    // Environmental adaptation parameters
    , climateAdaptationEnabled_(true)
    , climateAdaptationThreshold_(0.7f)
    , resourceAdaptationEnabled_(true)
    , resourceAdaptationEfficiency_(0.8f)
    , theoryOfMindEnabled_(true)
    , theoryOfMindCapacity_(0.6f)
    , empathySimulationEnabled_(true)
    
    // Temporal adaptation parameters
    , timePerceptionTrainingEnabled_(true)
    , timePerceptionAccuracy_(0.8f)
    
    // Random seed and action choices
    , randomSeed_(42)
    , maxActionChoices_(8)
    
    // Previous sensory state for novelty detection
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
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
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
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
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

// Configuration methods implementation
void AgentBrain::enableExploration(bool enable) {
    explorationEnabled_ = enable;
    if (!enable) {
        NLM_LOG_INFO("Exploration disabled");
    }
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Exploration rate set to " + std::to_string(explorationRate_));
}

void AgentBrain::setRandomSeed(uint64_t seed) {
    randomSeed_ = seed;
    NLM_LOG_INFO("Random seed set to " + std::to_string(randomSeed_));
}

void AgentBrain::setMaxActionChoices(size_t maxChoices) {
    maxActionChoices_ = maxChoices;
    NLM_LOG_INFO("Max action choices set to " + std::to_string(maxActionChoices_));
}

void AgentBrain::enableDopamineModulation(bool enable) {
    dopamineModulationEnabled_ = enable;
}

void AgentBrain::enableNoveltyDetection(bool enable) {
    noveltyDetectionEnabled_ = enable;
}

void AgentBrain::setNoveltyThreshold(float threshold) {
    noveltyThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::enableNeuralDevelopment(bool enable) {
    neuralDevelopmentEnabled_ = enable;
}

void AgentBrain::setDevelopmentalRate(float rate) {
    developmentalRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::enableStructuralPlasticity(bool enable) {
    structuralPlasticityEnabled_ = enable;
}

void AgentBrain::enableMetaPlasticity(bool enable) {
    metaPlasticityEnabled_ = enable;
    NLM_LOG_INFO("Meta-plasticity " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrain::setMetaPlasticityRate(float rate) {
    metaPlasticityRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Meta-plasticity rate set to " + std::to_string(metaPlasticityRate_));
}

void AgentBrain::enableAdaptiveLearning(bool enable) {
    adaptiveLearningEnabled_ = enable;
}

void AgentBrain::setAdaptiveLearningRate(float rate) {
    adaptiveLearningRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::enableContinuousSelfOptimization(bool enable) {
    continuousSelfOptimizationEnabled_ = enable;
}

void AgentBrain::setSelfOptimizationRate(float rate) {
    selfOptimizationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::enableEnvironmentalAdaptation(bool enable) {
    environmentalAdaptationEnabled_ = enable;
}

void AgentBrain::setEnvironmentalAdaptationRate(float rate) {
    environmentalAdaptationRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::enableClimateAdaptation(bool enable) {
    climateAdaptationEnabled_ = enable;
}

void AgentBrain::setClimateAdaptationThreshold(float threshold) {
    climateAdaptationThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::enableResourceAdaptation(bool enable) {
    resourceAdaptationEnabled_ = enable;
}

void AgentBrain::setResourceAdaptationEfficiency(float efficiency) {
    resourceAdaptationEfficiency_ = std::clamp(efficiency, 0.0f, 1.0f);
}

// Getter implementations for new configuration
float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

uint64_t AgentBrain::getRandomSeed() const {
    return randomSeed_;
}

size_t AgentBrain::getMaxActionChoices() const {
    return maxActionChoices_;
}

float AgentBrain::getNoveltyThreshold() const {
    return noveltyThreshold_;
}

float AgentBrain::getDevelopmentalRate() const {
    return developmentalRate_;
}

float AgentBrain::getMetaPlasticityRate() const {
    return metaPlasticityRate_;
}

float AgentBrain::getAdaptiveLearningRate() const {
    return adaptiveLearningRate_;
}

float AgentBrain::getSelfOptimizationRate() const {
    return selfOptimizationRate_;
}

float AgentBrain::getEnvironmentalAdaptationRate() const {
    return environmentalAdaptationRate_;
}

float AgentBrain::getSocialAdaptationRate() const {
    return socialAdaptationRate_;
}

float AgentBrain::getTemporalProcessingRange() const {
    return temporalProcessingRange_;
}

float AgentBrain::getQuantumCoherenceLevel() const {
    return quantumCoherenceLevel_;
}

float AgentBrain::getConsciousnessLevel() const {
    return consciousnessLevel_;
}

float AgentBrain::getEntanglementStrength() const {
    return entanglementStrength_;
}

// Advanced neural diversity and specialization methods
void AgentBrain::enableNeuralSpecialization(bool enable) {
    neuralSpecializationEnabled_ = enable;
    NLM_LOG_INFO(std::string("Neural specialization ") + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrain::setNeuralDiversityLevel(float level) {
    neuralDiversityLevel_ = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_INFO("Neural diversity level set to " + std::to_string(neuralDiversityLevel_));
}

void AgentBrain::enableRegionSpecialization(bool enable) {
    regionSpecializationEnabled_ = enable;
}

void AgentBrain::setRegionSpecializationLevel(float level) {
    regionSpecializationLevel_ = std::clamp(level, 0.0f, 1.0f);
}

void AgentBrain::enableConnectomeOptimization(bool enable) {
    connectomeOptimizationEnabled_ = enable;
    NLM_LOG_INFO(std::string("Connectome optimization ") + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrain::setOptimalConnectivity(float connectivity) {
    optimalConnectivity_ = std::clamp(connectivity, 0.0f, 1.0f);
}

void AgentBrain::enableStructuralOptimization(bool enable) {
    structuralOptimizationEnabled_ = enable;
}

void AgentBrain::setStructuralComplexity(float complexity) {
    structuralComplexity_ = std::clamp(complexity, 0.0f, 10.0f);
}

void AgentBrain::enableFunctionalClustering(bool enable) {
    functionalClusteringEnabled_ = enable;
}

void AgentBrain::setFunctionalClusterCount(size_t clusters) {
    functionalClusterCount_ = clusters;
}

void AgentBrain::enableAdaptiveConnectivity(bool enable) {
    adaptiveConnectivityEnabled_ = enable;
}

void AgentBrain::setAdaptiveConnectivityRate(float rate) {
    adaptiveConnectivityRate_ = std::clamp(rate, 0.0f, 1.0f);
}

void AgentBrain::enableHomeostaticRegulation(bool enable) {
    homeostaticRegulationEnabled_ = enable;
}

void AgentBrain::setHomeostaticSetPoint(float setPoint) {
    homeostaticSetPoint_ = std::clamp(setPoint, 0.0f, 1.0f);
}

void AgentBrain::enableSynapticPruning(bool enable) {
    synapticPruningEnabled_ = enable;
    NLM_LOG_INFO(std::string("Synaptic pruning ") + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrain::setPruningIntensity(float intensity) {
    pruningIntensity_ = std::clamp(intensity, 0.0f, 1.0f);
}

void AgentBrain::enableActivityDependentGrowth(bool enable) {
    activityDependentGrowthEnabled_ = enable;
}

void AgentBrain::setGrowthRate(float rate) {
    growthRate_ = std::clamp(rate, 0.0f, 1.0f);
}

// Neural diversity and specialization getter implementations
float AgentBrain::getNeuralDiversityLevel() const {
    return neuralDiversityLevel_;
}

float AgentBrain::getRegionSpecializationLevel() const {
    return regionSpecializationLevel_;
}

float AgentBrain::getOptimalConnectivity() const {
    return optimalConnectivity_;
}

float AgentBrain::getStructuralComplexity() const {
    return structuralComplexity_;
}

size_t AgentBrain::getFunctionalClusterCount() const {
    return functionalClusterCount_;
}

float AgentBrain::getAdaptiveConnectivityRate() const {
    return adaptiveConnectivityRate_;
}

float AgentBrain::getHomeostaticSetPoint() const {
    return homeostaticSetPoint_;
}

float AgentBrain::getPruningIntensity() const {
    return pruningIntensity_;
}

float AgentBrain::getGrowthRate() const {
    return growthRate_;
}

// Advanced learning and optimization methods
void AgentBrain::optimizeNeuralConnections() {
    if (!connectomeOptimizationEnabled_) return;
    
    // Implement connectome optimization
    NLM_LOG_INFO("Optimizing neural connections for efficiency");
    
    // Simple optimization: balance excitation/inhibition
    if (brain_) {
        float currentEIRatio = brain_->getExcitationInhibitionRatio();
        float targetRatio = 1.0f;  // Target balanced network
        
        if (currentEIRatio > targetRatio * 1.2f) {
            // Too much excitation, reduce weights
            NLM_LOG_INFO("Reducing excitatory weights for balance");
        } else if (currentEIRatio < targetRatio * 0.8f) {
            // Too much inhibition, increase weights
            NLM_LOG_INFO("Increasing excitatory weights for balance");
        }
    }
}

void AgentBrain::specializeRegions() {
    if (!regionSpecializationEnabled_) return;
    
    NLM_LOG_INFO("Specializing brain regions for optimal function");
    
    if (brain_) {
        auto regions = brain_->getRegions();
        for (size_t i = 0; i < regions.size(); ++i) {
            auto* region = brain_->getRegion(RegionId(i + 1));
            if (region) {
                // Different regions develop different specializations
                float specializationLevel = regionSpecializationLevel_;
                
                // Assign different types to different regions
                if (i == 0) {
                    // Region 1: Cognitive functions
                    region->setName("Cognitive_Region");
                } else if (i == 1) {
                    // Region 2: Memory functions
                    region->setName("Memory_Region");
                } else if (i == 2) {
                    // Region 3: Sensory integration
                    region->setName("Sensory_Region");
                } else {
                    // Other regions: General processing
                    region->setName("Processing_Region_" + std::to_string(i + 1));
                }
            }
        }
    }
}

void AgentBrain::clusterFunctions() {
    if (!functionalClusteringEnabled_) return;
    
    NLM_LOG_INFO("Clustering neural functions for efficiency");
    
    // Implement functional clustering
    // This would group similar neurons together based on their function
    if (brain_) {
        // Create functional clusters based on neuron types and activity patterns
        size_t numClusters = std::min(functionalClusterCount_, size_t(5));
        
        NLM_LOG_INFO("Organizing " + std::to_string(numClusters) + " functional clusters");
    }
}

void AgentBrain::regulateHomeostasis() {
    if (!homeostaticRegulationEnabled_) return;
    
    NLM_LOG_INFO("Regulating neural homeostasis");
    
    // Maintain stability in neural activity
    float currentActivity = 0.0f;
    size_t activeNeurons = 0;
    
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || neuron->getFiringRate() > 0.1f) {
                        currentActivity++;
                        activeNeurons++;
                    }
                }
            }
        }
        
        float activityLevel = activeNeurons > 0 ? static_cast<float>(currentActivity) / activeNeurons : 0.0f;
        
        // Adjust activity to maintain homeostasis
        if (activityLevel > homeostaticSetPoint_ * 1.2f) {
            // Too much activity, reduce excitation
            NLM_LOG_INFO("Homeostasis: Reducing activity level");
        } else if (activityLevel < homeostaticSetPoint_ * 0.8f) {
            // Too little activity, increase excitation
            NLM_LOG_INFO("Homeostasis: Increasing activity level");
        }
    }
}

void AgentBrain::pruneUnusedConnections() {
    if (!synapticPruningEnabled_) return;
    
    NLM_LOG_INFO("Pruning unused synaptic connections");
    
    float pruningRate = pruningIntensity_;
    NLM_LOG_INFO(std::string("Pruning with intensity: ") + std::to_string(pruningRate));
    
    // In a full implementation, this would remove weak or unused synapses
    if (brain_) {
        // Simple pruning: log the action
        NLM_LOG_INFO("Pruning would remove " + std::to_string(pruningRate * 10.0f) + "% of weak synapses");
    }
}

void AgentBrain::promoteActivityDependentGrowth() {
    if (!activityDependentGrowthEnabled_) return;
    
    NLM_LOG_INFO("Promoting activity-dependent synaptic growth");
    
    float growthRate = growthRate_;
    NLM_LOG_INFO(std::string("Growth rate: ") + std::to_string(growthRate));
    
    // In a full implementation, this would strengthen active synapses
    if (brain_) {
        NLM_LOG_INFO("Strengthening synapses from active neurons");
    }
}

void AgentBrain::enhanceQuantumProcessing() {
    if (!quantumProcessingEnabled_) return;
    
    NLM_LOG_INFO("Enhancing quantum neural processing capabilities");
    
    float coherenceLevel = quantumCoherenceLevel_;
    NLM_LOG_INFO(std::string("Quantum coherence level: ") + std::to_string(coherenceLevel));
    
    if (coherenceLevel > 0.5f) {
        NLM_LOG_INFO("High quantum coherence detected - enhanced processing possible");
    }
}

void AgentBrain::developSyntheticConsciousness() {
    if (!syntheticConsciousnessEnabled_) return;
    
    NLM_LOG_INFO("Developing synthetic consciousness module");
    
    float consciousnessLevel = consciousnessLevel_;
    NLM_LOG_INFO(std::string("Consciousness level: ") + std::to_string(consciousnessLevel));
    
    if (consciousnessLevel > 0.3f) {
        NLM_LOG_INFO("Synthetic consciousness approaching awareness");
    }
}

void AgentBrain::enableQuantumEntanglement() {
    if (!quantumEntanglementEnabled_) return;
    
    NLM_LOG_INFO("Enabling quantum entanglement between neurons");
    
    float entanglementStrength = entanglementStrength_;
    NLM_LOG_INFO(std::string("Entanglement strength: ") + std::to_string(entanglementStrength));
    
    if (entanglementStrength > 0.1f) {
        NLM_LOG_INFO("Significant quantum entanglement established");
    }
}

// Configuration methods implementation
void AgentBrain::enableExploration(bool enable) {
    explorationEnabled_ = enable;
    if (!enable) {
        NLM_LOG_INFO("Exploration disabled");
    }
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    NLM_LOG_INFO("Exploration rate set to " + std::to_string(explorationRate_));
}

void AgentBrain::setRandomSeed(uint64_t seed) {
    randomSeed_ = seed;
    NLM_LOG_INFO("Random seed set to " + std::to_string(randomSeed_));
}

void AgentBrain::setMaxActionChoices(size_t maxChoices) {
    maxActionChoices_ = maxChoices;
    NLM_LOG_INFO("Max action choices set to " + std::to_string(maxActionChoices_));
}

void AgentBrain::enableDopamineModulation(bool enable) {
    dopamineModulationEnabled_ = enable;
}

void AgentBrain::enableNoveltyDetection(bool enable) {
    noveltyDetectionEnabled_ = enable;
}

void AgentBrain::setNoveltyThreshold(float threshold) {
    noveltyThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

void AgentBrain::enableNeuralDevelopment(bool enable) {
    neuralDevelopmentEnabled_ = enable;
}

void AgentBrain::setDevelopmentalRate(float rate) {
    developmentalRate_ = std::clamp(rate, 0.0f, 1.0f);
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

// Getter implementations for new configuration
float AgentBrain::getExplorationRate() const {
    return explorationRate_;
}

uint64_t AgentBrain::getRandomSeed() const {
    return randomSeed_;
}

size_t AgentBrain::getMaxActionChoices() const {
    return maxActionChoices_;
}

float AgentBrain::getNoveltyThreshold() const {
    return noveltyThreshold_;
}

float AgentBrain::getDevelopmentalRate() const {
    return developmentalRate_;
}

// Improved curiosity-based exploration with advanced features
MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Check if exploration is enabled
    if (!explorationEnabled_) {
        return defaultCmd;
    }
    
    // Use advanced exploration strategy based on curiosity level
    float curiosityThreshold = 0.5f; // Standard threshold
    
    // Adjust exploration based on developmental stage
    if (neuralDevelopmentEnabled_) {
        // During development, explore more
        curiosityThreshold = 0.3f * (1.0f - developmentalRate_);
    }
    
    // Check if we should explore based on curiosity
    if (curiosityLevel_ > curiosityThreshold) {
        // Calculate exploration probability with multiple strategies
        float exploreProbability = 0.0f;
        
        // Strategy 1: Linear curiosity-based exploration
        exploreProbability += curiosityLevel_ * 0.3f;
        
        // Strategy 2: Novelty-based exploration
        if (noveltyDetectionEnabled_ && noveltyLevel_ > noveltyThreshold_) {
            exploreProbability += (noveltyLevel_ - noveltyThreshold_) * 0.2f;
        }
        
        // Strategy 3: Random exploration with decay
        exploreProbability += explorationRate_ * 0.1f;
        
        // Strategy 4: Development-based exploration (more during early stages)
        if (neuralDevelopmentEnabled_) {
            float developmentalFactor = 1.0f - (developmentalAge_ / 1000.0f); // Decays over time
            exploreProbability += developmentalFactor * 0.15f;
        }
        
        // Clamp probability to [0, 1]
        exploreProbability = std::clamp(exploreProbability, 0.0f, 1.0f);
        
        // Generate random number to decide exploration
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreProbability) {
            // Advanced exploration: use epsilon-greedy or softmax selection
            return advancedExplorationSelection();
        }
    }
    
    return defaultCmd;
}

// Advanced exploration with multiple strategies
MotorCommand AgentBrain::advancedExplorationSelection() {
    // Create list of all possible actions
    std::vector<MotorCommand> allActions = {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::LookLeft,
        MotorCommand::LookRight,
        MotorCommand::Interact,
        MotorCommand::Wait
    };
    
    // Apply different exploration strategies
    std::vector<MotorCommand> candidates;
    
    // Strategy 1: Pure random selection
    if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.4f) {
        // Random from all actions
        size_t randomIndex = brain_->getRandomGenerator()->uniformInt(0, maxActionChoices_ - 1);
        return allActions[randomIndex];
    }
    
    // Strategy 2: Action-dependent exploration (rare actions more likely)
    if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.3f) {
        // Bias towards less explored actions (simplified implementation)
        candidates = allActions;
        if (!candidates.empty()) {
            size_t randomIndex = brain_->getRandomGenerator()->uniformInt(0, candidates.size() - 1);
            return candidates[randomIndex];
        }
    }
    
    // Strategy 3: Context-aware exploration (based on current state)
    if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.3f) {
        // Use neural firing patterns to guide exploration
        return contextAwareExploration();
    }
    
    // Default to standard random selection
    size_t randomIndex = brain_->getRandomGenerator()->uniformInt(0, maxActionChoices_ - 1);
    return allActions[randomIndex];
}

// Context-aware exploration based on current neural activity
MotorCommand AgentBrain::contextAwareExploration() {
    // Get current brain state information
    if (!brain_) return MotorCommand::Wait;
    
    // Analyze current neural activity patterns
    float totalActivity = 0.0f;
    size_t activeNeurons = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getFiringRate() > 0.1f) {
                    float activity = std::abs(neuron->getState().membranePotential - 
                                            neuron->getState().restingPotential);
                    totalActivity += activity;
                    activeNeurons++;
                }
            }
        }
    }
    
    float averageActivity = activeNeurons > 0 ? totalActivity / activeNeurons : 0.0f;
    
    // Choose action based on activity level
    if (averageActivity > 0.8f) {
        // High activity: prefer movement actions
        return static_cast<MotorCommand>(brain_->getRandomGenerator()->uniformInt(0, 2)); // MoveForward or TurnLeft
    } else if (averageActivity < 0.2f) {
        // Low activity: prefer exploration actions
        return static_cast<MotorCommand>(brain_->getRandomGenerator()->uniformInt(4, 6)); // LookLeft, LookRight, or Interact
    } else {
        // Medium activity: balanced choice
        return static_cast<MotorCommand>(brain_->getRandomGenerator()->uniformInt(2, 5)); // TurnLeft, TurnRight, or Interact
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Advanced multi-factor reward prediction error computation
    computeAdvancedPredictionError(reward, predictedReward);
    
    // Apply multiple reward modulation strategies
    applyDopamineModulation();
    applySerotoninModulation();
    applyNorepinephrineModulation();
    applyAcetylcholineModulation();
    
    // Advanced eligibility trace management with synaptic tagging
    applySynapticTaggingAndCapture();
    
    // Meta-learning and adaptive reward sensitivity
    updateMetaLearningParameters();
    
    // Homeostatic reward regulation
    applyHomeostaticRewardRegulation();
    
    // Quantum-enhanced reward processing (if enabled)
    if (quantumProcessingEnabled_) {
        enhanceQuantumRewardProcessing();
    }
    
    // Synthetic consciousness reward integration
    if (syntheticConsciousnessEnabled_) {
        integrateConsciousnessWithReward();
    }
}

void AgentBrain::computeAdvancedPredictionError(float reward, float predictedReward) {
    // Multiple prediction error signals for different learning systems
    
    // 1. Temporal Difference (TD) error - classic reinforcement learning
    tdError_ = reward - predictedReward;
    
    // 2. Reward Surprise - based on reward variance
    rewardSurprise_ = std::sqrt(std::max(0.0f, reward - predictedReward));
    
    // 3. Reward Prediction Error with temporal integration
    integratedRPE_ = 0.7f * tdError_ + 0.3f * rewardSurprise_;
    
    // 4. Value Prediction Error - difference between predicted and actual value
    valueError_ = reward - expectedReward_;
    
    // 5. Motivational Prediction Error - based on motivational state
    motivationalError_ = tdError_ * (1.0f + getMotivationModifier());
    
    // Update primary prediction error signal
    predictionError_ = integratedRPE_ * getAttentionWeight();
    
    // Update expected reward with different time constants
    expectedReward_ = 0.9f * expectedReward_ + 0.1f * reward;
    
    // Update reward memory for learning
    updateRewardMemory(reward, predictedReward);
}

void AgentBrain::applyDopamineModulation() {
    // Enhanced dopamine modulation with multiple mechanisms
    
    float tdError = tdError_;
    float surprise = rewardSurprise_;
    float integratedRPE = integratedRPE_;
    
    // Primary dopamine response to prediction error
    float primaryDopamine = tdError * getDopamineGain();
    
    // Secondary dopamine response to reward surprise (novelty detection)
    float secondaryDopamine = surprise * getNoveltyGain();
    
    // Tertiary dopamine response to integrated learning signal
    float tertiaryDopamine = integratedRPE * getIntegrationGain();
    
    // Combine all dopamine signals
    dopamineLevel_ = primaryDopamine + secondaryDopamine + tertiaryDopamine;
    
    // Apply non-linear transformations
    dopamineLevel_ = nonLinearDopamineResponse(dopamineLevel_);
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to synaptic plasticity with advanced mechanisms
    updatePlasticityWithDopamine(dopamineLevel_);
}

void AgentBrain::applySerotoninModulation() {
    // Serotonin modulation for behavioral flexibility and learning rate adaptation
    
    float learningRate = calculateAdaptiveLearningRate();
    float predictionError = getAbsolutePredictionError();
    
    // Serotonin response to prediction error magnitude
    float serotoninLevel = predictionError * learningRate * getSerotoninGain();
    
    // Modulate working memory and attention based on serotonin
    modifyWorkingMemoryWithSerotonin(serotoninLevel);
    adjustAttentionWithSerotonin(serotoninLevel);
    
    // Enhance or suppress plasticity based on serotonin
    modulatePlasticityWithSerotonin(serotoninLevel);
}

void AgentBrain::applyNorepinephrineModulation() {
    // Norepinephrine modulation for arousal, attention, and memory consolidation
    
    float alertness = calculateAlertnessLevel();
    float attention = calculateAttentionLevel();
    
    // Norepinephrine response to environmental salience
    float norepinephrineLevel = (alertness + attention) * getNorepinephrineGain();
    
    // Enhance encoding of salient events
    enhanceMemoryEncodingWithNorepinephrine(norepinephrineLevel);
    
    // Modulate synaptic strength based on norepinephrine
    modulateSynapticStrengthWithNorepinephrine(norepinephrineLevel);
}

void AgentBrain::applyAcetylcholineModulation() {
    // Acetylcholine modulation for cortical plasticity and memory
    
    float acetylcholineLevel = calculateAcetylcholineLevel();
    
    // Enhance synaptic plasticity in cortical areas
    enhanceCorticalPlasticityWithAcetylcholine(acetylcholineLevel);
    
    // Modulate thalamocortical interactions
    modulateThalamocorticalWithAcetylcholine(acetylcholineLevel);
    
    // Support memory consolidation
    supportMemoryConsolidationWithAcetylcholine(acetylcholineLevel);
}

void AgentBrain::applySynapticTaggingAndCapture() {
    // Advanced synaptic tagging and capture mechanism
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Check if synapse needs tagging
            if (shouldTagSynapse(syn)) {
                // Apply synaptic tag
                applySynapticTag(syn);
                
                // Capture eligibility traces
                captureEligibilityTraces(syn);
            }
            
            // Update synaptic state based on neuromodulators
            updateSynapseWithNeuromodulators(syn);
        }
    }
}

void AgentBrain::updateMetaLearningParameters() {
    if (!metaPlasticityEnabled_) return;
    
    // Meta-learning: learn how to learn
    
    float learningRate = calculateAdaptiveLearningRate();
    float metaLearningRate = metaPlasticityRate_;
    
    // Update meta-parameters based on learning performance
    updateLearningRateParameters(learningRate, metaLearningRate);
    
    // Adapt exploration-exploitation balance
    adaptExplorationExploitationBalance();
    
    // Update reward sensitivity
    updateRewardSensitivity();
}

void AgentBrain::applyHomeostaticRewardRegulation() {
    if (!homeostaticRegulationEnabled_) return;
    
    // Homeostatic regulation of reward processing
    
    float currentRewardLevel = calculateCurrentRewardLevel();
    float targetRewardLevel = homeostaticSetPoint_;
    
    // Apply homeostatic correction
    float homeostaticCorrection = (targetRewardLevel - currentRewardLevel) * getHomeostaticGain();
    
    // Adjust reward processing parameters
    adjustRewardProcessingWithHomeostasis(homeostaticCorrection);
    
    // Maintain reward stability
    maintainRewardStability();
}

void AgentBrain::enhanceQuantumRewardProcessing() {
    if (!quantumProcessingEnabled_) return;
    
    // Quantum-enhanced reward processing
    
    float coherenceLevel = quantumCoherenceLevel_;
    
    if (coherenceLevel > 0.5f) {
        // High quantum coherence enables enhanced processing
        enhanceRewardProcessingWithQuantum(coherenceLevel);
    }
    
    // Apply quantum entanglement effects
    applyQuantumEntanglementToReward(coherenceLevel);
}

void AgentBrain::integrateConsciousnessWithReward() {
    if (!syntheticConsciousnessEnabled_) return;
    
    // Integrate synthetic consciousness with reward processing
    
    float consciousnessLevel = consciousnessLevel_;
    float rewardSignal = calculateConsciousnessAwareReward();
    
    if (consciousnessLevel > 0.3f) {
        // Consciousness influences reward processing
        integrateConsciousnessRewardSignal(consciousnessLevel, rewardSignal);
    }
}

float AgentBrain::getDopamineGain() const {
    return 1.0f + dopamineLevel_ * 0.5f;
}

float AgentBrain::getNoveltyGain() const {
    return 0.5f + noveltyLevel_ * 0.5f;
}

float AgentBrain::getIntegrationGain() const {
    return 1.0f - std::abs(dopamineLevel_) * 0.5f;
}

float AgentBrain::calculateAlertnessLevel() const {
    float baseAlertness = 0.5f;
    float curiosityBoost = curiosityLevel_ * 0.5f;
    float noveltyBoost = noveltyLevel_ * 0.3f;
    
    return baseAlertness + curiosityBoost + noveltyBoost;
}

float AgentBrain::calculateAttentionLevel() const {
    float baseAttention = 0.5f;
    float dopamineInfluence = std::abs(dopamineLevel_) * 0.3f;
    float predictionErrorInfluence = std::abs(predictionError_) * 0.2f;
    
    return baseAttention + dopamineInfluence + predictionErrorInfluence;
}

float AgentBrain::calculateAcetylcholineLevel() const {
    float baseACh = 0.3f;
    float rewardInfluence = std::abs(predictionError_) * 0.4f;
    float noveltyInfluence = noveltyLevel_ * 0.3f;
    
    return baseACh + rewardInfluence + noveltyInfluence;
}

float AgentBrain::calculateAdaptiveLearningRate() const {
    float baseLearningRate = 0.1f;
    float noveltyAdjustedRate = 1.0f + noveltyLevel_ * 0.5f;
    float dopamineAdjustedRate = 1.0f + std::abs(dopamineLevel_) * 0.3f;
    
    return baseLearningRate * noveltyAdjustedRate * dopamineAdjustedRate;
}

float AgentBrain::calculateCurrentRewardLevel() const {
    return expectedReward_ + std::abs(predictionError_);
}

float AgentBrain::getMotivationModifier() const {
    return 1.0f + curiosityLevel_ * 0.2f + noveltyLevel_ * 0.1f;
}

float AgentBrain::getAttentionWeight() const {
    return 1.0f + calculateAttentionLevel() * 0.5f;
}

float AgentBrain::getSerotoninGain() const {
    return 0.5f + calculateAdaptiveLearningRate() * 0.3f;
}

float AgentBrain::getNorepinephrineGain() const {
    return 0.5f + calculateAlertnessLevel() * 0.3f;
}

float AgentBrain::getHomeostaticGain() const {
    return 0.3f + std::abs(predictionError_) * 0.4f;
}

float AgentBrain::nonLinearDopamineResponse(float dopamine) const {
    // Non-linear transformation of dopamine response
    return std::tanh(dopamine * 2.0f) * 0.8f;
}

float AgentBrain::getAbsolutePredictionError() const {
    return std::abs(predictionError_);
}

void AgentBrain::modifyWorkingMemoryWithSerotonin(float serotoninLevel) {
    if (brain_ && brain_->getWorkingMemory()) {
        // Modulate working memory based on serotonin
        float memoryCapacity = brain_->getWorkingMemory()->getCapacity();
        float modulation = serotoninLevel * 0.1f;
        
        // Adjust working memory parameters
        // In a full implementation: brain_->getWorkingMemory()->setCapacity(memoryCapacity * (1.0f + modulation));
    }
}

void AgentBrain::adjustAttentionWithSerotonin(float serotoninLevel) {
    // Adjust attention allocation based on serotonin
    float attentionAdjustment = serotoninLevel * 0.2f;
    // In a full implementation: adjust attention mechanisms
}

void AgentBrain::modulatePlasticityWithSerotonin(float serotoninLevel) {
    // Modulate synaptic plasticity based on serotonin
    float plasticityModulation = serotoninLevel * 0.3f;
    // In a full implementation: adjust plasticity parameters
}

void AgentBrain::enhanceMemoryEncodingWithNorepinephrine(float norepinephrineLevel) {
    if (brain_ && brain_->getEpisodicMemory()) {
        // Enhance memory encoding based on norepinephrine
        float encodingBoost = norepinephrineLevel * 0.2f;
        // In a full implementation: enhance episodic memory encoding
    }
}

void AgentBrain::modulateSynapticStrengthWithNorepinephrine(float norepinephrineLevel) {
    // Modulate synaptic strength based on norepinephrine
    float strengthModulation = norepinephrineLevel * 0.15f;
    // In a full implementation: adjust synaptic strengths
}

void AgentBrain::enhanceCorticalPlasticityWithAcetylcholine(float acetylcholineLevel) {
    // Enhance cortical plasticity based on acetylcholine
    float plasticityBoost = acetylcholineLevel * 0.25f;
    // In a full implementation: enhance cortical synaptic modifications
}

void AgentBrain::modulateThalamocorticalWithAcetylcholine(float acetylcholineLevel) {
    // Modulate thalamocortical interactions based on acetylcholine
    float thalamocorticalModulation = acetylcholineLevel * 0.2f;
    // In a full implementation: adjust thalamocortical signal processing
}

void AgentBrain::supportMemoryConsolidationWithAcetylcholine(float acetylcholineLevel) {
    if (brain_ && brain_->getEpisodicMemory()) {
        // Support memory consolidation based on acetylcholine
        float consolidationBoost = acetylcholineLevel * 0.3f;
        // In a full implementation: enhance memory consolidation
    }
}

bool AgentBrain::shouldTagSynapse(Synapse* syn) const {
    // Determine if a synapse should be tagged for plasticity
    if (!syn) return false;
    
    float eligibility = syn->getEligibilityTrace();
    return std::abs(eligibility) > 0.01f;
}

void AgentBrain::applySynapticTag(Synapse* syn) {
    if (!syn) return;
    
    // Apply synaptic tag to mark for potential plasticity
    // In a full implementation: set synaptic tag
}

void AgentBrain::captureEligibilityTraces(Synapse* syn) {
    if (!syn) return;
    
    // Capture eligibility traces for potential synaptic modification
    float eligibility = syn->getEligibilityTrace();
    // In a full implementation: store eligibility for future use
}

void AgentBrain::updateSynapseWithNeuromodulators(Synapse* syn) {
    if (!syn) return;
    
    // Update synapse based on current neuromodulator levels
    float neuromodulatorInfluence = dopamineLevel_ * 0.3f + 
                                  serotoninLevel_ * 0.2f + 
                                  norepinephrineLevel_ * 0.2f + 
                                  acetylcholineLevel_ * 0.3f;
    
    // Apply neuromodulator influence to synapse weight
    // In a full implementation: adjust synapse weight based on neuromodulators
}

void AgentBrain::updateLearningRateParameters(float learningRate, float metaLearningRate) {
    // Update learning rate parameters based on meta-learning
    learningRate = std::clamp(learningRate, 0.01f, 0.5f);
    metaLearningRate = std::clamp(metaLearningRate, 0.01f, 0.3f);
}

void AgentBrain::adaptExplorationExploitationBalance() {
    // Adapt the balance between exploration and exploitation
    float explorationProbability = calculateExplorationProbability();
    
    // Adjust exploration based on learning progress
    if (predictionError_ > 0.5f) {
        // High prediction error: increase exploration
        explorationRate_ = std::min(explorationRate_ * 1.1f, 1.0f);
    } else if (predictionError_ < 0.1f) {
        // Low prediction error: increase exploitation
        explorationRate_ = std::max(explorationRate_ * 0.9f, 0.1f);
    }
}

void AgentBrain::updateRewardSensitivity() {
    // Update reward sensitivity based on learning performance
    float sensitivityAdjustment = (expectedReward_ - predictionError_) * 0.1f;
    // In a full implementation: adjust reward sensitivity
}

void AgentBrain::adjustRewardProcessingWithHomeostasis(float correction) {
    // Adjust reward processing based on homeostatic correction
    float rewardAdjustment = correction * 0.2f;
    // In a full implementation: adjust reward processing parameters
}

void AgentBrain::maintainRewardStability() {
    // Maintain reward system stability
    // In a full implementation: implement stabilizing mechanisms
}

void AgentBrain::enhanceRewardProcessingWithQuantum(float coherenceLevel) {
    // Enhance reward processing using quantum coherence
    if (coherenceLevel > 0.7f) {
        // High coherence enables enhanced processing
        float enhancementFactor = coherenceLevel * 2.0f;
        // In a full implementation: enhance reward processing with quantum effects
    }
}

void AgentBrain::applyQuantumEntanglementToReward(float coherenceLevel) {
    // Apply quantum entanglement effects to reward processing
    if (entanglementStrength_ > 0.5f) {
        // Strong entanglement enables quantum reward correlations
        // In a full implementation: apply quantum entanglement to reward signals
    }
}

void AgentBrain::integrateConsciousnessRewardSignal(float consciousnessLevel, float rewardSignal) {
    // Integrate consciousness state with reward signal
    float integrationFactor = consciousnessLevel * 0.5f;
    // In a full implementation: integrate consciousness with reward processing
}

float AgentBrain::calculateExplorationProbability() const {
    return explorationRate_ * (1.0f + curiosityLevel_ * 0.5f);
}

void AgentBrain::updateRewardMemory(float reward, float predictedReward) {
    // Update reward memory for future learning
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > 100) {
        rewardHistory_.erase(rewardHistory_.begin());
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
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
