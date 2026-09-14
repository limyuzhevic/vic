#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/DevelopmentalStage.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <memory>

namespace nlm {

// AgentBrain Configuration
struct AgentBrainConfig {
    // Motor neuron organization
    float motorNeuronDistributionBias_;  // Bias for action selection (0.0 = uniform, 1.0 = forward preference)
    
    // Curiosity parameters
    float curiosityThreshold_;           // Minimum curiosity level to trigger exploration
    float explorationDecayRate_;          // How quickly curiosity decays over time
    float noveltySensitivity_;            // How sensitive agent is to novelty
    float predictionErrorWeight_;         // Weight of prediction error in curiosity calculation
    
    // Development parameters
    float developmentSpeed_;              // Rate of developmental progression
    float plasticityCurve_[4];            // Plasticity modifiers for each developmental stage
    
    // Neuromodulation parameters
    float dopamineLearningRate_;          // Rate of dopamine signal adaptation
    float rewardPredictionErrorGain_;     // Gain factor for prediction error signals
    
    // Memory integration
    bool episodicMemoryEnabled_;          // Enable episodic memory recording
    float memoryConsolidationRate_;       // Rate at which memories consolidate
    
    // Environmental adaptation
    float temperatureAdaptation_;         // Adaptation to environmental stability
    float socialLearningRate_;             // Rate of social learning from other agents
    
    // Safety and constraints
    float maxEnergyLevel_;                 // Maximum energy before fatigue
    float stressThreshold_;               // Stress level triggering defensive behaviors
    
    AgentBrainConfig() 
        : motorNeuronDistributionBias_(0.5f)
        , curiosityThreshold_(0.3f)
        , explorationDecayRate_(0.995f)
        , noveltySensitivity_(1.0f)
        , predictionErrorWeight_(0.5f)
        , developmentSpeed_(1.0f)
        , episodicMemoryEnabled_(true)
        , memoryConsolidationRate_(0.9f)
        , temperatureAdaptation_(0.8f)
        , socialLearningRate_(0.2f)
        , maxEnergyLevel_(1.0f)
        , stressThreshold_(0.8f) {
        
        // Initialize developmental plasticity curve
        plasticityCurve_[0] = 1.0f;  // Initial - high plasticity
        plasticityCurve_[1] = 0.8f;  // Critical period - peak plasticity
        plasticityCurve_[2] = 0.5f;  // Maturation - moderate plasticity
        plasticityCurve_[3] = 0.2f;  // Adult - low plasticity
    }
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrainConfig> config)
    : brain_(brain)
    , config_(config ? config : std::make_shared<AgentBrainConfig>())
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
    , stressLevel_(0.0f)
    , energyLevel_(1.0f)
    , explorationHistory_()
    , learnedSequences_() {
    
    // Initialize random number generator for exploration
    randomEngine_.seed(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Initialize motor and sensory neuron groups with improved organization
    if (brain_) {
        organizeNeuronsImproved();
    }
}
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

// Improved neuron organization with better distribution and dynamics
    void organizeNeuronsImproved() {
        // Clear existing groups
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
        
        if (!brain_) return;
        
        // Get all regions and populations
        auto regions = brain_->getRegions();
        if (regions.empty()) return;
        
        std::vector<Neuron*> allMotorNeurons;
        std::vector<Neuron*> allSensoryNeurons;
        
        // Collect neurons by type
        for (const auto& region : regions) {
            if (!region) continue;
            auto populations = region->getPopulations();
            for (auto& pop : populations) {
                if (!pop) continue;
                NeuronType type = pop->getNeuronType();
                
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) continue;
                    
                    if (type == NeuronType::Motor) {
                        allMotorNeurons.push_back(n);
                    } else if (type == NeuronType::Sensory) {
                        allSensoryNeurons.push_back(n);
                    }
                }
            }
        }
        
        // Improved motor neuron distribution with bias
        if (!allMotorNeurons.empty()) {
            distributeMotorNeuronsBiased(allMotorNeurons);
        }
        
