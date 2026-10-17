#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain,
                      std::unique_ptr<NeuralGroupManager> groupManager,
                      std::unique_ptr<SensorProcessor> sensorProcessor,
                      std::unique_ptr<MotorDecoder> motorDecoder)
    : brain_(brain)
    , groupManager_(std::move(groupManager))
    , sensorProcessor_(std::move(sensorProcessor))
    , motorDecoder_(std::move(motorDecoder))
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , neuromodulationSensitivity_(1.0f)
    , structuralPlasticityRate_(1.0f)
    , developmentSpeed_(1.0f)
    , curiosityThreshold_(0.3f)
    , plasticityThreshold_(0.5f)
    , learningRate_(0.01f)
    , rewardModulationGain_(1.0f)
    , sensoryNoveltyDecay_(0.99f)
    , adaptiveNoveltySensitivity_(1.0f)
    , lastUpdateTime_(0.0)
    , processingCycles_(0)
    , healthScore_(1.0f)
{
    // Initialize default components if none provided
    if (!groupManager_) {
        groupManager_ = std::make_unique<DefaultNeuralGroupManager>(brain_);
    }
    if (!sensorProcessor_) {
        sensorProcessor_ = std::make_unique<DefaultSensorProcessor>();
    }
    if (!motorDecoder_) {
        motorDecoder_ = std::make_unique<DefaultMotorDecoder>();
    }
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        groupManager_->organizeNeuralGroups(brain_);
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world, 
                         const std::vector<std::unique_ptr<AgentBehavior>>& behaviors) {
    // Initialize component systems
    if (groupManager_) {
        groupManager_->organizeNeuralGroups(brain_);
    }
    
    // Initialize sensor processor with world dimensions
    if (sensorProcessor_) {
        // Vision processing initialization
        int visionWidth = world.getVisionWidth();
        int visionHeight = world.getVisionHeight();
        previousVision_.resize(visionWidth * visionHeight, 0.0f);
    }
    
    // Initialize behaviors
    for (auto& behavior : behaviors) {
        if (behavior) {
            behavior->initialize(brain_);
            behaviors_.push_back(std::move(behavior));
        }
    }
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    lastUpdateTime_ = 0.0;
    processingCycles_ = 0;
    healthScore_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(behaviors_.size()) + " behaviors and " +
                 std::to_string(brain_ ? brain_->getTotalNeuronCount() : 0) + " neurons");
}

//[[nodiscard]] size_t AgentBrain::getSensoryInputSize() const noexcept {
    if (sensorProcessor_) {
        return sensorProcessor_->getProcessingStatistics().count("vision_input"); // Placeholder
    }
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

//[[nodiscard]] size_t AgentBrain::getMotorOutputSize() const noexcept {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept,
                           const std::unordered_map<std::string, float>& processingOptions) {
    if (!brain_ || !sensorProcessor_) return;
    processingCycles_++;
    
    // Use advanced sensor processor for sensory data
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Process vision with adaptive sensitivity
    std::vector<float> processedVision = sensorProcessor_->processVision(vision);
    
    // Apply custom processing options if provided
    for (const auto& option : processingOptions) {
        if (option.first == "vision_sensitivity") {
            float sensitivity = option.second;
            for (float& v : processedVision) {
                v = std::min(1.0f, v * sensitivity);
            }
        }
    }
    
    // Inject processed vision into neural groups
    if (!processedVision.empty() && !previousVision_.empty()) {
        noveltyLevel_ = sensorProcessor_->computeNovelty(processedVision, previousVision_);
        
        // Update novelty with adaptive sensitivity
        noveltyLevel_ *= (adaptiveNoveltySensitivity_ * sensoryNoveltyDecay_);
        
        // Store for next time
        previousVision_ = processedVision;
    }
    
    // Process touch, internal, and proprioception signals
    std::vector<float> processedTouch = sensorProcessor_->processTouch(touch);
    std::vector<float> processedInternal = sensorProcessor_->processInternal(intern);
    std::vector<float> processedProprioception = sensorProcessor_->processProprioception(proprio);
    
    // Inject signals into neural groups using group manager
    if (groupManager_) {
        // Update curiosity based on novelty and prediction error
        if (curiosityEnabled_) {
            curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
            
            // Apply curiosity threshold
            if (curiosityLevel_ < curiosityThreshold_) {
                curiosityLevel_ = 0.0f;
            }
        }
    }
    
    // Update behavior processors if any
    for (auto& behavior : behaviors_) {
        if (behavior) {
            behavior->processSensoryInput(percept);
        }
    }
    
    calculateHealthScore();
}

//[[nodiscard]] MotorCommand AgentBrain::decodeMotorCommand(const std::string& strategy) {
    if (!brain_ || !motorDecoder_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeuronsAdvanced(strategy);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosityAdvanced(decoded, strategy);
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward,
                          const std::unordered_map<std::string, float>& modulationParams) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Apply custom modulation parameters if provided
    float neuromodulationGain = rewardModulationGain_;
    float plasticityFactor = plasticityModifier_;
    
    for (const auto& param : modulationParams) {
        if (param.first == "neuromodulation_gain") {
            neuromodulationGain = param.second;
        } else if (param.first == "plasticity_factor") {
            plasticityFactor = param.second;
        } else if (param.first == "learning_rate") {
            learningRate_ = param.second;
        }
    }
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error and neuromodulation sensitivity)
    dopamineLevel_ = predictionError_ * neuromodulationSensitivity_ * neuromodulationGain;
    
    // Apply neuromodulation sensitivity and clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_ * neuromodulationSensitivity_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with advanced parameters
                float delta = eligibility * dopamineLevel_ * plasticityFactor * learningRate_;
                
                // Apply plasticity threshold
                if (std::abs(delta) > plasticityThreshold_) {
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float advancedPlasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    advancedPlasticityFactor = std::clamp(advancedPlasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * advancedPlasticityFactor);
        stdp->setLTDWeight(0.012f * advancedPlasticityFactor);
    }
    
    // Update health score based on neuromodulation success
    calculateHealthScore();
}

void AgentBrain::updateDevelopment(double timestep,
                          const std::unordered_map<std::string, float>& devParams) {
    if (!brain_ || !developmentEnabled_) return;
    
    // Apply development speed multiplier
    developmentalAge_ += timestep * developmentSpeed_;
    
    // Apply custom development parameters
    float plasticitySpeed = plasticityModifier_;
    float structuralPlasticitySpeed = structuralPlasticityEnabled_ ? 1.0f : 0.0f;
    
    for (const auto& param : devParams) {
        if (param.first == "plasticity_speed") {
            plasticitySpeed = param.second;
        } else if (param.first == "structural_plasticity_speed") {
            structuralPlasticitySpeed = param.second;
        } else if (param.first == "development_speed") {
            developmentSpeed_ = param.second;
        }
    }
    
    // Enhanced developmental stages based on age with speed modifiers
    if (developmentalAge_ < 60.0 * developmentSpeed_) {  // Initial phase
        plasticityModifier_ = 1.0f * plasticitySpeed;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0 * developmentSpeed_) {  // Critical period
        plasticityModifier_ = 0.8f * plasticitySpeed;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0 * developmentSpeed_) {  // Maturation
        plasticityModifier_ = 0.5f * plasticitySpeed;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {  // Adult phase
        plasticityModifier_ = 0.2f * plasticitySpeed;  // More stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development (enhanced)
    if (structuralPlasticityEnabled_ && brain_->getStructuralPlasticity()) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development with speed modifiers
            float synRate = 0.0001f * plasticityModifier_ * structuralPlasticitySpeed;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * structuralPlasticitySpeed;
            
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Update behavior systems
    updateBehaviors(timestep);
}

//[[nodiscard]] DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

//[[nodiscard]] float AgentBrain::getNeuromodulationLevel(const std::string& metric) const noexcept {
    if (metric == "novelty") return noveltyLevel_;
    if (metric == "curiosity") return curiosityLevel_;
    if (metric == "prediction_error") return predictionError_;
    if (metric == "expected_reward") return expectedReward_;
    if (metric == "dopamine") return dopamineLevel_;
    return dopamineLevel_;  // Default to dopamine
}

//[[nodiscard]] float AgentBrain::getCuriosityLevel() const noexcept { return curiosityLevel_; }

//[[nodiscard]] float AgentBrain::getNoveltyLevel() const noexcept { return noveltyLevel_; }

//[[nodiscard]] float AgentBrain::getPredictionError() const noexcept { return predictionError_; }

void AgentBrain::reset(const std::string& resetMode) {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset advanced parameters
    neuromodulationSensitivity_ = 1.0f;
    structuralPlasticityRate_ = 1.0f;
    developmentSpeed_ = 1.0f;
    curiosityThreshold_ = 0.3f;
    plasticityThreshold_ = 0.5f;
    learningRate_ = 0.01f;
    rewardModulationGain_ = 1.0f;
    
    // Clear previous sensory data
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset behaviors
    for (auto& behavior : behaviors_) {
        if (behavior) {
            behavior->reset();
        }
    }
    
    // Apply reset mode specific behavior
    if (resetMode == "full") {
        // Full reset - clear all state including brain state if possible
        if (brain_) {
            brain_->reset();
        }
        healthScore_ = 0.0f;
    } else {
        // Normal reset
        healthScore_ = 1.0f;
    }
    
    // Reset adaptive novelty sensitivity
    adaptiveNoveltySensitivity_ = 1.0f;
}

} // namespace nlm