        // Improved sensory neuron distribution based on type hierarchy
        if (!allSensoryNeurons.empty()) {
            distributeSensoryNeuronsHierarchical(allSensoryNeurons);
        }
        
        NLM_LOG_INFO("Improved neuron organization: " +
                    std::to_string(allMotorNeurons.size()) + " motor, " +
                    std::to_string(allSensoryNeurons.size()) + " sensory neurons distributed");
    }
    
    // Biased motor neuron distribution
    void distributeMotorNeuronsBiased(const std::vector<Neuron*>& motorNeurons) {
        // Apply bias based on configuration
        float bias = config_->motorNeuronDistributionBias_;
        bias = std::clamp(bias, 0.0f, 1.0f);
        
        std::vector<std::pair<Neuron*, float>> weightedNeurons;
        
        for (Neuron* n : motorNeurons) {
            // Higher probability for forward movement if bias > 0.5
            float weight = 1.0f;
            if (bias > 0.5f) {
                // Preference for forward action
                weight = 1.0f + (bias - 0.5f) * 2.0f;  // Weight range: 1.0 to 3.0
            } else if (bias < 0.5f) {
                // Preference for other actions
                weight = 1.0f + (0.5f - bias) * 1.5f;  // Weight range: 1.0 to 2.0
            }
            
            weightedNeurons.push_back({n, weight});
        }
        
        // Sort by weight for deterministic distribution
        std::sort(weightedNeurons.begin(), weightedNeurons.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Distribute with priority
        for (size_t i = 0; i < weightedNeurons.size(); ++i) {
            Neuron* n = weightedNeurons[i].first;
            
            // Calculate priority based on index
            float priority = static_cast<float>(i) / weightedNeurons.size();
            
            if (priority > 0.6f) {  // Top 40% get forward action
                motorForward_.push_back(n);
            } else if (priority > 0.4f) {  // Next 20% get turn actions
                if (i % 2 == 0) motorTurnLeft_.push_back(n);
                else motorTurnRight_.push_back(n);
            } else if (priority > 0.2f) {  // Next 20% get backward and interact
                if (i % 2 == 0) motorBackward_.push_back(n);
                else motorInteract_.push_back(n);
            } else {  // Bottom 20% get wait
                motorWait_.push_back(n);
            }
        }
    }
    
    // Hierarchical sensory neuron distribution
    void distributeSensoryNeuronsHierarchical(const std::vector<Neuron*>& sensoryNeurons) {
        // Determine distribution based on sensory importance hierarchy
        // Vision (0) > Touch (1) > Internal (2) > Proprioception (3)
        
        // Estimate neuron types by sampling (full scan would be expensive)
        std::vector<NeuronType> typeSamples;
        typeSamples.reserve(std::min(size_t(50), sensoryNeurons.size()));
        
        size_t sampleCount = std::min(size_t(50), sensoryNeurons.size());
        for (size_t i = 0; i < sampleCount; ++i) {
            // Try to get neuron type from brain (approximation)
            if (brain_) {
                // Use a simple heuristic - in practice, you'd have better access
                // to neuron type information
                typeSamples.push_back(NeuronType::Sensory);  // Default to sensory
            }
        }
        
        // Count estimated types
        size_t visionCount = 0, touchCount = 0, internalCount = 0, proprioCount = 0;
        
        // Distribute based on estimated counts and hierarchy
        for (Neuron* n : sensoryNeurons) {
            if (!n) continue;
            
            // Simple round-robin based on hierarchy
            size_t total = visionCount + touchCount + internalCount + proprioCount;
            if (total >= sensoryNeurons.size()) {
                proprioCount++;  // Default to proprioception
            } else if (visionCount <= touchCount && visionCount <= internalCount && visionCount <= proprioCount) {
                visionCount++;
                sensoryVision_.push_back(n);
            } else if (touchCount <= internalCount && touchCount <= proprioCount) {
                touchCount++;
                sensoryTouch_.push_back(n);
            } else if (internalCount <= proprioCount) {
                internalCount++;
                sensoryInternal_.push_back(n);
            } else {
                proprioCount++;
                sensoryProprioception_.push_back(n);
            }
        }
        
        NLM_LOG_INFO("Hierarchical sensory distribution: " +
                    std::to_string(sensoryVision_.size()) + " vision, " +
                    std::to_string(sensoryTouch_.size()) + " touch, " +
                    std::to_string(sensoryInternal_.size()) + " internal, " +
                    std::to_string(sensoryProprioception_.size()) + " proprioception");
    }
    
    // Enhanced novelty computation with memory comparison
    float computeNoveltyEnhanced(const std::vector<float>& vision) {
        if (vision.empty() || previousVision_.empty())
            return 0.0f;
        
        float totalDiff = 0.0f;
        size_t count = std::min(vision.size(), previousVision_.size());
        
        // Multiple comparison strategies
        for (size_t i = 0; i < count; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
            
            // Emphasize sudden changes vs gradual changes
            if (diff > 0.5f) {
                totalDiff += diff * 0.5f;  // Boost for large changes
            }
        }
        
        // Normalize
        float novelty = totalDiff / std::max<size_t>(count, 1);
        
        // Apply temperature adaptation - reduce novelty sensitivity in stable environments
        novelty *= config_->temperatureAdaptation_;
        
        // Store for next time
        previousVision_ = vision;
        
        return novelty;
    }
    
    // Enhanced curiosity computation with multiple factors
    float computeCuriosityEnhanced(const std::vector<float>& vision, float predictionError) {
        float novelty = computeNoveltyEnhanced(vision);
        
        // Multiple curiosity components
        float noveltyComponent = novelty * config_->noveltySensitivity_;
        float predictionErrorComponent = std::abs(predictionError) * config_->predictionErrorWeight_;
        
        // Exploration history factor - more exploration if we've tried same actions
        float explorationFactor = 1.0f;
        if (!explorationHistory_.empty()) {
            // Analyze recent exploration patterns
            size_t recentActions = std::min(size_t(10), explorationHistory_.size());
            float explorationRate = 0.0f;
            for (size_t i = 0; i < recentActions; ++i) {
                if (explorationHistory_[i] == MotorCommand::Wait) {
                    explorationRate += 0.1f;
                }
            }
            explorationFactor = 1.0f + explorationRate;
        }
        
        // Combined curiosity with non-linear interaction
        float curiosity = noveltyComponent * 0.6f + predictionErrorComponent * 0.4f;
        curiosity *= explorationFactor;
        
        // Apply curiosity threshold from config
        if (curiosity < config_->curiosityThreshold_) {
            curiosity = 0.0f;
        }
        
        // Clamp to reasonable range
        curiosity = std::clamp(curiosity, 0.0f, 1.0f);
        
        return curiosity;
    }
    
    // Enhanced development stage system with gradual transitions
    void updateDevelopmentEnhanced(double timestep) {
        if (!brain_ || !developmentEnabled_) return;
        
        developmentalAge_ += timestep * config_->developmentSpeed_;
        
        // Smooth developmental transitions
        float normalizedAge = std::clamp(static_cast<float>(developmentalAge_) / 900.0f, 0.0f, 1.0f);
        
        // Quadratic easing for smooth transitions between stages
        float stageProgress;
        DevelopmentalStage newStage;
        
        if (normalizedAge < 0.33f) {
            stageProgress = normalizedAge / 0.33f;
            stageProgress = stageProgress * stageProgress;  // Quadratic ease-in
            newStage = DevelopmentalStage::Initial;
            plasticityModifier_ = config_->plasticityCurve_[0] * (1.0f - stageProgress) + 
                                config_->plasticityCurve_[1] * stageProgress;
        } else if (normalizedAge < 0.67f) {
            float progress = (normalizedAge - 0.33f) / 0.34f;
            stageProgress = progress * progress;
            newStage = DevelopmentalStage::CriticalPeriod;
            plasticityModifier_ = config_->plasticityCurve_[1] * (1.0f - stageProgress) + 
                                config_->plasticityCurve_[2] * stageProgress;
        } else if (normalizedAge < 0.97f) {
            float progress = (normalizedAge - 0.67f) / 0.3f;
            stageProgress = progress * progress;
            newStage = DevelopmentalStage::Maturation;
            plasticityModifier_ = config_->plasticityCurve_[2] * (1.0f - stageProgress) + 
                                config_->plasticityCurve_[3] * stageProgress;
        } else {
            stageProgress = (normalizedAge - 0.97f) / 0.03f;
            stageProgress = stageProgress * stageProgress;
            newStage = DevelopmentalStage::Adult;
            plasticityModifier_ = config_->plasticityCurve_[3] * (1.0f - stageProgress) + 0.1f * stageProgress;
        }
        
        brain_->setDevelopmentalStage(newStage);
        
        // Enhanced structural plasticity with developmental modulation
        if (structuralPlasticityEnabled_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                // Dynamic rates based on developmental stage
                float synaptogenesisFactor = 1.0f;
                float pruningFactor = 1.0f;
                
                switch (newStage) {
                    case DevelopmentalStage::Initial:
                        synaptogenesisFactor = 2.0f;
                        pruningFactor = 0.5f;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        synaptogenesisFactor = 1.5f;
                        pruningFactor = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        synaptogenesisFactor = 1.0f;
                        pruningFactor = 1.2f;
                        break;
                    case DevelopmentalStage::Adult:
                        synaptogenesisFactor = 0.5f;
                        pruningFactor = 1.5f;
                        break;
                    case DevelopmentalStage::Aging:
                        synaptogenesisFactor = 0.2f;
                        pruningFactor = 2.0f;
                        break;
                }
                
                // Temperature adaptation affects plasticity
                synaptogenesisFactor *= config_->temperatureAdaptation_;
                pruningFactor /= config_->temperatureAdaptation_;
                
                float synRate = 0.0001f * synaptogenesisFactor * plasticityModifier_;
                float pruneRate = 0.00001f * pruningFactor * (2.0f - plasticityModifier_);
                
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
        
        // Stress level affects development
        if (stressLevel_ > config_->stressThreshold_) {
            // Stress slows development but increases plasticity
            developmentSpeed_ *= 0.8f;
            plasticityModifier_ *= 1.5f;
        }
    }
    
    // Social learning and pattern extraction from exploration history
    void learnFromExplorationHistory() {
        if (explorationHistory_.size() < 10) return;
        
        // Analyze action sequences for learned behaviors
        std::unordered_map<MotorCommand, int> actionCounts;
        for (size_t i = 0; i < explorationHistory_.size(); ++i) {
            actionCounts[explorationHistory_[i]]++;
        }
        
        // Identify dominant action patterns
        MotorCommand mostCommonAction = MotorCommand::Wait;
        int maxCount = 0;
        for (const auto& pair : actionCounts) {
            if (pair.second > maxCount) {
                maxCount = pair.second;
                mostCommonAction = pair.first;
            }
        }
        
        // Store learned sequence if significant
        if (maxCount >= explorationHistory_.size() / 2) {
            LearnedSequence seq;
            seq.action = mostCommonAction;
            seq.frequency = static_cast<float>(maxCount) / explorationHistory_.size();
            seq.context = "exploration_pattern";
            
            // Check if we already have this sequence
            bool exists = false;
            for (const auto& existing : learnedSequences_) {
                if (existing.action == seq.action) {
                    exists = true;
                    existing.frequency = (existing.frequency + seq.frequency) * 0.5f;
                    break;
                }
            }
            
            if (!exists) {
                learnedSequences_.push_back(seq);
            }
        }
        
        // Clear old history to prevent memory buildup
        if (explorationHistory_.size() > 100) {
            explorationHistory_.erase(explorationHistory_.begin(), 
                                    explorationHistory_.begin() + 50);
        }
    }
    
    // Enhanced neuromodulation with stress and energy factors
    void applyRewardModulationEnhanced(float reward, float predictedReward) {
        if (!brain_ || !rewardModulationEnabled_) return;
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward with adaptive learning rate
        expectedReward_ = expectedReward_ * (1.0f - config_->dopamineLearningRate_) + 
                         reward * config_->dopamineLearningRate_;
        
        // Enhanced dopamine-like signal with multiple factors
        float predictionErrorComponent = predictionError_ * config_->rewardPredictionErrorGain_;
        
        // Energy and stress modulation
        float energyFactor = energyLevel_ / config_->maxEnergyLevel_;
        float stressFactor = 1.0f + stressLevel_ * 0.5f;  // Stress amplifies dopamine response
        
        float dopamineLevel = predictionErrorComponent * energyFactor * stressFactor;
        
        // Clamp to reasonable range
        dopamineLevel = std::clamp(dopamineLevel, -1.0f, 1.0f);
        
        // Apply neuromodulation with eligibility traces
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Enhanced plasticity modulation
                    float plasticityFactor = plasticityModifier_;
                    
                    // Energy-dependent plasticity
                    if (energyLevel_ < 0.5f) {
                        plasticityFactor *= 0.5f;
                    }
                    
                    // Temperature adaptation
                    plasticityFactor *= config_->temperatureAdaptation_;
                    
                    // Stress-dependent modulation
                    if (stressLevel_ > config_->stressThreshold_) {
                        plasticityFactor *= 0.7f;  // Stress reduces learning efficiency
                    }
                    
                    float delta = eligibility * dopamineLevel * plasticityFactor;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace with stress modulation
                    float decayRate = 0.1f;
                    if (stressLevel_ > config_->stressThreshold_) {
                        decayRate *= 1.5f;  // Stress accelerates trace decay
                    }
                    syn->decayEligibilityTrace(decayRate);
                }
            }
        }
        
        // Update stress level based on prediction error
        if (predictionError_ < -0.5f) {  // Large negative error = stress
            stressLevel_ = std::min(1.0f, stressLevel_ + 0.05f);
        } else if (predictionError_ > 0.5f) {  // Large positive error = relief
            stressLevel_ = std::max(0.0f, stressLevel_ - 0.05f);
        } else {
            // Gradual stress decay
            stressLevel_ *= 0.95f;
        }
        
        // Energy drain based on neuromodulation
        float energyDrain = 0.001f * std::abs(dopamineLevel);
        energyLevel_ = std::max(0.0f, energyLevel_ - energyDrain);
        
        // Recovery if energy is low and stress is low
        if (energyLevel_ < config_->maxEnergyLevel_ && stressLevel_ < 0.3f) {
            float recovery = 0.0005f;
            energyLevel_ = std::min(config_->maxEnergyLevel_, energyLevel_ + recovery);
        }
        
        dopamineLevel_ = dopamineLevel;
    }
    
    // Memory integration for episodic memory
    void updateMemoryIntegration(const std::vector<float>& vision, const MotorCommand& action, float reward) {
        if (!config_->episodicMemoryEnabled_) return;
        
        // Create episodic memory entry
        EpisodicMemoryEntry entry;
        entry.timestamp = developmentalAge_;
        entry.visionPattern = vision;
        entry.action = action;
        entry.reward = reward;
        entry.context = "enhanced_agent_brain";
        
        // Compute contextual importance
        float novelty = computeNoveltyEnhanced(vision);
        float predictionError = std::abs(predictionError_);
        
        entry.importance = (novelty * 0.6f + predictionError * 0.4f) * config_->memoryConsolidationRate_;
        
        // Store in episodic memory (simplified - would use actual memory system)
        // For now, store in exploration history for analysis
        explorationHistory_.push_back(action);
        
        // Learn from this experience
        learnFromExplorationHistory();
    }
    
    // Helper function for motor command to string conversion
    const char* motorCommandToStringEnhanced(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
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
